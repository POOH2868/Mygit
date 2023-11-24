// TestResultDescDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "TestResultDescDlg.h"
#include "afxdialogex.h"
#include ".\CustomUI\RichEditEx.h"


// CTestResultDescDlg dialog

IMPLEMENT_DYNAMIC(CTestResultDescDlg, CDialog)

CTestResultDescDlg::CTestResultDescDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestResultDescDlg::IDD, pParent)
{

}

CTestResultDescDlg::~CTestResultDescDlg()
{
}

void CTestResultDescDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT_RESULT_DESCRIPTION, m_RichEditResultDescription);
}


BEGIN_MESSAGE_MAP(CTestResultDescDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CTestResultDescDlg message handlers

BOOL CTestResultDescDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_RichEditResultDescription.SetBackgroundColor(FALSE, RGB(240, 240, 240));
	m_RichEditResultDescription.ShowWindow(SW_SHOW);

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CTestResultDescDlg::ShowDialog(int nCmdShow)
{
	m_RichEditResultDescription.SetVisible((nCmdShow == SW_SHOW) ? TRUE : FALSE);
	m_RichEditResultDescription.ShowWindow(nCmdShow);

	ShowWindow(nCmdShow);
}

void CTestResultDescDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRichEditEx * pRichEdit = (CRichEditEx*)GetDlgItem(IDC_RICHEDIT_RESULT_DESCRIPTION);
	if(pRichEdit)
	{
		CRect rect;
		GetWindowRect(&rect);

		CRect rectDescription;
		rectDescription.left = 10;
		rectDescription.right = rect.Width() - 10;
		rectDescription.top = 10;
		rectDescription.bottom = rect.Height() - 10;

		pRichEdit->MoveWindow(rectDescription);
	}
}

void CTestResultDescDlg::AddMessage(COLORREF color, LPCTSTR pstrFormat)
{
	m_RichEditResultDescription.SetColor(color);
	m_RichEditResultDescription.WriteStr(pstrFormat);

	m_RichEditResultDescription.SendMessage(WM_VSCROLL, SB_BOTTOM, NULL);
}

void CTestResultDescDlg::ClearMessage()
{
	m_RichEditResultDescription.SetWindowTextA(_T(""));
}

