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

//目录树控件，采用树的方式展示本机的所有文件目录
class CFileResTree : public CTreeCtrl
{
// Construction
public:
	CFileResTree();

	//初始化
	void Initial();

	//深度遍历树查找
    HTREEITEM FindItemText(HTREEITEM itemCurrent, CString strtext);  

	//广度遍历树查找
	HTREEITEM FindItem(HTREEITEM itemCurrent,CString strPath); 
	
	//设置相关的列表视图
	void SetRelatedList(CFileResList* listctrl){m_lstRight = listctrl;}

	//设置相关的地址栏
	void SetRelatedEdit(CEdit* address){m_edtAddress = address;}

protected:	
	
	//插入节点
	HTREEITEM InsertItemToTree(HTREEITEM hParent, CString strPath,CString DisplayName);
	
	// 获取盘符到目录树，作为根目录下一级节点
	void GetLogicalDrives(HTREEITEM hRoot);
	
	// 获取驱动器下一级目录到目录树上显示
	void GetDriveDir(HTREEITEM hRoot);
	
	// 根据目录树中任一节点获取其所有子目录
	void AddSubDir(HTREEITEM hParent);
	
	// 获取某节点的文件路径
	CString GetFullPath(HTREEITEM hParent);

	//展开指定节点
	void ExpandItem(HTREEITEM hParent);
	
	//删除指定节点下的所有子节点
	void DeleteChildItems(HTREEITEM hParent);

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
	//节点类型，保存在节点itemdata中
	enum ITEM_TYPE
	{
		TYPE_INVALID = -1,           //该节点是无效节点，将会被删除
			TYPE_EMPTY = 0,          //该节点不含子节点（不含目录）
			TYPE_HASINVALIDSUBITEM,  //该节点含有一个无效节点的节点，表示该目录下含子目录，但是目前只添加了一个空的子目录
			TYPE_HASVALIDSUBITEM};   //该节点含有一个或多个有效节点
	
	CImageList m_ImageList;//图标列表
	
	HTREEITEM m_hRoot;//根节点
	
	CFileResList* m_lstRight;

	CEdit* m_edtAddress;

	BOOL m_bShowDirOnly;//只显示文件夹
	BOOL m_bShowHide;//显示隐藏文件（夹）
	BOOL m_bShowSystem;//显示系统文件

public:

	//指定的目录是否含有子文件或目录
	bool IsDirHasChildren(CString strPath);
	
	//指定的目录是否为空
	static bool IsDirEmpty(CString strPath);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRTREE_H__DAA4C1B7_EDAB_4A46_ACFF_CAEA495B6793__INCLUDED_)
