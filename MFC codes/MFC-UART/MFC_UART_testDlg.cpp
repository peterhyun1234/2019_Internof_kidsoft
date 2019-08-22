
// MFC_UART_testDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFC_UART_test.h"
#include "MFC_UART_testDlg.h"
#include "afxdialogex.h"
#include "SerialPort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static BYTE* pByte = new BYTE[83];

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCUARTtestDlg 대화 상자



CMFCUARTtestDlg::CMFCUARTtestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_UART_TEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCUARTtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	DDX_Control(pDX, IDC_EDIT2, m_Edit_send);
}

BEGIN_MESSAGE_MAP(CMFCUARTtestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCUARTtestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCUARTtestDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCUARTtestDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CMFCUARTtestDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CMFCUARTtestDlg 메시지 처리기

BOOL CMFCUARTtestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCUARTtestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
	if (nID == SC_CLOSE)
	{
		m_pThread->SuspendThread();

		DWORD dwResult;
		::GetExitCodeThread(m_pThread->m_hThread, &dwResult);

		delete m_pThread;
		m_pThread = NULL;

		m_eThreadWork = THREAD_STOP;	

		delete[] pByte;

	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCUARTtestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCUARTtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// connect
void CMFCUARTtestDlg::OnBnClickedButton1()
{

}


UINT CMFCUARTtestDlg::ThreadFunction(LPVOID _mothod)
{

	CMFCUARTtestDlg* pDlg = (CMFCUARTtestDlg*)_mothod;

	CSerialPort _serial;
	CString cList;  //한줄씩 임시로 저장할 변수
	int data_len = 83;

	//BYTE* pByte = new BYTE[data_len];

	if (!_serial.OpenPort(L"COM5"))// 실제 사용될 COM Port
	{
		TRACE("Serial Open port error\n");
	}

	// BaudRate, ByteSize, fParity, Parity, StopBit 정보를 설정
	if(!_serial.ConfigurePort(CBR_115200, 8, FALSE, NOPARITY, ONESTOPBIT))
	{
		TRACE("Serial ConfigurePort error\n");
	}

	if(!_serial.SetCommunicationTimeouts(0, 0, 0, 0, 0))
	{
		TRACE("Serial SetCommunicationTimeouts error\n");
	}

	while (pDlg->m_eThreadWork == THREAD_RUNNING)
	{

		if (_serial.ReadByte(pByte, data_len)) {
			cList = CString(reinterpret_cast<char*>(pByte));
			TRACE(cList);
		}
		else {
			//TRACE("ReadByte error\n");
		}
	}


	//while (_serial.ReadByte(pByte, data_len))
	//{
	//	//_serial.ReadByte(pByte, data_len);
	//	pByte[data_len - 1] = '\0';
	//	cList = CString(reinterpret_cast<char*>(pByte));

	//	TRACE(cList);

	//	//sensor[0] = _ttoi(cList.Mid(16, 6));
	//	//sensor[1] = _ttoi(cList.Mid(26, 6));
	//	//sensor[2] = _ttoi(cList.Mid(36, 6));
	//	//sensor[3] = _ttoi(cList.Mid(46, 6));
	//	//sensor[4] = _ttoi(cList.Mid(56, 6));
	//	//sensor[5] = _ttoi(cList.Mid(66, 6));
	//	//sensor[6] = _ttoi(cList.Mid(76, 6));

	//	//TRACE("%d, %d, %d, %d, %d, %d, %d.\n", sensor[0], sensor[1], sensor[2], sensor[3], sensor[4], sensor[5], sensor[6]);
	//	//TRACE("thread running...\n");
	//	if (pDlg->m_eThreadWork != THREAD_RUNNING)
	//		break;
	//}

	_serial.ClosePort();
	

	return 0;
}



//receive: thread start
void CMFCUARTtestDlg::OnBnClickedButton3()
{
	if (m_pThread == NULL)
	{
		m_pThread = AfxBeginThread(ThreadFunction, this);

		if (m_pThread == NULL)
		{
			AfxMessageBox(L"Error");
		}

		m_pThread->m_bAutoDelete = FALSE;
		m_eThreadWork = THREAD_RUNNING;
	}
	else
	{
		if (m_eThreadWork == THREAD_PAUSE)
		{
			m_pThread->ResumeThread();
			m_eThreadWork = THREAD_RUNNING;
		}
	}
}


// thread pause
void CMFCUARTtestDlg::OnBnClickedButton4()
{
	if (m_pThread == NULL)
	{
		AfxMessageBox(L"Thread not start");
	}
	else
	{
		m_pThread->SuspendThread();
		m_eThreadWork = THREAD_PAUSE;
	}
}


//send
void CMFCUARTtestDlg::OnBnClickedButton2()
{
	CString send_str;
	m_Edit_send.GetWindowTextW(send_str);
}

void CMFCUARTtestDlg::Wait(DWORD dwMillisecond)
{
	MSG msg;
	DWORD dwStart;
	dwStart = GetTickCount();

	while (GetTickCount() - dwStart < dwMillisecond)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}