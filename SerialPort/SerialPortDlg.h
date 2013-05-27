
// SerialPortDlg.h : ͷ�ļ�
//

#pragma once

#include "SerialPortControl.h"

// CSerialPortDlg �Ի���
class CSerialPortDlg : public CDialogEx
{
// ����
public:
	CSerialPortDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CSerialPortDlg();

// �Ի�������
	enum { IDD = IDD_SERIALPORT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void OnSerialPortReceiveCompleted(const std::string &sData);
	void OnSerialPortTransmitCompleted();
	void OnSerialPortTransmitFailed();

	boost::signals2::connection m_connectionReceiveCompleted;
	boost::signals2::connection m_connectionReceiveFailed;
	boost::signals2::connection m_connectionTransmitCompleted;
	boost::signals2::connection m_connectionTransmitFailed;

	CString m_csCOMPort;
	CString m_csBaudRate;
	int m_nByteSizeIndex;
	int m_nParityIndex;
	int m_nStopBitsIndex;
	CString m_csReceive;
	CString m_csTransmit;

	CSerialPortControl m_serialPortControl;

	void FillData();
public:
	virtual void OnOK();

	afx_msg void OnBnClickedButtonSerialPortOpen();
	afx_msg void OnBnClickedButtonSerialPortClearReceive();
	afx_msg void OnBnClickedButtonSerialPortTransmit();
	afx_msg void OnBnClickedButtonSerialportClose();
	afx_msg void OnClose();
};
