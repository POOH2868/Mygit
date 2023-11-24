// BarcodeSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "BarcodeSettingDlg.h"
#include "afxdialogex.h"

#include "TestModeView.h"
#include "hid\UsbHidList.h"

#include "util.h"

enum BARCODE_LIST_ITEM_INDEX
{
	BARCODE_LIST_ITEM_NONE = 0,
	BARCODE_LIST_ITEM_DEVICE,
	BARCODE_LIST_ITEM_BARCODE,

	BARCODE_LIST_ITEM_MAX
};

// CBarcodeSettingDlg dialog

IMPLEMENT_DYNAMIC(CBarcodeSettingDlg, CDialogEx)

CBarcodeSettingDlg::CBarcodeSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBarcodeSettingDlg::IDD, pParent)
{
	m_pParent = pParent;

	m_bShowDialog = FALSE;

	m_nBarcode_Length = 0;

	m_nBarcodeIdx = 0;
	m_nBarcodeChIdx = 0;

	m_bPartialTestOn = FALSE;

	memset(m_cBarcode, 0x00, sizeof(m_cBarcode));

	for(int n=0; n<MAX_DEVICE_COUNT; n++)
		m_bBarcodeAutoClear[n] = FALSE;

	//m_BarcodeInputTime = GetTimeInMillis();
}

CBarcodeSettingDlg::~CBarcodeSettingDlg()
{
	if (m_ListFont.GetSafeHandle())
		m_ListFont.DeleteObject();
}

void CBarcodeSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TITLE_BARCODE, m_Title);
	DDX_Control(pDX, IDC_LIST_BARCODE, m_ListCtrl);
	DDX_Check(pDX, IDC_CHECK_BARCODE_PARTIAL_TEST, m_bPartialTestOn);

	DDX_Control(pDX, IDC_BUTTON_BARCODE_START_TEST_ALL, m_btnStartTestAll);

	for (int i = 0; i < MAX_DEVICE_COUNT; i++)
	{
		DDX_Control(pDX, IDC_BUTTON_PARTIAL_TEST_01 + i, m_btnPartialStart[i]);
	}
}


BEGIN_MESSAGE_MAP(CBarcodeSettingDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_BARCODE_START_TEST_ALL, &CBarcodeSettingDlg::OnBnClicked_StartTestAll)
	ON_BN_CLICKED(IDC_CHECK_BARCODE_PARTIAL_TEST, &CBarcodeSettingDlg::OnCheckBnClicked_PartialTest)
	ON_COMMAND_RANGE(IDC_BUTTON_PARTIAL_TEST_01, IDC_BUTTON_PARTIAL_TEST_08, &CBarcodeSettingDlg::OnBnClicked_StartPartialTest)
	ON_MESSAGE(WM_USER_LIST_CTRL_LOG_CLICK, &CBarcodeSettingDlg::OnClickListBarcode)
	ON_MESSAGE(WM_USER_BARCODE_SHOW_DIALOG, &CBarcodeSettingDlg::OnShowDialog)
	ON_MESSAGE(WM_USER_BARCODE_DEVICE_CHANGED, &CBarcodeSettingDlg::OnDeviceChanged)
	ON_MESSAGE(WM_USER_BARCODE_TEST_COMPLETED, &CBarcodeSettingDlg::OnTestCompleted)
END_MESSAGE_MAP()


// CBarcodeSettingDlg message handlers

BOOL CBarcodeSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Title.SetTitle("BARCODE");
	m_Title.SetBkColor(80, 100, 120, 0, 0, 0);

	SetButtonConfig(&m_btnStartTestAll);

	for (int i = 0; i < MAX_DEVICE_COUNT; i++)
	{
		SetButtonConfig(&m_btnPartialStart[i]);
	}	

	// Initialize ListCtrl
	m_ListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS); // 원하는 Row을 선택하기 위해서 중요

	CRect rect;
	m_ListCtrl.GetClientRect( rect );
	m_ListCtrl.InsertColumn ( BARCODE_LIST_ITEM_NONE, _T(""), LVCFMT_CENTER, 0, 0 );
	m_ListCtrl.InsertColumn ( BARCODE_LIST_ITEM_DEVICE, _T("DEVICE"), LVCFMT_CENTER, 80, 0 );
	m_ListCtrl.InsertColumn ( BARCODE_LIST_ITEM_BARCODE, _T("BARCODE"), LVCFMT_CENTER, rect.right - rect.left - 80, 0 );

	// Font 글자 설정
	LOGFONT LogFont;
	GetDlgItem(IDC_LIST_BARCODE)->GetFont()->GetLogFont(&LogFont);
	LogFont.lfWeight = 1000;	// 굵기설정( MAX : 1000 )
	LogFont.lfHeight = 15;		// Font Size 설정
	m_ListFont.CreateFontIndirect(&LogFont);
	GetDlgItem(IDC_LIST_BARCODE)->SetFont(&m_ListFont);

	// set ListCtrl height
	HIMAGELIST hList = ImageList_Create(24, 24, ILC_COLOR8 | ILC_MASK, 8, 1);
	CImageList	ImageList;
	ImageList.Attach(hList);
	m_ListCtrl.SetImageList(&ImageList, LVSIL_SMALL);
	ImageList_Destroy(hList);

	m_ListCtrl.SetParentHwnd(this->m_hWnd);

	// Get position of Partial button
	((CButton*)GetDlgItem(IDC_CHECK_BARCODE_PARTIAL_TEST))->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_nPartialTestButtonPosX = rect.left;
	m_nPartialTestButtonWidth = rect.Width();

	SetWindowTheme(GetDlgItem(IDC_CHECK_BARCODE_PARTIAL_TEST)->m_hWnd, L"", L"");

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CBarcodeSettingDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	m_Title.SetBkColor(80, 100, 120, 0, 0, 0);
}

void CBarcodeSettingDlg::SetBarcodeInfo(BOOL bAutoStart, BOOL bFixedLength, int nLength)
{
	m_bAutoStartTest = bAutoStart;
	m_bFixedBarcodeLength = bFixedLength;
	m_nBarcode_Length = nLength;	
}

void CBarcodeSettingDlg::UiCtrl_Initialize(BOOL bChangeFocus)
{
	CTestModeView * pTestModeView = (CTestModeView*)m_pParent;
	CUsbHidList	* pUsbHidList = (CUsbHidList*)pTestModeView->GetUsdHidList();
	CString strTemp;
	
	CStringArray strPrevBarcode;

	int nFirstItemIndex = -1;
	int nPrevSelectIndex = UiCtrl_GetListFocus();

	if(bChangeFocus == FALSE)
	{
		if(nPrevSelectIndex != -1 && m_ListCtrl.GetItemDisable(nPrevSelectIndex, 0) == TRUE)
			nPrevSelectIndex = -1;

		strPrevBarcode.RemoveAll();

		// get previous barcode
		for (int n = 0; n < MAX_DEVICE_COUNT; n++)
		{		
			if(pUsbHidList->GetUsbHidPtrByDevIndex(n) != NULL)
				strPrevBarcode.Add(m_ListCtrl.GetItemText(n, BARCODE_LIST_ITEM_BARCODE));
			else
				strPrevBarcode.Add("-");
		}
	}
		
	m_ListCtrl.DeleteAllItems();

	for (int nIndex = 0; nIndex < MAX_DEVICE_COUNT; nIndex++)
	{		
		CUsbHid * pUsbHid = pUsbHidList->GetUsbHidPtrByDevIndex(nIndex);

		m_ListCtrl.InsertItem( nIndex, _T("") );

		strTemp.Format(_T("DEVICE %d"), nIndex);
		m_ListCtrl.SetItemText( nIndex, BARCODE_LIST_ITEM_DEVICE, strTemp );

		if (pUsbHid)
		{
			m_ListCtrl.SetItemTextColor(nIndex, ZT_COLOR_BLACK);

			if (nFirstItemIndex == -1)
			{
				nFirstItemIndex = nIndex;

				UiCtrl_SetListFocus(nIndex);
			}

			// set previous barcode
			if (bChangeFocus == FALSE)
			{
				strTemp = strPrevBarcode.GetAt(nIndex);
				if(strTemp.Compare("-") != 0)
					m_ListCtrl.SetItemText(nIndex, BARCODE_LIST_ITEM_BARCODE, strTemp);
			}
		}
		else
		{
			m_ListCtrl.SetItemTextColor(nIndex, ZT_COLOR_GRAY);
			m_ListCtrl.SetItemText(nIndex, BARCODE_LIST_ITEM_BARCODE, "-");

			m_ListCtrl.SetItemDisable(nIndex, TRUE);

			// clear barcode input
			if( nPrevSelectIndex != -1 && nPrevSelectIndex == nIndex )
				ClearBarcodeInput();
		}
	}

	// check previous item disable
	if (bChangeFocus == FALSE 
	&& nPrevSelectIndex != -1 
	&& m_ListCtrl.GetItemDisable(nPrevSelectIndex, 0) == TRUE)
	{
		nPrevSelectIndex = nFirstItemIndex;
	}

	UiCtrl_SetListFocus(bChangeFocus ? nFirstItemIndex : nPrevSelectIndex);

	// move position of partial test buttons
	for (int n = 0; n < MAX_DEVICE_COUNT; n++)
	{		
		CRect rect;
		m_ListCtrl.GetSubItemRect(n, 0, LVIR_BOUNDS, rect);
		m_ListCtrl.ClientToScreen(rect);
		this->ScreenToClient(rect);

		rect.left = m_nPartialTestButtonPosX;
		rect.right = m_nPartialTestButtonPosX + m_nPartialTestButtonWidth;

		GetDlgItem(IDC_BUTTON_PARTIAL_TEST_01 + n)->MoveWindow(rect);			
	}

	UiCtrl_EnableButton();
}

void CBarcodeSettingDlg::UiCtrl_SetListFocus(int nIndex)
{
	m_nListSelectionItem = nIndex;

	m_ListCtrl.SetItemState(nIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	m_ListCtrl.SetSelectionMark(nIndex);
	m_ListCtrl.SetFocus();
}

int CBarcodeSettingDlg::UiCtrl_GetListFocus()
{
	m_nListSelectionItem = m_ListCtrl.GetSelectionMark();

	return m_nListSelectionItem;
}

void CBarcodeSettingDlg::UiCtrl_EnableButton()
{
	CTestModeView * pTestModeView = (CTestModeView*)m_pParent;
	CUsbHidList	* pUsbHidList = (CUsbHidList*)pTestModeView->GetUsdHidList();

	// disable button
	for (int n = 0; n < MAX_DEVICE_COUNT; n++)
		GetDlgItem(IDC_BUTTON_PARTIAL_TEST_01 + n)->EnableWindow(FALSE);

	// check partial test is ON
	if(m_bPartialTestOn == FALSE)
		return;

	// check test in progress
	BOOL bTesting = FALSE;
	for (int nIndex = 0; nIndex < MAX_DEVICE_COUNT; nIndex++)
	{
		CUsbHid * pUsbHid = pUsbHidList->GetUsbHidPtrByDevIndex(nIndex);

		if (pUsbHid && pUsbHid->IsTestAvailable() && pUsbHid->m_bTesting == TRUE)
		{
			bTesting = TRUE;
			break;
		}
	}

	GetDlgItem(IDC_CHECK_BARCODE_PARTIAL_TEST)->EnableWindow(bTesting ? FALSE : TRUE);
	GetDlgItem(IDC_BUTTON_BARCODE_START_TEST_ALL)->EnableWindow(bTesting ? FALSE : TRUE);

	if(pTestModeView->IsUseStartIndividualTest() == FALSE && bTesting)
		return;	

	// enable button
	for (int nIndex = 0; nIndex < MAX_DEVICE_COUNT; nIndex++)
	{
		CUsbHid * pUsbHid = pUsbHidList->GetUsbHidPtrByDevIndex(nIndex);

		if (pUsbHid && pUsbHid->IsTestAvailable() && pUsbHid->m_bTesting == FALSE)
		{
			GetDlgItem(IDC_BUTTON_PARTIAL_TEST_01 + nIndex)->EnableWindow(TRUE);
		}
	}
}

BOOL CBarcodeSettingDlg::PreTranslateMessage(MSG* pMsg)
{
	static BOOL bAlreadyEnter = FALSE;

	if (bAlreadyEnter == TRUE)
	{				
		return TRUE;
	}

	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}

		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_SPACE)
		{
			if (pMsg->hwnd != GetDlgItem(IDC_BUTTON_BARCODE_START_TEST_ALL)->GetSafeHwnd())
			{
				return TRUE;
			}
		}
		else if (pMsg->wParam == VK_DELETE || pMsg->wParam == VK_BACK)
		{
			EraseBarcodeInfo(UiCtrl_GetListFocus(), TRUE);
			return TRUE;
		}
	}

	if( pMsg->message == WM_CHAR || pMsg->message == WM_IME_CHAR )
	{
		if (pMsg->wParam == VK_RETURN)
		{
			CompletedBarcodeInput();
			return TRUE;
		}
	}
	else if( pMsg->message == WM_KEYUP && pMsg->wParam == VK_RETURN )
	{
		CompletedBarcodeInput();
		return TRUE;
	}
	else
	{
		return CDialogEx::PreTranslateMessage(pMsg);
	}

	int nIndex = UiCtrl_GetListFocus();
	if( nIndex == -1 )	return TRUE;

#if 0
	if (GetTimeInMillis() - m_BarcodeInputTime > BARCODE_VALID_TIME)
	{// 일정 시간 경과 후 바코드 인식 시 positon 을 초기화한다.
		ClearBarcodeInput();
	}
#endif

	if(m_bBarcodeAutoClear[nIndex] == TRUE)
	{
		// clear barcode info
		EraseBarcodeInfo(nIndex);

		ClearBarcodeInput();
	}

	if (pMsg->message == WM_CHAR)
	{
		if (pMsg->wParam == VK_TAB || 
			pMsg->wParam == VK_BACK || 
			pMsg->wParam == VK_SPACE)
		{
			return CDialogEx::PreTranslateMessage(pMsg);
		}
	
		if (m_nBarcodeIdx <= 0 || m_nBarcodeChIdx <= 0)
		{
			m_nBarcodeIdx = 0;
			m_nBarcodeChIdx = 0;
			m_cBarcode[m_nBarcodeIdx++] = (BYTE)pMsg->wParam;
			m_nBarcodeChIdx++;
		}
		else if (m_nBarcodeChIdx <= (m_nBarcode_Length + 20))	  //m_nBarcode_Length 보다 더 읽는다.
		{
			m_cBarcode[m_nBarcodeIdx++] = (BYTE)pMsg->wParam;
			m_nBarcodeChIdx++;
		}

		m_ListCtrl.SetItemText( nIndex, BARCODE_LIST_ITEM_BARCODE, m_cBarcode );
	}
	else if (pMsg->message == WM_IME_CHAR)
	{
		if (IsDBCSLeadByte((BYTE)(pMsg->wParam >> 8)))
		{
			m_cBarcode[m_nBarcodeIdx++] = HIBYTE(LOWORD(pMsg->wParam));
			m_cBarcode[m_nBarcodeIdx++] = LOBYTE(LOWORD(pMsg->wParam));
			m_nBarcodeChIdx++;
		}
		else
		{
			m_cBarcode[m_nBarcodeIdx++] = LOBYTE(LOWORD(pMsg->wParam));
			m_nBarcodeChIdx++;
		}

		m_ListCtrl.SetItemText( nIndex, BARCODE_LIST_ITEM_BARCODE, m_cBarcode );
	}

	//m_BarcodeInputTime = GetTimeInMillis();

	if (m_nBarcodeChIdx == m_nBarcode_Length)		// Key 입력수와 Length가 동일한 경우
	{
		CompletedBarcodeInput();
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

LRESULT CBarcodeSettingDlg::OnClickListBarcode(WPARAM wParam, LPARAM lParam)
{
	int nCurSel = (int)wParam;

	if(m_nListSelectionItem != -1 && m_nListSelectionItem != nCurSel)
	{
		ClearBarcodeInput();
	}

	m_nListSelectionItem = nCurSel;

	return 1L;
}

int CBarcodeSettingDlg::GetNextSelectItem(int nCurItem)
{
	int nNextItem = nCurItem + 1;

	int nCount = m_ListCtrl.GetItemCount();

	for ( ; nNextItem < nCount; nNextItem++)
	{
		if ( m_ListCtrl.GetItemDisable(nNextItem, 0) == FALSE )
			break;
	}

	if (nNextItem == nCount)
	{
		if (m_bAutoStartTest)
			return nCount;
		else
			return nCurItem;
	}
	
	return nNextItem;
}

void CBarcodeSettingDlg::CompletedBarcodeInput()
{
	// barcode is empty
	if (m_nBarcodeIdx == 0)
		return;

	int nSelectedItem = UiCtrl_GetListFocus();
	if( nSelectedItem == -1 )	return;

	// start test if next item
	if (m_bPartialTestOn && m_bAutoStartTest)
	{
		SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_PARTIAL_TEST_01 + nSelectedItem, BN_CLICKED), (LPARAM)this->m_hWnd);
		return;
	}

	// get next item
	nSelectedItem = GetNextSelectItem(nSelectedItem);

	if(m_bPartialTestOn == FALSE && (m_ListCtrl.GetItemCount() == nSelectedItem ))
	{
		SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_BARCODE_START_TEST_ALL, BN_CLICKED), (LPARAM)this->m_hWnd);
		return;
	}

	// set focus to next item
	UiCtrl_SetListFocus(nSelectedItem);

	ClearBarcodeInput();
}

void CBarcodeSettingDlg::ClearBarcodeInput()
{
	m_nBarcodeIdx = 0;
	m_nBarcodeChIdx = 0;

	memset(m_cBarcode, 0x00, sizeof(m_cBarcode));
}

// 테스트 후 이전 바코드 자동 CLEAR 기능
// - 테스트 완료 후 5초뒤 이전 바코드는 자동 CLEAR
// - 테스트 완료 후 5초가 지나기 전 바코드 입력 시 즉시 CLEAR
// - 테스트 완료 후 5초가 지나기 전 다른 DEVICE 테스트 시작 시 즉시 CLEAR
void CBarcodeSettingDlg::EraseBarcodeInfo(int nIndex, BOOL bForce)
{
	if(nIndex < 0 || nIndex >= MAX_DEVICE_COUNT)
		return;

	KillTimer(TIMER_CLEAR_BARCODE_INFO_0 + nIndex);

	if(bForce == FALSE && m_bBarcodeAutoClear[nIndex] == FALSE)
		return;

	m_bBarcodeAutoClear[nIndex] = FALSE;

	CTestModeView * pTestModeView = (CTestModeView*)m_pParent;
	CUsbHidList	* pUsbHidList = (CUsbHidList*)pTestModeView->GetUsdHidList();
	CUsbHid * pUsbHid = NULL;

	pUsbHid = pUsbHidList->GetUsbHidPtrByDevIndex(nIndex);
	if (pUsbHid)
		m_ListCtrl.SetItemText(nIndex, BARCODE_LIST_ITEM_BARCODE, "");
	else
		m_ListCtrl.SetItemText(nIndex, BARCODE_LIST_ITEM_BARCODE, "-");

	if(bForce == TRUE)
		ClearBarcodeInput();
}


BOOL CBarcodeSettingDlg::CheckBarcodeEmpty(int nIndex)
{
	BOOL bEmpty = FALSE;
	CString str;

	if(nIndex == -1)
	{
		int nCount = m_ListCtrl.GetItemCount();

		for(int i=0; i<nCount; i++)
		{
			str = m_ListCtrl.GetItemText(i, BARCODE_LIST_ITEM_BARCODE);

			// check empty
			if (str.IsEmpty())
			{
				bEmpty = TRUE;
				break;
			}
		}
	}
	else
	{
		str = m_ListCtrl.GetItemText(nIndex, BARCODE_LIST_ITEM_BARCODE);

		// check empty
		if (str.IsEmpty())
		{
			bEmpty = TRUE;
		}
	}

	return bEmpty;
}

BOOL CBarcodeSettingDlg::CheckBarcodeLength(int nIndex)
{
	BOOL bLengthOk = TRUE;
	CString str;

	if (m_bFixedBarcodeLength == FALSE)
		return TRUE;

	if(nIndex == -1)
	{
		int nCount = m_ListCtrl.GetItemCount();

		for(int i=0; i<nCount; i++)
		{
			if ( m_ListCtrl.GetItemDisable(i, 0) )
				continue;
				
			str = m_ListCtrl.GetItemText(i, BARCODE_LIST_ITEM_BARCODE);

			int nTextLength = str.GetLength();

			if ( nTextLength != m_nBarcode_Length)
			{
				bLengthOk = FALSE;
				break;
			}
		}
	}
	else
	{
		if ( m_ListCtrl.GetItemDisable(nIndex, 0) )
			return FALSE;
			
		str = m_ListCtrl.GetItemText(nIndex, BARCODE_LIST_ITEM_BARCODE);

		int nTextLength = str.GetLength();


		if ( nTextLength != m_nBarcode_Length)
		{
			bLengthOk = FALSE;
		}
	}

	return bLengthOk;
}

void CBarcodeSettingDlg::OnBnClicked_StartTestAll()
{
	CTestModeView * pTestModeView = (CTestModeView*)m_pParent;
	CUsbHidList	* pUsbHidList = (CUsbHidList*)pTestModeView->GetUsdHidList();

	for (int n = 0; n < MAX_DEVICE_COUNT; n++)
	{		
		// clear barcode info
		EraseBarcodeInfo(n);
	}

	if (CheckBarcodeEmpty(-1))
	{
		AfxMessageBox("Please input the barcode");
		return;
	}

	if (CheckBarcodeLength(-1) == FALSE)
	{
		CString str;
		str.Format("The length of the barcode is different from the set value.");
		AfxMessageBox(str);
		return;
	}

	// set barcode
	for (int n = 0; n < MAX_DEVICE_COUNT; n++)
	{
		CUsbHid * pUsbHid = pUsbHidList->GetUsbHidPtrByDevIndex(n);
		if (pUsbHid == NULL)		continue;

		pUsbHid->m_strBarcode.Format("%s", m_ListCtrl.GetItemText(n, BARCODE_LIST_ITEM_BARCODE));
	}

	// start TSM test - ALL
	pTestModeView->SendMessage(WM_USER_BARCODE_CALLBACK_EVENT, E_BARCODE_SETTING_START_TEST, -1);

	UiCtrl_EnableButton();
}

void CBarcodeSettingDlg::OnBnClicked_StartPartialTest(UINT nID)
{
	CTestModeView * pTestModeView = (CTestModeView*)m_pParent;
	CUsbHidList	* pUsbHidList = (CUsbHidList*)pTestModeView->GetUsdHidList();

	int nIndex = nID - IDC_BUTTON_PARTIAL_TEST_01;

	// clear barcode info
	EraseBarcodeInfo(nIndex);

	if (CheckBarcodeEmpty(nIndex))
	{
		AfxMessageBox("Please input the barcode");
		return;
	}

	if (CheckBarcodeLength(nIndex) == FALSE)
	{
		CString str;
		str.Format("The length of the barcode is different from the set value.");
		AfxMessageBox(str);
		return;
	}

	// set barcode
	CUsbHid * pUsbHid = pUsbHidList->GetUsbHidPtrByDevIndex(nIndex);
	if (pUsbHid == NULL || pUsbHid->IsTestAvailable() == FALSE || pUsbHid->m_bTesting == TRUE)
		return;

	pUsbHid->m_strBarcode.Format("%s", m_ListCtrl.GetItemText(nIndex, BARCODE_LIST_ITEM_BARCODE));

	UiCtrl_SetListFocus(nIndex);

	// start TSM test - PARTIAL
	pTestModeView->SendMessage(WM_USER_BARCODE_CALLBACK_EVENT, E_BARCODE_SETTING_START_TEST, nIndex);

	UiCtrl_EnableButton();

	ClearBarcodeInput();
}

void CBarcodeSettingDlg::OnCancel()
{
	CTestModeView * pTestModeView = (CTestModeView*)m_pParent;
	if(pTestModeView == NULL) return;

	pTestModeView->PostMessageEx(WM_USER_BARCODE_CALLBACK_EVENT, E_BARCODE_SETTING_HIDE_DIALOG, 0);
}

LRESULT CBarcodeSettingDlg::OnShowDialog(WPARAM wParam, LPARAM lParam)
{
	// show or hide
	ShowDialog((int)wParam);

	return 1L;
}

LRESULT CBarcodeSettingDlg::OnDeviceChanged(WPARAM wParam, LPARAM lParam)
{
	UiCtrl_Initialize(FALSE);

	return 1L;
}

LRESULT CBarcodeSettingDlg::OnTestCompleted(WPARAM wParam, LPARAM lParam)
{
	int nIndex = (int)wParam;
	BOOL bError = lParam ? TRUE : FALSE;

	UiCtrl_EnableButton();

	if( bError == FALSE )
	{
		m_bBarcodeAutoClear[nIndex] = TRUE;

		SetTimer(TIMER_CLEAR_BARCODE_INFO_0 + nIndex, 5000, NULL);
	}

	return 1L;
}

void CBarcodeSettingDlg::OnTimer(UINT_PTR nIDEvent)
{	
	if (nIDEvent >= TIMER_CLEAR_BARCODE_INFO_0 && nIDEvent <= TIMER_CLEAR_BARCODE_INFO_7)
	{
		// clear barcode info
		EraseBarcodeInfo(nIDEvent - TIMER_CLEAR_BARCODE_INFO_0, TRUE);
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CBarcodeSettingDlg::ShowDialog(int nShow)
{
	if (nShow)
	{
		// Initialize ListCtrl
		UiCtrl_Initialize(TRUE);

		OnCheckBnClicked_PartialTest();

		ClearBarcodeInput();
		
		CenterWindow();
	}

	m_bShowDialog = nShow ? TRUE : FALSE;

	ShowWindow(nShow);
}

void CBarcodeSettingDlg::OnCheckBnClicked_PartialTest()
{
	UpdateData(TRUE);

	UiCtrl_EnableButton();

	GetDlgItem(IDC_CHECK_BARCODE_PARTIAL_TEST)->Invalidate();
}

HBRUSH CBarcodeSettingDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if(pWnd->GetDlgCtrlID() == IDC_CHECK_BARCODE_PARTIAL_TEST)
	{
		ChangeCheckboxBgColor(pDC, m_bPartialTestOn);
	}

	return hbr;
}

