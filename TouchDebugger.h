// TouchDebugger.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// �� ��ȣ


// CTouchDebuggerApp:
// �� Ŭ������ ������ ���ؼ��� TouchDebugger.cpp�� �����Ͻʽÿ�.
//

class CTouchDebuggerApp : public CWinApp
{
public:
	CTouchDebuggerApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()		
};

extern CTouchDebuggerApp theApp;
