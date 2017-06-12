// XTDirWatcher.cpp : implementation file

/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
// #include "Common/XTPVC80Helpers.h"  // Visual Studio 2005 helper functions
// #include "XTDefines.h"
// #include "XTShellPidl.h"
#include "XTDirWatcher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL IsDirExist(LPCTSTR lpszDirectoryName) {
	DWORD dwAttricutes = ::GetFileAttributes(lpszDirectoryName);
	return dwAttricutes != DWORD(-1) && (dwAttricutes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/////////////////////////////////////////////////////////////////////////////
// CXTDirWatcher

CXTDirWatcher::CXTDirWatcher()
{
	m_bAutoDelete = TRUE;//当线程停止时自动删除
	m_dwMonitorEvents[0] = INVALID_HANDLE_VALUE;
	m_dwMonitorEvents[1] = INVALID_HANDLE_VALUE;
	m_dwMonitorEvents[2] = CreateEvent(NULL, TRUE, FALSE, 0);   // Path was changed event.
	m_dwMonitorEvents[3] = CreateEvent(NULL, TRUE, FALSE, 0);   // Stop notifications event.

	ZeroMemory(&m_tvid, sizeof(m_tvid));
}

CXTDirWatcher::~CXTDirWatcher()
{
	CloseHandle(m_dwMonitorEvents[2]);
	CloseHandle(m_dwMonitorEvents[3]);
}

void CXTDirWatcher::StopNotifications()
{
	SetEvent(m_dwMonitorEvents[3]);
	WaitForSingleObject(m_hThread, INFINITE);
}


IMPLEMENT_DYNCREATE(CXTDirWatcher, CWinThread)

BEGIN_MESSAGE_MAP(CXTDirWatcher, CWinThread)
	//{{AFX_MSG_MAP(CXTDirWatcher)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CXTDirWatcher::WaitPathChangedEvent()
{
	DWORD dw = ::WaitForMultipleObjects(2,
		&m_dwMonitorEvents[2], FALSE, INFINITE);

	return dw != WAIT_OBJECT_0 + 1;
}


BOOL CXTDirWatcher::InitInstance()
{
	BOOL bContinueThread = TRUE;
	
	while (bContinueThread)
	{
		bContinueThread = MonitorNotifications();//开始监测消息
	}
	
	return FALSE;
}

BOOL CXTDirWatcher::MonitorNotifications()
{
	BOOL bContinueThread = TRUE;

	::ResetEvent(m_dwMonitorEvents[2]);
	::ResetEvent(m_dwMonitorEvents[3]);

	if (!IsPathValid(m_strFolderPath))
		return WaitPathChangedEvent();

	// 1 Watch the directory for file creation and
	// deletion.
	m_dwMonitorEvents[0] = ::FindFirstChangeNotification(
		m_strFolderPath, // directory to watch
		FALSE, // do not watch the subtree
		FILE_NOTIFY_CHANGE_FILE_NAME); // watch file name changes
	if (m_dwMonitorEvents[0] == INVALID_HANDLE_VALUE)
		return WaitPathChangedEvent();

	// 2 Watch the tree for directory creation and
	// deletion.
	m_dwMonitorEvents[1] = ::FindFirstChangeNotification(
		m_strFolderPath, // directory to watch
		FALSE, // watch the subtree
		FILE_NOTIFY_CHANGE_DIR_NAME);  // watch dir. name changes
	if (m_dwMonitorEvents[1] == INVALID_HANDLE_VALUE)
		return WaitPathChangedEvent();

	// 3 Change notification is set. Now wait on both notification
	// handles and refresh accordingly.

	BOOL bConinueNotifications = TRUE;
	while (bConinueNotifications)
	{
		// Wait for notification.
		DWORD dwWaitStatus = ::WaitForMultipleObjects(_countof(m_dwMonitorEvents),
			m_dwMonitorEvents, FALSE, INFINITE);

		switch (dwWaitStatus)
		{
		case WAIT_OBJECT_0:
			// 目录下有文件被创建或被删除或重命名
			RefreshFolder();

			if (!::FindNextChangeNotification(m_dwMonitorEvents[0]))
				bConinueNotifications = FALSE;

			if (!IsPathValid(m_strFolderPath))
				bConinueNotifications = FALSE;

			break;

		case WAIT_OBJECT_0 + 1:
			// 目录下有子目录被创建或被删除或重命名			
			RefreshTree();

			if (!::FindNextChangeNotification(m_dwMonitorEvents[1]))
				bConinueNotifications = FALSE;

			if (!IsPathValid(m_strFolderPath))
				bConinueNotifications = FALSE;

			break;

		case WAIT_OBJECT_0 + 2:
			//文件路径改变
			bContinueThread = TRUE;
			bConinueNotifications = FALSE;
			break;

		case WAIT_OBJECT_0 + 3:
			//消息停止
			bContinueThread = FALSE;
			bConinueNotifications = FALSE;
			break;
		}
	}

	// Close the file change notification handle and return.
	::FindCloseChangeNotification (m_dwMonitorEvents[0]);
	::FindCloseChangeNotification (m_dwMonitorEvents[1]);

	m_dwMonitorEvents[0] = INVALID_HANDLE_VALUE;
	m_dwMonitorEvents[1] = INVALID_HANDLE_VALUE;

	return bContinueThread;
}

BOOL CXTDirWatcher::IsPathValid(LPCTSTR lpszFolderPath)
{
	if (_tcslen(lpszFolderPath) == 0)
		return FALSE;

	if (!IsDirExist(lpszFolderPath))
		return FALSE;

	return TRUE;
}

BOOL CXTDirWatcher::SetFolderPath(CWnd* pMainWnd, LPCTSTR lpszFolderPath)
{
	if (IsPathValid(lpszFolderPath))
	{
		if (GetFolderData(lpszFolderPath, m_tvid))
		{
			m_pMainWnd = pMainWnd;
			m_strFolderPath = lpszFolderPath;

			SetEvent(m_dwMonitorEvents[2]);  // Folder was changed event

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CXTDirWatcher::SetFolderData(CWnd* pMainWnd, XT_TVITEMDATA* lpTVID)
{
	if (!lpTVID)
		return FALSE;

	TCHAR szFolderPath[_MAX_PATH];
	if (::SHGetPathFromIDList(lpTVID->lpifq, szFolderPath))
	{
		return SetFolderPath(pMainWnd, szFolderPath);
	}

	return FALSE;
}

BOOL CXTDirWatcher::GetFolderData(LPCTSTR lpszFolderPath, XT_TVITEMDATA& lpTVID)
{
	LPITEMIDLIST  pidl;
	LPSHELLFOLDER pDesktopFolder;
	OLECHAR       szOleChar[MAX_PATH];
	ULONG         chEaten;
	ULONG         dwAttributes;

	if (!IsPathValid(lpszFolderPath))
		return FALSE;

	// Get a pointer to the Desktop's IShellFolder interface.
	if (SUCCEEDED(::SHGetDesktopFolder(&pDesktopFolder)))
	{
		// IShellFolder::ParseDisplayName requires the file name be in
		// Unicode.

#if !defined(_UNICODE)
		::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpszFolderPath, -1,
			szOleChar, MAX_PATH);
#else
		STRCPY_S(szOleChar, MAX_PATH, lpszFolderPath);
#endif

		// Convert the path to an ITEMIDLIST.
		if (SUCCEEDED(pDesktopFolder->ParseDisplayName(NULL, NULL, szOleChar,
			&chEaten, &pidl, &dwAttributes)))
		{
			IShellFolder *psfMyFolder;

			lpTVID.lpi = lpTVID.lpifq = pidl;

			pDesktopFolder->BindToObject(lpTVID.lpifq, NULL,
				IID_IShellFolder, (LPVOID*)&psfMyFolder);

			lpTVID.lpsfParent = psfMyFolder;
			pDesktopFolder->Release();

			return TRUE;
		}

		pDesktopFolder->Release();
	}

	return FALSE;
}

void CXTDirWatcher::RefreshFolder()
{
	m_pMainWnd->SendMessage(XTWM_SHELL_NOTIFY,
		SHN_XT_REFRESHFOLDER, (LPARAM)&m_tvid);
}

void CXTDirWatcher::RefreshTree()
{
	m_pMainWnd->SendMessage(XTWM_SHELL_NOTIFY,
		SHN_XT_REFRESHTREE, (LPARAM)&m_tvid);
}
