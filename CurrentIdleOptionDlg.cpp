// CurrentIdleOptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "CurrentIdleOptionDlg.h"
#include "afxdialogex.h"
#include "util.h"

enum
{
	CTRL_UPDATE_IDLE				= (1 << 0),
	CTRL_UPDATE_SLEEP 				= (1 << 1),
	CTRL_UPDATE_DEEP_SLEEP			= (1 << 2)
};

// CCurrentIdleOptionDlg dialog

IMPLEMENT_DYNAMIC(CCurrentIdleOptionDlg, CDialogEx)

CCurrentIdleOptionDlg::CCurrentIdleOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCurrentIdleOptionDlg::IDD, pParent)
{

}

CCurrentIdleOptionDlg::~CCurrentIdleOptionDlg()
{
}

void CCurrentIdleOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATIC_TITLE, m_Title);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);

	// Idle
	DDX_Check(pDX, IDC_CHECK_CURRENT_CONSUMPTION_IDLE, m_Idle.bCheckData);
	DDX_Text(pDX, IDC_EDIT_CURRENT_CONSUMPTION_IDLE, m_Idle.nThreshould_Max);
	DDX_Text(pDX, IDC_EDIT_CURRENT_CONSUMPTION_AVG_MS_IDLE, m_Idle.nAvgMs);

	// Sleep
	DDX_Check(pDX, IDC_CHECK_CURRENT_CONSUMPTION_SLEEP, m_Sleep.bCheckData);
	DDX_Text(pDX, IDC_EDIT_CURRENT_CONSUMPTION_SLEEP, m_Sleep.nThreshould_Max);
	DDX_Text(pDX, IDC_EDIT_CURRENT_CONSUMPTION_AVG_MS_SLEEP, m_Sleep.nAvgMs);

	// DeepSleep
	DDX_Check(pDX, IDC_CHECK_CURRENT_CONSUMPTION_DEEP_SLEEP, m_DeepSleep.bCheckData);
	DDX_Text(pDX, IDC_EDIT_CURRENT_CONSUMPTION_DEEP_SLEEP, m_DeepSleep.nThreshould_Max);
	DDX_Text(pDX, IDC_EDIT_CURRENT_CONSUMPTION_AVG_MS_DEEP_SLEEP, m_DeepSleep.nAvgMs);
}


BEGIN_MESSAGE_MAP(CCurrentIdleOptionDlg, CDialogEx)
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDOK, &CCurrentIdleOptionDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_CURRENT_CONSUMPTION_IDLE, &CCurrentIdleOptionDlg::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_CURRENT_CONSUMPTION_SLEEP, &CCurrentIdleOptionDlg::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_CURRENT_CONSUMPTION_DEEP_SLEEP, &CCurrentIdleOptionDlg::OnCheckBnClicked)
END_MESSAGE_MAP()


// CCurrentIdleOptionDlg message handlers

BOOL CCurrentIdleOptionDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CCurrentIdleOptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitUiContorl();

	UpdateData(FALSE);

	EnableDialogControls(0xFFFFFFFF);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CCurrentIdleOptionDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if( m_Idle.nAvgMs > 1000 ||
		m_Sleep.nAvgMs > 1000 ||
		m_DeepSleep.nAvgMs > 1000 )
	{
		AfxMessageBox("Please. Enter an current consumption delay time\nbetween 1 and 1000");
		return;
	}

	CDialogEx::OnOK();
}

void CCurrentIdleOptionDlg::InitUiContorl()
{
	m_Title.SetTitle("CURRENT CONSUMPTION");
	m_Title.SetBkColor(80, 100, 120, 0, 0, 0);

	m_btnOK.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	m_btnOK.SetIcon(IDI_ICON_OK, IDI_ICON_OK_GRAY);
	m_btnOK.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(125, 184, 56));
	m_btnOK.SetBtnCursor(IDC_CURSOR_HAND);

	m_btnCancel.SetShade(CShadeButtonST::SHS_HARDBUMP);
	m_btnCancel.SetIcon(IDI_ICON_CANCEL, IDI_ICON_CANCEL_GRAY);
	m_btnCancel.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(240, 26, 40));
	m_btnCancel.SetBtnCursor(IDC_CURSOR_HAND);

	SetWindowTheme(GetDlgItem(IDC_CHECK_CURRENT_CONSUMPTION_IDLE)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_CURRENT_CONSUMPTION_SLEEP)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_CURRENT_CONSUMPTION_DEEP_SLEEP)->m_hWnd, L"", L"");
}

void CCurrentIdleOptionDlg::OnCheckBnClicked()
{
	UpdateData(TRUE);

	int nID = GetFocus()->GetDlgCtrlID();
	int nUpdateID = 0xFFFF;

	if( nID == IDC_CHECK_CURRENT_CONSUMPTION_IDLE )
	{
		nUpdateID = CTRL_UPDATE_IDLE;
	}
	else if( nID == IDC_CHECK_CURRENT_CONSUMPTION_SLEEP )
	{
		nUpdateID = CTRL_UPDATE_SLEEP;
	}
	else if( nID == IDC_CHECK_CURRENT_CONSUMPTION_DEEP_SLEEP )
	{
		nUpdateID = CTRL_UPDATE_DEEP_SLEEP;
	}

	EnableDialogControls(nUpdateID);

	GetDlgItem(nID)->Invalidate();
}

void CCurrentIdleOptionDlg::EnableDialogControls(int nUpdateID)
{
	UpdateData(TRUE);

	if( nUpdateID & CTRL_UPDATE_IDLE )
	{
		BOOL bEnableCurrent = m_Idle.bCheckData ? TRUE : FALSE;

		GetDlgItem(IDC_EDIT_CURRENT_CONSUMPTION_IDLE)->EnableWindow(bEnableCurrent);
		GetDlgItem(IDC_EDIT_CURRENT_CONSUMPTION_AVG_MS_IDLE)->EnableWindow(bEnableCurrent);
		GetDlgItem(IDC_STATIC_CURRENT_CONSUMPTION_IDLE)->EnableWindow(bEnableCurrent);
		GetDlgItem(IDC_STATIC_CURRENT_CONSUMPTION_AVG_MS_IDLE)->EnableWindow(bEnableCurrent);
	}

	if( nUpdateID & CTRL_UPDATE_SLEEP )
	{
		BOOL bEnableCurrent = m_Sleep.bCheckData ? TRUE : FALSE;

		GetDlgItem(IDC_EDIT_CURRENT_CONSUMPTION_SLEEP)->EnableWindow(bEnableCurrent);
		GetDlgItem(IDC_EDIT_CURRENT_CONSUMPTION_AVG_MS_SLEEP)->EnableWindow(bEnableCurrent);
		GetDlgItem(IDC_STATIC_CURRENT_CONSUMPTION_SLEEP)->EnableWindow(bEnableCurrent);
		GetDlgItem(IDC_STATIC_CURRENT_CONSUMPTION_AVG_MS_SLEEP)->EnableWindow(bEnableCurrent);
	}

	if( nUpdateID & CTRL_UPDATE_DEEP_SLEEP )
	{
		BOOL bEnableCurrent = m_DeepSleep.bCheckData ? TRUE : FALSE;

		GetDlgItem(IDC_EDIT_CURRENT_CONSUMPTION_DEEP_SLEEP)->EnableWindow(bEnableCurrent);
		GetDlgItem(IDC_EDIT_CURRENT_CONSUMPTION_AVG_MS_DEEP_SLEEP)->EnableWindow(bEnableCurrent);
		GetDlgItem(IDC_STATIC_CURRENT_CONSUMPTION_DEEP_SLEEP)->EnableWindow(bEnableCurrent);
		GetDlgItem(IDC_STATIC_CURRENT_CONSUMPTION_AVG_MS_DEEP_SLEEP)->EnableWindow(bEnableCurrent);
	}
}

HBRUSH CCurrentIdleOptionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor != CTLCOLOR_STATIC)
		return hbr;

	int nState = -1;

	switch (pWnd->GetDlgCtrlID())
	{
	case IDC_CHECK_CURRENT_CONSUMPTION_IDLE:			nState = m_Idle.bCheckData;			break;
	case IDC_CHECK_CURRENT_CONSUMPTION_SLEEP:			nState = m_Sleep.bCheckData;		break;
	case IDC_CHECK_CURRENT_CONSUMPTION_DEEP_SLEEP:		nState = m_DeepSleep.bCheckData;	break;
	}

	if (nState != -1)
	{
		ChangeCheckboxBgColor(pDC, nState);
	}

	return hbr;
}
