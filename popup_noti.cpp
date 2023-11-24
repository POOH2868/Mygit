// popup_noti.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "popup_noti.h"
#include "afxdialogex.h"


// popup_noti ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(popup_noti, CDialog)

popup_noti::popup_noti(BOOL bModelessDlg, CWnd* pParent /*=NULL*/)
	: CDialog(popup_noti::IDD, pParent)
{
	m_bModelessDlg = bModelessDlg;

	m_bDialogExit = FALSE;
}

popup_noti::~popup_noti()
{
	if (mFont.GetSafeHandle())
		mFont.DeleteObject();

	if (mbtnFont.GetSafeHandle())
		mbtnFont.DeleteObject();

	if (mBrush.GetSafeHandle())
		mBrush.DeleteObject();
}

void popup_noti::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(popup_noti, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &popup_noti::OnBnClickedButtonExit)
END_MESSAGE_MAP()


// popup_noti �޽��� ó�����Դϴ�.

BOOL popup_noti::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE /*|| pMsg->wParam == VK_RETURN*/)
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL popup_noti::OnInitDialog()
{
	CDialog::OnInitDialog();

	LOGFONT        LogFont;
	LOGFONT        BtnFont;

	//========================================================================
	//POPUP NOTI ���� ����
	//=====================
	GetDlgItem(IDC_STATIC_POPUP_NOTI)->GetFont()->GetLogFont(&LogFont);
	// Font ���� ����
	LogFont.lfWeight = 1000;      // ���⼳��( MAX : 1000 )
	LogFont.lfHeight = 70;      // Font Size ����
	// LogFont�������μ���
	mFont.CreateFontIndirect(&LogFont);

	GetDlgItem(IDC_STATIC_POPUP_NOTI)->SetFont(&mFont);
	mBrush.CreateSolidBrush(LTGRAY_BRUSH);

	//========================================================================
	//Button ���� ����
	//=================
	GetDlgItem(IDC_BUTTON_EXIT)->GetFont()->GetLogFont(&BtnFont);
	// Font ���� ����
	BtnFont.lfWeight = 300;      // ���⼳��( MAX : 1000 )
	BtnFont.lfHeight = 30;      // Font Size ����
	// LogFont�������μ���
	mbtnFont.CreateFontIndirect(&BtnFont);
	GetDlgItem(IDC_BUTTON_EXIT)->SetFont(&mbtnFont);
	//========================================================================

	CenterWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control	
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


HBRUSH popup_noti::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_POPUP_NOTI)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(RGB(255, 255, 255));
		pDC->SetTextColor(RGB(255, 0, 0));
		return mBrush;
	}
	return hbr;
}


void popup_noti::OnBnClickedButtonExit()
{
	if(m_bModelessDlg)
		DestroyWindow();
	else
		OnOK();

	m_bDialogExit = TRUE;
}

