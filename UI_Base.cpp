// UI_Base.cpp: 구현 파일
//

#include "pch.h"
#include "UI_Base.h"
#include "afxdialogex.h"
//#include "INIsimple.h"


// UI_Base 대화 상자

IMPLEMENT_DYNAMIC(UI_Base, CDialog)

UI_Base::UI_Base(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_UI_Base, pParent)
{
	m_bFirst = TRUE;
	SetbCustomTitleBar(FALSE);
	m_bSysExitBtnUse = TRUE;
	m_bSysMaxiBtnUse = TRUE;
	m_bSysMiniBtnUse = TRUE;
	m_bSysFoldBtnUse = TRUE;
	m_bHelpBtnUse = TRUE;
	m_szdesc = _T("");
	m_rDesc = CRect(0, 0, 0, 0);
	m_rloadPos = CRect(0, 0, 0, 0);
	m_bMoveLock = FALSE;
	m_bFoldState = TRUE;

	m_foldR = CRect(0, 0, 0, 0);
	m_expandR = CRect(0, 0, 0, 0);
	m_Curpos = CPoint(0, 0);
	m_szaddString = _T("");
	m_szHelp = _T("");
	m_nViewType = FALSE;
}

UI_Base::UI_Base(UINT nID, CWnd* pParent /*=nullptr*/)
	: CDialog(nID, pParent)
{
	m_bFirst = TRUE;
	SetbCustomTitleBar(FALSE);
	m_bSysExitBtnUse = TRUE;
	m_bSysMaxiBtnUse = TRUE;
	m_bSysMiniBtnUse = TRUE;
	m_bSysFoldBtnUse = TRUE;
	m_bHelpBtnUse = TRUE;
	m_szdesc = _T("");
	m_rDesc = CRect(0, 0, 0, 0);
	m_rloadPos = CRect(0, 0, 0, 0);
	m_bMoveLock = FALSE;
	m_bFoldState = TRUE;

	m_foldR = CRect(0, 0, 0, 0);
	m_expandR = CRect(0, 0, 0, 0);
	m_Curpos = CPoint(0, 0);
	m_szaddString = _T("");
	m_szHelp = _T("");
}


UI_Base::~UI_Base()
{
	//::DeleteObject(&m_Borderpen);
	m_Borderpen.DeleteObject();
}

void UI_Base::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(UI_Base, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOVE()
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// UI_Base 메시지 처리기


BOOL UI_Base::OnEraseBkgnd(CDC* pDC)
{
	CRect	rect;
	CWnd::GetClientRect(&rect);
	CBrush	br(Utility::st_back_color3());
	pDC->FillRect(&rect, &br);

	if (m_bCustomTitlebar)
	{
		CBrush	tbr(Utility::st_back_color3());
		pDC->FillRect(&m_titleRect, &tbr);

		CRect	t;
		CWnd::GetClientRect(&t);
		//pDC->SelectStockObject(WHITE_PEN);
		CPen* pold = pDC->SelectObject(&m_Borderpen);
		pDC->MoveTo(0, 0);
		pDC->LineTo(0, t.bottom-1);
		pDC->LineTo(t.right-1, t.bottom-1);
		pDC->LineTo(t.right-1, 0);
		pDC->LineTo(0, 0);
		pDC->MoveTo(0, m_titleRect.bottom - 1);
		pDC->LineTo(t.right - 1, m_titleRect.bottom - 1);
		pDC->SelectObject(pold);

		CString		sztitle;
		GetWindowText(sztitle);

		HFONT	holdfont, hfont;
		//hfont = (HFONT)GetStockObject(ANSI_VAR_FONT);
// #define FW_THIN             100
// #define FW_EXTRALIGHT       200
// #define FW_LIGHT            300
// #define FW_NORMAL           400
// #define FW_MEDIUM           500
// #define FW_SEMIBOLD         600
// #define FW_BOLD             700
// #define FW_EXTRABOLD        800
// #define FW_HEAVY            900
		hfont = ::CreateFont(20, 0, 0, 0, FW_BOLD, 0, 0, 0,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS,
			PROOF_QUALITY, DEFAULT_PITCH | FF_DECORATIVE, _T("Arial"));
		holdfont = (HFONT)pDC->SelectObject(hfont);
		pDC->SetTextColor(UI_WHITE);
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(_T("  ") + sztitle + m_szaddString, m_titleRect
			, DT_LEFT | DT_VCENTER | DT_SINGLELINE);		
		pDC->SelectObject(holdfont);
		::DeleteObject(hfont);


		CPen		pen;
		pen.CreatePen(PS_SOLID, 1, Utility::st_txt_backcolor());
		CPen*	pOldpen = NULL;
		if (m_bSysExitBtnUse)
		{
			pDC->SelectStockObject(WHITE_PEN);
		}
		else
		{
			pOldpen = pDC->SelectObject(&pen);
		}
		pDC->SelectStockObject(BLACK_BRUSH);

		pDC->Rectangle(&m_rExit);
		pDC->MoveTo(m_rExit.TopLeft());
		pDC->LineTo(m_rExit.BottomRight());
		pDC->MoveTo(m_rExit.right-1, m_rExit.top);
		pDC->LineTo(m_rExit.left, m_rExit.bottom-1);

		if (m_bSysMaxiBtnUse)
		{
			pDC->SelectStockObject(WHITE_PEN);
		}
		else
		{

			pOldpen = pDC->SelectObject(&pen);
		}
		pDC->Rectangle(&m_rMaximize);
		CRect	r = m_rMaximize;
		r.DeflateRect(3, 3);
		pDC->Rectangle(&r);


		if (m_bSysMiniBtnUse)
		{
			pDC->SelectStockObject(WHITE_PEN);
		}
		else
		{

			pOldpen = pDC->SelectObject(&pen);
		}
		pDC->Rectangle(&m_rMinimize);
		r = m_rMinimize;
		r.DeflateRect(1, 4);
		pDC->MoveTo(r.left, r.bottom);
		pDC->LineTo(r.right, r.bottom);

		if (m_bSysFoldBtnUse)
		{
			pDC->SelectStockObject(WHITE_PEN);
		}
		else
		{
			pOldpen = pDC->SelectObject(&pen);
		}

		pDC->Rectangle(&m_rFold);
		r = m_rFold;
		r.DeflateRect(4, 4);
		if (m_bFoldState == FALSE)
		{
			pDC->MoveTo(r.left, r.top);
			pDC->LineTo(r.right, r.top);
			pDC->LineTo(
				r.left + r.Width() / 2, r.bottom
				);
			pDC->LineTo(r.left, r.top);
		}
		else
		{
			pDC->MoveTo(r.left, r.bottom);
			pDC->LineTo(r.right, r.bottom);
			pDC->LineTo(
				r.left + r.Width() / 2, r.top
			);
			pDC->LineTo(r.left, r.bottom);
		}


		COLORREF	clrh;
		if (m_bHelpBtnUse)
		{
			pDC->SelectStockObject(WHITE_PEN);
			clrh = UI_WHITE;
		}
		else
		{
			pOldpen = pDC->SelectObject(&pen);
			clrh = Utility::st_txt_backcolor();
		}
		pDC->Rectangle(&m_rHelp);
		Utility::uDrawText(pDC->GetSafeHdc(), _T("?"), m_rHelp, 7, 14, clrh, TRUE, DT_CENTER);

		if (pOldpen != NULL)
		{
			pDC->SelectObject(pOldpen);
		}
		::DeleteObject(&pen);
	}
	else
	{
		CRect	t;
		CWnd::GetClientRect(&t);
		//pDC->SelectStockObject(WHITE_PEN);
		CPen* pold = pDC->SelectObject(&m_Borderpen);
		pDC->MoveTo(0, 0);
		pDC->LineTo(0, t.bottom - 1);
		pDC->LineTo(t.right - 1, t.bottom - 1);
		pDC->LineTo(t.right - 1, 0);
		pDC->LineTo(0, 0);
		pDC->SelectObject(pold);
	}

	return TRUE;

	return CDialog::OnEraseBkgnd(pDC);
}


BOOL UI_Base::PreTranslateMessage(MSG* pMsg)
{
	if (Utility::ERKey(pMsg))
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}


HBRUSH UI_Base::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}


BOOL UI_Base::OnInitDialog()
{
	CDialog::OnInitDialog();

	LONG	retval;
	if (m_bCustomTitlebar)
	{
		LONG	style = ::GetWindowLong(this->m_hWnd, GWL_STYLE);
		style &= ~(WS_CAPTION | WS_THICKFRAME | WS_DLGFRAME | WS_BORDER);
		retval = ::SetWindowLong(this->m_hWnd, GWL_STYLE, style);
// 		if (retval == 0)
// 			util.GetLastError2();
		LONG exstyle = GetWindowLong(this->m_hWnd, GWL_EXSTYLE);
		exstyle &= ~(WS_EX_DLGMODALFRAME);
		LONG retval = SetWindowLong(this->m_hWnd, GWL_EXSTYLE, exstyle);
// 		if (retval == 0)
// 			util.GetLastError2();
	}
	else
	{
		LONG	style = ::GetWindowLong(this->m_hWnd, GWL_STYLE);
		style &= ~(WS_CAPTION | WS_THICKFRAME | WS_DLGFRAME | WS_BORDER);
		retval = ::SetWindowLong(this->m_hWnd, GWL_STYLE, style);
// 		if (retval == 0)
// 			util.GetLastError2();
		LONG exstyle = GetWindowLong(this->m_hWnd, GWL_EXSTYLE);
		exstyle &= ~(WS_EX_DLGMODALFRAME);
		LONG retval = SetWindowLong(this->m_hWnd, GWL_EXSTYLE, exstyle);
// 		if (retval == 0)
// 			util.GetLastError2();
	}

	if( m_Borderpen.m_hObject == NULL )
 		m_Borderpen.CreatePen(PS_SOLID, 1, Utility::st_line_color1());

	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void UI_Base::SetSizeChange(BOOL bchange)
{
	if (m_bCustomTitlebar)
	{
		LONG	style = ::GetWindowLong(this->m_hWnd, GWL_STYLE);
		if (bchange == FALSE)
			style &= ~(WS_THICKFRAME | WS_DLGFRAME | WS_BORDER);
		else
			style |= WS_THICKFRAME | WS_DLGFRAME | WS_BORDER;
		::SetWindowLong(this->m_hWnd, GWL_STYLE, style);
		LONG exstyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
		if (bchange == FALSE)
			exstyle &= ~(WS_EX_DLGMODALFRAME);
		else
			exstyle |= WS_EX_DLGMODALFRAME;
		SetWindowLong(m_hWnd, GWL_EXSTYLE, exstyle);
	}
	else
	{
		LONG	style = ::GetWindowLong(this->m_hWnd, GWL_STYLE);
		if (bchange == FALSE)
			style &= ~(WS_THICKFRAME | WS_DLGFRAME | WS_BORDER);
		else
			style |= WS_THICKFRAME | WS_DLGFRAME | WS_BORDER;
		::SetWindowLong(this->m_hWnd, GWL_STYLE, style);
		LONG exstyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
		if (bchange == FALSE)
			exstyle &= ~(WS_EX_DLGMODALFRAME);
		else
			exstyle |= WS_EX_DLGMODALFRAME;
		SetWindowLong(m_hWnd, GWL_EXSTYLE, exstyle);
	}

	this->Invalidate(TRUE);
}


void UI_Base::GetWindowRect(LPRECT lpRect) const
{
	CWnd::GetWindowRect(lpRect);
}
void UI_Base::GetClientRect(LPRECT lpRect) const
{
	CWnd::GetClientRect(lpRect);

	if (m_bCustomTitlebar)
	{
		lpRect->top = TITLEBAR_HEIGHT;
	}
}


void UI_Base::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_titleRect.PtInRect(point))
	{
		if (m_bSysExitBtnUse)
		{
			if (m_rExit.PtInRect(point))
			{
				this->ShowWindow(SW_HIDE);
				return;
			}
		}
		if (m_bSysMaxiBtnUse)
		{
			if (m_rMaximize.PtInRect(point))
			{
				m_nViewType = (this->GetStyle() & WS_MAXIMIZE) ? SW_RESTORE : SW_MAXIMIZE;
				this->ShowWindow(m_nViewType);
				return;
			}
		}
		if( m_bSysMiniBtnUse)
		{
			if (m_rMinimize.PtInRect(point))
			{
				this->ShowWindow(SW_MINIMIZE);
				return;
			}
		}
		if (m_bSysFoldBtnUse)
		{
			if (m_rFold.PtInRect(point))
			{
				m_bFoldState = !m_bFoldState;

				switch (m_bFoldState)
				{

				case FALSE:
				{
					CPoint		wp = point;
					ClientToScreen(&wp);
					wp -= point;
					m_foldR.MoveToXY(wp);
					m_expandR.MoveToXY(wp);

					Utility::RectinMonitor(this, &m_expandR);
					break;
				}

				}
				

				LocateControl();
				Invalidate();
				return;
			}
		}
		if (m_bHelpBtnUse)
		{
			if (m_rHelp.PtInRect(point))
			{
				Helpmsg();
			}
		}

		if( !m_bMoveLock )
			DefWindowProc(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	}

	CDialog::OnLButtonDown(nFlags, point);
}


void UI_Base::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	if (m_szTitle.IsEmpty())
		return;
	if (!m_szTitle.Compare(_T("대화 상자")))
		return;
// 	SetTimer(TIMERID_POS_RECORD, 1000, NULL);
}


void UI_Base::LocateControl()
{
	if (m_bCustomTitlebar == FALSE)
		return;
	//SetRedraw(FALSE);

	CWnd::GetClientRect(&m_titleRect);
	m_titleRect.bottom = TITLEBAR_HEIGHT;

	m_rExit = m_titleRect;
	m_rExit.bottom = TITLE_BUTTON_HEIGHT;
	m_rExit.OffsetRect(0, (TITLEBAR_HEIGHT - TITLE_BUTTON_HEIGHT) / 2);
	m_rExit.right = m_titleRect.right - 10;
	m_rExit.left = m_rExit.right - TITLE_BUTTON_HEIGHT;

	m_rMaximize = m_rExit;
	m_rMaximize.OffsetRect(-(TITLE_BUTTON_HEIGHT + 5), 0);

	m_rMinimize = m_rMaximize;
	m_rMinimize.OffsetRect(-(TITLE_BUTTON_HEIGHT + 5), 0);

	m_rFold = m_rMinimize;
	m_rFold.OffsetRect(-(TITLE_BUTTON_HEIGHT + 5), 0);

	m_rHelp = m_rFold;
	m_rHelp.OffsetRect(-(TITLE_BUTTON_HEIGHT + 5), 0);

// 	CRgn	rgn;
// 	CRect	rect;
// 	GetWindowRect(&rect);
// 	rgn.CreateRoundRectRgn(
// 		0,
// 		0,
// 		rect.Width(),
// 		rect.Height(),
// 		30,
// 		30
// 	);
// 	SetWindowRgn(static_cast<HRGN>(rgn.GetSafeHandle()), TRUE);
// 	rgn.Detach();

	if (m_bSysFoldBtnUse)
	{
		switch (m_bFoldState)
		{
		case FALSE:
			this->MoveWindow(&m_expandR);
			if (m_vexpandCTlist.size() != 0)
			{
				for each (CTINFO info in m_vexpandCTlist)
				{
					GetDlgItem(info.nID)->MoveWindow(info.rect);
				}
			}
			break;
			
		case TRUE:
			this->MoveWindow(&m_foldR);
			break;
		}
	}
//	SetRedraw(TRUE);
//	RedrawWindow(nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE);
//
//}

	//Invalidate();

}

void UI_Base::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

//	if (m_bFirst)
//	{
//		m_bFirst = FALSE;
//// 		LoadPos();
//		LocateControl();
//		Invalidate();
//	}
	
	if (bShow)
	{
		if (m_szTitle.IsEmpty())
			return;
		if (!m_szTitle.Compare(_T("대화 상자")))
			return;
// 		SetTimer(TIMERID_POS_RECORD, 1000, NULL);
	}
}

void UI_Base::LoadPos()
{
	// Stub: INIsimple dependency removed for standalone build
}

void UI_Base::SavePos()
{
	// Stub: INIsimple dependency removed for standalone build
}


void UI_Base::OnSize(UINT nType, int cx, int cy)
{
	m_nViewType = nType;

	CDialog::OnSize(nType, cx, cy);

	LocateControl();
	
	
	SetTimer(TIMERID_RESIZE_DELAY, 50, NULL);
	if (m_szTitle.IsEmpty())
		return;
	if (!m_szTitle.Compare(_T("대화 상자")))
		return;
// 	SetTimer(TIMERID_POS_RECORD, 1000, NULL);
}


void UI_Base::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bSysMaxiBtnUse)
	{
		if (m_rMaximize.PtInRect(point))
		{
			HCURSOR		hcursor;
			hcursor = LoadCursor(NULL, IDC_HAND);
			SetCursor(hcursor);
		}
	}

	if (m_bSysMiniBtnUse)
	{
		if (m_rMinimize.PtInRect(point))
		{
			HCURSOR		hcursor;
			hcursor = LoadCursor(NULL, IDC_HAND);
			SetCursor(hcursor);
		}
	}

	if (m_bSysExitBtnUse)
	{
		if (m_rExit.PtInRect(point))
		{
			HCURSOR		hcursor;
			hcursor = LoadCursor(NULL, IDC_HAND);
			SetCursor(hcursor);
		}
	}

	if (m_bSysFoldBtnUse)
	{
		if (m_rFold.PtInRect(point))
		{
			HCURSOR		hcursor;
			hcursor = LoadCursor(NULL, IDC_HAND);
			SetCursor(hcursor);
		}
	}

	if (m_bHelpBtnUse)
	{
		if (m_rHelp.PtInRect(point))
		{
			HCURSOR		hcursor;
			hcursor = LoadCursor(NULL, IDC_HAND);
			SetCursor(hcursor);
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}


void UI_Base::SetszDesc(CString str)
{ 
	m_szdesc = str;

	LONG	w = m_titleRect.Width();
	CRect	rdesc = m_titleRect;
	rdesc.left = w / 5;
	rdesc.right = rdesc.left + (LONG)(w / 1.6);
	m_rDesc = rdesc;

	CDC* pdc = this->GetDC();
	CBrush	bk(Utility::st_back_color3());
	CRect	rd = m_rDesc;
	rd.DeflateRect(2, 2);
	pdc->FillRect(&rd, &bk);
	Utility::uDrawText(pdc->GetSafeHdc(), m_szdesc, m_rDesc, 6, 15, UI_WHITE, TRUE, DT_CENTER);
	this->ReleaseDC(pdc);
}

void UI_Base::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent)
	{

	case TIMERID_POS_RECORD:
		KillTimer(TIMERID_POS_RECORD);
// 		SavePos();
		break;
	case TIMERID_RESIZE_DELAY:
		KillTimer(TIMERID_RESIZE_DELAY);
		Invalidate(TRUE);
	}

	CDialog::OnTimer(nIDEvent);
}


void UI_Base::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_bSysMaxiBtnUse)
	{
		if (RANGEIN(point.y, 0, TITLEBAR_HEIGHT - 1))
		{
			m_nViewType = (this->GetStyle() & WS_MAXIMIZE) ? SW_RESTORE : SW_MAXIMIZE;
			this->ShowWindow(m_nViewType);
			}
			}

	CDialog::OnLButtonDblClk(nFlags, point);
}

void UI_Base::SwitchScreenMinMax()
{
	
}


void UI_Base::SetFoldRect(CRect rect)
{
	m_foldR = rect;
	m_Curpos = m_foldR.TopLeft();
}

void UI_Base::SetExpandRect(CRect rect)
{
	m_expandR = rect;
}

void UI_Base::Show_AllCTlist(LONG bshow)
{
	if (m_vexpandCTlist.size() != 0)
	{
		for each (CTINFO info in m_vexpandCTlist)
		{
			GetDlgItem(info.nID)->ShowWindow(bshow);
		}
	}
}

void UI_Base::Helpmsg()
{
}

void UI_Base::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	//LocateControl();
	

}



