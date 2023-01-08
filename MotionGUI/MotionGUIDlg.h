
// MotionGUIDlg.h : header file
//

#pragma once
//#include "ChartCtrl/ChartCtrl.h"
//#include "ChartCtrl/ChartLineSerie.h"
//#include "ChartCtrl/ChartLabel.h"
//#include "D:\ProjLocation\VisualStudioProject\MotionControl\MotionGUI\MotionGUI\ChartCtrl\ChartCtrl.h"
#include "../MotionGUI/ChartCtrl/ChartCtrl.h"
#include "../MotionGUI/ChartCtrl/ChartLineSerie.h"
#include "../MotionGUI/ChartCtrl/ChartLabel.h"
#include "afxwin.h"
#include "afxcmn.h"

#include <math.h>
#include "../MotionGUI/JogButton.h"
#include "JogButton.h"



// CMotionGUIDlg dialog
class CMotionGUIDlg : public CDialogEx
{
// Construction
public:
	CMotionGUIDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MOTIONGUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	//Chart: Path planning 
	CChartCtrl m_ChartPath;
	CChartPointsSerie* m_pChartPathPointSerie;
	//Chart: Cmd Pos
	CChartCtrl m_ChartPos;
	CChartLineSerie* m_pChartPos0LineSerie;//Axis 0
	CChartLineSerie* m_pChartPos1LineSerie;//Axis 1
	//Chart: Cmd Vel
	CChartCtrl m_ChartVel;
	CChartLineSerie* m_pChartVel0LineSerie;//Axis 0
	CChartLineSerie* m_pChartVel1LineSerie;//Axis 1



	CListBox m_lsbLog;
	CComboBox m_cboAxisSet;
	CComboBox m_cboPulseOutMethod;
	CComboBox m_cboEncoderInputMethod;
	CComboBox m_cboVelProfile;
	CComboBox m_cboAbsRelMode;
	CSliderCtrl m_sliValue;


	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonTest();


	double GetDlgItemDouble(int nID);
	void SetDlgItemDouble(int nID, double value, int nPoint = 3);
	void WriteLog1(LPCTSTR lpszString, CListBox &m_lsbg);
	void WriteLog2(LPCTSTR lpszString);	//using m_lsbLog to write event log
	void WriteLog(LPCTSTR lpszString, int nID = IDC_LIST_EVENT_LOG);
	void DisplayLocation(double PosX, double PosY, int nID = IDC_STATIC_PATH_LOCATION);
	void EnableListboxHorizontalScrollbar(int nID = IDC_LIST_EVENT_LOG);



	void InitControl();
	void InitLibrary();
	void InitMotionSignal();
	
	void ApplySettingAxis(long lAxisNo);
	void ApplySettingAxisAll();
	void UpdateAxisInfo(long lAxisNo);
	void ChartDraw();
	void SetAxisHome(long lAxisNo);




	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangeComboAxisset();
	afx_msg void OnBnClickedButtonSettingAxis();
	afx_msg void OnBnClickedButtonSettingAxisAll();
	afx_msg void OnBnClickedButtonAddPoint();
	afx_msg void OnBnClickedButtonClearPath();
	afx_msg void OnEnKillfocusEditPathRangeMaxY();
	afx_msg void OnEnKillfocusEditPathRangeMaxX();
	afx_msg void OnEnKillfocusEditPathRangeMinX();
	afx_msg void OnEnKillfocusEditPathRangeMinY();
	afx_msg void OnBnClickedButtonChartObservationTime();
	

	CButton m_rdoMonitorAxis0;
	CButton m_rdoMonitorAxis1;
	int m_rdoMonitorAxis;
	int m_rdoSingleAxis;
	int m_rdoOverride;
	int m_rdoInterpolation;
	int m_rdoContiAddNode;
	bool xIsContiBegin = false;
	bool xIsContiEnd = true;

	CButton m_rdoSingleAxis0;

	CButton m_chkSingleSynAsyn;
	CButton m_rdoOvrdPos;
	CButton m_rdoIntrplAbs;
	CButton m_chkIntrplCirCenterDir;
	CButton m_chkIntrplCirRadiusDir;
	CButton m_chkIntrplCirRadiusDistance;
	CButton m_chkIntrplCirAngleDir;
	CButton m_chkChartLegend;
	afx_msg void OnBnClickedCheckChartLegend();
	CButton m_chkSimulate;
	afx_msg void OnBnClickedButtonSingleMove();
	afx_msg void OnBnClickedButtonSingleStop();
	afx_msg void OnBnClickedButtonSingleEstop();
	afx_msg void OnBnClickedButtonMultiMove();
	
	afx_msg void OnBnClickedButtonMultiStop();
	afx_msg void OnBnClickedButtonMultiEstop();
	afx_msg void OnBnClickedButtonMonitorClear();
	afx_msg void OnBnClickedButtonMonitorClearAll();
	CButton m_chkServoOn;
	afx_msg void OnBnClickedCheckServoOn();
	CButton m_chkAlarm;
	afx_msg void OnBnClickedCheckAlarm();
	afx_msg void OnBnClickedButtonOvrdOverride();
	CButton m_chkOvrdTarget;
	afx_msg void OnBnClickedButtonIntrplStrline();
	afx_msg void OnBnClickedButtonIntrplCircenter();
	afx_msg void OnBnClickedButtonIntrplCirradius();
	afx_msg void OnBnClickedButtonIntrplCirangle();
	afx_msg void OnBnClickedButtonIntrplCirpoint();
	CButton m_chkIntrplCirPointArcCircle;
	afx_msg void OnBnClickedButtonSingleRepeat();
	afx_msg void OnBnClickedButtonSingleJogP();
	afx_msg void OnBnClickedButtonSingleJogN();
	afx_msg void OnBnClickedButtonServoOn();
	afx_msg void OnBnClickedButtonResetAlarm();
	afx_msg void OnBnClickedButtonContiBeginNode();
	afx_msg void OnBnClickedButtonContiEndNode();
	afx_msg void OnBnClickedButtonContiAddNode();
	afx_msg void OnBnClickedButtonContiStart();

	

	HANDLE	m_hThread;

	CJogButton m_jbtnSingleP;
	CJogButton m_jbtnSingleN;
	afx_msg LRESULT OnJogBtnDn(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnJogBtnUp(WPARAM wParam, LPARAM lParam);

	CButton m_rdoIntrpRel;
};
