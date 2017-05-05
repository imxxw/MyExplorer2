#if !defined(AFX_DIRTREE_H__DAA4C1B7_EDAB_4A46_ACFF_CAEA495B6793__INCLUDED_)
#define AFX_DIRTREE_H__DAA4C1B7_EDAB_4A46_ACFF_CAEA495B6793__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DirTree.h : header file
//

#include "FileResList.h"

/////////////////////////////////////////////////////////////////////////////
// CFileResTree window

//Ŀ¼���ؼ����������ķ�ʽչʾ�����������ļ�Ŀ¼
class CFileResTree : public CTreeCtrl
{
// Construction
public:
	CFileResTree();

	//��ʼ��
	void Initial();

	//��ȱ���������
    HTREEITEM FindItemText(HTREEITEM itemCurrent, CString strtext);  

	//��ȱ���������
	HTREEITEM FindItem(HTREEITEM itemCurrent,CString strPath); 
	
	//������ص��б���ͼ
	void SetRelatedList(CFileResList* listctrl){m_lstRight = listctrl;}

	//������صĵ�ַ��
	void SetRelatedEdit(CEdit* address){m_edtAddress = address;}

protected:	
	
	//����ڵ�
	HTREEITEM InsertItemToTree(HTREEITEM hParent, CString strPath,CString DisplayName);
	
	// ��ȡ�̷���Ŀ¼������Ϊ��Ŀ¼��һ���ڵ�
	void GetLogicalDrives(HTREEITEM hRoot);
	
	// ��ȡ��������һ��Ŀ¼��Ŀ¼������ʾ
	void GetDriveDir(HTREEITEM hRoot);
	
	// ����Ŀ¼������һ�ڵ��ȡ��������Ŀ¼
	void AddSubDir(HTREEITEM hParent);
	
	// ��ȡĳ�ڵ���ļ�·��
	CString GetFullPath(HTREEITEM hCurrent);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileResTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFileResTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFileResTree)
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	
	CImageList m_ImageList;//ͼ���б�
	
	HTREEITEM m_hRoot;//���ڵ�
	
	CFileResList* m_lstRight;

	CEdit* m_edtAddress;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRTREE_H__DAA4C1B7_EDAB_4A46_ACFF_CAEA495B6793__INCLUDED_)
