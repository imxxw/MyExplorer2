// DirTree.cpp : implementation file
//

#include "stdafx.h"
#include "myexplorer2.h"
#include "FileResTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileResTree

CFileResTree::CFileResTree()
{
	m_lstRight = NULL;
	m_edtAddress = NULL;
}

CFileResTree::~CFileResTree()
{
}


BEGIN_MESSAGE_MAP(CFileResTree, CTreeCtrl)
//{{AFX_MSG_MAP(CFileResTree)
ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//初始化，一般在创建该控件被创建后调用
void CFileResTree::Initial()
{	
	//设置外观样式
	LONG lStyle = GetWindowLong(this->m_hWnd, GWL_STYLE);
	SetWindowLong(this->m_hWnd, GWL_STYLE, lStyle | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES /*| TVS_DISABLEDRAGDROP*/ );
	
	//创建ImageList
	m_ImageList.Create(16,16,ILC_COLOR32,16,16);
	//设置背景色 
	m_ImageList.SetBkColor( GetSysColor(COLOR_WINDOW) ); 
	//将"我的电脑"图标加入链表	
	int iComputerIcon = m_ImageList.Add(
		ExtractIcon( AfxGetApp()->m_hInstance, "shell32.dll", 15));
	SetImageList(&m_ImageList,TVSIL_NORMAL);//设置TreeCtrl与ImageList关联
	m_hRoot = this->InsertItem(_T("我的电脑"),iComputerIcon,iComputerIcon,TVI_ROOT);
	
	GetLogicalDrives(m_hRoot);//获取本地驱动器加载到目录树
	GetDriveDir(m_hRoot);//获取驱动器下一级文件夹到目录树
	
	Expand(m_hRoot, TVE_EXPAND);//展开根目录，显示所有的盘符
}


//采用深度遍历树来查找对应的节点
//很有可能查不到，因为树中没有保存所有的节点
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
		CString curDir = this->GetItemText(itemCurrent);//当前节点的名称
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
			else if(this->ItemHasChildren(itemCurrent))
			{
				itemCurrent = this->GetChildItem(itemCurrent); 
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
			if(this->GetNextSiblingItem(itemCurrent) != NULL)
			{
				itemCurrent = this->GetNextSiblingItem(itemCurrent);
			}
			//若无兄弟节点，继续查找子节点
			else if(this->ItemHasChildren(itemCurrent))
			{
				itemCurrent = this->GetChildItem(itemCurrent); 
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


//插入节点
HTREEITEM CFileResTree::InsertItemToTree(HTREEITEM hParent, CString strPath,CString DisplayName)
{
	SHFILEINFO fileInfo;
	SHGetFileInfo(strPath,0,&fileInfo,sizeof(&fileInfo),SHGFI_ICON|SHGFI_DISPLAYNAME|SHGFI_TYPENAME);
	int iIcon = m_ImageList.Add(fileInfo.hIcon);
	TRACE0("添加:" + strPath + "\n");
	return this->InsertItem(DisplayName,iIcon,iIcon,hParent);
	//return this->InsertItem(fileInfo.szDisplayName,iIcon,iIcon,hParent);
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
	while(szDriveString>0)
	{
		InsertItemToTree(hRoot,pTempDrive,pTempDrive);
		pTempDrive += szDriveString + 1;
		szDriveString = _tcslen(pTempDrive);
	}
	delete pDriveStrings;
}

// 获取驱动器下一级目录到目录树上显示
void CFileResTree::GetDriveDir(HTREEITEM hRoot)
{
	if (this->ItemHasChildren(hRoot))
	{
		HTREEITEM hChild = this->GetChildItem(hRoot);//获取根节点下的第一个盘符节点
		while(hChild)
		{
			//获取查找路径
			CString strPath = this->GetItemText(hChild);//获取节点的文字
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
				if(fileFind.IsDirectory() && !fileFind.IsDots())
				{
					CString sname = fileFind.GetFileName();
					InsertItemToTree(hChild, strPath+sname, sname);
				}
			}
			//到下一个驱动器
			hChild = this->GetNextItem(hChild,TVGN_NEXT);
		}
	}
}

// 根据目录树中任一节点获取其所有子目录
void CFileResTree::AddSubDir(HTREEITEM hParent)
{
	CString strPath = GetFullPath(hParent);
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
		if(fileFind.IsDirectory() && !fileFind.IsDots() && !fileFind.IsHidden())
		{
			CString sname = fileFind.GetFileName();
			TRACE0("添加:" + strPath+sname + "\n");
			InsertItemToTree(hParent,strPath+sname,sname);
		}
	}
}

// 获取某节点的文件路径
CString CFileResTree::GetFullPath(HTREEITEM hCurrent)
{
	CString strTemp = _T("");
	CString strFullPath = _T("");
	while(hCurrent != m_hRoot)
	{
		//从当前节点找起，依次找出其父节点，到根节点结束
		strTemp = this->GetItemText(hCurrent);
		if(strTemp.Right(1) != _T("\\"))
			strTemp += _T("\\");
		strFullPath = strTemp + strFullPath;
		hCurrent = this->GetParentItem(hCurrent);
	}
	return strFullPath;
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

	//如果当前展开节点为根节点,则返回
	if(item.hItem == m_hRoot)
		return;
	
/*
展开一个节点，就给该节点下所有子节点再添加一层子节点。
收起一个节点，就把该节点下所有子节点的子节点删除。
这样，就保证每个选中的节点含有一层子目录。
缺点：左侧树形列表中，展开某个节点，如果节点下的子节点过多，则非常慢，可能假死。因为子节点的子节点会非常多，添加起来比较慢。
*/
	CString sText = this->GetFullPath(item.hItem);
	TRACE0("当前目录:" + sText + "\n");
	if(this->ItemHasChildren(item.hItem))
	{
		HTREEITEM hChild = this->GetChildItem(item.hItem);
		sText = this->GetFullPath(hChild);
		TRACE0("收起子目录:" + sText + "\n");
		if(pNMTreeView->action == 1)  //收起
		{	
			//删除所有子节点下的子节点
			while(hChild!=NULL)
			{
				sText = this->GetFullPath(hChild);
				TRACE0("收起子目录:" + sText + "\n");
				while(this->ItemHasChildren(hChild))
				{
					HTREEITEM item = this->GetChildItem(hChild);
					sText = this->GetFullPath(item);
					TRACE0("删除:" + sText + "\n");
					this->DeleteItem(item);
				}
				hChild = this->GetNextItem(hChild,TVGN_NEXT);
			}
		}
		else if(pNMTreeView->action == 2) //展开
		{
			//轮训展开节点的每个子节点，加载文件目录信息到子节点上
			//如果子节点过多，则非常慢，可能假死。因为子子节点的子节点可能会非常多，添加起来比较慢。
			while(hChild != NULL)
			{
				sText = this->GetFullPath(hChild);
				TRACE0("展开子目录:" + sText + "\n");
				AddSubDir(hChild);
				hChild = this->GetNextItem(hChild,TVGN_NEXT);
			}
		}
	}
	
// 	if(pNMTreeView->action == 1)  //收起
// 	{
// 		if(this->ItemHasChildren(item.hItem))
// 			this->Expand(item.hItem,TVE_COLLAPSE);
// 	}
// 	else if(pNMTreeView->action == 2) //展开
// 	{
// 		if(!this->ItemHasChildren(item.hItem))
// 		{
// 			HTREEITEM hChild = this->GetChildItem(item.hItem);
// 			//轮训展开节点的每个子节点，加载文件目录信息到子节点上
// 			while(hChild != NULL)
// 			{
// 				AddSubDir(hChild);
// 				hChild = this->GetNextItem(hChild,TVGN_NEXT);
// 			}
// 		}
// 	}
	
	*pResult = 0;
}

void CFileResTree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	
	//获取当前目录路径
	TVITEM item = pNMTreeView->itemNew;
	CString strDirPath = GetFullPath(item.hItem);
	if(m_edtAddress)
	{
		if(item.hItem == m_hRoot)
		{
			m_edtAddress->SetWindowText(_T(""));
			return;
		}
		
		m_edtAddress->SetWindowText(strDirPath); //返回现实到地址栏
		if(strDirPath.Right(1)=="\\")
		{
			strDirPath += "*.*";
		}
	}
	
	if(m_lstRight)
	{
		m_lstRight->DeleteAllItems();//删除ListView中所有元素
		
		//查找当前目录下的所有文件添加到ListView中
		CFileFind fileFind;
		BOOL bContinue = fileFind.FindFile(strDirPath);
		int iNum = 0;
		while(bContinue)
		{
			bContinue = fileFind.FindNextFile();
			if(/*!fileFind.IsDirectory() && */!fileFind.IsDots()&&!fileFind.IsHidden())
			{
				SHFILEINFO fileInfo;
				CString tempPath = strDirPath;
				int index = tempPath.Find(_T("*.*"));
				tempPath.Delete(index,3);
				SHGetFileInfo(tempPath + fileFind.GetFileName(),0,&fileInfo,sizeof(&fileInfo),SHGFI_ICON|SHGFI_DISPLAYNAME|SHGFI_TYPENAME);
				int iIcon = m_lstRight->GetImageList(LVSIL_SMALL)->Add(fileInfo.hIcon);
				int ret = m_lstRight->InsertItem(iNum,fileInfo.szDisplayName,iIcon);
				
				m_lstRight->SetItemText(iNum,2,fileInfo.szTypeName);
				
				//声明文件属性变量
				CFileStatus status;
				CFile::GetStatus(tempPath + fileFind.GetFileName(), status);
				CString strSize;
				strSize.Format(_T("%d B"),status.m_size);
				m_lstRight->SetItemText(iNum,1,strSize);
				
				CString strModifyTime = status.m_ctime.Format("%Y-%m-%d %H:%M:%S");
				m_lstRight->SetItemText(iNum,3,strModifyTime);
				
				iNum++;
			}
		}
	}
	
	*pResult = 0;
}
