#if !defined(AFX_DIRTREE_H__DAA4C1B7_EDAB_4A46_ACFF_CAEA495B6793__INCLUDED_)
#define AFX_DIRTREE_H__DAA4C1B7_EDAB_4A46_ACFF_CAEA495B6793__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DirTree.h : header file
//

#include "FileResList.h"

class CXTDirWatcher;

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
	
	//从某节点往下查找指定名称的节点
	//采用深度遍历树来查找对应的节点
	//很有可能查不到，因为树并不完整，没有保存所有的节点
    HTREEITEM FindItemText(HTREEITEM itemCurrent, CString strtext);  
	
	//采用广度遍历树的算法来查找对应的节点,就是在树种逐层查找，而不是按照一个分支往下查找
	//很有可能查不到，因为树中没有保存所有的节点
	HTREEITEM FindItem(HTREEITEM itemCurrent,CString strPath); 
	
	//设置相关的列表视图
	void SetRelatedList(CFileResList* listctrl){m_lstRelated = listctrl;}
	
	//设置相关的地址栏
	void SetRelatedEdit(CEdit* address){m_edtAddress = address;}
	
protected:	
	
	//插入节点
	HTREEITEM InsertItemToTree(HTREEITEM hParent, CString strPath,CString DisplayName);
	
	// 获取盘符到目录树，作为根目录下一级节点
	void GetLogicalDrives(HTREEITEM hRoot);
	
	// 获取驱动器下一级目录到目录树上显示
	void GetDriveDir(HTREEITEM hRoot);
		
	// 获取某节点的文件路径
	CString GetFullPath(HTREEITEM hParent);
	
	//初始化指定节点
	void InitItem(HTREEITEM hParent);
	
	//删除指定节点下的所有子节点
	void DeleteChildItems(HTREEITEM hParent);

	//更新指定的节点
	void UpdateItem(HTREEITEM hParent);
	
	//指定的目录是否含有子文件或目录
	bool IsDirHasChildren(CString strPath);
	
	//指定的目录是否为空
	static bool IsDirEmpty(CString strPath);
	
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
	afx_msg LRESULT OnUpdateShell(WPARAM wParam, LPARAM lParam);
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
		
		BOOL m_bShowDirOnly;//只显示文件夹
		BOOL m_bShowHide;//显示隐藏文件（夹）
		BOOL m_bShowSystem;//显示系统文件
		
		CImageList m_ImageList;//图标列表
		
		HTREEITEM m_hRoot;//根节点
		
		CFileResList* m_lstRelated;//关联的列表框，显示当前选择的目录下所有的目录和文件
		
		CEdit* m_edtAddress;//关联的编辑框，显示当前绝对路径
		
		CXTDirWatcher* m_pDirThread;//监测指定目录活动的线程
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRTREE_H__DAA4C1B7_EDAB_4A46_ACFF_CAEA495B6793__INCLUDED_)
