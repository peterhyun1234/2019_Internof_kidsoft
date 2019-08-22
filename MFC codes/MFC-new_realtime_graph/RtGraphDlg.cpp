#include "PCH.cpp"
// RtGraphDlg.cpp : implementation file

#include "stdafx.h"
#include "RtGraph.h"
#include "RtGraphDlg.h"

//#include <stdio.h>
#include <queue>U_Pressure_Cnt
#include <iostream>
#include <math.h>
#include <vector>
#include <sstream>
#include "SerialPort.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static bool Wifi = false;
static bool UART = false;
static bool UART_SET_SIGNAL = false;
static bool UDIS_SIGNAL = false;
static bool WDIS_SIGNAL = false;
static bool Chart_Start = false;

//DATA
static queue<int*> Data_Queue;
static int years = 0;
static int months = 0;
static int days = 0;
static int hour = 0;
static int minute = 0;
static int second = 0;
static int msecond = 0;
static int milli_Gap = 0;


static bool histogram = false;
static bool	statistics = true;
static bool	statistics_one = true;
static bool result_table = true;
static bool result_table_one = true;
static CString IP;
static CColorListBox m_ano_list;

//UART
static int U_PORT;
static int U_RATE;
static CString UART_PORT[100];
static int UART_RATE[20];
static BYTE* pByte = new BYTE[83];
static char set_cmd[18] = { '0','4','1','0','5','1','0','6','2','0','7','1','0','9','1','0','8','1' };

static int Machine = 1;

// Maximun value and Count
static int U_Pressure_Max[8] = { 0, };
static int U_Pressure_Cnt[8] = { 0, };

//statistics
static int sample_Num = 0;
static int sample_Max = 0;
static int sample_Min = 0;
static int sample_Median = 0;
static int sample_Mean = 0;
static int sample_SD = 0;

//DB variable
//static sqlite3* db;
//static char* zErrMsg = 0;
//static int rc;
//static char* sql;
//static char query[400];


//static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
//	int i;
//	for (i = 0; i < argc; i++) {
//		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//	}
//	printf("\n");
//	return 0;
//}


void CRtGraphDlg::Wait(DWORD dwMillisecond)
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



// CRtGraphDlg dialog
CRtGraphDlg::CRtGraphDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRtGraphDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRtGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDR_CTRL, m_IP);
	DDX_Control(pDX, IDC_ANO_LIST, m_ano_list);
	DDX_Control(pDX, IDC_COMBO2, m_UPORT_combo);
	DDX_Control(pDX, IDC_COMBO3, m_URATE_combo);
	DDX_Control(pDX, IDC_COMBO5, m_SRATE_combo);
	DDX_Control(pDX, IDC_COMBO4, m_SALGO_combo);
	DDX_Control(pDX, IDC_COMBO1, m_Thesh_combo);
	DDX_Control(pDX, IDC_COMBO6, m_Offset_combo);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_DTP_START);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, M_DTP_END);
	DDX_Control(pDX, IDC_EDIT3, m_SSID_edit);
	DDX_Control(pDX, IDC_EDIT4, m_PASSWORD_edit);
}

BEGIN_MESSAGE_MAP(CRtGraphDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &CRtGraphDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON5, &CRtGraphDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CRtGraphDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON2, &CRtGraphDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CRtGraphDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON3, &CRtGraphDlg::OnBnClickedButton3)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_BUTTON7, &CRtGraphDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CRtGraphDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CRtGraphDlg::OnBnClickedButton9)
END_MESSAGE_MAP()


// CRtGraphDlg message handlers

BOOL CRtGraphDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	//
	//DB CREATE and open
	//


	///* Open database */
	//rc = sqlite3_open("pressure_data.db", &db);

	//if (rc) {
	//	TRACE("Can't open database\n");
	//}
	//else {
	//	TRACE("Opened database successfully\n");
	//}

	///* Create realtime SQL statement */
	//sql = "CREATE TABLE pressures("  \
	//	"years			INT," \
	//	"months			INT," \
	//	"days           INT," \
	//	"hours          INT," \
	//	"minutes        INT," \
	//	"seconds        INT," \
	//	"mseconds       INT," \
	//	"sensor1        INT," \
	//	"sensor2        INT," \
	//	"sensor3        INT," \
	//	"sensor4        INT," \
	//	"sensor5        INT," \
	//	"sensor6        INT," \
	//	"sensor7        INT," \
	//	"sensor8	    INT );";

	///* Execute realtime SQL statement */
	//rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	//if (rc != SQLITE_OK) {
	//	TRACE("SQL error\n");
	//	sqlite3_free(zErrMsg);
	//}
	//else {
	//	TRACE("pressure_data Table created successfully\n");
	//}

	///* Create result SQL statement */
	//sql = "CREATE TABLE results("  \
	//	"years			INT," \
	//	"months			INT," \
	//	"days           INT," \
	//	"hours          INT," \
	//	"minutes        INT," \
	//	"seconds        INT," \
	//	"mseconds       INT," \
	//	"max1        INT," \
	//	"max2        INT," \
	//	"max3        INT," \
	//	"max4        INT," \
	//	"max5        INT," \
	//	"max6        INT," \
	//	"max7        INT," \
	//	"max8        INT," \
	//	"cnt1        INT," \
	//	"cnt2        INT," \
	//	"cnt3        INT," \
	//	"cnt4        INT," \
	//	"cnt5        INT," \
	//	"cnt6        INT," \
	//	"cnt7        INT," \
	//	"cnt8	    INT );";

	///* Execute result SQL statement */
	//rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	//if (rc != SQLITE_OK) {
	//	TRACE("SQL error\n");
	//	sqlite3_free(zErrMsg);
	//}
	//else {
	//	TRACE("result Table created successfully\n");
	//}



	//
	// 각종 박스들 default 처리 (저장된 파일로 이전 default값 유지)
	//
	m_IP.SetAddress(192, 168, 0, 13);

	getSerialPort();
	m_UPORT_combo.SetCurSel(1);

	UART_RATE[0] = 110;
	UART_RATE[1] = 300;
	UART_RATE[2] = 600;
	UART_RATE[3] = 1200;
	UART_RATE[4] = 2400;
	UART_RATE[5] = 4800;
	UART_RATE[6] = 9600;
	UART_RATE[7] = 14400;
	UART_RATE[8] = 19600;
	UART_RATE[9] = 38400;
	UART_RATE[10] = 57600;
	UART_RATE[11] = 115200;
	m_URATE_combo.AddString(_T("110"));
	m_URATE_combo.AddString(_T("300"));
	m_URATE_combo.AddString(_T("600"));
	m_URATE_combo.AddString(_T("1200"));
	m_URATE_combo.AddString(_T("2400"));
	m_URATE_combo.AddString(_T("4800"));
	m_URATE_combo.AddString(_T("9600"));
	m_URATE_combo.AddString(_T("14400"));
	m_URATE_combo.AddString(_T("19600"));
	m_URATE_combo.AddString(_T("38400"));
	m_URATE_combo.AddString(_T("57600"));
	m_URATE_combo.AddString(_T("115200"));
	m_URATE_combo.SetCurSel(11);

	m_SRATE_combo.AddString(_T("200Hz"));
	m_SRATE_combo.AddString(_T("150Hz"));
	m_SRATE_combo.AddString(_T("100Hz"));
	m_SRATE_combo.AddString(_T("50Hz"));
	m_SRATE_combo.AddString(_T("10Hz"));
	m_SRATE_combo.SetCurSel(0);

	m_SALGO_combo.AddString(_T("Minimum value"));
	m_SALGO_combo.AddString(_T("Minimum average value"));
	m_SALGO_combo.SetCurSel(0);

	// threashold setting
	m_Thesh_combo.AddString(_T("90 %"));
	m_Thesh_combo.AddString(_T("85 %"));
	m_Thesh_combo.AddString(_T("80 %"));
	m_Thesh_combo.AddString(_T("75 %"));
	m_Thesh_combo.AddString(_T("70 %"));
	m_Thesh_combo.AddString(_T("65 %"));
	m_Thesh_combo.AddString(_T("60 %"));
	m_Thesh_combo.AddString(_T("55 %"));
	m_Thesh_combo.AddString(_T("50 %"));
	m_Thesh_combo.SetCurSel(1);

	// offset mode setting
	m_Offset_combo.AddString(_T("Without offset"));
	m_Offset_combo.AddString(_T("Offset Test"));
	m_Offset_combo.AddString(_T("With offset"));
	m_Offset_combo.AddString(_T("Offset reset"));
	m_Offset_combo.SetCurSel(0);


	//Scope setting
	//SetDlgItemInt(IDC_EDIT15, 0); //x start
	//SetDlgItemInt(IDC_EDIT12, 10); //x end
	SetDlgItemInt(IDC_EDIT14, 0); //y start
	SetDlgItemInt(IDC_EDIT13, 30000); //y end

	
	//"send wifi information" 
	m_SSID_edit.SetWindowText("gearS2_2G");
	m_PASSWORD_edit.SetWindowText("itsywatch04");


	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRtGraphDlg::OnPaint()
{
	// Print Histogram
	if (histogram) {
		CDC* pDC = GetDC();

		/*
		CPen pen;
		pen.CreatePen(PS_DOT, 3, RGB(0, 0, 0));    // 색 펜을 생성
		CPen* oldPen = pDC->SelectObject(&pen);
		CBrush brush;
		brush.CreateSolidBrush(RGB(0, 0, 0));     // 색 채움색을 생성
		CBrush* oldBrush = pDC->SelectObject(&brush);
		*/

		pDC->Rectangle(ORIGIN_X - 20, ORIGIN_Y + 20, ORIGIN_X + X_AXIS_LENGTH + 10, ORIGIN_Y - Y_AXIS_LENGTH - 10);
		//pDC->SelectObject(oldPen);     // 시스템 펜 객체를 돌려줌
		//pDC->SelectObject(oldBrush);    // 시스템 브러시 객체를 돌려줌


		pDC->MoveTo(ORIGIN_X, ORIGIN_Y); // 원점으로 이동
		pDC->LineTo(ORIGIN_X + X_AXIS_LENGTH, ORIGIN_Y); // X축 선 그리기
		pDC->MoveTo(ORIGIN_X, ORIGIN_Y); // 원점으로 이동
		pDC->LineTo(ORIGIN_X, ORIGIN_Y - Y_AXIS_LENGTH); // Y축 선 그리기


		/*
		 * data 읽어오기
		 */
		 // Scope : max - min
		int arr[N];

		load_gaussian(arr);

		int Min_pressure = arr[0];
		int Max_pressure = arr[0];
		for (int i = 0; i < N; i++) {
			if (Min_pressure > arr[i]) Min_pressure = arr[i];
			if (Max_pressure < arr[i]) Max_pressure = arr[i];
		}

		int Bar_cnt[bar_num] = { 0, }; // 각 scope마다 가지고 있는 데이터의 수
		double Scope = (double)(Max_pressure - Min_pressure) / (bar_num + 1);
		for (int num = 0; num < N; num++)
		{
			if ((Min_pressure <= arr[num]) && (Min_pressure + Scope >= arr[num]))
				Bar_cnt[0]++;
			else if ((Min_pressure + Scope < arr[num]) && (Min_pressure + Scope * 2 >= arr[num]))
				Bar_cnt[1]++;
			else if ((Min_pressure + Scope * 2 < arr[num]) && (Min_pressure + Scope * 3 >= arr[num]))
				Bar_cnt[2]++;
			else if ((Min_pressure + Scope * 3 < arr[num]) && (Min_pressure + Scope * 4 >= arr[num]))
				Bar_cnt[3]++;
			else if ((Min_pressure + Scope * 4 < arr[num]) && (Min_pressure + Scope * 5 >= arr[num]))
				Bar_cnt[4]++;
			else if ((Min_pressure + Scope * 5 < arr[num]) && (Min_pressure + Scope * 6 >= arr[num]))
				Bar_cnt[5]++;
			else if ((Min_pressure + Scope * 6 < arr[num]) && (Min_pressure + Scope * 7 >= arr[num]))
				Bar_cnt[6]++;
			else if ((Min_pressure + Scope * 7 < arr[num]) && (Min_pressure + Scope * 8 >= arr[num]))
				Bar_cnt[7]++;
			else if ((Min_pressure + Scope * 8 < arr[num]) && (Min_pressure + Scope * 9 >= arr[num]))
				Bar_cnt[8]++;
		}

		//Y축 레이블
		int max_height = Bar_cnt[bar_num / 2];
		int zero = 0;
		int y_scope = max_height / Y_label_num;
		CString Y_label;
		//점선 만들기
		CPen pen, * oldpen;
		pen.CreatePen(PS_DOT, 1, RGB(0, 0, 0));
		oldpen = pDC->SelectObject(&pen);

		for (int y = 0; y < Y_label_num + 1; y++) {
			Y_label.Format(_T("%d"), y_scope * y);
			pDC->TextOut(ORIGIN_X / 3, ORIGIN_Y - (57) * y - 15, Y_label);
			pDC->MoveTo(ORIGIN_X, ORIGIN_Y - (57) * y - 15);
			pDC->LineTo(ORIGIN_X + X_AXIS_LENGTH, ORIGIN_Y - (57) * y - 15);
		}
		pDC->SelectObject(oldpen);
		pen.DeleteObject();




		//bar의 높이 비율 맞추기
		int Height_ratio = 1;
		if (max_height < ORIGIN_Y)
			Height_ratio++;


		int value; // 각 사각형의 x축 레이블

		for (int i = 0; i < bar_num; i++) {
			value = Min_pressure + Scope * i;
			drawSolidRectangle(BAR_WIDTH * (i + 1) + 20, BAR_WIDTH - 1, Bar_cnt[i] * Height_ratio, value);
			value++;
		}


		histogram = false;
		ReleaseDC(pDC);
	}

	if (result_table)
	{

		int RESULT_X = 768;
		int RESULT_Y = 42;
		int RESULT_X_LENGTH = 176;
		int RESULT_Y_LENGTH = 275;

		CDC* rtDC = GetDC();

		if (result_table_one) {
			// 위
			rtDC->FillSolidRect(RESULT_X, RESULT_Y, RESULT_X_LENGTH, 35, RGB(205, 205, 205));


			// 목록
			rtDC->FillSolidRect(RESULT_X, RESULT_Y + 35, RESULT_X_LENGTH - 140, RESULT_Y_LENGTH - 35, RGB(185, 185, 185));


			// 값
			rtDC->FillSolidRect(RESULT_X + RESULT_X_LENGTH - 140, RESULT_Y + 35, RESULT_X_LENGTH - 36, RESULT_Y_LENGTH - 35, RGB(255, 255, 255));
			rtDC->SetBkColor(RGB(255, 255, 255));

			// TABLE 테두리
			rtDC->MoveTo(RESULT_X, RESULT_Y); // 원점으로 이동
			rtDC->LineTo(RESULT_X + RESULT_X_LENGTH, RESULT_Y);
			rtDC->LineTo(RESULT_X + RESULT_X_LENGTH, RESULT_Y + RESULT_Y_LENGTH);
			rtDC->LineTo(RESULT_X, RESULT_Y + RESULT_Y_LENGTH);
			rtDC->LineTo(RESULT_X, RESULT_Y);

			// Table의 X축
			rtDC->MoveTo(RESULT_X, RESULT_Y + 35);
			rtDC->LineTo(RESULT_X + RESULT_X_LENGTH, RESULT_Y + 35);
			rtDC->MoveTo(RESULT_X, RESULT_Y + 65);
			rtDC->LineTo(RESULT_X + RESULT_X_LENGTH, RESULT_Y + 65);
			rtDC->MoveTo(RESULT_X, RESULT_Y + 95);
			rtDC->LineTo(RESULT_X + RESULT_X_LENGTH, RESULT_Y + 95);
			rtDC->MoveTo(RESULT_X, RESULT_Y + 125);
			rtDC->LineTo(RESULT_X + RESULT_X_LENGTH, RESULT_Y + 125);
			rtDC->MoveTo(RESULT_X, RESULT_Y + 155);
			rtDC->LineTo(RESULT_X + RESULT_X_LENGTH, RESULT_Y + 155);
			rtDC->MoveTo(RESULT_X, RESULT_Y + 185);
			rtDC->LineTo(RESULT_X + RESULT_X_LENGTH, RESULT_Y + 185);
			rtDC->MoveTo(RESULT_X, RESULT_Y + 215);
			rtDC->LineTo(RESULT_X + RESULT_X_LENGTH, RESULT_Y + 215);
			rtDC->MoveTo(RESULT_X, RESULT_Y + 245);
			rtDC->LineTo(RESULT_X + RESULT_X_LENGTH, RESULT_Y + 245);

			// Table의 Y축
			rtDC->MoveTo(RESULT_X + 36, RESULT_Y);
			rtDC->LineTo(RESULT_X + 36, RESULT_Y + RESULT_Y_LENGTH);
			rtDC->MoveTo(RESULT_X + 36 + 70, RESULT_Y);
			rtDC->LineTo(RESULT_X + 36 + 70, RESULT_Y + RESULT_Y_LENGTH);

			result_table_one = false;
		}
		else	// 직접 값 갱신
		{

			CString TEXT;

			// Max_pressure 
			for (int a = 0; a < 8; a++)
			{
				TEXT.Format("%d", U_Pressure_Max[a]);
				rtDC->TextOut(RESULT_X + 55, RESULT_Y + 45 + 30 * a, TEXT);
			}

			// cnt_pressure
			for (int b = 0; b < 8; b++)
			{
				TEXT.Format("%d", U_Pressure_Cnt[b]);
				rtDC->TextOut(RESULT_X + 130, RESULT_Y + 45 + 30 * b, TEXT);
			}

		}


		result_table = false;
		ReleaseDC(rtDC);
	}

	if (statistics)
	{
		CDC* stDC = GetDC();

		int RESULT_X = 505;
		int RESULT_Y = 715;
		int RESULT_X_LENGTH = 436;
		int RESULT_Y_LENGTH = 156;

		if (statistics_one) {
			// 목록
			stDC->FillSolidRect(RESULT_X, RESULT_Y, RESULT_X_LENGTH / 2, RESULT_Y_LENGTH, RGB(225, 225, 225));


			// 값
			stDC->FillSolidRect(RESULT_X + RESULT_X_LENGTH / 2, RESULT_Y, RESULT_X_LENGTH / 2, RESULT_Y_LENGTH, RGB(255, 255, 255));
			stDC->SetBkColor(RGB(255, 255, 255));

			// TABLE 테두리
			stDC->MoveTo(RESULT_X, RESULT_Y); // 원점으로 이동
			stDC->LineTo(RESULT_X + RESULT_X_LENGTH, RESULT_Y);
			stDC->LineTo(RESULT_X + RESULT_X_LENGTH, RESULT_Y + RESULT_Y_LENGTH);
			stDC->LineTo(RESULT_X, RESULT_Y + RESULT_Y_LENGTH);
			stDC->LineTo(RESULT_X, RESULT_Y);

			// Table의 X축
			stDC->MoveTo(RESULT_X, RESULT_Y + 26);
			stDC->LineTo(RESULT_X + RESULT_X_LENGTH, RESULT_Y + 26);
			stDC->MoveTo(RESULT_X, RESULT_Y + 52);
			stDC->LineTo(RESULT_X + RESULT_X_LENGTH, RESULT_Y + 52);
			stDC->MoveTo(RESULT_X, RESULT_Y + 78);
			stDC->LineTo(RESULT_X + RESULT_X_LENGTH, RESULT_Y + 78);
			stDC->MoveTo(RESULT_X, RESULT_Y + 104);
			stDC->LineTo(RESULT_X + RESULT_X_LENGTH, RESULT_Y + 104);
			stDC->MoveTo(RESULT_X, RESULT_Y + 130);
			stDC->LineTo(RESULT_X + RESULT_X_LENGTH, RESULT_Y + 130);

			// Table의 Y축
			stDC->MoveTo(RESULT_X + RESULT_X_LENGTH / 2, RESULT_Y);
			stDC->LineTo(RESULT_X + RESULT_X_LENGTH / 2, RESULT_Y + RESULT_Y_LENGTH);
			statistics_one = false;
		}
		else	// 직접 값 갱신
		{
			CString TEXT;

			TEXT.Format(_T("%d"), sample_Num);
			stDC->TextOut(RESULT_X + 320, RESULT_Y + 4, TEXT); //Sample number
			TEXT.Format(_T("%d"), sample_Max);
			stDC->TextOut(RESULT_X + 320, RESULT_Y + 30, TEXT); //Sample max
			TEXT.Format(_T("%d"), sample_Min);
			stDC->TextOut(RESULT_X + 320, RESULT_Y + 56, TEXT); //Sample min
			TEXT.Format(_T("%d"), sample_Median);
			stDC->TextOut(RESULT_X + 320, RESULT_Y + 82, TEXT); //Sample median
			TEXT.Format(_T("%d"), sample_Mean);
			stDC->TextOut(RESULT_X + 320, RESULT_Y + 108, TEXT); //Sample mean
			TEXT.Format(_T("%d"), sample_SD);
			stDC->TextOut(RESULT_X + 320, RESULT_Y + 134, TEXT); //Sample standard deviation
		}



		statistics = false;
		ReleaseDC(stDC);
	}


	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRtGraphDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CRtGraphDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 1000) {

		double value[8] = { 0, };
		int* temp_data;


		if (!Data_Queue.empty())
		{
			//TRACE("size1: %d\n", Data_Queue.size());
			temp_data = Data_Queue.back();

			for (int a = 0; a < 8; a++)
			{
				value[a] = temp_data[a];
				//TRACE("value[%d]: %f\n", a, value[a]);
			}

			SetDlgItemInt(IDC_EDIT11, temp_data[0]);
			SetDlgItemInt(IDC_EDIT24, temp_data[1]);
			SetDlgItemInt(IDC_EDIT25, temp_data[2]);
			SetDlgItemInt(IDC_EDIT26, temp_data[3]);
			SetDlgItemInt(IDC_EDIT27, temp_data[4]);
			SetDlgItemInt(IDC_EDIT28, temp_data[5]);
			SetDlgItemInt(IDC_EDIT29, temp_data[6]);
			SetDlgItemInt(IDC_EDIT30, temp_data[7]);

			//(in database)
			//sprintf_s(query, "insert into pressures (years, months, days, hours, minutes, seconds, mseconds, sensor1, sensor2, sensor3, sensor4, sensor5, sensor6, sensor7, sensor8) values ('%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d');", years, months, days, hour, minute, second, msecond, temp_data[0], temp_data[1], temp_data[2], temp_data[3], temp_data[4], temp_data[5], temp_data[6], temp_data[7]);

			//TRACE("%s\n", query);

			/* Execute SQL statement */
			/*rc = sqlite3_exec(db, query, NULL, 0, &zErrMsg);
			if (rc != SQLITE_OK) {
				TRACE("%s\n",zErrMsg);
				sqlite3_free(zErrMsg);
			}
*/
			Data_Queue.pop();

			_rtGraph->AppendPoints(value);

		}


		//result table
		result_table = true;
		OnPaint();

	}

	CDialog::OnTimer(nIDEvent);
}

void CRtGraphDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	delete _rtGraph;
}


void CRtGraphDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE)
	{
		if (UART) {
			//thread stop
			m_U_Thread->SuspendThread();
			DWORD dwResult;
			::GetExitCodeThread(m_U_Thread->m_hThread, &dwResult);

			delete m_U_Thread;
			m_U_Thread = NULL;

			m_U_ThreadWork = THREAD_STOP;
			//delete[] pByte;
		}

		if (Wifi)
		{
			//thread stop
			m_W_Thread->SuspendThread();

			DWORD dwResult;
			::GetExitCodeThread(m_W_Thread->m_hThread, &dwResult);

			delete m_W_Thread;
			m_W_Thread = NULL;

			m_W_ThreadWork = THREAD_STOP;
		}

		delete _rtGraph;

	}

	CDialog::OnSysCommand(nID, lParam);
}










void CRtGraphDlg::getSerialPort()
{

	HKEY hKey;


	//오픈할 레지스터 키에 대한 기본키 이름  
	//오픈할 레지스터 서브키 이름  
	//레지스터키에 대한 핸들  
	RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), &hKey);

	TCHAR szData[20], szName[100];
	DWORD index = 0, dwSize = 100, dwSize2 = 20, dwType = REG_SZ;
	m_UPORT_combo.ResetContent();
	memset(szData, 0x00, sizeof(szData));
	memset(szName, 0x00, sizeof(szName));



	//hKey - 레지스터키 핸들  
	//index - 값을 가져올 인덱스.. 다수의 값이 있을 경우 필요  
	//szName - 항목값이 저장될 배열  
	//dwSize - 배열의 크기  
	while (ERROR_SUCCESS == RegEnumValue(hKey, index, szName, &dwSize, NULL, NULL, NULL, NULL))
	{
		index++;

		//szName-레지터스터 항목의 이름  
		//dwType-항목의 타입, 여기에서는 널로 끝나는 문자열  
		//szData-항목값이 저장될 배열  
		//dwSize2-배열의 크기  
		RegQueryValueEx(hKey, szName, NULL, &dwType, (LPBYTE)szData, &dwSize2);
		m_UPORT_combo.AddString(CString(szData));

		UART_PORT[index] = CString(szData);


		memset(szData, 0x00, sizeof(szData));
		memset(szName, 0x00, sizeof(szName));
		dwSize = 100;
		dwSize2 = 20;
	}

	RegCloseKey(hKey);
}


void CRtGraphDlg::drawSolidRectangle(int x, int width, int height, int Value)
{
	CClientDC pDC(this);
	pDC.FillSolidRect(x, ORIGIN_Y - height, width, height, BAR_COLOR);
	pDC.SetBkColor(RGB(255, 255, 255));

	CString X_label;
	X_label.Format(_T("%d"), Value);

	pDC.TextOut(x - BAR_WIDTH / 3, ORIGIN_Y + 1, X_label);
}

void CRtGraphDlg::load_gaussian(int arr[])
{
	srand(GetTickCount64());

	for (int i = 0; i < N; i++) {
		arr[i] = (int)((gaussianRandom() + 3.0) * RNG);
		//TRACE("arr: %d\n", arr[i]);
	}
}

double CRtGraphDlg::gaussianRandom(void)
{
	static double V1, V2, S;
	static int phase = 0;
	double X;

	if (phase == 0) {
		do {
			double U1 = (double)rand() / RAND_MAX;
			double U2 = (double)rand() / RAND_MAX;
			V1 = 2 * U1 - 1;
			V2 = 2 * U2 - 1;
			S = V1 * V1 + V2 * V2;
		} while (S >= 1 || S == 0);

		X = V1 * sqrt(-2 * log(S) / S);

	}
	else X = V2 * sqrt(-2 * log(S) / S);

	phase = 1 - phase;
	return X;
}





// "Save as file" Button
void CRtGraphDlg::OnBnClickedButton2()
{
	CString DTP_start;
	CString DTP_end;

	m_DTP_START.GetWindowText(DTP_start);
	M_DTP_END.GetWindowText(DTP_end);

	
	CString m_strPath;

	CStdioFile file;

	// CFile file;

	CFileException ex;

	CFileDialog dlg(FALSE, _T("*.txt"), NULL, OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT, _T("TXT Files(*.txt)|*.txt|"), NULL);

	if (dlg.DoModal() == IDOK)

	{

		m_strPath = dlg.GetPathName();

		if (m_strPath.Right(4) != ".txt")

		{

			m_strPath += ".txt";

		}

		file.Open(m_strPath, CFile::modeCreate | CFile::modeReadWrite, &ex);

		UpdateData(TRUE);


		m_ano_list.InsertString(-1, _T("The Processing Data saved"));
		m_ano_list.InsertString(-1, _T("---------------------------------"));
		file.Close();
	}



	// Data Processing



	// >> 저장되는 파일 형태!
	//"<~일부터 ~일까지의 데이터>"
	//" T:2131312~~~~~"
	// {데이터들}
	// +
	// <Statistical Data>
	// mean : ~~
	//median : ~~
}

// "Normality test start" button
void CRtGraphDlg::OnBnClickedButton4()
{
	m_ano_list.InsertString(-1, _T("Nomality test started"));
	m_ano_list.InsertString(-1, _T("The result is printed below the button"));
	m_ano_list.InsertString(-1, _T("---------------------------------"));
}

// "start chart" button
void CRtGraphDlg::OnBnClickedButton3()
{
	m_ano_list.InsertString(-1, _T("The real-time chart started"));
	m_ano_list.InsertString(-1, _T("---------------------------------"));
	Chart_Start = true;

	//int X_Min = GetDlgItemInt(IDC_EDIT15);
	//int X_Max = GetDlgItemInt(IDC_EDIT12);
	int Y_Min = GetDlgItemInt(IDC_EDIT14);
	int Y_Max = GetDlgItemInt(IDC_EDIT13);

	//
	// 실시간 그래프 동작
	//
	CRect rtGraph;
	GetDlgItem(IDC_STATIC_RT_GRAPH)->GetWindowRect(rtGraph);

	ScreenToClient(rtGraph);

	_rtGraph = new COScopeCtrl(8);		//sensor 8개의 그래프 예약
	_rtGraph->Create(WS_VISIBLE | WS_CHILD, rtGraph, this, IDC_STATIC_RT_GRAPH);
	_rtGraph->SetRanges(Y_Min, Y_Max);
	_rtGraph->autofitYscale = true;
	CString XUnit("Pressure");
	CString YUnit("Time");
	CString Unit1("Sensor1");
	CString Unit2("Sensor2");
	CString Unit3("Sensor3");
	CString Unit4("Sensor4");
	CString Unit5("Sensor5");
	CString Unit6("Sensor6");
	CString Unit7("Sensor7");
	CString Unit8("Sensor8");

	_rtGraph->SetYUnits(XUnit);
	_rtGraph->SetXUnits(YUnit);
	_rtGraph->SetLegendLabel(Unit1, 0);
	_rtGraph->SetLegendLabel(Unit2, 1);
	_rtGraph->SetLegendLabel(Unit3, 2);
	_rtGraph->SetLegendLabel(Unit4, 3);
	_rtGraph->SetLegendLabel(Unit5, 4);
	_rtGraph->SetLegendLabel(Unit6, 5);
	_rtGraph->SetLegendLabel(Unit7, 6);
	_rtGraph->SetLegendLabel(Unit8, 7);
	_rtGraph->SetPlotColor(RGB(255, 0, 0), 0);
	_rtGraph->SetPlotColor(RGB(0, 255, 0), 1);
	_rtGraph->SetPlotColor(RGB(0, 0, 255), 2);
	_rtGraph->SetPlotColor(RGB(255, 0, 255), 3);
	_rtGraph->SetPlotColor(RGB(255, 100, 0), 4);
	_rtGraph->SetPlotColor(RGB(255, 255, 0), 5);
	_rtGraph->SetPlotColor(RGB(255, 255, 255), 6);
	_rtGraph->SetPlotColor(RGB(100, 255, 255), 7);
	_rtGraph->InvalidateCtrl();

	SetTimer(1000, 10, NULL);	//그래프 주기 변경(그래프가 나타나는 속도만 control)
}



// "Print Processing data" button
void CRtGraphDlg::OnBnClickedButton1()
{
	CString DTP_start;
	CString DTP_end;

	m_DTP_START.GetWindowText(DTP_start);
	M_DTP_END.GetWindowText(DTP_end);

	//TRACE("from %s to %s", DTP_start, DTP_end);

	m_ano_list.InsertString(-1, _T("Data processing end"));
	m_ano_list.InsertString(-1, _T("Printed out histogram"));
	m_ano_list.InsertString(-1, _T("Printed out statistics"));
	m_ano_list.InsertString(-1, _T("---------------------------------"));
	//Histogram
	histogram = true;
	statistics = true;
	result_table = true;
	sample_Num = 7860;
	sample_Max = 32032;
	sample_Min = 7644;
	sample_Median = 23034;
	sample_Mean = 15634;
	sample_SD = 345;
	OnPaint();
}

// "Wifi connect" Button
void CRtGraphDlg::OnBnClickedButton5()
{
	if (Wifi)
	{
		MessageBox(_T("Wifi is already connected."), _T("Wifi connection"), MB_ICONINFORMATION);
	}
	else
	{
		//WIFI mode on
		Wifi = true;
		BYTE IP_a, IP_b, IP_c, IP_d;
		m_IP.GetAddress(IP_a, IP_b, IP_c, IP_d);
		IP.Format(_T("%d.%d.%d.%d"), IP_a, IP_b, IP_c, IP_d);


		// Wifi thread start
		if (m_W_Thread == NULL)
		{
			m_W_Thread = AfxBeginThread(Wifi_Thread, this);

			if (m_W_Thread == NULL)
			{
				TRACE("Error: m_W_Thread == NULL\n");
			}

			m_W_Thread->m_bAutoDelete = FALSE;
			m_W_ThreadWork = THREAD_RUNNING;
		}
		else
		{
			if (m_W_ThreadWork == THREAD_PAUSE)
			{
				m_W_Thread->ResumeThread();
				m_W_ThreadWork = THREAD_RUNNING;
			}
		}
	}
}

// "UART connect" Button
void CRtGraphDlg::OnBnClickedButton6()
{

	U_PORT = m_UPORT_combo.GetCurSel();
	U_RATE = m_URATE_combo.GetCurSel();

	//TRACE("\nport: %s, rate: %d\n\n", UART_PORT[U_PORT+1], UART_RATE[U_RATE]);
	if (UART)
	{
		MessageBox(_T("UART is already connected."), _T("UART connection"), MB_ICONINFORMATION);
	}
	else
	{
		UART = true;

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
}


//
// thread functions
//

//wifi thread
UINT CRtGraphDlg::Wifi_Thread(LPVOID pParam)
{
	CRtGraphDlg* pDlg = (CRtGraphDlg*)pParam;


	//소켓 관련 변수 선언
	int readcnt = 0;

	SOCKET listenSock;
	SOCKET connSock;


	// 소켓 기본 설정
	listenSock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in server_addr;

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(IP);
	server_addr.sin_port = htons(PORT);

	/*
	nSockOpt = 1;
	setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, &nSockOpt, sizeof(nSockOpt));
	*/

	if (bind(listenSock, (LPSOCKADDR)& server_addr, sizeof(server_addr)) < 0) {
		//MessageBox(_T("Please wait a few seconds and try again"), _T("Bind error"), MB_ICONHAND);
		m_ano_list.InsertString(-1, _T("Server Can't bind"));
		TRACE("Server Can't bind \n");
	}
	else
	{
		//m_ano_list.InsertString(-1, _T("Bind complete"));
	}


	if (listen(listenSock, 1) < 0)
	{
		//MessageBox(_T("Please wait a few seconds and try again"), _T("Listen error"), MB_ICONHAND);
		m_ano_list.InsertString(-1, _T("Server Can't listen"));
		TRACE("Server Can't listen \n");
	}
	else
	{
		//m_ano_list.InsertString(-1, _T("Listen complete"));
	}

	m_ano_list.InsertString(-1, _T("Waiting for sensor to connect .."));

	while (1) {

		SOCKADDR_IN client_addr;
		memset(&client_addr, 0, sizeof(client_addr));
		int len = sizeof(client_addr);

		if ((connSock = accept(listenSock, (sockaddr*)& client_addr, &len)) < 0) {
			TRACE("Talk Server failed in accepting\n");
			//MessageBox(_T("Please wait a few seconds and try again"), _T("Accept error"), MB_ICONHAND);
			m_ano_list.InsertString(-1, _T("Server Can't accept"));
			readcnt = 0;
			break;
		}
		else
		{
			if (readcnt == 0)
			{
				m_ano_list.InsertString(-1, _T("The sensor is connected with Wifi"));
				m_ano_list.InsertString(-1, _T("---------------------------------"));
				readcnt++;
			}
		}


		int Readlen = 0;
		char rcvBuf[1500];
		char* rcvLine;


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

		char result_temp[25];	// 파싱한 데이터 char형

		int Line_len = 0;


		// parsing about curren time 

		float sec = 0;

		CString time;

		CStdioFile file;

		CString cList; //한줄로 저장할 데이터

		// pressure data variables
		int sensor[8] = { 0, };

		int msec_time = 0;

		int msec_temp = 0;

		int data_count = 0;

		int temp_count = 0;



		SYSTEMTIME cur_time;


		//file and DB open
		GetLocalTime(&cur_time);

		years = cur_time.wYear;
		months = cur_time.wMonth;
		days = cur_time.wDay;
		CString fName;
		fName.Format(_T("D:\\2019_Summer_Kidsoft\\Intern_Hyunbin\\RtGraph\\data\\%d.%d.%d.(Machine%d).txt"), years, months, days, Machine); //오픈할 파일 경로

		file.Open(fName, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate | CFile::typeText);
		//TRACE("file open\n");
		file.SeekToEnd();

		while (pDlg->m_W_ThreadWork == THREAD_RUNNING)
		{
			if (Readlen = recv(connSock, rcvBuf, sizeof(rcvBuf), 0))
			{

				if (Readlen == -1)
				{
					TRACE("Read Error\n");
					break;
				}

				//Wfile.Write(rcvBuf,readlen+1);
				rcvBuf[Readlen] = NULL;	// 클라이언트로부터 받은 데이터의 마지막은 NULL로
				//TRACE("%s\n", rcvBuf);


				// 15개 라인 자르기
				rcvLine = strtok(rcvBuf, "\n");

				while (rcvLine != NULL)
				{
					if (UART == false)	//only Wifi
					{
						//TRACE("Recieved Line : %s\n", rcvLine);


						SYSTEMTIME cur_time;

						GetLocalTime(&cur_time);

						// pressure data
						if (!strncmp("T:", rcvLine, 2))
						{

							if (time_check) {
								//현재시간 파싱

								GetLocalTime(&cur_time); //현재 시간 가져오기
								hour = cur_time.wHour;
								minute = cur_time.wMinute;
								second = cur_time.wSecond;
								msecond = cur_time.wMilliseconds;

								sec = second + (float)msecond / 1000;

								//TRACE("[%02d:%02d:%02d.%03d]\n", cur_time.wHour, cur_time.wMinute, cur_time.wSecond, cur_time.wMilliseconds);
								time_check = false;
							}

							Line_len = strlen(rcvLine);
							//TRACE("len: %d\n",Line_len);

							for (data_count = 2; data_count < Line_len; data_count++)
							{
								if (T_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
									{

										//data_count = data_count + 2;
										T_on = false;
										M_on = true;
										result_temp[temp_count - 1] = NULL;

										msec_time = atoi(result_temp);
										//TRACE("%d\n", milli_Gap);
										if (msec_temp)
										{
											milli_Gap = msec_time - msec_temp;
											sec = sec + (float)(milli_Gap) / 1000;
											msecond = msecond + (msec_time - msec_temp);
											if (msecond >= 1000)
											{
												second++;
												msecond = msecond - 1000;
											}
										}

										msec_temp = msec_time;

										time.Format(_T("%02d:%02d:%0.3f"), hour, minute, sec);

										//TRACE("time: %S\n", time);
										temp_count = 0;
									}
								}
								else if (M_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
									{

										data_count = data_count + 3;
										M_on = false;
										S1_on = true;
										result_temp[temp_count - 1] = NULL;
										int temp_Machine = atoi(result_temp);
										if (Machine != temp_Machine)
										{
											Machine = atoi(result_temp);
										}

										//TRACE("result_temp2: %s\n",result_temp);
										temp_count = 0;
									}
								}
								else if (S1_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
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
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
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
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
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
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
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
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
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
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
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
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
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
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (data_count + 1 == Line_len)
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

							//(in file)
							//TRACE("%s, M%d, A1:%d, A2:%d, A3:%d, A4:%d, A5:%d, A6:%d, A7:%d, A8:%d\n", time, Machine, sensor[0], sensor[1], sensor[2], sensor[3], sensor[4], sensor[5], sensor[6], sensor[7]);

							//queue에 데이터 저장 - chart로 보여주기 위함!
							//TRACE("A1:%d, A2:%d, A3:%d, A4:%d, A5:%d, A6:%d, A7:%d, A8:%d\n", sensor[0], sensor[1], sensor[2], sensor[3], sensor[4], sensor[5], sensor[6], sensor[7]);
							if (Chart_Start)
							{
								Data_Queue.push(sensor);
								pDlg->Wait(10);
							}

							cList.Format(_T("%s, M%d, A1:%d, A2:%d, A3:%d, A4:%d, A5:%d, A6:%d, A7:%d, A8:%d\n"), time, Machine, sensor[0], sensor[1], sensor[2], sensor[3], sensor[4], sensor[5], sensor[6], sensor[7]);
							//TRACE("%s\n", cList);
							file.WriteString(cList);

							// initialization
							Line_len = 0;
							//for (int a = 0; a < 8; a++)
								//sensor[a] = 0;
							msec_time = 0;
							data_count = 0;
							temp_count = 0;
						}

						// result(max pressure인가 min인가 머시기)
						if (!strncmp("A1", rcvLine, 2))
						{

							Line_len = strlen(rcvLine);

							S1_on = true;
							for (data_count = 3; data_count < Line_len; data_count++)
							{
								if (S1_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
									{

										data_count = data_count + 3;
										S1_on = false;
										S2_on = true;
										result_temp[temp_count - 1] = NULL;
										U_Pressure_Max[0] = atoi(result_temp);
										//TRACE("result_temp3: %s\n",result_temp);
										temp_count = 0;
									}
								}
								else if (S2_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
									{

										data_count = data_count + 3;
										S2_on = false;
										S3_on = true;
										result_temp[temp_count - 1] = NULL;
										U_Pressure_Max[1] = atoi(result_temp);
										//TRACE("result_temp4: %s\n",result_temp);
										temp_count = 0;
									}
								}
								else if (S3_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
									{

										data_count = data_count + 3;
										S3_on = false;
										S4_on = true;
										result_temp[temp_count - 1] = NULL;
										U_Pressure_Max[2] = atoi(result_temp);
										//TRACE("result_temp5: %s\n",result_temp);
										temp_count = 0;
									}
								}
								else if (S4_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
									{

										data_count = data_count + 3;
										S4_on = false;
										S5_on = true;
										result_temp[temp_count - 1] = NULL;
										U_Pressure_Max[3] = atoi(result_temp);
										//TRACE("result_temp6: %s\n",result_temp);
										temp_count = 0;
									}
								}
								else if (S5_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
									{

										data_count = data_count + 3;
										S5_on = false;
										S6_on = true;
										result_temp[temp_count - 1] = NULL;
										U_Pressure_Max[4] = atoi(result_temp);
										//TRACE("result_temp7: %s\n",result_temp);
										temp_count = 0;
									}
								}
								else if (S6_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
									{

										data_count = data_count + 3;
										S6_on = false;
										S7_on = true;
										result_temp[temp_count - 1] = NULL;
										U_Pressure_Max[5] = atoi(result_temp);
										//TRACE("result_temp8: %s\n",result_temp);
										temp_count = 0;
									}
								}
								else if (S7_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
									{

										data_count = data_count + 3;
										S7_on = false;
										S8_on = true;
										result_temp[temp_count - 1] = NULL;
										U_Pressure_Max[6] = atoi(result_temp);
										//TRACE("result_temp9: %s\n",result_temp);
										temp_count = 0;
									}
								}
								else if (S8_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (data_count + 1 == Line_len)
									{
										//data_count = data_count + 2;
										S8_on = false;
										result_temp[temp_count - 1] = NULL;
										U_Pressure_Max[7] = atoi(result_temp);
										//TRACE("result_temp10: %s\n",result_temp);
										temp_count = 0;
									}
								}

							}

							// file에 max저장
							cList.Format(_T("\n%s, M%d, A1_MAX:%d, A2_MAX:%d, A3_MAX:%d, A4_MAX:%d, A5_MAX:%d, A6_MAX:%d, A7_MAX:%d, A8_MAX:%d\n"), time, Machine, U_Pressure_Max[0], U_Pressure_Max[1], U_Pressure_Max[2], U_Pressure_Max[3], U_Pressure_Max[4], U_Pressure_Max[5], U_Pressure_Max[6], U_Pressure_Max[7]);
							file.WriteString(cList);

							//TRACE("MAX: %d, %d, %d, %d, %d, %d, %d, %d\n", U_Pressure_Max[0], U_Pressure_Max[1], U_Pressure_Max[2], U_Pressure_Max[3], U_Pressure_Max[4], U_Pressure_Max[5], U_Pressure_Max[6], U_Pressure_Max[7]);

							// initialization
							Line_len = 0;
							//for (int a = 0; a < 8; a++)
							//	U_Pressure_Max[a] = 0;
							data_count = 0;
							temp_count = 0;
						}

						//count
						if (!strncmp("Co", rcvLine, 2))
						{
							Line_len = strlen(rcvLine);

							S1_on = true;
							for (data_count = 9; data_count < Line_len; data_count++)
							{
								if (S1_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
									{

										data_count = data_count + 3;
										S1_on = false;
										S2_on = true;
										result_temp[temp_count - 1] = NULL;
										U_Pressure_Cnt[0] = atoi(result_temp);
										//TRACE("result_temp3: %s\n",result_temp);
										temp_count = 0;
									}
								}
								else if (S2_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
									{

										data_count = data_count + 3;
										S2_on = false;
										S3_on = true;
										result_temp[temp_count - 1] = NULL;
										U_Pressure_Cnt[1] = atoi(result_temp);
										//TRACE("result_temp4: %s\n",result_temp);
										temp_count = 0;
									}
								}
								else if (S3_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
									{

										data_count = data_count + 3;
										S3_on = false;
										S4_on = true;
										result_temp[temp_count - 1] = NULL;
										U_Pressure_Cnt[2] = atoi(result_temp);
										//TRACE("result_temp5: %s\n",result_temp);
										temp_count = 0;
									}
								}
								else if (S4_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
									{

										data_count = data_count + 3;
										S4_on = false;
										S5_on = true;
										result_temp[temp_count - 1] = NULL;
										U_Pressure_Cnt[3] = atoi(result_temp);
										//TRACE("result_temp6: %s\n",result_temp);
										temp_count = 0;
									}
								}
								else if (S5_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
									{

										data_count = data_count + 3;
										S5_on = false;
										S6_on = true;
										result_temp[temp_count - 1] = NULL;
										U_Pressure_Cnt[4] = atoi(result_temp);
										//TRACE("result_temp7: %s\n",result_temp);
										temp_count = 0;
									}
								}
								else if (S6_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
									{

										data_count = data_count + 3;
										S6_on = false;
										S7_on = true;
										result_temp[temp_count - 1] = NULL;
										U_Pressure_Cnt[5] = atoi(result_temp);
										//TRACE("result_temp8: %s\n",result_temp);
										temp_count = 0;
									}
								}
								else if (S7_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (rcvLine[data_count] == ',')
									{

										data_count = data_count + 3;
										S7_on = false;
										S8_on = true;
										result_temp[temp_count - 1] = NULL;
										U_Pressure_Cnt[6] = atoi(result_temp);
										//TRACE("result_temp9: %s\n",result_temp);
										temp_count = 0;
									}
								}
								else if (S8_on)
								{
									result_temp[temp_count] = rcvLine[data_count];
									temp_count++;
									if (data_count + 1 == Line_len)
									{
										//data_count = data_count + 2;
										S8_on = false;
										result_temp[temp_count] = NULL;
										U_Pressure_Cnt[7] = atoi(result_temp);
										//TRACE("result_temp10: %s\n",result_temp);
										temp_count = 0;
									}
								}

							}
							// file에 cnt저장
							cList.Format(_T("%s, M%d, A1_CNT:%d, A2_CNT:%d, A3_CNT:%d, A4_CNT:%d, A5_CNT:%d, A6_CNT:%d, A7_CNT:%d, A8_CNT:%d\n\n"), time, Machine, U_Pressure_Cnt[0], U_Pressure_Cnt[1], U_Pressure_Cnt[2], U_Pressure_Cnt[3], U_Pressure_Cnt[4], U_Pressure_Cnt[5], U_Pressure_Cnt[6], U_Pressure_Cnt[7]);
							file.WriteString(cList);

							//TRACE("cnt: %d, %d, %d, %d, %d, %d, %d, %d\n", U_Pressure_Cnt[0], U_Pressure_Cnt[1], U_Pressure_Cnt[2], U_Pressure_Cnt[3], U_Pressure_Cnt[4], U_Pressure_Cnt[5], U_Pressure_Cnt[6], U_Pressure_Cnt[7]);

							// initialization
							Line_len = 0;
							//for (int a = 0; a < 8; a++)
							//	U_Pressure_Cnt[a] = 0;
							data_count = 0;
							temp_count = 0;
						}
					}
					//TRACE(rcvLine);
					rcvLine = strtok(NULL, "\n");
				}
				//pDlg->Wait(100);
			}
			else
			{
				msec_temp = 0;
				time_check = true;
				break;
				//file and DB close
				//file.Close();
			}
		}
		file.Close();

		if (Line_len == 0)
		{
			//m_ano_list.InsertString(-1, _T("The sensor is disconnected through the Wifi"));
			//m_ano_list.InsertString(-1, _T("---------------------------------"));
			Wifi = false;
		}

		// disconnect button
		if (WDIS_SIGNAL)
		{
			m_ano_list.InsertString(-1, _T("The sensor is disconnected through the Wifi"));
			m_ano_list.InsertString(-1, _T("---------------------------------"));
			Wifi = false;

			closesocket(connSock);
			closesocket(listenSock);
			WDIS_SIGNAL = false;
			return 0;
		}

		//Wfile.Close();
		closesocket(connSock);
	}
	closesocket(listenSock);

	return 0;
}










//uart thread
UINT CRtGraphDlg::UART_Thread(LPVOID pParam)
{

	CRtGraphDlg* pDlg = (CRtGraphDlg*)pParam;

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

	float sec = 0;

	CString time;

	CStdioFile file;

	CString cList; //한줄로 저장할 데이터

	// pressure data variables
	int sensor[8] = { 0, };

	int msec_time = 0;

	int msec_temp = 0;

	int data_count = 0;

	int temp_count = 0;

	// Maximun value and Count
	//int U_Pressure_Max[8] = { 0, };

	//int U_Pressure_Cnt[8] = { 0, };

	if (!_serial.OpenPort(UART_PORT[U_PORT + 1]))// 실제 사용될 COM Port
	{
		TRACE("Serial Open port error\n");
		TRACE("error code : %d\n", GetLastError());
		m_ano_list.InsertString(-1, _T("Serial port open error"));
		return 0;
	}
	else
	{
		m_ano_list.InsertString(-1, _T("Serial port open"));
	}

	//BaudRate, ByteSize, fParity, Parity, StopBit 정보를 설정
	if (!_serial.ConfigurePort(UART_RATE[U_RATE], 8, FALSE, NOPARITY, ONESTOPBIT))
	{
		TRACE("Serial ConfigurePort error\n");
		m_ano_list.InsertString(-1, _T("Configure serial port error"));
		return 0;
	}
	else
	{
		m_ano_list.InsertString(-1, _T("Configure serial port complete"));
	}

	if (!_serial.SetCommunicationTimeouts(0, 0, 0, 0, 0))
	{
		TRACE("Serial SetCommunicationTimeouts error\n");
		m_ano_list.InsertString(-1, _T("Serial SetCommunicationTimeouts error"));
		return 0;
	}
	else
	{
		m_ano_list.InsertString(-1, _T("The sensor is connected with UART"));
		m_ano_list.InsertString(-1, _T("---------------------------------"));
	}


	SYSTEMTIME cur_time;


	//file and DB open
	GetLocalTime(&cur_time);

	years = cur_time.wYear;
	months = cur_time.wMonth;
	days = cur_time.wDay;
	CString fName;

	while (pDlg->m_U_ThreadWork == THREAD_RUNNING)
	{


		if (_serial.Readline(ReadBuf)) {

			fName.Format(_T("D:\\2019_Summer_Kidsoft\\Intern_Hyunbin\\RtGraph\\data\\%d.%d.%d.(Machine%d).txt"), years, months, days, Machine); //오픈할 파일 경로

			file.Open(fName, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate | CFile::typeText);
			TRACE("file open\n");
			file.SeekToEnd();

			TRACE("%s\n", ReadBuf);

			if (!strcmp(ReadBuf, temp)) // UART usb disconnected
			{
				//TRACE("Same str\n");
				pDlg->OnBnClickedButton8(); //Thread disconnect
			}


			strcpy(temp, ReadBuf);

			SYSTEMTIME cur_time;


			//file and DB open
			GetLocalTime(&cur_time);

			int temp_days = cur_time.wDay;

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

					sec = second + (float)msecond / 1000;

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
							//TRACE("%d\n", milli_Gap);
							if (msec_temp)
							{
								milli_Gap = msec_time - msec_temp;
								sec = sec + (float)(milli_Gap) / 1000;
								msecond = msecond + (msec_time - msec_temp);
								if (msecond >= 1000)
								{
									second++;
									msecond = msecond - 1000;
								}
							}

							msec_temp = msec_time;

							time.Format(_T("%02d:%02d:%0.3f"), hour, minute, sec);

							//TRACE("time: %S\n", time);
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
							int temp_Machine = atoi(result_temp);
							if (Machine != temp_Machine)
							{
								Machine = atoi(result_temp);
							}

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

				if (Chart_Start)
				{
					Data_Queue.push(sensor);
					pDlg->Wait(10);
				}


				cList.Format(_T("%s, M%d, A1:%d, A2:%d, A3:%d, A4:%d, A5:%d, A6:%d, A7:%d, A8:%d\n"), time, Machine, sensor[0], sensor[1], sensor[2], sensor[3], sensor[4], sensor[5], sensor[6], sensor[7]);
				//TRACE("%s\n", cList);
				file.WriteString(cList);

				// initialization
				readlen = 0;
				//for (int a = 0; a < 8; a++)
					//sensor[a] = 0;
				msec_time = 0;
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
							U_Pressure_Max[0] = atoi(result_temp);
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
							U_Pressure_Max[1] = atoi(result_temp);
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
							U_Pressure_Max[2] = atoi(result_temp);
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
							U_Pressure_Max[3] = atoi(result_temp);
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
							U_Pressure_Max[4] = atoi(result_temp);
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
							U_Pressure_Max[5] = atoi(result_temp);
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
							U_Pressure_Max[6] = atoi(result_temp);
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
							U_Pressure_Max[7] = atoi(result_temp);
							//TRACE("result_temp10: %s\n",result_temp);
							temp_count = 0;
						}
					}

				}

				// file에 max저장
				cList.Format(_T("\n%s, M%d, A1_MAX:%d, A2_MAX:%d, A3_MAX:%d, A4_MAX:%d, A5_MAX:%d, A6_MAX:%d, A7_MAX:%d, A8_MAX:%d\n"), time, Machine, U_Pressure_Max[0], U_Pressure_Max[1], U_Pressure_Max[2], U_Pressure_Max[3], U_Pressure_Max[4], U_Pressure_Max[5], U_Pressure_Max[6], U_Pressure_Max[7]);
				file.WriteString(cList);

				//pDlg->Wait(10);
				//TRACE("MAX: %d, %d, %d, %d, %d, %d, %d, %d\n", U_Pressure_Max[0], U_Pressure_Max[1], U_Pressure_Max[2], U_Pressure_Max[3], U_Pressure_Max[4], U_Pressure_Max[5], U_Pressure_Max[6], U_Pressure_Max[7]);

				// initialization
				readlen = 0;
				//for (int a = 0; a < 8; a++)
				//	U_Pressure_Max[a] = 0;
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
							U_Pressure_Cnt[0] = atoi(result_temp);
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
							U_Pressure_Cnt[1] = atoi(result_temp);
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
							U_Pressure_Cnt[2] = atoi(result_temp);
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
							U_Pressure_Cnt[3] = atoi(result_temp);
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
							U_Pressure_Cnt[4] = atoi(result_temp);
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
							U_Pressure_Cnt[5] = atoi(result_temp);
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
							U_Pressure_Cnt[6] = atoi(result_temp);
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
							U_Pressure_Cnt[7] = atoi(result_temp);
							//TRACE("result_temp10: %s\n",result_temp);
							temp_count = 0;
						}
					}

				}

				// file에 cnt저장
				cList.Format(_T("%s, M%d, A1_CNT:%d, A2_CNT:%d, A3_CNT:%d, A4_CNT:%d, A5_CNT:%d, A6_CNT:%d, A7_CNT:%d, A8_CNT:%d\n\n"), time, Machine, U_Pressure_Cnt[0], U_Pressure_Cnt[1], U_Pressure_Cnt[2], U_Pressure_Cnt[3], U_Pressure_Cnt[4], U_Pressure_Cnt[5], U_Pressure_Cnt[6], U_Pressure_Cnt[7]);
				file.WriteString(cList);
				//TRACE("cnt: %d, %d, %d, %d, %d, %d, %d, %d\n", U_Pressure_Cnt[0], U_Pressure_Cnt[1], U_Pressure_Cnt[2], U_Pressure_Cnt[3], U_Pressure_Cnt[4], U_Pressure_Cnt[5], U_Pressure_Cnt[6], U_Pressure_Cnt[7]);

				// initialization
				readlen = 0;
				//for (int a = 0; a < 8; a++)
				//	U_Pressure_Cnt[a] = 0;
				data_count = 0;
				temp_count = 0;
			}
			file.Close();
		}
		else
		{
			msec_temp = 0;
			time_check = true;

		}


		if (UART_SET_SIGNAL)
		{
			//_serial.WriteLine(CMD);
			for (int i = 0; i < 18; i++) {
				if (_serial.WriteByte(set_cmd[i]))
				{
					//TRACE("I send %c\n", CMD[i]);
				}

			}
			m_ano_list.InsertString(-1, _T("The settings have been reset"));
			m_ano_list.InsertString(-1, _T("---------------------------------"));
			UART_SET_SIGNAL = false;
		}

		if (UDIS_SIGNAL)
		{
			m_ano_list.InsertString(-1, _T("The sensor is disconnected through the UART"));
			m_ano_list.InsertString(-1, _T("---------------------------------"));
			//temp.Empty();
			memset(temp, '0', 200);
			//cList.Empty();
			memset(ReadBuf, '0', 200);

			UART = false;

			_serial.ClosePort();
			TRACE("port close\n");
			UDIS_SIGNAL = false;
			return 0;

		}
	}

	m_ano_list.InsertString(-1, _T("The sensor is disconnected through the UART"));
	m_ano_list.InsertString(-1, _T("---------------------------------"));
	//temp.Empty();
	memset(temp, '0', 200);
	//cList.Empty();
	memset(ReadBuf, '0', 200);
	UART = false;


	_serial.ClosePort();
	TRACE("port close\n");


	return 0;
}



// "Wifi disconnect" button
void CRtGraphDlg::OnBnClickedButton7()
{
	//Wifi Disconnect signal
	if (Wifi) {
		WDIS_SIGNAL = true;
		Wifi = false;
	}
	else
	{
		MessageBox(_T("Wifi is not connected."), _T("Wifi connection"), MB_ICONINFORMATION);
	}

	Wait(100);

	//thread stop
	m_W_Thread->SuspendThread();

	DWORD dwResult;
	::GetExitCodeThread(m_W_Thread->m_hThread, &dwResult);

	delete m_W_Thread;
	m_W_Thread = NULL;

	m_W_ThreadWork = THREAD_STOP;

}

// "UART disconnect" button
void CRtGraphDlg::OnBnClickedButton8()
{
	//TRACE("UART disconnect button on\n");

	if (UART) {
		//UART Disconnect signal
		UDIS_SIGNAL = true;
		UART = false;
	}
	else
	{
		MessageBox(_T("UART is not connected."), _T("UART connection"), MB_ICONINFORMATION);
		return;
	}

	Wait(100);

	//thread stop
	m_U_Thread->SuspendThread();
	DWORD dwResult;
	::GetExitCodeThread(m_U_Thread->m_hThread, &dwResult);

	delete m_U_Thread;
	m_U_Thread = NULL;

	m_U_ThreadWork = THREAD_STOP;

	TRACE("Uthread stop\n");
}

// "Set measurement" button
void CRtGraphDlg::OnBnClickedButton9()
{

	int Sampling_Rate = 1;
	int Sampling_Algo = 1;
	int Threshold = 2;
	int Offset_mode = 1;

	int Howdata = 1;

	Sampling_Rate = m_SRATE_combo.GetCurSel() + 1;
	set_cmd[2] = Sampling_Rate + '0';

	Sampling_Algo = m_SALGO_combo.GetCurSel() + 1;
	set_cmd[5] = Sampling_Algo + '0';
	//set_cmd[5] = '1';

	Threshold = m_Thesh_combo.GetCurSel() + 1;
	set_cmd[8] = Threshold + '0';
	//set_cmd[8] = '2';

	Offset_mode = m_Offset_combo.GetCurSel() + 1;
	set_cmd[11] = Offset_mode + '0';
	//set_cmd[11] = '2';


	//TRACE("set_cmd : %s\n", set_cmd);

	if (UART)
	{
		UART_SET_SIGNAL = true;
	}
	else
	{
		MessageBox(_T("UART is not connected.\nTo set up, this program must be connected to UART."), _T("UART connection"), MB_ICONINFORMATION);
		m_ano_list.InsertString(-1, _T("The settings failed"));
	}

}
