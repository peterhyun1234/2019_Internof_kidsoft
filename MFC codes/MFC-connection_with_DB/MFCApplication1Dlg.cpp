
// MFCApplication1Dlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CMFCApplication1Dlg 대화 상자



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplication1Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCApplication1Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCApplication1Dlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CMFCApplication1Dlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CMFCApplication1Dlg 메시지 처리기

BOOL CMFCApplication1Dlg::OnInitDialog()
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

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCApplication1Dlg::OnPaint()
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
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//uart로 디바이스와 연결해서 데이터를 받을 준비! + 연결한 마지막 날짜(데이터가 써지기 시작한 마지막 날짜가 좋을듯)
// 데이터가 써지기 시작한 마지막 날짜 이후로 파일로 쌓아 놓고 연결된 이후로는 있었던 파일 지우고 연결된 동안은 파일에 저장하지 않는다.!
void CMFCApplication1Dlg::OnBnClickedButton4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


//data load in scv file
// -hardware에서 축적해놓은 파일들을 받아서 데이터베이스에 등록한다.
void CMFCApplication1Dlg::OnBnClickedButton1()
{

	CString fName = TEXT("D:\\2019_Summer_Kidsoft\\Intern_Hyunbin\\code\\sample pressure data\\sample_pressure_data.txt"); //오픈할 파일 경로

	CStdioFile file(fName, CFile::modeRead | CFile::typeText);
	CString cList;  //한줄씩 임시로 저장할 변수
	CString time;
	float sensor[7] = {0,};
	char query[255];
	mysql_init(&Connect);	//초기화

	if (!mysql_real_connect(&Connect, "localhost", DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0))
	{
		MessageBox(TEXT("DB 접속에 실패했습니다.\n응용 프로그램을 시작할 수 없습니다."), TEXT("접속 에러"), MB_OK);
		TRACE("Connection error %d: %s\n", mysql_errno(&Connect), mysql_error(&Connect));
	}
	else
	{
		mysql_query(&Connect, "set names euckr"); //한글 인식
		TRACE("DB 연결 성공");
	}

	while (file.ReadString(cList))
	{
		if (cList.GetLength() < 80)
			break;
		//TRACE("%S  and len: %d\n", cList, cList.GetLength());
		//parsing_data string
		time = cList.Mid(2,8);
		sensor[0] = _tstof(cList.Mid(14,6));
		sensor[1] = _tstof(cList.Mid(24,6));
		sensor[2] = _tstof(cList.Mid(34,6));
		sensor[3] = _tstof(cList.Mid(44,6));
		sensor[4] = _tstof(cList.Mid(54,6));
		sensor[5] = _tstof(cList.Mid(64,6));
		sensor[6] = _tstof(cList.Mid(74,6));
		//TRACE("%S, %f, %f, %f, %f, %f, %f, %f.\n", time, sensor[0], sensor[1], sensor[2], sensor[3], sensor[4], sensor[5], sensor[6]);

		sprintf_s(query, "insert into pressure (time, sensor1, sensor2, sensor3, sensor4, sensor5, sensor6, sensor7) values ('%S', '%f', '%f', '%f', '%f', '%f', '%f', '%f');",time, sensor[0], sensor[1], sensor[2], sensor[3], sensor[4], sensor[5], sensor[6]);
		
		//TRACE("%s\n", query);
		if (mysql_query(&Connect, query))
		{
			TRACE("Connection error %d: %s\n", mysql_errno(&Connect), mysql_error(&Connect));
			return;
		}

	}
	file.Close();
}




//store data in database
void CMFCApplication1Dlg::OnBnClickedButton3()
{
	mysql_init(&Connect);	//초기화

	if (!mysql_real_connect(&Connect, "localhost", DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0))
	{
		MessageBox(TEXT("DB 접속에 실패했습니다.\n응용 프로그램을 시작할 수 없습니다."), TEXT("접속 에러"), MB_OK);
		TRACE("Connection error %d: %s\n", mysql_errno(&Connect), mysql_error(&Connect));
	}
	else
	{
		mysql_query(&Connect, "set names euckr"); //한글 인식
		TRACE("DB 연결 성공");
	}

	//쿼리 전송부분
	if (mysql_query(&Connect, "select * from pressure"))
	{ // 쿼리 요청
		TRACE("Connection error %d: %s\n", mysql_errno(&Connect), mysql_error(&Connect));
		return;
	}

	if ((Sql_Result = mysql_store_result(&Connect)) == NULL)
	{
		//쿼리저장
		TRACE("Connection error %d: %s\n", mysql_errno(&Connect), mysql_error(&Connect));
		return;
	}

	while ((Sql_Row = mysql_fetch_row(Sql_Result)) != NULL)	//
	{
		TRACE("mysql에 받은 값입니다 : 0:%s 1:%s 2:%s 3:%s 4:%s 5:%s 6:%s 7:%s \n", Sql_Row[0], Sql_Row[1], Sql_Row[2], Sql_Row[3], Sql_Row[4], Sql_Row[5], Sql_Row[6]
			, Sql_Row[7]);
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

//print loaded data
void CMFCApplication1Dlg::OnBnClickedButton2()
{
	MessageBox(TEXT("dsdsd"));
}