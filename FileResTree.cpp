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


//��ʼ����һ���ڴ����ÿؼ������������
void CFileResTree::Initial()
{	
	//���������ʽ
	LONG lStyle = GetWindowLong(this->m_hWnd, GWL_STYLE);
	SetWindowLong(this->m_hWnd, GWL_STYLE, lStyle | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES /*| TVS_DISABLEDRAGDROP*/ );
	
	//����ImageList
	m_ImageList.Create(16,16,ILC_COLOR32,16,16);
	//���ñ���ɫ 
	m_ImageList.SetBkColor( GetSysColor(COLOR_WINDOW) ); 
	//��"�ҵĵ���"ͼ���������	
	int iComputerIcon = m_ImageList.Add(
		ExtractIcon( AfxGetApp()->m_hInstance, "shell32.dll", 15));
	SetImageList(&m_ImageList,TVSIL_NORMAL);//����TreeCtrl��ImageList����
	m_hRoot = this->InsertItem(_T("�ҵĵ���"),iComputerIcon,iComputerIcon,TVI_ROOT);
	
	GetLogicalDrives(m_hRoot);//��ȡ�������������ص�Ŀ¼��
	GetDriveDir(m_hRoot);//��ȡ��������һ���ļ��е�Ŀ¼��
	
	Expand(m_hRoot, TVE_EXPAND);//չ����Ŀ¼����ʾ���е��̷�
}


//������ȱ����������Ҷ�Ӧ�Ľڵ�
//���п��ܲ鲻������Ϊ����û�б������еĽڵ�
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
		CString curDir = this->GetItemText(itemCurrent);//��ǰ�ڵ������
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
			else if(this->ItemHasChildren(itemCurrent))
			{
				itemCurrent = this->GetChildItem(itemCurrent); 
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
			if(this->GetNextSiblingItem(itemCurrent) != NULL)
			{
				itemCurrent = this->GetNextSiblingItem(itemCurrent);
			}
			//�����ֵܽڵ㣬���������ӽڵ�
			else if(this->ItemHasChildren(itemCurrent))
			{
				itemCurrent = this->GetChildItem(itemCurrent); 
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
	TRACE0("���:" + strPath + "\n");
	return this->InsertItem(DisplayName,iIcon,iIcon,hParent);
	//return this->InsertItem(fileInfo.szDisplayName,iIcon,iIcon,hParent);
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
		InsertItemToTree(hRoot,pTempDrive,pTempDrive);
		pTempDrive += szDriveString + 1;
		szDriveString = _tcslen(pTempDrive);
	}
	delete pDriveStrings;
}

// ��ȡ��������һ��Ŀ¼��Ŀ¼������ʾ
void CFileResTree::GetDriveDir(HTREEITEM hRoot)
{
	if (this->ItemHasChildren(hRoot))
	{
		HTREEITEM hChild = this->GetChildItem(hRoot);//��ȡ���ڵ��µĵ�һ���̷��ڵ�
		while(hChild)
		{
			//��ȡ����·��
			CString strPath = this->GetItemText(hChild);//��ȡ�ڵ������
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
				if(fileFind.IsDirectory() && !fileFind.IsDots())
				{
					CString sname = fileFind.GetFileName();
					InsertItemToTree(hChild, strPath+sname, sname);
				}
			}
			//����һ��������
			hChild = this->GetNextItem(hChild,TVGN_NEXT);
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
CString CFileResTree::GetFullPath(HTREEITEM hCurrent)
{
	CString strTemp = _T("");
	CString strFullPath = _T("");
	while(hCurrent != m_hRoot)
	{
		//�ӵ�ǰ�ڵ����������ҳ��丸�ڵ㣬�����ڵ����
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TVITEM item = pNMTreeView->itemNew;//��ȡ��ǰչ���ڵ�

	//�����ǰչ���ڵ�Ϊ���ڵ�,�򷵻�
	if(item.hItem == m_hRoot)
		return;
	
/*
չ��һ���ڵ㣬�͸��ýڵ��������ӽڵ������һ���ӽڵ㡣
����һ���ڵ㣬�ͰѸýڵ��������ӽڵ���ӽڵ�ɾ����
�������ͱ�֤ÿ��ѡ�еĽڵ㺬��һ����Ŀ¼��
ȱ�㣺��������б��У�չ��ĳ���ڵ㣬����ڵ��µ��ӽڵ���࣬��ǳ��������ܼ�������Ϊ�ӽڵ���ӽڵ��ǳ��࣬��������Ƚ�����
*/
	CString sText = this->GetFullPath(item.hItem);
	TRACE0("��ǰĿ¼:" + sText + "\n");
	if(this->ItemHasChildren(item.hItem))
	{
		HTREEITEM hChild = this->GetChildItem(item.hItem);
		sText = this->GetFullPath(hChild);
		TRACE0("������Ŀ¼:" + sText + "\n");
		if(pNMTreeView->action == 1)  //����
		{	
			//ɾ�������ӽڵ��µ��ӽڵ�
			while(hChild!=NULL)
			{
				sText = this->GetFullPath(hChild);
				TRACE0("������Ŀ¼:" + sText + "\n");
				while(this->ItemHasChildren(hChild))
				{
					HTREEITEM item = this->GetChildItem(hChild);
					sText = this->GetFullPath(item);
					TRACE0("ɾ��:" + sText + "\n");
					this->DeleteItem(item);
				}
				hChild = this->GetNextItem(hChild,TVGN_NEXT);
			}
		}
		else if(pNMTreeView->action == 2) //չ��
		{
			//��ѵչ���ڵ��ÿ���ӽڵ㣬�����ļ�Ŀ¼��Ϣ���ӽڵ���
			//����ӽڵ���࣬��ǳ��������ܼ�������Ϊ���ӽڵ���ӽڵ���ܻ�ǳ��࣬��������Ƚ�����
			while(hChild != NULL)
			{
				sText = this->GetFullPath(hChild);
				TRACE0("չ����Ŀ¼:" + sText + "\n");
				AddSubDir(hChild);
				hChild = this->GetNextItem(hChild,TVGN_NEXT);
			}
		}
	}
	
// 	if(pNMTreeView->action == 1)  //����
// 	{
// 		if(this->ItemHasChildren(item.hItem))
// 			this->Expand(item.hItem,TVE_COLLAPSE);
// 	}
// 	else if(pNMTreeView->action == 2) //չ��
// 	{
// 		if(!this->ItemHasChildren(item.hItem))
// 		{
// 			HTREEITEM hChild = this->GetChildItem(item.hItem);
// 			//��ѵչ���ڵ��ÿ���ӽڵ㣬�����ļ�Ŀ¼��Ϣ���ӽڵ���
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
