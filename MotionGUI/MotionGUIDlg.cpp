
// MotionGUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MotionGUI.h"
#include "MotionGUIDlg.h"
#include "afxdialogex.h"

//-------------------dT begin--------------------------------------
#include <math.h>

#include "AXM.h"
#include "AXL.h"

#include "../MotionGUI/ChartCtrl/ChartCtrl.h"
#include "../MotionGUI/ChartCtrl/ChartAxisLabel.h"
#include "../MotionGUI/ChartCtrl/ChartLabel.h"
#include "../MotionGUI/ChartCtrl/ChartString.h"
#include "../MotionGUI/ChartCtrl/ChartLineSerie.h"
#include "../MotionGUI/ChartCtrl/ChartPointsSerie.h"

#include "../MotionGUI/JogButton.h"


#define TICK_INCREMENT			50
#define PLOT_COUNTS(miliSec)	miliSec/TICK_INCREMENT
#define TIMER1					0		//draw chart
#define TIMER1_INTERVAL			TICK_INCREMENT
#define TIMER2					1
#define TIMER2_INTERVAL			250
#define CHART_PATH_RANGE		5000
#define SLIDER_RANGE			1000
#define CHART_POINT_SIZE		7
#define NUMBER_OF_AXES			2
#define AXIS_X					0
#define AXIS_Y					1 - AXIS_X
#define THREAD_SLEEP			2000

//-------Chart begin-----
long lPlotCount = 0;
double dObsTime = 10000;	//initial value of observation time = 10s
//-------Chart end-----

bool xThreadRepeatStop;

UINT RepeatThread(LPVOID Param);
//-------------------dT end--------------------------------------




#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CMotionGUIDlg dialog



CMotionGUIDlg::CMotionGUIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMotionGUIDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMotionGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHART_PATH, m_ChartPath);
	DDX_Control(pDX, IDC_CHART_POS, m_ChartPos);
	DDX_Control(pDX, IDC_CHART_VEL, m_ChartVel);
	DDX_Control(pDX, IDC_LIST_EVENT_LOG, m_lsbLog);
	DDX_Control(pDX, IDC_COMBO_AXISSET, m_cboAxisSet);
	DDX_Control(pDX, IDC_COMBO_PULSE_OUT_METHOD, m_cboPulseOutMethod);
	DDX_Control(pDX, IDC_COMBO_ENCODER_INPUT_METHOD, m_cboEncoderInputMethod);
	DDX_Control(pDX, IDC_COMBO_VELOCITY_PROFILE, m_cboVelProfile);
	DDX_Control(pDX, IDC_COMBO_ABS_REL_MODE, m_cboAbsRelMode);
	DDX_Control(pDX, IDC_SLIDER_VALUE, m_sliValue);
	DDX_Control(pDX, IDC_RADIO_MONITOR_AXIS_0, m_rdoMonitorAxis0);
	DDX_Radio(pDX, IDC_RADIO_MONITOR_AXIS_0, m_rdoMonitorAxis);
	DDX_Radio(pDX, IDC_RADIO_SINGLE_AXIS_0, m_rdoSingleAxis);
	DDX_Radio(pDX, IDC_RADIO_OVRD_POS, m_rdoOverride);
	DDX_Radio(pDX, IDC_RADIO_INTRPL_ABS, m_rdoInterpolation);
	DDX_Radio(pDX, IDC_RADIO_CONTI_STRLINE, m_rdoContiAddNode);
	DDX_Control(pDX, IDC_RADIO_SINGLE_AXIS_0, m_rdoSingleAxis0);
	DDX_Control(pDX, IDC_CHECK_SINGLE_SYN_ASYN, m_chkSingleSynAsyn);
	DDX_Control(pDX, IDC_RADIO_OVRD_POS, m_rdoOvrdPos);
	DDX_Control(pDX, IDC_RADIO_INTRPL_ABS, m_rdoIntrplAbs);
	DDX_Control(pDX, IDC_CHECK_INTRPL_CIRCENTER_DIR, m_chkIntrplCirCenterDir);
	DDX_Control(pDX, IDC_CHECK_INTRPL_CIRRADIUS_DIR, m_chkIntrplCirRadiusDir);
	DDX_Control(pDX, IDC_CHECK_INTRPL_CIRRADIUS_DISTANCE, m_chkIntrplCirRadiusDistance);
	DDX_Control(pDX, IDC_CHECK_INTRPL_CIRANGLE_DIR, m_chkIntrplCirAngleDir);
	DDX_Control(pDX, IDC_CHECK_CHART_LEGEND, m_chkChartLegend);
	DDX_Control(pDX, IDC_CHECK_SIMULATE, m_chkSimulate);
	DDX_Control(pDX, IDC_CHECK_SERVO_ON, m_chkServoOn);
	DDX_Control(pDX, IDC_CHECK_ALARM, m_chkAlarm);
	DDX_Control(pDX, IDC_CHECK4, m_chkOvrdTarget);
	DDX_Control(pDX, IDC_CHECK_INTRPL_CIRPOINT_ARC_CIRCLE, m_chkIntrplCirPointArcCircle);
	DDX_Control(pDX, IDC_JOGBUTTON_SINGLE_P, m_jbtnSingleP);
	DDX_Control(pDX, IDC_JOGBUTTON_SINGLE_N, m_jbtnSingleN);
}

BEGIN_MESSAGE_MAP(CMotionGUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CMotionGUIDlg::OnBnClickedButtonTest)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_AXISSET, &CMotionGUIDlg::OnCbnSelchangeComboAxisset)
	ON_BN_CLICKED(IDC_BUTTON_SETTING_AXIS, &CMotionGUIDlg::OnBnClickedButtonSettingAxis)
	ON_BN_CLICKED(IDC_BUTTON_SETTING_AXIS_ALL, &CMotionGUIDlg::OnBnClickedButtonSettingAxisAll)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_PATH, &CMotionGUIDlg::OnBnClickedButtonClearPath)
	ON_BN_CLICKED(IDC_BUTTON_ADD_POINT, &CMotionGUIDlg::OnBnClickedButtonAddPoint)

	ON_EN_KILLFOCUS(IDC_EDIT_PATH_RANGE_MAX_Y, &CMotionGUIDlg::OnEnKillfocusEditPathRangeMaxY)
	ON_EN_KILLFOCUS(IDC_EDIT_PATH_RANGE_MAX_X, &CMotionGUIDlg::OnEnKillfocusEditPathRangeMaxX)
	ON_EN_KILLFOCUS(IDC_EDIT_PATH_RANGE_MIN_X, &CMotionGUIDlg::OnEnKillfocusEditPathRangeMinX)
	ON_EN_KILLFOCUS(IDC_EDIT_PATH_RANGE_MIN_Y, &CMotionGUIDlg::OnEnKillfocusEditPathRangeMinY)
	ON_BN_CLICKED(IDC_BUTTON_CHART_OBSERVATION_TIME, &CMotionGUIDlg::OnBnClickedButtonChartObservationTime)
	ON_BN_CLICKED(IDC_CHECK_CHART_LEGEND, &CMotionGUIDlg::OnBnClickedCheckChartLegend)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_MOVE, &CMotionGUIDlg::OnBnClickedButtonSingleMove)
	ON_BN_CLICKED(IDC_BUTTON_MULTI_MOVE, &CMotionGUIDlg::OnBnClickedButtonMultiMove)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_STOP, &CMotionGUIDlg::OnBnClickedButtonSingleStop)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_ESTOP, &CMotionGUIDlg::OnBnClickedButtonSingleEstop)
	ON_BN_CLICKED(IDC_BUTTON_MULTI_STOP, &CMotionGUIDlg::OnBnClickedButtonMultiStop)
	ON_BN_CLICKED(IDC_BUTTON_MULTI_ESTOP, &CMotionGUIDlg::OnBnClickedButtonMultiEstop)
	ON_BN_CLICKED(IDC_BUTTON_MONITOR_CLEAR, &CMotionGUIDlg::OnBnClickedButtonMonitorClear)
	ON_BN_CLICKED(IDC_BUTTON_MONITOR_CLEAR_ALL, &CMotionGUIDlg::OnBnClickedButtonMonitorClearAll)
	ON_BN_CLICKED(IDC_CHECK_SERVO_ON, &CMotionGUIDlg::OnBnClickedCheckServoOn)
	ON_BN_CLICKED(IDC_CHECK_ALARM, &CMotionGUIDlg::OnBnClickedCheckAlarm)
	ON_BN_CLICKED(IDC_BUTTON_OVRD_OVERRIDE, &CMotionGUIDlg::OnBnClickedButtonOvrdOverride)
	ON_BN_CLICKED(IDC_BUTTON_INTRPL_STRLINE, &CMotionGUIDlg::OnBnClickedButtonIntrplStrline)
	ON_BN_CLICKED(IDC_BUTTON_INTRPL_CIRCENTER, &CMotionGUIDlg::OnBnClickedButtonIntrplCircenter)
	ON_BN_CLICKED(IDC_BUTTON_INTRPL_CIRRADIUS, &CMotionGUIDlg::OnBnClickedButtonIntrplCirradius)
	ON_BN_CLICKED(IDC_BUTTON_INTRPL_CIRANGLE, &CMotionGUIDlg::OnBnClickedButtonIntrplCirangle)
	ON_BN_CLICKED(IDC_BUTTON_INTRPL_CIRPOINT, &CMotionGUIDlg::OnBnClickedButtonIntrplCirpoint)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_REPEAT, &CMotionGUIDlg::OnBnClickedButtonSingleRepeat)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_JOG_P, &CMotionGUIDlg::OnBnClickedButtonSingleJogP)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_JOG_N, &CMotionGUIDlg::OnBnClickedButtonSingleJogN)
	ON_BN_CLICKED(IDC_BUTTON_SERVO_ON, &CMotionGUIDlg::OnBnClickedButtonServoOn)
	ON_BN_CLICKED(IDC_BUTTON_RESET_ALARM, &CMotionGUIDlg::OnBnClickedButtonResetAlarm)
	ON_BN_CLICKED(IDC_BUTTON_CONTI_BEGIN_NODE, &CMotionGUIDlg::OnBnClickedButtonContiBeginNode)
	ON_BN_CLICKED(IDC_BUTTON_CONTI_END_NODE, &CMotionGUIDlg::OnBnClickedButtonContiEndNode)
	ON_BN_CLICKED(IDC_BUTTON_CONTI_ADD_NODE, &CMotionGUIDlg::OnBnClickedButtonContiAddNode)
	ON_BN_CLICKED(IDC_BUTTON_CONTI_START, &CMotionGUIDlg::OnBnClickedButtonContiStart)
	ON_MESSAGE(UM_JOG_BTN_UP, OnJogBtnUp)
	ON_MESSAGE(UM_JOG_BTN_DN, OnJogBtnDn)
END_MESSAGE_MAP()


// CMotionGUIDlg message handlers

void CMotionGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMotionGUIDlg::OnPaint()
{
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMotionGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CMotionGUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//-------------------------dT begin------------------------------------------------------------------------------------
	m_sliValue.SetRange(-SLIDER_RANGE, SLIDER_RANGE);	//dTTesting: simulate value
	m_sliValue.SetPos(-SLIDER_RANGE);
	
	InitLibrary();
	InitMotionSignal();							//disable unnecessary motion singal (elimit, inpos,...)
	InitControl();								//step1: initialized value for control, not yet the axes  
	ApplySettingAxisAll();						//step2: apply all axes once
	UpdateAxisInfo(m_cboAxisSet.GetCurSel());	//step3: and then display axes' value on control, not initialized value any more
	EnableListboxHorizontalScrollbar();
	SetTimer(TIMER1, TICK_INCREMENT, NULL);
	SetTimer(TIMER2, TIMER2_INTERVAL, NULL);
	//GetDlgItem(IDC_STATIC_PATH_LOCATION)->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	//-------------------------dT end------------------------------------------------------------------------------------
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CMotionGUIDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	long lAxisCounts = NUMBER_OF_AXES;
	for (int i = 0; i < lAxisCounts; i++){
		AxmMoveSStop(i);
		AxmSignalServoOn(i, 0);	//turn off
	}
	WriteLog(_T("Turned off all servo"));
	AxlClose();
}

void CMotionGUIDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER1){
		//path planning
		static double m_dLastActPos0 = 0;
		static double m_dLastActPos1 = 0;
		double m_dActPos0, m_dActPos1;
		if (!m_chkSimulate.GetCheck()){
			AxmStatusGetActPos(0, &m_dActPos0);

			//*implement temp: encode axis 1 k co nen dung tam cmd
			AxmStatusGetActPos(1, &m_dActPos1);	
			AxmStatusGetCmdPos(1, &m_dActPos1);	
			//------------

		}
		else{
			m_dActPos0 = m_sliValue.GetPos();
			m_dActPos1 = abs(m_sliValue.GetPos()) - 50;
		}

		if ((m_dActPos0 != m_dLastActPos0) || (m_dActPos1 != m_dLastActPos1)){	//if position changed
			m_pChartPathPointSerie->AddPoint(m_dActPos0, m_dActPos1);
			DisplayLocation(m_dActPos0, m_dActPos1);
			m_dLastActPos0 = m_dActPos0;
			m_dLastActPos1 = m_dActPos1;

			double dRangeMin[2], dRangeMax[2];	//[0]: bottom axis, [1]: left axis
			m_ChartPath.GetBottomAxis()->GetMinMax(dRangeMin[0], dRangeMax[0]);
			m_ChartPath.GetLeftAxis()->GetMinMax(dRangeMin[1], dRangeMax[1]);
			m_ChartPath.RefreshCtrl();	//display new point
			//RefreshCtrl will change the range, so chart range is needed to be set again
			m_ChartPath.GetBottomAxis()->SetMinMax(dRangeMin[0], dRangeMax[0]);
			m_ChartPath.GetLeftAxis()->SetMinMax(dRangeMin[1], dRangeMax[1]);
		}
		
		//*implement: writelog to notify whether point is outside of range

		double dTickIncrement = TICK_INCREMENT;
		static double dTime = -dTickIncrement;
		lPlotCount++;
		dTime += dTickIncrement;
		double m_dCmdPos0, m_dCmdPos1;
		if (!m_chkSimulate.GetCheck()){
			AxmStatusGetCmdPos(0, &m_dCmdPos0);
			AxmStatusGetCmdPos(1, &m_dCmdPos1);
		}
		else{
			m_dCmdPos0 = m_sliValue.GetPos();
			m_dCmdPos1 = abs(m_sliValue.GetPos()) - 50;
		}
		//PLOT_COUNTS = (Total time plot) / (Timer)
		if (lPlotCount >= PLOT_COUNTS(dObsTime)){
			m_pChartPos0LineSerie->RemovePointsFromBegin(1);
			m_pChartPos1LineSerie->RemovePointsFromBegin(1);
		}
		m_pChartPos0LineSerie->AddPoint(dTime / 1000, m_dCmdPos0);
		m_pChartPos1LineSerie->AddPoint(dTime / 1000, m_dCmdPos1);

		/*double dTickIncrement = TICK_INCREMENT;
		static long lPlotCount = 0;
		static double dTime = -dTickIncrement;
		lPlotCount++;
		dTime += dTickIncrement;*/
		double m_dCmdVel0, m_dCmdVel1;
		if (!m_chkSimulate.GetCheck()){
			AxmStatusReadVel(0, &m_dCmdVel0);
			AxmStatusReadVel(1, &m_dCmdVel1);
		}
		else{
			m_dCmdVel0 = m_sliValue.GetPos();
			m_dCmdVel1 = abs(m_sliValue.GetPos()) - 50;
		}
		if (lPlotCount >= PLOT_COUNTS(dObsTime)){
			m_pChartVel0LineSerie->RemovePointsFromBegin(1);
			m_pChartVel1LineSerie->RemovePointsFromBegin(1);
		}
		m_pChartVel0LineSerie->AddPoint(dTime / 1000, m_dCmdVel0);
		m_pChartVel1LineSerie->AddPoint(dTime / 1000, m_dCmdVel1);



		double m_dActVel0, m_dActVel1;
		if (!m_chkSimulate.GetCheck()){
			/*AxmStatusReadActVel(0, &m_dActVel0);	//ajinextek does not support this
			AxmStatusReadActVel(1, &m_dActVel1);*/	//so assign cmd value to act value
			m_dActVel0 = m_dCmdVel0;
			m_dActVel1 = m_dCmdVel1;
		}
		else{
			m_dActVel0 = m_dCmdVel0;
			m_dActVel1 = m_dCmdVel1;
		}
		UpdateData(true);	//use for the DDX_Radio()
		long lAxisNo = m_rdoMonitorAxis;
		//*implementation done
		if (lAxisNo == 0){			//Axis 0
			SetDlgItemDouble(IDC_EDIT_MONITOR_CMD_POS, m_dCmdPos0);
			SetDlgItemDouble(IDC_EDIT_MONITOR_ACT_POS, m_dActPos0);
			SetDlgItemDouble(IDC_EDIT_MONITOR_CMD_VEL, m_dCmdVel0);
			SetDlgItemDouble(IDC_EDIT_MONITOR_ACT_VEL, m_dActVel0);
		}
		else if (lAxisNo == 1){	//Axis 1
			SetDlgItemDouble(IDC_EDIT_MONITOR_CMD_POS, m_dCmdPos1);
			SetDlgItemDouble(IDC_EDIT_MONITOR_ACT_POS, m_dActPos1);
			SetDlgItemDouble(IDC_EDIT_MONITOR_CMD_VEL, m_dCmdVel1);
			SetDlgItemDouble(IDC_EDIT_MONITOR_ACT_VEL, m_dActVel1);
		}
	}

	if (nIDEvent == TIMER2){
		UpdateData(true);
		if (!m_chkSimulate.GetCheck()){
			DWORD uAbsRelMode[2];	//Axis 0 & 1
			AxmMotGetAbsRelMode(0, &uAbsRelMode[0]);	//axis 0
			AxmMotGetAbsRelMode(1, &uAbsRelMode[1]);	//axis 1
			//Single axis
			long lAxisNo = m_rdoSingleAxis;
			if (uAbsRelMode[lAxisNo] == 0)		GetDlgItem(IDC_EDIT_SINGLE_ABS_REL)->SetWindowTextW(_T("Abs"));
			else if (uAbsRelMode[lAxisNo] == 1)	GetDlgItem(IDC_EDIT_SINGLE_ABS_REL)->SetWindowTextW(_T("Rel"));
			else								GetDlgItem(IDC_EDIT_SINGLE_ABS_REL)->SetWindowTextW(_T("FAULT"));
			//Multi axis
			lAxisNo = 0;
			if (uAbsRelMode[lAxisNo] == 0)		GetDlgItem(IDC_EDIT_MULTI_ABS_REL_0)->SetWindowTextW(_T("Abs"));
			else if (uAbsRelMode[lAxisNo] == 1)	GetDlgItem(IDC_EDIT_MULTI_ABS_REL_0)->SetWindowTextW(_T("Rel"));
			else								GetDlgItem(IDC_EDIT_MULTI_ABS_REL_0)->SetWindowTextW(_T("FAULT"));
			lAxisNo = 1;
			if (uAbsRelMode[lAxisNo] == 0)		GetDlgItem(IDC_EDIT_MULTI_ABS_REL_1)->SetWindowTextW(_T("Abs"));
			else if (uAbsRelMode[lAxisNo] == 1)	GetDlgItem(IDC_EDIT_MULTI_ABS_REL_1)->SetWindowTextW(_T("Rel"));
			else								GetDlgItem(IDC_EDIT_MULTI_ABS_REL_1)->SetWindowTextW(_T("FAULT"));

			//Axis Monitor
			DWORD uStatus;
			lAxisNo = m_rdoMonitorAxis;
			AxmSignalReadServoAlarm(lAxisNo, &uStatus);
			m_chkAlarm.SetCheck(uStatus);
			AxmSignalIsServoOn(lAxisNo, &uStatus);
			m_chkServoOn.SetCheck(uStatus);
		}
		else{
			GetDlgItem(IDC_EDIT_SINGLE_ABS_REL)->SetWindowTextW(_T("Test"));
			GetDlgItem(IDC_EDIT_MULTI_ABS_REL_0)->SetWindowTextW(_T("Test"));
			GetDlgItem(IDC_EDIT_MULTI_ABS_REL_1)->SetWindowTextW(_T("Test"));
		}

		//EnableListboxHorizontalScrollbar();
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CMotionGUIDlg::OnBnClickedButtonTest()
{
	
	//static bool xState = 0;
	//xState = 1 - xState;	//toggle
	//if (xState){
	//	//AfxBeginThread(RepeatThread, 0);
	//	xThreadRepeatStop = false;
	//	unsigned long	ThreadID;
	//	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RepeatThread, this, NULL, &ThreadID);
	//}
	//else{
	//	xThreadRepeatStop = true;
	//}

	//AxmOverrideVelAtPos(0, 1000, 20, 100, 100, 200, 100, 1);


	WriteLog(_T("Test button is clicked"));

	/*long m_lAxis = m_cboAxisSet.GetCurSel();
	CString sTemp;
	sTemp.Format(_T("m_lAxis = %ld"), m_lAxis);
	m_lsbLog.InsertString(0, sTemp);*/


	////test get data
	//long lTemp[50];
	//lTemp[0] = m_cboAxisSet.GetCurSel();
	//lTemp[1] = m_cboPulseOutMethod.GetCurSel();
	//lTemp[2] = m_cboEncoderInputMethod.GetCurSel();
	//lTemp[3] = m_cboVelProfile.GetCurSel();
	//lTemp[4] = m_cboAbsRelMode.GetCurSel();
	//lTemp[5] = GetDlgItemDouble(IDC_EDIT_VELOCITY_MAX);
	//lTemp[6] = GetDlgItemDouble(IDC_EDIT_VELOCITY_MIN);
	//lTemp[7] = GetDlgItemDouble(IDC_EDIT_UNIT);	
	//lTemp[8] = (long)GetDlgItemDouble(IDC_EDIT_PULSE);


	//UpdateData(true);	//use for the "DDX_Radio(pDX, IDC_RADIO_MONITOR_AXIS_0, m_rdoMonitorAxis);"
	//int iTemp = m_rdoMonitorAxis;
	//CString sTemp;
	//sTemp.Format(_T("Axis 1:%d"), iTemp);
	//WriteLog(sTemp);
	
	//m_pChartPos0LineSerie->SetVisible(m_chkIntrplCirAngleDir.GetCheck());
}

void CMotionGUIDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
	if (pScrollBar == (CScrollBar *)&m_sliValue) {
		int value = m_sliValue.GetPos();
	}
	else {
		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}
}




double CMotionGUIDlg::GetDlgItemDouble(int nID)
{
	double dRet;
	CString sTemp;
	GetDlgItem(nID)->GetWindowText(sTemp);
	//dRet = atof((LPCTSTR)sTemp);
	dRet = _wtof(sTemp);
	return dRet;
}

void CMotionGUIDlg::SetDlgItemDouble(int nID, double value, int nPoint)
{
	CString sData;
	CString sTemp1, sTemp2;
	sTemp2.Format(_T("%dlf"), nPoint);
	sTemp1 = _T("%.") + sTemp2;
	sData.Format(sTemp1, value);
	GetDlgItem(nID)->SetWindowTextW(sData);
}

void CMotionGUIDlg::WriteLog1(LPCTSTR lpszString, CListBox &m_lsb)
{
	CString strLog;
	CString strTime;
	strTime = CTime::GetCurrentTime().Format("%H:%M:%S");
	strLog = _T("") + strTime + _T(": ") + lpszString;
	m_lsb.InsertString(0, strLog);
}

void CMotionGUIDlg::WriteLog2(LPCTSTR lpszString)
{
	CListBox* m_lsb;
	m_lsb = &m_lsbLog;	//Change the listbox here

	CString strLog;
	CString strTime;
	strTime = CTime::GetCurrentTime().Format("%H:%M:%S");
	strLog = _T("") + strTime + _T(": ") + lpszString;
	m_lsb->InsertString(0, strLog);
}

void CMotionGUIDlg::WriteLog(LPCTSTR lpszString, int nID)
{
	CListBox* pList = (CListBox*)GetDlgItem(nID);
	CString strLog;
	CString strTime;
	strTime = CTime::GetCurrentTime().Format("%H:%M:%S");
	strLog = _T("") + strTime + _T(": ") + lpszString;
	if (false){
		pList->InsertString(0, strLog);					//Write at the first line
	}
	else{
		pList->InsertString(pList->GetCount(), strLog);	//Write at the last line
		//display most recent event logs
		pList->SetTopIndex(pList->GetCount()-1);	//or pList->SetCurSel(pList->GetCount() - 1);	
	}
	EnableListboxHorizontalScrollbar();
}

void CMotionGUIDlg::DisplayLocation(double PosX, double PosY, int nID)
{
	CStatic* pStatic = (CStatic*)GetDlgItem(nID);
	CString sTemp[2];
	sTemp[0].Format(_T("X: %.2lf"), PosX);
	sTemp[1].Format(_T("Y: %.2lf"), PosY);
	pStatic->SetWindowTextW(sTemp[0] + _T("\n") + sTemp[1]);
}

void CMotionGUIDlg::EnableListboxHorizontalScrollbar(int nID){
	CString str;
	CSize sz;
	CListBox* pList = (CListBox*)GetDlgItem(nID);
	int dx = 0;
	CDC* pDC = pList->GetDC();
	for (int i = 0; i < pList->GetCount(); i++)
	{
		pList->GetText(i, str);
		sz = pDC->GetTextExtent(str);
		if (sz.cx > dx)
			dx = sz.cx;
	}
	pList->ReleaseDC(pDC);

	if (pList->GetHorizontalExtent() < dx)
	{
		pList->SetHorizontalExtent(dx);
		ASSERT(pList->GetHorizontalExtent() == dx);
	}
}


void CMotionGUIDlg::InitLibrary()
{
	CString sTemp;
	DWORD Code;
	int iRet = 0;
	do {
		Code = AxlOpen(7);
		if (Code == AXT_RT_SUCCESS)
		{
			sTemp = _T("Library is initialized");
			WriteLog(sTemp);
			m_chkSimulate.SetCheck(0);
		}
		else
		{
			sTemp.Format(_T("Library Open Error. \nError code 0x%x\n"), Code);
			iRet = AfxMessageBox(sTemp, MB_RETRYCANCEL | MB_ICONSTOP);
			WriteLog(sTemp);
			m_chkSimulate.SetCheck(1);

		}
	} while (iRet == IDRETRY);

	//*implement: check wheter axis exists
}

void CMotionGUIDlg::InitControl()
{

	//---------------------Setting begin--------------------------------------------------------------------------
	{
	//cbo init: axis no
	long lAxisCounts = NUMBER_OF_AXES;
	CString sOption;
	m_cboAxisSet.ResetContent();	//Avoid adding options again
	for (int i = 0; i < lAxisCounts; i++){
		sOption.Format(_T("Axis No.%ld"), i);
		m_cboAxisSet.InsertString(i, sOption);
	}
	m_cboAxisSet.SetCurSel(0);		//default option

	//cbo init: pulse out method
	m_cboPulseOutMethod.ResetContent();
	m_cboPulseOutMethod.InsertString(0, _T("00: OneHighLowHigh"));
	m_cboPulseOutMethod.InsertString(1, _T("01: OneHighHighLow"));
	m_cboPulseOutMethod.InsertString(2, _T("02: OneLowLowHigh"));
	m_cboPulseOutMethod.InsertString(3, _T("03: OneLowHighLow"));
	m_cboPulseOutMethod.InsertString(4, _T("04: TwoCcwCwHigh"));
	m_cboPulseOutMethod.InsertString(5, _T("05: TwoCcWCwLow"));
	m_cboPulseOutMethod.InsertString(6, _T("06: TwoCwCcwHigh"));
	m_cboPulseOutMethod.InsertString(7, _T("07: TwoCwCcwLow"));
	m_cboPulseOutMethod.InsertString(8, _T("08: TwoPhase"));
	m_cboPulseOutMethod.InsertString(9, _T("09: TwoPhaseReverse"));
	m_cboPulseOutMethod.InsertString(10, _T("10: NOT USE"));
	m_cboPulseOutMethod.SetCurSel(10);

	//cbo init: encoder input method	
	m_cboEncoderInputMethod.ResetContent();
	m_cboEncoderInputMethod.InsertString(0, _T("00: ObverseUpDownMode"));
	m_cboEncoderInputMethod.InsertString(1, _T("01: ObverseSqr1Mode"));
	m_cboEncoderInputMethod.InsertString(2, _T("02: ObverseSqr2Mode"));
	m_cboEncoderInputMethod.InsertString(3, _T("03: ObverseSqr4Mode"));
	m_cboEncoderInputMethod.InsertString(4, _T("04: ReverseUpDownMode"));
	m_cboEncoderInputMethod.InsertString(5, _T("05: ReverseSqr1Mode"));
	m_cboEncoderInputMethod.InsertString(6, _T("06: ReverseSqr2Mode"));
	m_cboEncoderInputMethod.InsertString(7, _T("07: ReverseSqr4Mode"));
	m_cboEncoderInputMethod.SetCurSel(3);

	//cbo init: velocity profile	
	m_cboVelProfile.ResetContent();
	m_cboVelProfile.InsertString(0, _T("00: SYM_TRAPEZOIDE_MODE"));
	m_cboVelProfile.InsertString(1, _T("01: ASYM_TRAPEZOIDE_MODE"));
	m_cboVelProfile.InsertString(2, _T("02: QUASI_S_CURVE_MODE"));
	m_cboVelProfile.InsertString(3, _T("03: SYM_S_CURVE_MODE"));
	m_cboVelProfile.InsertString(4, _T("04: ASYM_SCURVE_MODE"));
	m_cboVelProfile.SetCurSel(1);

	//cbo init: absolute/relative mode
	m_cboAbsRelMode.ResetContent();
	m_cboAbsRelMode.InsertString(0, _T("00: Absolute"));
	m_cboAbsRelMode.InsertString(1, _T("01: Relative"));
	m_cboAbsRelMode.SetCurSel(1);

	//edt init
	SetDlgItemDouble(IDC_EDIT_VELOCITY_MAX, 50);
	SetDlgItemDouble(IDC_EDIT_VELOCITY_MIN, 1);
	SetDlgItemDouble(IDC_EDIT_UNIT, 10);
	SetDlgItemDouble(IDC_EDIT_PULSE, 131072);
	}
	//---------------------Setting end--------------------------------------------------------------------------


	//---------------------Single Axis begin--------------------------------------------------------------------------
	m_rdoSingleAxis0.SetCheck(1);

	SetDlgItemDouble(IDC_EDIT_SINGLE_POS,	200);
	SetDlgItemDouble(IDC_EDIT_SINGLE_VEL,	20);
	SetDlgItemDouble(IDC_EDIT_SINGLE_ACCEL, 10);
	SetDlgItemDouble(IDC_EDIT_SINGLE_DECEL,	10);
	
	m_chkSingleSynAsyn.SetCheck(1);	//asyn
	//---------------------Single Axis end--------------------------------------------------------------------------


	//---------------------Single Axis Override begin--------------------------------------------------------------------------
	m_rdoOvrdPos.SetCheck(1);
	m_chkOvrdTarget.SetCheck(0);	//cmd target

	SetDlgItemDouble(IDC_EDIT_OVRD_MAX_VEL, 50);
	SetDlgItemDouble(IDC_EDIT_OVRD_POS, 500);
	SetDlgItemDouble(IDC_EDIT_OVRD_VEL, 30);
	//---------------------Single Axis Override end--------------------------------------------------------------------------


	//---------------------Multi Axis begin--------------------------------------------------------------------------
	SetDlgItemDouble(IDC_EDIT_MULTI_POS_0, 50);
	SetDlgItemDouble(IDC_EDIT_MULTI_VEL_0, 20);
	SetDlgItemDouble(IDC_EDIT_MULTI_ACCEL_0, 20);
	SetDlgItemDouble(IDC_EDIT_MULTI_DECEL_0, 20);
	SetDlgItemDouble(IDC_EDIT_MULTI_POS_1, 100);
	SetDlgItemDouble(IDC_EDIT_MULTI_VEL_1, 15);
	SetDlgItemDouble(IDC_EDIT_MULTI_ACCEL_1, 15);
	SetDlgItemDouble(IDC_EDIT_MULTI_DECEL_1, 15);
	//---------------------Multi Axis end--------------------------------------------------------------------------


	//---------------------Interpolation begin--------------------------------------------------------------------------
	SetDlgItemDouble(IDC_EDIT_INTRPL_COORDINATE, 0, 0);
	SetDlgItemDouble(IDC_EDIT_INTRPL_AXIS_X, AXIS_X, 0);
	SetDlgItemDouble(IDC_EDIT_INTRPL_AXIS_Y, AXIS_Y, 0);

	//m_rdoIntrplAbs.SetCheck(1);
	CheckRadioButton(IDC_RADIO_INTRPL_ABS, IDC_RADIO_INTRPL_REL, IDC_RADIO_INTRPL_REL);

	SetDlgItemDouble(IDC_EDIT_INTRPL_VEL, 10);
	SetDlgItemDouble(IDC_EDIT_INTRPL_ACCEL, 10);
	SetDlgItemDouble(IDC_EDIT_INTRPL_DECEL, 10);

	SetDlgItemDouble(IDC_EDIT_INTRPL_STRLINE_PX, 100);
	SetDlgItemDouble(IDC_EDIT_INTRPL_STRLINE_PY, 50);

	SetDlgItemDouble(IDC_EDIT_INTRPL_CIRCENTER_EX, 0);
	SetDlgItemDouble(IDC_EDIT_INTRPL_CIRCENTER_EY, -200);
	SetDlgItemDouble(IDC_EDIT_INTRPL_CIRCENTER_CX, 0);
	SetDlgItemDouble(IDC_EDIT_INTRPL_CIRCENTER_CY, -100);
	m_chkIntrplCirCenterDir.SetCheck(0);

	SetDlgItemDouble(IDC_EDIT_INTRPL_CIRRADIUS_EX, 300);
	SetDlgItemDouble(IDC_EDIT_INTRPL_CIRRADIUS_EY, 200);
	SetDlgItemDouble(IDC_EDIT_INTRPL_CIRRADIUS_RADIUS, 120);
	m_chkIntrplCirRadiusDir.SetCheck(0);
	m_chkIntrplCirRadiusDistance.SetCheck(0);

	SetDlgItemDouble(IDC_EDIT_INTRPL_CIRANGLE_CX, 200);
	SetDlgItemDouble(IDC_EDIT_INTRPL_CIRANGLE_CY, 200);
	SetDlgItemDouble(IDC_EDIT_INTRPL_CIRANGLE_ANGLE, 45);
	m_chkIntrplCirAngleDir.SetCheck(0);

	SetDlgItemDouble(IDC_EDIT_INTRPL_CIRPOINT_EX, 150);
	SetDlgItemDouble(IDC_EDIT_INTRPL_CIRPOINT_EY, 150);
	SetDlgItemDouble(IDC_EDIT_INTRPL_CIRPOINT_MX, 100);
	SetDlgItemDouble(IDC_EDIT_INTRPL_CIRPOINT_MY, 50);
	m_chkIntrplCirPointArcCircle.SetCheck(0);
	//---------------------Interpolation end--------------------------------------------------------------------------


	//---------------------Axis Monitor begin--------------------------------------------------------------------------
	m_rdoMonitorAxis0.SetCheck(1);
	m_chkServoOn.SetCheck(0);
	m_chkAlarm.SetCheck(0);
	//---------------------Axis Monitor end--------------------------------------------------------------------------

	//---------------------Chart Setting begin--------------------------------------------------------------------------
	SetDlgItemDouble(IDC_EDIT_PATH_RANGE_MIN_X, -CHART_PATH_RANGE);
	SetDlgItemDouble(IDC_EDIT_PATH_RANGE_MAX_X, CHART_PATH_RANGE);
	SetDlgItemDouble(IDC_EDIT_PATH_RANGE_MIN_Y, -CHART_PATH_RANGE);
	SetDlgItemDouble(IDC_EDIT_PATH_RANGE_MAX_Y, CHART_PATH_RANGE);

	SetDlgItemDouble(IDC_EDIT_CHART_OBSERVATION_TIME, dObsTime,0);
	m_chkChartLegend.SetCheck(0);
	//---------------------Chart Setting end--------------------------------------------------------------------------


	//---------------------Chart begin--------------------------------------------------------------------------
	//Chart: Path Planning
	//CChartCtrl ref;
	//ref.RemoveAllSeries();
	CChartStandardAxis* m_pAxis;
	CChartAxisLabel* m_pAxisLabel;
	m_pAxis = m_ChartPath.CreateStandardAxis(CChartCtrl::LeftAxis);
	m_pAxis->SetMinMax(-CHART_PATH_RANGE, CHART_PATH_RANGE);
	m_pAxisLabel = m_pAxis->GetLabel();
	m_pAxisLabel->SetText(_T("Y (mm)"));
	m_pAxis = m_ChartPath.CreateStandardAxis(CChartCtrl::BottomAxis);
	m_pAxis->SetMinMax(-CHART_PATH_RANGE, CHART_PATH_RANGE);
	m_pAxisLabel = m_pAxis->GetLabel();
	m_pAxisLabel->SetText(_T("X (mm)"));	//m_pAxis->GetLabel()->SetText(_T("X (mm)"));	
	m_ChartPath.GetTitle()->AddString(_T("Path Planning (Act Pos)"));
	m_ChartPath.GetTitle()->SetColor(RGB(0, 0, 255));
	m_pChartPathPointSerie = m_ChartPath.CreatePointsSerie();
	m_pChartPathPointSerie->ClearSerie();
	m_pChartPathPointSerie->SetPointSize(CHART_POINT_SIZE, CHART_POINT_SIZE);
	m_pChartPathPointSerie->SetBorderColor(RGB(255, 0, 0));	//point border color

	//Chart: Cmd Pos
	//CChartStandardAxis* m_pAxis;
	//CChartAxisLabel* m_pAxisLabel;
	m_pAxis = m_ChartPos.CreateStandardAxis(CChartCtrl::LeftAxis);
	m_pAxis->SetAutomaticMode(CChartAxis::ScreenAutomatic);
	m_pAxisLabel = m_pAxis->GetLabel();
	m_pAxisLabel->SetText(_T("Position (mm)"));
	m_pAxis = m_ChartPos.CreateStandardAxis(CChartCtrl::BottomAxis);
	m_pAxis->SetAutomaticMode(CChartAxis::FullAutomatic);
	m_pAxisLabel = m_pAxis->GetLabel();
	m_pAxisLabel->SetText(_T("Time (s)"));
	//m_pAxis->SetTickIncrement(true, 5);
	//m_pAxis->SetDiscrete(false);
	m_ChartPos.GetTitle()->AddString(_T("Axes Position (Cmd Pos)"));
	m_ChartPos.GetTitle()->SetColor(RGB(0, 0, 255));
	m_pChartPos0LineSerie = m_ChartPos.CreateLineSerie();
	m_pChartPos0LineSerie->ClearSerie();
	m_pChartPos0LineSerie->SetWidth(3); //line width
	m_pChartPos0LineSerie->SetColor(RGB(0, 0, 255)); //color of line
	m_pChartPos0LineSerie->SetName(_T("Axis 0"));
	m_pChartPos1LineSerie = m_ChartPos.CreateLineSerie();
	m_pChartPos1LineSerie->ClearSerie();
	m_pChartPos1LineSerie->SetWidth(3); //line width
	m_pChartPos1LineSerie->SetColor(RGB(255, 0, 0)); //color of line
	m_pChartPos1LineSerie->SetName(_T("Axis 1"));

	//Chart: Cmd Vel
	//CChartStandardAxis* m_pAxis;
	//CChartAxisLabel* m_pAxisLabel;
	m_pAxis = m_ChartVel.CreateStandardAxis(CChartCtrl::LeftAxis);
	//m_pAxis->SetAutomaticMode(CChartAxis::ScreenAutomatic);
	m_pAxis->SetMinMax(-5, GetDlgItemDouble(IDC_EDIT_VELOCITY_MAX));
	m_pAxisLabel = m_pAxis->GetLabel();
	m_pAxisLabel->SetText(_T("Velocity (mm/s)"));
	m_pAxis = m_ChartVel.CreateStandardAxis(CChartCtrl::BottomAxis);
	m_pAxis->SetAutomaticMode(CChartAxis::FullAutomatic);
	m_pAxisLabel = m_pAxis->GetLabel();
	m_pAxisLabel->SetText(_T("Time (s)"));
	//m_pAxis->SetTickIncrement(true, 5);
	//m_pAxis->SetDiscrete(true);
	m_ChartVel.GetTitle()->AddString(_T("Axes Vel (Cmd Vel)"));
	m_ChartVel.GetTitle()->SetColor(RGB(0, 0, 255));
	m_pChartVel0LineSerie = m_ChartVel.CreateLineSerie();
	m_pChartVel0LineSerie->ClearSerie();
	m_pChartVel0LineSerie->SetWidth(3); //line width
	m_pChartVel0LineSerie->SetColor(RGB(0, 0, 255)); //color of line
	m_pChartVel0LineSerie->SetName(_T("Axis 0"));

	m_pChartVel1LineSerie = m_ChartVel.CreateLineSerie();
	m_pChartVel1LineSerie->ClearSerie();
	m_pChartVel1LineSerie->SetWidth(3); //line width
	m_pChartVel1LineSerie->SetColor(RGB(255, 0, 0)); //color of line	
	m_pChartVel1LineSerie->SetName(_T("Axis 1"));
	//---------------------Chart end--------------------------------------------------------------------------

}

void CMotionGUIDlg::InitMotionSignal(){
	//*implementation done
	long lAxisCounts = NUMBER_OF_AXES;
	for (int i = 0; i < lAxisCounts; i++){
		SetAxisHome(i);			//set home

		AxmSignalSetLimit(i, SLOWDOWN_STOP, UNUSED, UNUSED);	//Setup active level value of End Limit.
		AxmSignalSetInpos(i, UNUSED);							// Setup active level value of in-position signal.
		AxmSignalSetServoAlarm(i, HIGH);					// Setup active level value of alarm signal.
		AxmSignalSetStop(i, SLOWDOWN_STOP, UNUSED);		//Setup active level value of Emg signal.
	}
	//WriteLog(_T("Set Home all axes"));
	WriteLog(_T("Disabled SetLimit, SetInpos, SetStop of all axes."));
	WriteLog(_T("SetServoAlarm high level of all axes."));
}

void CMotionGUIDlg::ApplySettingAxis(long lAxisNo)
{
	//*implementation done
	//assign value from control to axis
	
	int iPulseOutMethod = m_cboPulseOutMethod.GetCurSel();
	if (iPulseOutMethod != 10){	// [10]: NOT USED
		AxmMotSetPulseOutMethod(lAxisNo, iPulseOutMethod);
	}
	int iEncInputMethod = m_cboEncoderInputMethod.GetCurSel();
	AxmMotSetEncInputMethod(lAxisNo, iEncInputMethod);
	int iVelProfile = m_cboVelProfile.GetCurSel();
	AxmMotSetProfileMode(lAxisNo, iVelProfile);
	int iAbsRelMode = m_cboAbsRelMode.GetCurSel();
	AxmMotSetAbsRelMode(lAxisNo, iAbsRelMode);
	double dMaxVel = GetDlgItemDouble(IDC_EDIT_VELOCITY_MAX);
	AxmMotSetMaxVel(lAxisNo, dMaxVel);
	double dMinVel = GetDlgItemDouble(IDC_EDIT_VELOCITY_MIN);
	AxmMotSetMinVel(lAxisNo, dMinVel);
	double dUnit = GetDlgItemDouble(IDC_EDIT_UNIT);
	long lPulse = long(GetDlgItemDouble(IDC_EDIT_PULSE));
	AxmMotSetMoveUnitPerPulse(lAxisNo, dUnit, lPulse);

	CString sLog;
	sLog.Format(_T("Applied setting for axis no.%ld"), lAxisNo);
	WriteLog(sLog);
}

void CMotionGUIDlg::ApplySettingAxisAll()
{
	long lAxisCounts = NUMBER_OF_AXES;
	for (int i = 0; i < lAxisCounts; i++){
		ApplySettingAxis(i);
	}

	CString sLog;
	sLog.Format(_T("Applied setting for all axes"));
	WriteLog(sLog);
}

void CMotionGUIDlg::UpdateAxisInfo(long lAxisNo)
{
	//*implementation done
	const DWORD RANDOM_INIT_VALUE = 100;
	DWORD uPulseOutMethod = RANDOM_INIT_VALUE;
	DWORD uEncInputMethod = RANDOM_INIT_VALUE;
	DWORD uVelProfile =		RANDOM_INIT_VALUE;
	DWORD uAbsRelMode =		RANDOM_INIT_VALUE;
	double dMaxVel;
	double dMinVel;
	double dUnit;
	long lPulse;

	if (!m_chkSimulate.GetCheck()){
		//PulseOutMethod API does not support in this board library	
		AxmMotSetEncInputMethod(lAxisNo, uEncInputMethod);
		m_cboEncoderInputMethod.SetCurSel(uEncInputMethod);
		AxmMotGetProfileMode(lAxisNo, &uVelProfile);
		m_cboVelProfile.SetCurSel(uVelProfile);
		AxmMotGetAbsRelMode(lAxisNo, &uAbsRelMode);
		m_cboAbsRelMode.SetCurSel(uAbsRelMode);
		AxmMotGetMaxVel(lAxisNo, &dMaxVel);
		SetDlgItemDouble(IDC_EDIT_VELOCITY_MAX, dMaxVel);
		
		AxmMotGetMinVel(lAxisNo, &dMinVel);
		SetDlgItemDouble(IDC_EDIT_VELOCITY_MIN, dMinVel);
		AxmMotGetMoveUnitPerPulse(lAxisNo, &dUnit, &lPulse);
		SetDlgItemDouble(IDC_EDIT_UNIT, dUnit);
		SetDlgItemDouble(IDC_EDIT_PULSE, (double)lPulse, 0);
		SetDlgItemDouble(IDC_EDIT_UNIT_PER_PULSE, dUnit /(long)lPulse, 8);
	}
	else{
		//-----test only begin-----
		m_cboVelProfile.SetCurSel(2);
		SetDlgItemDouble(IDC_EDIT_VELOCITY_MAX, 8888);
		//-----test only end-----

		GetDlgItem(IDC_EDIT_UNIT_PER_PULSE)->SetWindowTextW(_T("Test"));
	}

	CString sLog;
	sLog.Format(_T("Axis %ld Info Updated."), lAxisNo);
	WriteLog(sLog);
}

void CMotionGUIDlg::ChartDraw(){
	//path planning
	static double m_dLastActPos0 = 0;
	static double m_dLastActPos1 = 0;
	double m_dActPos0, m_dActPos1;
	if (!m_chkSimulate.GetCheck()){
		AxmStatusGetActPos(0, &m_dActPos0);
		AxmStatusGetActPos(1, &m_dActPos1);
	}
	else{
		m_dActPos0 = m_sliValue.GetPos();
		m_dActPos1 = m_sliValue.GetPos();
	}

	if ((m_dActPos0 != m_dLastActPos0) || (m_dActPos1 != m_dLastActPos1)){	//if position is changed
		m_pChartPathPointSerie->AddPoint(m_dActPos0, m_dActPos1);
		m_dLastActPos0 = m_dActPos0;
		m_dLastActPos1 = m_dActPos1;

		double dRangeMin[2], dRangeMax[2];	//[0]: bottom axis, [1]: left axis
		m_ChartPath.GetBottomAxis()->GetMinMax(dRangeMin[0], dRangeMax[0]);
		m_ChartPath.GetLeftAxis()->GetMinMax(dRangeMin[1], dRangeMax[1]);
		m_ChartPath.RefreshCtrl();	//display new point
		//RefreshCtrl will change the range, so chart range is needed to be set again
		m_ChartPath.GetBottomAxis()->SetMinMax(dRangeMin[0], dRangeMax[0]);
		m_ChartPath.GetLeftAxis()->SetMinMax(dRangeMin[1], dRangeMax[1]);
		DisplayLocation(m_dActPos0, m_dActPos1);
	}

	//*implement: writelog to notify whether point is outside of range

	double dTickIncrement = TICK_INCREMENT;
	static double dTime = -dTickIncrement;
	lPlotCount++;
	dTime += dTickIncrement;
	double m_dCmdPos0, m_dCmdPos1;
	if (!m_chkSimulate.GetCheck()){
		AxmStatusGetCmdPos(0, &m_dCmdPos0);
		AxmStatusGetCmdPos(1, &m_dCmdPos1);
	}
	else{
		m_dCmdPos0 = m_sliValue.GetPos();
		m_dCmdPos1 = abs(m_sliValue.GetPos()) - 50;
	}
	//PLOT_COUNTS = (Total time plot) / (Timer)
	if (lPlotCount >= PLOT_COUNTS(dObsTime)){
		m_pChartPos0LineSerie->RemovePointsFromBegin(1);
		m_pChartPos1LineSerie->RemovePointsFromBegin(1);
	}
	m_pChartPos0LineSerie->AddPoint(dTime / 1000, m_dCmdPos0);
	m_pChartPos1LineSerie->AddPoint(dTime / 1000, m_dCmdPos1);

	/*double dTickIncrement = TICK_INCREMENT;
	static long lPlotCount = 0;
	static double dTime = -dTickIncrement;
	lPlotCount++;
	dTime += dTickIncrement;*/
	double m_dCmdVel0, m_dCmdVel1;
	if (!m_chkSimulate.GetCheck()){
		AxmStatusReadVel(0, &m_dCmdVel0);
		AxmStatusReadVel(1, &m_dCmdVel1);
	}
	else{
		m_dCmdVel0 = m_sliValue.GetPos();
		m_dCmdVel1 = abs(m_sliValue.GetPos()) - 50;
	}
	if (lPlotCount >= PLOT_COUNTS(dObsTime)){
		m_pChartVel0LineSerie->RemovePointsFromBegin(1);
		m_pChartVel1LineSerie->RemovePointsFromBegin(1);
	}
	m_pChartVel0LineSerie->AddPoint(dTime / 1000, m_dCmdVel0);
	m_pChartVel1LineSerie->AddPoint(dTime / 1000, m_dCmdVel1);
}




void CMotionGUIDlg::OnCbnSelchangeComboAxisset()
{
	UpdateAxisInfo(m_cboAxisSet.GetCurSel());
}


void CMotionGUIDlg::OnBnClickedButtonSettingAxis()
{
	ApplySettingAxis(m_cboAxisSet.GetCurSel());
	UpdateAxisInfo(m_cboAxisSet.GetCurSel());
}


void CMotionGUIDlg::OnBnClickedButtonSettingAxisAll()
{
	ApplySettingAxisAll();
	UpdateAxisInfo(m_cboAxisSet.GetCurSel());

}

void CMotionGUIDlg::OnBnClickedButtonClearPath()
{
	m_pChartPathPointSerie->ClearSerie();
}

void CMotionGUIDlg::OnBnClickedButtonAddPoint()
{
	double m_dActPos0, m_dActPos1;
	m_dActPos0 = GetDlgItemDouble(IDC_EDIT_PATH_POS_X);
	m_dActPos1 = GetDlgItemDouble(IDC_EDIT_PATH_POS_Y);
	m_pChartPathPointSerie->AddPoint(m_dActPos0, m_dActPos1);
	m_ChartPath.RefreshCtrl();	//display new point
}

void CMotionGUIDlg::OnEnKillfocusEditPathRangeMinX()
{
	double dRangeMin, dRangeMax;
	m_ChartPath.GetBottomAxis()->GetMinMax(dRangeMin, dRangeMax);
	dRangeMin = GetDlgItemDouble(IDC_EDIT_PATH_RANGE_MIN_X);
	if (dRangeMax > dRangeMin){
		m_ChartPath.GetBottomAxis()->SetMinMax(dRangeMin, dRangeMax);
	}
	else{
		CString sTemp;
		sTemp.Format(_T("MAX range must be bigger than MIN range"));
		AfxMessageBox(sTemp, MB_ICONINFORMATION);
		m_ChartPath.GetBottomAxis()->GetMinMax(dRangeMin, dRangeMax);
		SetDlgItemDouble(IDC_EDIT_PATH_RANGE_MIN_X, dRangeMin);	//Assign previous value to edit box
	}
	return;
}

void CMotionGUIDlg::OnEnKillfocusEditPathRangeMinY()
{
	double dRangeMin, dRangeMax;
	m_ChartPath.GetLeftAxis()->GetMinMax(dRangeMin, dRangeMax);
	dRangeMin = GetDlgItemDouble(IDC_EDIT_PATH_RANGE_MIN_Y);
	if (dRangeMax > dRangeMin){
		m_ChartPath.GetLeftAxis()->SetMinMax(dRangeMin, dRangeMax);
	}
	else{
		CString sTemp;
		sTemp.Format(_T("MAX range must be bigger than MIN range"));
		AfxMessageBox(sTemp, MB_ICONINFORMATION);
		m_ChartPath.GetLeftAxis()->GetMinMax(dRangeMin, dRangeMax);
		SetDlgItemDouble(IDC_EDIT_PATH_RANGE_MIN_Y, dRangeMin);	//Assign previous value to edit box
	}
}

void CMotionGUIDlg::OnEnKillfocusEditPathRangeMaxX()
{
	double dRangeMin, dRangeMax;
	m_ChartPath.GetBottomAxis()->GetMinMax(dRangeMin, dRangeMax);
	dRangeMax = GetDlgItemDouble(IDC_EDIT_PATH_RANGE_MAX_X);
	if (dRangeMax > dRangeMin){
		m_ChartPath.GetBottomAxis()->SetMinMax(dRangeMin, dRangeMax);
	}
	else{
		CString sTemp;
		sTemp.Format(_T("MAX range must be bigger than MIN range"));
		AfxMessageBox(sTemp, MB_ICONINFORMATION);
		m_ChartPath.GetBottomAxis()->GetMinMax(dRangeMin, dRangeMax);
		SetDlgItemDouble(IDC_EDIT_PATH_RANGE_MAX_X, dRangeMax);	//Assign previous value to edit box
	}
}

void CMotionGUIDlg::OnEnKillfocusEditPathRangeMaxY()
{
	double dRangeMin, dRangeMax;
	m_ChartPath.GetLeftAxis()->GetMinMax(dRangeMin, dRangeMax);
	dRangeMax = GetDlgItemDouble(IDC_EDIT_PATH_RANGE_MAX_Y);
	if (dRangeMax > dRangeMin){
		m_ChartPath.GetLeftAxis()->SetMinMax(dRangeMin, dRangeMax);
	}
	else{
		CString sTemp;
		sTemp.Format(_T("MAX range must be bigger than MIN range"));
		AfxMessageBox(sTemp, MB_ICONINFORMATION);
		m_ChartPath.GetLeftAxis()->GetMinMax(dRangeMin, dRangeMax);
		SetDlgItemDouble(IDC_EDIT_PATH_RANGE_MAX_Y, dRangeMax);	//Assign previous value to edit box
	}
}

void CMotionGUIDlg::OnBnClickedButtonChartObservationTime()
{
	if (GetDlgItemDouble(IDC_EDIT_CHART_OBSERVATION_TIME) > 0){
		if (GetDlgItemDouble(IDC_EDIT_CHART_OBSERVATION_TIME) != dObsTime){
			lPlotCount = 0;
			dObsTime = GetDlgItemDouble(IDC_EDIT_CHART_OBSERVATION_TIME);
			m_pChartPos0LineSerie->ClearSerie();
			m_pChartPos1LineSerie->ClearSerie();
			m_pChartVel0LineSerie->ClearSerie();
			m_pChartVel1LineSerie->ClearSerie();
		}
	}
	else{
		CString sTemp;
		sTemp.Format(_T("Observation time must be bigger than 0"));
		AfxMessageBox(sTemp, MB_ICONINFORMATION);
	}
}



void CMotionGUIDlg::OnBnClickedCheckChartLegend()
{
	if (m_chkChartLegend.GetCheck()){
		m_ChartPos.GetLegend()->SetVisible(true);
		m_ChartVel.GetLegend()->SetVisible(true);
	}
	else{
		m_ChartPos.GetLegend()->SetVisible(false);
		m_ChartVel.GetLegend()->SetVisible(false);
	}
}


void CMotionGUIDlg::OnBnClickedButtonSingleMove()
{
	UpdateData(true);
	long lAxisNo = m_rdoSingleAxis;
	double dPos = GetDlgItemDouble(IDC_EDIT_SINGLE_POS);
	double dVel = GetDlgItemDouble(IDC_EDIT_SINGLE_VEL);
	double dAcc = GetDlgItemDouble(IDC_EDIT_SINGLE_ACCEL);
	double dDec = GetDlgItemDouble(IDC_EDIT_SINGLE_DECEL);
	if (m_chkSingleSynAsyn.GetCheck() == 0)		AxmMoveStartPos(lAxisNo, dPos, dVel, dAcc, dAcc);
	else										AxmMoveStartPos(lAxisNo, dPos, dVel, dAcc, dDec);

	/*CString sLog[5];
	sLog[1].Format(_T("%.1lf"), dPos);
	sLog[2].Format(_T("%.1lf"), dVel);
	sLog[3].Format(_T("%.1lf"), dAcc);
	sLog[4].Format(_T("%.1lf"), dDec);
	sLog[0] = _T("AxmMoveStartPos(") + sLog[1] + _T(", ") + sLog[2] + _T(", ") + sLog[3] + _T(", ") + sLog[4] + _T(")");
	WriteLog(sLog[0]);*/
	CString sLog;
	sLog.Format(_T("AxmMoveStartPos axis %ld"), lAxisNo);
	WriteLog(sLog);
}

void CMotionGUIDlg::OnBnClickedButtonSingleStop()
{
	long lAxisNo = m_rdoSingleAxis;
	AxmMoveSStop(lAxisNo);

	xThreadRepeatStop = 1;

	CString sLog;
	sLog.Format(_T("Stop axis %ld"), lAxisNo);
	WriteLog(sLog);
}


void CMotionGUIDlg::OnBnClickedButtonSingleEstop()
{
	long lAxisNo = m_rdoSingleAxis;
	AxmMoveEStop(lAxisNo);

	xThreadRepeatStop = 1;

	CString sLog;
	sLog.Format(_T("Emergency Stop axis %ld"), lAxisNo);
	WriteLog(sLog);
}


void CMotionGUIDlg::OnBnClickedButtonMultiMove()
{
	long lSize = 2;
	long lAxis[2] = { 0, 1 };
	double dPos[2];
	double dVel[2];
	double dAcc[2];
	double dDec[2];


	dPos[0] = GetDlgItemDouble(IDC_EDIT_MULTI_POS_0);
	dVel[0] = GetDlgItemDouble(IDC_EDIT_MULTI_VEL_0);
	dAcc[0] = GetDlgItemDouble(IDC_EDIT_MULTI_ACCEL_0);
	dDec[0] = GetDlgItemDouble(IDC_EDIT_MULTI_DECEL_0);

	dPos[1] = GetDlgItemDouble(IDC_EDIT_MULTI_POS_1);
	dVel[1] = GetDlgItemDouble(IDC_EDIT_MULTI_VEL_1);
	dAcc[1] = GetDlgItemDouble(IDC_EDIT_MULTI_ACCEL_1);
	dDec[1] = GetDlgItemDouble(IDC_EDIT_MULTI_DECEL_1);

	AxmMoveStartMultiPos(lSize, lAxis, dPos, dVel, dAcc, dDec); 

	WriteLog(_T("AxmMoveStartMultiPos"));
}




void CMotionGUIDlg::OnBnClickedButtonMultiStop()
{
	AxmMoveSStop(0);
	AxmMoveSStop(1);

	xThreadRepeatStop = true;

	WriteLog(_T("Stop axis 0 and axis 1"));
}


void CMotionGUIDlg::OnBnClickedButtonMultiEstop()
{
	AxmMoveEStop(0);
	AxmMoveEStop(1);

	xThreadRepeatStop = true;

	WriteLog(_T("Emergency Stop axis 0 and axis 1"));
}


void CMotionGUIDlg::OnBnClickedButtonMonitorClear()
{
	UpdateData(true);
	long lAxisNo = m_rdoMonitorAxis;
	if (!m_chkSimulate.GetCheck())	SetAxisHome(lAxisNo);
	else							m_sliValue.SetPos(0);
}

void CMotionGUIDlg::SetAxisHome(long lAxisNo){
	AxmStatusSetActPos(lAxisNo, 0.0);
	AxmStatusSetCmdPos(lAxisNo, 0.0);

	CString sLog;
	sLog.Format(_T("Set axis %ld home"), lAxisNo);
	WriteLog(sLog);
}


void CMotionGUIDlg::OnBnClickedButtonMonitorClearAll()
{
	long lAxisCounts = NUMBER_OF_AXES;
	if (!m_chkSimulate.GetCheck()){
		for (int i = 0; i < lAxisCounts; i++){
			SetAxisHome(i);
		}
	}
	else
	{
		m_sliValue.SetPos(0);
	}
}


void CMotionGUIDlg::OnBnClickedCheckServoOn()
{
	//UpdateData(true);		//*implement: temp
	//long lAxisNo = m_rdoMonitorAxis;
	//if (!m_chkServoOn.GetCheck()){		//servo is being disable
	//	AxmSignalServoOn(lAxisNo, 1);	//turn on
	//}
	//else {								//servo is on
	//	AxmSignalServoOn(lAxisNo, 0);	//turn off
	//}
}


void CMotionGUIDlg::OnBnClickedCheckAlarm()
{
	//UpdateData(true);		//*implement: temp
	//long lAxisNo = m_rdoMonitorAxis;
	//if (m_chkAlarm.GetCheck()){		//if alarm exists
	//	AxmSignalServoAlarmReset(lAxisNo, 1);	//reset
	//}
	//else {							//alarm does not exist
	//	AxmSignalServoAlarmReset(lAxisNo, 0);	//*implement: check if it works
	//}
}


void CMotionGUIDlg::OnBnClickedButtonOvrdOverride()
{
	UpdateData(true);
	long lAxisNo = m_rdoSingleAxis;
	int iOvrdMode = m_rdoOverride;
	double dOvrdMaxVel = GetDlgItemDouble(IDC_EDIT_OVRD_MAX_VEL);
	double dOvrdPos = GetDlgItemDouble(IDC_EDIT_OVRD_POS);
	double dOvrdVel = GetDlgItemDouble(IDC_EDIT_OVRD_VEL);
	
	AxmOverrideSetMaxVel(lAxisNo, dOvrdMaxVel);
	WriteLog(_T("Set Override Max Vel"));
	if (iOvrdMode == 0){
		AxmOverridePos(lAxisNo, dOvrdPos);
		WriteLog(_T("Override Pos"));
	}
	if (iOvrdMode == 1){
		AxmOverrideVel(lAxisNo, dOvrdVel);
		WriteLog(_T("Override Vel"));
	}
	if (iOvrdMode == 2)	{
		double dPos = GetDlgItemDouble(IDC_EDIT_SINGLE_POS);
		double dVel = GetDlgItemDouble(IDC_EDIT_SINGLE_VEL);
		double dAcc = GetDlgItemDouble(IDC_EDIT_SINGLE_ACCEL);
		double dDec = GetDlgItemDouble(IDC_EDIT_SINGLE_DECEL);
		long lTarget = m_chkOvrdTarget.GetCheck(); //[0] cmd, [1] act

		CString sTemp;
		int iRet;
		sTemp.Format(_T("This function may trigger breakpoint! \nWould you like to continue?"));
		iRet = AfxMessageBox(sTemp, MB_OKCANCEL);
		if (iRet == IDOK) {
			AxmOverrideVelAtPos(lAxisNo, dPos, dVel, dAcc, dDec, dOvrdPos, dOvrdVel, lTarget);	//*implement bug
			WriteLog(_T("Override At Pos"));
		}
		if (iRet == IDCANCEL){
			WriteLog(_T("Override At Pos is skipped."));
		}
	}
}


void CMotionGUIDlg::OnBnClickedButtonIntrplStrline()
{
	long lCoordinate;
	long lSize = 2;
	long lAxesNo[2];
	DWORD uAbsRelMode;
	double dMaxVelocity;
	double dMaxAccel;
	double dMaxDecel;
	lCoordinate = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_COORDINATE);
	lAxesNo[0] = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_AXIS_X);
	lAxesNo[1] = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_AXIS_Y);
	UpdateData(true);
	uAbsRelMode = m_rdoMonitorAxis;
	dMaxVelocity = GetDlgItemDouble(IDC_EDIT_INTRPL_VEL);
	dMaxAccel = GetDlgItemDouble(IDC_EDIT_INTRPL_ACCEL);
	dMaxDecel = GetDlgItemDouble(IDC_EDIT_INTRPL_DECEL);
	AxmContiWriteClear(lCoordinate);
	AxmContiSetAxisMap(lCoordinate, lSize, &lAxesNo[0]);
	AxmContiSetAbsRelMode(lCoordinate, uAbsRelMode); // Set to relative position drive.


	double dPosition[2];
	dPosition[0] = GetDlgItemDouble(IDC_EDIT_INTRPL_STRLINE_PX);
	dPosition[1] = GetDlgItemDouble(IDC_EDIT_INTRPL_STRLINE_PY);	
	AxmLineMove(lCoordinate, &dPosition[0], dMaxVelocity, dMaxAccel, dMaxDecel);

	WriteLog(_T("Straight Line Interpolation"));
}


void CMotionGUIDlg::OnBnClickedButtonIntrplCircenter()
{
	long lCoordinate;
	long lSize = 2;
	long lAxesNo[2];
	DWORD uAbsRelMode;
	double dMaxVelocity;
	double dMaxAccel;
	double dMaxDecel;
	lCoordinate = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_COORDINATE);
	lAxesNo[0] = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_AXIS_X);
	lAxesNo[1] = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_AXIS_Y);
	UpdateData(true);
	uAbsRelMode = m_rdoMonitorAxis;
	dMaxVelocity = GetDlgItemDouble(IDC_EDIT_INTRPL_VEL);
	dMaxAccel = GetDlgItemDouble(IDC_EDIT_INTRPL_ACCEL);
	dMaxDecel = GetDlgItemDouble(IDC_EDIT_INTRPL_DECEL);
	AxmContiWriteClear(lCoordinate);
	AxmContiSetAxisMap(lCoordinate, lSize, &lAxesNo[0]);
	AxmContiSetAbsRelMode(lCoordinate, uAbsRelMode); // Set to relative position drive.

	double dCenPos[2];
	double dEndPos[2];
	DWORD uDir = m_chkIntrplCirCenterDir.GetCheck();
	dCenPos[0] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRCENTER_CX);
	dCenPos[1] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRCENTER_CY);
	dEndPos[0] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRCENTER_EX);
	dEndPos[1] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRCENTER_EY);
	AxmCircleCenterMove(lCoordinate, &lAxesNo[0], &dCenPos[0], &dEndPos[0], dMaxVelocity, dMaxAccel, dMaxDecel, uDir);

	WriteLog(_T("Circle Center Interpolation"));
}


void CMotionGUIDlg::OnBnClickedButtonIntrplCirradius()
{
	CString sTemp;
	int iRet;
	sTemp.Format(_T("This function may not work. \nWould you like to continue?"));
	iRet = AfxMessageBox(sTemp, MB_OKCANCEL);
	if (iRet == IDCANCEL){
		WriteLog(_T("Interpolation Circle Radius is skipped."));
	}
	if (iRet == IDOK) {
		long lCoordinate;
		long lSize = 2;
		long lAxesNo[2];
		DWORD uAbsRelMode;
		double dMaxVelocity;
		double dMaxAccel;
		double dMaxDecel;
		lCoordinate = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_COORDINATE);
		lAxesNo[0] = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_AXIS_X);
		lAxesNo[1] = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_AXIS_Y);
		UpdateData(true);
		uAbsRelMode = m_rdoMonitorAxis;
		dMaxVelocity = GetDlgItemDouble(IDC_EDIT_INTRPL_VEL);
		dMaxAccel = GetDlgItemDouble(IDC_EDIT_INTRPL_ACCEL);
		dMaxDecel = GetDlgItemDouble(IDC_EDIT_INTRPL_DECEL);
		AxmContiWriteClear(lCoordinate);
		AxmContiSetAxisMap(lCoordinate, lSize, &lAxesNo[0]);
		AxmContiSetAbsRelMode(lCoordinate, uAbsRelMode); // Set to relative position drive.

		double dEndPos[2];
		double dRadius;
		DWORD uDir = m_chkIntrplCirRadiusDir.GetCheck();
		DWORD uDistance = m_chkIntrplCirRadiusDistance.GetCheck();
		dEndPos[0] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRRADIUS_EX);
		dEndPos[1] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRRADIUS_EY);
		dRadius = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRRADIUS_RADIUS);
		AxmCircleRadiusMove(lCoordinate, &lAxesNo[0], dRadius, &dEndPos[0], dMaxVelocity, dMaxAccel, dMaxDecel, uDir, uDistance);

		WriteLog(_T("Circle Radius Interpolation"));
	}	
}


void CMotionGUIDlg::OnBnClickedButtonIntrplCirangle()
{
	long lCoordinate;
	long lSize = 2;
	long lAxesNo[2];
	DWORD uAbsRelMode;
	double dMaxVelocity;
	double dMaxAccel;
	double dMaxDecel;
	lCoordinate = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_COORDINATE);
	lAxesNo[0] = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_AXIS_X);
	lAxesNo[1] = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_AXIS_Y);
	UpdateData(true);
	uAbsRelMode = m_rdoMonitorAxis;
	dMaxVelocity = GetDlgItemDouble(IDC_EDIT_INTRPL_VEL);
	dMaxAccel = GetDlgItemDouble(IDC_EDIT_INTRPL_ACCEL);
	dMaxDecel = GetDlgItemDouble(IDC_EDIT_INTRPL_DECEL);
	AxmContiWriteClear(lCoordinate);
	AxmContiSetAxisMap(lCoordinate, lSize, &lAxesNo[0]);
	AxmContiSetAbsRelMode(lCoordinate, uAbsRelMode); // Set to relative position drive.

	double dCenPos[2];
	double dAngle;
	DWORD uDir = m_chkIntrplCirAngleDir.GetCheck();
	dCenPos[0] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRANGLE_CX);
	dCenPos[1] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRANGLE_CY);
	dAngle = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRANGLE_ANGLE);
	AxmCircleAngleMove(lCoordinate, &lAxesNo[0], &dCenPos[0], dAngle, dMaxVelocity, dMaxAccel, dMaxDecel, uDir);

	WriteLog(_T("Circle Angle Interpolation"));
}


void CMotionGUIDlg::OnBnClickedButtonIntrplCirpoint()
{
	long lCoordinate;
	long lSize = 2;
	long lAxesNo[2];
	DWORD uAbsRelMode;
	double dMaxVelocity;
	double dMaxAccel;
	double dMaxDecel;
	lCoordinate = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_COORDINATE);
	lAxesNo[0] = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_AXIS_X);
	lAxesNo[1] = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_AXIS_Y);
	UpdateData(true);
	uAbsRelMode = m_rdoMonitorAxis;
	dMaxVelocity = GetDlgItemDouble(IDC_EDIT_INTRPL_VEL);
	dMaxAccel = GetDlgItemDouble(IDC_EDIT_INTRPL_ACCEL);
	dMaxDecel = GetDlgItemDouble(IDC_EDIT_INTRPL_DECEL);
	AxmContiWriteClear(lCoordinate);
	AxmContiSetAxisMap(lCoordinate, lSize, &lAxesNo[0]);
	AxmContiSetAbsRelMode(lCoordinate, uAbsRelMode); // Set to relative position drive.

	double dMidPos[2];
	double dEndPos[2];
	long lArcCircle = m_chkIntrplCirPointArcCircle.GetCheck();
	dMidPos[0] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRPOINT_MX);
	dMidPos[1] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRPOINT_MY);
	dEndPos[0] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRPOINT_EX);
	dEndPos[1] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRPOINT_EY);
	AxmCirclePointMove(lCoordinate, &lAxesNo[0], &dMidPos[0], &dEndPos[0], dMaxVelocity, dMaxAccel, dMaxDecel, lArcCircle);

	WriteLog(_T("Circle Point Interpolation"));
}


void CMotionGUIDlg::OnBnClickedButtonSingleRepeat()
{
	UpdateData(true); //for the thread
	static bool xState = 0;
	xState = 1 - xState;	//toggle
	if (xState){
		//AfxBeginThread(RepeatThread, 0);
		xThreadRepeatStop = false;
		unsigned long	ThreadID;
		m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RepeatThread, this, NULL, &ThreadID);
	}
	else{
		xThreadRepeatStop = true;
	}


	//UpdateData(true);
	//long lAxisNo = m_rdoSingleAxis;
	//DWORD uStatus;
	//AxmStatusReadInMotion(lAxisNo, &uStatus);	//[0] not in motion / [1] in motion
	//if (!uStatus){	// not in motion: the axis had stopped
	//}
	//else{
	//	WriteLog(_T("Axis is running"));
	//}

}

UINT RepeatThread(LPVOID Param) {
	CMotionGUIDlg *pDlg = (CMotionGUIDlg *)Param;
	pDlg->WriteLog(_T("Thread Repeat has started"));
	//pDlg->UpdateData(true); //places where the thread was called. put this here causes errror


	if (!pDlg->m_chkSimulate.GetCheck()){
		
		long lAxisNo = pDlg->m_rdoSingleAxis;
		double dPos = pDlg->GetDlgItemDouble(IDC_EDIT_SINGLE_POS);
		double dVel = pDlg->GetDlgItemDouble(IDC_EDIT_SINGLE_VEL);
		double dAcc = pDlg->GetDlgItemDouble(IDC_EDIT_SINGLE_ACCEL);
		double dDec = pDlg->GetDlgItemDouble(IDC_EDIT_SINGLE_DECEL);
		int iASyn = pDlg->m_chkSingleSynAsyn.GetCheck();

		//check if continue to run in abs mode
		//if running abs mode, the axis will stop and never start again after reach the target position
		//so give notificatiton
		DWORD uIsRelative;
		AxmMotGetAbsRelMode(lAxisNo, &uIsRelative);
		if (!uIsRelative){
			CString sTemp;
			int iRet;
			sTemp.Format(_T("Axis is running in Absolute Mode. \nWould you like to continue?"));
			iRet = AfxMessageBox(sTemp, MB_OKCANCEL);
			if (iRet == IDCANCEL){
				pDlg->WriteLog(_T("Repeat is skipped."));
				xThreadRepeatStop = true;
			}
			if (iRet == IDOK) {}
		}

		//check if the axis has been running beforehand
		DWORD uStatus;
		AxmStatusReadInMotion(lAxisNo, &uStatus);	//[0] not in motion / [1] in motion
		if (uStatus){	//in motion: the axis is running
			pDlg->WriteLog(_T("Axis has been running beforehand"));
			AfxMessageBox(_T("Axis has been running beforehand"));
			xThreadRepeatStop = true;
		}


		while (!xThreadRepeatStop){
			if (!iASyn){
				AxmMoveStartPos(lAxisNo, dPos, dVel, dAcc, dAcc);
			}
			else{
				AxmMoveStartPos(lAxisNo, dPos, dVel, dAcc, dDec);
			}
			do {
				AxmStatusReadInMotion(lAxisNo, &uStatus); 
			} while (uStatus);
			Sleep(THREAD_SLEEP);
		}
			
	}
	else{
		//--------------Test only begin-------------
		//DWORD uIsRelative;
		////AxmMotGetAbsRelMode(lAxisNo, &uIsRelative);
		//uIsRelative = 0;
		//if (!uIsRelative){
		//	CString sTemp;
		//	int iRet;
		//	sTemp.Format(_T("Axis is running in Absolute Mode. \nWould you like to continue?"));
		//	iRet = AfxMessageBox(sTemp, MB_OKCANCEL);
		//	if (iRet == IDCANCEL){
		//		pDlg->WriteLog(_T("Repeat is skipped."));
		//		xThreadRepeatStop = true;
		//	}
		//	if (iRet == IDOK) {}
		//}

		////check if the axis has been running beforehand
		//DWORD uStatus;
		////AxmStatusReadInMotion(lAxisNo, &uStatus);	//[0] not in motion / [1] in motion
		//uStatus = 1;
		//if (uStatus){	//in motion: the axis is running
		//	pDlg->WriteLog(_T("Axis has been running beforehand"));
		//	AfxMessageBox(_T("Axis has been running beforehand"));
		//	xThreadRepeatStop = true;
		//}
		//--------------Test only end-------------

		while (!xThreadRepeatStop){
			int iTemp = pDlg->m_sliValue.GetPos() % SLIDER_RANGE;
			pDlg->m_sliValue.SetPos(++iTemp);
		}
	}
	pDlg->WriteLog(_T("Thread Repeat has Stop"));
	return TRUE;
}

void CMotionGUIDlg::OnBnClickedButtonSingleJogP()
{
	UpdateData(true);
	long lAxisNo = m_rdoSingleAxis;
	double dVel = GetDlgItemDouble(IDC_EDIT_SINGLE_VEL);
	double dAcc = GetDlgItemDouble(IDC_EDIT_SINGLE_ACCEL);
	double dDec = GetDlgItemDouble(IDC_EDIT_SINGLE_DECEL);
	if (m_chkSingleSynAsyn.GetCheck() == 0)		AxmMoveVel(lAxisNo, dVel, dAcc, dAcc);
	else										AxmMoveVel(lAxisNo, dVel, dAcc, dDec);
}


void CMotionGUIDlg::OnBnClickedButtonSingleJogN()
{
	UpdateData(true);
	long lAxisNo = m_rdoSingleAxis;
	double dVel = -abs(GetDlgItemDouble(IDC_EDIT_SINGLE_VEL));
	double dAcc = GetDlgItemDouble(IDC_EDIT_SINGLE_ACCEL);
	double dDec = GetDlgItemDouble(IDC_EDIT_SINGLE_DECEL);
	if (m_chkSingleSynAsyn.GetCheck() == 0)		AxmMoveVel(lAxisNo, dVel, dAcc, dAcc);
	else										AxmMoveVel(lAxisNo, dVel, dAcc, dDec);
}


void CMotionGUIDlg::OnBnClickedButtonServoOn()
{
	UpdateData(true);
	long lAxisNo = m_rdoMonitorAxis;
	DWORD uStatus;
	AxmSignalIsServoOn(lAxisNo, &uStatus);
	AxmSignalServoOn(lAxisNo, 1 - uStatus);
}


void CMotionGUIDlg::OnBnClickedButtonResetAlarm()
{
	UpdateData(true);
	long lAxisNo = m_rdoMonitorAxis;
	AxmSignalServoAlarmReset(lAxisNo, 0);
}


void CMotionGUIDlg::OnBnClickedButtonContiBeginNode()
{

	//Begin node
	if (!xIsContiBegin){
		long lCoordinate;
		long lSize = 2;
		long lAxesNo[2];
		DWORD uAbsRelMode;
		double dMaxVelocity;
		double dMaxAccel;
		double dMaxDecel;
		lCoordinate = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_COORDINATE);
		lAxesNo[0] = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_AXIS_X);
		lAxesNo[1] = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_AXIS_Y);
		UpdateData(true);
		uAbsRelMode = m_rdoMonitorAxis;
		dMaxVelocity = GetDlgItemDouble(IDC_EDIT_INTRPL_VEL);
		dMaxAccel = GetDlgItemDouble(IDC_EDIT_INTRPL_ACCEL);
		dMaxDecel = GetDlgItemDouble(IDC_EDIT_INTRPL_DECEL);
		AxmContiWriteClear(lCoordinate);
		AxmContiSetAxisMap(lCoordinate, lSize, &lAxesNo[0]);
		AxmContiSetAbsRelMode(lCoordinate, uAbsRelMode); // Set to relative position drive.

		AxmContiBeginNode(lCoordinate);
		xIsContiBegin = true;
		xIsContiEnd = false;
		WriteLog(_T("Conti begin node"));
	}
	else{
		WriteLog(_T("Already begin node"));
	}
}

void CMotionGUIDlg::OnBnClickedButtonContiAddNode()
{
	UpdateData(true);
	int iSelect = m_rdoContiAddNode;
	//DWORD uAbsRelMode = m_rdoMonitorAxis; 
	long lCoordinate;
	//long lSize = 2;
	long lAxesNo[2];
	double dMaxVelocity;
	double dMaxAccel;
	double dMaxDecel;
	lCoordinate = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_COORDINATE);
	lAxesNo[0] = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_AXIS_X);
	lAxesNo[1] = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_AXIS_Y);
	dMaxVelocity = GetDlgItemDouble(IDC_EDIT_INTRPL_VEL);
	dMaxAccel = GetDlgItemDouble(IDC_EDIT_INTRPL_ACCEL);
	dMaxDecel = GetDlgItemDouble(IDC_EDIT_INTRPL_DECEL);
	if (!xIsContiBegin)	{ WriteLog(_T("Conti has not begin node")); }
	//if (xIsContiBegin && xIsContiEnd) { WriteLog(_T("Conti has end node")); }
	if (xIsContiBegin && !xIsContiEnd){
		if (iSelect == 0){	//add straight line node
			double dPosition[2];
			dPosition[0] = GetDlgItemDouble(IDC_EDIT_INTRPL_STRLINE_PX);
			dPosition[1] = GetDlgItemDouble(IDC_EDIT_INTRPL_STRLINE_PY);
			AxmLineMove(lCoordinate, &dPosition[0], dMaxVelocity, dMaxAccel, dMaxDecel);

			CString sLog[7];
			sLog[1].Format(_T("%ld"), lCoordinate);
			sLog[2].Format(_T("%.1lf"), dPosition[0]);
			sLog[3].Format(_T("%.1lf"), dPosition[1]);
			sLog[4].Format(_T("%.2lf"), dMaxVelocity);
			sLog[5].Format(_T("%.2lf"), dMaxAccel);
			sLog[6].Format(_T("%.2lf"), dMaxDecel);
			sLog[0] = _T("Add node: AxmLineMove(") + sLog[1] + _T(", {") + sLog[2] + _T(", ") + sLog[3] + _T("}, ")
				+ sLog[4] + _T(", ") + sLog[5] + _T(", ") + sLog[6] + _T(")");
			WriteLog(sLog[0]);
		}
		if (iSelect == 1){	//add circle center node
			double dCenPos[2];
			double dEndPos[2];
			DWORD uDir = m_chkIntrplCirCenterDir.GetCheck();
			dCenPos[0] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRCENTER_CX);
			dCenPos[1] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRCENTER_CY);
			dEndPos[0] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRCENTER_EX);
			dEndPos[1] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRCENTER_EY);
			AxmCircleCenterMove(lCoordinate, &lAxesNo[0], &dCenPos[0], &dEndPos[0], dMaxVelocity, dMaxAccel, dMaxDecel, uDir);

			CString sLog[12];
			sLog[1].Format(_T("%ld"), lCoordinate);
			sLog[2].Format(_T("%ld"), lAxesNo[0]);
			sLog[3].Format(_T("%ld"), lAxesNo[1]);
			sLog[4].Format(_T("%.1lf"), dCenPos[0]);
			sLog[5].Format(_T("%.1lf"), dCenPos[1]);
			sLog[6].Format(_T("%.1lf"), dEndPos[0]);
			sLog[7].Format(_T("%.1lf"), dEndPos[1]);
			sLog[8].Format(_T("%.2lf"), dMaxVelocity);
			sLog[9].Format(_T("%.2lf"), dMaxAccel);
			sLog[10].Format(_T("%.2lf"), dMaxDecel);
			sLog[11].Format(_T("%u"), uDir);
			sLog[0] = _T("Add node: AxmCircleCenterMove(") + sLog[1] + _T(", {") + sLog[2] + _T(", ") + sLog[3] + _T("}, ")
				+ _T("{") + sLog[4] + _T(", ") + sLog[5] + _T("}, ") + _T("{") + sLog[6] + _T(", ") + sLog[7] + _T("}, ")
				+ sLog[8] + _T(", ") + sLog[9] + _T(", ") + sLog[10] + _T(", ") + sLog[11] + _T(")");
			WriteLog(sLog[0]);
		}
		if (iSelect == 2){	//add circle radius node
			CString sTemp;
			int iRet;
			sTemp.Format(_T("This node may not work. \nWould you like to continue?"));
			iRet = AfxMessageBox(sTemp, MB_OKCANCEL);
			if (iRet == IDCANCEL){
				WriteLog(_T("Node Interpolation Circle Radius is skipped."));
			}
			if (iRet == IDOK) {
				double dEndPos[2];
				double dRadius;
				DWORD uDir = m_chkIntrplCirRadiusDir.GetCheck();
				DWORD uDistance = m_chkIntrplCirRadiusDistance.GetCheck();
				dEndPos[0] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRRADIUS_EX);
				dEndPos[1] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRRADIUS_EY);
				dRadius = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRRADIUS_RADIUS);
				AxmCircleRadiusMove(lCoordinate, &lAxesNo[0], dRadius, &dEndPos[0], dMaxVelocity, dMaxAccel, dMaxDecel, uDir, uDistance);

				CString sLog[12];
				sLog[1].Format(_T("%ld"), lCoordinate);
				sLog[2].Format(_T("%ld"), lAxesNo[0]);
				sLog[3].Format(_T("%ld"), lAxesNo[1]);
				sLog[4].Format(_T("%.1lf"), dRadius);
				sLog[5].Format(_T("%.1lf"), dEndPos[0]);
				sLog[6].Format(_T("%.1lf"), dEndPos[1]);
				sLog[7].Format(_T("%.2lf"), dMaxVelocity);
				sLog[8].Format(_T("%.2lf"), dMaxAccel);
				sLog[9].Format(_T("%.2lf"), dMaxDecel);
				sLog[10].Format(_T("%u"), uDir);
				sLog[11].Format(_T("%u"), uDistance);
				sLog[0] = _T("Add node: AxmCircleRadiusMove(") + sLog[1] + _T(", {") + sLog[2] + _T(", ") + sLog[3] + _T("}, ")
					+ sLog[4] + _T(", {") + sLog[5] + _T(", ") + sLog[6] + _T("}, ")
					+ sLog[7] + _T(", ") + sLog[8] + _T(", ") + sLog[9] + _T(", ") + sLog[10] + _T(", ") + sLog[11] + _T(")");
				WriteLog(sLog[0]);
			}
			
		}
		if (iSelect == 3){	//add cir angle node
			double dCenPos[2];
			double dAngle;
			DWORD uDir = m_chkIntrplCirAngleDir.GetCheck();
			dCenPos[0] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRANGLE_CX);
			dCenPos[1] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRANGLE_CY);
			dAngle = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRANGLE_ANGLE);
			AxmCircleAngleMove(lCoordinate, &lAxesNo[0], &dCenPos[0], dAngle, dMaxVelocity, dMaxAccel, dMaxDecel, uDir);

			CString sLog[11];
			sLog[1].Format(_T("%ld"), lCoordinate);
			sLog[2].Format(_T("%ld"), lAxesNo[0]);
			sLog[3].Format(_T("%ld"), lAxesNo[1]);
			sLog[4].Format(_T("%.1lf"), dCenPos[0]);
			sLog[5].Format(_T("%.1lf"), dCenPos[1]);
			sLog[6].Format(_T("%.1lf"), dAngle);
			sLog[7].Format(_T("%.2lf"), dMaxVelocity);
			sLog[8].Format(_T("%.2lf"), dMaxAccel);
			sLog[9].Format(_T("%.2lf"), dMaxDecel);
			sLog[10].Format(_T("%u"), uDir);

			sLog[0] = _T("Add node: AxmCircleAngleMove(") + sLog[1] + _T(", {") + sLog[2] + _T(", ") + sLog[3] + _T("}, ")
				+ _T("{") + sLog[4] + _T(", ") + sLog[5] + _T("}, ") + sLog[6] + _T(", ")
				+ sLog[7] + _T(", ") + sLog[8] + _T(", ") + sLog[9] + _T(", ") + sLog[10] + _T(")");
			WriteLog(sLog[0]);
		}
		if (iSelect == 4){
			double dMidPos[2];
			double dEndPos[2];
			long lArcCircle = m_chkIntrplCirPointArcCircle.GetCheck();
			dMidPos[0] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRPOINT_MX);
			dMidPos[1] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRPOINT_MY);
			dEndPos[0] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRPOINT_EX);
			dEndPos[1] = GetDlgItemDouble(IDC_EDIT_INTRPL_CIRPOINT_EY);
			AxmCirclePointMove(lCoordinate, &lAxesNo[0], &dMidPos[0], &dEndPos[0], dMaxVelocity, dMaxAccel, dMaxDecel, lArcCircle);

			CString sLog[12];
			sLog[1].Format(_T("%ld"), lCoordinate);
			sLog[2].Format(_T("%ld"), lAxesNo[0]);
			sLog[3].Format(_T("%ld"), lAxesNo[1]);
			sLog[4].Format(_T("%.1lf"), dMidPos[0]);
			sLog[5].Format(_T("%.1lf"), dMidPos[1]);
			sLog[6].Format(_T("%.1lf"), dEndPos[0]);
			sLog[7].Format(_T("%.1lf"), dEndPos[1]);
			sLog[8].Format(_T("%.2lf"), dMaxVelocity);
			sLog[9].Format(_T("%.2lf"), dMaxAccel);
			sLog[10].Format(_T("%.2lf"), dMaxDecel);
			sLog[11].Format(_T("%ld"), lArcCircle);

			sLog[0] = _T("Add node: AxmCirclePointMove(") + sLog[1] + _T(", {") + sLog[2] + _T(", ") + sLog[3] + _T("}, ")
				+ _T("{") + sLog[4] + _T(", ") + sLog[5] + _T("}, ")
				+ _T("{") + sLog[6] + _T(", ") + sLog[7] + _T("}, ")
				+ sLog[8] + _T(", ") + sLog[8] + _T(", ") + sLog[10] + _T(", ") + sLog[11] + _T(")");
			WriteLog(sLog[0]);
		}
	}
	
	
}

void CMotionGUIDlg::OnBnClickedButtonContiEndNode()
{
	if (!xIsContiEnd){
		long lCoordinate = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_COORDINATE);
		AxmContiEndNode(lCoordinate);
		xIsContiBegin = false;
		xIsContiEnd = true;

		WriteLog(_T("Conti end node"));
	}
	else{
		WriteLog(_T("Already end node"));
	}
	
}

void CMotionGUIDlg::OnBnClickedButtonContiStart()
{
	long lCoordinate = (long)GetDlgItemDouble(IDC_EDIT_INTRPL_COORDINATE);
	AxmContiStart(lCoordinate, 0, 0);
	WriteLog(_T("Conti start"));

}

LRESULT CMotionGUIDlg::OnJogBtnDn(WPARAM wParam, LPARAM lParam)
{
	CString strData;
	DWORD	dwRetCode;
	double	dJogVel, dJogAcc, dJogDec;
	

	dJogVel = fabs(GetDlgItemDouble(IDC_EDIT_SINGLE_VEL));
	dJogAcc = fabs(GetDlgItemDouble(IDC_EDIT_SINGLE_ACCEL));
	dJogDec = fabs(GetDlgItemDouble(IDC_EDIT_SINGLE_DECEL));
	UpdateData(true);	long m_lAxisNo = m_rdoSingleAxis;

	switch (wParam)
	{
	case IDC_JOGBUTTON_SINGLE_P:
		dwRetCode = AxmMoveVel(m_lAxisNo, dJogVel, dJogAcc, dJogDec);
		if (dwRetCode != AXT_RT_SUCCESS){
			strData.Format(_T("AxmMoveVel return error[Code:% 04d]"), dwRetCode);
			MessageBox(strData, _T("Error"), MB_OK | MB_ICONERROR);
		}
		break;

	case IDC_JOGBUTTON_SINGLE_N:
		dwRetCode = AxmMoveVel(m_lAxisNo, -dJogVel, dJogAcc, dJogDec);
		if (dwRetCode != AXT_RT_SUCCESS){
			strData.Format(_T("AxmMoveVel return error[Code:%04d]"), dwRetCode);
			MessageBox(strData, _T("Error"), MB_OK | MB_ICONERROR);
		}
		break;
	}

	return 0;
}

LRESULT CMotionGUIDlg::OnJogBtnUp(WPARAM wParam, LPARAM lParam)
{
	UpdateData(true);	long m_lAxisNo = m_rdoSingleAxis;
	AxmMoveSStop(m_lAxisNo);
	return 0;
}


