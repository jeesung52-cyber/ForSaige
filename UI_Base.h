#pragma once

//#include "INIsimple.h"
//#include "CtrlUtil/CListCtrlEx.h"

// UI_Base 대화 상자
#define		TITLEBAR_HEIGHT		25
#define		TITLE_BUTTON_HEIGHT	15

#define		TIMERID_POS_RECORD	25
#define		TIMERID_RESIZE_DELAY	100


struct CTINFO
{
	UINT	nID;
	CRect	rect;
	CTINFO()
	{
		nID = 0;
		rect = CRect(0, 0, 0, 0);
	}
};
#define WM_USER_RESIZE (WM_USER + 0x77)

class UI_Base : public CDialog
{
	DECLARE_DYNAMIC(UI_Base)

public:
	UI_Base(UINT nID, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	UI_Base(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~UI_Base();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UI_Base };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	INT		m_nViewType;

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();

	void GetWindowRect(LPRECT lpRect) const;
	void GetClientRect(LPRECT lpRect) const;
	virtual void	LocateControl();
	void	SetSysBtnUse(BOOL bExit, BOOL bMaxi, BOOL bMini, BOOL bfold = FALSE)
	{ 
		m_bSysExitBtnUse = bExit;
		m_bSysMaxiBtnUse = bMaxi;
		m_bSysMiniBtnUse = bMini;
		m_bSysFoldBtnUse = bfold;
	}
	void	SetSysExitBtnUse(BOOL buse) { m_bSysExitBtnUse = buse; }
	void	SetSizeChange(BOOL bchange);
	void	SetAddString(CString szadd) { m_szaddString = szadd; }
	void	SetbHelp(BOOL buse) { m_bHelpBtnUse = buse; }
	virtual void	Helpmsg();

	CPen	m_Borderpen;

	BOOL	m_bFirst;
	BOOL	m_bCustomTitlebar;
	CRect	m_titleRect;
	CString	m_szaddString;
	BOOL	m_bSysExitBtnUse;
	BOOL	m_bSysMaxiBtnUse;
	BOOL	m_bSysMiniBtnUse;
	BOOL	m_bSysFoldBtnUse;
	BOOL	m_bFoldState;
	BOOL	m_bHelpBtnUse;
	CString		m_szHelp;

	std::vector<CTINFO>		m_vexpandCTlist;
	void	Show_AllCTlist(LONG bshow);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void	SetbCustomTitleBar(BOOL b) { m_bCustomTitlebar = b; }
	void	SetszDesc(CString str);
	void	SetMoveLock(BOOL bmove) { m_bMoveLock = bmove; }
	void	SetFoldRect(CRect rect);
	void	SetExpandRect(CRect rect);
	CRect	GetFoldR() { return m_foldR; }
	CRect	GetExpandR() { return m_expandR; }
	void	SetSysFoldingUse(BOOL buse) { m_bSysFoldBtnUse = buse; }
	void	SetFoldingState(BOOL bstate) { m_bFoldState = bstate; }
	UINT	GetViewMax(){ return m_nViewType; }
public:
	CRect		m_rExit;
	CRect		m_rMaximize;
	CRect		m_rMinimize;
	CRect		m_rFold;
	CRect		m_rHelp;

private:
	CString		m_szdesc;
	CRect		m_rDesc;
	CRect		m_rloadPos;
	CString		m_szTitle;
	BOOL		m_bMoveLock;
	CRect		m_foldR;
	CRect		m_expandR;
	CPoint		m_Curpos;

	

	void		LoadPos();
	void		SavePos();

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);



private:
	
	void SwitchScreenMinMax();
public:
	afx_msg void OnPaint();
};
