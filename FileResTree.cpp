// DirTree.cpp : implementation file
//

#include "stdafx.h"
#include "myexplorer2.h"
#include "FileResTree.h"
#include "DirWatcher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define IDM_FIRST_SHELLMENUID	20000
#define IDM_LAST_SHELLMENUID	(IDM_FIRST_SHELLMENUID+1000)
/////////////////////////////////////////////////////////////////////////////
// CFileResTree

CFileResTree::CFileResTree()
{
	m_lstRelated = NULL;
	m_edtAddress = NULL;
	
	m_bShowDirOnly = FALSE;//只显示文件夹
	m_bShowHide = FALSE;//显示隐藏文件（夹）
	m_bShowSystem = FALSE;//显示系统文件
	
	m_pDirThread = NULL;

	m_bContextMenu = TRUE;
}

CFileResTree::~CFileResTree()
{
	if (m_pDirThread)
	{
		m_pDirThread->StopNotifications();
		m_pDirThread = NULL;
	}
}

BEGIN_MESSAGE_MAP(CFileResTree, CTreeCtrl)
//{{AFX_MSG_MAP(CFileResTree)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_MESSAGE(WM_XXW_SHELL_NOTIFY, OnUpdateShell)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_COMMAND_RANGE(IDM_FIRST_SHELLMENUID, IDM_LAST_SHELLMENUID, OnShellCommand)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//初始化，在创建该控件被创建后调用
void CFileResTree::Initial()
{	
	//设置外观样式
	LONG lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle | TVS_HASBUTTONS /*| TVS_LINESATROOT*/ | TVS_HASLINES /*| TVS_DISABLEDRAGDROP*/ );
	
	//创建ImageList
	m_ImageList.Create(16,16,ILC_COLOR32,16,16);
	
	//设置背景色 
	m_ImageList.SetBkColor( GetSysColor(COLOR_WINDOW)); 
	
	//将"我的电脑"图标加入链表	
	int iComputerIcon = m_ImageList.Add(
		ExtractIcon( AfxGetApp()->m_hInstance, "shell32.dll", 15));
	SetImageList(&m_ImageList,TVSIL_NORMAL);//设置TreeCtrl与ImageList关联
	m_hRoot = InsertItem(_T("我的电脑"),iComputerIcon,iComputerIcon,TVI_ROOT);
	
	GetLogicalDrives(m_hRoot);//获取本地驱动器加载到目录树
	GetDriveDir(m_hRoot);//获取驱动器下一级文件夹到目录树
	
	Expand(m_hRoot, TVE_EXPAND);//展开根目录，显示所有的盘符
}


//获取当前选择的节点（目录或文件）的绝对路径
CString CFileResTree::GetSelectedFullPath()
{
	HTREEITEM itemSel = GetSelectedItem();
	return GetFullPath(itemSel);
}

//从某节点往下查找指定名称的节点
//采用深度遍历树来查找对应的节点
//很有可能查不到，因为树并不完整，没有保存所有的节点
HTREEITEM CFileResTree::FindItemText(HTREEITEM itemCurrent, CString strtext) 
{ 	
	//空树，直接返回NULL
	if(itemCurrent == NULL)   
		return NULL;  
	
	//深度遍历树
	HTREEITEM hfind; 
	
	//遍历查找
	while(itemCurrent != NULL) 
	{   
		//当前节点即所需查找节点
		if(GetItemText(itemCurrent) == strtext)   
			return itemCurrent;   	    
		
		//有子节点，查找当前节点的子节点
		if(ItemHasChildren(itemCurrent))  
		{
			itemCurrent = GetChildItem(itemCurrent); 
			//递归调用查找子节点下节点
			hfind = FindItemText(itemCurrent,strtext);
			if(hfind)
			{   
				return hfind;  
			}  
			//子节点中未发现所需节点，继续查找兄弟节点
			else 
				itemCurrent = GetNextSiblingItem(GetParentItem(itemCurrent)); 
		}
		//无子节点，继续查找兄弟节点
		else
		{ 			
			itemCurrent = GetNextSiblingItem(itemCurrent);   
		}
	}
	
    return NULL;//没有找到
}


//采用广度遍历树的算法来查找对应的节点,就是在树种逐层查找，而不是按照一个分支往下查找
//很有可能查不到，因为树中没有保存所有的节点
HTREEITEM CFileResTree::FindItem(HTREEITEM itemCurrent,CString strPath)
{
	//空树，直接返回NULL
	if(itemCurrent == NULL)   
		return NULL;  
	
	//广度遍历树
	HTREEITEM hfind;
	if(strPath.Right(1) == "\\")//去除最右侧的'\'
	{
		strPath.Delete(strPath.GetLength()-1);
	}
	CString DirName;//当前目录名
	int pos = strPath.Find('\\');
	if(pos > 0)
		DirName = strPath.Left(pos);
	else
		DirName = strPath;
	
	//遍历查找
	while(itemCurrent != NULL) 
	{   
		CString curDir = GetItemText(itemCurrent);//当前节点的名称
		//去除最右边的"\"
		if(curDir.Right(1) == "\\")
		{
			curDir.Delete(curDir.GetLength()-1);
		}
		//当前节点即所需查找节点
		if(curDir.CompareNoCase(DirName) == 0)
		{
			//如果路径完全匹配，表示找到了该节点
			if(curDir.CompareNoCase(strPath) == 0)
				return itemCurrent; 
			//否则，继续往下找子节点
			else if(ItemHasChildren(itemCurrent))
			{
				itemCurrent = GetChildItem(itemCurrent); 
				//递归调用查找子节点下节点
				CString sPathNew  = strPath.Mid(pos+1);
				hfind = FindItem(itemCurrent,sPathNew);
				if(hfind)   
				{   
					return hfind;  
				}
			}
			return NULL;  //未查找到 
		}
		//当前节点不是所查节点，就查找其兄弟节点
		else
		{
			//有兄弟节点，查找兄弟节点
			if(GetNextSiblingItem(itemCurrent) != NULL)
			{
				itemCurrent = GetNextSiblingItem(itemCurrent);
			}
			//若无兄弟节点，继续查找子节点
			else if(ItemHasChildren(itemCurrent))
			{
				itemCurrent = GetChildItem(itemCurrent); 
			}
			else
				return NULL;//未查找到 
			
			hfind = FindItem(itemCurrent,strPath);
			if(hfind)   
			{   
				return hfind;  
			}
		}
	}
	
	return itemCurrent;
}


//////////////////////////////////////////////////////////////////////////
//功能：插入节点
//参数：
//     hParent:插入的父节点
//     strPath:插入的节点路径
//     hParent:插入的节点名称
HTREEITEM CFileResTree::InsertItemToTree(HTREEITEM hParent, CString strPath,CString DisplayName)
{
	SHFILEINFO fileInfo;
	SHGetFileInfo(strPath,0,&fileInfo,sizeof(&fileInfo),SHGFI_ICON|SHGFI_DISPLAYNAME|SHGFI_TYPENAME);
	int iIcon = m_ImageList.Add(fileInfo.hIcon);
	HTREEITEM itemNew = InsertItem(DisplayName,iIcon,iIcon,hParent);
	TRACE0("添加:" + strPath + "\n");
	
// 	DWORD dData = TYPE_EMPTY;//默认是空目录
// 	if(IsDirHasChildren(strPath))
// 		dData = TYPE_HASINVALIDSUBITEM;//该目录下含有有效的子目录
// 	SetItemData(itemNew,dData);
// 	
// 	SetItemData(hParent,TYPE_HASVALIDSUBITEM);
	
	return itemNew;
}

// 获取盘符到目录树，作为根目录下一级节点
void CFileResTree::GetLogicalDrives(HTREEITEM hRoot)
{
	size_t szAllDrivesStrings = GetLogicalDriveStrings(0,NULL);//获取驱动器字符长度
	TCHAR* pDriveStrings = new TCHAR[szAllDrivesStrings + sizeof(_T(""))];//定义字符串存储驱动器字符pDriveStrings
	GetLogicalDriveStrings((DWORD)szAllDrivesStrings,pDriveStrings);//获取驱动器字符到pDriveStrings
	
	//截取单个驱动器字符出来添加到目录树上
	//定义临时字符指针指向驱动器字符信息
	//(为了最后可以释放驱动器字符串方便,如果直接移动字符串指针,后面调用delete释放字符串资源的时候会报错)
	TCHAR* pTempDrive = pDriveStrings;//盘符
	size_t szDriveString = _tcslen(pTempDrive);
	while(szDriveString > 0)
	{
		HTREEITEM hItem = InsertItemToTree(hRoot,pTempDrive,pTempDrive);		
		pTempDrive += szDriveString + 1;
		szDriveString = _tcslen(pTempDrive);
	}
	delete pDriveStrings;
}

// 获取驱动器下一级目录到目录树上显示
void CFileResTree::GetDriveDir(HTREEITEM hRoot)
{
	if (ItemHasChildren(hRoot))
	{
		HTREEITEM hChild = GetChildItem(hRoot);//获取根节点下的第一个盘符节点
		while(hChild)
		{
			//获取查找路径
			CString strPath = GetItemText(hChild);//获取节点的文字
			if(!strPath.IsEmpty() && strPath.Right(1) != _T("\\"))
			{
				strPath += _T("\\");
			}
			CString sFind = strPath + _T("*.*");
			CFileFind fileFind;
			BOOL bContinue = fileFind.FindFile(sFind);
			while(bContinue)
			{
				bContinue = fileFind.FindNextFile();
				//磁盘下的文件是目录并且不是[.]或[..]的时候，加载到目录树
				BOOL bAdd = !fileFind.IsDots();
				if(m_bShowDirOnly)
					bAdd = bAdd && fileFind.IsDirectory();//只显示目录
				if(!m_bShowHide)
					bAdd = bAdd && !fileFind.IsHidden();//不显示隐藏
				if(!m_bShowSystem)
					bAdd = bAdd && !fileFind.IsSystem();//不显示系统
				
				if(bAdd)
				{
					CString sname = fileFind.GetFileName();
					InsertItemToTree(hChild, strPath+sname, sname);
				}
			}
			//到下一个驱动器
			hChild = GetNextItem(hChild,TVGN_NEXT);
		}
	}
}

// 获取某节点的文件全路径
CString CFileResTree::GetFullPath(HTREEITEM hParent)
{
	CString strTemp = _T("");
	CString strFullPath = _T("");
	while(hParent != m_hRoot)
	{
		//从当前节点找起，依次找出其父节点，到根节点结束
		strTemp = GetItemText(hParent);
		if(strTemp.Right(1) != _T("\\"))
			strTemp += _T("\\");
		strFullPath = strTemp + strFullPath;
		hParent = GetParentItem(hParent);
	}
	return strFullPath;
}


//删除指定节点下的所有子节点
void CFileResTree::DeleteChildItems(HTREEITEM hParent)
{
	if(hParent == m_hRoot)
		return;
	
	//删除子节点下的所有子节点(包括空的子节点)
	while(ItemHasChildren(hParent))
	{
		HTREEITEM hChild = GetChildItem(hParent);
		CString sPathChild = GetFullPath(hChild);
		TRACE0("删除:" + sPathChild + "\n");
		DeleteItem(hChild);
	}
}


//初始化指定节点
void CFileResTree::InitItem(HTREEITEM hParent)
{	
	if(hParent == m_hRoot)
		return;

	//添加所有的子目录
	CString sPathParent = GetFullPath(hParent);
	if(sPathParent.Right(1) != _T("\\"))
	{
		sPathParent += _T("\\");
	}
	CString sFind = sPathParent + _T("*.*");
	CFileFind fileFind;
	BOOL bContinue = fileFind.FindFile(sFind);
	BOOL bValid = FALSE;
	while(bContinue)
	{
		bContinue = fileFind.FindNextFile();		
		BOOL bAdd = !fileFind.IsDots();
		if(m_bShowDirOnly)
			bAdd = bAdd && fileFind.IsDirectory();//只显示目录
		if(!m_bShowHide)
			bAdd = bAdd && !fileFind.IsHidden();//不显示隐藏
		if(!m_bShowSystem)
			bAdd = bAdd && !fileFind.IsSystem();//不显示系统		
		if(bAdd)
		{
			bValid = TRUE;
			CString sname = fileFind.GetFileName();
			CString sPathFull = fileFind.GetFilePath();
			TRACE0("添加:" + sPathParent+sname + "\n");
			//添加节点
			HTREEITEM hItenSub = InsertItemToTree(hParent,sPathParent+sname,sname);
			
			//如果该节点类型是含有一个无效子节点，先添加一个空节点，以显示该节点前的"+"
			if( IsDirHasChildren(sPathFull))
			{
				HTREEITEM itemEmpty = InsertItem( "",hItenSub);//添加空子目录，用来显示前面的"+"
			}
		}
	}
}


//更新指定的节点
void CFileResTree::UpdateItem(HTREEITEM itemCurrent)
{	
	if(!itemCurrent)
		return;

	if(itemCurrent == m_hRoot)
		return;
	
	//删除该节点的所有子项
	DeleteChildItems(itemCurrent);
	
	InitItem(itemCurrent);
}


//更新相关联的控件
void CFileResTree::UpdateRelatedCtrls()
{
	HTREEITEM itemCurrent = GetSelectedItem();
	CString sPath = GetFullPath(itemCurrent);//当前路径
	
	//获取当前目录路径
	if(m_edtAddress)
	{
		if(itemCurrent == m_hRoot)
		{
			m_edtAddress->SetWindowText(_T(""));
			return;
		}
		
		m_edtAddress->SetWindowText(sPath); //返回现实到地址栏
		if(sPath.Right(1)=="\\")
		{
			sPath += "*.*";
		}
	}
	
	if(m_lstRelated)
	{
		m_lstRelated->DeleteAllItems();//删除ListView中所有元素
		
		//查找当前目录下的所有文件添加到ListView中
		CFileFind fileFind;
		BOOL bContinue = fileFind.FindFile(sPath);
		int iNum = 0;
		while(bContinue)
		{
			bContinue = fileFind.FindNextFile();
			BOOL bAdd = !fileFind.IsDots();
			// 			if(m_bShowDirOnly)
			// 				bAdd = bAdd && fileFind.IsDirectory();//只显示目录
			if(!m_bShowHide)
				bAdd = bAdd && !fileFind.IsHidden();//不显示隐藏
			if(!m_bShowSystem)
				bAdd = bAdd && !fileFind.IsSystem();//不显示系统
			if(bAdd)
			{
				SHFILEINFO fileInfo;
				CString tempPath = sPath;
				int index = tempPath.Find(_T("*.*"));
				tempPath.Delete(index,3);
				SHGetFileInfo(tempPath + fileFind.GetFileName(),0,&fileInfo,sizeof(&fileInfo),SHGFI_ICON|SHGFI_DISPLAYNAME|SHGFI_TYPENAME);
				int iIcon = m_lstRelated->GetImageList(LVSIL_SMALL)->Add(fileInfo.hIcon);
				int ret = m_lstRelated->InsertItem(iNum,fileInfo.szDisplayName,iIcon);
				
				m_lstRelated->SetItemText(iNum,2,fileInfo.szTypeName);
				
				//声明文件属性变量
				CFileStatus status;
				CFile::GetStatus(tempPath + fileFind.GetFileName(), status);
				CString strSize;
				strSize.Format(_T("%d B"),status.m_size);
				m_lstRelated->SetItemText(iNum,1,strSize);
				
				CString strModifyTime = status.m_ctime.Format("%Y-%m-%d %H:%M:%S");
				m_lstRelated->SetItemText(iNum,3,strModifyTime);
				
				iNum++;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFileResTree message handlers

void CFileResTree::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	TVITEM item = pNMTreeView->itemNew;//获取当前展开节点
	HTREEITEM hIitemCurrent = item.hItem;//当前节点
	
	//如果当前展开节点为根节点,则返回
	if(hIitemCurrent == m_hRoot)
		return;
	
	if(pNMTreeView->action == 2) //展开
	{
		UpdateItem(hIitemCurrent);
	}
	
	*pResult = 0;
}

//选择改变，关联的列表框和地址栏都相应的改变
void CFileResTree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	
	HTREEITEM itemCurrent = GetSelectedItem();
	CString sPath = GetFullPath(itemCurrent);//当前路径
	
	UpdateItem(itemCurrent);
	
	//采用监视线程来监测当前选择的目录
	if (m_pDirThread == NULL)
	{
		m_pDirThread = (CDirWatcher*)AfxBeginThread(RUNTIME_CLASS(CDirWatcher),
			THREAD_PRIORITY_LOWEST, 0, CREATE_SUSPENDED, NULL);
		
		//方法1 
		CString sPath;//当前选择的目录
		m_pDirThread->SetFolderPath(this, sPath);
		m_pDirThread->ResumeThread();
	}	
	else
	{
		CString sFolderPath = m_pDirThread->GetFolderPath();
		if (sFolderPath.CompareNoCase(sPath) != 0)
		{
			m_pDirThread->SuspendThread();
			m_pDirThread->SetFolderPath(this, sPath);
			m_pDirThread->ResumeThread();
		}
	}
	
	UpdateRelatedCtrls();
	
	*pResult = 0;
}


//指定的目录是否含子文件或目录
bool CFileResTree::IsDirHasChildren(CString strPath)
{
	int nCnt = 0;
	if(strPath.Right(1) != _T("\\"))
	{
		strPath += _T("\\");
	}
	CString sFind = strPath + _T("*.*");
	CFileFind fileFind;
	BOOL bContinue = fileFind.FindFile(sFind);
	while(bContinue)
	{
		bContinue = fileFind.FindNextFile();
		
		BOOL bAdd = !fileFind.IsDots();
		if(m_bShowDirOnly)
			bAdd = bAdd && fileFind.IsDirectory();//只显示目录
		if(!m_bShowHide)
			bAdd = bAdd && !fileFind.IsHidden();//不显示隐藏
		if(!m_bShowSystem)
			bAdd = bAdd && !fileFind.IsSystem();//不显示系统
		
		//if(fileFind.IsDirectory() && !fileFind.IsDots() && !fileFind.IsHidden())
		if(bAdd)
		{
			return true;//只要有一个子目录，就返回true
		}
	}
	fileFind.Close();
	return false;
}


//指定的目录是否为空
bool CFileResTree::IsDirEmpty(CString strPath)
{
	int nCnt = 0;
	if(strPath.Right(1) != _T("\\"))
	{
		strPath += _T("\\");
	}
	CString sFind = strPath + _T("*.*");
	CFileFind fileFind;
	BOOL bContinue = fileFind.FindFile(sFind);
	while(bContinue)
	{
		bContinue = fileFind.FindNextFile();
		if(!fileFind.IsDots())
		{
			return false;//找到一个文件或文件夹，表示非空
		}
	}
	fileFind.Close();
	return true;
}


LRESULT CFileResTree::OnUpdateShell(WPARAM wParam, LPARAM lParam)
{
	int nMessage = (int)wParam;
	XXW_TVITEMDATA* pItemData = (XXW_TVITEMDATA*)lParam;
	switch (nMessage)
	{
	case WM_XXW_REFRESHTREE:
		{	
			HTREEITEM itemCurrent = GetSelectedItem();
			UpdateItem(itemCurrent);
			UpdateRelatedCtrls();
			break;
		}
	default:
		break;
	}
	
	return 0;
}


void CFileResTree::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	HTREEITEM itemCurrent = GetSelectedItem();
	CString sPath = GetFullPath(itemCurrent);
	
	BOOL bIsFile;//是否是文件
	CFileFind FileFind;
	BOOL res = FileFind.FindFile(sPath);
	if (res)
	{
		if (res = FileFind.FindNextFile())
		{
			if (!FileFind.IsDirectory() && !FileFind.IsDots())//如果是文件
				bIsFile = TRUE;
		}          
	}
	FileFind.Close();
	
	//如果是文件，调用系统命令打开该文件
	if(bIsFile)
		ShellExecute(NULL, NULL, sPath, NULL, NULL, SW_SHOWNORMAL);
	
	*pResult = 0;
}


void CFileResTree::OnRclick(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	// Display the shell context menu.
	if (m_bContextMenu == TRUE)
	{
// 		HTREEITEM hItem = GetContextMenu();
// 		if (hItem != NULL)
// 		{
// 			// TODO: Additional error handling.
// 		}
		
		CPoint pt(GetMessagePos());
		SendMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, MAKELPARAM(pt.x, pt.y));	
	}
	
	*pResult = 0;
}

HTREEITEM CFileResTree::GetContextMenu()
{
	// 	CPoint point;
	// 	::GetCursorPos(&point);
	// 	this->ScreenToClient(&point);
	// 	
	// 	TV_HITTESTINFO tvhti;
	// 	tvhti.pt = point;
	// 	this->HitTest(&tvhti);
	// 	
	// 	if (tvhti.flags & (TVHT_ONITEMLABEL | TVHT_ONITEMICON))
	// 	{
	// 		// Long pointer to TreeView item data
	// 		XXW_TVITEMDATA*  lptvid = (XXW_TVITEMDATA*)this->GetItemData(tvhti.hItem);
	// 		
	// 		this->ClientToScreen(&point);
	// 		
	// 		if (lptvid->lpsfParent == NULL)
	// 		{
	// 			LPSHELLFOLDER lpShellFolder;
	// 			if (FAILED(::SHGetDesktopFolder(&lpShellFolder)))
	// 			{
	// 				return NULL;
	// 			}
	// 			
	// 			ShowContextMenu(this->m_hWnd,
	// 				lpShellFolder, lptvid->lpi, &point);
	// 			
	// 			if (lpShellFolder)
	// 			{
	// 				lpShellFolder->Release();
	// 			}
	// 		}
	// 		else
	// 		{
	// 			ShowContextMenu(this->m_hWnd,
	// 				lptvid->lpsfParent, lptvid->lpi, &point);
	// 		}
	// 		
	// 		return tvhti.hItem;
	// 	}	
	
	return NULL;
}

BOOL CFileResTree::GetItemContextMenu(HTREEITEM hItem, CShellContextMenu& rCtxMenu)
{
	TVITEMDATA* pData = (TVITEMDATA*)GetItemData(hItem);
	if (!pData->IsValid())
		return FALSE;
	
	return rCtxMenu.Create(pData->pParentFolder, pData->pidlAbs.GetLastChild());
}


CMenu* CFileResTree::FindMenuItemByText(CMenu *pMenu, LPCTSTR pszText, UINT& nIndex, BOOL bRecursive)
{
	if (!::IsMenu(pMenu->GetSafeHmenu()))
		return NULL;
	
	CString text;
	UINT count = pMenu->GetMenuItemCount();
	for (UINT id=0; id < count; id++)
	{
		if (pMenu->GetMenuString(id, text, MF_BYPOSITION) > 0
			&& text == pszText)
		{
			nIndex = id;
			return pMenu;
		}
		// search recursively in sub-menus
		if (bRecursive && pMenu->GetMenuState(id, MF_BYPOSITION) & MF_POPUP)
		{
			CMenu *pSubMenu = FindMenuItemByText(pMenu->GetSubMenu(id),
				pszText, nIndex, bRecursive);
			if (pSubMenu != NULL)
				return pSubMenu;
		}
	}
	return NULL;
}

void CFileResTree::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (pWnd->GetSafeHwnd() != m_hWnd)
	{
		Default();
		return;
	}
	
	// get clicked item
	UINT nFlags = 0;
	ScreenToClient(&point);
	HTREEITEM hItemCurrent = HitTest(point, &nFlags);
	
	if (!(nFlags & TVHT_ONITEM))
		return;
	
	// load menu
	CMenu menu;
	menu.LoadMenu(IDR_TESTMENU);
	CMenu *pPopupMenu = menu.GetSubMenu(0);//获取菜单的第一列，其实总共也就一列
	
	//The GetAsyncKeyState function determines whether a key is up or down at the time the function is called, 
	//and whether the key was pressed after a previous call to GetAsyncKeyState. 
	// Ctrl key down, make a submenu
	BOOL bPopup = (GetAsyncKeyState(VK_CONTROL) & 0x8000);
	
	// search insert position
	UINT nInsertAt = 0;

	CMenu *pMenu = FindMenuItemByText(pPopupMenu, _T("&Shell"), nInsertAt);
	if (pMenu == NULL)
	{
		pMenu = pPopupMenu;
		nInsertAt = 0;
	}
	else if (bPopup)
	{
		// add a separator
		pMenu->InsertMenu(nInsertAt+1, MF_BYPOSITION|MF_SEPARATOR, 0);		
		pMenu = pMenu->GetSubMenu(nInsertAt);
		nInsertAt = 0;
	}
	pMenu->DeleteMenu(nInsertAt, MF_BYPOSITION);
	
	GetItemContextMenu(hItemCurrent, m_shellMenu);
	if (!m_shellMenu.FillMenu(pMenu, nInsertAt, IDM_FIRST_SHELLMENUID,
		IDM_LAST_SHELLMENUID, CMF_NODEFAULT|CMF_EXPLORE))
	{
		// no items added
		if (bPopup)
			pMenu->AppendMenu(MF_GRAYED, ID_SHELLMENU, _T("(empty)"));
	}
	
	// display menu
	m_shellMenu.SetOwner(this);
	ClientToScreen(&point);
	pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y, this);
	m_shellMenu.SetOwner(NULL);
}

void CFileResTree::OnShellCommand(UINT nID) 
{
	// shell command
	m_shellMenu.InvokeCommand(nID);
}