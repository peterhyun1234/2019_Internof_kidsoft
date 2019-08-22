
// RtGraphDlg.h : header file
//

#pragma once

#include "sqlite3.h"

#define ORIGIN_X 55
#define ORIGIN_Y 970
#define X_AXIS_LENGTH 430
#define Y_AXIS_LENGTH 320
#define BAR_WIDTH 45
#define Y_label_num 5
#define bar_num 9 // bar 갯수
#define BAR_COLOR RGB(0,0,255) // 파란색

#define N 500
#define RNG 3200

#define PORT 9002

#include "OScopeCtrl.h"
#include "ColorListBox.h"

// CRtGraphDlg dialog
class CRtGraphDlg : public CDialog
{
// Construction
public:
	CRtGraphDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_RTGRAPH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	COScopeCtrl *_rtGraph;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	void getSerialPort();
	void drawSolidRectangle(int x, int width, int height, int Value);
	void load_gaussian(int arr[]);
	double gaussianRandom(void);
	void Wait(DWORD dwMillisecond);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton3();
	afx_msg
		static UINT Wifi_Thread(LPVOID pParam);
	afx_msg
		static UINT UART_Thread(LPVOID pParam);
	afx_msg
		void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg
		void OnEnChangeEdit1();


	// 스레드 관련 변수들
	enum ThreadWorkingType
	{
		THREAD_STOP = 0,
		THREAD_RUNNING = 1,
		THREAD_PAUSE = 2
	};
	bool m_W_ThreadStart;
	bool m_U_ThreadStart;
	CWinThread* m_W_Thread = NULL;
	CWinThread* m_U_Thread = NULL;
	ThreadWorkingType m_W_ThreadWork;
	ThreadWorkingType m_U_ThreadWork;

	//제어를 위한 변수들
	CIPAddressCtrl m_IP;

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	CComboBox m_UPORT_combo;
	CComboBox m_URATE_combo;
	CComboBox m_SRATE_combo;
	CComboBox m_SALGO_combo;
	CSliderCtrl m_ctr_slider;
	CEdit m_edit_slider;
	CComboBox m_Thesh_combo;
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	CComboBox m_Offset_combo;
	CDateTimeCtrl m_DTP_START;
	CDateTimeCtrl M_DTP_END;
	CEdit m_SSID_edit;
	CEdit m_PASSWORD_edit;
};
