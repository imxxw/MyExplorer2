#if !defined(AFX_DIRTREE_H__DAA4C1B7_EDAB_4A46_ACFF_CAEA495B6793__INCLUDED_)
#define AFX_DIRTREE_H__DAA4C1B7_EDAB_4A46_ACFF_CAEA495B6793__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DirTree.h : header file
//
#include "FileResList.h"
#include "ShellPidl.h"
#include "ShellString.h"
#include "ShellContextMenu.h"

class CDirWatcher;

/////////////////////////////////////////////////////////////////////////////
// CFileResTree window

//Ŀ¼���ؼ����������ķ�ʽչʾ�����������ļ�Ŀ¼
class CFileResTree : public CTreeCtrl
{
private:
	struct TVITEMDATA
	{
		SShellFolderPtr pParentFolder;
		CShellPidl pidlAbs;
		UINT nFlags;
		
		BOOL IsValid()
		{
			return (this != NULL)
				&& pParentFolder.IsValid() && pidlAbs.IsValid();
		}
	};

	static CMenu* FindMenuItemByText(CMenu* pMenu, LPCTSTR pszText, UINT &nIndex, BOOL bRecursive = TRUE);

	// Construction
	public:
		CFileResTree();
		
		//��ʼ��
		void Initial();
		
		//��ȡ��ǰѡ��Ľڵ㣨Ŀ¼���ļ����ľ���·��
		CString GetSelectedFullPath();	
		
		//������ص��б���ͼ
		void SetRelatedList(CFileResList* listctrl){m_lstRelated = listctrl;}
		
		//������صĵ�ַ��
		void SetRelatedEdit(CEdit* address){m_edtAddress = address;}
		
		//��ĳ�ڵ����²���ָ�����ƵĽڵ�
		//������ȱ����������Ҷ�Ӧ�Ľڵ�
		//���п��ܲ鲻������Ϊ������������û�б������еĽڵ�
		HTREEITEM FindItemText(HTREEITEM itemCurrent, CString strtext);  
		
		//���ù�ȱ��������㷨�����Ҷ�Ӧ�Ľڵ�,���������������ң������ǰ���һ����֧���²���
		//���п��ܲ鲻������Ϊ����û�б������еĽڵ�
		HTREEITEM FindItem(HTREEITEM itemCurrent,CString strPath); 
		
		BOOL GetItemContextMenu(HTREEITEM hItem, CShellContextMenu &rCtxMenu);
		
	protected:	
		
		//����ڵ�
		HTREEITEM InsertItemToTree(HTREEITEM hParent, CString strPath,CString DisplayName);
		
		// ��ȡ�̷���Ŀ¼������Ϊ��Ŀ¼��һ���ڵ�
		void GetLogicalDrives(HTREEITEM hRoot);
		
		// ��ȡ��������һ��Ŀ¼��Ŀ¼������ʾ
		void GetDriveDir(HTREEITEM hRoot);
		
		// ��ȡĳ�ڵ���ļ�·��
		CString GetFullPath(HTREEITEM hParent);
		
		//��ʼ��ָ���ڵ�
		void InitItem(HTREEITEM hParent);
		
		//ɾ��ָ���ڵ��µ������ӽڵ�
		void DeleteChildItems(HTREEITEM hParent);
		
		//����ָ���Ľڵ�
		void UpdateItem(HTREEITEM hParent);
		
		//����������Ŀؼ�
		void UpdateRelatedCtrls();
		
		//ָ����Ŀ¼�Ƿ������ļ���Ŀ¼
		bool IsDirHasChildren(CString strPath);
		
		//ָ����Ŀ¼�Ƿ�Ϊ��
		bool IsDirEmpty(CString strPath);
		
		void EnableContextMenu(BOOL bEnable)
		{
			m_bContextMenu = bEnable;
		}
		
		//-----------------------------------------------------------------------
		// Summary:
		//     This member function displays the system popup menu for the selected
		//     item or folder.
		// Returns:
		//     A handle to the currently selected HTREEITEM.
		//-----------------------------------------------------------------------
		virtual HTREEITEM GetContextMenu();
		
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
		afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
		afx_msg void OnShellCommand(UINT nID);
		//}}AFX_MSG
		
		DECLARE_MESSAGE_MAP()
			
	private:
// 		//�ڵ����ͣ������ڽڵ�itemdata��
// 		enum ITEM_TYPE
// 		{
// 			TYPE_INVALID = -1,           //�ýڵ�����Ч�ڵ㣬���ᱻɾ��
// 				TYPE_EMPTY = 0,          //�ýڵ㲻���ӽڵ㣨����Ŀ¼��
// 				TYPE_HASINVALIDSUBITEM,  //�ýڵ㺬��һ����Ч�ڵ�Ľڵ㣬��ʾ��Ŀ¼�º���Ŀ¼������Ŀǰֻ�����һ���յ���Ŀ¼
// 				TYPE_HASVALIDSUBITEM};   //�ýڵ㺬��һ��������Ч�ڵ�
			
			BOOL m_bShowDirOnly;//ֻ��ʾ�ļ���
			BOOL m_bShowHide;//��ʾ�����ļ����У�
			BOOL m_bShowSystem;//��ʾϵͳ�ļ�
			
			CImageList m_ImageList;//ͼ���б�
			
			HTREEITEM m_hRoot;//���ڵ�
			
			CFileResList* m_lstRelated;//�������б����ʾ��ǰѡ���Ŀ¼�����е�Ŀ¼���ļ�
			
			CEdit* m_edtAddress;//�����ı༭����ʾ��ǰ����·��
			
			CDirWatcher* m_pDirThread;//���ָ��Ŀ¼����߳�
			
			BOOL m_bContextMenu; //�Ƿ���ʾ�Ҽ��˵�
			
			CShellContextMenu m_shellMenu;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRTREE_H__DAA4C1B7_EDAB_4A46_ACFF_CAEA495B6793__INCLUDED_)
