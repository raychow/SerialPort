#pragma once
#include <array>
#include <memory>

#include <boost/signals2.hpp>
#include <boost/thread.hpp>

#define SERAILPORT_BUFFERSIZE 128

class CSerialPortControl
{
public:
	typedef boost::signals2::signal<void (const std::string &)> signal_string_t;
	typedef boost::signals2::signal<void (void)> signal_void_t;

	static const int m_nBUFFERSIZE = 128;

	CSerialPortControl(void);
	virtual ~CSerialPortControl(void);

	BOOL Open(LPCTSTR lpszCOMPort, DWORD dwBaudRate, int nByteSize, int nParity, int nStopBits);
	void Close();

	boost::signals2::connection ConnectReceiveCompleted(const signal_string_t::slot_type &subscriber);
	boost::signals2::connection ConnectReceiveFailed(const signal_string_t::slot_type &subscriber);
	boost::signals2::connection ConnectTransmitCompleted(const signal_void_t::slot_type &subscriber);
	boost::signals2::connection ConnectTransmitFailed(const signal_void_t::slot_type &subscriber);

	void Transmit(const std::string &sData);
protected:
	BOOL m_bOpened;
	DWORD m_dwOldCommMask;

	signal_string_t m_signalReceiveCompleted;
	signal_string_t m_signalReceiveFailed;
	signal_void_t m_signalTransmitCompleted;
	signal_void_t m_signalTransmitFailed;

	CEvent m_eventTerminate;

	std::shared_ptr<void> m_shandleSerialPort;
	boost::thread m_threadInspector;
	boost::thread m_threadTransmit;

	void _SerialPortInspectorThread();
};

