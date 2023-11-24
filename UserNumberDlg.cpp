// UserNumberDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "UserNumberDlg.h"
#include "afxdialogex.h"


// CUserNumberDlg dialog

IMPLEMENT_DYNAMIC(CUserNumberDlg, CDialogEx)

CUserNumberDlg::CUserNumberDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUserNumberDlg::IDD, pParent)
{
	m_strUserNumber = _T("");
}

CUserNumberDlg::~CUserNumberDlg()
{
}

void CUserNumberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TITLE, m_Title);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Text(pDX, IDC_EDIT_START_USER_NUMBER, m_strUserNumber);
}


BEGIN_MESSAGE_MAP(CUserNumberDlg, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CUserNumberDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CUserNumberDlg message handlers

BOOL CUserNumberDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
			return TRUE;

		if (pMsg->wParam == VK_RETURN)
		{
			if (pMsg->hwnd == GetDlgItem(IDC_EDIT_START_USER_NUMBER)->GetSafeHwnd())
			{
				UpdateData(TRUE);

				if (m_strUserNumber.IsEmpty() == TRUE)
					return TRUE;

				BOOL bRet = -1;

				do
				{
					bRet = PostMessage(WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), (LPARAM)this->m_hWnd);
				}while( bRet == 0 );
			}

			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CUserNumberDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Title.SetTitle("USER NUMBER");
	m_Title.SetBkColor(80, 100, 120, 0, 0, 0);

	m_btnOK.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	m_btnOK.SetIcon(IDI_ICON_OK, IDI_ICON_OK_GRAY);
	m_btnOK.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(125, 184, 56));
	m_btnOK.SetBtnCursor(IDC_CURSOR_HAND);

	m_btnCancel.SetShade(CShadeButtonST::SHS_HARDBUMP);
	m_btnCancel.SetIcon(IDI_ICON_CANCEL, IDI_ICON_CANCEL_GRAY);
	m_btnCancel.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(240, 26, 40));
	m_btnCancel.SetBtnCursor(IDC_CURSOR_HAND);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CUserNumberDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	m_Title.SetBkColor(80, 100, 120, 0, 0, 0);
}

void CUserNumberDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	CDialogEx::OnOK();
}

