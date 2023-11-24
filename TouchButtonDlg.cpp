// TouchButtonDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "TouchButtonDlg.h"
#include "afxdialogex.h"

#include "TestModeView.h"

// CTouchButtonDlg dialog

IMPLEMENT_DYNAMIC(CTouchButtonDlg, CDialogEx)

CTouchButtonDlg::CTouchButtonDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTouchButtonDlg::IDD, pParent)
{
	m_pParent = pParent;
}

CTouchButtonDlg::~CTouchButtonDlg()
{
	if (mFont.GetSafeHandle())
		mFont.DeleteObject();

	if (mbtnFont.GetSafeHandle())
		mbtnFont.DeleteObject();

	if (mBrush.GetSafeHandle())
		mBrush.DeleteObject();
}

void CTouchButtonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	for (int i = 0; i < MAX_DEVICE_COUNT; i++)
	{
		DDX_Control(pDX, IDC_STATIC_BUTTON_0 + i, m_ButtonStatus[i]);
	}
}


BEGIN_MESSAGE_MAP(CTouchButtonDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_USER_SHOW_BUTTON_TEST, CTouchButtonDlg::OnShowDialog)
	ON_MESSAGE(WM_USER_UPDATE_BUTTON_STATUS, CTouchButtonDlg::OnUpdateButtonStatus)
#ifdef _DEBUG
	ON_MESSAGE(WM_USER_UPDATE_BUTTON_VALUE, CTouchButtonDlg::OnUpdateButtonValue)
#endif
END_MESSAGE_MAP()


// CTouchButtonDlg message handlers

BOOL CTouchButtonDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CTouchButtonDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

#ifdef _DEBUG
	GetDlgItem(IDC_STATIC_REMAIN_TIME)->ShowWindow(TRUE);
#else
	GetDlgItem(IDC_STATIC_REMAIN_TIME)->ShowWindow(FALSE);
#endif

#if defined(D_USE_CHECK_BUTTON_TEST)
	LOGFONT 	   LogFont;
	LOGFONT 	   BtnFont;

	//========================================================================
	//POPUP NOTI 글자 설정
	GetDlgItem(IDC_STATIC_NOTI_MSG)->GetFont()->GetLogFont(&LogFont);
	// Font 글자 설정
	LogFont.lfWeight = 1000;	  // 굵기설정( MAX : 1000 )
	LogFont.lfHeight = 70;		// Font Size 설정
	// LogFont의정보로설정	
	mFont.CreateFontIndirect(&LogFont);

	GetDlgItem(IDC_STATIC_NOTI_MSG)->SetFont(&mFont);
	mBrush.CreateSolidBrush(LTGRAY_BRUSH);

	//========================================================================
	//Button 글자 설정
	GetDlgItem(IDC_STATIC_REMAIN_TIME)->GetFont()->GetLogFont(&BtnFont);
	// Font 글자 설정
	BtnFont.lfWeight = 300; 	 // 굵기설정( MAX : 1000 )
	BtnFont.lfHeight = 30;		// Font Size 설정
	// LogFont의정보로설정
	mbtnFont.CreateFontIndirect(&BtnFont);
	GetDlgItem(IDC_STATIC_REMAIN_TIME)->SetFont(&mbtnFont);

	//========================================================================
	// Touch Button

	for (int nIndex = 0; nIndex < MAX_DEVICE_COUNT; nIndex++)
	{
		m_ButtonStatus[nIndex].SetFont(&mbtnFont, FALSE);
	}
#endif

	return TRUE;  // return TRUE unless you set the focus to a control	
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTouchButtonDlg::OnCancel()
{
	ShowDialog(SW_HIDE);
}

LRESULT CTouchButtonDlg::OnShowDialog(WPARAM wParam, LPARAM lParam)
{
	// show or hide
	ShowDialog((int)wParam);

	return 1L;
}

void CTouchButtonDlg::ShowDialog(int nShow)
{
#if defined(D_USE_CHECK_BUTTON_TEST)
	BUTTON_STATUS * pStatus = ((CTestModeView*)m_pParent)->GetTouchButtonStatus();

	if (nShow)
	{
		// Touch Button
		for (int nIndex = 0; nIndex < MAX_DEVICE_COUNT; nIndex++)
		{
			if (pStatus->nStatus[nIndex] == E_BUTTON_TEST_REQUEST)
			{
				((CStatic*)GetDlgItem(IDC_STATIC_BUTTON_0 + nIndex))->EnableWindow(TRUE);
				((CStatic*)GetDlgItem(IDC_STATIC_BUTTON_DEVICE_0 + nIndex))->EnableWindow(TRUE);
			}
			else
			{
				((CStatic*)GetDlgItem(IDC_STATIC_BUTTON_0 + nIndex))->EnableWindow(FALSE);
				((CStatic*)GetDlgItem(IDC_STATIC_BUTTON_DEVICE_0 + nIndex))->EnableWindow(FALSE);
			}

			SetButtonStatus(nIndex, pStatus->nStatus[nIndex], TRUE);
		}

		CenterWindow();
	}

	pStatus->bShowDialog = nShow ? TRUE : FALSE;

	ShowWindow(nShow);
#endif
}

LRESULT CTouchButtonDlg::OnUpdateButtonStatus(WPARAM wParam, LPARAM lParam)
{
	BOOL bEnable = (BOOL)wParam;

	SetButtonStatus((int)wParam, (int)lParam, FALSE);

	return 1L;
}

void CTouchButtonDlg::SetButtonStatus(int nIndex, int nStatus, BOOL bInit)
{
#if defined(D_USE_CHECK_BUTTON_TEST)
	switch(nStatus)
	{
	case E_BUTTON_TEST_OFF:
		m_ButtonStatus[nIndex].SetBackgroundColor(RGB(225, 225, 225), FALSE);
		m_ButtonStatus[nIndex].SetText("-");
		break;

	case E_BUTTON_TEST_ON:
	case E_BUTTON_TEST_REQUEST:
		m_ButtonStatus[nIndex].SetBackgroundColor(RGB(0, 128, 0), FALSE);
		m_ButtonStatus[nIndex].SetText("Ready");
		break;

	case E_BUTTON_TEST_OK:
		m_ButtonStatus[nIndex].SetBackgroundColor(RGB(0, 225, 0), FALSE);
		m_ButtonStatus[nIndex].SetText("OK");
		break;
	}

	if ((bInit == FALSE) && (nStatus == E_BUTTON_TEST_OK))
	{
		BOOL bShowDialog = FALSE;

		BUTTON_STATUS * pStatus = ((CTestModeView*)m_pParent)->GetTouchButtonStatus();

		// veriry button test is completed...
		for (int nIndex = 0; nIndex < MAX_DEVICE_COUNT; nIndex++)
		{
			if (pStatus->nStatus[nIndex] == E_BUTTON_TEST_OFF)
				continue;
				
			if (pStatus->nStatus[nIndex] != E_BUTTON_TEST_OK)
			{
				bShowDialog = TRUE;
				break;
			}
		}

		// if all device is completed, hide dialog
		if (bShowDialog == FALSE)
		{
			ShowDialog(SW_HIDE);
		}
	}
#endif
}

HBRUSH CTouchButtonDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_NOTI_MSG)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(RGB(255, 255, 255));
		pDC->SetTextColor(ZT_COLOR_RED);
		return mBrush;
	}

	return hbr;
}

#ifdef _DEBUG
LRESULT CTouchButtonDlg::OnUpdateButtonValue(WPARAM wParam, LPARAM lParam)
{
	int nValue = (BOOL)wParam;
	CString strTemp;

	strTemp.Format("%d", nValue);

	GetDlgItem(IDC_STATIC_REMAIN_TIME)->SetWindowText(strTemp);

	return 1L;
}
#endif

