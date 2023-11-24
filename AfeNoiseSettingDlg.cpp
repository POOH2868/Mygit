// AfeNoiseSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "AfeNoiseSettingDlg.h"
#include "afxdialogex.h"


// CAfeNoiseSettingDlg dialog

IMPLEMENT_DYNAMIC(CAfeNoiseSettingDlg, CDialogEx)

CAfeNoiseSettingDlg::CAfeNoiseSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAfeNoiseSettingDlg::IDD, pParent)
	, m_nNCount(0)
	, m_nUCount(0)
	, m_nFrequency(0)
	, m_nFrequency1(0)
	, m_nFrequency2(0)
	, m_nFrequency3(0)
	, m_nFrequency4(0)
	, bCheckFrequency1(0)
	, bCheckFrequency2(0)
	, bCheckFrequency3(0)
	, bCheckFrequency4(0)
{

}

CAfeNoiseSettingDlg::~CAfeNoiseSettingDlg()
{
}

void CAfeNoiseSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATIC_TITLE, m_Title);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);

	DDX_Text(pDX, IDC_EDIT_AFE_N_COUNT, m_nNCount);
	DDX_Text(pDX, IDC_EDIT_AFE_U_COUNT, m_nUCount);
	DDX_Text(pDX, IDC_EDIT_AFE_FREQUENCY, m_nFrequency);
	
	// NOISE
	DDX_Text(pDX, IDC_EDIT_AFE_FREQUENCY1, m_nFrequency1);
	DDX_Text(pDX, IDC_EDIT_AFE_FREQUENCY2, m_nFrequency2);
	DDX_Text(pDX, IDC_EDIT_AFE_FREQUENCY3, m_nFrequency3);
	DDX_Text(pDX, IDC_EDIT_AFE_FREQUENCY4, m_nFrequency4);

	DDX_Check(pDX, IDC_CHECK_AFE_FREQUENCY1, bCheckFrequency1);
	DDX_Check(pDX, IDC_CHECK_AFE_FREQUENCY2, bCheckFrequency2);
	DDX_Check(pDX, IDC_CHECK_AFE_FREQUENCY3, bCheckFrequency3);
	DDX_Check(pDX, IDC_CHECK_AFE_FREQUENCY4, bCheckFrequency4);
}


BEGIN_MESSAGE_MAP(CAfeNoiseSettingDlg, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CAfeNoiseSettingDlg message handlers

BOOL CAfeNoiseSettingDlg::PreTranslateMessage(MSG* pMsg) 
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

BOOL CAfeNoiseSettingDlg::OnInitDialog()
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

void CAfeNoiseSettingDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	m_Title.SetBkColor(80, 100, 120, 0, 0, 0);
}
