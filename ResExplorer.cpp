// CResExplorer.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyExplorer2.h"
#include "ResExplorer.h"


// CResExplorer

IMPLEMENT_DYNAMIC(CResExplorer, CTreeCtrl)

CResExplorer::CResExplorer()
{
}

CResExplorer::~CResExplorer()
{
}


BEGIN_MESSAGE_MAP(CResExplorer, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpandedTreedir)
END_MESSAGE_MAP()



// CResExplorer ��Ϣ�������
void CResExplorer::Initial()
{
	//����ImageList
	m_ImageList.Create(16,16,ILC_COLOR32,16,16);
	//���ñ���ɫ 
	m_ImageList.SetBkColor( GetSysColor(COLOR_WINDOW) ); 

	this->SetImageList(&m_ImageList,TVSIL_NORMAL);               //����TreeCtrl��ImageList����

	m_hRoot = this->InsertItem(_T("�ҵĵ���"),0,0,TVI_ROOT);
	GetLogicalDrives(m_hRoot);						//��ȡ�������������ص�Ŀ¼��
	GetDriveDir(m_hRoot);							//��ȡ��������һ���ļ��е�Ŀ¼��

	//Expand(GetRootItem(), TVE_EXPAND);	
	Expand(m_hRoot, TVE_EXPAND);
}

HTREEITEM CResExplorer::FindItemText(CTreeCtrl* tree,HTREEITEM item, CString strtext) 
{   
	//��ȱ�����
  HTREEITEM hfind;    

  //������ֱ�ӷ���NULL
  if(item == NULL)   
	return NULL;  

  //��������
  while(item!=NULL) 
  {   
	  //��ǰ�ڵ㼴������ҽڵ�
	  if(tree->GetItemText(item) == strtext)   
		  return item;   	    

	  //���ҵ�ǰ�ڵ���ӽڵ�
	  if(tree->ItemHasChildren(item))  
	  {
		  item = tree->GetChildItem(item); 
		  //�ݹ���ò����ӽڵ��½ڵ�
		  hfind = FindItemText(tree,item,strtext);
		  if(hfind)   
		  {   
			return hfind;  
		  }   
		  else //�ӽڵ���δ��������ڵ㣬���������ֵܽڵ�
			item = tree->GetNextSiblingItem(tree->GetParentItem(item)); 
	  } 
	  else
	  { //�����ӽڵ㣬���������ֵܽڵ�
		 item = tree->GetNextSiblingItem(item);   
	  }   

	}
    return item;   
}

HTREEITEM CResExplorer::FindItem(HTREEITEM item,CString strPath)
{
	//��ȱ�����
	HTREEITEM hfind;
	if(strPath.Right(1) == "\\")  //ȥ���Ҳ�'\'
	{
		strPath.Delete(strPath.GetLength()-1);
	}
	CString DirName;
	int pos = strPath.Find('\\');
	if(pos>0)
	    DirName = strPath.Left(pos);
	else
		DirName = strPath;

	//��������
   while(item!=NULL) 
   {   
	   CString curDir = this->GetItemText(item);
	   //ȥ�����ұߵ�"\"
		if(curDir.Right(1) == "\\")
		{
			curDir.Delete(curDir.GetLength()-1);
		}
	  //��ǰ�ڵ㼴������ҽڵ�
		if(curDir.CompareNoCase(DirName) == 0)
	  {
		  if(curDir.CompareNoCase(strPath) == 0)
			  return item; 
		  //����ҵ��ڵ㣬�����������ӽڵ�
		  else if(this->ItemHasChildren(item))
		  {
			  item = this->GetChildItem(item); 
			  //�ݹ���ò����ӽڵ��½ڵ�
			  hfind = FindItem(item,strPath.Mid(pos+1));
			  if(hfind)   
			  {   
				return hfind;  
			  }
		  }
		  return NULL;  //δ���ҵ� 
	  }
	  else
	  {
		  //�����ֵܽڵ�
		  if(this->GetNextSiblingItem(item) != NULL)
		  {
			  item = this->GetNextSiblingItem(item);
		  }
          //�����ֵܽڵ㣬���������ӽڵ�
		  else if(this->ItemHasChildren(item))
		  {
			  item = this->GetChildItem(item); 
		  }
		  else
			  return NULL;     //δ���ҵ� 

		  hfind = FindItem(item,strPath);
		  if(hfind)   
		  {   
			return hfind;  
		  }
	  }
   }
	return item;
}

// ��ȡ�̷���Ŀ¼������Ϊ��Ŀ¼��һ���ڵ�
void CResExplorer::GetLogicalDrives(HTREEITEM hRoot)
{
	size_t szAllDrivesStrings = GetLogicalDriveStrings(0,NULL);							//��ȡ�������ַ�����
	TCHAR* pDriveStrings = new TCHAR[szAllDrivesStrings + sizeof(_T(""))];				//�����ַ����洢�������ַ�pDriveStrings
	GetLogicalDriveStrings((DWORD)szAllDrivesStrings,pDriveStrings);					//��ȡ�������ַ���pDriveStrings
	//��ȡ�����������ַ�������ӵ�Ŀ¼����
	//������ʱ�ַ�ָ��ָ���������ַ���Ϣ
	//(Ϊ���������ͷ��������ַ�������,���ֱ���ƶ��ַ���ָ��,�������delete�ͷ��ַ�����Դ��ʱ��ᱨ��)
	TCHAR* pTempDrive = pDriveStrings;
	size_t szDriveString = _tcslen(pTempDrive);
	while(szDriveString>0)
	{
		//this->InsertItem(pTempDrive,hRoot);					//���ؽ������һ����������Ϣ��Ŀ¼��
		InsertItemToTree(hRoot,pTempDrive,pTempDrive);
		pTempDrive += szDriveString + 1;
		szDriveString = _tcslen(pTempDrive);
	}
	delete pDriveStrings;
}

// ��ȡ��������һ��Ŀ¼��Ŀ¼������ʾ
void CResExplorer::GetDriveDir(HTREEITEM hRoot)
{
	HTREEITEM hChild = this->GetChildItem(hRoot);				//��ȡ���ڵ��µĵ�һ���̷��ڵ�
	while(hChild)
	{
		//��ȡ����·��
		CString strText = this->GetItemText(hChild);
		if(strText.Right(1) != _T("\\"))
		{
			strText += _T("\\");
		}
		strText += "*.*";
		CFileFind fileFind;
		BOOL bContinue = fileFind.FindFile(strText);
		while(bContinue)
		{
			bContinue = fileFind.FindNextFile();
			//�����µ��ļ���Ŀ¼���Ҳ���[.]��[..]��ʱ�򣬼��ص�Ŀ¼��
			if(fileFind.IsDirectory() && !fileFind.IsDots())
			{
				CString tempPath = strText;
				int index = tempPath.Find(_T("*.*"));
				tempPath.Delete(index,3);
				InsertItemToTree(hChild,tempPath+fileFind.GetFileName(),fileFind.GetFileName());
			}
		}
		//����һ��������
		hChild = this->GetNextItem(hChild,TVGN_NEXT);
	}
}

// ����Ŀ¼������һ�ڵ��ȡ��������Ŀ¼
void CResExplorer::AddSubDir(HTREEITEM hParent)
{
	CString strPath = GetFullPath(hParent);
	if(strPath.Right(1) != "\\")
	{
		strPath += "\\";
	}
	strPath += "*.*";
	CFileFind fileFind;
	BOOL bContinue = fileFind.FindFile(strPath);
	while(bContinue)
	{
		bContinue = fileFind.FindNextFile();
		if(fileFind.IsDirectory() && !fileFind.IsDots()&&!fileFind.IsHidden())
		{
			//this->InsertItem(fileFind.GetFileName(),hParent);
			CString tempPath = strPath;
			int index = tempPath.Find(_T("*.*"));
			strPath.Delete(index,3);
			CString sname = fileFind.GetFileName();
			InsertItemToTree(hParent,tempPath+sname,sname);
		}
	}
}

// ��ȡĳ�ڵ���ļ�·��
CString CResExplorer::GetFullPath(HTREEITEM hCurrent)
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

// չ��ĳ�ڵ㴥���¼�
void CResExplorer::OnItemexpandedTreedir(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TVITEM item = pNMTreeView->itemNew;				//��ȡ��ǰչ���ڵ�
	//�����ǰչ���ڵ�Ϊ���ڵ�,�򷵻�
	if(item.hItem == m_hRoot)
		return;
	HTREEITEM hChild = this->GetChildItem(item.hItem);
	if(pNMTreeView->action == 1)  //����
	{	
		while(hChild!=NULL)
		{
			while(this->ItemHasChildren(hChild))
			{
				HTREEITEM item = this->GetChildItem(hChild);
				this->DeleteItem(item);
			}
		    hChild = this->GetNextItem(hChild,TVGN_NEXT);
		}
	}
	else if(pNMTreeView->action == 2) //չ��
	{
		//��ѵչ���ڵ��ÿ���ӽڵ㣬�����ļ�Ŀ¼��Ϣ���ӽڵ���
		while(hChild!=NULL)
		{
			AddSubDir(hChild);
			hChild = this->GetNextItem(hChild,TVGN_NEXT);
		}
	}

	*pResult = 0;
}


void CResExplorer::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	this->Expand(this->GetSelectedItem(),TVE_EXPAND);

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
// 	m_lstRight->DeleteAllItems();					//ɾ��ListView������Ԫ��
// 	//��ȡ��ǰĿ¼·��
// 	TVITEM item = pNMTreeView->itemNew;
// 	if(item.hItem == m_hRoot)
// 	{
// 		m_edtAddress->SetWindowText(_T(""));
// 		return;
// 	}
// 	CString strDirPath = GetFullPath(item.hItem);
// 	m_edtAddress->SetWindowText(strDirPath); //������ʵ����ַ��
// 	if(strDirPath.Right(1)=="\\")
// 	{
// 		strDirPath += "*.*";
// 	}
// 	//���ҵ�ǰĿ¼�µ������ļ���ӵ�ListView��
// 	CFileFind fileFind;
// 	BOOL bContinue = fileFind.FindFile(strDirPath);
// 	int iNum = 0;
// 	while(bContinue)
// 	{
// 		bContinue = fileFind.FindNextFile();
// 		if(/*!fileFind.IsDirectory() && */!fileFind.IsDots()&&!fileFind.IsHidden())
// 		{
// 			SHFILEINFO fileInfo;
// 			CString tempPath = strDirPath;
// 			int index = tempPath.Find(_T("*.*"));
// 			tempPath.Delete(index,3);
// 			SHGetFileInfo(tempPath + fileFind.GetFileName(),0,&fileInfo,sizeof(&fileInfo),SHGFI_ICON|SHGFI_DISPLAYNAME|SHGFI_TYPENAME);
// 			int iIcon = m_lstRight->GetImageList(LVSIL_SMALL)->Add(fileInfo.hIcon);
// 			int ret = m_lstRight->InsertItem(iNum,fileInfo.szDisplayName,iIcon);
// 			
// 			m_lstRight->SetItemText(iNum,2,fileInfo.szTypeName);
// 
// 			//�����ļ����Ա���
// 			CFileStatus status;
// 			CFile::GetStatus(tempPath + fileFind.GetFileName(), status);
// 			CString strSize;
// 			strSize.Format(_T("%d B"),status.m_size);
//             m_lstRight->SetItemText(iNum,1,strSize);
// 
// 			CString strModifyTime = status.m_ctime.Format("%Y-%m-%d %H:%M:%S");
// 			m_lstRight->SetItemText(iNum,3,strModifyTime);
// 
// 			iNum++;
// 		}
// 	}
	 *pResult = 0;
}

HTREEITEM CResExplorer::InsertItemToTree(HTREEITEM hParent, CString strPath,CString DisplayName)
{
	SHFILEINFO fileInfo;
	SHGetFileInfo(strPath,0,&fileInfo,sizeof(&fileInfo),SHGFI_ICON|SHGFI_DISPLAYNAME|SHGFI_TYPENAME);
	int iIcon = m_ImageList.Add(fileInfo.hIcon);
	return this->InsertItem(/*fileInfo.szDisplayName*/DisplayName,iIcon,iIcon,hParent);
}
