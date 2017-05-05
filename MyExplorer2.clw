; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFileResList
LastTemplate=CListCtrl
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "myexplorer2.h"
LastPage=0

ClassCount=5
Class1=CMyExplorer2App
Class2=CAboutDlg
Class3=CMyExplorer2Dlg

ResourceCount=2
Resource1=IDD_ABOUTBOX
Class4=CDirTree
Class5=CFileResList
Resource2=IDD_MYEXPLORER2_DIALOG

[CLS:CMyExplorer2App]
Type=0
BaseClass=CWinApp
HeaderFile=MyExplorer2.h
ImplementationFile=MyExplorer2.cpp
LastObject=CMyExplorer2App

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=MyExplorer2Dlg.cpp
ImplementationFile=MyExplorer2Dlg.cpp
LastObject=CAboutDlg

[CLS:CMyExplorer2Dlg]
Type=0
BaseClass=CDialog
HeaderFile=MyExplorer2Dlg.h
ImplementationFile=MyExplorer2Dlg.cpp
LastObject=IDC_BTN_GOTO
Filter=D
VirtualFilter=dWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_MYEXPLORER2_DIALOG]
Type=1
Class=CMyExplorer2Dlg
ControlCount=5
Control1=IDC_TREE,SysTreeView32,1350633472
Control2=IDC_STATIC,static,1342308352
Control3=IDC_EDIT_ADD,edit,1350631552
Control4=IDC_BTN_GOTO,button,1342242817
Control5=IDC_LIST,SysListView32,1350631441

[CLS:CDirTree]
Type=0
HeaderFile=DirTree.h
ImplementationFile=DirTree.cpp
BaseClass=CTreeCtrl
Filter=W
LastObject=CDirTree
VirtualFilter=GWC

[CLS:CFileResList]
Type=0
HeaderFile=FileResList.h
ImplementationFile=FileResList.cpp
BaseClass=CListCtrl
Filter=W
VirtualFilter=FWC
LastObject=CFileResList

