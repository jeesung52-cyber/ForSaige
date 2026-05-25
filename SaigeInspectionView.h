#pragma once

#include "SaigeSeg.h"

class CSaigeInspectionView : public CFormView
{
protected:
	CSaigeInspectionView() noexcept;
	DECLARE_DYNCREATE(CSaigeInspectionView)

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SAIGEINSPECTION_FORM };
#endif

	CSaigeInspectionDoc* GetDocument() const;

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnInitialUpdate();

	DECLARE_MESSAGE_MAP()

public:
	virtual ~CSaigeInspectionView();

private:
	CStatic m_ctlImageDisplay;
	CButton m_btnLoadImage;
	CButton m_btnLoadModel;
	CButton m_btnRunInspection;
	CStatic m_staticImagePath;
	CStatic m_staticModelPath;
	CStatic m_staticResult;
	CStatic m_staticClassCount;
	CStatic m_staticInfo;
	CListBox m_listResults;

	CString m_strImagePath;
	CString m_strModelPath;

	cv::Mat m_cvInputImage;
	cv::Mat m_cvDisplayImage;

	CSaigeSeg m_saigeSeg;
	bool m_bModelLoaded;

	afx_msg void OnBnClickedLoadImage();
	afx_msg void OnBnClickedLoadModel();
	afx_msg void OnBnClickedRunInspection();
	afx_msg void OnPaint();

	void DisplayImage(const cv::Mat& cvImg);
	void UpdateUIState();
	void DrawBlobOverlay(cv::Mat& cvImg, const std::vector<stAIBLOBClassInfo>& vtBlobs);
};

#ifndef _DEBUG
inline CSaigeInspectionDoc* CSaigeInspectionView::GetDocument() const
{
	return reinterpret_cast<CSaigeInspectionDoc*>(m_pDocument);
}
#endif
