#include "stdafx.h"
#include "SerialPortControl.h"

#include <string>

CSerialPortControl::CSerialPortControl(void)
	: m_bOpened(FALSE)
	, m_dwOldCommMask(0)
	, m_eventTerminate(FALSE, TRUE)
{
}

CSerialPortControl::~CSerialPortControl(void)
{
	Close();
}

BOOL CSerialPortControl::Open(LPCTSTR lpszCOMPort, DWORD dwBaudRate, int nByteSize, int nParity, int nStopBits)
{
	if (m_bOpened || m_threadInspector.joinable())
	{
		return FALSE;
	}
	m_shandleSerialPort = std::shared_ptr<void>(CreateFile(lpszCOMPort,
			GENERIC_READ | GENERIC_WRITE,
			0,
			nullptr,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			0),
		::CloseHandle
		);
	
	if (m_shandleSerialPort.get() == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if (!::GetCommMask(m_shandleSerialPort.get(), &m_dwOldCommMask))
	{
		ASSERT(FALSE);
	}
	DWORD dwCommMask = EV_RXCHAR | EV_TXEMPTY;
	if (!::SetCommMask(m_shandleSerialPort.get(), dwCommMask))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	DCB dcb = {0};
	if (!::GetCommState(m_shandleSerialPort.get(), &dcb))
	{
		ASSERT(FALSE);
		return FALSE;
	}
	dcb.BaudRate = dwBaudRate;
	dcb.ByteSize = nByteSize;
	dcb.Parity = nParity;
	dcb.StopBits = nStopBits;
	if (!::SetCommState(m_shandleSerialPort.get(), &dcb))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	COMMTIMEOUTS commTimeouts;

	commTimeouts.ReadIntervalTimeout = MAXDWORD; 
	commTimeouts.ReadTotalTimeoutMultiplier = 0;
	commTimeouts.ReadTotalTimeoutConstant = 0;
	commTimeouts.WriteTotalTimeoutMultiplier = 0;
	commTimeouts.WriteTotalTimeoutConstant = 0;

	if (!::SetCommTimeouts(m_shandleSerialPort.get(), &commTimeouts))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_threadInspector = boost::thread(std::bind(&CSerialPortControl::_SerialPortInspectorThread, this));
	m_bOpened = TRUE;
	return TRUE;
}

void CSerialPortControl::Close()
{
	if (!m_bOpened)
	{
		return;
	}
	m_eventTerminate.SetEvent();
	if (m_threadInspector.joinable())
	{
		m_threadInspector.join();
	}
	m_eventTerminate.ResetEvent();
	m_shandleSerialPort.reset();
	m_bOpened = FALSE;
}

void CSerialPortControl::_SerialPortInspectorThread()
{
	DWORD dwCommEvent = 0;
	OVERLAPPED overLapped = {0};
	std::shared_ptr<void> seventOverLapped(CreateEvent(nullptr,
			FALSE,
			FALSE,
			nullptr),
		::CloseHandle);	// unique_ptr 似乎不支持将 deleter 写在参数列表中，而只能写在模板中（且必须是个类）。
	if (!seventOverLapped)
	{
		ASSERT(FALSE);
		return;
	}
	overLapped.hEvent = seventOverLapped.get();
	std::array<HANDLE, 2> ahandleWait = { m_eventTerminate.m_hObject, seventOverLapped.get() };
	BOOL bContinue = TRUE;
	DWORD dwWaitResult = 0;
	while (bContinue)
	{
		if (!WaitCommEvent(m_shandleSerialPort.get(), &dwCommEvent, &overLapped) && GetLastError() != ERROR_IO_PENDING)
		{
			TRACE(_T("%d"), GetLastError());
			ASSERT(FALSE);
		}

		
		dwWaitResult = ::WaitForMultipleObjects(ahandleWait.size(), ahandleWait.data(), FALSE, INFINITE);
		switch (dwWaitResult)
		{
		case WAIT_OBJECT_0:
			bContinue = FALSE;
			break;
		case WAIT_OBJECT_0 + 1:
			if (dwCommEvent & EV_TXEMPTY)
			{
				TRACE(_T("Transmit completed.\n"));
				m_signalTransmitCompleted();
			}
			else if (dwCommEvent & EV_RXCHAR)
			{
				std::string sData;
				DWORD dwBytesRead = 0;
				std::array<char, SERAILPORT_BUFFERSIZE> abyBuffer;
				do
				{
					if (!::ReadFile(m_shandleSerialPort.get(), abyBuffer.data(), abyBuffer.size(), &dwBytesRead, &overLapped))
					{
						TRACE(_T("Receive failed.\n"));
						m_signalReceiveFailed(sData);
						break;
					}
					if (dwBytesRead)
					{
						sData.append(abyBuffer.data(), abyBuffer.data() + dwBytesRead);
					}
				} while (dwBytesRead);
				TRACE(_T("Receive completed.\n"));
				m_signalReceiveCompleted(sData);
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
}

boost::signals2::connection CSerialPortControl::ConnectReceiveCompleted(const signal_string_t::slot_type &subscriber)
{
	return m_signalReceiveCompleted.connect(subscriber);
}

boost::signals2::connection CSerialPortControl::ConnectReceiveFailed(const signal_string_t::slot_type &subscriber)
{
	return m_signalReceiveFailed.connect(subscriber);
}

boost::signals2::connection CSerialPortControl::ConnectTransmitCompleted(const signal_void_t::slot_type &subscriber)
{
	return m_signalTransmitCompleted.connect(subscriber);
}

boost::signals2::connection CSerialPortControl::ConnectTransmitFailed(const signal_void_t::slot_type &subscriber)
{
	return m_signalTransmitFailed.connect(subscriber);
}

void CSerialPortControl::Transmit(const std::string &sData)
{
	OVERLAPPED overLapped = {0};
	std::shared_ptr<void> seventOverLapped(CreateEvent(nullptr,
			FALSE,
			FALSE,
			nullptr),
		::CloseHandle);
	if (!seventOverLapped)
	{
		ASSERT(FALSE);
		return;
	}
	overLapped.hEvent = seventOverLapped.get();

	DWORD dwBytesWritten = 0;
	if(!::WriteFile(m_shandleSerialPort.get(), sData.c_str(), sData.size(), &dwBytesWritten, &overLapped))
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			TRACE(_T("Transmit failed.\n"));
			m_signalTransmitFailed();
			return;
		}
	}
	TRACE(_T("Transmit in progress.\n"));
}

