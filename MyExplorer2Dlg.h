// MyExplorer2Dlg.h : header file
//

#if !defined(AFX_MYEXPLORER2DLG_H__731EA9D0_0E89_408C_B798_552A375B6386__INCLUDED_)
#define AFX_MYEXPLORER2DLG_H__731EA9D0_0E89_408C_B798_552A375B6386__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FileResTree.h"
#include "FileResList.h"

/////////////////////////////////////////////////////////////////////////////
// CMyExplorer2Dlg dialog

class CMyExplorer2Dlg : public CDialog
{
// Construction
public:
	CMyExplorer2Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMyExplorer2Dlg)
	enum { IDD = IDD_MYEXPLORER2_DIALOG };
	CFileResList	m_wndList;
	CEdit	m_edtAddress;
	CButton	m_btnGoto;
	CFileResTree	m_wndTree;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyExplorer2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMyExplorer2Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnGoto();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYEXPLORER2DLG_H__731EA9D0_0E89_408C_B798_552A375B6386__INCLUDED_)
