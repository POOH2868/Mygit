// UsbHidThread.cpp : implementation file
//

#include "stdafx.h"
#include "TsmTestThread.h"
#include "Global.h"
#include "util.h"

#include "resource.h"
#include "TestModeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTsmTestThread

IMPLEMENT_DYNCREATE(CTsmTestThread, CWinThread)

CTsmTestThread::CTsmTestThread()
{
	m_pUsbHid = NULL;

	m_ThreadState = THREAD_STATE_NONE;
}

CTsmTestThread::~CTsmTestThread()
{
}

BOOL CTsmTestThread::InitInstance()
{
	return TRUE;
}

int CTsmTestThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CTsmTestThread, CWinThread)
	//{{AFX_MSG_MAP(CTsmTestThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_THREAD_MESSAGE(WM_USER_TSM_TEST_START, CTsmTestThread::OnTsmTest)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTsmTestThread message handlers

void CTsmTestThread::SetUsbHidInfo(CUsbHid * pUsbHid)
{
	m_pUsbHid = pUsbHid;
}

void CTsmTestThread::SendThreadExitEvent()
{
	BOOL bRet;

	ResumeThread();

	do
	{
		bRet = PostThreadMessage(WM_QUIT, 0, 0);
	}while( bRet == 0 );
}

void CTsmTestThread::StartTsmTest()
{
	BOOL bRet;

	ResumeThread();

	do
	{
		bRet = PostThreadMessage(WM_USER_TSM_TEST_START, NULL, NULL);
	}while( bRet == 0 );
}

void CTsmTestThread::OnTsmTest(WPARAM wParam, LPARAM lParam)
{
	CUsbHid * pUsbHid = m_pUsbHid;
	if (pUsbHid == NULL)
		return;
	
	CTestModeView * pTestModeView = (CTestModeView*)g_pTestModeViewWnd;
	if (pTestModeView == NULL)
		return;

	m_ThreadState = THREAD_STATE_RUNNING;

	pTestModeView->TSM_MainTest(pUsbHid);

	m_ThreadState = THREAD_STATE_NONE;

	SuspendThread();
}
