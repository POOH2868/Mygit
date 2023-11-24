// TouchDebuggerDlg.h : ��� ����
//

#pragma once

#include "stdafx.h"
#include "afxwin.h"
#include "TestModeView.h"
#include "CustomUI\StatusBarView.h"

// CTouchDebuggerDlg ��ȭ ����
class CTouchDebuggerDlg : public CDialog
{
// ����
public:
	CTouchDebuggerDlg(CWnd* pParent = NULL);	// ǥ�� ������
	virtual ~CTouchDebuggerDlg();
	
// ��ȭ ���� ������
	enum { IDD = IDD_TOUCHDEBUGGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����

protected:

	//window position
	RECT	m_MainRect;
	RECT	m_PaneRect;

	CView *	m_testModeView;
	CView * m_statusBarView;

	//Spite Window		
	long m_lSpliteTopOffset;
	long m_lSpliteBottomOffset;
	long m_lSpliteSideOffset;

	CProgressCtrl m_progressBar;

	HDEVNOTIFY m_DeviceNotificationHandle;

// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwEventData);
	
	afx_msg void OnBnClickedButtonTestMode();
	afx_msg void OnBnClickedButtonExit();

	afx_msg LRESULT OnUMProgressbar(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	void RegisterForDeviceNotifications();
};
