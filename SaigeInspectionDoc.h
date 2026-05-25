#pragma once

class CSaigeInspectionDoc : public CDocument
{
protected:
	CSaigeInspectionDoc() noexcept;
	DECLARE_DYNCREATE(CSaigeInspectionDoc)
public:
	virtual ~CSaigeInspectionDoc();
	DECLARE_MESSAGE_MAP()
};
