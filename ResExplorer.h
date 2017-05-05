#if !defined(AFX_RESEXPLORER_H__C179E784_D432_4013_BDEB_19C7246AFB35__INCLUDED_)
#define AFX_RESEXPLORER_H__C179E784_D432_4013_BDEB_19C7246AFB35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DirTreeCtrl.h : header file

// CResExplorer

class CResExplorer : public CTreeCtrl
{
	DECLARE_DYNAMIC(CResExplorer)

public:
	CResExplorer();
	virtual ~CResExplorer();
	void Initial();
  
    HTREEITEM FindItemText(CTreeCtrl* tree,HTREEITEM item, CString strtext);  //��ȱ���������

	HTREEITEM FindItem(HTREEITEM item,CString strPath);   //��ȱ���������

protected:
// 	 //������ص��б���ͼ
// 	void SetRelatedList(CListCtrl* listctrl){m_lstRight = listctrl;}
// 
// 	//������صĵ�ַ��
// 	void SetRelatedEdit(CEdit* address){m_edtAddress = address;}

	HTREEITEM InsertItemToTree(HTREEITEM hParent, CString strPath,CString DisplayName);

	// ��ȡ�̷���Ŀ¼������Ϊ��Ŀ¼��һ���ڵ�
	void GetLogicalDrives(HTREEITEM hRoot);

	// ��ȡ��������һ��Ŀ¼��Ŀ¼������ʾ
	void GetDriveDir(HTREEITEM hRoot);

	// ����Ŀ¼������һ�ڵ��ȡ��������Ŀ¼
	void AddSubDir(HTREEITEM hParent);

	// ��ȡĳ�ڵ���ļ�·��
	CString GetFullPath(HTREEITEM hCurrent);

protected:
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) ;

	afx_msg void OnItemexpandedTreedir(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()


private:
	
	HTREEITEM m_hRoot;//���ڵ�
	
	CImageList m_ImageList;//ͼ���б�

// 	CListCtrl* m_lstRight;//��Ӧ���ļ���Դ�����
// 
// 	CEdit* m_edtAddress;//��Ӧ��Ŀ¼����ַ��
};

#endif //AFX_RESEXPLORER_H__C179E784_D432_4013_BDEB_19C7246AFB35__INCLUDED_


