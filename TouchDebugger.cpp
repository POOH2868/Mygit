// TouchDebugger.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
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


// CTouchDebuggerApp ����

CTouchDebuggerApp::CTouchDebuggerApp()
{
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}


// ������ CTouchDebuggerApp ��ü�Դϴ�.

CTouchDebuggerApp theApp;


// CTouchDebuggerApp �ʱ�ȭ

BOOL CTouchDebuggerApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControls()�� �ʿ��մϴ�. 
	// InitCommonControls()�� ������� ������ â�� ���� �� �����ϴ�.
	
	InitCommonControls();

	AfxInitRichEdit();
	
	CWinApp::InitInstance();

	/* Program �ѹ��� �ٿ�� */
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
		// �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// �ڵ带 ��ġ�մϴ�.
	}

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�
	// ���� ���α׷��� ���� �� �ֵ��� FALSE�� ��ȯ�մϴ�.
	return FALSE;
}

