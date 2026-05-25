#include "pch.h"
#include "MainDlg.h"
#include "resource.h"
#include <algorithm>

IMPLEMENT_DYNAMIC(CMainDlg, CDialogEx)

const COLORREF CMainDlg::BG_MAIN    = RGB(0x1A, 0x1A, 0x2E);
const COLORREF CMainDlg::BG_TITLE   = RGB(0x16, 0x21, 0x3E);
const COLORREF CMainDlg::BG_TOOLBAR = RGB(0x1F, 0x34, 0x60);
const COLORREF CMainDlg::BG_LIST    = RGB(0x14, 0x1E, 0x3A);
const COLORREF CMainDlg::BG_BTNLOAD = RGB(0x2E, 0x4A, 0x8A);
const COLORREF CMainDlg::SEP        = RGB(0x2A, 0x40, 0x70);
const COLORREF CMainDlg::TXT        = RGB(0xEE, 0xEE, 0xEE);
const COLORREF CMainDlg::TXT_DIM    = RGB(0x88, 0x88, 0xAA);
const COLORREF CMainDlg::HOVER      = RGB(0x3A, 0x5B, 0xA0);
const COLORREF CMainDlg::CLOSE_HOVER= RGB(0xE7, 0x4C, 0x3C);
const COLORREF CMainDlg::ICON_COLOR = RGB(0xCC, 0xCC, 0xDD);
const COLORREF CMainDlg::ACCENT     = RGB(0x7A, 0xB7, 0xFF);
const COLORREF CMainDlg::BG_PANEL   = RGB(0x10, 0x18, 0x30);
const COLORREF CMainDlg::BG_ROW     = RGB(0x18, 0x24, 0x42);
const COLORREF CMainDlg::BG_ROW_ALT = RGB(0x16, 0x20, 0x3B);
const COLORREF CMainDlg::BG_SELECTED= RGB(0x2F, 0x62, 0xA6);

CMainDlg::CMainDlg(CWnd* pParent)
	: CDialogEx(IDD_MAIN_DIALOG, pParent)
{
	m_eHover  = HVR_NONE;
	m_bDrag   = false;
	m_bMaxed  = false;
	m_nCurIdx = -1;
	m_hBrList = nullptr;
	m_bModelLoaded = false;
	m_strModelPath = _T("D:\\forSaigeUI\\Saige\\model\\Source_Seg_Test_TrainModel.finished.srSeg");
	m_strModelName = _T("Source_Seg_Test_TrainModel.finished.srSeg");
	m_strModelStatus = _T("Model not loaded");
	m_strModelInfo = _T("Click Load Model to initialize Saige v1 segmentation model.");
}

CMainDlg::~CMainDlg()
{
	if (m_hBrList) DeleteObject(m_hBrList);
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE_LIST, m_ctlImageList);
	DDX_Control(pDX, IDC_BTN_LOAD,   m_btnLoad);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSELEAVE()
	ON_WM_TIMER()
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_BN_CLICKED(IDC_BTN_LOAD, &CMainDlg::OnBnClickedLoad)
	ON_LBN_SELCHANGE(IDC_IMAGE_LIST, &CMainDlg::OnSelList)
	ON_COMMAND(ID_LOAD_FOLDER, &CMainDlg::OnLoadFolderCmd)
	ON_COMMAND(ID_LOAD_FILE,   &CMainDlg::OnLoadFileCmd)
END_MESSAGE_MAP()

BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ModifyStyle(WS_CAPTION | WS_SYSMENU, 0);
	ModifyStyleEx(WS_EX_DLGMODALFRAME, 0);

	m_fontTitle.CreateFont(26, 0,0,0, FW_BOLD,   FALSE,FALSE,0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, _T("Segoe UI"));
	m_fontTime .CreateFont(20, 0,0,0, FW_NORMAL, FALSE,FALSE,0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, _T("Segoe UI"));
	m_fontBtn  .CreateFont(20, 0,0,0, FW_NORMAL, FALSE,FALSE,0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, _T("Segoe UI"));
	m_fontList .CreateFont(20, 0,0,0, FW_NORMAL,FALSE,FALSE,0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, _T("Segoe UI"));

	m_btnLoad.SetFont(&m_fontBtn);
	m_btnLoad.ShowWindow(SW_HIDE);
	m_ctlImageList.SetFont(&m_fontList);
	m_ctlImageList.SetHorizontalExtent(900);
	m_hBrList = CreateSolidBrush(BG_LIST);

	SetTimer(1, 1000, nullptr);
	GetWindowRect(&m_rcRestore);
	ApplyFullScreen();

	return TRUE;
}

void CMainDlg::ApplyFullScreen()
{
	SetWindowPos(nullptr, 0,0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOZORDER);
	m_bMaxed = true;
}

void CMainDlg::ToggleMax()
{
	if (m_bMaxed)
	{
		SetWindowPos(nullptr, m_rcRestore.left, m_rcRestore.top,
			m_rcRestore.Width(), m_rcRestore.Height(), SWP_NOZORDER);
	}
	else
	{
		GetWindowRect(&m_rcRestore);
		ApplyFullScreen();
	}
	m_bMaxed = !m_bMaxed;
	Invalidate(FALSE);
}

CMainDlg::HoverBtn CMainDlg::HitTest(CPoint pt)
{
	if (m_rcModelLoad.PtInRect(pt)) return HVR_MODEL_LOAD;
	if (m_rcLoad .PtInRect(pt)) return HVR_LOAD;
	if (m_rcClose.PtInRect(pt)) return HVR_CLOSE;
	if (m_rcMax  .PtInRect(pt)) return HVR_MAX;
	if (m_rcMin  .PtInRect(pt)) return HVR_MIN;
	return HVR_NONE;
}

LRESULT CMainDlg::OnNcHitTest(CPoint pt)
{
	LRESULT hit = CDialogEx::OnNcHitTest(pt);
	if (hit == HTCLIENT)
	{
		ScreenToClient(&pt);
		// Always let button areas receive normal WM_LBUTTONDOWN
		if (m_rcClose.PtInRect(pt)) return HTCLIENT;
		if (m_rcMax  .PtInRect(pt)) return HTCLIENT;
		if (m_rcMin  .PtInRect(pt)) return HTCLIENT;
		// All other titlebar area -> drag via HTCAPTION
		if (pt.y < BAR_HEIGHT) return HTCAPTION;
	}
	return hit;
}

void CMainDlg::RecRects()
{
	CRect rc; GetClientRect(&rc);
	m_rcLogo = CRect(0, 0, rc.right, BAR_HEIGHT);
	m_rcTool = CRect(0, BAR_HEIGHT, rc.right, BAR_HEIGHT + TOOLBAR_H);

	int r = rc.right;
	int y = (BAR_HEIGHT - BTN_H) / 2;
	m_rcClose = CRect(r - 64,  y, r - 12,   y + BTN_H);
	m_rcMax   = CRect(r - 126, y, r - 72,   y + BTN_H);
	m_rcMin   = CRect(r - 188, y, r - 134,  y + BTN_H);
	m_rcLoad  = CRect(24, BAR_HEIGHT + 22, 190, BAR_HEIGHT + TOOLBAR_H - 22);

	int bodyTop = BAR_HEIGHT + TOOLBAR_H;
	int viewW = (int)(rc.Width() * 0.7f);
	m_rcRightPanel = CRect(viewW, bodyTop, rc.right, rc.bottom);

	int rightH = m_rcRightPanel.Height();
	int margin = 20;
	int listH = (std::max)(210, (int)(rightH * 0.30f));
	int modelH = (std::max)(280, (int)(rightH * 0.40f));

	m_rcImageListPanel = CRect(m_rcRightPanel.left, m_rcRightPanel.top,
		m_rcRightPanel.right, (std::min)(m_rcRightPanel.top + listH, m_rcRightPanel.bottom));
	m_rcModelPanel = CRect(m_rcRightPanel.left, m_rcImageListPanel.bottom,
		m_rcRightPanel.right, (std::min)(m_rcImageListPanel.bottom + modelH, m_rcRightPanel.bottom));
	m_rcBlankPanel = CRect(m_rcRightPanel.left, m_rcModelPanel.bottom,
		m_rcRightPanel.right, m_rcRightPanel.bottom);

	m_rcImageListFrame = CRect(m_rcImageListPanel.left + margin, m_rcImageListPanel.top + 82,
		m_rcImageListPanel.right - margin, m_rcImageListPanel.bottom - 14);
	m_rcModelLoad = CRect(m_rcModelPanel.left + margin, m_rcModelPanel.top + 78,
		m_rcModelPanel.right - margin, m_rcModelPanel.top + 132);
}

void CMainDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (!IsWindow(m_ctlImageList.GetSafeHwnd())) return;

	RecRects();
	m_ctlImageList.MoveWindow(&m_rcImageListFrame);
	Invalidate(FALSE);
}

BOOL CMainDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CMainDlg::DrawLogo(CDC& dc)
{
	dc.FillSolidRect(&m_rcLogo, BG_TITLE);

	CFont* pf = dc.SelectObject(&m_fontTitle);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(TXT);
	CRect rcT(20, 0, m_rcLogo.right - 220, BAR_HEIGHT);
	dc.DrawText(_T("Labeling Assistant"), &rcT, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	SYSTEMTIME st; GetLocalTime(&st);
	CString strTm; strTm.Format(_T("%02d:%02d:%02d"), st.wHour, st.wMinute, st.wSecond);
	dc.SelectObject(&m_fontTime);
	CRect rcTime(rcT.right + 20, 0, m_rcLogo.right - 220, BAR_HEIGHT);
	dc.SetTextColor(TXT_DIM);
	dc.DrawText(strTm, &rcTime, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	dc.SelectObject(pf);

	DrawIcon(dc, m_rcMin,   0);
	DrawIcon(dc, m_rcMax,   m_bMaxed ? 3 : 1);
	DrawIcon(dc, m_rcClose, 2);
}

void CMainDlg::DrawIcon(CDC& dc, CRect rc, int type)
{
	bool bHover = false;
	if      (type == 0)           bHover = (m_eHover == HVR_MIN);
	else if (type == 1 || type == 3) bHover = (m_eHover == HVR_MAX);
	else if (type == 2)           bHover = (m_eHover == HVR_CLOSE);

	if (bHover)
		dc.FillSolidRect(&rc, (type == 2) ? CLOSE_HOVER : HOVER);

	COLORREF clr = (type == 2 && bHover) ? TXT : ICON_COLOR;
	CPen pen(PS_SOLID, 3, clr);
	CPen* pp = dc.SelectObject(&pen);
	int p = 16;

	if (type == 0)
	{
		int my = rc.CenterPoint().y;
		dc.MoveTo(rc.left + p, my);
		dc.LineTo(rc.right - p, my);
	}
	else if (type == 1)
	{
		dc.Rectangle(rc.left + p, rc.top + 12, rc.right - p, rc.bottom - 12);
	}
	else if (type == 2)
	{
		int pad = 18;
		dc.MoveTo(rc.left + pad, rc.top + pad);
		dc.LineTo(rc.right - pad + 2, rc.bottom - pad + 2);
		dc.MoveTo(rc.right - pad + 2, rc.top + pad);
		dc.LineTo(rc.left + pad, rc.bottom - pad + 2);
	}
	else if (type == 3)
	{
		int p2 = 14, ov = 6;
		CRect back(rc.left + p2, rc.top + 12 + ov, rc.right - p2 - ov, rc.bottom - 12);
		CRect frnt(rc.left + p2 + ov, rc.top + 12, rc.right - p2, rc.bottom - 12 - ov);

		dc.MoveTo(back.left, back.top);
		dc.LineTo(back.right, back.top);
		dc.LineTo(back.right, back.bottom);
		dc.LineTo(back.left, back.bottom);
		dc.LineTo(back.left, back.top);

		dc.MoveTo(frnt.left, frnt.top);
		dc.LineTo(frnt.right, frnt.top);
		dc.LineTo(frnt.right, frnt.bottom);
		dc.LineTo(frnt.left, frnt.bottom);
		dc.LineTo(frnt.left, frnt.top);

		CBrush br(BG_TITLE);
		CBrush* pb = dc.SelectObject(&br);
		CRect er(frnt.left + 1, frnt.top - 2, frnt.right, frnt.top + 3);
		dc.FillRect(&er, &br);
		dc.SelectObject(pb);
	}

	dc.SelectObject(pp);
}

void CMainDlg::DrawTool(CDC& dc)
{
	dc.FillSolidRect(&m_rcTool, BG_TOOLBAR);

	CFont* pf = dc.SelectObject(&m_fontBtn);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(TXT_DIM);
	CRect rcHint(m_rcLoad.right + 22, m_rcTool.top, m_rcTool.right - 20, m_rcTool.bottom);
	dc.DrawText(_T("Load images from a folder or selected files"), &rcHint, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	dc.SelectObject(pf);

	DrawLoadButton(dc);

	CPen pSep(PS_SOLID, 2, SEP);
	CPen* pp = dc.SelectObject(&pSep);
	dc.MoveTo(0, m_rcTool.bottom - 2);
	dc.LineTo(m_rcTool.right, m_rcTool.bottom - 2);
	dc.SelectObject(pp);
}

void CMainDlg::DrawLoadButton(CDC& dc)
{
	CRect rc = m_rcLoad;
	COLORREF bg = (m_eHover == HVR_LOAD) ? HOVER : BG_BTNLOAD;
	CBrush br(bg);
	CBrush* oldBr = dc.SelectObject(&br);
	CPen pen(PS_SOLID, 1, RGB(0x5F, 0x88, 0xC8));
	CPen* oldPen = dc.SelectObject(&pen);
	dc.RoundRect(&rc, CPoint(14, 14));
	dc.SelectObject(oldPen);
	dc.SelectObject(oldBr);

	CFont* pf = dc.SelectObject(&m_fontBtn);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(TXT);
	CRect rcText = rc;
	rcText.left += 20;
	rcText.right -= 38;
	dc.DrawText(_T("Load"), &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	dc.SelectObject(pf);

	CPen arrowPen(PS_SOLID, 2, TXT);
	CPen* oldArrow = dc.SelectObject(&arrowPen);
	int cx = rc.right - 24;
	int cy = rc.CenterPoint().y + 1;
	dc.MoveTo(cx - 5, cy - 3);
	dc.LineTo(cx, cy + 3);
	dc.LineTo(cx + 5, cy - 3);
	dc.SelectObject(oldArrow);
}

void CMainDlg::DrawModelLoadButton(CDC& dc)
{
	CRect rc = m_rcModelLoad;
	COLORREF bg = (m_eHover == HVR_MODEL_LOAD) ? HOVER : RGB(0x35, 0x67, 0xA8);
	CBrush br(bg);
	CBrush* oldBr = dc.SelectObject(&br);
	CPen pen(PS_SOLID, 1, ACCENT);
	CPen* oldPen = dc.SelectObject(&pen);
	dc.RoundRect(&rc, CPoint(14, 14));
	dc.SelectObject(oldPen);
	dc.SelectObject(oldBr);

	CFont* oldFont = dc.SelectObject(&m_fontBtn);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(TXT);
	CRect textRc = rc;
	textRc.left += 20;
	dc.DrawText(_T("Load Model"), &textRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	dc.SelectObject(oldFont);
}

void CMainDlg::DrawModelPanel(CDC& dc)
{
	CRect rcTitle(m_rcModelPanel.left + 22, m_rcModelPanel.top + 22,
		m_rcModelPanel.right - 22, m_rcModelPanel.top + 68);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(TXT);
	CFont* oldFont = dc.SelectObject(&m_fontTitle);
	dc.DrawText(_T("Saige Model Load"), &rcTitle, DT_LEFT | DT_TOP | DT_SINGLELINE);
	dc.SelectObject(oldFont);

	DrawModelLoadButton(dc);

	CRect rcInfo(m_rcModelPanel.left + 22, m_rcModelLoad.bottom + 22,
		m_rcModelPanel.right - 22, m_rcModelPanel.bottom - 22);
	CBrush br(BG_LIST);
	CBrush* oldBr = dc.SelectObject(&br);
	CPen pen(PS_SOLID, 1, SEP);
	CPen* oldPen = dc.SelectObject(&pen);
	dc.RoundRect(&rcInfo, CPoint(12, 12));
	dc.SelectObject(oldPen);
	dc.SelectObject(oldBr);

	CFont* infoFont = dc.SelectObject(&m_fontBtn);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(m_bModelLoaded ? ACCENT : TXT_DIM);
	CRect line = rcInfo;
	line.DeflateRect(18, 14);
	line.bottom = line.top + 28;
	dc.DrawText(m_strModelStatus, &line, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

	dc.SetTextColor(TXT);
	line.OffsetRect(0, 34);
	CString name;
	name.Format(_T("File: %s"), (LPCTSTR)m_strModelName);
	dc.DrawText(name, &line, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

	dc.SetTextColor(TXT_DIM);
	line.OffsetRect(0, 34);
	dc.DrawText(m_strModelInfo, &line, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
	dc.SelectObject(infoFont);
}

void CMainDlg::DrawBody(CDC& dc)
{
	CRect rc; GetClientRect(&rc);
	CRect rcBody(0, BAR_HEIGHT + TOOLBAR_H, rc.right, rc.bottom);
	int vw = (int)(rcBody.Width() * 0.7f);
	CRect rcView(0, rcBody.top, vw, rcBody.bottom);

	dc.FillSolidRect(&rcView, BG_MAIN);
	dc.FillSolidRect(&m_rcRightPanel, BG_PANEL);

	CPen p(PS_SOLID, 1, SEP);
	CPen* pp = dc.SelectObject(&p);
	dc.MoveTo(vw, rcBody.top);
	dc.LineTo(vw, rcBody.bottom);
	dc.MoveTo(m_rcImageListPanel.left + 20, m_rcImageListPanel.bottom);
	dc.LineTo(m_rcImageListPanel.right - 20, m_rcImageListPanel.bottom);
	dc.MoveTo(m_rcModelPanel.left + 20, m_rcModelPanel.bottom);
	dc.LineTo(m_rcModelPanel.right - 20, m_rcModelPanel.bottom);
	dc.SelectObject(pp);

	CRect rcHeader(m_rcImageListPanel.left + 22, m_rcImageListPanel.top + 20,
		m_rcImageListPanel.right - 22, m_rcImageListPanel.top + 74);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(TXT);
	CFont* pfHdr = dc.SelectObject(&m_fontTitle);
	dc.DrawText(_T("Image List"), &rcHeader, DT_LEFT | DT_TOP | DT_SINGLELINE);
	dc.SelectObject(&m_fontBtn);
	dc.SetTextColor(TXT_DIM);
	CString strCount;
	strCount.Format(_T("%d image(s)"), (int)m_vPaths.size());
	CRect rcCnt(rcHeader.left, rcHeader.top + 34, rcHeader.right, rcHeader.bottom);
	dc.DrawText(strCount, &rcCnt, DT_LEFT | DT_TOP | DT_SINGLELINE);
	dc.SelectObject(pfHdr);

	CBrush brList(BG_LIST);
	CBrush* pOldBr = dc.SelectObject(&brList);
	CPen penList(PS_SOLID, 1, SEP);
	CPen* pOldPen = dc.SelectObject(&penList);
	dc.RoundRect(&m_rcImageListFrame, CPoint(12, 12));
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBr);

	DrawModelPanel(dc);

	if (!m_cvDisplay.empty())
	{
		CRect ri = rcView; ri.DeflateRect(4, 4);
		float sx = (float)ri.Width()  / m_cvDisplay.cols;
		float sy = (float)ri.Height() / m_cvDisplay.rows;
		float sc = (std::min)(sx, sy);
		int dw = (int)(m_cvDisplay.cols * sc);
		int dh = (int)(m_cvDisplay.rows * sc);
		int dx = ri.left + (ri.Width()  - dw) / 2;
		int dy = ri.top  + (ri.Height() - dh) / 2;

		BITMAPINFO bi = {};
		bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bi.bmiHeader.biWidth  = m_cvDisplay.cols;
		bi.bmiHeader.biHeight = -m_cvDisplay.rows;
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biBitCount = m_cvDisplay.channels() * 8;
		bi.bmiHeader.biCompression = BI_RGB;

		CDC dcImg; dcImg.CreateCompatibleDC(&dc);
		void* bits = nullptr;
		HBITMAP hb = CreateDIBSection(dcImg.GetSafeHdc(), &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
		if (hb && bits)
		{
			int nB = m_cvDisplay.cols * m_cvDisplay.rows * m_cvDisplay.channels();
			memcpy(bits, m_cvDisplay.data, nB);
			CBitmap* pOld2 = dcImg.SelectObject(CBitmap::FromHandle(hb));
			dc.SetStretchBltMode(COLORONCOLOR);
			dc.StretchBlt(dx, dy, dw, dh, &dcImg, 0, 0, m_cvDisplay.cols, m_cvDisplay.rows, SRCCOPY);
			dcImg.SelectObject(pOld2);
			DeleteObject(hb);
		}
		dcImg.DeleteDC();
	}
	else
	{
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(TXT_DIM);
		CFont* pf2 = dc.SelectObject(&m_fontTitle);
		dc.DrawText(_T("No Image"), &rcView, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		dc.SelectObject(pf2);
	}
}

void CMainDlg::OnPaint()
{
	CPaintDC dc(this);
	CRect rc; GetClientRect(&rc);

	CDC mem; mem.CreateCompatibleDC(&dc);
	CBitmap bmp; bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap* pOld = mem.SelectObject(&bmp);

	DrawLogo(mem);
	DrawTool(mem);
	DrawBody(mem);

	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &mem, 0, 0, SRCCOPY);
	mem.SelectObject(pOld);
	mem.DeleteDC();
}

void CMainDlg::OnLButtonDown(UINT, CPoint pt)
{
	HoverBtn h = HitTest(pt);
	if (h == HVR_MODEL_LOAD) { LoadSaigeModel(); return; }
	if (h == HVR_LOAD)  { ShowLoadMenu(); return; }
	if (h == HVR_MIN)   { ShowWindow(SW_MINIMIZE); return; }
	if (h == HVR_MAX)   { ToggleMax(); return; }
	if (h == HVR_CLOSE) { EndDialog(IDOK); return; }

	if (pt.y < BAR_HEIGHT)
	{
		m_bDrag = true; m_ptDrag = pt;
		SetCapture(); return;
	}
	CDialogEx::OnLButtonDown(0, pt);
}

void CMainDlg::OnLButtonUp(UINT, CPoint)
{
	if (m_bDrag) { m_bDrag = false; ReleaseCapture(); }
	CDialogEx::OnLButtonUp(0, CPoint(0, 0));
}

void CMainDlg::OnMouseMove(UINT, CPoint pt)
{
	if (m_bDrag)
	{
		CRect rc; GetWindowRect(&rc);
		int dx = pt.x - m_ptDrag.x, dy = pt.y - m_ptDrag.y;

		if (m_bMaxed && (abs(dx) > 8 || abs(dy) > 8))
		{
			ToggleMax();
			m_ptDrag = pt; ClientToScreen(&m_ptDrag);
			CRect rn; GetWindowRect(&rn);
			SetWindowPos(nullptr, rn.left + dx, rn.top + dy, rn.Width(), rn.Height(), SWP_NOZORDER);
			return;
		}
		SetWindowPos(nullptr, rc.left + dx, rc.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		return;
	}

	HoverBtn h = HitTest(pt);
	if (h != m_eHover)
	{
		m_eHover = h;
		TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, m_hWnd, 0 };
		TrackMouseEvent(&tme);
		InvalidateRect(&m_rcLogo, FALSE);
		InvalidateRect(&m_rcTool, FALSE);
		InvalidateRect(&m_rcModelPanel, FALSE);
	}
	CDialogEx::OnMouseMove(0, pt);
}

void CMainDlg::OnMouseLeave()
{
	if (m_eHover != HVR_NONE)
	{
		m_eHover = HVR_NONE;
		InvalidateRect(&m_rcLogo, FALSE);
		InvalidateRect(&m_rcTool, FALSE);
		InvalidateRect(&m_rcModelPanel, FALSE);
	}
	CDialogEx::OnMouseLeave();
}

HBRUSH CMainDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (pWnd && pWnd->GetDlgCtrlID() == IDC_IMAGE_LIST)
	{
		pDC->SetBkColor(BG_LIST);
		pDC->SetTextColor(TXT);
		return m_hBrList;
	}
	return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CMainDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (nIDCtl == IDC_IMAGE_LIST)
	{
		lpMeasureItemStruct->itemHeight = 42;
		return;
	}
	CDialogEx::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CMainDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl != IDC_IMAGE_LIST)
	{
		CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
		return;
	}

	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	CRect rc(lpDrawItemStruct->rcItem);
	bool selected = (lpDrawItemStruct->itemState & ODS_SELECTED) != 0;
	bool odd = (lpDrawItemStruct->itemID % 2) != 0;
	COLORREF bg = selected ? BG_SELECTED : (odd ? BG_ROW_ALT : BG_ROW);
	dc.FillSolidRect(&rc, bg);

	if (lpDrawItemStruct->itemID != (UINT)-1)
	{
		CString text;
		m_ctlImageList.GetText(lpDrawItemStruct->itemID, text);
		CFont* oldFont = dc.SelectObject(&m_fontList);
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(selected ? RGB(255, 255, 255) : TXT);
		CRect txtRc = rc;
		txtRc.left += 18;
		txtRc.right -= 12;
		dc.DrawText(text, &txtRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
		dc.SelectObject(oldFont);
	}

	if (selected)
	{
		CPen pen(PS_SOLID, 2, ACCENT);
		CPen* oldPen = dc.SelectObject(&pen);
		dc.MoveTo(rc.left + 6, rc.top + 8);
		dc.LineTo(rc.left + 6, rc.bottom - 8);
		dc.SelectObject(oldPen);
	}

	dc.Detach();
}

void CMainDlg::OnTimer(UINT_PTR nID)
{
	if (nID == 1 && IsWindow(m_hWnd))
		InvalidateRect(&m_rcLogo, FALSE);
	CDialogEx::OnTimer(nID);
}

void CMainDlg::OnLButtonDblClk(UINT, CPoint pt)
{
	if (pt.y < BAR_HEIGHT && HitTest(pt) == HVR_NONE)
	{
		ToggleMax(); return;
	}
	CDialogEx::OnLButtonDblClk(0, pt);
}

void CMainDlg::OnBnClickedLoad()

{
	ShowLoadMenu();
}

void CMainDlg::ShowLoadMenu()
{
	CMenu m; m.CreatePopupMenu();
	m.AppendMenu(MF_STRING, ID_LOAD_FOLDER, _T("  Folder"));
	m.AppendMenu(MF_STRING, ID_LOAD_FILE,   _T("  File"));
	CRect rc = m_rcLoad;
	ClientToScreen(&rc);
	m.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN, rc.left, rc.bottom + 4, this);
}

void CMainDlg::OnLoadFolderCmd() { LoadFolder(); }
void CMainDlg::OnLoadFileCmd()   { LoadFiles(); }

int CMainDlg::SafeLoadSaigeModel()
{
	int ret = UNKNOWN_ERROR;
	__try
	{
		ret = m_saigeSeg.LoadModel(m_strModelPath.GetString());
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		ret = NO_LICENSE_DONGLE;
	}
	return ret;
}

void CMainDlg::LoadSaigeModel()
{
	if (GetFileAttributes(m_strModelPath) == INVALID_FILE_ATTRIBUTES)
	{
		m_bModelLoaded = false;
		m_strModelStatus = _T("Model file not found");
		m_strModelInfo.Format(_T("Path: %s"), (LPCTSTR)m_strModelPath);
		InvalidateRect(&m_rcModelPanel, FALSE);
		return;
	}

	CWaitCursor wait;
	m_strModelStatus = _T("Loading model...");
	m_strModelInfo = _T("Initializing Saige v1 segmentation handle...");
	InvalidateRect(&m_rcModelPanel, FALSE);
	UpdateWindow();

	int ret = SafeLoadSaigeModel();
	m_bModelLoaded = (ret == NO_ERROR_OUT);

	if (m_bModelLoaded)
	{
		m_strModelStatus = _T("Model loaded successfully");
		m_strModelInfo.Format(_T("Type: Segmentation v1 | Classes: %d"), m_saigeSeg.GetClassCount());
	}
	else
	{
		m_strModelStatus = _T("Model load failed");
		m_strModelInfo.Format(_T("Error: %s (%d) | Classes: %d"),
			(LPCTSTR)m_saigeSeg.GetErrorInfo(ret), ret, m_saigeSeg.GetClassCount());

		if (ret == NO_LICENSE_DONGLE || ret == NO_LICENCE)
		{
			AfxMessageBox(_T("USB Dongle Not Detected"), MB_ICONWARNING | MB_OK);
		}
	}

	InvalidateRect(&m_rcModelPanel, FALSE);
}

void CMainDlg::LoadFolder()
{
	BROWSEINFO bi = {}; bi.hwndOwner = m_hWnd;
	bi.lpszTitle = _T("Select Image Folder");
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl)
	{
		TCHAR sz[MAX_PATH];
		if (SHGetPathFromIDList(pidl, sz)) LoadFromFolder(CString(sz));
		CoTaskMemFree(pidl);
	}
}

void CMainDlg::LoadFiles()
{
	CFileDialog dlg(TRUE, nullptr, nullptr,
		OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY,
		_T("Images (*.bmp;*.jpg;*.jpeg;*.png)|*.bmp;*.jpg;*.jpeg;*.png|All (*.*)|*.*||"), this);

	const int BUF = 32768;
	TCHAR* pBuf = new TCHAR[BUF]; pBuf[0] = 0;
	dlg.m_ofn.lpstrFile = pBuf; dlg.m_ofn.nMaxFile = BUF;

	if (dlg.DoModal() == IDOK)
	{
		std::vector<CString> v;
		POSITION p = dlg.GetStartPosition();
		while (p) v.push_back(dlg.GetNextPathName(p));
		LoadFromFiles(v);
	}
	delete[] pBuf;
}

void CMainDlg::LoadFromFolder(const CString& strFolder)
{
	CString pat = strFolder + _T("\\*.*"); CFileFind ff;
	std::vector<CString> v;
	for (BOOL b = ff.FindFile(pat); b; b = ff.FindNextFile())
	{
		if (ff.IsDots() || ff.IsDirectory()) continue;
		CString nm = ff.GetFileName(); nm.MakeLower();
		if (nm.Find(_T(".png")) >= 0 || nm.Find(_T(".bmp")) >= 0 ||
			nm.Find(_T(".jpg")) >= 0 || nm.Find(_T(".jpeg")) >= 0)
			v.push_back(ff.GetFilePath());
	}
	std::sort(v.begin(), v.end());
	LoadFromFiles(v);
}

void CMainDlg::LoadFromFiles(const std::vector<CString>& vFiles)
{
	m_vPaths.clear(); m_vImgs.clear(); m_ctlImageList.ResetContent();
	for (const auto& sp : vFiles)
	{
		cv::Mat im = cv::imread(std::string(CT2A(sp)), cv::IMREAD_UNCHANGED);
		if (!im.empty()) { m_vPaths.push_back(sp); m_vImgs.push_back(im); }
	}
	UpdList();
	if (!m_vImgs.empty()) SelImage(0);
	else { m_cvDisplay.release(); Invalidate(FALSE); }
}

void CMainDlg::UpdList()
{
	for (int i = 0; i < (int)m_vPaths.size(); ++i)
	{
		const CString fileName = m_vPaths[i].Mid(m_vPaths[i].ReverseFind(_T('\\')) + 1);
		CString displayText;
		displayText.Format(_T("%d. %s"), i + 1, fileName.GetString());
		m_ctlImageList.AddString(displayText);
	}
}

void CMainDlg::SelImage(int nIdx)
{
	if (nIdx < 0 || nIdx >= (int)m_vImgs.size()) return;
	m_nCurIdx = nIdx; m_ctlImageList.SetCurSel(nIdx);

	cv::Mat& im = m_vImgs[nIdx];
	if (im.channels() == 1)      cv::cvtColor(im, m_cvDisplay, cv::COLOR_GRAY2BGR);
	else if (im.channels() == 3) m_cvDisplay = im.clone();
	else if (im.channels() == 4) cv::cvtColor(im, m_cvDisplay, cv::COLOR_BGRA2BGR);
	Invalidate(FALSE);
}

void CMainDlg::OnSelList()
{
	int n = m_ctlImageList.GetCurSel();
	if (n >= 0) SelImage(n);
}
