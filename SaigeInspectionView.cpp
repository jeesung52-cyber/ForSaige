#include "pch.h"
#include "SaigeInspectionView.h"
#include "SaigeInspectionDoc.h"
#include <gdiplus.h>

using namespace Gdiplus;

IMPLEMENT_DYNCREATE(CSaigeInspectionView, CFormView)

BEGIN_MESSAGE_MAP(CSaigeInspectionView, CFormView)
	ON_BN_CLICKED(IDC_BTN_LOAD_IMAGE, &CSaigeInspectionView::OnBnClickedLoadImage)
	ON_BN_CLICKED(IDC_BTN_LOAD_MODEL, &CSaigeInspectionView::OnBnClickedLoadModel)
	ON_BN_CLICKED(IDC_BTN_RUN_INSPECTION, &CSaigeInspectionView::OnBnClickedRunInspection)
	ON_WM_PAINT()
END_MESSAGE_MAP()

CSaigeInspectionView::CSaigeInspectionView() noexcept
	: CFormView(IDD_SAIGEINSPECTION_FORM)
{
	m_strImagePath = _T("");
	m_strModelPath = _T("");
	m_bModelLoaded = false;
}

CSaigeInspectionView::~CSaigeInspectionView()
{
	m_saigeSeg.CloseModel();
}

void CSaigeInspectionView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE_DISPLAY, m_ctlImageDisplay);
	DDX_Control(pDX, IDC_BTN_LOAD_IMAGE, m_btnLoadImage);
	DDX_Control(pDX, IDC_BTN_LOAD_MODEL, m_btnLoadModel);
	DDX_Control(pDX, IDC_BTN_RUN_INSPECTION, m_btnRunInspection);
	DDX_Control(pDX, IDC_STATIC_IMAGE_PATH, m_staticImagePath);
	DDX_Control(pDX, IDC_STATIC_MODEL_PATH, m_staticModelPath);
	DDX_Control(pDX, IDC_STATIC_RESULT, m_staticResult);
	DDX_Control(pDX, IDC_STATIC_CLASS_COUNT, m_staticClassCount);
	DDX_Control(pDX, IDC_STATIC_INFO, m_staticInfo);
	DDX_Control(pDX, IDC_LIST_RESULTS, m_listResults);
}

BOOL CSaigeInspectionView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFormView::PreCreateWindow(cs);
}

void CSaigeInspectionView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	UpdateUIState();
}

void CSaigeInspectionView::OnPaint()
{
	CPaintDC dc(this);

	if (!m_cvDisplayImage.empty())
	{
		CRect rcDisplay;
		m_ctlImageDisplay.GetWindowRect(&rcDisplay);
		ScreenToClient(&rcDisplay);

		CDC* pDC = m_ctlImageDisplay.GetDC();

		CRect rcClient;
		m_ctlImageDisplay.GetClientRect(&rcClient);

		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		CBitmap bitmap;

		BITMAPINFO bmi = {};
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = m_cvDisplayImage.cols;
		bmi.bmiHeader.biHeight = -m_cvDisplayImage.rows;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = (WORD)(m_cvDisplayImage.channels() * 8);
		bmi.bmiHeader.biCompression = BI_RGB;

		void* pBits = nullptr;
		HBITMAP hBitmap = CreateDIBSection(memDC.GetSafeHdc(), &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
		if (hBitmap && pBits)
		{
			int nBytesPerLine = m_cvDisplayImage.cols * m_cvDisplayImage.channels();
			memcpy(pBits, m_cvDisplayImage.data, nBytesPerLine * m_cvDisplayImage.rows);

			CBitmap* pOldBitmap = memDC.SelectObject(CBitmap::FromHandle(hBitmap));

			float fScaleX = (float)rcClient.Width() / m_cvDisplayImage.cols;
			float fScaleY = (float)rcClient.Height() / m_cvDisplayImage.rows;
			float fScale = min(fScaleX, fScaleY);

			int nDstW = (int)(m_cvDisplayImage.cols * fScale);
			int nDstH = (int)(m_cvDisplayImage.rows * fScale);
			int nDstX = (rcClient.Width() - nDstW) / 2;
			int nDstY = (rcClient.Height() - nDstH) / 2;

			COLORREF bgColor = RGB(50, 50, 50);
			pDC->FillSolidRect(&rcClient, bgColor);

			pDC->SetStretchBltMode(COLORONCOLOR);
			pDC->StretchBlt(nDstX, nDstY, nDstW, nDstH,
				&memDC, 0, 0, m_cvDisplayImage.cols, m_cvDisplayImage.rows, SRCCOPY);

			memDC.SelectObject(pOldBitmap);
			::DeleteObject(hBitmap);
		}

		memDC.DeleteDC();
		m_ctlImageDisplay.ReleaseDC(pDC);
	}
	else
	{
		CRect rcClient;
		m_ctlImageDisplay.GetClientRect(&rcClient);
		CClientDC dcDisplay(&m_ctlImageDisplay);
		dcDisplay.FillSolidRect(&rcClient, RGB(50, 50, 50));
		dcDisplay.SetTextColor(RGB(150, 150, 150));
		dcDisplay.SetBkMode(TRANSPARENT);
		dcDisplay.DrawText(_T("이미지를 로드하세요"), &rcClient, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
}

void CSaigeInspectionView::OnBnClickedLoadImage()
{
	CFileDialog dlg(TRUE, _T("bmp"), nullptr,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("Image Files (*.bmp;*.jpg;*.jpeg;*.png;*.tif;*.tiff)|*.bmp;*.jpg;*.jpeg;*.png;*.tif;*.tiff|All Files (*.*)|*.*||"),
		this);

	if (dlg.DoModal() == IDOK)
	{
		m_strImagePath = dlg.GetPathName();
		m_staticImagePath.SetWindowText(m_strImagePath);

		std::string strPath = CT2A(m_strImagePath);
		m_cvInputImage = cv::imread(strPath, cv::IMREAD_UNCHANGED);
		if (!m_cvInputImage.empty())
		{
			if (m_cvInputImage.channels() == 1)
				cv::cvtColor(m_cvInputImage, m_cvDisplayImage, cv::COLOR_GRAY2BGR);
			else
				m_cvDisplayImage = m_cvInputImage.clone();

			CString strInfo;
			strInfo.Format(_T("Size: %d x %d, Channels: %d"),
				m_cvInputImage.cols, m_cvInputImage.rows, m_cvInputImage.channels());
			m_staticInfo.SetWindowText(strInfo);

			InvalidateRect(nullptr);
		}
		else
		{
			AfxMessageBox(_T("이미지 로드 실패"), MB_ICONERROR);
		}
	}

	UpdateUIState();
}

void CSaigeInspectionView::OnBnClickedLoadModel()
{
	CFileDialog dlg(TRUE, _T("smodel"), nullptr,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("Saige Model Files (*.smodel)|*.smodel|All Files (*.*)|*.*||"),
		this);

	if (dlg.DoModal() == IDOK)
	{
		m_strModelPath = dlg.GetPathName();
		m_staticModelPath.SetWindowText(m_strModelPath);

		CWaitCursor wait;
		int nRet = m_saigeSeg.LoadModel(m_strModelPath.GetString());

		if (nRet == 0)
		{
			m_bModelLoaded = true;
			CString strMsg;
			strMsg.Format(_T("Classes: %d"), m_saigeSeg.GetClassCount());
			m_staticClassCount.SetWindowText(strMsg);
			m_staticResult.SetWindowText(_T("Model loaded OK"));
		}
		else
		{
			m_bModelLoaded = false;
			CString strMsg = m_saigeSeg.GetErrorInfo(nRet);
			AfxMessageBox(_T("Model 로드 실패: ") + strMsg, MB_ICONERROR);
		}
	}

	UpdateUIState();
}

void CSaigeInspectionView::OnBnClickedRunInspection()
{
	if (m_cvInputImage.empty())
	{
		AfxMessageBox(_T("먼저 이미지를 로드하세요"), MB_ICONWARNING);
		return;
	}

	if (!m_bModelLoaded)
	{
		AfxMessageBox(_T("먼저 모델을 로드하세요"), MB_ICONWARNING);
		return;
	}

	m_listResults.ResetContent();

	std::vector<stAIBLOBClassInfo> vtBlobs;

	CWaitCursor wait;
	DWORD dwStart = GetTickCount();
	int nRet = m_saigeSeg.RunInspection(m_cvInputImage, vtBlobs);
	DWORD dwElapsed = GetTickCount() - dwStart;

	if (nRet == 0)
	{
		if (m_cvInputImage.channels() == 1)
			cv::cvtColor(m_cvInputImage, m_cvDisplayImage, cv::COLOR_GRAY2BGR);
		else
			m_cvDisplayImage = m_cvInputImage.clone();

		DrawBlobOverlay(m_cvDisplayImage, vtBlobs);

		CString strResult;
		strResult.Format(_T("Inspection Complete | Time: %dms | Defects: %d"),
			dwElapsed, (int)vtBlobs.size());
		m_staticResult.SetWindowText(strResult);

		for (size_t i = 0; i < vtBlobs.size(); i++)
		{
			CString strItem;
			strItem.Format(_T("[%d] Class%d | Score:%.1f | Area:%d | Center:(%d,%d)"),
				(int)i,
				vtBlobs[i].class_id,
				vtBlobs[i].segmentscore[0],
				vtBlobs[i].area,
				vtBlobs[i].center_x,
				vtBlobs[i].center_y);
			m_listResults.AddString(strItem);
		}

		InvalidateRect(nullptr);
	}
	else
	{
		CString strMsg = m_saigeSeg.GetErrorInfo(nRet);
		m_staticResult.SetWindowText(_T("Inspection Failed: ") + strMsg);
	}
}

void CSaigeInspectionView::DrawBlobOverlay(cv::Mat& cvImg, const std::vector<stAIBLOBClassInfo>& vtBlobs)
{
	std::vector<cv::Scalar> vColors = {
		cv::Scalar(0, 0, 255),
		cv::Scalar(0, 255, 0),
		cv::Scalar(255, 0, 0),
		cv::Scalar(0, 255, 255),
		cv::Scalar(255, 0, 255),
		cv::Scalar(255, 255, 0),
		cv::Scalar(128, 0, 255),
		cv::Scalar(0, 128, 255),
		cv::Scalar(255, 128, 0),
		cv::Scalar(128, 255, 0),
	};

	for (const auto& blob : vtBlobs)
	{
		int nClassIdx = min(blob.class_id, (unsigned int)(vColors.size() - 1));
		cv::Scalar color = vColors[nClassIdx];

		cv::Rect rt(blob.rtRect.left, blob.rtRect.top,
			blob.rtRect.Width(), blob.rtRect.Height());
		cv::rectangle(cvImg, rt, color, 2);

		if (!blob.contour_points.empty())
		{
			std::vector<std::vector<cv::Point>> contours(1);
			for (const auto& pt : blob.contour_points)
				contours[0].push_back(cv::Point(pt.x, pt.y));
			cv::drawContours(cvImg, contours, -1, color, 1);
		}

		CString strLabel;
		strLabel.Format(_T("C%d S:%.0f A:%d"), blob.class_id,
			blob.segmentscore[0], blob.area);

		std::string strLabelA = CT2A(strLabel);
		cv::putText(cvImg, strLabelA,
			cv::Point(blob.rtRect.left, max(blob.rtRect.top - 5, 10)),
			cv::FONT_HERSHEY_SIMPLEX, 0.4, color, 1, cv::LINE_AA);
	}
}

void CSaigeInspectionView::DisplayImage(const cv::Mat& cvImg)
{
	m_cvDisplayImage = cvImg.clone();
	InvalidateRect(nullptr);
}

void CSaigeInspectionView::UpdateUIState()
{
	m_btnRunInspection.EnableWindow(!m_cvInputImage.empty() && m_bModelLoaded);
}
