#pragma once

#include <vector>
#include "SaigeSeg.h"

class CMainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = nullptr);
	virtual ~CMainDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	static const int BAR_HEIGHT = 100;
	static const int TOOLBAR_H = 100;
	static const int BTN_W = 56;
	static const int BTN_H = 44;

	static const COLORREF BG_MAIN;
	static const COLORREF BG_TITLE;
	static const COLORREF BG_TOOLBAR;
	static const COLORREF BG_LIST;
	static const COLORREF BG_BTNLOAD;
	static const COLORREF SEP;
	static const COLORREF TXT;
	static const COLORREF TXT_DIM;
	static const COLORREF HOVER;
	static const COLORREF CLOSE_HOVER;
	static const COLORREF ICON_COLOR;
	static const COLORREF ACCENT;
	static const COLORREF BG_PANEL;
	static const COLORREF BG_ROW;
	static const COLORREF BG_ROW_ALT;
	static const COLORREF BG_SELECTED;

	enum HoverBtn { HVR_NONE, HVR_MIN, HVR_MAX, HVR_CLOSE, HVR_LOAD, HVR_MODEL_LOAD };
	HoverBtn m_eHover;

	CListBox m_ctlImageList;
	HBRUSH m_hBrList;

	CButton m_btnLoad;
	CFont m_fontTitle, m_fontTime, m_fontBtn, m_fontList;

	CRect m_rcLogo, m_rcTool;
	CRect m_rcMin, m_rcMax, m_rcClose, m_rcLoad;
	CRect m_rcRightPanel, m_rcImageListPanel, m_rcModelPanel, m_rcBlankPanel;
	CRect m_rcImageListFrame, m_rcModelLoad;

	bool m_bDrag, m_bMaxed;
	CPoint m_ptDrag;
	CRect m_rcRestore;

	std::vector<CString> m_vPaths;
	std::vector<cv::Mat> m_vImgs;
	int m_nCurIdx;
	cv::Mat m_cvDisplay;

	CSaigeSeg m_saigeSeg;
	CString m_strModelPath;
	CString m_strModelName;
	CString m_strModelStatus;
	CString m_strModelInfo;
	bool m_bModelLoaded;

	void ApplyFullScreen();
	void ToggleMax();
	void RecRects();

	void DrawLogo(CDC& dc);
	void DrawTool(CDC& dc);
	void DrawBody(CDC& dc);
	void DrawIcon(CDC& dc, CRect rc, int type);
	void DrawLoadButton(CDC& dc);
	void DrawModelPanel(CDC& dc);
	void DrawModelLoadButton(CDC& dc);
	void ShowLoadMenu();

	HoverBtn HitTest(CPoint pt);

	void LoadFolder();
	void LoadFiles();
	void LoadFromFolder(const CString& strFolder);
	void LoadFromFiles(const std::vector<CString>& vFiles);
	void SelImage(int nIdx);
	void UpdList();
	void LoadSaigeModel();
	int SafeLoadSaigeModel();

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT, int, int);
	afx_msg void OnLButtonDown(UINT, CPoint);
	afx_msg void OnLButtonUp(UINT, CPoint);
	afx_msg void OnMouseMove(UINT, CPoint);
	afx_msg void OnLButtonDblClk(UINT, CPoint);
	afx_msg void OnMouseLeave();
	afx_msg void OnTimer(UINT_PTR);
	afx_msg LRESULT OnNcHitTest(CPoint pt);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnBnClickedLoad();
	afx_msg void OnLoadFolderCmd();
	afx_msg void OnLoadFileCmd();
	afx_msg void OnSelList();
};
