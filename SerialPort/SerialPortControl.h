#pragma once
#include <array>
#include <memory>

#include <boost/thread.hpp>

#define WM_SERIALPORT_BASE				WM_USER
#define WM_SERIALPORT_RECEIVECOMPLETED	WM_SERIALPORT_BASE
#define WM_SERIALPORT_RECEIVEFAILED		WM_SERIALPORT_BASE + 1
#define WM_SERIALPORT_TRANSMITCOMPLETED	WM_SERIALPORT_BASE + 2
#define WM_SERIALPORT_TRANSMITFAILED	WM_SERIALPORT_BASE + 3

#define SERAILPORT_BUFFERSIZE 128

class CSerialPortControl
{
public:
	CSerialPortControl(void);
	virtual ~CSerialPortControl(void);

	void SetWindowHandle(HWND m_hWindow);

	BOOL Open(LPCTSTR lpszCOMPort, DWORD dwBaudRate, int nByteSize, int nParity, int nStopBits);
	void Close();

	void Transmit(const std::string &sData);
protected:
	BOOL m_bOpened;
	DWORD m_dwOldCommMask;
	HWND m_hWindow;

	CEvent m_eventTerminate;

	std::shared_ptr<void> m_shandleSerialPort;
	boost::thread m_threadInspector;
	boost::thread m_threadTransmit;

	void _SerialPortInspectorThread();
};

