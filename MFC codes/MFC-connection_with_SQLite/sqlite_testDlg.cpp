
// sqlite_testDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "sqlite_test.h"
#include "sqlite_testDlg.h"
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


// CsqlitetestDlg 대화 상자



CsqlitetestDlg::CsqlitetestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SQLITE_TEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CsqlitetestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CsqlitetestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &CsqlitetestDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CsqlitetestDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &CsqlitetestDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CsqlitetestDlg 메시지 처리기

BOOL CsqlitetestDlg::OnInitDialog()
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

void CsqlitetestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CsqlitetestDlg::OnPaint()
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
HCURSOR CsqlitetestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
	int i;
	for (i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

//Update - 아예 처음 프로그램을 이용할 때 자체적으로 table 만들기!
void CsqlitetestDlg::OnBnClickedButton2()
{
	sqlite3* db;
	char* zErrMsg = 0;
	int rc;
	char* sql;

	/* Open database */
	rc = sqlite3_open("pressure_data.db", &db);

	if (rc) {
		TRACE("Can't open database\n");
		return ;
	}
	else {
		TRACE("Opened database successfully\n");
	}

	/* Create realtime SQL statement */
	sql = "CREATE TABLE real_time("  \
		"months			INT," \
		"days           INT," \
		"hours          INT," \
		"minutes        INT," \
		"seconds        INT," \
		"mcseconds      INT," \
		"sensor1        INT," \
		"sensor2        INT," \
		"sensor3        INT," \
		"sensor4        INT," \
		"sensor5        INT," \
		"sensor6        INT," \
		"sensor7	    INT );";

	/* Execute realtime SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		TRACE("SQL error\n");
		sqlite3_free(zErrMsg);
	}
	else {
		TRACE("real-time Table created successfully\n");
	}

	/* Create result SQL statement */
	sql = "CREATE TABLE result("  \
		"months			INT," \
		"days           INT," \
		"hours          INT," \
		"minutes        INT," \
		"seconds        INT," \
		"mcseconds      INT," \
		"sensor1        INT," \
		"sensor2        INT," \
		"sensor3        INT," \
		"sensor4        INT," \
		"sensor5        INT," \
		"sensor6        INT," \
		"sensor7	    INT );";

	/* Execute result SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		TRACE("SQL error\n");
		sqlite3_free(zErrMsg);
	}
	else {
		TRACE("result Table created successfully\n");
	}

	sqlite3_close(db);
}

//만들어진 테이블에 축적된 데이터 불러와서 넣기
void CsqlitetestDlg::OnBnClickedButton3()
{

	CString fName = TEXT("D:\\2019_Summer_Kidsoft\\Intern_Hyunbin\\code\\sample pressure data\\sample_pressure_data.txt"); //오픈할 파일 경로

	CStdioFile file(fName, CFile::modeRead | CFile::typeText);
	CString cList;  //한줄씩 임시로 저장할 변수
	CString time;
	int months = 7;
	int days = 16;
	int hours, minutes, seconds, mcseconds;
	int sensor[7] = { 0, };
	char query[255];

	sqlite3* db;
	char* zErrMsg = 0;
	int rc;

	/* Open database */
	rc = sqlite3_open("pressure_data.db", &db);

	if (rc) {
		TRACE("Can't open database");
		return ;
	}
	else {
		TRACE("Opened database successfully\n");
	}


	while (file.ReadString(cList))
	{
		if (cList.GetLength() < 80)
			break;
		//TRACE("%S  and len: %d\n", cList, cList.GetLength());
		//parsing_data string
		hours = _ttoi(cList.Left(2));
		minutes = _ttoi(cList.Mid(3, 2));
		seconds = _ttoi(cList.Mid(6, 2));
		mcseconds = _ttoi(cList.Mid(9, 2));
		sensor[0] = _ttoi(cList.Mid(16, 6));
		sensor[1] = _ttoi(cList.Mid(26, 6));
		sensor[2] = _ttoi(cList.Mid(36, 6));
		sensor[3] = _ttoi(cList.Mid(46, 6));
		sensor[4] = _ttoi(cList.Mid(56, 6));
		sensor[5] = _ttoi(cList.Mid(66, 6));
		sensor[6] = _ttoi(cList.Mid(76, 6));
		//TRACE("%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d.\n", hours, minutes, seconds, mcseconds, sensor[0], sensor[1], sensor[2], sensor[3], sensor[4], sensor[5], sensor[6]);

		sprintf_s(query, "insert into result (months, days, hours, minutes, seconds, mcseconds, sensor1, sensor2, sensor3, sensor4, sensor5, sensor6, sensor7) values ('%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d');", months, days, hours, minutes, seconds, mcseconds, sensor[0], sensor[1], sensor[2], sensor[3], sensor[4], sensor[5], sensor[6]);

		TRACE("%s\n", query);

		/* Execute SQL statement */
		rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);		
		if (rc != SQLITE_OK) {
			TRACE("SQL error\n");
			sqlite3_free(zErrMsg);
		}
	}



	sqlite3_close(db);
	file.Close();
}

//select - DB에서 데이터 받아서 출력
void CsqlitetestDlg::OnBnClickedButton1()
{
	sqlite3* db;
	char* zErrMsg = 0;
	int rc;
	char* sql;
	const char* data = "Callback function called";

	/* Open database */
	rc = sqlite3_open("pressure_data.db", &db);

	if (rc) {
		TRACE("Can't open database\n");
		
	}
	else {
		TRACE("Opened database successfully\n");
	}

	/* Create SQL statement */
	sql = "SELECT * from result";

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
	
	// 데이터 받아오는 과정 구현 ㄱㄱ!!

	if (rc != SQLITE_OK) {
		TRACE("SQL error\n");
		sqlite3_free(zErrMsg);
	}
	else {
		TRACE("Operation done successfully\n");
	}
	sqlite3_close(db);
}
