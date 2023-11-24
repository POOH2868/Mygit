// AfeMultiSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "AfeMultiSettingDlg.h"
#include "afxdialogex.h"


// CAfeMultiSettingDlg dialog

IMPLEMENT_DYNAMIC(CAfeMultiSettingDlg, CDialogEx)

CAfeMultiSettingDlg::CAfeMultiSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAfeMultiSettingDlg::IDD, pParent)
	, m_nNCount_A(0)
	, m_nUCount_A(0)
	, m_nFrequency_A(0)
	, m_nNCount_B(0)
	, m_nUCount_B(0)
	, m_nFrequency_B(0)
{

}

CAfeMultiSettingDlg::~CAfeMultiSettingDlg()
{
}

void CAfeMultiSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATIC_TITLE, m_Title);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);

	DDX_Text(pDX, IDC_EDIT_MULTI_A_AFE_N_COUNT, m_nNCount_A);
	DDX_Text(pDX, IDC_EDIT_MULTI_A_AFE_U_COUNT, m_nUCount_A);
	DDX_Text(pDX, IDC_EDIT_MULTI_A_AFE_FREQUENCY, m_nFrequency_A);

	DDX_Text(pDX, IDC_EDIT_MULTI_B_AFE_N_COUNT, m_nNCount_B);
	DDX_Text(pDX, IDC_EDIT_MULTI_B_AFE_U_COUNT, m_nUCount_B);
	DDX_Text(pDX, IDC_EDIT_MULTI_B_AFE_FREQUENCY, m_nFrequency_B);
}


BEGIN_MESSAGE_MAP(CAfeMultiSettingDlg, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CAfeMultiSettingDlg message handlers

BOOL CAfeMultiSettingDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CAfeMultiSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Title.SetTitle("AFE Setting");
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



void CAfeMultiSettingDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	m_Title.SetBkColor(80, 100, 120, 0, 0, 0);
}
