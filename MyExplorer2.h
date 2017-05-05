// MyExplorer2.h : main header file for the MYEXPLORER2 application
//

#if !defined(AFX_MYEXPLORER2_H__55CDD4EC_D930_421D_BE8A_160E802D256B__INCLUDED_)
#define AFX_MYEXPLORER2_H__55CDD4EC_D930_421D_BE8A_160E802D256B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMyExplorer2App:
// See MyExplorer2.cpp for the implementation of this class
//

class CMyExplorer2App : public CWinApp
{
public:
	CMyExplorer2App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyExplorer2App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMyExplorer2App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYEXPLORER2_H__55CDD4EC_D930_421D_BE8A_160E802D256B__INCLUDED_)
