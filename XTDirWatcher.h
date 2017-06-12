// XTDirWatcher.h : header file
#if !defined(__XTDIRWATCHER_H__)
#define __XTDIRWATCHER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const UINT WM_XTP_CONTROLS_BASE = (WM_USER + 9000);
const UINT XTWM_SHELL_NOTIFY = (WM_XTP_CONTROLS_BASE + 20);
const int SHN_XT_REFRESHFOLDER   = 7; //<ALIAS XTWM_SHELL_NOTIFY>
const int SHN_XT_REFRESHTREE     = 8; //<ALIAS XTWM_SHELL_NOTIFY>

// determine number of elements in an array (not bytes)
#define _countof(array) (sizeof(array)/sizeof(array[0]))

// ----------------------------------------------------------------------
// Summary:
//     XT_TVITEMDATA structure is used to maintain information for
//     a particular tree item in a CXTShellTreeCtrl or CXTShellTreeView
//     window.
// See Also:
//     CXTShellTreeCtrl, CXTShellTreeView
// ----------------------------------------------------------------------
struct XT_TVITEMDATA
{
	LPITEMIDLIST    lpi;        // Pointer to an item ID list.
	LPITEMIDLIST    lpifq;      // Pointer to an item ID list.
	LPSHELLFOLDER   lpsfParent; // Pointer to the parent shell folder item.
};

//�ж��ļ��Ƿ����
BOOL IsDirExist(LPCTSTR lpszDirectoryName);

//===========================================================================
// Summary:
//     The CXTDirWatcher is a thread class that will monitor a specified
//     path for file and directory changes to occur.
// Remarks:
//     Whenever an item is created, deleted or renamed the thread will send a notification to
//     the its owner in the form of a XTWM_SHELL_NOTIFY message.  The
//     WPARAM value for the notification will be set to SHN_XT_TREESELCHANGE
//     if the folder list contents require updating or SHN_XT_CONTENTSCHANGED
//     if the tree contents require updating.  The LPARAM value is set to
//     the address of a XT_TVITEMDATA structure that contains information
//     the path monitored.
//
// Example: To use CXTDirWatcher, add a CXTDirWatcher* m_pDirThread member
//          to your class and instantiate a new thread using AfxBeginThread:
//
// <code>
// // Start the directory monitoring thread.
// m_pDirThread = (CXTDirWatcher*)AfxBeginThread(RUNTIME_CLASS(CXTDirWatcher),
//     THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL);
// </code>
// You can then specify what directory you wish to monitor by calling SetFolderData
// or SetFolderPath:
// <code>
// // Specify the folder to monitor.
// m_pDirThread->SetFolderData(this, lpTVID);
// m_pDirThread->ResumeThread();
// </code>
//
//          You can add a message handler to the window receiving messages and handle your
//          change event notifications.  The thread will send two notifications, the wParam indicates
//          what event has occurred.  If wParam equal HN_XT_REFRESHFOLDER this indicates that the
//          folder list has changed and needs to be updated.  If wParam equals SHN_XT_REFRESHTREE this
//          indicates the folder tree has changed and needs to be updated.  The lParam value represents
//          a pointer to a XT_TVITEMDATA structure for the folder:
//
// <code>
// BEGIN_MESSAGE_MAP(CNotifyWnd, CWnd)
//     //{{AFX_MSG_MAP(CNotifyWnd)
//     //}}AFX_MSG_MAP
//     ON_MESSAGE(XTWM_SHELL_NOTIFY, OnUpdateShell)
// END_MESSAGE_MAP()
//
// LRESULT CNotifyWnd::OnUpdateShell(WPARAM wParam, LPARAM lParam)
// {
//     switch (wParam)
//     {
//         case SHN_XT_REFRESHFOLDER:
//         case SHN_XT_REFRESHTREE:
//         {
//             // Directory monitory thread has issued an update notification,
//             // refresh the list control.
//             XT_TVITEMDATA*  lpTVID = (XT_TVITEMDATA*)lParam;
//             ASSERT(lpTVID);
//
//             PopulateListView(lpTVID, lpTVID->lpsfParent);
//             break;
//         }
//
//         default:
//             break;
//     }
//
//     return 0;
// }
// </code>
//
//          When you are finished using the CXTDirWatcher thread, you will need to free the
//          memory associated with the object. To do so you can use the SAFE_DELETE
//          macro like so:
//
// <code>
// SAFE_DELETE(m_pDirThread);
// </code>
//
//          The CXTDirWatcher class is used internally by the CXTShellListCtrl and
//          CXTShellListView classes to handle refreshing the list when a file or directory
//          change occurs.
//===========================================================================
class CXTDirWatcher : public CWinThread
{
	DECLARE_DYNCREATE(CXTDirWatcher)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTDirWatcher object
	//-----------------------------------------------------------------------
	CXTDirWatcher();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTDirWatcher object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTDirWatcher();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to monitor the directory specified by strPath.
	// Parameters:
	//     pMainWnd       - Points to the window to receive change notifications.
	//     lpszFolderPath - Full path of directory to monitor.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL SetFolderPath(CWnd* pMainWnd, LPCTSTR lpszFolderPath);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve the full path to the directory that
	//     is currently monitored.
	// Returns:
	//     A CString object representing the path monitored.
	//-----------------------------------------------------------------------
	CString GetFolderPath() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to monitor the directory using a pointer
	//     to a XT_TVITEMDATA pointer.
	// Parameters:
	//     pMainWnd - Pointer to the window to receive update notifications.
	//     lpTVID   - Pointer to tree view item data.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL SetFolderData(CWnd* pMainWnd, XT_TVITEMDATA* lpTVID);

	// ----------------------------------------------------------------------
	// Summary:
	//     Constructs or retrieves XT_TVITEMDATA structure information.
	// Parameters:
	//     lpszFolderPath -  Full path to directory.
	//     lpTVID -          Reference to tree view item data.
	// Remarks:
	//     In the first version this member function is called to return the
	//     a pointer to a XT_TVITEMDATA structure for the directory that is
	//     currently monitored. In the second version the function constructs
	//     a XT_TVITEMDATA structure from the specified path.
	// Returns:
	//     In the first version a pointer to a XT_TVITEMDATA structure. In
	//     the second version TRUE if successful, otherwise FALSE.
	// ----------------------------------------------------------------------
	XT_TVITEMDATA* GetFolderData();
	BOOL GetFolderData(LPCTSTR lpszFolderPath, XT_TVITEMDATA& lpTVID); //<combine CXTDirWatcher::GetFolderData>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the specified path
	//     is a valid directory name.
	// Parameters:
	//     lpszFolderPath - Full path to directory.
	// Returns:
	//     TRUE if the path is valid, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsPathValid(LPCTSTR lpszFolderPath);

	void StopNotifications();

public:
//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_VIRTUAL(CXTDirWatcher)
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL
//}}AFX_CODEJOCK_PRIVATE

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the thread whenever the monitored
	//     directory list needs to be refreshed.
	//-----------------------------------------------------------------------
	virtual void RefreshFolder();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the thread whenever the monitored
	//     directory tree needs to be refreshed.
	//-----------------------------------------------------------------------
	virtual void RefreshTree();

	BOOL MonitorNotifications();

	BOOL WaitPathChangedEvent();

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTDirWatcher)
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:

	/*һ���ĸ����
	m_dwMonitorEvents[0] : �ļ����޸�
	m_dwMonitorEvents[1] : Ŀ¼���޸�
	m_dwMonitorEvents[2] : ·���޸�
	m_dwMonitorEvents[3] : ֹͣ��Ϣ
	*/
	HANDLE          m_dwMonitorEvents[4];   // Change event handles.

	CString         m_strFolderPath;        // Path that is monitored.

	XT_TVITEMDATA   m_tvid;                 // Tree view item data.
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE CString CXTDirWatcher::GetFolderPath() const {
	return m_strFolderPath;
}
AFX_INLINE XT_TVITEMDATA* CXTDirWatcher::GetFolderData() {
	return &m_tvid;
}

#endif // !defined(__XTDIRWATCHER_H__)
