// FileResList.cpp : implementation file
//

#include "stdafx.h"
#include "myexplorer2.h"
#include "FileResList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileResList

CFileResList::CFileResList()
{
}

CFileResList::~CFileResList()
{
}


BEGIN_MESSAGE_MAP(CFileResList, CListCtrl)
	//{{AFX_MSG_MAP(CFileResList)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileResList message handlers


void CFileResList::Initial()
{	
	this->SetExtendedStyle(this->GetExtendedStyle()|LVS_EX_FULLROWSELECT );//�������
	
	//������ۣ���ʽ LVS_ICON��LVS_SMALLICON��LVS_LIST �� LVS_REPORT ָ�������б�ؼ� (List Control) ��ͼ��
	LONG lStyle = GetWindowLong(this->m_hWnd, GWL_STYLE);
	SetWindowLong(this->m_hWnd, GWL_STYLE, lStyle | LVS_REPORT );

	m_RightImageList.Create(16,16,ILC_COLOR32,16,16);
	this->SetImageList(&m_RightImageList,LVSIL_SMALL);//����ListCtrl��ImageList����
	
	this->InsertColumn(0,_T("����"),LVCFMT_LEFT,120);
	this->InsertColumn(1,_T("��С"),LVCFMT_LEFT,60);
	this->InsertColumn(2,_T("����"),LVCFMT_LEFT,80);
	this->InsertColumn(3,_T("�޸�����"),LVCFMT_LEFT,150);	
}


int CALLBACK CFileResList::ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CFileResList *sort=(CFileResList *)lParamSort;
	CString string1 = sort->GetItemText(lParam1,sort->GetSortedCol());
	CString string2= sort->GetItemText(lParam2,sort->GetSortedCol());
	
	//if(sort->GetAsc()==TRUE)
	//	return string1>string2;
	//else 
	//	return string1<string2;
	
	if(sort->GetAsc()==TRUE)
		return string1.CompareNoCase(string2);
	else 
		return string1.CompareNoCase(string2)*(-1);
	
}

void CFileResList::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int nCouter,i;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	 
	CFileResList *m_FileList=this;
	if(pNMListView->iSubItem == m_nSortedCol)
	{
		m_bAsc=!m_bAsc;
	}
	else
	{
		m_bAsc=this->GetStyle()&LVS_SORTASCENDING;;
		m_nSortedCol=pNMListView->iSubItem;
	}
	nCouter=m_FileList->GetItemCount();
	for (i=0;i<nCouter;i++)
	{
		CString strText = m_FileList->GetItemText(i,m_nSortedCol);
		m_FileList->SetItemData(i,i);
	}
	m_FileList->SortItems(ListCompare,(DWORD)m_FileList);
	*pResult = 0;
}
