
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

	afx_msg LRESULT OnSerialPortReceiveCompleted(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSerialPortTransmitCompleted(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSerialPortTransmitFailed(WPARAM wParam, LPARAM lParam);
	
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
};
