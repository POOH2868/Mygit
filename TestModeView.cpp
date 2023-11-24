// TestModeView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "shlwapi.h"
#include <sys/timeb.h>

#include "TouchDebugger.h"
#include "TestModeView.h"

#include "util.h"

#include "TestOptionDlg.h"
#include "ASIC_ID_dlg.h"

#include "InputDlg.h"
#include "StartupPasswordDlg.h"

#include "ZDataAnalyzerDlg.h"

#if defined(D_HV_FILE_UNLOCKER)
extern void FileUnlocker_Unlock(CString strFileName);
#endif

// CTestModeView //

CUsbHidList	* CTestModeView::m_pUsbHidList = NULL;

int CTestModeView::m_Pass_Count;
int CTestModeView::m_Fail_Count;
BOOL CTestModeView::m_bResetCount;

IMPLEMENT_DYNCREATE(CTestModeView, CFormView)

CTestModeView::CTestModeView()
	: CFormView(CTestModeView::IDD)
{
	m_bCreatePane = FALSE;
	m_bAleadyTestEnter = FALSE;
	m_bAleadyTestReady = FALSE;
	m_bTestEnterByFwAutoDownload = FALSE;

	m_pBarcodeSettingDlg      = NULL;

	m_bIsDefaultConfig = TRUE;
	m_nEnableTestCount = 0;
	m_nMaxMpToolVersion = 0;

	m_nNeedResearch = 0;
	m_bExistDevice = FALSE;

	m_nBarcodeChIdx = 0;
	m_nBarcodeIdx = 0;

#ifdef _DEBUG
	m_bIsLogin = TRUE;
#else
	m_bIsLogin = FALSE;
#endif

	m_nTestAvailableCount = 0;

	m_bEnableStartButton = FALSE;

#if defined(D_USE_CHECK_MPTOOL_REVISION)
	m_TestInfo_CURRENT_CONSUMPTION.bEnable = FALSE;
#else
	m_TestInfo_CURRENT_CONSUMPTION.bEnable = TRUE;
#endif

	m_imageListCtrl = NULL;
	m_nMaxDeviceIndex = 0;

	m_pUsbHidList = NULL;

	m_pTabWndShow = NULL;

	#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
	m_pConnectInfoDlg = NULL;
	#endif

#if defined(D_USE_CHECK_BUTTON_TEST)
	m_pGpioButtonDlg = NULL;
	memset(&m_GpioButtonStatus, 0x00, sizeof(BUTTON_STATUS));

	m_pTouchButtonDlg = NULL;
	memset(&m_TouchButtonStatus, 0x00, sizeof(BUTTON_STATUS));
#endif

	m_BarcodeInputTime = GetTimeInMillis();

	m_strIniErroMsg = _T("");
	m_strUserNumber = _T("");

	m_Pass_Count = 0;
	m_Fail_Count = 0;
	m_bResetCount = FALSE;

	m_nModuleVendorCode = 0;

	m_eAllRetryStatus = eAllRetryStatus_Nothing;

	memset(&g_FwDownloadInfo, 0x00, sizeof(T_FW_DOWNLOAD_INFO));
}

CTestModeView::~CTestModeView()
{
	BOOL bDirExist = TRUE;

	// rename LOGGING file
	if (PathFileExists(m_strLoggingFileName))
	{
		COleDateTime oleDateTime = COleDateTime::GetCurrentTime();
		CString strFileDir;

		if (m_bAdditionalLOG)
		{
			strFileDir.Format("%s\\SAVE_LOG", m_strSaveResultDir);
		}
		else
		{
			strFileDir.Format("%s\\DATA", m_strSaveResultDir);

			if (PathFileExists(strFileDir) == FALSE)
				CreateDirectory((LPCTSTR)strFileDir, NULL);

			strFileDir.Format("%s\\DATA\\%02d%02d%02d", m_strSaveResultDir, oleDateTime.GetYear() % 100, oleDateTime.GetMonth(), oleDateTime.GetDay());
		}

		if ((PathFileExists(strFileDir) == FALSE) && (CreateDirectory((LPCTSTR)strFileDir, NULL) == FALSE))
		{
			bDirExist = FALSE;

			CString str;
			str.Format("Cannot create directory (%s)", strFileDir);
			AfxMessageBox(str);
		}
		else
		{
			if (m_bAdditionalLOG)
			{
				strFileDir.Format("%s\\SAVE_LOG\\LOGGING", m_strSaveResultDir);

				if ((PathFileExists(strFileDir) == FALSE) && (CreateDirectory((LPCTSTR)strFileDir, NULL) == FALSE))
				{
					bDirExist = FALSE;

					CString str;
					str.Format("Cannot create directory (%s)", strFileDir);
					AfxMessageBox(str);
				}
			}
		}

		if (bDirExist)
		{
			CString strNewFile;
			strNewFile.Format("%s\\LOGGING_%02d%02d%02d_%02d%02d%02d.csv", (LPCTSTR)strFileDir, oleDateTime.GetYear() % 100, oleDateTime.GetMonth(), oleDateTime.GetDay(), oleDateTime.GetHour(), oleDateTime.GetMinute(), oleDateTime.GetSecond());

			Rename(m_strLoggingFileName, strNewFile, FALSE);
		}
	}

	#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
	if (m_pConnectInfoDlg)
	{
		m_pConnectInfoDlg->DestroyWindow();
		delete m_pConnectInfoDlg;
		m_pConnectInfoDlg = NULL;
	}
	#endif

#if defined(D_USE_CHECK_BUTTON_TEST)
	if (m_pGpioButtonDlg)
	{
		m_pGpioButtonDlg->DestroyWindow();
		delete m_pGpioButtonDlg;
		m_pGpioButtonDlg = NULL;
	}

	if (m_pTouchButtonDlg)
	{
		m_pTouchButtonDlg->DestroyWindow();
		delete m_pTouchButtonDlg;
		m_pTouchButtonDlg = NULL;
	}
#endif

	if(m_pBarcodeSettingDlg)
	{
		m_pBarcodeSettingDlg->DestroyWindow();
		delete m_pBarcodeSettingDlg;
	}

	if (m_ListFont.GetSafeHandle())
		m_ListFont.DeleteObject();

	if (m_StartButtonFont.GetSafeHandle())
		m_StartButtonFont.DeleteObject();

	// Remove connected devices
	RemoveAllHidDevice();

	for(int i=0; i<MAX_DEVICE_COUNT; i++)
	{
		m_TestResultDescDlg[i].DestroyWindow();
	}

	ZT_DELETE_OBJ(m_pUsbHidList);

	ZT_DELETE_OBJ(m_imageListCtrl);

	ZT_FREE(g_FwDownloadInfo.pFwBinaryData);

	HV_TRACE_FINALIZE();
}

void CTestModeView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INFO, m_listInfo);
	DDX_Control(pDX, IDC_BUTTON_OPTION, m_btnOption);
	DDX_Control(pDX, IDC_BUTTON_START_TEST_ALL, m_btnStartTestAll);
	DDX_Control(pDX, IDC_STATIC_TITLE, m_picTitle);
	DDX_Control(pDX, IDC_STATIC_CUSTOMER_TITLE, m_staticCustomerTitle);
	DDX_Control(pDX, IDC_STATIC_CUSTOMER, m_staticCustomer);
	DDX_Control(pDX, IDC_STATIC_PROJECT_NAME_TITLE, m_staticProjectNameTitle);
	DDX_Control(pDX, IDC_STATIC_PROJECT_NAME, m_staticProjectName);
	DDX_Control(pDX, IDC_STATIC_RELEASE_VERSION_TITLE, m_staticVersionTitle);
	DDX_Control(pDX, IDC_STATIC_RELEASE_VERSION, m_staticVersion);
	DDX_Control(pDX, IDC_STATIC_BARCODE_TITLE, m_staticBarcodeTitle);
	DDX_Control(pDX, IDC_STATIC_BARCODE, m_staticBarcode);
	DDX_Control(pDX, IDC_PIC_PASS, m_picPass);
	DDX_Control(pDX, IDC_PIC_FAIL, m_picFail);
	DDX_Control(pDX, IDC_PIC_STANDBY, m_picStandBy);
	DDX_Control(pDX, IDC_STATIC_PASS_COUNT_TITLE, m_staticPassCountTitle);
	DDX_Control(pDX, IDC_STATIC_PASS_COUNT, m_staticPassCount);
	DDX_Control(pDX, IDC_STATIC_FAIL_COUNT_TITLE, m_staticFailCountTitle);
	DDX_Control(pDX, IDC_STATIC_FAIL_COUNT, m_staticFailCount);
	DDX_Control(pDX, IDC_STATIC_TOTAL_COUNT_TITLE, m_staticTotalCountTitle);
	DDX_Control(pDX, IDC_STATIC_TOTAL_COUNT, m_staticTotalCount);
	DDX_Control(pDX, IDC_TAB_TEST_RESULT_DESC, m_TabCtrl);
}

BEGIN_MESSAGE_MAP(CTestModeView, CFormView)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_START_TEST_ALL, CTestModeView::OnBnClickedButtonStartTestAll)
	ON_BN_CLICKED(IDC_BUTTON_OPTION, CTestModeView::OnBnClickedButtonOption)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_INFO, CTestModeView::OnLvnGetdispinfoListInfo)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_INFO, CTestModeView::OnNMCustomdrawListInfo)
	ON_WM_DRAWITEM()	//Edit
	ON_MESSAGE(WM_USER_LIST_CTRL_HEADER_CLICK, &CTestModeView::OnListCtrlColumnClick)
	ON_MESSAGE(WM_USER_TSM_TEST_START_BY_FW_AUTO_DOWNLOAD, &CTestModeView::OnStartTest_ByFwAutoDownload)
	ON_MESSAGE(WM_USER_EXECUTE_ASIC_ID_MODE, &CTestModeView::OnExecuteAsicIdMode)
	ON_MESSAGE(WM_USER_EXECUTE_GRAPH_MODE, &CTestModeView::OnExecuteGraphMode)
	ON_MESSAGE(WM_USER_BARCODE_RUN_DIALOG, &CTestModeView::OnExecuteBarcodeSetting)
	ON_MESSAGE(WM_USER_BARCODE_CALLBACK_EVENT, &CTestModeView::OnCallbackByBarcodeSetting)
	ON_MESSAGE(WM_USER_USB_HID_DEVICE_DISCONNECT, &CTestModeView::OnUsbHidDeviceDisconnect)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_TEST_RESULT_DESC, &CTestModeView::TestResultDesc_OnTcnSelchangeTab)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB_TEST_RESULT_DESC, &CTestModeView::TestResultDesc_OnSelchangingTab)
	ON_MESSAGE(WM_USER_SET_FOCUS_TO_LIST, &CTestModeView::OnSetFocusToList)
#ifdef _DEBUG
	ON_MESSAGE(WM_USER_MY_TEST_IUM_DEBUGGING_EVENT, CTestModeView::OnMyTestEvent)
#endif
END_MESSAGE_MAP()


// CTestModeView 진단입니다.

#ifdef _DEBUG
void CTestModeView::AssertValid() const
{
	CFormView::AssertValid();
}

void CTestModeView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

CUsbHid * CTestModeView::GetUsbHidByIndex(int nIndex)
{
	if ((nIndex < 0) || (nIndex >= GetMaxDeviceCount()))
	{
		return NULL;
	}

	return m_pUsbHidList->GetUsbHidPtr(nIndex);
}

CUsbHid * CTestModeView::GetUsbHidByDevIndex(int nIndex)
{
	if ((nIndex < 0) || (nIndex >= GetMaxDeviceCount()))
	{
		return NULL;
	}

	return m_pUsbHidList->GetUsbHidPtrByDevIndex(nIndex);
}

void CTestModeView::OnInitialUpdate()
{
	CRect rect;

	CFormView::OnInitialUpdate();

	// Get program file path
	m_strZtdExeDir = GetCurrentAppDirectory();
	PathRemoveBackslash(m_strZtdExeDir.GetBuffer());

	// Create "CONFIG" directory
	m_strConfigDir.Format("%s\\CONFIG", m_strZtdExeDir);
	CreateDirectory((LPCTSTR)m_strConfigDir, NULL);

	// Create "FIRMWARE" directory
	m_strFirmwareDir.Format("%s\\FIRMWARE", m_strZtdExeDir);
	//CreateDirectory((LPCTSTR)m_strFirmwareDir, NULL);			// create directory after load ini file

	DeleteTempLoggingFile();

	TestInfo_LoadData();

	BarcodeDisplayInTestVisible();

	CString str;
	str.Format("%s\\LOG\\", m_strZtdExeDir);
	HV_TRACE_INITIALIZE(str);

	g_pTestModeViewWnd = this;

	// Create USB HID devices list
	m_pUsbHidList = new CUsbHidList(this);

	// Initialize list control
	m_listInfo.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);
	m_listInfo.ModifyStyle(0, LVS_REPORT);

	m_imageListCtrl = new CImageList();
	m_imageListCtrl->Create(18, 18, ILC_COLOR32 | ILC_MASK, 0, 1);
	m_imageListCtrl->Add(AfxGetApp()->LoadIcon(IDI_ICON_LED_OFF));
	m_imageListCtrl->Add(AfxGetApp()->LoadIcon(IDI_ICON_LED_ON));

	m_listInfo.SetImageList(m_imageListCtrl, LVSIL_SMALL);
	m_listInfo.SetImageList(m_imageListCtrl, LVSIL_NORMAL);

#if 1	// Set font for list control
	LOGFONT        LogFont;
	m_listInfo.GetFont()->GetLogFont(&LogFont);

	LogFont.lfWeight = 1000;	// 굵기설정( MAX : 1000 )
	LogFont.lfHeight = 15;		// Font Size 설정

	m_ListFont.CreateFontIndirect(&LogFont);
	m_listInfo.SetFont(&m_ListFont);

	LogFont.lfHeight = 20;		// Font Size 설정

	m_StartButtonFont.CreateFontIndirect(&LogFont);
	m_btnStartTestAll.SetFont(&m_StartButtonFont);
#endif

	// Hide pass, fail bitmap
	m_picPass.ShowWindow(FALSE);
	m_picFail.ShowWindow(FALSE);
	m_picStandBy.ShowWindow(TRUE);

	UpdateData(FALSE);

	m_bCreatePane = TRUE;
}

BOOL CTestModeView::ConfirmStartupPassword()
{
	if( m_bUseStartup == FALSE )
		return TRUE;

	BOOL bStartupPassword = FALSE;

	CStartupPasswordDlg dlg;

	if (dlg.DoModal() == IDOK)
	{
		CString strPassword = dlg.m_strPassword;

		//if (strPassword.IsEmpty())
		//	return FALSE;

		if (strPassword.CompareNoCase("ZINITIX1") == 0 ||
			strPassword.CompareNoCase(m_strStartupPassword) == 0)
		{
			bStartupPassword = TRUE;
		}
	}

	return bStartupPassword;
}

void CTestModeView::EnterTestModeView()
{
	ListCtrl_Initialize();

	TestResultDesc_Initialize();

	DisplayTotalTestStep();

	ShowWindow(SW_SHOW);

	if( !m_strIniErroMsg.IsEmpty() )
	{
		AfxMessageBox(m_strIniErroMsg);
	}

	::SetFocus((GetDlgItem(IDC_BUTTON_START_TEST_ALL))->m_hWnd);

	EnableStartButton(TRUE);

	UsbHidDevice_StartSurveyTimer(TRUE);
}

void CTestModeView::EnableStartButton(BOOL bEnable)
{
	if (bEnable)
	{
		if( GetTestAvailableCount() == 0 )
			bEnable = FALSE;
		else if(m_pBarcodeSettingDlg && m_pBarcodeSettingDlg->IsShowDialog())
			bEnable = FALSE;
		else if (m_bUseBarcode && m_bFixedBarcodeLength && m_nBarcodeChIdx != GetBarcodeLength())
			bEnable = FALSE;
	}

	if (bEnable)
	{
		m_btnStartTestAll.EnableWindow(TRUE);
	}
	else
	{
		m_btnStartTestAll.EnableWindow(FALSE);
	}

	m_bEnableStartButton = bEnable;
}

void CTestModeView::EnableOptionButton(BOOL bEnable)
{
	m_btnOption.EnableWindow(bEnable);
}

void CTestModeView::GotoStandBy()
{
	ListCtrl_Initialize();

	TestResultDesc_Initialize();

	m_picPass.ShowWindow(FALSE);
	m_picFail.ShowWindow(FALSE);
	m_picStandBy.ShowWindow(TRUE);

	// update list control
	DisplayTotalTestStep();
}

void CTestModeView::ExitSolutionResetNotify()
{
	CUsbHid * pUsbHid;

	KillTimer(TIMER_SOLUTION_RESULT_NOTIFY);

	int nDeviceCount = m_TestDevIndex.GetCount();

	for (int n = 0; n < nDeviceCount; n++)
	{
		pUsbHid = GetUsbHidByDevIndex(m_TestDevIndex.GetAt(n));
		if (pUsbHid != NULL)
		{
			pUsbHid->ControlGpioPb15(FALSE);
		}
	}
}

void CTestModeView::DeleteTempLoggingFile()
{
	// set LOGGING file name
	COleDateTime oleDateTime = COleDateTime::GetCurrentTime();

	m_strLoggingFileName.Format("%s\\LOGGING_Temp_%04d%02d%02d_%02d%02d%02d", m_strZtdExeDir, oleDateTime.GetYear(), oleDateTime.GetMonth(), oleDateTime.GetDay(), oleDateTime.GetHour(), oleDateTime.GetMinute(), oleDateTime.GetSecond());

	// Delete Temp Logging File
	CString strFindPath, strName;
	CFileFind m_finder;

	strFindPath.Format("%s\\*.*", m_strZtdExeDir);

	BOOL bFound = m_finder.FindFile(strFindPath);
	while (bFound)
	{
		bFound = m_finder.FindNextFile();

		if (m_finder.IsHidden() || m_finder.IsSystem() || m_finder.IsDirectory())
			continue;

		strName = m_finder.GetFileName();

		if (strName.Find("LOGGING_Temp_") == 0)
		{
			CString str;
			str.Format("%s\\%s", m_strZtdExeDir, strName);
			DeleteFile(str);
		}
	}
	m_finder.Close();
}

void CTestModeView::ResetTestCount()
{
	m_Pass_Count = m_Fail_Count = 0;

	m_bResetCount = TRUE;

	CString str;
	str.Format(" Initializes the count ");
	AfxMessageBox(str);
}

// CTestModeView 메시지 처리기입니다.
#define	START_BUTTON_HEIGHT		30
#define	TITLE_HEIGHT			45
#define	STATIC_TEXT_HEIGHT		20
#define	BUTTON_HEIGHT			(STATIC_TEXT_HEIGHT * 3 + TOP_OFFSET * 2)	// 45
#define	TOP_OFFSET				5
#define	VERTICAL_OFFSET			5
#define	SIDE_OFFSET				10
#define	BB_OFFSET				1

#define PASS_FAIL_ICON_HEIGHT	140	//140
#define PASS_FAIL_ICON_WIDTH	210

#define BUTTON_COUNT			7

void CTestModeView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if(m_bCreatePane == FALSE)	return;

	CRect wrect, rect, rect_tmp , rect_barcodeTitle , rect_barcode;
	int nButtonWidth;

	GetClientRect( &wrect );

	nButtonWidth = (wrect.Width() - SIDE_OFFSET * 2 - BB_OFFSET) / 10;

	// title
	rect = wrect;
	rect.left += SIDE_OFFSET;
	rect.right = rect.left + 159;
	rect.top = TOP_OFFSET;
	rect.bottom = TOP_OFFSET + TITLE_HEIGHT;
	m_picTitle.MoveWindow(&rect);

	// Customer
	rect = wrect;
	rect.left += (rect.right - SIDE_OFFSET - (nButtonWidth * 6) - (BB_OFFSET * 3));
	rect.right = rect.left + nButtonWidth;
	rect.top = TOP_OFFSET + TITLE_HEIGHT - STATIC_TEXT_HEIGHT;
	rect.bottom = TOP_OFFSET + TITLE_HEIGHT;
	m_staticCustomerTitle.MoveWindow(&rect);

	rect_barcodeTitle = rect;	//Customer 위에 위치 하므로.

	rect.left += (nButtonWidth + BB_OFFSET);
	rect.right = rect.left + nButtonWidth;
	m_staticCustomer.MoveWindow(&rect);

	rect_barcode = rect;		//Customer 위에 위치 하므로.

	rect.left += (nButtonWidth + BB_OFFSET);
	rect.right = rect.left + nButtonWidth;
	m_staticProjectNameTitle.MoveWindow(&rect);

	rect.left += (nButtonWidth + BB_OFFSET);
	rect.right = rect.left + nButtonWidth;
	m_staticProjectName.MoveWindow(&rect);

	rect.left += (nButtonWidth + BB_OFFSET);
	rect.right = rect.left + nButtonWidth;
	m_staticVersionTitle.MoveWindow(&rect);

	rect.left += (nButtonWidth + BB_OFFSET);
	rect.right = rect.left + nButtonWidth;
	m_staticVersion.MoveWindow(&rect);

	// Barcode Display
	// title 기준으로 위치 , 사이즈 설정.
	rect_barcodeTitle.top -= TITLE_HEIGHT/2;
	rect_barcodeTitle.bottom -= TITLE_HEIGHT/2;
	m_staticBarcodeTitle.MoveWindow(&rect_barcodeTitle);

	rect_barcode.top -= TITLE_HEIGHT/2;
	rect_barcode.bottom -= TITLE_HEIGHT/2;
	rect_barcode.right = rect.right;
	m_staticBarcode.MoveWindow(&rect_barcode);

	// ListCtrl
	rect = wrect;
	rect.left += SIDE_OFFSET;
	rect.right -= SIDE_OFFSET;
	rect.top = TOP_OFFSET + TITLE_HEIGHT + TOP_OFFSET;
	rect.bottom = rect.bottom - BUTTON_HEIGHT - VERTICAL_OFFSET - TOP_OFFSET - PASS_FAIL_ICON_HEIGHT;
	m_listInfo.MoveWindow(&rect);

	rect_tmp = rect;

	// ListBox - Result Description
	//rect.left += SIDE_OFFSET;
	rect.right -= (PASS_FAIL_ICON_WIDTH + SIDE_OFFSET);
	rect.top = rect.bottom + VERTICAL_OFFSET;
	rect.bottom = rect.top + PASS_FAIL_ICON_HEIGHT;
	m_TabCtrl.MoveWindow(&rect);

	for (int i = 0; i < MAX_DEVICE_COUNT; i++)
	{
		if (m_TestResultDescDlg[i])
			m_TestResultDescDlg[i].SetWindowPos(NULL, 10, 10, rect.Width() - 10, rect.Height() - 30, SWP_NOMOVE | SWP_NOZORDER);
	}

	// pass/fail image
	rect = rect_tmp;

	rect.top = rect.bottom + VERTICAL_OFFSET;
	rect.bottom = rect.top + PASS_FAIL_ICON_HEIGHT;
	rect.left = wrect.Width() - PASS_FAIL_ICON_WIDTH - SIDE_OFFSET;
	rect.right = rect.left + PASS_FAIL_ICON_WIDTH;
	m_picPass.MoveWindow(&rect);
	m_picFail.MoveWindow(&rect);
	m_picStandBy.MoveWindow(&rect);
	m_picPass.RedrawWindow();
	m_picFail.RedrawWindow();
	m_picStandBy.RedrawWindow();

	// bottom buttons
	rect.left = rect_tmp.left;

	rect.top = rect.bottom + VERTICAL_OFFSET;
	rect.bottom = rect.top + BUTTON_HEIGHT;
	rect.right = rect.left + nButtonWidth;
	m_btnOption.MoveWindow(&rect);

	// Start All
	rect.left = rect.right + BB_OFFSET;
	rect.right = rect.left + (nButtonWidth * BUTTON_COUNT);
	m_btnStartTestAll.MoveWindow(&rect);

	rect_tmp = rect;

	//Pass Count position
	rect.left = rect_tmp.right + BB_OFFSET;
	rect.right = rect.left + nButtonWidth;
	rect.top = rect_tmp.top;
	rect.bottom = rect.top + STATIC_TEXT_HEIGHT;
	m_staticPassCountTitle.MoveWindow(&rect);

	rect.left = rect.right + BB_OFFSET;
	rect.right = rect.left + nButtonWidth;
	rect.top = rect_tmp.top;
	rect.bottom = rect.top + STATIC_TEXT_HEIGHT;
	m_staticPassCount.MoveWindow(&rect);

	//Fail Count position
	rect.left = rect_tmp.right + BB_OFFSET;
	rect.right = rect.left + nButtonWidth;
	rect.top = rect.top + STATIC_TEXT_HEIGHT + TOP_OFFSET;
	rect.bottom = rect.top + STATIC_TEXT_HEIGHT;
	m_staticFailCountTitle.MoveWindow(&rect);

	rect.left = rect.right + BB_OFFSET;
	rect.right = rect.left + nButtonWidth;
	//rect.top = rect.top + STATIC_TEXT_HEIGHT;
	//rect.bottom = rect.top + STATIC_TEXT_HEIGHT;
	m_staticFailCount.MoveWindow(&rect);

	//Total Count position
	rect.left = rect_tmp.right + BB_OFFSET;
	rect.right = rect.left + nButtonWidth;
	rect.top = rect.top + STATIC_TEXT_HEIGHT + TOP_OFFSET;
	rect.bottom = rect.top + STATIC_TEXT_HEIGHT;
	m_staticTotalCountTitle.MoveWindow(&rect);

	rect.left = rect.right + BB_OFFSET;
	rect.right = rect.left + nButtonWidth;
	//rect.top = rect.top + STATIC_TEXT_HEIGHT;
	//rect.bottom = rect.top + STATIC_TEXT_HEIGHT;
	m_staticTotalCount.MoveWindow(&rect);

	// Re-Draw
	m_picTitle.RedrawWindow();
	m_staticCustomerTitle.RedrawWindow();
	m_staticCustomer.RedrawWindow();
	m_staticProjectNameTitle.RedrawWindow();
	m_staticProjectName.RedrawWindow();
	m_staticVersionTitle.RedrawWindow();
	m_staticVersion.RedrawWindow();

	m_staticBarcodeTitle.RedrawWindow();
	m_staticBarcode.RedrawWindow();

	m_listInfo.RedrawWindow();
	m_staticPassCountTitle.RedrawWindow();
	m_staticPassCount.RedrawWindow();
	m_staticFailCountTitle.RedrawWindow();
	m_staticFailCount.RedrawWindow();
	m_staticTotalCountTitle.RedrawWindow();
	m_staticTotalCount.RedrawWindow();
	m_TabCtrl.RedrawWindow();

	m_btnOption.SetShade(CShadeButtonST::SHS_HARDBUMP);
	m_btnStartTestAll.SetShade(CShadeButtonST::SHS_HARDBUMP);
}

// custom drawing handler for start_test_all button
void CTestModeView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CFormView::OnDrawItem(nIDCtl, lpDrawItemStruct);

	if (nIDCtl == IDC_BUTTON_START_TEST_ALL)         // 해당 버튼인지 체크
	{
#if 1
		CDC dc;
		RECT rect;

		if (lpDrawItemStruct->itemState & ODS_DISABLED)
			return;

		dc.Attach(lpDrawItemStruct->hDC);   //  버튼의 DC 구하기
		rect = lpDrawItemStruct->rcItem;     // 버튼 영역 구하기

		UINT state = lpDrawItemStruct->itemState;  //  버튼 상태 구하기
		if ((state & ODS_SELECTED))
		{
			dc.DrawEdge(&rect, EDGE_SUNKEN, BF_RECT);
		}
		else
		{
			dc.DrawEdge(&rect, EDGE_RAISED, BF_RECT);
		}

		//dc.SetBkColor(ZT_COLOR_GREEN); // TEXT 의 백그라운드 색상

		// TEXT 색상
		if (m_bUseBarcode == TRUE && m_bAutoStartTest == TRUE)
			dc.SetTextColor(RGB(0, 0, 255));
		else if (m_bExistDevice == TRUE)
			dc.SetTextColor(ZT_COLOR_GREEN);
		else
			dc.SetTextColor(RGB(0, 0, 255));

		TCHAR buffer[MAX_PATH];           // 버튼의 TEXT를 얻기 위한 임시 버퍼
		ZeroMemory(buffer, MAX_PATH);     // 버퍼 초기화
		::GetWindowText(lpDrawItemStruct->hwndItem, buffer, MAX_PATH); // 버튼의 TEXT 얻기
		dc.DrawText(buffer, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);// 버튼에 TEXT 넣기
		dc.Detach();  // Button DC 풀어 주기

		InvalidateRect(&rect, FALSE);	//화면 갱신

		//UsbHidDevice_StartSurveyTimer();		// HIVE_SIM
#endif
	}

}

//==============================================================================================
// List Control

void CTestModeView::ListCtrl_Initialize()
{
	static BOOL bInit = FALSE;
	CUsbHid * pUsbHid = NULL;
	BOOL bEnabled = FALSE;

	int nTotalDeviceCount = GetMaxDeviceCount();

	CString str;

	if (bInit == FALSE)
	{
		bInit = TRUE;

		#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
		m_pConnectInfoDlg = new CConnectInfoDlg;
		m_pConnectInfoDlg->Create(IDD_DIALOG_CONNECT_INFO, NULL);
		m_pConnectInfoDlg->ShowWindow(SW_HIDE);
		#endif

#if defined(D_USE_CHECK_BUTTON_TEST)
		m_pGpioButtonDlg = new CGpioButtonDlg(this);
		m_pGpioButtonDlg->Create(IDD_DIALOG_GPIO_BUTTON, NULL);
		m_pGpioButtonDlg->ShowWindow(SW_HIDE);

		m_pTouchButtonDlg = new CTouchButtonDlg(this);
		m_pTouchButtonDlg->Create(IDD_DIALOG_TOUCH_BUTTON, NULL);
		m_pTouchButtonDlg->ShowWindow(SW_HIDE);
#endif

		m_pBarcodeSettingDlg = new CBarcodeSettingDlg(this);
		m_pBarcodeSettingDlg->Create(IDD_DIALOG_BARCODE, NULL);
		m_pBarcodeSettingDlg->ShowWindow(SW_HIDE);

		// Initialize
		ClearTsmTestMessageOnList(-1);

		m_listInfo.GetWindowRect(&m_rect_list);
		m_listInfo.SetParentHwnd(this->m_hWnd);

		ChangeDeviceStatus();
	}

	m_listInfo.DeleteAllItems();
	while (m_listInfo.DeleteColumn(0));

	int nColWidth = (m_rect_list.Width() - 72) / (nTotalDeviceCount + 4);

	// ListCtrl Initialize
	LV_COLUMN lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_CENTER;
	lvcolumn.cx = 22;
	lvcolumn.pszText = "";
	m_listInfo.InsertColumn(0, &lvcolumn);
	m_listInfo.SetItemState(0, ITEM_STATE_INFO);

	lvcolumn.cx = 50;
	lvcolumn.pszText = "INDEX";
	m_listInfo.InsertColumn(1, &lvcolumn);
	m_listInfo.SetItemState(1, ITEM_STATE_INFO);

	lvcolumn.fmt = LVCFMT_LEFT;
	lvcolumn.cx = (m_rect_list.Width() - 72) * 4 / (nTotalDeviceCount + 4);
	lvcolumn.pszText = "INFO";
	m_listInfo.InsertColumn(2, &lvcolumn);
	m_listInfo.SetItemState(2, ITEM_STATE_INFO);

	for (int n = 0; n < nTotalDeviceCount; n++)
	{
		pUsbHid = GetUsbHidByDevIndex(n);

		if (pUsbHid && pUsbHid->IsTestAvailable())
			bEnabled = TRUE;
		else
			bEnabled = FALSE;

		str.Format("  DEVICE %d  \n  [ START ]  ", n);

		lvcolumn.fmt = LVCFMT_CENTER;
		lvcolumn.cx = nColWidth;
		lvcolumn.pszText = str.GetBuffer();
		m_listInfo.InsertColumn(n + FIRST_DEVICE_INDEX, &lvcolumn);

		SetListState(pUsbHid, n + FIRST_DEVICE_INDEX, bEnabled ? ITEM_STATE_ENABLE : ITEM_STATE_DISABLE);
	}

	m_listInfo.Initializing(250, "Arial", LVCFMT_CENTER);

	SetButtonConfig(&m_btnOption);

	m_btnStartTestAll.SetAlign(CButtonST::ST_ALIGN_HORIZ);
	m_btnStartTestAll.SetShade(CShadeButtonST::SHS_HARDBUMP);
	m_btnStartTestAll.SetBtnCursor(IDC_CURSOR_HAND);
}

LRESULT CTestModeView::OnListCtrlColumnClick(WPARAM wParam, LPARAM lParam)
{
	int nDeviceIndex = (int)wParam - FIRST_DEVICE_INDEX;

	if (m_bUseBarcode == TRUE && m_bAutoStartTest == TRUE)
	{
		CString strMsg;
		strMsg.Format("Barcode Mode&Auto Start Mode Enable.");
		AfxMessageBox(strMsg);
		return 0L;
	}

	if (m_bAleadyTestEnter)
		TSM_StartTest_Individual(nDeviceIndex);
	else
		TSM_StartTest(nDeviceIndex);

	return 1L;
}

// LVN_GETDISPINFO event handler
void CTestModeView::OnLvnGetdispinfoListInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	//Create a pointer to the item
	LV_ITEM* pItem= &(pDispInfo)->item;
	//Which item number?
	int itemid = pItem->iItem;

	if(itemid >= (int)m_infoDatabase.size())	return;

#if 0	// HIVE_SIM. for listctrl update
	if(SemaWaitImmediate(g_hInfoSemaphore) == FALSE) return;
#endif

	//Do the list need text information?
	if (pItem->mask & LVIF_TEXT)
	{
		CString text = "";
		//Which column?
		if(pItem->iSubItem == 0)
		{
		}
		else if(pItem->iSubItem == 1)
		{
			text = m_infoDatabase[itemid].m_item;
		}
		else if (pItem->iSubItem == 2)
		{
#if defined(D_DISPLAY_NOT_USE_TEST_TO_RESERVED)
			// text is name
			if (m_infoDatabase[itemid].m_image == IMAGE_LIST_DISABLE)
				text = STRING_RESERVED_ITEM;
			else
#endif
				text = m_infoDatabase[itemid].m_info;
		}
		else if (pItem->iSubItem < (GetMaxDeviceCount() + FIRST_DEVICE_INDEX))
		{
			CString strCol = m_infoDatabase[itemid].strResult[pItem->iSubItem - FIRST_DEVICE_INDEX];
			char * chTest = strCol.GetBuffer(0);

			if (strncmp(chTest, "DOWNLOAD", 8) == 0){
				text = m_infoDatabase[itemid].strResult[pItem->iSubItem - FIRST_DEVICE_INDEX];
				text = text.Right(text.GetLength() - 8);
			}
			if (strncmp(chTest, "VOLTAGE", 7) == 0){
				text = m_infoDatabase[itemid].strResult[pItem->iSubItem - FIRST_DEVICE_INDEX];
				text = text.Right(text.GetLength() - 7);
			}
			if (strncmp(chTest, "VER_", 4) == 0){
				text = m_infoDatabase[itemid].strResult[pItem->iSubItem - FIRST_DEVICE_INDEX];
			}
			else if (strncmp(chTest, "TOTAL_TIME", 10) == 0){
				text = m_infoDatabase[itemid].strResult[pItem->iSubItem - FIRST_DEVICE_INDEX];
				text = text.Right(text.GetLength() - 10);
			}
			else if (strncmp(chTest, STRING_NOT_TEST, 8) == 0)
			{
				text = m_infoDatabase[itemid].strResult[pItem->iSubItem - FIRST_DEVICE_INDEX];
			}
			else
			{
				if (strncmp(chTest, "FAIL", 4) == 0)
					text = "FAIL";
				else if (strncmp(chTest, "PASS", 4) == 0 || strncmp(chTest, "DETECT", 6) == 0)
					text = "PASS";
			}
		}
		//Copy the text to the LV_ITEM structure
		//Maximum number of characters is in pItem->cchTextMax
		lstrcpyn(pItem->pszText, text, pItem->cchTextMax);
	}

	//Do the list need image information?
	if( pItem->mask & LVIF_IMAGE) {
		//Set which image to use
		if (pItem->iSubItem == 0)
			pItem->iImage = m_infoDatabase[itemid].m_image;
#if 1	// Delete icon display
		else
			pItem->iImage = -1;
#else
		else if (pItem->iSubItem == 1)
			pItem->iImage = -1;
		else if (pItem->iSubItem < (GetMaxDeviceCount() + FIRST_DEVICE_INDEX)) {
			char *chTest;
			CString strCol;
			strCol = m_infoDatabase[itemid].strResult[pItem->iSubItem - FIRST_DEVICE_INDEX];
			chTest = strCol.GetBuffer(0);

			if (strncmp(chTest, "FAIL", 4) == 0)
				pItem->iImage = 1;
			else if (strncmp(chTest, "PASS", 4) == 0 || strncmp(chTest, "DETECT", 6) == 0)
				pItem->iImage = 2;
			else if (strncmp(chTest, "DOWNLOAD", 8) == 0)
				pItem->iImage = 3;
			else
				pItem->iImage = -1;
		}
#endif
	}

#if 0	// HIVE_SIM. for listctrl update
	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);
#endif

	*pResult = 0;
}

// NM_CUSTOMDRAW event handler
void CTestModeView::OnNMCustomdrawListInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = (NMLVCUSTOMDRAW*)pNMHDR;

	*pResult = 0;

	switch(pLVCD->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
			return;
		}
		case CDDS_ITEMPREPAINT:
		{
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			return;
		}
		case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
		{
			COLORREF crText = RGB(0, 0, 0);

			LVITEM lv;
			lv.iItem = pLVCD->nmcd.dwItemSpec;
			lv.iSubItem = 0;
			lv.mask = LVIF_IMAGE;
			m_listInfo.GetItem(&lv);

			if( lv.iImage == IMAGE_LIST_DISABLE )
			{
				crText = ZT_COLOR_GRAY;
			}
			else if ((2 < (int)(pLVCD->iSubItem)) && ((int)(pLVCD->iSubItem) < (GetMaxDeviceCount() + FIRST_DEVICE_INDEX)))
			{
				int nItem = pLVCD->nmcd.dwItemSpec;
				CString strCol = m_listInfo.GetItemText(nItem, pLVCD->iSubItem);
				char * chTest = strCol.GetBuffer(0);

				//글자색
				if (strncmp(chTest, "FAIL", 4) == 0)
					crText = ZT_COLOR_RED;
				else if (strncmp(chTest, "PASS", 4) == 0 || strncmp(chTest, "DETECT", 6) == 0)
					crText = ZT_COLOR_GREEN;
				else if (strncmp(chTest, STRING_NOT_TEST, 8) == 0)
					crText = ZT_COLOR_GRAY;
				else if (nItem == 0)
				{
					CUsbHid * pUsbHid = GetUsbHidByDevIndex(pLVCD->iSubItem - 3);
					if (pUsbHid == NULL)
						crText = ZT_COLOR_GREEN;
					else if (m_nMaxMpToolVersion == pUsbHid->m_nMpToolVersion)
						crText = ZT_COLOR_GREEN;
					else
						crText = ZT_COLOR_BLACK;
				}
				else
					crText = ZT_COLOR_BLACK;

#if 0
				if (GetUsbHidByDevIndex(pLVCD->iSubItem - FIRST_DEVICE_INDEX)->IsTestAvailable())
					pLVCD->clrTextBk = GetSysColor(COLOR_WINDOW);
				else
					pLVCD->clrTextBk = GetSysColor(COLOR_BTNFACE);
#endif
			}

			pLVCD->clrText = crText;

			*pResult = CDRF_DODEFAULT;
			return;
		}
	}

}

void CTestModeView::AddMessageToList(int nIndex, CString strInfo, int image)
{
	CTestResultData cs;
	CString strIndex;

	if (g_bAppExit)
		return;

	strIndex.Format("%02d", nIndex);

	cs.m_item = strIndex;
	cs.m_info = strInfo;
	cs.m_image = image;

	//if( cs.m_image < 0 )
	//	cs.m_image = -1;

	if(image == IMAGE_LIST_ENABLE)
		m_nEnableTestCount++;

	SemaWait(g_hInfoSemaphore);

	m_infoDatabase.push_back( cs );
	m_listInfo.SetItemCount( (int)m_infoDatabase.size() );
	m_listInfo.EnsureVisible((int)m_infoDatabase.size() - 1, FALSE);

	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);

	m_strArrTestInfo.Add(strInfo);
}

#if 0
void CTestModeView::AddMessageOrModifyToList(CUsbHid * pUsbHid, int index, CString strIndex, CString strInfo, int image)
{
	CTestResultData cs;

	if (g_bAppExit || (pUsbHid == NULL) || (pUsbHid->m_nDevIndex < 0))
		return;

	SemaWait(g_hInfoSemaphore);

	if (index >= (int)m_infoDatabase.size())
	{
		cs.m_item = strIndex;
		cs.m_info = strInfo;
		cs.m_image = image;

		//if( cs.m_image < 0 )
		//	cs.m_image = -1;

		cs.strResult[pUsbHid->m_nDevIndex] = pUsbHid->m_strResult;

		m_infoDatabase.push_back(cs);
	}
	else
	{
		m_infoDatabase[index].strResult[pUsbHid->m_nDevIndex] = pUsbHid->m_strResult;
	}

	m_listInfo.SetItemCount((int)m_infoDatabase.size());
	//m_listInfo.EnsureVisible((int)m_infoDatabase.size() - 1, FALSE);
	m_listInfo.EnsureVisible((int)index, TRUE);

	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);
}
#endif

// Test 결과에 의해 ListCtrl 에 표시하는 함수
void CTestModeView::DisplayResultToList(CUsbHid * pUsbHid, int nResult)
{
	if (g_bAppExit || (pUsbHid == NULL) || (pUsbHid->m_nDevIndex < 0))
		return;

	int index = pUsbHid->m_nTestStep;

	SemaWait(g_hInfoSemaphore);

	if (index >= (int)m_infoDatabase.size())
	{
		ASSERT(0);
		::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);
		return;
	}

	if (nResult == RESULT_PASS)
		m_infoDatabase[index].strResult[pUsbHid->m_nDevIndex] = "PASS";
	else if (nResult == RESULT_FAIL)
		m_infoDatabase[index].strResult[pUsbHid->m_nDevIndex] = "FAIL";
	else
		m_infoDatabase[index].strResult[pUsbHid->m_nDevIndex] = STRING_NOT_TEST;

	if (index == 0 && nResult == RESULT_PASS && pUsbHid->m_bTesting == FALSE)
	{
		CString strTemp;

		if(pUsbHid->m_fVoltageVcc != 0 && pUsbHid->m_fVoltageVi2c != 0)
			strTemp.Format("VOLTAGE%.2fV / %.2fV", pUsbHid->m_fVoltageVcc, pUsbHid->m_fVoltageVi2c);
		else
			strTemp.Format("VER_%02d", pUsbHid->m_nMpToolVersion);

		m_infoDatabase[index].strResult[pUsbHid->m_nDevIndex] = strTemp;
	}

	m_infoDatabase[index].nResult[pUsbHid->m_nDevIndex] = nResult;

	m_listInfo.SetItemCount((int)m_infoDatabase.size());
	m_listInfo.EnsureVisible((int)index, TRUE);

	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);

	if (nResult == RESULT_FAIL)
	{
		pUsbHid->m_nResult = RESULT_FAIL;
	}
}

// Test 결과에 의해 ListCtrl 에 표시하고 Error List 에 error 문구를 추가하는 함수
void CTestModeView::DisplayResultFailToList(CUsbHid * pUsbHid, LPCTSTR pstrFormat, ...)
{
	if (g_bAppExit || (pUsbHid == NULL) || (pUsbHid->m_nDevIndex < 0))
		return;

	int index = pUsbHid->m_nTestStep;

	SemaWait(g_hInfoSemaphore);

	if (index >= (int)m_infoDatabase.size())
	{
		ASSERT(0);
		::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);
		return;
	}

	m_infoDatabase[index].strResult[pUsbHid->m_nDevIndex] = "FAIL";
	m_infoDatabase[index].nResult[pUsbHid->m_nDevIndex] = RESULT_FAIL;

	m_listInfo.SetItemCount((int)m_infoDatabase.size());
	m_listInfo.EnsureVisible((int)index, TRUE);

	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);

	pUsbHid->m_nResult = RESULT_FAIL;

	// save Test Error Description
	va_list args;
	va_start(args, pstrFormat);
	CString str;
	str.FormatV(pstrFormat, args);
	va_end(args);

	if (str.IsEmpty() == FALSE)
	{
		pUsbHid->TestErrDesc_AddDesc(str);
		pUsbHid->TestResultDesc_AddDesc(str);
	}
}

// 사용자가 원하는 문구를 ListCtrl 에 표시하는 함수 - download 진행률 표시
void CTestModeView::DisplayResultToListEx(CUsbHid * pUsbHid, int nResult, LPCTSTR pstrFormat, ...)
{
	if (g_bAppExit || (pUsbHid == NULL) || (pUsbHid->m_nDevIndex < 0))
		return;

	va_list args;
	va_start(args, pstrFormat);
	pUsbHid->m_strResult.FormatV(pstrFormat, args);
	va_end(args);

	int index = pUsbHid->m_nTestStep;

	SemaWait(g_hInfoSemaphore);

	if (index >= (int)m_infoDatabase.size())
	{
		ASSERT(0);
		::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);
		return;
	}

	m_infoDatabase[index].strResult[pUsbHid->m_nDevIndex] = pUsbHid->m_strResult;
	m_infoDatabase[index].nResult[pUsbHid->m_nDevIndex] = nResult;

	m_listInfo.SetItemCount((int)m_infoDatabase.size());
	m_listInfo.EnsureVisible((int)index, TRUE);

	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);

	if (nResult == RESULT_FAIL)
	{
		pUsbHid->m_nResult = RESULT_FAIL;
	}
}

void CTestModeView::DisplayResultByTestStop(CUsbHid * pUsbHid)
{
	if (pUsbHid->m_nTestStep == TEST_CHECK_VERSION)
		Log_AppendEx(pUsbHid, "\r\n");

	for( int i = pUsbHid->m_nTestStep+1; i<m_nAvailableTestStep; i++ )
	{
		pUsbHid->m_nTestStep++;

		Log_Append(pUsbHid, RESULT_NONE);

		if (i == TEST_CHECK_VERSION)
			Log_AppendEx(pUsbHid, "\r\n");

		DisplayResultToList(pUsbHid, RESULT_NONE);

		UpdateProgressBar(pUsbHid);
	}

	pUsbHid->m_nTestStep = m_nAvailableTestStep;
	UpdateProgressBar(pUsbHid);
}

void CTestModeView::ClearTsmTestMessageOnList(int nDeviceIndex)
{
	CRect rect_button;
	CUsbHid * pUsbHid;

	SemaWait(g_hInfoSemaphore);

	if (nDeviceIndex == -1)
	{// ALL Clear
		m_infoDatabase.clear();
	}
	else if (nDeviceIndex != -1)
	{
		int nCount = m_infoDatabase.size();

		pUsbHid = GetUsbHidByDevIndex(nDeviceIndex);
		if (pUsbHid == NULL)	return;

		pUsbHid->m_bInit = TRUE;

		pUsbHid->m_nResult = RESULT_NONE;
		pUsbHid->m_strResult = "";

		for (int i = 1; i < nCount; i++)	// by Paul : i=0 -> i=1
		{
			m_infoDatabase[i].strResult[pUsbHid->m_nDevIndex] = "";
		}
	}

	m_listInfo.SetItemCount((int)m_infoDatabase.size());
	m_listInfo.EnsureVisible((int)m_infoDatabase.size(), FALSE);

	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);
}

int CTestModeView::GetImageListIndex(int bCheckTestEnable)
{
	if( bCheckTestEnable )
		return IMAGE_LIST_ENABLE;
	else
		return IMAGE_LIST_DISABLE;
}

void CTestModeView::DisplayTotalTestStep()
{
	int nListRow = 0;

	m_bIsDefaultConfig = TRUE;
	m_nEnableTestCount = 0;

	ClearTsmTestMessageOnList(-1);

	m_strArrTestInfo.RemoveAll();

	// "01_DETECTED DEVICE"
	AddMessageToList(++nListRow, "DETECTED_DEVICE", IMAGE_LIST_ENABLE);

	// "02_FW DOWNLOAD"
	if (m_bVerifyFirmware)
		AddMessageToList(++nListRow, "VERIFY_FIRMWARE", GetImageListIndex(m_bVerifyFirmware));
	else
		AddMessageToList(++nListRow, "FW_DOWNLOAD", GetImageListIndex(m_bDownload));

	// "03_CHECK_PIN"
	if(m_bCheckIntPin && m_bCheckReset)
		AddMessageToList(++nListRow, "CHECK_PIN(IC_INITIAL) ( INT )( RESET )", IMAGE_LIST_ENABLE);
	else if (m_bCheckIntPin)
		AddMessageToList(++nListRow, "CHECK_PIN(IC_INITIAL) ( INT )", IMAGE_LIST_ENABLE);
	else if (m_bCheckReset)
		AddMessageToList(++nListRow, "CHECK_PIN(IC_INITIAL) ( RESET )", IMAGE_LIST_ENABLE);
	else
		AddMessageToList(++nListRow, "CHECK_PIN(IC_INITIAL) ( INT )( RESET )", IMAGE_LIST_DISABLE);

	// "04_CHECK_VERSION"
	AddMessageToList(++nListRow, "CHECK_VERSION", GetImageListIndex(m_bCheckVersion));

	// "05_DND"
	AddMessageToList(++nListRow, "DND", GetImageListIndex(m_TestInfo_DND.bCheckData));

	// "06_DND_H_GAP_&_V_GAP"
	AddMessageToList(++nListRow, "DND_H_GAP_&_V_GAP", GetImageListIndex(m_TestInfo_DND.bCheckData && m_TestInfo_DND.bCheckDataDiff));

	// "07_HF_DND"
	AddMessageToList(++nListRow, "HF_DND", GetImageListIndex(m_TestInfo_HF_DND.bCheckData));

	// "08_HF_DND_H_GAP_&_V_GAP"
	AddMessageToList(++nListRow, "HF_DND_H_GAP_&_V_GAP", GetImageListIndex(m_TestInfo_HF_DND.bCheckData && m_TestInfo_HF_DND.bCheckDataDiff));

	// "09_SELF_DND"
	AddMessageToList(++nListRow, "SELF_DND", GetImageListIndex(m_TestInfo_SELF_DND.bCheckData));

	// "10_SELF_DND_GAP"
	AddMessageToList(++nListRow, "SELF_DND_GAP", GetImageListIndex(m_TestInfo_SELF_DND.bCheckData && m_TestInfo_SELF_DND.bCheckDataDiff));

	// "11_SELF_SATURATION"
	AddMessageToList(++nListRow, "SELF_SATURATION", GetImageListIndex(m_TestInfo_SELF_SATURATION.bCheckData));

	// "12_TRX_SHORT"
	AddMessageToList(++nListRow, "TRX_SHORT", GetImageListIndex(m_bCheckTrxShort));

	// "13_CURRENT_COMSUMPTION"
	AddMessageToList(++nListRow, "CURRENT_CONSUMPTION", GetImageListIndex(m_TestInfo_CURRENT_CONSUMPTION.Active.bCheckData == TRUE && m_TestInfo_CURRENT_CONSUMPTION.bEnable == TRUE));

	// "14_JITTER"
	AddMessageToList(++nListRow, "JITTER", GetImageListIndex(m_TestInfo_JITTER.bCheckData));

	// "15_HW_CALIBRATION"
	AddMessageToList(++nListRow, "HW_CALIBRATION", GetImageListIndex(m_TestInfo_HW_CALIBRATION.bCheckData));

	// "16_MEASURE DELTA"
	AddMessageToList(++nListRow, "MEASURE_DELTA", GetImageListIndex(m_TestInfo_MEASURE_DELTA.bCheckData));

	// "17_DIGITAL_WHEEL"
	AddMessageToList(++nListRow, "DIGITAL_WHEEL", GetImageListIndex(m_TestInfo_DIGITAL_WHEEL.bCheckData));

#if defined(D_USE_CHECK_BUTTON_TEST)
	// "18_GPIO_BUTTON"
	AddMessageToList(++nListRow, "GPIO_BUTTON", GetImageListIndex(m_bCheckGpioButton));
	// "19_TOUCH_BUTTON"
	AddMessageToList(++nListRow, "TOUCH_BUTTON", GetImageListIndex(m_bCheckTouchButton));
#endif

	// "20_NOISE"
	AddMessageToList(++nListRow, "NOISE", GetImageListIndex(m_TestInfo_NOISE.m_bCheckNoise));

	// "21_ONLY_SELF_RAW_DATA
	AddMessageToList(++nListRow, "ONLY_SELF_RAW_DATA", GetImageListIndex(m_TestInfo_ONLYSELF_DND.bCheckData));

	// "22_HIGH_VOLTAGE
	AddMessageToList(++nListRow, "HIGH_VOLTAGE", GetImageListIndex(m_TestInfo_HIGH_VOLTAGE.bCheckData));

	// "23_SELF_CND" for ZTM730 CND
	AddMessageToList(++nListRow, "CND", GetImageListIndex(m_TestInfo_CND.bCheckData));

	// "24_VIRTUAL_DRAW_TEST"
	AddMessageToList(++nListRow, "VIRTUAL_DRAW_TEST", GetImageListIndex(m_bCheckVirtualDrawTest));

	// "25_VIH_TEST"
	AddMessageToList(++nListRow, "I2C_TEST", GetImageListIndex(m_bCheckI2C_Level));

	m_nAvailableTestStep = nListRow;

	// calculate maximum number of test
	int nMaxCharCount = 0;
	int nListCount = m_infoDatabase.size();
	int nCharCount;
	for (int i = 0; i < nListCount; i++)
	{
		nCharCount = m_infoDatabase[i].m_info.GetLength();
		if (nCharCount > nMaxCharCount)
			nMaxCharCount = nCharCount;
	}

	m_nLogLimitNumOfChar_0 = nMaxCharCount + strlen("ERROR_");

	// check whether default config is using or not
	if (m_bCheckVersion || m_TestInfo_DND.bCheckData || m_TestInfo_DND.bCheckDataDiff ||
		m_TestInfo_HF_DND.bCheckData || m_TestInfo_HF_DND.bCheckDataDiff ||
		m_TestInfo_SELF_DND.bCheckData || m_TestInfo_SELF_DND.bCheckDataDiff ||
		m_TestInfo_SELF_SATURATION.bCheckData || m_bCheckTrxShort || m_TestInfo_MEASURE_DELTA.bCheckData ||
		m_TestInfo_HW_CALIBRATION.bCheckData || m_TestInfo_JITTER.bCheckData || m_TestInfo_DIGITAL_WHEEL.bCheckData ||
		(m_TestInfo_CURRENT_CONSUMPTION.Active.bCheckData == TRUE && m_TestInfo_CURRENT_CONSUMPTION.bEnable == TRUE)
#if defined(D_USE_CHECK_BUTTON_TEST)
		|| m_bCheckGpioButton || m_bCheckTouchButton
#endif
		|| m_TestInfo_NOISE.m_bCheckNoise
		|| m_TestInfo_ONLYSELF_DND.bCheckData
		|| m_TestInfo_HIGH_VOLTAGE.bCheckData
		|| m_TestInfo_CND.bCheckData
		|| m_bCheckVirtualDrawTest
		|| m_bCheckI2C_Level
		)
	{
		m_bIsDefaultConfig = FALSE;
	}

	int nDeviceCount = m_pUsbHidList->GetCount();

	for (int n = 0; n < nDeviceCount; n++)
	{
		CUsbHid * pUsbHid = GetUsbHidByIndex(n);
		if (pUsbHid == NULL || pUsbHid->IsTestAvailable() == FALSE)
			continue;

		pUsbHid->m_nTestStep = 0;

		if (pUsbHid->m_fVoltageVcc != 0 && pUsbHid->m_fVoltageVi2c != 0)
			DisplayResultToListEx(pUsbHid, RESULT_PASS, "VOLTAGE%.2fV / %.2fV / %.2fV", pUsbHid->m_fVoltageVcc, pUsbHid->m_fVoltageVi2c, pUsbHid->m_fVIH_1);
		else
			DisplayResultToListEx(pUsbHid, RESULT_PASS, "VER_%02d", pUsbHid->m_nMpToolVersion);
	}
}

int CTestModeView::GetBarcodeLength()
{
	int nBarcodeLength = m_nBarcode_Length;		// Option > BARCODE LENGTH

	if (m_bFixedBarcodeLength == FALSE && m_bAutoStartTest == FALSE)
	{
		nBarcodeLength = m_nBarcodeChIdx;		// received barcode length
	}

#if 0
	if (m_nBarcodeChIdx > m_nBarcode_Length)
	{
		nBarcodeLength = 0;
	}
#endif

	return nBarcodeLength;
}

BOOL CTestModeView::PreTranslateMessage(MSG* pMsg)
{
	if (m_bAleadyTestEnter == TRUE)
	{
		return CFormView::PreTranslateMessage(pMsg);
	}

	if (pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
			case VK_F1:
			{
				if (m_bEnableStartButton)
				{
					PostMessageEx(WM_USER_EXECUTE_ASIC_ID_MODE, 0, 0);
				}
			}
			break;

#ifdef D_DEBUG_IUM_ACCESS
			case VK_F2:
				PostMessageEx(WM_USER_MY_TEST_IUM_DEBUGGING_EVENT, 0, 0);
			break;
			case VK_F3:
				PostMessageEx(WM_USER_MY_TEST_IUM_DEBUGGING_EVENT, 1, 0);
			break;
#endif

			#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
			case VK_F5:
			{
				m_pConnectInfoDlg->ShowMsgBox(SW_SHOW);
			}
			break;
			#endif

			case VK_F8:
			{
				PostMessageEx(WM_USER_BARCODE_RUN_DIALOG, 0, 0);
			}
			break;

			case VK_F12:
			{
				PostMessageEx(WM_USER_EXECUTE_GRAPH_MODE, 0, 0);
			}
			break;

			default:
			break;
		}
	}

	ProcessKeyToBarcode(pMsg);

	return CFormView::PreTranslateMessage(pMsg);
}

void CTestModeView::ProcessKeyToBarcode(MSG* pMsg)
{
	if(m_pBarcodeSettingDlg
	&& m_pBarcodeSettingDlg->IsShowDialog()
	&&(pMsg->message == WM_CHAR || pMsg->message == WM_IME_CHAR))
	{
		m_pBarcodeSettingDlg->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam );
		return;
	}

	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_SPACE)
		{
			UINT nFlags = (UINT)((pMsg->lParam)>>16);

			// previous key statue
			if (nFlags & 0x8000)
			{// 눌려진 상태
				return;
			}

			// 한번 눌림 처리
			if (m_bUseBarcode == TRUE && m_bAutoStartTest == TRUE)
			{//Barcode Mode & Auto Start
				CString strMsg;
				strMsg.Format("Barcode Mode&Auto Start Mode Enable\nSpaceBar does not operate.");
				AfxMessageBox(strMsg);
			}
			else if (m_bUseBarcode == TRUE && m_bAutoStartTest == FALSE)
			{
				TRACE("START TSM TEST BY SPECE BAR????????????????????\n");

				if (m_nBarcodeChIdx == GetBarcodeLength())
				{
					// start TMS Test
					OnBnClickedButtonStartTestAll();
				}
				else
				{
					m_nBarcodeChIdx = 0;
					m_nBarcodeIdx = 0;
				}
			}
			else if (m_bUseBarcode == FALSE)
			{
				TRACE("START TSM TEST BY SPECE BAR\n");

				// start TMS Test
				OnBnClickedButtonStartTestAll();

				memset(m_cBarcode, '\0', 1000);
			}

			return;
		}
		else if (pMsg->wParam == VK_RETURN)		// Barcode Sacn의 마지막 Key 입력 종류(CR)
		{
			GetDlgItem(IDC_LIST_INFO)->SetFocus();

			TSM_StartTest_ByBarcode();
			return;
		}
	}
	else if (pMsg->message == WM_KEYUP)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_SPACE)
			return;
	}

	if( pMsg->message == WM_CHAR || pMsg->message == WM_IME_CHAR )
	{
		if (pMsg->wParam == VK_RETURN)	// ignore ENTER key
		{
			m_nBarcodeIdx = 0;
			m_nBarcodeChIdx = 0;

			return;
		}
	}
	else
	{
		return;
	}


	//============================================================================================================
	//	USE Barcode Code Added
	//	Key 입력이 들어온 후 마지막 Enter Key가 들어오면 자리수를 확인한 후 Test 시작 - OLD
	//	Enter Key 가 들어오지 않는 바코드기 대응을 위해서 Key 입력수와 Length가 동일한 경우 Test 시작 - NEW. By HIVE 2019/01/09
	//===========================================================================================================
	if (m_bUseBarcode == TRUE) 	   //Use Barcode 가 체크 되어 있을 때
	{
		if (GetTimeInMillis() - m_BarcodeInputTime > BARCODE_VALID_TIME)
		{// 일정 시간 경과 후 바코드 인식 시 positon 을 초기화한다.
			m_nBarcodeIdx = 0;
			m_nBarcodeChIdx = 0;
		}

		if (pMsg->message == WM_CHAR)
		{
			//if (IsCharAlphaNumeric(pMsg->wParam) == FALSE)
			//	return;

			if (pMsg->wParam == VK_TAB ||
				pMsg->wParam == VK_BACK ||
				pMsg->wParam == VK_SPACE)
			{
				return;
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
		}

		if(m_bBarcodeDisplayInTest)
		{
			if(m_nBarcodeChIdx == 0)
			{
				((GetDlgItem(IDC_STATIC_BARCODE)))->SetWindowText("-");
			}
			else
			{
				m_cBarcode[m_nBarcodeIdx] = '\0';

				CString strBarcode;
				strBarcode.Format("%s", m_cBarcode);
				((GetDlgItem(IDC_STATIC_BARCODE)))->SetWindowText((LPCTSTR)strBarcode);
			}

			EnableStartButton(TRUE);
		}

		if(m_bFixedBarcodeLength && m_nBarcode_AutoReset > 0)
		{
			KillTimer(TIMER_BARCODE_AUTO_RESET);

			if(m_nBarcodeChIdx != GetBarcodeLength())
				SetTimer(TIMER_BARCODE_AUTO_RESET, m_nBarcode_AutoReset * 1000, NULL);
		}

		m_BarcodeInputTime = GetTimeInMillis();

		if (m_bFixedBarcodeLength && (m_nBarcodeChIdx == m_nBarcode_Length))		//Key 입력수와 Length가 동일한 경우
		{
			TSM_StartTest_ByBarcode();
		}
	}
}

void CTestModeView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_CHECK_HID_DEVICE)
	{
		UsbHidDevice_KillSurveyTimer();

		if (m_bAleadyTestEnter != FALSE)
		{
			UsbHidDevice_StartSurveyTimer();
			return CFormView::OnTimer(nIDEvent);
		}

		if (SemaWaitImmediate(g_hSemaphore) == FALSE)
		{
			UsbHidDevice_StartSurveyTimer(TRUE);
			return CFormView::OnTimer(nIDEvent);
		}

		if (m_nNeedResearch > 0)
		{
			m_nNeedResearch--;

			if (FindUsbHid() == FALSE)
			{
				m_nNeedResearch = 0;
			}

			UsbHidDevice_StartSurveyTimer((m_nNeedResearch > 0) ? TRUE : FALSE);

			::ReleaseSemaphore(g_hSemaphore, 1, NULL);
			CFormView::OnTimer(nIDEvent);
			return;
		}
		else
		{
			if ((m_nCheckHidDeviceCount % 3) == 2)
			{
				FindUsbHid();
			}
		}

		::ReleaseSemaphore(g_hSemaphore, 1, NULL);

		if ( TSM_StartTest_ByStartPin() == FALSE )
		{
			UsbHidDevice_StartSurveyTimer();
		}
	}
	else if (nIDEvent == TIMER_GOTO_STANDBY)
	{
		KillTimer(TIMER_GOTO_STANDBY);

		GotoStandBy();
	}
	else if (nIDEvent == TIMER_BARCODE_AUTO_RESET)
	{
		KillTimer(TIMER_BARCODE_AUTO_RESET);

		m_nBarcodeChIdx = 0;
		m_nBarcodeIdx = 0;

		memset(m_cBarcode, 0x00, sizeof(m_cBarcode));

		BarcodeDisplayInTestVisible();
	}
	else if (nIDEvent == TIMER_DEBUG_TEST_AGAIN)
	{
		KillTimer(TIMER_DEBUG_TEST_AGAIN);

		TSM_StartTest(m_nStartTestIndex);
	}
	else if (nIDEvent == TIMER_SOLUTION_RESULT_NOTIFY)
	{
		KillTimer(TIMER_SOLUTION_RESULT_NOTIFY);

		ExitSolutionResetNotify();
	}

	#if defined(D_DEBUG_FW_UPGRADE_TEST)
	else if (nIDEvent == TIMER_DEBUG_FW_UPGRADE_TEST)
	{
		KillTimer(TIMER_DEBUG_FW_UPGRADE_TEST);

		OnBnClickedButtonStartTestAll();
	}
	#endif

	CFormView::OnTimer(nIDEvent);
}

void CTestModeView::UpdateProgressBar(CUsbHid * pCurrUsbHid)
{
	if (g_bAppExit)
		return;

	if (pCurrUsbHid == NULL)
	{
		TestResultDesc_AddTestStep(0, 0);
		return;
	}

	int nTestStep = pCurrUsbHid->m_nTestStep;

	SemaWait(g_hInfoSemaphore);

	if(INDIVIDUAL_TEST_START_ON)
	{// Individual display
		TestResultDesc_AddResultEx(pCurrUsbHid, nTestStep);

		TestResultDesc_AddTestStep(pCurrUsbHid->m_nDevIndex, nTestStep);	// display next test step
	}
	else
	{// Integrated display
		int nLeastTestStep = m_nAvailableTestStep;

		// calculate the least test step value
		for (int n = 0; n < m_nMaxDeviceIndex; n++)
		{
			CUsbHid * pUsbHid = GetUsbHidByDevIndex(n);

			if (pUsbHid && pUsbHid->IsTestAvailable() && pUsbHid->m_bTesting)
			{
				if (pUsbHid->m_nTestStep < nLeastTestStep)
					nLeastTestStep = pUsbHid->m_nTestStep;
			}
		}

		// display result description if the least test step value is changed
		if (m_nTestStepMin != nLeastTestStep)
		{
			m_nTestStepMin = nLeastTestStep;

			TestResultDesc_AddResult(nLeastTestStep);

			TestResultDesc_AddTestStep(0, nLeastTestStep);	// display next test step
		}
	}

	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);
}

void CTestModeView::UpdateStatusBar(CString * strText)
{
	if (g_bAppExit || (m_bCreatePane == FALSE))
		return;

	::SendMessage(m_parentHWnd, WM_USER_PROGRESS_UPDATE, (WPARAM)-1, (LPARAM)strText);
}

//==============================================================================================
// Option & TEST_CONF.INI

void CTestModeView::Login()
{
	if (m_bIsLogin){
		return;
	}

	CInputDlg dlg;
	dlg.m_strTitle.Format("PASSWORD");
	dlg.m_bPassword = TRUE;

	if (dlg.DoModal() != IDOK)		return;

	if (dlg.m_strInput.IsEmpty())	return;

	if (dlg.m_strInput.CompareNoCase("MPZTD1231") == 0) {
		gCapInfo.authorityDepth = ROOT_AUTHORITY;
	}
	else if (dlg.m_strInput.CompareNoCase(m_strUserAdminPassword) == 0) {
		gCapInfo.authorityDepth = ROOT_AUTHORITY;
	}
	//else if (dlg.m_strInput.CompareNoCase("OPTION1") == 0) {
	//	g_bRootTestMode = TRUE;
	//}
	else {
		AfxMessageBox("[ERROR] WRONG PASSWORD");
		gCapInfo.authorityDepth = NORMAL_AUTHORITY;
		return;
	}

	m_bIsLogin = TRUE;
}

#if 0
BOOL CTestModeView::ReadAllRegister(CUsbHid * pUsbHid)
{
	BOOL bRet = TRUE;
	CString str;

	TRACE("READ ALL REGISTER MODE\n");

	SemaWait(g_hSemaphore);

	MpTool_PowerControl(pUsbHid, TRUE);
	Sleep(10);

	str.Format("UPLOADING ALL REGISTER");
	UpdateStatusBar(&str);

	for (int i = 0; i < gCapInfo.totalRegisterCount; i++)
	{
		if (gRegNameInfo[i].bValid)
		{
			if (pUsbHid->GetTouchConfig(i, (BYTE *)&pUsbHid->m_CapInfo.icRegData[i]) != 0)
			{
				str.Format("[TOUCH DEVICE ERROR] CANNOT READ REGISTER (0x%04X)", i);
				UpdateStatusBar(&str);

				bRet = FALSE;
				goto EXIT_READ_REG_FUNC;
			}
		}
	}

	if (pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] <= 0 || pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] >= DEF_MAX_X_LINE_NUM ||
		pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] <= 0 || pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] >= DEF_MAX_Y_LINE_NUM ||
		pUsbHid->m_CapInfo.icRegData[REG_X_RESOLUTION] < 0 || pUsbHid->m_CapInfo.icRegData[REG_Y_RESOLUTION] < 0)
	{
		str.Format("[TOUCH DEVICE ERROR] TOUCH DEVICE READ REGISTER : INVALID VALUE");
		UpdateStatusBar(&str);

		bRet = FALSE;
		goto EXIT_READ_REG_FUNC;
	}

	str.Format("SUCCESS UPLOADING ALL REGISTER");
	UpdateStatusBar(&str);

EXIT_READ_REG_FUNC:

	MpTool_PowerControl(pUsbHid, FALSE);

	::ReleaseSemaphore(g_hSemaphore, 1, NULL);
	return bRet;
}
#endif

BOOL CTestModeView::ReadTotalLineNumber(s16 * pTotalNumX, s16 * pTotalNumY, s16 * pSelfNumX, s16 * pSelfNumY, u8 * pMaxLineX, u8 * pMaxLineY)
{
	BOOL bRet = FALSE;
	s16 nTotalNumX = DEF_MAX_X_LINE_NUM;
	s16 nTotalNumY = DEF_MAX_Y_LINE_NUM;
	s16 nSelfNumX = DEF_MAX_X_LINE_NUM;
	s16 nSelfNumY = DEF_MAX_Y_LINE_NUM;
	s16 nSupprtBntNum = 0;
	s16 nBntLineType = 0;

	SemaWait(g_hSemaphore);

	for (int n = 0; n < m_nMaxDeviceIndex; n++)
	{
		CUsbHid * pUsbHid = GetUsbHidByDevIndex(n);

		if (pUsbHid == NULL || pUsbHid->IsTestAvailable() == FALSE)
			continue;

		if (MpTool_PowerControl(pUsbHid, TRUE) != Z_HID_OK)
		{
			continue;
		}
		Sleep(10);

		for (int i = 0; i < 5; i++)
		{
			if (pUsbHid->GetTouchConfigEx(REG_TOTAL_NUMBER_OF_X, (BYTE *)&nTotalNumX) != 0)			continue;
			if (pUsbHid->GetTouchConfigEx(REG_TOTAL_NUMBER_OF_Y, (BYTE *)&nTotalNumY) != 0)			continue;
			if (pUsbHid->GetTouchConfigEx(REG_CUSTOM_BUTTON_CHANNEL, (BYTE *)&nSelfNumX) != 0)			continue;
			if (pUsbHid->GetTouchConfigEx(REG_CUSTOM_BUTTON_Y_CHANNEL, (BYTE *)&nSelfNumY) != 0)		continue;
			if (pUsbHid->GetTouchConfigEx(REG_BUTTON_SUPPORTED_NUM, (BYTE *)&nSupprtBntNum) != 0)		continue;
			if (pUsbHid->GetTouchConfigEx(REG_BUTTON_LINE_TYPE, (BYTE *)&nBntLineType) != 0)			continue;

			bRet = TRUE;
			break;
		}

#if defined(D_USE_FW_CHINA_FRAMEWORK)
		// Set maximum X, Y line number to self-data (using CHINA)
		SetMaxLineNumber(pUsbHid, pMaxLineX, pMaxLineY);
#endif

		MpTool_PowerControl(pUsbHid, FALSE);

		break;
	}

	if (bRet && (nTotalNumX <= 0 || nTotalNumX >= DEF_MAX_X_LINE_NUM || nTotalNumY <= 0 || nTotalNumY >= DEF_MAX_Y_LINE_NUM
			  || nSelfNumX <= 0 || nSelfNumX >= DEF_MAX_X_LINE_NUM || nSelfNumY <= 0 || nSelfNumY >= DEF_MAX_Y_LINE_NUM) )
	{
		CString str;
		str.Format("[TOUCH DEVICE ERROR] TOUCH DEVICE READ REGISTER : INVALID VALUE");
		UpdateStatusBar(&str);

		*pTotalNumX = *pSelfNumX = DEF_MAX_X_LINE_NUM;
		*pTotalNumY = *pSelfNumY = DEF_MAX_Y_LINE_NUM;

		bRet = FALSE;
		goto EXIT_READ_REG_FUNC;
	}

	*pTotalNumX = nTotalNumX;
	*pTotalNumY = nTotalNumY;
	*pSelfNumX = nSelfNumX + nSupprtBntNum * (nBntLineType == 1 ? 1 : 0);
	*pSelfNumY = nSelfNumY;

EXIT_READ_REG_FUNC:

	::ReleaseSemaphore(g_hSemaphore, 1, NULL);
	return bRet;
}

void CTestModeView::SetMaxLineNumber(CUsbHid * pUsbHid, u8* pLineX, u8 * pLineY)
{
	u8 nMaxLineNum_X, nMaxLineNum_Y;

	if (pUsbHid->m_CapInfo.icSignature == 0xE548) // 7548
	{
		nMaxLineNum_X = 30;
		nMaxLineNum_Y = 18;
	}
	else if (pUsbHid->m_CapInfo.icSignature == 0xE538) // 7538
	{
		nMaxLineNum_X = 24;
		nMaxLineNum_Y = 14;
	}
	else if (pUsbHid->m_CapInfo.icSignature == 0xE532) // 7532s, 522
	{
		nMaxLineNum_X = 20;
		nMaxLineNum_Y = 12;
	}
	else if (pUsbHid->m_CapInfo.icSignature == 0xE628)
	{
		nMaxLineNum_X = 18;
		nMaxLineNum_Y = 10;
	}
	else if (pUsbHid->m_CapInfo.icSignature == 0xE622
	|| pUsbHid->m_CapInfo.icSignature == 0xE630)
	{
		nMaxLineNum_X = 8;
		nMaxLineNum_Y = 8;
	}
	else if (pUsbHid->m_CapInfo.icSignature == 0xE650
	|| pUsbHid->m_CapInfo.icSignature == 0x650E)
	{
		nMaxLineNum_X = 34;
		nMaxLineNum_Y = 16;
	}
	else if (pUsbHid->m_CapInfo.icSignature == 0xE623)
	{
		nMaxLineNum_X = 10;
		nMaxLineNum_Y = 8;
	}
	else if (pUsbHid->m_CapInfo.icSignature == 0xE730)
	{
		nMaxLineNum_X = 10;
		nMaxLineNum_Y = 10;
	}
	else if (pUsbHid->m_CapInfo.icSignature == 0xE760)
	{
		nMaxLineNum_X = 36;
		nMaxLineNum_Y = 24;
	}
	else if (pUsbHid->m_CapInfo.icSignature == 0xE137)
	{
		nMaxLineNum_X = 18;
		nMaxLineNum_Y = 25;
	}
	else if (pUsbHid->m_CapInfo.icSignature == 0xE722)
	{
		nMaxLineNum_X = 10;
		nMaxLineNum_Y = 10;
	}
	else 
	{
		nMaxLineNum_X = 18;
		nMaxLineNum_Y = 10;
	}

	*pLineX = nMaxLineNum_X;
	*pLineY = nMaxLineNum_Y;
}

void CTestModeView::OnBnClickedButtonOption()
{
	BOOL bAdditionalLOG_prev = m_bAdditionalLOG;
	int nSlaveAddress = g_nSlaveAddress;
	int nMaxDeviceCount = GetMaxDeviceCount();
	int nMptoolCount = m_nMptoolCount;
	BOOL bUseMultiDevice = m_bUseMultiDevice;
	BOOL bUseStartIndividualTest = m_bUseStartIndividualTest;
	BOOL bUseMptoolver =m_bUseMptoolver;

	KillAllTimer();

	if (m_bIsLogin == FALSE)
	{
		Login();
	}

	if (m_bIsLogin == FALSE || gCapInfo.authorityDepth < USER_AUTHORITY)
	{
		UsbHidDevice_StartSurveyTimer();
		return;
	}

	// reset barcode info
	m_nBarcodeChIdx = 0;
	m_nBarcodeIdx = 0;

	memset(m_cBarcode, 0x00, sizeof(m_cBarcode));

	// create Option Dialog
	CTestOptionDlg dlg(this);

	// copy total number of line from first device
	ReadTotalLineNumber(&dlg.m_icRegData_TotalNumOfX, &dlg.m_icRegData_TotalNumOfY, &dlg.m_icRegData_SelfNumOfX, &dlg.m_icRegData_SelfNumOfY, &dlg.m_IC_Max_X_Line_Num, &dlg.m_IC_Max_Y_Line_Num);

	//===============================================
	// [DEFAULT]
	dlg.m_bUseIFE = m_bUseIFE;
	dlg.m_bUseRetry = m_bUseRetry;

	dlg.m_bUseMptoolver = m_bUseMptoolver;

	dlg.m_nAllRetryCount = m_nAllRetryCount;
	dlg.m_nMptoolCount = 	m_nMptoolCount;

	dlg.m_strCustomer = m_strCustomer;
	dlg.m_strProjectName = m_strProjectName;

	dlg.m_nStandBySec = m_nStandBySec;

	dlg.m_bAdditionalLOG = m_bAdditionalLOG;
	dlg.m_strTestName = m_strTestName;
	dlg.m_strTestStage = m_strTestStage;
	dlg.m_strModuleMaker = m_strModuleMaker;
	dlg.m_nInOutType = m_nInOutType;

	dlg.m_nIcType = g_nIcType;
	dlg.m_nSlaveAddress = g_nSlaveAddress;
	dlg.m_bSaveResult = m_bSaveResult;
	dlg.m_bSaveFixedPosition = m_bSaveFixedPosition;
	dlg.m_bCreatePassFailDirectory = m_bCreatePassFailDirectory;
	dlg.m_strSaveResultDir = m_strSaveResultDir;

	//===============================================
	// [BARCODE]
	dlg.m_bUseBarcode = m_bUseBarcode;
	dlg.m_bBarcodeDisplayInTest = m_bBarcodeDisplayInTest;
	dlg.m_bAutoStartTest = m_bAutoStartTest;
	dlg.m_bFixedBarcodeLength = m_bFixedBarcodeLength;
	dlg.m_nBarcode_Length = m_nBarcode_Length;
	dlg.m_nBarcode_AutoReset = m_nBarcode_AutoReset;

	//===============================================
	// [FW DOWNLOAD]
	dlg.m_bDownload = m_bDownload;
	dlg.m_bDownloadFixedPosition = m_bDownloadFixedPosition;
	dlg.m_strFirmwareDir = m_strFirmwareDir;
	dlg.m_nDownloadRetryCount = m_nDownloadRetryCount;

	dlg.m_bVerifyFirmware = m_bVerifyFirmware;
	dlg.m_bBurstUpgrade = m_bBurstUpgrade;

	dlg.m_bSaveInfo = m_bSaveInfo;
	dlg.m_nSaveInfoLocation = m_nSaveInfoLocation;

	dlg.m_strProjectID = m_strProjectID;
	dlg.m_strDeviceID = m_strDeviceID;

	dlg.m_nIUMFormat = m_nIUMFormat;
	dlg.m_nI2CFormat = m_nI2CFormat;

	//===============================================
	// [CHECK INT]
	dlg.m_bCheckIntPin = m_bCheckIntPin;
	dlg.m_bCheckReset = m_bCheckReset;
	dlg.m_bCheckResetPin = m_bCheckResetPin;

	//===============================================
	// [CHECK VERSION]
	dlg.m_bCheckVersion = m_bCheckVersion;
	dlg.m_bCheckAutoFwDownload = m_bCheckAutoFwDownload;
	dlg.m_strIcRevision = m_strIcRevision;
	dlg.m_strHwID = m_strHwID;
	dlg.m_strModuleID = m_strModuleID;
	dlg.m_strMajorVersion = m_strMajorVersion;
	dlg.m_strMinorVersion = m_strMinorVersion;
	dlg.m_strRegisterVersion = m_strRegisterVersion;

	dlg.m_bCheckIUM = m_bCheckIUM;
	//===============================================
	dlg.m_bCheckI2C_Level = m_bCheckI2C_Level;
	//===============================================
	dlg.m_strUserAdminPassword = m_strUserAdminPassword;
	dlg.m_bUseStartup = m_bUseStartup;
	dlg.m_strStartupPassword = m_strStartupPassword;

	//===============================================
	// [TEST INFO]
	memcpy(&dlg.m_TestInfo_DND, &m_TestInfo_DND, sizeof(TEST_INFO_DND));
	memcpy(&dlg.m_TestInfo_HF_DND, &m_TestInfo_HF_DND, sizeof(TEST_INFO_DND));

	memcpy(&dlg.m_TestInfo_CND, &m_TestInfo_CND, sizeof(TEST_INFO_CND));	//add 2022.11.14 for ZTM730 CND

	memcpy(&dlg.m_TestInfo_SELF_DND, &m_TestInfo_SELF_DND, sizeof(TEST_INFO_SELF));
	memcpy(&dlg.m_TestInfo_SELF_SATURATION, &m_TestInfo_SELF_SATURATION, sizeof(TEST_INFO_SELF));

	memcpy(&dlg.m_TestInfo_MEASURE_DELTA, &m_TestInfo_MEASURE_DELTA, sizeof(TEST_INFO_MEASURE_DELTA));
	memcpy(&dlg.m_TestInfo_HW_CALIBRATION, &m_TestInfo_HW_CALIBRATION, sizeof(TEST_INFO_HW_CALIBRATION));
	memcpy(&dlg.m_TestInfo_JITTER, &m_TestInfo_JITTER, sizeof(TEST_INFO_JITTER));
	memcpy(&dlg.m_TestInfo_CURRENT_CONSUMPTION, &m_TestInfo_CURRENT_CONSUMPTION, sizeof(TEST_INFO_CURRENT_CONSUMPTION));
	memcpy(&dlg.m_TestInfo_DIGITAL_WHEEL, &m_TestInfo_DIGITAL_WHEEL, sizeof(TEST_INFO_DIGITAL_WHEEL));
	memcpy(&dlg.m_TestInfo_NOISE, &m_TestInfo_NOISE, sizeof(TEST_INFO_NOISE));

	memcpy(&dlg.m_TestInfo_ONLYSELF_DND, &m_TestInfo_ONLYSELF_DND, sizeof(TEST_INFO_ONLYSELF));

	memcpy(&dlg.m_TestInfo_HIGH_VOLTAGE, &m_TestInfo_HIGH_VOLTAGE, sizeof(TEST_INFO_HIGH_VOLTAGE));



	dlg.m_TestInfo_NOISE.m_nCheckNoise_ReadTimes = dlg.m_TestInfo_NOISE.m_nCheckNoise_ReadTime_Org;

	dlg.m_bCheckTrxShort = m_bCheckTrxShort;

	//===============================================
	// [ADD TEST ITEM]
	dlg.m_bUseMultiDevice = m_bUseMultiDevice;
	dlg.m_nMultiDeviceCount = m_nMultiDeviceCount;
	dlg.m_bUseStartIndividualTest = m_bUseStartIndividualTest;

	dlg.m_bCheckUserNumber = m_bCheckUserNumber;

	dlg.m_bHidMode = m_bHidMode;

#if defined(D_USE_CHECK_BUTTON_TEST)
	dlg.m_bCheckGpioButton = m_bCheckGpioButton;

	dlg.m_bCheckTouchButton = m_bCheckTouchButton;
	dlg.m_nTouchButton_Threshold = m_nTouchButton_Threshold;
#endif

	dlg.m_bCheckVirtualDrawTest = m_bCheckVirtualDrawTest;

	//==============================================================================================
	// Display Option Dialog
	//==============================================================================================
	INT_PTR nRet = dlg.DoModal();

	if(m_bResetCount)
	{
		CString str;
		str.Format("%d", m_Pass_Count);
		GetDlgItem(IDC_STATIC_PASS_COUNT)->SetWindowTextA(str);
		str.Format("%d", m_Fail_Count);
		GetDlgItem(IDC_STATIC_FAIL_COUNT)->SetWindowTextA(str);
		str.Format("%d", m_Pass_Count + m_Fail_Count);
		GetDlgItem(IDC_STATIC_TOTAL_COUNT)->SetWindowTextA(str);

		m_bResetCount = FALSE;
	}

	if (nRet != IDOK)
	{
		UsbHidDevice_StartSurveyTimer();
		return;
	}

	//===============================================
	// [DEFAULT]
	m_bUseIFE = dlg.m_bUseIFE;
	m_bUseRetry = dlg.m_bUseRetry;
	m_bUseMptoolver = dlg.m_bUseMptoolver;

	m_nAllRetryCount = dlg.m_nAllRetryCount;
	m_nMptoolCount = dlg.m_nMptoolCount;

	m_strCustomer = dlg.m_strCustomer;
	m_strProjectName = dlg.m_strProjectName;

	m_nStandBySec = dlg.m_nStandBySec;

	m_bAdditionalLOG = dlg.m_bAdditionalLOG;
	m_strTestName = dlg.m_strTestName;
	m_strTestStage = dlg.m_strTestStage;
	m_strModuleMaker = dlg.m_strModuleMaker;
	m_nInOutType = dlg.m_nInOutType;

	g_nIcType = dlg.m_nIcType;
	g_nSlaveAddress = dlg.m_nSlaveAddress;
	m_bSaveResult = dlg.m_bSaveResult;
	m_bSaveFixedPosition = dlg.m_bSaveFixedPosition;
	m_bCreatePassFailDirectory = dlg.m_bCreatePassFailDirectory;
	m_strSaveResultDir = dlg.m_strSaveResultDir;

	//===============================================
	// [BARCODE]
	m_bUseBarcode = dlg.m_bUseBarcode;
	m_bBarcodeDisplayInTest = dlg.m_bBarcodeDisplayInTest;
	m_bAutoStartTest = dlg.m_bAutoStartTest;
	m_bFixedBarcodeLength = dlg.m_bFixedBarcodeLength;
	m_nBarcode_Length = dlg.m_nBarcode_Length;
	m_nBarcode_AutoReset = dlg.m_nBarcode_AutoReset;
	//===============================================
	// [FW DOWNLOAD]
	m_bDownload = dlg.m_bDownload;
	m_bDownloadFixedPosition = dlg.m_bDownloadFixedPosition;
	m_strFirmwareDir = dlg.m_strFirmwareDir;
	m_nDownloadRetryCount = dlg.m_nDownloadRetryCount;
	m_bVerifyFirmware = dlg.m_bVerifyFirmware;
	m_bBurstUpgrade = dlg.m_bBurstUpgrade;

	m_bSaveInfo = dlg.m_bSaveInfo;
	m_nSaveInfoLocation = dlg.m_nSaveInfoLocation;

	m_strProjectID = dlg.m_strProjectID;
	m_strDeviceID = dlg.m_strDeviceID;

	m_nIUMFormat = dlg.m_nIUMFormat;
	m_nI2CFormat = dlg. m_nI2CFormat;

	//===============================================
	// [CHECK INT]
	m_bCheckIntPin = dlg.m_bCheckIntPin;
	m_bCheckReset = dlg.m_bCheckReset;
	m_bCheckResetPin = dlg.m_bCheckResetPin;

	//===============================================
	// [CHECK VERSION]
	m_bCheckVersion = dlg.m_bCheckVersion;
	m_bCheckAutoFwDownload = dlg.m_bCheckAutoFwDownload;
	m_strIcRevision = dlg.m_strIcRevision;
	m_strHwID = dlg.m_strHwID;
	m_strModuleID = dlg.m_strModuleID;
	m_strMajorVersion = dlg.m_strMajorVersion;
	m_strMinorVersion = dlg.m_strMinorVersion;
	m_strRegisterVersion = dlg.m_strRegisterVersion;

	m_bCheckIUM = dlg.m_bCheckIUM;

	//===============================================
	m_bCheckI2C_Level = dlg.m_bCheckI2C_Level;
	//===============================================
	m_strUserAdminPassword = dlg.m_strUserAdminPassword;
	m_bUseStartup = dlg.m_bUseStartup;
	m_strStartupPassword = dlg.m_strStartupPassword;

	//===============================================
	// [TEST INFO]
	memcpy(&m_TestInfo_DND, &dlg.m_TestInfo_DND, sizeof(TEST_INFO_DND));
	memcpy(&m_TestInfo_HF_DND, &dlg.m_TestInfo_HF_DND, sizeof(TEST_INFO_DND));

	memcpy(&m_TestInfo_CND, &dlg.m_TestInfo_CND, sizeof(TEST_INFO_CND));	//add 2022.11.14 for ZTM730 CND

	memcpy(&m_TestInfo_SELF_DND, &dlg.m_TestInfo_SELF_DND, sizeof(TEST_INFO_SELF));
	memcpy(&m_TestInfo_SELF_SATURATION, &dlg.m_TestInfo_SELF_SATURATION, sizeof(TEST_INFO_SELF));

	memcpy(&m_TestInfo_MEASURE_DELTA, &dlg.m_TestInfo_MEASURE_DELTA, sizeof(TEST_INFO_MEASURE_DELTA));
	memcpy(&m_TestInfo_HW_CALIBRATION, &dlg.m_TestInfo_HW_CALIBRATION, sizeof(TEST_INFO_HW_CALIBRATION));
	memcpy(&m_TestInfo_JITTER, &dlg.m_TestInfo_JITTER, sizeof(TEST_INFO_JITTER));
	memcpy(&m_TestInfo_CURRENT_CONSUMPTION, &dlg.m_TestInfo_CURRENT_CONSUMPTION, sizeof(TEST_INFO_CURRENT_CONSUMPTION));
	memcpy(&m_TestInfo_DIGITAL_WHEEL, &dlg.m_TestInfo_DIGITAL_WHEEL, sizeof(TEST_INFO_DIGITAL_WHEEL));
	memcpy(&m_TestInfo_NOISE, &dlg.m_TestInfo_NOISE, sizeof(TEST_INFO_NOISE));

	memcpy(&m_TestInfo_ONLYSELF_DND, &dlg.m_TestInfo_ONLYSELF_DND, sizeof(TEST_INFO_ONLYSELF));

	memcpy(&m_TestInfo_HIGH_VOLTAGE, &dlg.m_TestInfo_HIGH_VOLTAGE, sizeof(TEST_INFO_HIGH_VOLTAGE));

	
	m_TestInfo_NOISE.m_nCheckNoise_ReadTime_Org = m_TestInfo_NOISE.m_nCheckNoise_ReadTimes;
	if(m_TestInfo_NOISE.m_nCheckNoise_ReadTimes_ScalerCoef > 1
	&& m_TestInfo_NOISE.m_nCheckNoise_ReadTimes >= m_TestInfo_NOISE.m_nCheckNoise_ReadTimes_ScalerCoef)
	{
		m_TestInfo_NOISE.m_nCheckNoise_ReadTimes /= m_TestInfo_NOISE.m_nCheckNoise_ReadTimes_ScalerCoef;
	}

	m_bCheckTrxShort = dlg.m_bCheckTrxShort;

	//===============================================
	// [ADD TEST ITEM]
	m_bUseMultiDevice = dlg.m_bUseMultiDevice;
	m_nMultiDeviceCount = dlg.m_nMultiDeviceCount;
	m_bUseStartIndividualTest = dlg.m_bUseStartIndividualTest;

	m_bCheckUserNumber = dlg.m_bCheckUserNumber;

	m_bHidMode = dlg.m_bHidMode;

#if defined(D_USE_CHECK_BUTTON_TEST)
	m_bCheckGpioButton = dlg.m_bCheckGpioButton;

	m_bCheckTouchButton = dlg.m_bCheckTouchButton;
	m_nTouchButton_Threshold = dlg.m_nTouchButton_Threshold;
#endif

	m_bCheckVirtualDrawTest = dlg.m_bCheckVirtualDrawTest;

	((GetDlgItem(IDC_STATIC_CUSTOMER)))->SetWindowText((LPCTSTR)m_strCustomer);
	((GetDlgItem(IDC_STATIC_PROJECT_NAME)))->SetWindowText((LPCTSTR)m_strProjectName);
	((GetDlgItem(IDC_STATIC_RELEASE_VERSION)))->SetWindowText((LPCTSTR)m_strRegisterVersion);

	// Remove connected devices
	if (gCapInfo.icSignature != IC_SIGNATURE[g_nIcType] || 			// IC signature is changed.
		g_nSlaveAddress != nSlaveAddress || 						// I2C Slave address is changed.
		bAdditionalLOG_prev != m_bAdditionalLOG ||					// additional log content
		bUseMultiDevice != m_bUseMultiDevice ||
		bUseStartIndividualTest != m_bUseStartIndividualTest ||
		nMaxDeviceCount != GetMaxDeviceCount()||
		nMptoolCount != m_nMptoolCount ||
		bUseMptoolver != m_bUseMptoolver)
	{
		Util_SetIcSignature(IC_SIGNATURE[g_nIcType]);

		RemoveAllHidDevice();

		TestResultDesc_Initialize();
	}
	else
	{
		int nDeviceCount = m_pUsbHidList->GetCount();
		for (int n = 0; n < nDeviceCount; n++)
		{
			CUsbHid * pUsbHid = GetUsbHidByIndex(n);
			if (pUsbHid == NULL)	continue;

			pUsbHid->m_bIfeModeOn = GetUseIFE();
			pUsbHid->m_bHidMode = m_bHidMode;
		}
	}

	// save TEST_CONF.INI
	TestInfo_SaveData();

	GotoStandBy();

	if(m_pBarcodeSettingDlg && m_pBarcodeSettingDlg->IsShowDialog())
	{
		if(m_bUseBarcode == FALSE)
			BarcodeSetting_ShowDialog(FALSE);
		else
			m_pBarcodeSettingDlg->SetBarcodeInfo(m_bAutoStartTest, m_bFixedBarcodeLength, m_nBarcode_Length);
	}

	BarcodeDisplayInTestVisible();
	EnableStartButton(TRUE);

	UpdateData(TRUE);	// by Paul

	// start check USB HID device timer
	UsbHidDevice_StartSurveyTimer(TRUE);
}

/*******************************************************************************************************************/

HANDLE CTestModeView::GetHidHandle(CUsbHid * pUsbHid, char * pDevicePath)
{
	if( pUsbHid == NULL )
		return NULL;

	HV_TRACE("== %s == [%d] [0x%x]\r\n", __FUNCTION__, pUsbHid->m_nDevIndex, pUsbHid->m_nDevSerial);

#if 1	// for GetLastError() 32(Broken pipe)
	if( pUsbHid->m_DeviceHandle )
	{
		pUsbHid->CloseHidHandle();
		MyDelayMs(200);

		if( pUsbHid->m_DeviceHandle )
		{
			HV_TRACE("## ERROR ## CloseHidHandle fail\n");
			return NULL;
		}
	}
#endif

	return pUsbHid->GetHidHandle(pDevicePath);
}

int CTestModeView::GetTestAvailableCount()
{
	return m_nTestAvailableCount;
}

void CTestModeView::SetListState(CUsbHid * pUsbHid, int nIndex, int nState)
{
	if(pUsbHid)
	{
		pUsbHid->m_nListState = nState;
	}

	m_listInfo.SetItemState(nIndex, nState);
}

void CTestModeView::UsbHidDevice_StartSurveyTimer(BOOL bSoon)
{
	if (m_bAleadyTestEnter)
		return;

	if (bSoon)
	{
		m_nCheckHidDeviceCount = 0;

		SetTimer(TIMER_CHECK_HID_DEVICE, 1000, NULL);
	}
	else
	{
		m_nCheckHidDeviceCount++;

		SetTimer(TIMER_CHECK_HID_DEVICE, 1000, NULL);
	}
}

void CTestModeView::UsbHidDevice_KillSurveyTimer()
{
	KillTimer(TIMER_CHECK_HID_DEVICE);
}

void CTestModeView::KillAllTimer(BOOL bTestStart)
{
	UsbHidDevice_KillSurveyTimer();

	KillTimer(TIMER_GOTO_STANDBY);
	KillTimer(TIMER_DEBUG_TEST_AGAIN);

	ExitSolutionResetNotify();

	if(bTestStart == FALSE)
	{
		m_eAllRetryStatus = eAllRetryStatus_Nothing;
		m_nAllRetryCopyCount = 0;
	}

#if defined(D_DEBUG_FW_UPGRADE_TEST)
	KillTimer(TIMER_DEBUG_FW_UPGRADE_TEST);
#endif

	if (g_bAppExit == FALSE && m_nStandBySec > 0)
	{
		GotoStandBy();
	}
}

BOOL CTestModeView::SurveyUsbHidDevices()
{
	BOOL bDeviceInserted = FALSE;
	BOOL bAleadyTestEnter = m_bAleadyTestEnter;
	CUsbHid * pUsbHid = NULL;
	int n, nDeviceCount;

	char buf[1024];
	DWORD dwRequiredSize;
	DWORD dwDataType;
	int nDevSerial;
	int nDevIndex;

	GUID Guid;
	HDEVINFO info;
	HANDLE deviceHandle;

	DWORD devIndex, needed;
	SP_INTERFACE_DEVICE_DATA ifData;

	if (g_bAppExit)
		return FALSE;

	if (m_bAleadyTestEnter == FALSE)
	{
		g_bUseMyDelayMs = FALSE;
	}

	HidD_GetHidGuid(&Guid);

	info = SetupDiGetClassDevs(&Guid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if (info == INVALID_HANDLE_VALUE)
		return FALSE;

	// save preivous state
	nDeviceCount = m_pUsbHidList->GetCount();
	for (n = 0; n < nDeviceCount; n++)
	{
		pUsbHid = GetUsbHidByIndex(n);
		if (pUsbHid == NULL)	continue;

		pUsbHid->m_bExistPrev = pUsbHid->m_bExist;
		pUsbHid->m_bExistCheck = 0;
	}

	//HV_TRACE("-\r\n");

	// Loop until device interfaces are found.
	for (devIndex = 0; ; ++devIndex)
	{
		// Get device Interface data
		ZeroMemory(&ifData, sizeof(ifData));
		ifData.cbSize = sizeof(ifData);

		// A return of FALSE from this function means that there are no more devices.
		if( !SetupDiEnumDeviceInterfaces(info, NULL, &Guid, devIndex, &ifData) )
			break;

		// Get the size of the detail information for the device
		SetupDiGetDeviceInterfaceDetail(info, &ifData, NULL, 0, &needed, NULL);

		PSP_INTERFACE_DEVICE_DETAIL_DATA detail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)new BYTE[needed];
		detail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
		SP_DEVINFO_DATA did = { sizeof(SP_DEVINFO_DATA) };

		// Get device interface : ex) "\\?\hid#vid_14e5&pid_003a&mi_00#7&1c35c3f7&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}"
		if (SetupDiGetDeviceInterfaceDetail(info, &ifData, detail, needed, NULL, &did) == FALSE)
			goto DELETE_DEVICE_DETAIL;

		// Check Vendodr ID
		if ( !(StrStrI(detail->DevicePath, "vid_14E5") && StrStrI(detail->DevicePath, "mi_00")) )
			goto DELETE_DEVICE_DETAIL;

		// Get device name from registry : ex) "HID\VID_14E5&PID_003A&REV_0500&MI_00"
		if (SetupDiGetDeviceRegistryPropertyA(info, &did, SPDRP_HARDWAREID, &dwDataType, (PBYTE)buf, sizeof(buf), &dwRequiredSize) == FALSE)
		{
			goto DELETE_DEVICE_DETAIL;
		}

		// Get UNIQUE serial number of device : ex) 0x1c35c3f7
		nDevSerial = GetDevSerial(detail->DevicePath);
		if (nDevSerial == 0)
		{
			HV_TRACE("Invalide Device Serial Number\n");
			goto DELETE_DEVICE_DETAIL;
		}

		// Get USB HID device from UNIQUE serial number of device
		pUsbHid = m_pUsbHidList->GetUsbHidPtrBySerial(nDevSerial);

		// Get device index (REV_xx)
		nDevIndex = GetTsmId(buf);

		//HV_TRACE("\r\n");
		//HV_TRACE("HID detected [0x%x] \t [%d]\r\n", nDevSerial, nDevIndex);

		if (bAleadyTestEnter)
		{
			if (pUsbHid != NULL)
			{
				pUsbHid->m_bExistCheck = 1;
			}

			goto DELETE_DEVICE_DETAIL;
		}

		if (pUsbHid == NULL)
		{
			// Add USB HID device
			pUsbHid = m_pUsbHidList->AddUsbHid(nDevSerial, nDevIndex, GetMaxDeviceCount(), detail->DevicePath);
			if (pUsbHid == NULL)
			{
				HV_TRACE("## ERROR ## Add USB HID list failed  [0x%x]\n", nDevSerial);
				goto DELETE_DEVICE_DETAIL;
			}

			pUsbHid->m_bIfeModeOn = GetUseIFE();
			pUsbHid->m_bHidMode = m_bHidMode;

			pUsbHid->m_nMpToolVersion = GetTsmVersion(buf);

			HV_TRACE("== Add USB HID == [%d] [0x%x]\n", pUsbHid->m_nDevIndex, nDevSerial);

			#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
			pUsbHid->m_bIsReConnect = FALSE;
			#endif

			// Get HID device handle
			deviceHandle = GetHidHandle(pUsbHid, detail->DevicePath);
			if (deviceHandle == NULL)
			{
				HV_TRACE("## ERROR ## Get HID Handle failed - 1 [0x%x]\r\n", pUsbHid->m_nDevSerial);
				goto DEVICE_DETECTED;
			}
		}
		else
		{
			if (pUsbHid->m_bDevLost)
			{
				// Device is lost then found by RESET button while TSM_TEST is in progress.
				// When device is lost, CUsbHid::CloseHidHandle() is called in Hid_WriteData().

				pUsbHid->m_bDevLost = FALSE;
			}
			else
			{
				#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
				pUsbHid->m_bIsReConnect = FALSE;

				if (pUsbHid->m_nSignatureErrorTimeout)
				{
					pUsbHid->m_bExistCheck = 1;
					goto DELETE_DEVICE_DETAIL;
				}
				#endif

				if (pUsbHid->m_bDuplicateError)
				{// ignore duplicated device
					pUsbHid->m_bExistCheck = 1;
					goto DELETE_DEVICE_DETAIL;
				}

				#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
				if (pUsbHid->m_bSignatureError == TRUE)
				{// ERROR Signature
					HV_TRACE("Retry get signature [0x%x]\r\n", nDevSerial);
					pUsbHid->m_bIsReConnect = TRUE;
				}
				else
				#endif
				{// NO ERROR
					if( pUsbHid->m_nDevIndex != nDevIndex )
					{
						HV_TRACE("\r\n");
						HV_TRACE("## CAUTION ## Device serial is changed [%d]\r\n", nDevIndex);
						HV_TRACE("\r\n");

						RemoveHidDevice(pUsbHid);

						bDeviceInserted = TRUE;

						#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
						m_pConnectInfoDlg->AddLog("DIS-CONNECT", pUsbHid);
						#endif

						goto DELETE_DEVICE_DETAIL;
					}
					else if (strcmp(pUsbHid->m_DevicePath, detail->DevicePath) != 0)
					{// Check device path
						UpdateUsbHidDevices(pUsbHid, nDevSerial, nDevIndex, GetMaxDeviceCount(), detail->DevicePath);

						pUsbHid->m_bExistCheck = 1;
						pUsbHid->m_bMpToolInitDone = FALSE;
						bDeviceInserted = TRUE;
						goto DELETE_DEVICE_DETAIL;
					}
					#if !defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
					else if (pUsbHid->m_bMpToolInitDone == FALSE)
					{
						pUsbHid->m_bExistCheck = 1;
						bDeviceInserted = TRUE;
						goto DELETE_DEVICE_DETAIL;
					}
					#endif
					else
					{
						pUsbHid->m_bExistCheck = 1;
					}

					goto DELETE_DEVICE_DETAIL;
				}
			}
		}

		// Retry to get HID device handle
		if (pUsbHid->m_DeviceHandle == NULL)
		{
			HV_TRACE("GetHidHandle() retry [0x%x]\r\n", nDevSerial);

			deviceHandle = GetHidHandle(pUsbHid, detail->DevicePath);
			if (deviceHandle == NULL)
			{
				HV_TRACE("## ERROR ## Get HID Handle failed - 2 [0x%x]\r\n", nDevSerial);
				goto DEVICE_DETECTED;
			}
		}

DEVICE_DETECTED:

		bDeviceInserted = TRUE;

		pUsbHid->m_bExistCheck = 1;

#if defined(D_USE_CHECK_MPTOOL_REVISION)
		m_TestInfo_CURRENT_CONSUMPTION.bEnable = (pUsbHid->m_nMpToolVersion < 5) ? FALSE : TRUE;
#endif

DELETE_DEVICE_DETAIL:

		delete[](PBYTE)detail;
	}

	SetupDiDestroyDeviceInfoList(info);

	return bDeviceInserted;
}

BOOL CTestModeView::FindUsbHid()
{
	BOOL bAleadyTestEnter = m_bAleadyTestEnter;
	int n, nDeviceCount;
	CUsbHid * pUsbHid = NULL;

	BOOL bDeviceInserted = SurveyUsbHidDevices();
	BOOL bDeviceRemoved = FALSE;

	nDeviceCount = m_pUsbHidList->GetCount();

	// detect new device or retry signature error
	if (bDeviceInserted)
	{
		// [Don't Delete] for stable time
		Sleep(300);

		#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
		// Check IC Signature
		for (n = 0; n < nDeviceCount; n++)
		{
			pUsbHid = GetUsbHidByIndex(n);
			if (pUsbHid == NULL)					continue;

			// ignore duplicated device
			if (pUsbHid->m_bDuplicateError)			continue;

			if (pUsbHid->m_nSignatureErrorTimeout)	continue;

			if (pUsbHid->m_DeviceHandle == NULL)
			{
				CheckSignatureError(pUsbHid);
				continue;
			}

			if (pUsbHid->m_bSignatureError)
			{
				// we need some delay time to HID communicate after createfile()
				UINT64 dt = GetTimeInMillis() - pUsbHid->m_GetHandleTime;
				if (dt < GET_HID_DEV_HANDLE_DELAY_TIME)
					Sleep(GET_HID_DEV_HANDLE_DELAY_TIME - dt);

				// Check IC Signature
				pUsbHid->m_bSignatureError = CheckIcSignature(pUsbHid) ? FALSE : TRUE;

				CheckSignatureError(pUsbHid);

				m_pConnectInfoDlg->AddLog(pUsbHid->m_bIsReConnect ? "RE-CONNECT" : "CONNECT", pUsbHid);

				if (pUsbHid->IsTestAvailable())
					bDeviceInserted = TRUE;
				else
					pUsbHid->FailLedControl(TRUE);
			}
		}
		#endif
	}

	// Init MPTool
	#if !defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
	for (n = 0; n < nDeviceCount; n++)
	{
		pUsbHid = GetUsbHidByIndex(n);
		if (pUsbHid == NULL)						continue;

		// ignore duplicated device
		if (pUsbHid->m_bDuplicateError)				continue;

		if (pUsbHid->m_bMpToolInitDone == TRUE)		continue;

		MpTool_Init(pUsbHid);

		if (pUsbHid->IsTestAvailable() == FALSE)
			pUsbHid->FailLedControl(TRUE);
	}
	#endif

	// check whether device is lost or not
	for (n = nDeviceCount-1; n >= 0; n--)
	{
		pUsbHid = GetUsbHidByIndex(n);
		if (pUsbHid == NULL) continue;

		if (pUsbHid->m_bExistPrev && (pUsbHid->m_bExistCheck == 0))		// device is lost
		{
			if (bAleadyTestEnter == FALSE)
			{
				bDeviceRemoved = TRUE;

				RemoveHidDevice(pUsbHid);

				#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
				m_pConnectInfoDlg->AddLog("DIS-CONNECT", pUsbHid);
				#endif
			}
			else if (pUsbHid->m_bDevLost == FALSE)						// device is lost while TSM test is going on.
			{
				bDeviceRemoved = TRUE;

				pUsbHid->m_bDevLost = TRUE;
				pUsbHid->m_bInit = FALSE;

				SetListState(pUsbHid, pUsbHid->m_nDevIndex + FIRST_DEVICE_INDEX, ITEM_STATE_DISABLE);

				m_listInfo.SetItemCount((int)m_infoDatabase.size());

				#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
				m_pConnectInfoDlg->AddLog("DIS-CONNECT", pUsbHid);
				#endif
			}
		}
	}

	if (bDeviceInserted || bDeviceRemoved)
	{
		ChangeDeviceStatus();
		TestResultDesc_Initialize();

		EnableStartButton(TRUE);
	}

	g_bUseMyDelayMs = TRUE;

	return (GetTestAvailableCount() > 0) ? TRUE : FALSE;
}

BOOL CTestModeView::IsExistUsbHid(CUsbHid * pUsbHid, BOOL bGetHandle)
{
	if(pUsbHid == NULL)
		return FALSE;

	BOOL bRet = FALSE;

	GUID Guid;
	HDEVINFO info;

	DWORD devIndex, needed;
	SP_INTERFACE_DEVICE_DATA ifData;

	HidD_GetHidGuid(&Guid);

	info = SetupDiGetClassDevs(&Guid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if (info == INVALID_HANDLE_VALUE)
		return FALSE;

	for (devIndex = 0; ; ++devIndex)
	{
		ZeroMemory(&ifData, sizeof(ifData));
		ifData.cbSize = sizeof(ifData);

		// A return of FALSE from this function means that there are no more devices.
		if( !SetupDiEnumDeviceInterfaces(info, NULL, &Guid, devIndex, &ifData) )
			break;

		// Get the size of the detail information for the device
		SetupDiGetDeviceInterfaceDetail(info, &ifData, NULL, 0, &needed, NULL);

		PSP_INTERFACE_DEVICE_DETAIL_DATA detail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)new BYTE[needed];
		detail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
		SP_DEVINFO_DATA did = { sizeof(SP_DEVINFO_DATA) };

		// Get device interface : ex) "\\?\hid#vid_14e5&pid_003a&mi_00#7&1c35c3f7&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}"
		if (SetupDiGetDeviceInterfaceDetail(info, &ifData, detail, needed, NULL, &did) == FALSE)
			goto DELETE_DEVICE_DETAIL;

		// Check Vendodr ID
		if ( !(StrStrI(detail->DevicePath, "vid_14E5") && StrStrI(detail->DevicePath, "mi_00")) )
			goto DELETE_DEVICE_DETAIL;

		// Check device path
		if (strcmp(pUsbHid->m_DevicePath, detail->DevicePath) == 0)
		{
			if (bGetHandle == FALSE)
			{
				bRet = TRUE;
			}
			else
			{// when TSM test is started, handle of HID device should re-create.
				if (GetDevSerial(pUsbHid->m_DevicePath) != 0 && pUsbHid->ReGetHidHandle() != NULL)
					bRet = TRUE;
			}
		}
		// if device interface is changed, update device interface and re-get hid handle
		else if(bGetHandle == TRUE)
		{
			char buf[1024];
			DWORD dwRequiredSize;
			DWORD dwDataType;

			if(SetupDiGetDeviceRegistryPropertyA(info, &did, SPDRP_HARDWAREID, &dwDataType, (PBYTE)buf, sizeof(buf), &dwRequiredSize) == FALSE)
				goto DELETE_DEVICE_DETAIL;

			int nDevSerial = GetDevSerial(detail->DevicePath);
			int nDevIndex = GetTsmId(buf);

			if(nDevSerial != 0 && nDevIndex == pUsbHid->m_nDevIndex)
			{
				if(UpdateUsbHidDevices(pUsbHid, nDevSerial, nDevIndex, GetMaxDeviceCount(), detail->DevicePath))
					bRet = TRUE;
			}
		}

DELETE_DEVICE_DETAIL:

		delete[](PBYTE)detail;

		if(bRet == TRUE)
			break;
	}

	SetupDiDestroyDeviceInfoList(info);

	return bRet;
}

BOOL CTestModeView::UpdateUsbHidDevices(CUsbHid * pUsbHid, int nDevSerial, int nDevIndex, int nMaxDeviceCount, char * pDevicePath)
{
	HV_TRACE("== Update USB HID == [%d] [0x%x]\n", nDevIndex, nDevSerial);

	m_pUsbHidList->UpdateUsbHid(nDevSerial, nDevIndex, nMaxDeviceCount, pDevicePath);

	if(pUsbHid->ReGetHidHandle() == NULL)
		return FALSE;

	return TRUE;
}

/*
원인) F/W Download 중 PC 에서 해당 USB 장치가 disconnect > HID 통신 에러 발생 > 테스트 FAIL
	> HID 장치 Handle Close > 테스트 중에는 USB HID 장치의 disconnect 를 detect 하지 못함
	> HID 장치 리스트에서는 여전히 이전의 device interface 를 유지
	> USB 장치 connect 시 device interface 가 변경이 되었을 경우
	> 다시 테스트 시작할 때 이전의 device interface 를 이용하여 장치가 존재하는지 확인 시 장치가 존재하지 않으므로 테스트를 진행하지 않고 종료해 버림
대책) 테스트 도중 HID 통신 에러 등으로 인해 HID 장치 Handle Close 후 HID 장치 리스트 업데이트하도록 수정
	1) 해당 장치가 연결이 되었다면 HID 장치 리스트에서 device interfae 정보만 업데이트
	2) 해당 장치가 연결이 되지 않았다면 HID 장치 리스트에서 해당 장치를 제거
*/
LRESULT CTestModeView::OnUsbHidDeviceDisconnect(WPARAM wParam, LPARAM lParam)
{
	CUsbHid * pUsbHid = GetUsbHidByDevIndex((int)wParam);
	if (pUsbHid == NULL) return 0L;

	// device interface does not change
	if (IsExistUsbHid(pUsbHid) == TRUE)
		return 1L;

	SetListState(pUsbHid, pUsbHid->m_nDevIndex + FIRST_DEVICE_INDEX, ITEM_STATE_DISABLE);

	RemoveHidDevice(pUsbHid);

	// apply delay time after CloseHidHandle()
	MyDelayMs(500);

	return 1L;
}

#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
BOOL CTestModeView::CheckIcSignature(CUsbHid * pUsbHid)
{
	MpTool_Init(pUsbHid);

	// Power Reset & Power Sequence
	int nError = MpTool_PowerSequenceAfterReset(pUsbHid, FALSE);
	if (nError != Z_HID_OK)
	{
		if (nError == Z_ERR_POWER_CONTROL)
			HV_TRACE("## ERROR ## %s() ## POWER CONTROL\n", __FUNCTION__);
		else
			HV_TRACE("## ERROR ## %s() ## POWER SEQUENCE (0x%04X)\n", __FUNCTION__, pUsbHid->GetLastError());
		return FALSE;
	}

	GetTouchVoltage(pUsbHid);

	if (m_bAdditionalLOG)
	{
		if (ReadASICID(pUsbHid) == FALSE)
		{
			HV_TRACE("## ERROR ## %s() ## ReadASICID() fail \n", __FUNCTION__);
			//return FALSE;				// Do not return to IC firmware error
		}
	}

	HV_TRACE("## OK ## %s() ##  [%d] [0x%x]\n", __FUNCTION__, pUsbHid->m_nDevIndex, pUsbHid->m_nDevSerial);
	return TRUE;
}

void CTestModeView::CheckSignatureError(CUsbHid * pUsbHid)
{
	if( pUsbHid == NULL )
		return;
	else if (pUsbHid->m_bSignatureError == FALSE)
		return;

	pUsbHid->m_nSignatureErrorCount++;

	if (pUsbHid->m_nSignatureErrorCount > 10)
	{// limit 10 times to retry
		HV_TRACE("limit 10 times to retry\n");

		pUsbHid->m_nSignatureErrorTimeout = TRUE;
	}
	else if( pUsbHid->m_DeviceHandle )
	{
		pUsbHid->m_nDeviceCloseSkipCount++;

		if (pUsbHid->m_nDeviceCloseSkipCount >= 3)
		{// If IC signature error is occur coninuous, close device then re-open.
			HV_TRACE("m_nDeviceCloseSkipCount is over\n");

			pUsbHid->CloseHidHandle();
		}
	}
}
#endif

void CTestModeView::RemoveHidDevice(CUsbHid * pUsbHid)
{
	if (pUsbHid == NULL)
		return;

	HV_TRACE("\r\n");
	HV_TRACE("== Remove USB HID == [%d] [0x%x] \n", pUsbHid->m_nDevIndex, pUsbHid->m_nDevSerial);

	if (g_bAppExit == FALSE)
	{
		pUsbHid->m_nResult = RESULT_NONE;
		pUsbHid->m_strResult = "";

		SemaWait(g_hInfoSemaphore);

		int nCount = m_infoDatabase.size();
		for (int i = 0; i < nCount; i++)
		{
			m_infoDatabase[i].strResult[pUsbHid->m_nDevIndex] = "";
		}

		::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);
	}

	pUsbHid->CloseHidHandle();

	if (m_pUsbHidList->DeleteUsbHid(pUsbHid->m_nDevSerial))
	{
		if (m_nTestAvailableCount > 0)
			m_nTestAvailableCount--;
	}
}

void CTestModeView::RemoveAllHidDevice()
{
	CUsbHid * pUsbHid;
	int nDeviceCount = m_pUsbHidList->GetCount();

	HV_TRACE("== Remove ALL USB HID == \r\n");

	SemaWait(g_hSemaphore);

	for (int n = nDeviceCount-1; n >= 0; n--)
	{
		pUsbHid = GetUsbHidByIndex(n);
		if (pUsbHid == NULL) continue;

		SetListState(pUsbHid, pUsbHid->m_nDevIndex + FIRST_DEVICE_INDEX, ITEM_STATE_DISABLE);

		RemoveHidDevice(pUsbHid);
	}

	::ReleaseSemaphore(g_hSemaphore, 1, NULL);
}

int CTestModeView::GetMaxDeviceCount()
{
	if( m_bUseMultiDevice )
	{
		if( m_nMultiDeviceCount <= 0 )
			m_nMultiDeviceCount = 1;
		else if( m_nMultiDeviceCount >= MAX_DEVICE_COUNT )
			m_nMultiDeviceCount = MAX_DEVICE_COUNT;

		return m_nMultiDeviceCount;
	}
	else
	{
		return 1;
	}
}

void CTestModeView::CalculateMaxDeviceCount()
{
	int nDeviceCount = m_pUsbHidList->GetCount();
	BOOL bEnabled;

	int nPrevMaxDeviceIndex = m_nMaxDeviceIndex;

	m_nMaxDeviceIndex = -1;
	m_nTestAvailableCount = 0;

	// Get maximum device ID
	for (int n = 0; n < nDeviceCount; n++)
	{
		CUsbHid * pUsbHid = GetUsbHidByIndex(n);

		if (pUsbHid && pUsbHid->IsTestAvailable())
		{
			m_nTestAvailableCount++;

			if (pUsbHid->m_nDevIndex > m_nMaxDeviceIndex)
			{
				m_nMaxDeviceIndex = pUsbHid->m_nDevIndex;
			}
		}
	}

	// Set maximum device count
	if (m_nMaxDeviceIndex == -1){
		m_nMaxDeviceIndex = 0;
	}
	else if (nDeviceCount > 0 && m_nMaxDeviceIndex >= 0){
		m_nMaxDeviceIndex++;
	}

	if (nPrevMaxDeviceIndex != m_nMaxDeviceIndex)
	{
		ListCtrl_Initialize();
	}
	else
	{
		for (int n = 0; n < m_nMaxDeviceIndex; n++)
		{
			CUsbHid * pUsbHid = GetUsbHidByDevIndex(n);
			if (pUsbHid && pUsbHid->IsTestAvailable())
				bEnabled = TRUE;
			else
				bEnabled = FALSE;

			SetListState(pUsbHid, n + FIRST_DEVICE_INDEX, bEnabled ? ITEM_STATE_ENABLE : ITEM_STATE_DISABLE);
		}
	}

	//DisplayTotalTestStep();
}

void CTestModeView::ChangeDeviceStatus()
{
	if (m_bAleadyTestEnter)
		return;

	CUsbHid * pUsbHid;

	m_nMaxMpToolVersion = 0;

	CalculateMaxDeviceCount();

	if (GetTestAvailableCount() > 0)
		m_bExistDevice = TRUE;
	else
		m_bExistDevice = FALSE;

	m_btnStartTestAll.Invalidate(TRUE);

	for (int n = 0; n < m_nMaxDeviceIndex; n++)
	{
		pUsbHid = GetUsbHidByDevIndex(n);
		if (pUsbHid == NULL)	continue;

		if (pUsbHid->m_bInit == FALSE)
		{
			// ListCtrl erase first time when device is connected.
			// Or Device is lost by RESET button while FW downloading is in progress.
			ClearTsmTestMessageOnList(pUsbHid->m_nDevIndex);
		}
	}

	SemaWait(g_hInfoSemaphore);
	m_listInfo.SetItemCount((int)m_infoDatabase.size());
	m_listInfo.EnsureVisible((int)m_infoDatabase.size(), FALSE);
	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);

	// 초기 연결 시 PASS 표시
	if (m_bExistDevice == TRUE)
	{
		for (int n = 0; n < m_nMaxDeviceIndex; n++)
		{
			pUsbHid = GetUsbHidByDevIndex(n);
			if (pUsbHid == NULL)	continue;

			if (m_nMaxMpToolVersion < pUsbHid->m_nMpToolVersion)
				m_nMaxMpToolVersion = pUsbHid->m_nMpToolVersion;

			#if(0)
			if (pUsbHid->m_nMpToolVersion < 6 ||
				pUsbHid->m_fVoltageVcc == 0 ||
				pUsbHid->m_fVoltageVi2c == 0)
			{
				pUsbHid->m_nTestStep = 0;
				DisplayResultToList(pUsbHid, RESULT_PASS);
			}
			#endif
		}
	}

	if(m_pBarcodeSettingDlg)
	{
		BOOL bRet = -1;

		do
		{
			bRet = m_pBarcodeSettingDlg->PostMessage(WM_USER_BARCODE_DEVICE_CHANGED, 0, 0);
		}while( bRet == 0 );
	}
}

LRESULT CTestModeView::OnExecuteAsicIdMode(WPARAM wParam, LPARAM lParam)
{
	KillAllTimer();

	TSM_SetUpgradeInfo();

	ASIC_ID_dlg dlg(this);

	dlg.DoModal();

	UsbHidDevice_StartSurveyTimer(TRUE);
	return 1L;
}

LRESULT CTestModeView::OnExecuteGraphMode(WPARAM wParam, LPARAM lParam)
{
	if (m_bAleadyTestEnter)
		return 0L;

	KillAllTimer();

#if !defined(_DEBUG)
	// password dialog
	CInputDlg dlg;
	dlg.m_strTitle.Format("PASSWORD");
	dlg.m_bPassword = TRUE;

	if (dlg.DoModal() == IDOK && dlg.m_strInput.CompareNoCase("ZINITIX1") == 0)
#endif
	{
		// zDataAnalyzer dialog
		CZDataAnalyzerDlg zDataAnalyzerDlg;
		zDataAnalyzerDlg.SetTestModeViewPtr(this);
		zDataAnalyzerDlg.SetDataAnalysisDir(m_strSaveResultDir);
		zDataAnalyzerDlg.DoModal();
	}

	UsbHidDevice_StartSurveyTimer(TRUE);
	return 1L;
}

LRESULT CTestModeView::OnExecuteBarcodeSetting(WPARAM wParam, LPARAM lParam)
{
	if (m_bAleadyTestEnter || m_bUseBarcode == FALSE)
		return 0L;

	if( GetTestAvailableCount() == 0 )
		return 0L;

	KillAllTimer();

	BarcodeSetting_ShowDialog(TRUE);

	return 1L;
}

void CTestModeView::BarcodeSetting_ShowDialog(BOOL bShow)
{
	if (m_pBarcodeSettingDlg == NULL)
		return;

	if (bShow)
		m_pBarcodeSettingDlg->SetBarcodeInfo(m_bAutoStartTest, m_bFixedBarcodeLength, m_nBarcode_Length);

	m_pBarcodeSettingDlg->SendMessage(WM_USER_BARCODE_SHOW_DIALOG, bShow ? 1 : 0, 0);

	EnableStartButton(bShow ? FALSE : TRUE);
}

void CTestModeView::BarcodeSetting_TestCompleted(int nIndex, BOOL bError)
{
	if(m_pBarcodeSettingDlg == NULL || m_pBarcodeSettingDlg->IsShowDialog() == FALSE)
		return;

	BOOL bRet = -1;

	do
	{
		bRet = m_pBarcodeSettingDlg->PostMessage(WM_USER_BARCODE_TEST_COMPLETED, nIndex, bError ? 1 : 0);
	}while( bRet == 0 );
}

void CTestModeView::PostMessageEx(UINT msg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet;

	do
	{
		bRet = PostMessage(msg, wParam, lParam);
	}while( bRet == 0 );
}

void CTestModeView::MpTool_Init(CUsbHid * pUsbHid)
{
	if (pUsbHid == NULL)
		return;

	if (pUsbHid->m_bMpToolInitDone == TRUE)
		return;

	if (pUsbHid->m_DeviceHandle == NULL)
	{
		HV_TRACE("== %s == GetHidHandle() RETRY ~~~\r\n", __FUNCTION__);
		GetHidHandle(pUsbHid, pUsbHid->m_DevicePath);
		if (pUsbHid->m_DeviceHandle == NULL)
			return;
	}

	// Set Slave Address
	if (pUsbHid->SetSlaveAddress(I2C_SLAVE_ADDRESS[g_nSlaveAddress], FALSE) != Z_HID_OK)
	{
		HV_TRACE("## ERROR ## SET SLAVE ADDRESS\n");
		return;
	}
	Sleep(10);

	if (pUsbHid->DisableDigitizerMode(TRUE) != Z_HID_OK)
	{
		HV_TRACE("## ERROR ## CANNOT DIGITIZER MODE DISABLE\n");
		return;
	}
	Sleep(10);

	if (m_bPowerOffWhenUsbDetect) //22.09.02 송성용 특정 MPtool의 버전으로 전류측정을 구분하면서 해당 부분을 사용유무가 아닌 버전의 타입으로 판별
	{// power off
		if (pUsbHid->m_nMpToolVersion >= 80)
		{
			if (MpTool_PowerControl(pUsbHid, TRUE) == Z_HID_OK)
			{
				GetTouchVoltage(pUsbHid);
			}
		}
		else
		{
			if (MpTool_PowerControl(pUsbHid, FALSE) != Z_HID_OK)
				return;
			Sleep(10);
		}
	}
	else
	{// power on
		if (pUsbHid->PowerResetControlEx(100, 100) != Z_HID_OK)
			return;
		Sleep(10);

		GetTouchVoltage(pUsbHid);
	}
#if !defined(D_USE_GET_VOLTAGE_ADC)
	DisplayMpToolVersion(pUsbHid);

#else
	/*
	if (pUsbHid->m_nMpToolVersion < 80)
	{
		if (pUsbHid->m_nMpToolVersion == 05 && m_nModuleVendorCode == 51)
		{
			DisplayMpToolVersion(pUsbHid);
			AfxMessageBox("Cannot Use MPtool Ver_05");
			EnableStartButton(FALSE);
			pUsbHid->m_bMpToolInitDone = 0;
			return;

		}
		DisplayMpToolVersion(pUsbHid);
	}
	*/

	if (m_nMptoolCount > pUsbHid->m_nMpToolVersion //org =>  change 221012
		&& m_nModuleVendorCode == 51
		&& m_bUseMptoolver == 1)					//22.09.22 Karl 요청사항 추가 MPtool 특정버전 이하 검출 시 테스트 실행 x
	{
		DisplayMpToolVersion(pUsbHid);
		AfxMessageBox("Please Check MPtool's Version");
		EnableStartButton(FALSE);
		pUsbHid->m_bMpToolInitDone = 0;
		return;
	}



#endif


	if(pUsbHid->m_nMpToolVersion < 80) //ver80 아래 즉 전류측정을 하지 않았던 MPtool
	{
		DisplayMpToolVersion(pUsbHid);
		pUsbHid->m_bMpToolInitDone = TRUE;
	}
	else            //80이상인 ver들은 MPtool6 을 사용하여 별다른 시방없이 사용가능 하며 Ver을 나타내지 않는다.
		pUsbHid->m_bMpToolInitDone = TRUE;


}

int CTestModeView::MpTool_PowerControl(CUsbHid * pUsbHid, BOOL bOn, BOOL bJumpRom)
{
	if (bOn)
	{
		int nError = MpTool_PowerSequenceAfterReset(pUsbHid, FALSE, bJumpRom);
		if (nError != Z_HID_OK)
			return nError;
	}
	else
	{
		if (pUsbHid->PowerControl(FALSE) != Z_HID_OK)
		{
			HV_TRACE("## ERROR ## %s() ## POWER_CONTROL FAIL\n", __FUNCTION__);
			return Z_ERR_POWER_CONTROL;
		}
		Sleep(100);
	}

	return Z_HID_OK;
}

int CTestModeView::MpTool_PowerSequenceAfterReset(CUsbHid * pUsbHid, BOOL bChecksum, BOOL bJumpRom)
{
	int nError;

	for(int nRetry = 0; nRetry < 3; nRetry++)
	{
		nError = pUsbHid->PowerResetControlEx(100, 100);
		if (nError != Z_HID_OK)
		{
			HV_TRACE("## ERROR ## %s() ## [%d] POWER_CONTROL FAIL\n", __FUNCTION__, pUsbHid->m_nDevIndex);
			nError = Z_ERR_POWER_CONTROL;
			continue;
		}
		Sleep(m_nPowerCtrlDelayTime);

		nError = pUsbHid->PowerSequenceEx(bChecksum, bJumpRom) ? Z_HID_OK : Z_HID_FAIL;
		if (nError != Z_HID_OK)
		{
			HV_TRACE("## ERROR ## %s() ## [%d] POWER SEQUENCE() failed\n", __FUNCTION__, pUsbHid->m_nDevIndex);
			nError = Z_ERR_POWER_SEQUENCE;
			continue;
		}
		Sleep(100);
		break;
	}

	return nError;
}

void CTestModeView::GetTouchVoltage(CUsbHid * pUsbHid)
{
#if defined(D_USE_GET_VOLTAGE_ADC)
	if (pUsbHid == NULL)
		return;
	//else if (pUsbHid->m_nMpToolVersion < 10)
	//	return;
	else if (pUsbHid->m_fVoltageVcc != 0 && pUsbHid->m_fVoltageVi2c != 0 )
		return;

	int nRet;
	u16 nVoltage1, nVoltage2, nVIH_1;

	nRet = pUsbHid->GetTouchVoltage(&nVoltage1, &nVoltage2, &nVIH_1);

	//if (nRet != 0 || nVoltage1 == 0 || nVoltage2 == 0)
		//return;


	double fVoltage1, fVoltage2, fVIH_1;

	if(m_bMeasureLdo)
	{// 측정된 값을 그대로 출력
		fVoltage1 = (nVoltage1 * 3.3) / (float)4095;
		fVoltage2 = (nVoltage2 * 3.3) / (float)4095;
		fVIH_1 = (nVIH_1 * 3.3) / (float)4095;

		m_vih_ref =  fVIH_1;
		g_voltage = fVoltage2;
		g_vcca_ref = fVoltage1;
	}
	else
	{
		if (nVoltage1 == 4095)		fVoltage1 = 3.30;
		else						fVoltage1 = (nVoltage1 * 3.3) / (float)4095;

		if (nVoltage2 == 4095)		fVoltage2 = 3.30;
		else						fVoltage2 = (nVoltage2 * 3.3) / (float)4095;

		if (nVIH_1 == 4095)			fVIH_1 = 3.30;
		else						fVIH_1 = (nVIH_1 * 3.3) / (float)4095;


		if (fVoltage1 >= 2.7 && fVoltage1 < 2.9)
			fVoltage1 = 2.8;
		else if (fVoltage1 >= 2.9 && fVoltage1 < 3.15)
			fVoltage1 = 3.0;
		else if (fVoltage1 >= 3.15 && fVoltage1 < 3.3)
			fVoltage1 = 3.2;
		else if (fVoltage1 >= 3.3  && fVoltage1 < 3.45)
			fVoltage1 = 3.4;
		else if (fVoltage1 >= 3.45  )
			fVoltage1 = 3.6;

		if (fVoltage2 >= 1.15 && fVoltage2 < 1.25)
			fVoltage2 = 1.2;
		else if (fVoltage2 >= 1.25 && fVoltage2 < 1.35)
			fVoltage2 = 1.3;
		else if (fVoltage2 >= 1.35 && fVoltage2 < 1.45)
			fVoltage2 = 1.4;
		else if (fVoltage2 >= 1.45 && fVoltage2 < 1.55)
			fVoltage2 = 1.5;
		else if (fVoltage2 >= 1.55 && fVoltage2 < 1.65)
			fVoltage2 = 1.6;
		else if (fVoltage2 >= 1.65 && fVoltage2 < 1.74)  //김극래 책임 요청 75-> 74
			fVoltage2 = 1.7;
		else if (fVoltage2 >= 1.74 && fVoltage2 < 1.85)
			fVoltage2 = 1.8;
		else if (fVoltage2 >= 1.85 && fVoltage2 < 1.95)
			fVoltage2 = 1.9;
		else if (fVoltage2 >= 1.95 && fVoltage2 < 2.05)
			fVoltage2 = 2.0;
		else if (fVoltage2 >= 2.05 && fVoltage2 < 2.15)
			fVoltage2 = 2.1;
		else if (fVoltage2 >= 2.15 && fVoltage2 < 2.25)
			fVoltage2 = 2.2;
		else
			fVoltage2 = fVoltage1;

		//김극래 책임 요청 VIH 값 조정
		if (fVIH_1 < 1.08)
			fVIH_1 = fVoltage1;
		else if ( fVIH_1 >= 1.08 && fVIH_1 < 1.13)
			fVIH_1 = 1.1;
		else if (fVIH_1 >= 1.13 && fVIH_1 < 1.18)
			fVIH_1 = fVoltage1;
		else if (fVIH_1 >= 1.18 && fVIH_1 < 1.23)
			fVIH_1 = fVoltage1;
		else if (fVIH_1 >= 1.23 && fVIH_1 < 1.29)
			fVIH_1 = 1.26;
		else if (fVIH_1 >= 1.29 && fVIH_1 < 1.33)
			fVIH_1 = fVoltage1;
		else if (fVIH_1 >= 1.33 && fVIH_1 < 1.39)
			fVIH_1 = 1.36;
		else if(fVIH_1 >= 1.39 && fVIH_1 < 1.45)
			fVIH_1 = fVoltage1;
		else if(fVIH_1 > 1.46)
			fVIH_1 = fVoltage1;

		m_vih_ref =  fVIH_1;
		g_voltage = fVoltage2;
		g_vcca_ref = fVoltage1;
	}

 	TRACE("GET_ADC : [%d]  \t%.2f\t%.2f\t%.2f \n", pUsbHid->m_nDevIndex, fVoltage1, fVoltage2, fVIH_1);

	pUsbHid->m_fVoltageVcc = fVoltage1;
	pUsbHid->m_fVoltageVi2c = fVoltage2;
	pUsbHid->m_fVIH_1 = fVIH_1;

	pUsbHid->m_nTestStep = 0;
	DisplayResultToListEx(pUsbHid, RESULT_PASS, "VOLTAGE%.2fV / %.2fV / %.2fV", pUsbHid->m_fVoltageVcc, pUsbHid->m_fVoltageVi2c, pUsbHid->m_fVIH_1);
#endif
}

void CTestModeView::DisplayMpToolVersion(CUsbHid * pUsbHid)
{
	if (pUsbHid == NULL)
		return;

	pUsbHid->m_nTestStep = 0;
	DisplayResultToListEx(pUsbHid, RESULT_PASS, "VER_%02d", pUsbHid->m_nMpToolVersion);
}

#ifdef _DEBUG
LRESULT CTestModeView::OnMyTestEvent(WPARAM wParam, LPARAM lParam)
{
	CUsbHid * pUsbHid = GetUsbHidByDevIndex(0);
	if (pUsbHid == NULL)
		return 0L;

	static BOOL bInit = FALSE;

	if(bInit == FALSE)
	{
		bInit = TRUE;
		MpTool_PowerControl(pUsbHid, TRUE);
	}

	#if(0)
	static BOOL bOn = false;
	bOn = !bOn;
	pUsbHid->ControlGpioPb15(bOn);
	return 1L;
	#endif

#ifdef D_DEBUG_IUM_ACCESS
	int nIsWrite = (int)wParam;

	pUsbHid->FailLedControl(nIsWrite ? TRUE : FALSE);
	return 0L;

	TSM_SetUpgradeInfo();

	pUsbHid->m_tIumCmdInfo = ZT_GET_IUM_CMD_INFO(pUsbHid->m_CapInfo.icSignature);

	if (nIsWrite)
	{
		//wParam == 1
		//pUsbHid->m_CapInfo.icSignature = 0xE622;
		BOOL bRet = FALSE;

		bRet = TSM_SaveIUM(pUsbHid);

		if(bRet == TRUE)
			MessageBox(_T("TSM_SaveIUM Success!"),_T("Debugging IUM"), MB_OK);
		else
			MessageBox(_T("TSM_SaveIUM Fail!"),_T("Debugging IUM"), MB_OK);
	}
	else
#if(0)	//730 IUM3개 Access 용.
	{
		//wParam == 0
		for (int i = 0; i < 3; i++)
		{
			if (pUsbHid->SetTouchConfigEx(ZT_VCMD(VCMD_IUM_WRITE_PREV), i) != 0)
				break;

			memset((BYTE*)pUsbHid->m_IumData, 0xFF, 128);

			BOOL bRet = pUsbHid->GetTouchRegReadCmd(ZT_VCMD(VCMD_IUM_READ_DATA), (BYTE*)pUsbHid->m_IumData, g_FwDownloadInfo.icIumSize);

			if (bRet == 0)
			{
				CString str;
				str.AppendFormat("IUM_%02d.bin", i);

				CFile file;
				if (file.Open(str, CFile::modeCreate | CFile::modeWrite))
				{
					file.Write(pUsbHid->m_IumData, 128);
					file.Close();
				}
			}
		}
	}
#else
	{
		//기존 사용하던 IUM 1개 128byte Read
		memset((BYTE*)pUsbHid->m_IumData, 0xFF, 128);

		BOOL bRet = pUsbHid->GetTouchRegReadCmd(ZT_VCMD(VCMD_IUM_READ_DATA), (BYTE*)pUsbHid->m_IumData, g_FwDownloadInfo.icIumSize);

		if (bRet == 0)
		{
			CFile file;
			if ( file.Open("IUM.bin", CFile::modeCreate | CFile::modeWrite))
			{
				file.Write(pUsbHid->m_IumData, 128);
				file.Close();
			}
		}
	}
#endif

	MpTool_PowerControl(pUsbHid, FALSE);
#endif

	//GetTouchVoltage(pUsbHid);

	return 1L;
}

/*
// 최초 EEPROM 형태로 write 하는 코드
saveIUM
{
	unsigned short int ium[64];

	BYTE info[8];
	info[0] = 0x69;
	info[1] = 0x3A;
	info[2] = 0x08;
	info[3] = 0x1F;
	info[4] = 0x09;
	info[5] = 0x44;
	info[6] = 0x4A;
	info[7] = 0x53;

	memset(ium, 0x00, sizeof(unsigned short int)*64);

	memcpy(&ium[0x00], info, 8);
	memcpy(&ium[0x20 / 2], info, 8);
	memcpy(&ium[0x40 / 2], info, 8);
	memcpy(&ium[0x60 / 2], info, 8);

	for (int i = 0; i < 128 / 2; i++) // 128byte 모두 클리어 필요. 2byte 통신이기 때문에 /2 필요.
	{
	#if(0)
		if (pUsbHid->SetTouchConfigEx(pUsbHid->m_IumAddress + (i * 2), ium[i]) != 0) {
			goto error_SaveInformation;
		}
	#else
		if (pUsbHid->SetTouchConfigEx(pUsbHid->m_tIumCmdInfo.nIumCmdCustomAddr + (i * 2), ium[i]) != 0) {
			goto error_SaveInformation;
		}
	#endif
	}
}
*/
#endif

//-----------------------------------------------
// Test Result Description

void CTestModeView::TestResultDesc_Initialize()
{
	static BOOL bCreateTabDlg = FALSE;

	if(bCreateTabDlg == FALSE)
	{
		bCreateTabDlg = TRUE;

		CRect rect;
		m_TabCtrl.GetClientRect(&rect);

		// Create tab control
		for(int i=0; i<MAX_DEVICE_COUNT; i++)
		{
			m_TestResultDescDlg[i].Create(static_cast<UINT>(CTestResultDescDlg::IDD), &m_TabCtrl);

			if(i == 0)
				m_TestResultDescDlg[i].SetWindowPos(NULL, 5, 25, rect.Width()-10, rect.Height()-30, SWP_SHOWWINDOW | SWP_NOZORDER);
			else
				m_TestResultDescDlg[i].SetWindowPos(NULL, 5, 25, rect.Width()-10, rect.Height()-30, SWP_HIDEWINDOW | SWP_NOZORDER);
		}

		m_pTabWndShow = TestResultDesc_GetHandle(0);

		m_TabCtrl.SetDimDataFunc(&TestResultDesc_GetListState);
	}

	m_TabCtrl.DeleteAllItems();

	// Insert tab
	if(m_bUseMultiDevice == FALSE || INDIVIDUAL_TEST_START_OFF)
	{
		m_TabCtrl.InsertItem(0, _T("Test Result Description"), 0);
	}
	else
	{
		if(m_nMultiDeviceCount > MAX_DEVICE_COUNT)
			m_nMultiDeviceCount = MAX_DEVICE_COUNT;

		for(int i=0; i<m_nMultiDeviceCount; i++)
		{
			CString str;
			str.Format("DEVICE %d", i);
			m_TabCtrl.InsertItem(i, str, 0);
		}
	}

	TestResultDesc_ClearAll();

	TestResultDesc_SetCurSel(-1);

	if( m_pTabWndShow != NULL )
	{
		((CTestResultDescDlg*)m_pTabWndShow)->ShowDialog(SW_HIDE);

		m_pTabWndShow = TestResultDesc_GetHandle(m_TabCtrl.GetCurSel());

		if(m_pTabWndShow)
			((CTestResultDescDlg*)m_pTabWndShow)->ShowDialog(SW_SHOW);
	}

	((CTestModeView*)g_pTestModeViewWnd)->PostMessageEx(WM_USER_SET_FOCUS_TO_LIST, 0, 0);
}

void CTestModeView::TestResultDesc_SetCurSel(int nIndex)
{
	int nPrevIndex = m_TabCtrl.GetCurSel();

	if(nIndex == -1)
	{
		CUsbHid * pUsbHid = NULL;
		int nTotalDeviceCount = GetMaxDeviceCount();

		nIndex = 0;

		for (int n = 0; n < nTotalDeviceCount; n++)
		{
			pUsbHid = GetUsbHidByDevIndex(n);

			if (pUsbHid && pUsbHid->IsTestAvailable())
			{
				nIndex = n;
				break;
			}
		}
	}

	SemaWait(g_hInfoSemaphore);
	m_TabCtrl.SetCurSel(nIndex);
	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);

	if(nPrevIndex == nIndex || INDIVIDUAL_TEST_START_OFF)
	{
		m_TabCtrl.EnableDraw(BTC_TABS);
	}

	// send TCN_SELCHANGE message to IDC_TAB_TEST_RESULT_DESC
	NMHDR nmhdr;
	nmhdr.code = TCN_SELCHANGE;
	nmhdr.idFrom = IDC_TAB_TEST_RESULT_DESC;
	nmhdr.hwndFrom = m_TabCtrl.m_hWnd;
	SendMessage(WM_NOTIFY, IDC_TAB_TEST_RESULT_DESC, (LPARAM)&nmhdr);
}

CTestResultDescDlg * CTestModeView::TestResultDesc_GetHandle(int nDevIndex)
{
	if(nDevIndex < 0 || nDevIndex >= MAX_DEVICE_COUNT)
		return NULL;

	return &m_TestResultDescDlg[nDevIndex];
}

// 탭 컨트롤에서 탭 변경 이벤트 핸들러
void CTestModeView::TestResultDesc_OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int tabIndex = m_TabCtrl.GetCurSel();

	CWnd * pCurWnd = TestResultDesc_GetHandle(tabIndex);

	if(pCurWnd != m_pTabWndShow)
	{
		if( m_pTabWndShow != NULL )
		{
			((CTestResultDescDlg*)m_pTabWndShow)->ShowDialog(SW_HIDE);
			m_pTabWndShow = NULL;
		}

		if(pCurWnd)
		{
			((CTestResultDescDlg*)pCurWnd)->ShowDialog(SW_SHOW);

			m_pTabWndShow = pCurWnd;
		}
	}

	*pResult = 0;
}

// Returns TRUE to prevent the selection from changing, or FALSE to allow the selection to change.
void CTestModeView::TestResultDesc_OnSelchangingTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	TCHITTESTINFO ti;
	GetCursorPos(&ti.pt);
	m_TabCtrl.ScreenToClient(&ti.pt);
	int new_index = m_TabCtrl.HitTest(&ti);

	//TRACE(_T("TCN_SELCHANGING [%d -> %d]\n"), m_TabCtrl.GetCurSel(), new_index);

	if(TestResultDesc_GetListState(new_index) == ITEM_STATE_DISABLE)
	{
		TRACE(_T("IGNORE Index %d\n"), new_index);

		*pResult = 1;
		return;
	}

	// *pResult가 0값이면 탭이 활성화, 1값이면 탭이 비활성화된다.
	*pResult = 0;
}

int CTestModeView::TestResultDesc_GetListState(int nDevIndex)
{
	CTestModeView * pTestModeView = (CTestModeView*)m_pUsbHidList->GetParent();
	CUsbHid * pUsbHid = NULL;

	if(pTestModeView->m_bUseStartIndividualTest == FALSE)
	{
		// Find first device
		int nDeviceCount = pTestModeView->m_TestDevIndex.GetCount();
		int nIndex;

		for(int i=0; i<nDeviceCount; i++)
		{
			nIndex = pTestModeView->m_TestDevIndex.GetAt(i);
			pUsbHid = m_pUsbHidList->GetUsbHidPtrByDevIndex(nDevIndex);
			if (pUsbHid)
			{
				nDevIndex = nIndex;
				break;
			}
		}
	}

	pUsbHid = m_pUsbHidList->GetUsbHidPtrByDevIndex(nDevIndex);
	if (pUsbHid == NULL)
		return ITEM_STATE_DISABLE;

	return pUsbHid->m_nListState;
}

void CTestModeView::TestResultDesc_ClearAll()
{
	for(int i=0; i<MAX_DEVICE_COUNT; i++)
	{
		m_TestResultDescDlg[i].ClearMessage();
	}
}

LRESULT CTestModeView::OnSetFocusToList(WPARAM wParam, LPARAM lParam)
{
	CWnd* pCWnd_temp = GetFocus();
	if (pCWnd_temp != NULL && IsWindow(pCWnd_temp->m_hWnd))
	{
		if (IDC_LIST_INFO != pCWnd_temp->GetDlgCtrlID())
		{
			GetDlgItem(IDC_LIST_INFO)->SetFocus();
		}
	}
	return 0;
}
