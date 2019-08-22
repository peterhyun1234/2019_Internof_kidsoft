

// MFC_ParsingData_and_SQLiteDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFC_ParsingData_and_SQLite.h"
#include "MFC_ParsingData_and_SQLiteDlg.h"
#include "afxdialogex.h"
#include "SerialPort.h"

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


// CMFCParsingDataandSQLiteDlg 대화 상자



CMFCParsingDataandSQLiteDlg::CMFCParsingDataandSQLiteDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_PARSINGDATA_AND_SQLITE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCParsingDataandSQLiteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCParsingDataandSQLiteDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCParsingDataandSQLiteDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCParsingDataandSQLiteDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CMFCParsingDataandSQLiteDlg 메시지 처리기

BOOL CMFCParsingDataandSQLiteDlg::OnInitDialog()
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

void CMFCParsingDataandSQLiteDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCParsingDataandSQLiteDlg::OnPaint()
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
HCURSOR CMFCParsingDataandSQLiteDlg::OnQueryDragIcon()
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

void CMFCParsingDataandSQLiteDlg::OnBnClickedButton1()
{
	// UART thread start
	if (m_U_Thread == NULL)
	{
		m_U_Thread = AfxBeginThread(UART_Thread, this);

		if (m_U_Thread == NULL)
		{
			TRACE("Error: m_W_Thread == NULL\n");
		}

		m_U_Thread->m_bAutoDelete = FALSE;
		m_U_ThreadWork = THREAD_RUNNING;
	}
	else
	{
		if (m_U_ThreadWork == THREAD_PAUSE)
		{
			m_U_Thread->ResumeThread();
			m_U_ThreadWork = THREAD_RUNNING;
		}
	}
}

//uart thread
UINT CMFCParsingDataandSQLiteDlg::UART_Thread(LPVOID pParam)
{

	CMFCParsingDataandSQLiteDlg* pDlg = (CMFCParsingDataandSQLiteDlg*)pParam;

	//
	//DB CREATE and open
	//
	sqlite3* db;
	char* zErrMsg = 0;
	int rc;
	char* sql;

	/* Open database */
	rc = sqlite3_open("pressure_data.db", &db);

	if (rc) {
		TRACE("Can't open database\n");
	}
	else {
		TRACE("Opened database successfully\n");
	}

	/* Create realtime SQL statement */
	sql = "CREATE TABLE pressures("  \
		"years			INT," \
		"months			INT," \
		"days           INT," \
		"hours          INT," \
		"minutes        INT," \
		"seconds        INT," \
		"mseconds       INT," \
		"sensor1        INT," \
		"sensor2        INT," \
		"sensor3        INT," \
		"sensor4        INT," \
		"sensor5        INT," \
		"sensor6        INT," \
		"sensor7        INT," \
		"sensor8	    INT );";

	/* Execute realtime SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		TRACE("SQL error\n");
		sqlite3_free(zErrMsg);
	}
	else {
		TRACE("pressure_data Table created successfully\n");
	}

	/* Create result SQL statement */
	sql = "CREATE TABLE results("  \
		"years			INT," \
		"months			INT," \
		"days           INT," \
		"hours          INT," \
		"minutes        INT," \
		"seconds        INT," \
		"mseconds       INT," \
		"max1        INT," \
		"max2        INT," \
		"max3        INT," \
		"max4        INT," \
		"max5        INT," \
		"max6        INT," \
		"max7        INT," \
		"max8        INT," \
		"cnt1        INT," \
		"cnt2        INT," \
		"cnt3        INT," \
		"cnt4        INT," \
		"cnt5        INT," \
		"cnt6        INT," \
		"cnt7        INT," \
		"cnt8	    INT );";

	/* Execute result SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		TRACE("SQL error\n");
		sqlite3_free(zErrMsg);
	}
	else {
		TRACE("result Table created successfully\n");
	}

	char query[400];


	CSerialPort _serial;

	//CString cList;  //한줄씩 임시로 저장할 변수

	bool T_on = true;
	bool M_on = false;
	bool S1_on = false;
	bool S2_on = false;
	bool S3_on = false;
	bool S4_on = false;
	bool S5_on = false;
	bool S6_on = false;
	bool S7_on = false;
	bool S8_on = false;

	bool time_check = true;

	char temp[200];	// USB 뽑히면 UART 정지시키기 위한 버퍼

	char ReadBuf[200];	// 한줄 씩 받는 버퍼

	char result_temp[25];	// 파싱한 데이터 char형

	int readlen = 0;


	// parsing about curren time 
	int hour = 0;

	int minute = 0;

	int second = 0;

	int msecond = 0;

	float sec = 0;

	CString time;

	CStdioFile file;

	CString cList; //한줄로 저장할 데이터

	// pressure data variables
	int sensor[8] = { 0, };

	int msec_time = 0;

	int msec_temp = 0;

	int machine_num = 0;

	int data_count = 0;

	int temp_count = 0;

	// Maximun value and Count
	int Max_pressure[8] = { 0, };

	int cnt_pressure[8] = { 0, };

	if (!_serial.OpenPort(L"COM9"))// 실제 사용될 COM Port
	{
		TRACE("Serial Open port error\n");
		TRACE("error code : %d\n", GetLastError());
		//m_ano_list.InsertString(-1, _T("Serial port open error"));
		return 0;
	}
	else
	{
		//m_ano_list.InsertString(-1, _T("Serial port open"));
	}

	//BaudRate, ByteSize, fParity, Parity, StopBit 정보를 설정
	if (!_serial.ConfigurePort(CBR_115200, 8, FALSE, NOPARITY, ONESTOPBIT))
	{
		TRACE("Serial ConfigurePort error\n");
		//m_ano_list.InsertString(-1, _T("Configure serial port error"));
		return 0;
	}
	else
	{
		//m_ano_list.InsertString(-1, _T("Configure serial port complete"));
	}

	if (!_serial.SetCommunicationTimeouts(0, 0, 0, 0, 0))
	{
		TRACE("Serial SetCommunicationTimeouts error\n");
		//m_ano_list.InsertString(-1, _T("Serial SetCommunicationTimeouts error"));
		return 0;
	}
	else
	{
		//m_ano_list.InsertString(-1, _T("The sensor is connected with UART"));
		//m_ano_list.InsertString(-1, _T("---------------------------------"));	
	}


	SYSTEMTIME cur_time;


	//file and DB open
	GetLocalTime(&cur_time);

	int years = cur_time.wYear;
	int months = cur_time.wMonth;
	int days = cur_time.wDay;
	CString fName;
	//fName.Format(_T("D:\\2019_Summer_Kidsoft\\Intern_Hyunbin\\code\\DATA\\[UART]%d.%d.%d.txt"), years, months, days); //오픈할 파일 경로
	fName.Format(_T("D:\\2019_Summer_Kidsoft\\Intern_Hyunbin\\code\\DATA\\[UART]%d.%d.%d.txt"), years, months, days); //오픈할 파일 경로

	file.Open(fName, CFile::modeCreate | CFile::modeWrite | CFile::typeText);


	while (pDlg->m_U_ThreadWork == THREAD_RUNNING)
	{

		if (_serial.Readline(ReadBuf)) {

			SYSTEMTIME cur_time;
			
			
			//file and DB open
			GetLocalTime(&cur_time);

			int years = cur_time.wYear;
			int months = cur_time.wMonth;
			int days = cur_time.wDay;


			// pressure data
			if (!strncmp("T:", ReadBuf, 2))
			{

				if (time_check) {
					//현재시간 파싱
					
					GetLocalTime(&cur_time); //현재 시간 가져오기
					hour = cur_time.wHour;
					minute = cur_time.wMinute;
					second = cur_time.wSecond;
					msecond = cur_time.wMilliseconds;

					sec = second + (float)msecond/1000;

					//TRACE("[%02d:%02d:%02d.%03d]\n", cur_time.wHour, cur_time.wMinute, cur_time.wSecond, cur_time.wMilliseconds);
					time_check = false;
				}

				readlen = strlen(ReadBuf);
				//TRACE("len: %d\n",readlen);

				for (data_count = 2; data_count < readlen; data_count++)
				{
					if (T_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							//data_count = data_count + 2;
							T_on = false;
							M_on = true;
							result_temp[temp_count - 1] = NULL;

							msec_time = atoi(result_temp);

							if (msec_temp) {
								sec = sec + (float)(msec_time - msec_temp) / 1000;
								msecond = msecond + (msec_time - msec_temp);
							}

							msec_temp = msec_time;

							time.Format(_T("%02d:%02d:%0.3f"),hour, minute, sec);

							//TRACE("%S\n", time);
							temp_count = 0;
						}
					}
					else if (M_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							M_on = false;
							S1_on = true;
							result_temp[temp_count - 1] = NULL;
							machine_num = atoi(result_temp);
							//TRACE("result_temp2: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S1_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S1_on = false;
							S2_on = true;
							result_temp[temp_count - 1] = NULL;
							sensor[0] = atoi(result_temp);
							//TRACE("result_temp3: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S2_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S2_on = false;
							S3_on = true;
							result_temp[temp_count - 1] = NULL;
							sensor[1] = atoi(result_temp);
							//TRACE("result_temp4: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S3_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S3_on = false;
							S4_on = true;
							result_temp[temp_count - 1] = NULL;
							sensor[2] = atoi(result_temp);
							//TRACE("result_temp5: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S4_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S4_on = false;
							S5_on = true;
							result_temp[temp_count - 1] = NULL;
							sensor[3] = atoi(result_temp);
							//TRACE("result_temp6: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S5_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S5_on = false;
							S6_on = true;
							result_temp[temp_count - 1] = NULL;
							sensor[4] = atoi(result_temp);
							//TRACE("result_temp7: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S6_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S6_on = false;
							S7_on = true;
							result_temp[temp_count - 1] = NULL;
							sensor[5] = atoi(result_temp);
							//TRACE("result_temp8: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S7_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S7_on = false;
							S8_on = true;
							result_temp[temp_count - 1] = NULL;
							sensor[6] = atoi(result_temp);
							//TRACE("result_temp9: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S8_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (data_count + 1 == readlen)
						{
							//data_count = data_count + 2;
							S8_on = false;
							T_on = true;
							result_temp[temp_count - 1] = NULL;
							sensor[7] = atoi(result_temp);
							//TRACE("result_temp10: %s\n",result_temp);
							temp_count = 0;
						}
					}

				}
				//
				//data save! 
				//
				//(in database)
				sprintf_s(query, "insert into pressures (years, months, days, hours, minutes, seconds, mseconds, sensor1, sensor2, sensor3, sensor4, sensor5, sensor6, sensor7, sensor8) values ('%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d');", years, months, days, hour, minute, second, msecond, sensor[0], sensor[1], sensor[2], sensor[3], sensor[4], sensor[5], sensor[6], sensor[7]);
				
				//TRACE(query);

				/* Execute SQL statement */
				rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
				if (rc != SQLITE_OK) {
					TRACE("SQL error\n");
					sqlite3_free(zErrMsg);
				}

				//(in file)
				cList.Format(_T("%s, M%d, A1:%d, A2:%d, A3:%d, A4:%d, A5:%d, A6:%d, A7:%d, A8:%d\n"), time, machine_num, sensor[0], sensor[1], sensor[2], sensor[3], sensor[4], sensor[5], sensor[6], sensor[7]);
				//TRACE("%S\n", cList);
				file.WriteString(cList);

				// initialization
				readlen = 0;
				msec_time = 0;
				machine_num = 0;
				data_count = 0;
				temp_count = 0;
			}

			// result(max pressure인가 min인가 머시기)
			if (!strncmp("A1", ReadBuf, 2))
			{

				readlen = strlen(ReadBuf);

				S1_on = true;
				for (data_count = 3; data_count < readlen; data_count++)
				{
					if (S1_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S1_on = false;
							S2_on = true;
							result_temp[temp_count - 1] = NULL;
							Max_pressure[0] = atoi(result_temp);
							//TRACE("result_temp3: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S2_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S2_on = false;
							S3_on = true;
							result_temp[temp_count - 1] = NULL;
							Max_pressure[1] = atoi(result_temp);
							//TRACE("result_temp4: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S3_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S3_on = false;
							S4_on = true;
							result_temp[temp_count - 1] = NULL;
							Max_pressure[2] = atoi(result_temp);
							//TRACE("result_temp5: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S4_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S4_on = false;
							S5_on = true;
							result_temp[temp_count - 1] = NULL;
							Max_pressure[3] = atoi(result_temp);
							//TRACE("result_temp6: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S5_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S5_on = false;
							S6_on = true;
							result_temp[temp_count - 1] = NULL;
							Max_pressure[4] = atoi(result_temp);
							//TRACE("result_temp7: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S6_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S6_on = false;
							S7_on = true;
							result_temp[temp_count - 1] = NULL;
							Max_pressure[5] = atoi(result_temp);
							//TRACE("result_temp8: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S7_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S7_on = false;
							S8_on = true;
							result_temp[temp_count - 1] = NULL;
							Max_pressure[6] = atoi(result_temp);
							//TRACE("result_temp9: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S8_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (data_count + 1 == readlen)
						{
							//data_count = data_count + 2;
							S8_on = false;
							result_temp[temp_count - 1] = NULL;
							Max_pressure[7] = atoi(result_temp);
							//TRACE("result_temp10: %s\n",result_temp);
							temp_count = 0;
						}
					}

				}
				// initialization
				readlen = 0;
				data_count = 0;
				temp_count = 0;
			}

			//count
			if (!strncmp("Co", ReadBuf, 2))
			{
				readlen = strlen(ReadBuf);

				S1_on = true;
				for (data_count = 9; data_count < readlen; data_count++)
				{
					if (S1_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S1_on = false;
							S2_on = true;
							result_temp[temp_count - 1] = NULL;
							cnt_pressure[0] = atoi(result_temp);
							//TRACE("result_temp3: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S2_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S2_on = false;
							S3_on = true;
							result_temp[temp_count - 1] = NULL;
							cnt_pressure[1] = atoi(result_temp);
							//TRACE("result_temp4: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S3_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S3_on = false;
							S4_on = true;
							result_temp[temp_count - 1] = NULL;
							cnt_pressure[2] = atoi(result_temp);
							//TRACE("result_temp5: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S4_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S4_on = false;
							S5_on = true;
							result_temp[temp_count - 1] = NULL;
							cnt_pressure[3] = atoi(result_temp);
							//TRACE("result_temp6: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S5_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S5_on = false;
							S6_on = true;
							result_temp[temp_count - 1] = NULL;
							cnt_pressure[4] = atoi(result_temp);
							//TRACE("result_temp7: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S6_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S6_on = false;
							S7_on = true;
							result_temp[temp_count - 1] = NULL;
							cnt_pressure[5] = atoi(result_temp);
							//TRACE("result_temp8: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S7_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (ReadBuf[data_count] == ',')
						{

							data_count = data_count + 3;
							S7_on = false;
							S8_on = true;
							result_temp[temp_count - 1] = NULL;
							cnt_pressure[6] = atoi(result_temp);
							//TRACE("result_temp9: %s\n",result_temp);
							temp_count = 0;
						}
					}
					else if (S8_on)
					{
						result_temp[temp_count] = ReadBuf[data_count];
						temp_count++;
						if (data_count + 1 == readlen)
						{
							//data_count = data_count + 2;
							S8_on = false;
							result_temp[temp_count - 1] = NULL;
							cnt_pressure[7] = atoi(result_temp);
							//TRACE("result_temp10: %s\n",result_temp);
							temp_count = 0;
						}
					}

				}

				sprintf_s(query, "insert into results (years, months, days, hours, minutes, seconds, mseconds, max1, max2, max3, max4, max5, max6, max7, max8, cnt1, cnt2, cnt3, cnt4, cnt5, cnt6, cnt7, cnt8) values ('%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d');", years, months, days, hour, minute, second, msecond, Max_pressure[0], Max_pressure[1], Max_pressure[2], Max_pressure[3], Max_pressure[4], Max_pressure[5], Max_pressure[6], Max_pressure[7], cnt_pressure[0], cnt_pressure[1], cnt_pressure[2], cnt_pressure[3], cnt_pressure[4], cnt_pressure[5], cnt_pressure[6], cnt_pressure[7]);

				//TRACE(query);

				/* Execute SQL statement */
				rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
				if (rc != SQLITE_OK) {
					TRACE("SQL error\n");
					sqlite3_free(zErrMsg);
				}

				// initialization
				readlen = 0;
				data_count = 0;
				temp_count = 0;
			}
		}
		else
		{
			msec_temp = 0;
			time_check = true;
			//file.Close();
		}

	}
	sqlite3_close(db);
	file.Close();

	//m_ano_list.InsertString(-1, _T("The sensor is disconnected through the UART"));
	//m_ano_list.InsertString(-1, _T("---------------------------------"));

	//temp.Empty();
	memset(temp, '0', 200);
	//cList.Empty();
	memset(ReadBuf, '0', 200);
	//UART = false;


	_serial.ClosePort();
	TRACE("port close\n");

	return 0;
}

void CMFCParsingDataandSQLiteDlg::OnBnClickedButton2()
{


}
