// GpioButtonDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "GpioButtonDlg.h"
#include "afxdialogex.h"

#include "TestModeView.h"

#define		TEST_BUTTON_SWTICH_TIME		30

// CGpioButtonDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CGpioButtonDlg, CDialogEx)

CGpioButtonDlg::CGpioButtonDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGpioButtonDlg::IDD, pParent)
{
	m_pParent = pParent;

	m_nRemainTime = TEST_BUTTON_SWTICH_TIME;
}

CGpioButtonDlg::~CGpioButtonDlg()
{
	if (mFont.GetSafeHandle())
		mFont.DeleteObject();

	if (mbtnFont.GetSafeHandle())
		mbtnFont.DeleteObject();

	if (mBrush.GetSafeHandle())
		mBrush.DeleteObject();
}

void CGpioButtonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
	for (int i = 0; i < MAX_DEVICE_COUNT; i++)
	{
		DDX_Control(pDX, IDC_STATIC_BUTTON_0 + i, m_ButtonStatus[i]);
	}

	DDX_Control(pDX, IDC_STATIC_REMAIN_TIME, m_StRemainTime);
}


BEGIN_MESSAGE_MAP(CGpioButtonDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_MESSAGE(WM_USER_SHOW_BUTTON_TEST, CGpioButtonDlg::OnShowDialog)
	ON_MESSAGE(WM_USER_UPDATE_BUTTON_STATUS, CGpioButtonDlg::OnUpdateButtonStatus)
END_MESSAGE_MAP()


// CGpioButtonDlg 메시지 처리기입니다.

BOOL CGpioButtonDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CGpioButtonDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

#if defined(D_USE_CHECK_BUTTON_TEST)
	LOGFONT        LogFont;
	LOGFONT        BtnFont;

	//========================================================================
	//POPUP NOTI 글자 설정
	GetDlgItem(IDC_STATIC_NOTI_MSG)->GetFont()->GetLogFont(&LogFont);
	// Font 글자 설정
	LogFont.lfWeight = 1000;      // 굵기설정( MAX : 1000 )
	LogFont.lfHeight = 70;      // Font Size 설정
	// LogFont의정보로설정	
	mFont.CreateFontIndirect(&LogFont);

	GetDlgItem(IDC_STATIC_NOTI_MSG)->SetFont(&mFont);
	mBrush.CreateSolidBrush(LTGRAY_BRUSH);

	//========================================================================
	//Button 글자 설정
	GetDlgItem(IDC_STATIC_REMAIN_TIME)->GetFont()->GetLogFont(&BtnFont);
	// Font 글자 설정
	BtnFont.lfWeight = 300;      // 굵기설정( MAX : 1000 )
	BtnFont.lfHeight = 30;      // Font Size 설정
	// LogFont의정보로설정
	mbtnFont.CreateFontIndirect(&BtnFont);
	GetDlgItem(IDC_STATIC_REMAIN_TIME)->SetFont(&mbtnFont);

	m_StRemainTime.SetTextColor(ZT_COLOR_RED, FALSE);
	m_StRemainTime.SetFont(&mbtnFont, FALSE);
	m_StRemainTime.SetBold(TRUE);

	//========================================================================
	// Gpio Button

	for (int nIndex = 0; nIndex < MAX_DEVICE_COUNT; nIndex++)
	{
		m_ButtonStatus[nIndex].SetFont(&mbtnFont, FALSE);
	}
#endif

	return TRUE;  // return TRUE unless you set the focus to a control	
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CGpioButtonDlg::OnCancel()
{
	ShowDialog(SW_HIDE);
}

LRESULT CGpioButtonDlg::OnShowDialog(WPARAM wParam, LPARAM lParam)
{
	// show or hide
	ShowDialog((int)wParam);

	return 1L;
}

void CGpioButtonDlg::ShowDialog(int nShow)
{
#if defined(D_USE_CHECK_BUTTON_TEST)
	BUTTON_STATUS * pStatus = ((CTestModeView*)m_pParent)->GetGpioButtonStatus();

	if (nShow)
	{
		// Gpio Button
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

		m_nRemainTime = TEST_BUTTON_SWTICH_TIME;

		CString strTemp;
		strTemp.Format("%d", m_nRemainTime);
		m_StRemainTime.SetText(strTemp);

		SetTimer(TIMER_GPIO_BUTTON_TEST_REMAIN_TIME, 1000, NULL);
	}
	else
	{
		KillTimer(TIMER_GPIO_BUTTON_TEST_REMAIN_TIME);
	}

	pStatus->bShowDialog = nShow ? TRUE : FALSE;

	ShowWindow(nShow);
#endif
}

LRESULT CGpioButtonDlg::OnUpdateButtonStatus(WPARAM wParam, LPARAM lParam)
{
	BOOL bEnable = (BOOL)wParam;

	SetButtonStatus((int)wParam, (int)lParam, FALSE);

	return 1L;
}

void CGpioButtonDlg::SetButtonStatus(int nIndex, int nStatus, BOOL bInit)
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

		BUTTON_STATUS * pStatus = ((CTestModeView*)m_pParent)->GetGpioButtonStatus();

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

HBRUSH CGpioButtonDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CGpioButtonDlg::OnTimer(UINT_PTR nIDEvent)
{
#if defined(D_USE_CHECK_BUTTON_TEST)
	m_nRemainTime--;

	if (m_nRemainTime >= 0)
	{
		CString strTemp;
		strTemp.Format("%d", m_nRemainTime);
		m_StRemainTime.SetText(strTemp);
	}
	else
	{
		OnCancel();
	}
#endif

	CDialogEx::OnTimer(nIDEvent);
}

