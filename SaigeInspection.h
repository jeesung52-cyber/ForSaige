#pragma once

#ifndef __AFXWIN_H__
	#error "pch.h를 먼저 include 하십시오."
#endif

#include "resource.h"

class CSaigeInspectionApp : public CWinApp
{
public:
	CSaigeInspectionApp() noexcept;
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};
