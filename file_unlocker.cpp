
#include "stdafx.h"

#if defined(D_HV_FILE_UNLOCKER)
#include <RestartManager.h>

#define TA_FAILED			0
#define TA_SUCCESS_CLEAN	1
#define TA_SUCCESS_KILL		2
#define TA_SUCCESS_16		3

extern DWORD WINAPI TerminateApp(DWORD dwPID, DWORD dwTimeout);

static CStringArray m_strLockInfo;


static BOOL FileUnlocker_GetInfo(CString strFileName)
{
	DWORD dwSession;
	WCHAR fullProcessNameSessionKey[CCH_RM_SESSION_KEY + 1] = { 0 };
	DWORD dwError;

	m_strLockInfo.RemoveAll();

	// start up the Start manager. This api was created for install utilities to minimize reboots
	dwError = RmStartSession(&dwSession, 0, fullProcessNameSessionKey);
	if (dwError != ERROR_SUCCESS)
	{
		TRACE(_T("Unable to initialize the RestartManager."));
		return FALSE;
	}

	// tell the start  manager what file we want information for
#ifdef UNICODE
	PCWSTR pfullProcessNameFile = strFileName.GetBuffer();
#else
USES_CONVERSION;
	PCWSTR pfullProcessNameFile = T2CW(strFileName.GetBuffer());
#endif

	dwError = RmRegisterResources(dwSession, 1, &pfullProcessNameFile, 0, NULL, 0, NULL);
	if (dwError != ERROR_SUCCESS)
	{
		TRACE(_T("Unable to RegsiterResources for the file."));
		return FALSE;
	}

	// call it with zero to get the number of processes, yup, that is how you ask
	DWORD dwReason;
	UINT nProcInfoNeeded = 0;
	UINT nProcInfo = 0;
	RM_PROCESS_INFO * rgpi = NULL;
	CString text;

	rgpi = new RM_PROCESS_INFO;
	dwError = RmGetList(dwSession, &nProcInfoNeeded, &nProcInfo, rgpi, &dwReason);
	delete rgpi;

	if (dwError != ERROR_SUCCESS && dwError != ERROR_MORE_DATA)
	{
		TRACE(_T("RMGetList failedfor the file. [0]"));
		return FALSE;
	}

	if (nProcInfoNeeded <= 0)
	{
		return FALSE;
	}

	// something does have our file open, get the details
	WCHAR fullProcessName[MAX_PATH];
	DWORD cch = MAX_PATH;

	nProcInfo = nProcInfoNeeded;
	rgpi = new RM_PROCESS_INFO[nProcInfoNeeded];

	dwError = RmGetList(dwSession, &nProcInfoNeeded, &nProcInfo, rgpi, &dwReason);

	if (dwError != ERROR_SUCCESS)
	{
		if (rgpi)
			delete rgpi;

		TRACE(_T("RMGetList failedfor the file. [1]"));
		return FALSE;
	}

	HANDLE hProcess;
	FILETIME ftCreate, ftExit, ftKernel, ftUser;

	for (UINT i = 0; i < nProcInfo; i++)
	{
		hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, rgpi[i].Process.dwProcessId);

		fullProcessName[0] = 0;

		if (hProcess)
		{
			if (GetProcessTimes(hProcess, &ftCreate, &ftExit, &ftKernel, &ftUser) && CompareFileTime(&rgpi[i].Process.ProcessStartTime, &ftCreate) == 0)
			{
				QueryFullProcessImageNameW(hProcess, 0, fullProcessName, &cch);
			}
			CloseHandle(hProcess);
		}

		text.Format(_T("App name: %ls  Full Process Name: %ls  App type: %d  Process ID: %d"),
			rgpi[i].strAppName, fullProcessName, rgpi[i].ApplicationType, rgpi[i].Process.dwProcessId);

		m_strLockInfo.Add(text);
	}

	if (rgpi)
		delete rgpi;

	return TRUE;
}

void FileUnlocker_Unlock(CString strFileName)
{
	if (FileUnlocker_GetInfo(strFileName) == FALSE)
		return;

	int nCount = m_strLockInfo.GetCount();

	if (nCount == 0)
	{
		TRACE(_T("No Locks."));
		return;
	}

	UINT start, end;
	CString text, temp;
	CString procName;
	DWORD procID;

	for (int i = 0; i < nCount; i++)
	{
		// get the app name from the list
		text = m_strLockInfo.GetAt(i);

		start = text.Find(_T("Full Process Name"), 0) + 19;
		end = text.Find(_T("App type"), 0) - 1;
		procName = text.Mid(start, end - start);

		// get the process ID
		start = text.Find(_T("Process ID"), 0) + 12;
		temp = text.Mid(start);
		
		// we put that ID there so we are assuming it is a valid number
#ifdef UNICODE
		procID = _wtol(temp.GetBuffer());
#else
		procID = atol(temp.GetBuffer());
#endif

		// the user has asked us to stop this process!
		TerminateApp(procID, 1000);
	}
}

BOOL CALLBACK TerminateAppEnum(HWND hwnd, LPARAM lParam)
{
	DWORD dwID;

	GetWindowThreadProcessId(hwnd, &dwID);

	if (dwID == (DWORD)lParam)
	{
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	}

	return TRUE;
}

/*----------------------------------------------------------------
DWORD WINAPI TerminateApp( DWORD dwPID, DWORD dwTimeout )

Purpose:
Shut down a 32-Bit Process (or 16-bit process under Windows 95)

Parameters:
dwPID
Process ID of the process to shut down.

dwTimeout
Wait time in milliseconds before shutting down the process.

Return Value:
TA_FAILED - If the shutdown failed.
TA_SUCCESS_CLEAN - If the process was shutdown using WM_CLOSE.
TA_SUCCESS_KILL - if the process was shut down with TerminateProcess().
NOTE:  See header for these defines.
----------------------------------------------------------------*/
DWORD WINAPI TerminateApp(DWORD dwPID, DWORD dwTimeout)
{
	HANDLE   hProc;
	DWORD   dwRet;

	// If we can't open the process with PROCESS_TERMINATE rights, then we give up immediately.
	hProc = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, dwPID);

	if (hProc == NULL)
	{
		return TA_FAILED;
	}

	// TerminateAppEnum() posts WM_CLOSE to all windows whose PID matches your process's.
	EnumWindows((WNDENUMPROC)TerminateAppEnum, (LPARAM)dwPID);

	// Wait on the handle. If it signals, great. If it times out, then you kill it.
	if (WaitForSingleObject(hProc, dwTimeout) != WAIT_OBJECT_0)
		dwRet = TerminateProcess(hProc, 0) ? TA_SUCCESS_KILL : TA_FAILED;
	else
		dwRet = TA_SUCCESS_CLEAN;

	CloseHandle(hProc);

	return dwRet;
}
#endif
