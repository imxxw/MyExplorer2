#if !defined(AFX_FILERESLIST_H__D741D760_BD7F_4BAC_B87F_DD2E3817F25A__INCLUDED_)
#define AFX_FILERESLIST_H__D741D760_BD7F_4BAC_B87F_DD2E3817F25A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileResList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileResList window
//�ļ���Դ�б��
class CFileResList : public CListCtrl
{
// Construction
public:
	CFileResList();

	void Initial();

	int GetSortedCol(){return m_nSortedCol;}

	BOOL GetAsc(){return m_bAsc;}

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileResList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFileResList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFileResList)
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
		
private:
	//�б�����
	static int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	
	CImageList m_RightImageList;
	int m_nSortedCol;//��ǰ�������
	BOOL m_bAsc;//�Ƿ�˳������
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILERESLIST_H__D741D760_BD7F_4BAC_B87F_DD2E3817F25A__INCLUDED_)
