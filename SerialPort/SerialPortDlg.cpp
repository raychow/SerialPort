
// SerialPortDlg.cpp : 实现文件
//

#include "stdafx.h"

#include <string>

#include "SerialPort.h"
#include "SerialPortDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSerialPortDlg 对话框



CSerialPortDlg::CSerialPortDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSerialPortDlg::IDD, pParent)
	, m_csCOMPort(_T(""))
	, m_csBaudRate(_T(""))
	, m_nByteSizeIndex(0)
	, m_nParityIndex(0)
	, m_nStopBitsIndex(0)
	, m_csReceive(_T(""))
	, m_csTransmit(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_connectionReceiveCompleted = m_serialPortControl.ConnectReceiveCompleted(std::bind(&CSerialPortDlg::OnSerialPortReceiveCompleted, this, std::placeholders::_1));
	m_connectionTransmitCompleted = m_serialPortControl.ConnectTransmitCompleted(std::bind(&CSerialPortDlg::OnSerialPortTransmitCompleted, this));
	m_connectionTransmitFailed = m_serialPortControl.ConnectTransmitFailed(std::bind(&CSerialPortDlg::OnSerialPortTransmitFailed, this));
}

CSerialPortDlg::~CSerialPortDlg()
{
}

void CSerialPortDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_COMBO_SERIALPORT_COMPORT, m_csCOMPort);
	DDX_CBString(pDX, IDC_COMBO_SERIALPORT_BAUDRATE, m_csBaudRate);
	DDX_CBIndex(pDX, IDC_COMBO_SERIALPORT_BYTESIZE, m_nByteSizeIndex);
	DDX_CBIndex(pDX, IDC_COMBO_SERIALPORT_PARITY, m_nParityIndex);
	DDX_CBIndex(pDX, IDC_COMBO_SERIALPORT_STOPBITS, m_nStopBitsIndex);
	DDX_Text(pDX, IDC_EDIT_SERIALPOT_RECEIVE, m_csReceive);
	DDX_Text(pDX, IDC_EDIT_SERIALPOT_TRANSMIT, m_csTransmit);
}

BEGIN_MESSAGE_MAP(CSerialPortDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SERIALPORT_OPEN, &CSerialPortDlg::OnBnClickedButtonSerialPortOpen)
	ON_BN_CLICKED(IDC_BUTTON_SERIALPORT_CLEARRECEIVE, &CSerialPortDlg::OnBnClickedButtonSerialPortClearReceive)
	ON_BN_CLICKED(IDC_BUTTON_SERIALPORT_TRANSMIT, &CSerialPortDlg::OnBnClickedButtonSerialPortTransmit)
	ON_BN_CLICKED(IDC_BUTTON_SERIALPORT_CLOSE, &CSerialPortDlg::OnBnClickedButtonSerialportClose)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CSerialPortDlg 消息处理程序

BOOL CSerialPortDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	FillData();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSerialPortDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSerialPortDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSerialPortDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSerialPortDlg::OnBnClickedButtonSerialPortOpen()
{
	UpdateData(TRUE);
	m_csCOMPort.Trim();
	m_csBaudRate.Trim();
	BOOL bAllFilled = TRUE;
	CString csEmptyField;

	if (m_csCOMPort.IsEmpty())
	{
		bAllFilled = FALSE;
		csEmptyField = _T("COM Port");
	}
	
	int nBaudRate = _ttoi(m_csBaudRate);
	if (bAllFilled && nBaudRate <= 0)
	{
		bAllFilled = FALSE;
		csEmptyField = _T("Baud Rate");
	}

	int nByteSize = m_nByteSizeIndex + 5;
	if (bAllFilled && !(4 < nByteSize && nByteSize < 9))
	{
		bAllFilled = FALSE;
		csEmptyField = _T("Byte Size");
	}

	int nParity = 0;
	if (bAllFilled)
	{
		switch (m_nParityIndex)
		{
		case 0:
			nParity = NOPARITY;
			break;
		case 1:
			nParity = ODDPARITY;
			break;
		case 2:
			nParity = EVENPARITY;
			break;
		case 3:
			nParity = MARKPARITY;
			break;
		default:
			bAllFilled = FALSE;
			csEmptyField = _T("Parity");
			break;
		}
	}

	int nStopBits = 0;
	if (bAllFilled)
	{
		switch (m_nStopBitsIndex)
		{
		case 0:
			nStopBits = ONESTOPBIT;
			break;
		case 1:
			nStopBits = ONE5STOPBITS;
			break;
		case 2:
			nStopBits = TWOSTOPBITS;
			break;
		default:
			bAllFilled = FALSE;
			csEmptyField = _T("Stop Bits");
			break;
		}
	}

	if (!bAllFilled)
	{
		AfxMessageBox(_T("Incorrect ") + csEmptyField + _T("."));
		return;
	}

	if (!m_serialPortControl.Open(m_csCOMPort, nBaudRate, nByteSize, nParity, nStopBits))
	{
		AfxMessageBox(_T("Serial port open failed."), MB_ICONERROR);
		return;
	}

	GetDlgItem(IDC_BUTTON_SERIALPORT_OPEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SERIALPORT_CLOSE)->EnableWindow(TRUE);
}

void CSerialPortDlg::FillData()
{
	CString csTemp;

	CComboBox &comboCOMPort = *static_cast<CComboBox *>(GetDlgItem(IDC_COMBO_SERIALPORT_COMPORT));
	for (int i = 1; i != 10; ++i)
	{
		csTemp.Format(_T("COM%d"), i);
		comboCOMPort.AddString(csTemp);
	}
	comboCOMPort.SetCurSel(0);

	CComboBox &comboBaudRate = *static_cast<CComboBox *>(GetDlgItem(IDC_COMBO_SERIALPORT_BAUDRATE));
	comboBaudRate.AddString(_T("600"));
	comboBaudRate.AddString(_T("1200"));
	comboBaudRate.AddString(_T("2400"));
	comboBaudRate.AddString(_T("4800"));
	comboBaudRate.AddString(_T("9600"));
	comboBaudRate.AddString(_T("14400"));
	comboBaudRate.AddString(_T("28800"));
	comboBaudRate.AddString(_T("38400"));
	comboBaudRate.AddString(_T("56000"));
	comboBaudRate.AddString(_T("57600"));
	comboBaudRate.AddString(_T("115200"));
	comboBaudRate.AddString(_T("128000"));
	comboBaudRate.AddString(_T("256000"));
	comboBaudRate.SetCurSel(0);

	CComboBox &comboByteSize = *static_cast<CComboBox *>(GetDlgItem(IDC_COMBO_SERIALPORT_BYTESIZE));
	for (int i = 5; i != 8 + 1; ++i)
	{
		csTemp.Format(_T("%d"), i);
		comboByteSize.AddString(csTemp);
	}
	comboByteSize.SetCurSel(3);

	CComboBox &comboParity = *static_cast<CComboBox *>(GetDlgItem(IDC_COMBO_SERIALPORT_PARITY));
	comboParity.AddString(_T("No parity"));
	comboParity.AddString(_T("Odd"));
	comboParity.AddString(_T("Even"));
	comboParity.AddString(_T("Mark"));
	comboParity.SetCurSel(0);

	CComboBox &comboStopBits = *static_cast<CComboBox *>(GetDlgItem(IDC_COMBO_SERIALPORT_STOPBITS));
	comboStopBits.AddString(_T("1"));
	comboStopBits.AddString(_T("1.5"));
	comboStopBits.AddString(_T("2"));
	comboStopBits.SetCurSel(0);
}

void CSerialPortDlg::OnOK()
{
	// Nothing
	// CDialogEx::OnOK();
}

void CSerialPortDlg::OnSerialPortReceiveCompleted(const std::string &sData)
{
	USES_CONVERSION;
	LPCTSTR lpszData = A2T(sData.c_str());
	m_csReceive.Append(lpszData);
	GetDlgItem(IDC_EDIT_SERIALPOT_RECEIVE)->SetWindowText(m_csReceive);
	return;
}

void CSerialPortDlg::OnSerialPortTransmitCompleted()
{
	m_csTransmit.Empty();
	GetDlgItem(IDC_EDIT_SERIALPOT_TRANSMIT)->SetWindowText(m_csTransmit);
	return;
}

void CSerialPortDlg::OnSerialPortTransmitFailed()
{
	AfxMessageBox(IDS_TRANSMITFAILED, MB_ICONERROR);
	return;
}

void CSerialPortDlg::OnBnClickedButtonSerialPortClearReceive()
{
	m_csReceive.Empty();
	UpdateData(FALSE);
}

void CSerialPortDlg::OnBnClickedButtonSerialPortTransmit()
{
	UpdateData(TRUE);
	USES_CONVERSION;
	const char* pchData = T2A(m_csTransmit);
	std::string sData(pchData);
	m_serialPortControl.Transmit(sData);
}

void CSerialPortDlg::OnBnClickedButtonSerialportClose()
{
	m_serialPortControl.Close();
	GetDlgItem(IDC_BUTTON_SERIALPORT_OPEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SERIALPORT_CLOSE)->EnableWindow(FALSE);
}


void CSerialPortDlg::OnClose()
{
	m_connectionReceiveCompleted.disconnect();
	m_connectionReceiveFailed.disconnect();
	m_connectionTransmitCompleted.disconnect();
	m_connectionTransmitFailed.disconnect();
	CDialogEx::OnClose();
}
