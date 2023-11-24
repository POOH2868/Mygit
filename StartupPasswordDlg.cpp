// StartupPasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "StartupPasswordDlg.h"
#include "afxdialogex.h"


// CStartupPasswordDlg dialog

IMPLEMENT_DYNAMIC(CStartupPasswordDlg, CDialogEx)

CStartupPasswordDlg::CStartupPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStartupPasswordDlg::IDD, pParent)
{
	m_strPassword = _T("");
}

CStartupPasswordDlg::~CStartupPasswordDlg()
{
}

void CStartupPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_START_USER_NUMBER, m_strPassword);
}


BEGIN_MESSAGE_MAP(CStartupPasswordDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CStartupPasswordDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CStartupPasswordDlg message handlers

BOOL CStartupPasswordDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			//return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CStartupPasswordDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	CDialogEx::OnOK();
}

