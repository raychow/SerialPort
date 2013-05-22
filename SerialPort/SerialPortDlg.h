
// SerialPortDlg.h : 头文件
//

#pragma once

#include "SerialPortControl.h"

// CSerialPortDlg 对话框
class CSerialPortDlg : public CDialogEx
{
// 构造
public:
	CSerialPortDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SERIALPORT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
