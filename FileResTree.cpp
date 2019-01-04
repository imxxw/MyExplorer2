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
	
	m_bShowDirOnly = FALSE;//ֻ��ʾ�ļ���
	m_bShowHide = FALSE;//��ʾ�����ļ����У�
	m_bShowSystem = FALSE;//��ʾϵͳ�ļ�
	
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


//��ʼ�����ڴ����ÿؼ������������
void CFileResTree::Initial()
{	
	//���������ʽ
	LONG lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle | TVS_HASBUTTONS /*| TVS_LINESATROOT*/ | TVS_HASLINES /*| TVS_DISABLEDRAGDROP*/ );
	
	//����ImageList
	m_ImageList.Create(16,16,ILC_COLOR32,16,16);
	
	//���ñ���ɫ 
	m_ImageList.SetBkColor( GetSysColor(COLOR_WINDOW)); 
	
	//��"�ҵĵ���"ͼ���������	
	int iComputerIcon = m_ImageList.Add(
		ExtractIcon( AfxGetApp()->m_hInstance, "shell32.dll", 15));
	SetImageList(&m_ImageList,TVSIL_NORMAL);//����TreeCtrl��ImageList����
	m_hRoot = InsertItem(_T("�ҵĵ���"),iComputerIcon,iComputerIcon,TVI_ROOT);
	
	GetLogicalDrives(m_hRoot);//��ȡ�������������ص�Ŀ¼��
	GetDriveDir(m_hRoot);//��ȡ��������һ���ļ��е�Ŀ¼��
	
	Expand(m_hRoot, TVE_EXPAND);//չ����Ŀ¼����ʾ���е��̷�
}


//��ȡ��ǰѡ��Ľڵ㣨Ŀ¼���ļ����ľ���·��
CString CFileResTree::GetSelectedFullPath()
{
	HTREEITEM itemSel = GetSelectedItem();
	return GetFullPath(itemSel);
}

//��ĳ�ڵ����²���ָ�����ƵĽڵ�
//������ȱ����������Ҷ�Ӧ�Ľڵ�
//���п��ܲ鲻������Ϊ������������û�б������еĽڵ�
HTREEITEM CFileResTree::FindItemText(HTREEITEM itemCurrent, CString strtext) 
{ 	
	//������ֱ�ӷ���NULL
	if(itemCurrent == NULL)   
		return NULL;  
	
	//��ȱ�����
	HTREEITEM hfind; 
	
	//��������
	while(itemCurrent != NULL) 
	{   
		//��ǰ�ڵ㼴������ҽڵ�
		if(GetItemText(itemCurrent) == strtext)   
			return itemCurrent;   	    
		
		//���ӽڵ㣬���ҵ�ǰ�ڵ���ӽڵ�
		if(ItemHasChildren(itemCurrent))  
		{
			itemCurrent = GetChildItem(itemCurrent); 
			//�ݹ���ò����ӽڵ��½ڵ�
			hfind = FindItemText(itemCurrent,strtext);
			if(hfind)
			{   
				return hfind;  
			}  
			//�ӽڵ���δ��������ڵ㣬���������ֵܽڵ�
			else 
				itemCurrent = GetNextSiblingItem(GetParentItem(itemCurrent)); 
		}
		//���ӽڵ㣬���������ֵܽڵ�
		else
		{ 			
			itemCurrent = GetNextSiblingItem(itemCurrent);   
		}
	}
	
    return NULL;//û���ҵ�
}


//���ù�ȱ��������㷨�����Ҷ�Ӧ�Ľڵ�,���������������ң������ǰ���һ����֧���²���
//���п��ܲ鲻������Ϊ����û�б������еĽڵ�
HTREEITEM CFileResTree::FindItem(HTREEITEM itemCurrent,CString strPath)
{
	//������ֱ�ӷ���NULL
	if(itemCurrent == NULL)   
		return NULL;  
	
	//��ȱ�����
	HTREEITEM hfind;
	if(strPath.Right(1) == "\\")//ȥ�����Ҳ��'\'
	{
		strPath.Delete(strPath.GetLength()-1);
	}
	CString DirName;//��ǰĿ¼��
	int pos = strPath.Find('\\');
	if(pos > 0)
		DirName = strPath.Left(pos);
	else
		DirName = strPath;
	
	//��������
	while(itemCurrent != NULL) 
	{   
		CString curDir = GetItemText(itemCurrent);//��ǰ�ڵ������
		//ȥ�����ұߵ�"\"
		if(curDir.Right(1) == "\\")
		{
			curDir.Delete(curDir.GetLength()-1);
		}
		//��ǰ�ڵ㼴������ҽڵ�
		if(curDir.CompareNoCase(DirName) == 0)
		{
			//���·����ȫƥ�䣬��ʾ�ҵ��˸ýڵ�
			if(curDir.CompareNoCase(strPath) == 0)
				return itemCurrent; 
			//���򣬼����������ӽڵ�
			else if(ItemHasChildren(itemCurrent))
			{
				itemCurrent = GetChildItem(itemCurrent); 
				//�ݹ���ò����ӽڵ��½ڵ�
				CString sPathNew  = strPath.Mid(pos+1);
				hfind = FindItem(itemCurrent,sPathNew);
				if(hfind)   
				{   
					return hfind;  
				}
			}
			return NULL;  //δ���ҵ� 
		}
		//��ǰ�ڵ㲻������ڵ㣬�Ͳ������ֵܽڵ�
		else
		{
			//���ֵܽڵ㣬�����ֵܽڵ�
			if(GetNextSiblingItem(itemCurrent) != NULL)
			{
				itemCurrent = GetNextSiblingItem(itemCurrent);
			}
			//�����ֵܽڵ㣬���������ӽڵ�
			else if(ItemHasChildren(itemCurrent))
			{
				itemCurrent = GetChildItem(itemCurrent); 
			}
			else
				return NULL;//δ���ҵ� 
			
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
//���ܣ�����ڵ�
//������
//     hParent:����ĸ��ڵ�
//     strPath:����Ľڵ�·��
//     hParent:����Ľڵ�����
HTREEITEM CFileResTree::InsertItemToTree(HTREEITEM hParent, CString strPath,CString DisplayName)
{
	SHFILEINFO fileInfo;
	SHGetFileInfo(strPath,0,&fileInfo,sizeof(&fileInfo),SHGFI_ICON|SHGFI_DISPLAYNAME|SHGFI_TYPENAME);
	int iIcon = m_ImageList.Add(fileInfo.hIcon);
	HTREEITEM itemNew = InsertItem(DisplayName,iIcon,iIcon,hParent);
	TRACE0("���:" + strPath + "\n");
	
// 	DWORD dData = TYPE_EMPTY;//Ĭ���ǿ�Ŀ¼
// 	if(IsDirHasChildren(strPath))
// 		dData = TYPE_HASINVALIDSUBITEM;//��Ŀ¼�º�����Ч����Ŀ¼
// 	SetItemData(itemNew,dData);
// 	
// 	SetItemData(hParent,TYPE_HASVALIDSUBITEM);
	
	return itemNew;
}

// ��ȡ�̷���Ŀ¼������Ϊ��Ŀ¼��һ���ڵ�
void CFileResTree::GetLogicalDrives(HTREEITEM hRoot)
{
	size_t szAllDrivesStrings = GetLogicalDriveStrings(0,NULL);//��ȡ�������ַ�����
	TCHAR* pDriveStrings = new TCHAR[szAllDrivesStrings + sizeof(_T(""))];//�����ַ����洢�������ַ�pDriveStrings
	GetLogicalDriveStrings((DWORD)szAllDrivesStrings,pDriveStrings);//��ȡ�������ַ���pDriveStrings
	
	//��ȡ�����������ַ�������ӵ�Ŀ¼����
	//������ʱ�ַ�ָ��ָ���������ַ���Ϣ
	//(Ϊ���������ͷ��������ַ�������,���ֱ���ƶ��ַ���ָ��,�������delete�ͷ��ַ�����Դ��ʱ��ᱨ��)
	TCHAR* pTempDrive = pDriveStrings;//�̷�
	size_t szDriveString = _tcslen(pTempDrive);
	while(szDriveString > 0)
	{
		HTREEITEM hItem = InsertItemToTree(hRoot,pTempDrive,pTempDrive);		
		pTempDrive += szDriveString + 1;
		szDriveString = _tcslen(pTempDrive);
	}
	delete pDriveStrings;
}

// ��ȡ��������һ��Ŀ¼��Ŀ¼������ʾ
void CFileResTree::GetDriveDir(HTREEITEM hRoot)
{
	if (ItemHasChildren(hRoot))
	{
		HTREEITEM hChild = GetChildItem(hRoot);//��ȡ���ڵ��µĵ�һ���̷��ڵ�
		while(hChild)
		{
			//��ȡ����·��
			CString strPath = GetItemText(hChild);//��ȡ�ڵ������
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
				//�����µ��ļ���Ŀ¼���Ҳ���[.]��[..]��ʱ�򣬼��ص�Ŀ¼��
				BOOL bAdd = !fileFind.IsDots();
				if(m_bShowDirOnly)
					bAdd = bAdd && fileFind.IsDirectory();//ֻ��ʾĿ¼
				if(!m_bShowHide)
					bAdd = bAdd && !fileFind.IsHidden();//����ʾ����
				if(!m_bShowSystem)
					bAdd = bAdd && !fileFind.IsSystem();//����ʾϵͳ
				
				if(bAdd)
				{
					CString sname = fileFind.GetFileName();
					InsertItemToTree(hChild, strPath+sname, sname);
				}
			}
			//����һ��������
			hChild = GetNextItem(hChild,TVGN_NEXT);
		}
	}
}

// ��ȡĳ�ڵ���ļ�ȫ·��
CString CFileResTree::GetFullPath(HTREEITEM hParent)
{
	CString strTemp = _T("");
	CString strFullPath = _T("");
	while(hParent != m_hRoot)
	{
		//�ӵ�ǰ�ڵ����������ҳ��丸�ڵ㣬�����ڵ����
		strTemp = GetItemText(hParent);
		if(strTemp.Right(1) != _T("\\"))
			strTemp += _T("\\");
		strFullPath = strTemp + strFullPath;
		hParent = GetParentItem(hParent);
	}
	return strFullPath;
}


//ɾ��ָ���ڵ��µ������ӽڵ�
void CFileResTree::DeleteChildItems(HTREEITEM hParent)
{
	if(hParent == m_hRoot)
		return;
	
	//ɾ���ӽڵ��µ������ӽڵ�(�����յ��ӽڵ�)
	while(ItemHasChildren(hParent))
	{
		HTREEITEM hChild = GetChildItem(hParent);
		CString sPathChild = GetFullPath(hChild);
		TRACE0("ɾ��:" + sPathChild + "\n");
		DeleteItem(hChild);
	}
}


//��ʼ��ָ���ڵ�
void CFileResTree::InitItem(HTREEITEM hParent)
{	
	if(hParent == m_hRoot)
		return;

	//������е���Ŀ¼
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
			bAdd = bAdd && fileFind.IsDirectory();//ֻ��ʾĿ¼
		if(!m_bShowHide)
			bAdd = bAdd && !fileFind.IsHidden();//����ʾ����
		if(!m_bShowSystem)
			bAdd = bAdd && !fileFind.IsSystem();//����ʾϵͳ		
		if(bAdd)
		{
			bValid = TRUE;
			CString sname = fileFind.GetFileName();
			CString sPathFull = fileFind.GetFilePath();
			TRACE0("���:" + sPathParent+sname + "\n");
			//��ӽڵ�
			HTREEITEM hItenSub = InsertItemToTree(hParent,sPathParent+sname,sname);
			
			//����ýڵ������Ǻ���һ����Ч�ӽڵ㣬�����һ���սڵ㣬����ʾ�ýڵ�ǰ��"+"
			if( IsDirHasChildren(sPathFull))
			{
				HTREEITEM itemEmpty = InsertItem( "",hItenSub);//��ӿ���Ŀ¼��������ʾǰ���"+"
			}
		}
	}
}


//����ָ���Ľڵ�
void CFileResTree::UpdateItem(HTREEITEM itemCurrent)
{	
	if(!itemCurrent)
		return;

	if(itemCurrent == m_hRoot)
		return;
	
	//ɾ���ýڵ����������
	DeleteChildItems(itemCurrent);
	
	InitItem(itemCurrent);
}


//����������Ŀؼ�
void CFileResTree::UpdateRelatedCtrls()
{
	HTREEITEM itemCurrent = GetSelectedItem();
	CString sPath = GetFullPath(itemCurrent);//��ǰ·��
	
	//��ȡ��ǰĿ¼·��
	if(m_edtAddress)
	{
		if(itemCurrent == m_hRoot)
		{
			m_edtAddress->SetWindowText(_T(""));
			return;
		}
		
		m_edtAddress->SetWindowText(sPath); //������ʵ����ַ��
		if(sPath.Right(1)=="\\")
		{
			sPath += "*.*";
		}
	}
	
	if(m_lstRelated)
	{
		m_lstRelated->DeleteAllItems();//ɾ��ListView������Ԫ��
		
		//���ҵ�ǰĿ¼�µ������ļ���ӵ�ListView��
		CFileFind fileFind;
		BOOL bContinue = fileFind.FindFile(sPath);
		int iNum = 0;
		while(bContinue)
		{
			bContinue = fileFind.FindNextFile();
			BOOL bAdd = !fileFind.IsDots();
			// 			if(m_bShowDirOnly)
			// 				bAdd = bAdd && fileFind.IsDirectory();//ֻ��ʾĿ¼
			if(!m_bShowHide)
				bAdd = bAdd && !fileFind.IsHidden();//����ʾ����
			if(!m_bShowSystem)
				bAdd = bAdd && !fileFind.IsSystem();//����ʾϵͳ
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
				
				//�����ļ����Ա���
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TVITEM item = pNMTreeView->itemNew;//��ȡ��ǰչ���ڵ�
	HTREEITEM hIitemCurrent = item.hItem;//��ǰ�ڵ�
	
	//�����ǰչ���ڵ�Ϊ���ڵ�,�򷵻�
	if(hIitemCurrent == m_hRoot)
		return;
	
	if(pNMTreeView->action == 2) //չ��
	{
		UpdateItem(hIitemCurrent);
	}
	
	*pResult = 0;
}

//ѡ��ı䣬�������б��͵�ַ������Ӧ�ĸı�
void CFileResTree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	HTREEITEM itemCurrent = GetSelectedItem();
	CString sPath = GetFullPath(itemCurrent);//��ǰ·��
	
	UpdateItem(itemCurrent);
	
	//���ü����߳�����⵱ǰѡ���Ŀ¼
	if (m_pDirThread == NULL)
	{
		m_pDirThread = (CDirWatcher*)AfxBeginThread(RUNTIME_CLASS(CDirWatcher),
			THREAD_PRIORITY_LOWEST, 0, CREATE_SUSPENDED, NULL);
		
		//����1 
		CString sPath;//��ǰѡ���Ŀ¼
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


//ָ����Ŀ¼�Ƿ����ļ���Ŀ¼
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
			bAdd = bAdd && fileFind.IsDirectory();//ֻ��ʾĿ¼
		if(!m_bShowHide)
			bAdd = bAdd && !fileFind.IsHidden();//����ʾ����
		if(!m_bShowSystem)
			bAdd = bAdd && !fileFind.IsSystem();//����ʾϵͳ
		
		//if(fileFind.IsDirectory() && !fileFind.IsDots() && !fileFind.IsHidden())
		if(bAdd)
		{
			return true;//ֻҪ��һ����Ŀ¼���ͷ���true
		}
	}
	fileFind.Close();
	return false;
}


//ָ����Ŀ¼�Ƿ�Ϊ��
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
			return false;//�ҵ�һ���ļ����ļ��У���ʾ�ǿ�
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
	
	BOOL bIsFile;//�Ƿ����ļ�
	CFileFind FileFind;
	BOOL res = FileFind.FindFile(sPath);
	if (res)
	{
		if (res = FileFind.FindNextFile())
		{
			if (!FileFind.IsDirectory() && !FileFind.IsDots())//������ļ�
				bIsFile = TRUE;
		}          
	}
	FileFind.Close();
	
	//������ļ�������ϵͳ����򿪸��ļ�
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
	CMenu *pPopupMenu = menu.GetSubMenu(0);//��ȡ�˵��ĵ�һ�У���ʵ�ܹ�Ҳ��һ��
	
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