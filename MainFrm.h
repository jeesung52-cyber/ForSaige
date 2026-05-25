#pragma once

class CMainFrame : public CFrameWnd
{
protected:
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

public:
	virtual ~CMainFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};
