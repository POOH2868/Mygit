// TouchDebugger.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "TouchDebuggerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTouchDebuggerApp

BEGIN_MESSAGE_MAP(CTouchDebuggerApp, CWinApp)
	//ON_COMMAND(ID_HELP, CWinApp::OnHelp)			// Disable VK_F1
END_MESSAGE_MAP()


// CTouchDebuggerApp 생성

CTouchDebuggerApp::CTouchDebuggerApp()
{
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CTouchDebuggerApp 개체입니다.

CTouchDebuggerApp theApp;


// CTouchDebuggerApp 초기화

BOOL CTouchDebuggerApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControls()가 필요합니다. 
	// InitCommonControls()를 사용하지 않으면 창을 만들 수 없습니다.
	
	InitCommonControls();

	AfxInitRichEdit();
	
	CWinApp::InitInstance();

	/* Program 한번만 뛰우기 */
	HANDLE hInst = NULL;
	::CreateMutex(NULL, TRUE, _T(AfxGetAppName()));

	if (::GetLastError() == ERROR_ALREADY_EXISTS)		exit(0);
	else if (!hInst)		::ReleaseMutex(hInst);
	
	AfxEnableControlContainer();
	
	CTouchDebuggerDlg dlg;
	m_pMainWnd = &dlg;
	
	INT_PTR nResponse = dlg.DoModal();
	
	if (nResponse == IDOK)
	{
		// 코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// 코드를 배치합니다.
	}

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고
	// 응용 프로그램을 끝낼 수 있도록 FALSE를 반환합니다.
	return FALSE;
}

