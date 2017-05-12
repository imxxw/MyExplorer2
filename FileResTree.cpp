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

	m_bShowDirOnly = FALSE;//ֻ��ʾ�ļ���
	m_bShowHide = FALSE;//��ʾ�����ļ����У�
	m_bShowSystem = FALSE;//��ʾϵͳ�ļ�
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


//��ʼ�����ڴ����ÿؼ������������
void CFileResTree::Initial()
{	
	//���������ʽ
	LONG lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES /*| TVS_DISABLEDRAGDROP*/ );
	
	//����ImageList
	m_ImageList.Create(16,16,ILC_COLOR32,16,16);
	//���ñ���ɫ 
	m_ImageList.SetBkColor( GetSysColor(COLOR_WINDOW) ); 
	//��"�ҵĵ���"ͼ���������	
	int iComputerIcon = m_ImageList.Add(
		ExtractIcon( AfxGetApp()->m_hInstance, "shell32.dll", 15));
	SetImageList(&m_ImageList,TVSIL_NORMAL);//����TreeCtrl��ImageList����
	m_hRoot = InsertItem(_T("�ҵĵ���"),iComputerIcon,iComputerIcon,TVI_ROOT);
	
	GetLogicalDrives(m_hRoot);//��ȡ�������������ص�Ŀ¼��
	GetDriveDir(m_hRoot);//��ȡ��������һ���ļ��е�Ŀ¼��
	
	Expand(m_hRoot, TVE_EXPAND);//չ����Ŀ¼����ʾ���е��̷�
}


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


//����ڵ�
HTREEITEM CFileResTree::InsertItemToTree(HTREEITEM hParent, CString strPath,CString DisplayName)
{
	SHFILEINFO fileInfo;
	SHGetFileInfo(strPath,0,&fileInfo,sizeof(&fileInfo),SHGFI_ICON|SHGFI_DISPLAYNAME|SHGFI_TYPENAME);
	int iIcon = m_ImageList.Add(fileInfo.hIcon);
	HTREEITEM itemNew = InsertItem(DisplayName,iIcon,iIcon,hParent);
	TRACE0("���:" + strPath + "\n");

	DWORD dData = TYPE_EMPTY;//Ĭ���ǿ�Ŀ¼
	if(IsDirHasChildren(strPath))
		dData = TYPE_HASINVALIDSUBITEM;//��Ŀ¼�º�����Ч����Ŀ¼
	SetItemData(itemNew,dData);

	SetItemData(hParent,TYPE_HASVALIDSUBITEM);

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
	while(szDriveString>0)
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

// ����Ŀ¼������һ�ڵ��ȡ��������Ŀ¼
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
			TRACE0("���:" + strPath+sname + "\n");
			InsertItemToTree(hParent,strPath+sname,sname);
		}
	}
}

// ��ȡĳ�ڵ���ļ�·��
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


//չ��ָ���ڵ�
void CFileResTree::ExpandItem(HTREEITEM hParent)
{	
	if(ItemHasChildren(hParent))//�����ӽڵ�
	{
		if(TYPE_HASVALIDSUBITEM == GetItemData(hParent))//���ӽڵ㺬����Ч�ӽڵ�
		{
			HTREEITEM hChild = GetChildItem(hParent);
			//��ѵչ���ڵ��ÿ���ӽڵ�
			while(hChild != NULL)
			{
				//�ýڵ�������Ǻ���һ����Ч�ڵ�����ͣ��ҽڵ㲻�����ӽڵ㣬��ô�͸������һ���սڵ�
				if( TYPE_HASINVALIDSUBITEM == GetItemData(hChild)
					&& !ItemHasChildren(hChild))
				{
					HTREEITEM itemEmpty = InsertItem( "",hChild);//��ӿ���Ŀ¼��������ʾǰ���"+"
					SetItemData(itemEmpty,TYPE_INVALID);
				}
				hChild = GetNextItem(hChild,TVGN_NEXT);//ת����һ���ӽڵ�
			}
		}
		else if(TYPE_HASINVALIDSUBITEM == GetItemData(hParent))//�ýڵ�������Ǻ���һ����Ч�ڵ������
		{
			HTREEITEM hChild = GetChildItem(hParent);
			if(hChild && TYPE_INVALID == GetItemData(hChild))//�ӽڵ�����Ч�Ŀսڵ�
				DeleteItem(hChild);//ɾ����Ч�Ŀ��ӽڵ�
			
			//������е���Ŀ¼
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
					CString sname = fileFind.GetFileName();
					TRACE0("���:" + strPath+sname + "\n");
					//��ӽڵ�
					HTREEITEM hItenSub = InsertItemToTree(hParent,strPath+sname,sname);
					//����ýڵ������Ǻ���һ����Ч�ӽڵ㣬�����һ���սڵ㣬����ʾ�ýڵ�ǰ��"+"
					if( TYPE_HASINVALIDSUBITEM == GetItemData(hItenSub) 
						&& !ItemHasChildren(hItenSub))
					{
						HTREEITEM itemEmpty = InsertItem( "",hItenSub);//��ӿ���Ŀ¼��������ʾǰ���"+"
						SetItemData(itemEmpty,TYPE_INVALID);
					}
				}
			}
			SetItemData(hParent,TYPE_HASVALIDSUBITEM);
		}
	}
}



//ɾ��ָ���ڵ��µ������ӽڵ�
void CFileResTree::DeleteChildItems(HTREEITEM hParent)
{
	if(hParent == m_hRoot)
		return;
	
	//ɾ���ӽڵ��µ������ӽڵ�
	while(ItemHasChildren(hParent))
	{
		HTREEITEM hChild = GetChildItem(hParent);
		CString sText = GetFullPath(hChild);
		TRACE0("ɾ��:" + sText + "\n");
		DeleteItem(hChild);
	}
	
	CString sItemPath = GetFullPath(hParent);
	DWORD dData = TYPE_EMPTY;
	if(IsDirHasChildren(sItemPath))
		dData = TYPE_HASINVALIDSUBITEM;//��Ŀ¼�º�����Ч����Ŀ¼
	SetItemData(hParent,dData);
	
	HTREEITEM itemEmpty = InsertItem( "",hParent);//��ӿ���Ŀ¼��������ʾǰ���"+"
	SetItemData(itemEmpty,TYPE_INVALID);
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

	//�����ǰչ���ڵ�Ϊ���ڵ�,�򷵻�
	if(item.hItem == m_hRoot)
		return;

/*
����2
����ڵ㣬��ɾ���ýڵ��µ����е��ӽڵ�
չ���ڵ㣬���¼��������ӽڵ㣬��֤�˸ýڵ��¶�������
�÷����ٶȿ����
*/
	
	if(pNMTreeView->action == 1)  //����
	{
		HTREEITEM hParent = item.hItem;
		DeleteChildItems(hParent);
	}
	else	
	if(pNMTreeView->action == 2) //չ��
	{
		HTREEITEM hParent = item.hItem;
		ExpandItem(hParent);
	}
	
	*pResult = 0;
}

//ѡ��ı䣬�������б��͵�ַ������Ӧ�ĸı�
void CFileResTree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	//��ȡ��ǰĿ¼·��
	TVITEM item = pNMTreeView->itemNew;
	CString strDirPath = GetFullPath(item.hItem);
	if(m_edtAddress)
	{
		if(item.hItem == m_hRoot)
		{
			m_edtAddress->SetWindowText(_T(""));
			return;
		}
		
		m_edtAddress->SetWindowText(strDirPath); //������ʵ����ַ��
		if(strDirPath.Right(1)=="\\")
		{
			strDirPath += "*.*";
		}
	}
	
	if(m_lstRight)
	{
		m_lstRight->DeleteAllItems();//ɾ��ListView������Ԫ��
		
		//���ҵ�ǰĿ¼�µ������ļ���ӵ�ListView��
		CFileFind fileFind;
		BOOL bContinue = fileFind.FindFile(strDirPath);
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
				CString tempPath = strDirPath;
				int index = tempPath.Find(_T("*.*"));
				tempPath.Delete(index,3);
				SHGetFileInfo(tempPath + fileFind.GetFileName(),0,&fileInfo,sizeof(&fileInfo),SHGFI_ICON|SHGFI_DISPLAYNAME|SHGFI_TYPENAME);
				int iIcon = m_lstRight->GetImageList(LVSIL_SMALL)->Add(fileInfo.hIcon);
				int ret = m_lstRight->InsertItem(iNum,fileInfo.szDisplayName,iIcon);
				
				m_lstRight->SetItemText(iNum,2,fileInfo.szTypeName);
				
				//�����ļ����Ա���
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
