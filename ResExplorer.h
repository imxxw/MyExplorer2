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
  
    HTREEITEM FindItemText(CTreeCtrl* tree,HTREEITEM item, CString strtext);  //深度遍历树查找

	HTREEITEM FindItem(HTREEITEM item,CString strPath);   //广度遍历树查找

protected:
// 	 //设置相关的列表视图
// 	void SetRelatedList(CListCtrl* listctrl){m_lstRight = listctrl;}
// 
// 	//设置相关的地址栏
// 	void SetRelatedEdit(CEdit* address){m_edtAddress = address;}

	HTREEITEM InsertItemToTree(HTREEITEM hParent, CString strPath,CString DisplayName);

	// 获取盘符到目录树，作为根目录下一级节点
	void GetLogicalDrives(HTREEITEM hRoot);

	// 获取驱动器下一级目录到目录树上显示
	void GetDriveDir(HTREEITEM hRoot);

	// 根据目录树中任一节点获取其所有子目录
	void AddSubDir(HTREEITEM hParent);

	// 获取某节点的文件路径
	CString GetFullPath(HTREEITEM hCurrent);

protected:
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) ;

	afx_msg void OnItemexpandedTreedir(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()


private:
	
	HTREEITEM m_hRoot;//根节点
	
	CImageList m_ImageList;//图标列表

// 	CListCtrl* m_lstRight;//对应的文件资源报表框
// 
// 	CEdit* m_edtAddress;//对应的目录（地址）
};

#endif //AFX_RESEXPLORER_H__C179E784_D432_4013_BDEB_19C7246AFB35__INCLUDED_


