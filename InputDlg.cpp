// InputDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "InputDlg.h"


// CInputDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CInputDlg, CDialog)
CInputDlg::CInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputDlg::IDD, pParent)
{
	m_strInput = "";
	m_strTitle = "";
	m_bPassword = FALSE;
}

CInputDlg::~CInputDlg()
{
}

void CInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INPUT, m_editInput);
}


BEGIN_MESSAGE_MAP(CInputDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CInputDlg 메시지 처리기입니다.

BOOL CInputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect;
	GetClientRect(rect);

	SetWindowPos(&wndTopMost, 0, 0, rect.Width(), rect.Height(), SWP_NOREPOSITION);
	CenterWindow();

	((GetDlgItem(IDC_STATIC_TITLE)))->SetWindowText((LPCTSTR)m_strTitle);	
	
	if(m_bPassword) {
		m_editInput.SetPasswordChar( '*' );
		m_editInput.ModifyStyle(0, ES_PASSWORD);	
		m_editInput.SetWindowText("");
	} else
		m_editInput.SetWindowText((LPCTSTR)m_strInput);
	
	return TRUE;  // return TRUE unless you set the focus to a control	
}

BOOL CInputDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CInputDlg::OnBnClickedOk()
{	
	m_editInput.GetWindowText(m_strInput);
	OnOK();
}

