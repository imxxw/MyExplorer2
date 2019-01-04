; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFileResTree
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "myexplorer2.h"
LastPage=0

ClassCount=6
Class1=CDirWatcher
Class2=CFileResList
Class3=CFileResTree
Class4=CMyExplorer2App
Class5=CAboutDlg
Class6=CMyExplorer2Dlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDD_MYEXPLORER2_DIALOG
Resource3=IDR_TESTMENU (English (U.S.))

[CLS:CDirWatcher]
Type=0
BaseClass=CWinThread
HeaderFile=DirWatcher.h
ImplementationFile=DirWatcher.cpp

[CLS:CFileResList]
Type=0
BaseClass=CListCtrl
HeaderFile=FileResList.h
ImplementationFile=FileResList.cpp

[CLS:CFileResTree]
Type=0
BaseClass=CTreeCtrl
HeaderFile=FileResTree.h
ImplementationFile=FileResTree.cpp
Filter=W
VirtualFilter=GWC
LastObject=CFileResTree

[CLS:CMyExplorer2App]
Type=0
BaseClass=CWinApp
HeaderFile=MyExplorer2.h
ImplementationFile=MyExplorer2.cpp

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

[MNU:IDR_TESTMENU (English (U.S.))]
Type=1
Class=?
CommandCount=0

