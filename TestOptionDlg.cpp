// TestOptionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "TestOptionDlg.h"

#include "TestModeView.h"

s16 CTestOptionDlg::m_icRegData_TotalNumOfX;
s16 CTestOptionDlg::m_icRegData_TotalNumOfY;

s16 CTestOptionDlg::m_icRegData_SelfNumOfX;
s16 CTestOptionDlg::m_icRegData_SelfNumOfY;

u8 CTestOptionDlg::m_IC_Max_X_Line_Num;
u8 CTestOptionDlg::m_IC_Max_Y_Line_Num;

// default
BOOL CTestOptionDlg::m_bUseIFE;
BOOL CTestOptionDlg::m_bUseRetry;
BOOL CTestOptionDlg::m_bUseMptoolver;


int CTestOptionDlg::m_nIcType;
int CTestOptionDlg::m_nSlaveAddress;

BOOL CTestOptionDlg::m_bSaveResult;
BOOL CTestOptionDlg::m_bSaveFixedPosition;
BOOL CTestOptionDlg::m_bCreatePassFailDirectory;
CString CTestOptionDlg::m_strSaveResultDir;

int CTestOptionDlg::m_nStandBySec;

// download
BOOL CTestOptionDlg::m_bDownload;
BOOL CTestOptionDlg::m_bDownloadFixedPosition;
int CTestOptionDlg::m_nDownloadRetryCount;
int CTestOptionDlg::m_nAllRetryCount;
int CTestOptionDlg::m_nMptoolCount;

CString CTestOptionDlg::m_strFirmwareDir;
BOOL CTestOptionDlg::m_bVerifyFirmware;
BOOL CTestOptionDlg::m_bBurstUpgrade;

BOOL CTestOptionDlg::m_bSaveInfo;
int CTestOptionDlg::m_nSaveInfoLocation;

CString CTestOptionDlg::m_strProjectID;
CString CTestOptionDlg::m_strDeviceID;

int CTestOptionDlg::m_nIUMFormat;
int CTestOptionDlg::m_nI2CFormat;

// check pin
BOOL CTestOptionDlg::m_bCheckIntPin;
BOOL CTestOptionDlg::m_bCheckReset;
BOOL CTestOptionDlg::m_bCheckResetPin;

// Check Version
BOOL CTestOptionDlg::m_bCheckVersion;
BOOL CTestOptionDlg::m_bCheckAutoFwDownload;
CString CTestOptionDlg::m_strIcRevision;
CString CTestOptionDlg::m_strHwID;
CString CTestOptionDlg::m_strModuleID;
CString CTestOptionDlg::m_strMajorVersion;
CString CTestOptionDlg::m_strMinorVersion;
CString CTestOptionDlg::m_strRegisterVersion;
BOOL CTestOptionDlg::m_bCheckIUM;

//CHECK I2C LVL
BOOL CTestOptionDlg::m_bCheckI2C_Level;

//barcode
BOOL CTestOptionDlg::m_bUseBarcode;
BOOL CTestOptionDlg::m_bBarcodeDisplayInTest;
BOOL CTestOptionDlg::m_bAutoStartTest;
BOOL CTestOptionDlg::m_bFixedBarcodeLength;
int CTestOptionDlg::m_nBarcode_Length;
int CTestOptionDlg::m_nBarcode_AutoReset;

CString CTestOptionDlg::m_strCustomer;
CString CTestOptionDlg::m_strProjectName;

CString CTestOptionDlg::m_strUserAdminPassword;
BOOL CTestOptionDlg::m_bUseStartup;
CString CTestOptionDlg::m_strStartupPassword;
CString CTestOptionDlg::m_strZtdExeDir;

BOOL CTestOptionDlg::m_bAdditionalLOG;
CString CTestOptionDlg::m_strTestName;
CString CTestOptionDlg::m_strTestStage;
CString CTestOptionDlg::m_strModuleMaker;
int CTestOptionDlg::m_nInOutType;

// TEST INFO
TEST_INFO_DND CTestOptionDlg::m_TestInfo_DND;
TEST_INFO_DND CTestOptionDlg::m_TestInfo_HF_DND;

TEST_INFO_CND CTestOptionDlg::m_TestInfo_CND;							//add 2022.11.14 for ZTM730 CND

TEST_INFO_HIGH_VOLTAGE CTestOptionDlg::m_TestInfo_HIGH_VOLTAGE;

TEST_INFO_SELF CTestOptionDlg::m_TestInfo_SELF_DND;
TEST_INFO_ONLYSELF CTestOptionDlg::m_TestInfo_ONLYSELF_DND;				//add 2022.09.16
TEST_INFO_SELF CTestOptionDlg::m_TestInfo_SELF_SATURATION;

TEST_INFO_MEASURE_DELTA CTestOptionDlg::m_TestInfo_MEASURE_DELTA;
TEST_INFO_HW_CALIBRATION CTestOptionDlg::m_TestInfo_HW_CALIBRATION;
TEST_INFO_JITTER CTestOptionDlg::m_TestInfo_JITTER;
TEST_INFO_CURRENT_CONSUMPTION CTestOptionDlg::m_TestInfo_CURRENT_CONSUMPTION;
TEST_INFO_DIGITAL_WHEEL	CTestOptionDlg::m_TestInfo_DIGITAL_WHEEL;
TEST_INFO_NOISE CTestOptionDlg::m_TestInfo_NOISE;

BOOL CTestOptionDlg::m_bCheckTrxShort;

BOOL CTestOptionDlg::m_bUseMultiDevice;
int CTestOptionDlg::m_nMultiDeviceCount;
BOOL CTestOptionDlg::m_bUseStartIndividualTest;

BOOL CTestOptionDlg::m_bCheckUserNumber;

BOOL CTestOptionDlg::m_bHidMode;

#if defined(D_USE_CHECK_BUTTON_TEST)
BOOL CTestOptionDlg::m_bCheckGpioButton;

BOOL CTestOptionDlg::m_bCheckTouchButton;
int CTestOptionDlg::m_nTouchButton_Threshold;
#endif

BOOL CTestOptionDlg:: m_bCheckVirtualDrawTest;

// CTestOptionDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTestOptionDlg, CDialog)

CTestOptionDlg::CTestOptionDlg(CWnd* pParent /*=NULL*/)
	: CScrollDlg(CTestOptionDlg::IDD, pParent)
{
	// default
	m_bUseIFE = TRUE;
	m_nSlaveAddress = 0;
	m_nStandBySec = 0;
	m_bSaveResult = FALSE;
	m_bSaveFixedPosition = FALSE;
	m_bCreatePassFailDirectory = FALSE;
	m_strSaveResultDir = _T("");
	m_bDownload = FALSE;
	m_bDownloadFixedPosition = FALSE;
	m_nDownloadRetryCount = 0;
	m_bUseRetry = FALSE;
	m_bUseMptoolver = FALSE;
	m_nAllRetryCount = 0;
	m_nMptoolCount = 0;
	m_strFirmwareDir = _T("");
	m_bVerifyFirmware = FALSE;
	m_bBurstUpgrade = FALSE;
	m_bSaveInfo = FALSE;
	m_nSaveInfoLocation = SAVE_INFO_EEPROM;
	m_bCheckIntPin = FALSE;
	m_bCheckReset = FALSE;
	m_bCheckResetPin = FALSE;
	m_bCheckVersion = FALSE;
	
	m_bCheckI2C_Level = FALSE;
	m_nI2CFormat = 0;
	
	m_bCheckAutoFwDownload = FALSE;
	m_bCheckIUM = FALSE;
	m_strIcRevision = _T("");
	m_strHwID = _T("");
	m_strModuleID = _T("");
	m_strMajorVersion = _T("");
	m_strMinorVersion = _T("");
	m_strRegisterVersion = _T("");
	m_bUseBarcode = FALSE;
	m_bBarcodeDisplayInTest = FALSE;
	m_bAutoStartTest = FALSE;
	m_bFixedBarcodeLength = FALSE;
	m_nBarcode_Length = 0;
	m_nBarcode_AutoReset = 0;
	m_strCustomer = _T("");
	m_strProjectName = _T("");
	m_bUseMultiDevice = FALSE;
	m_nMultiDeviceCount = MAX_DEVICE_COUNT;
	m_bUseStartIndividualTest = FALSE;
	m_bCheckUserNumber = FALSE;
	m_bHidMode = FALSE;
	m_strUserAdminPassword = _T("");
	m_bUseStartup = FALSE;
	m_strStartupPassword = _T("");
#if defined(D_USE_CHECK_BUTTON_TEST)
	m_bCheckGpioButton = FALSE;

	m_bCheckTouchButton = FALSE;
	m_nTouchButton_Threshold = 0;
#endif

	m_bAdditionalLOG = FALSE;
	m_strTestName = _T("");
	m_strTestStage = _T("");
	m_strModuleMaker = _T("");

	m_bCheckVirtualDrawTest = FALSE;

	m_nInOutType = 0;

	m_strProjectID = _T("");
	m_strDeviceID = _T("");

	// TEST INFO
	memset(&m_TestInfo_DND, 0x00, sizeof(TEST_INFO_DND));
	memset(&m_TestInfo_HF_DND, 0x00, sizeof(TEST_INFO_DND));

	memset(&m_TestInfo_CND, 0x00, sizeof(TEST_INFO_CND));

	memset(&m_TestInfo_HIGH_VOLTAGE, 0x00, sizeof(TEST_INFO_HIGH_VOLTAGE));

	memset(&m_TestInfo_SELF_DND, 0x00, sizeof(TEST_INFO_SELF));
	memset(&m_TestInfo_SELF_SATURATION, 0x00, sizeof(TEST_INFO_SELF));
	memset(&m_TestInfo_ONLYSELF_DND, 0x00, sizeof(TEST_INFO_ONLYSELF));

	memset(&m_TestInfo_MEASURE_DELTA, 0x00, sizeof(TEST_INFO_MEASURE_DELTA));
	memset(&m_TestInfo_HW_CALIBRATION, 0x00, sizeof(TEST_INFO_HW_CALIBRATION));
	memset(&m_TestInfo_JITTER, 0x00, sizeof(TEST_INFO_JITTER));
	memset(&m_TestInfo_CURRENT_CONSUMPTION, 0x00, sizeof(TEST_INFO_CURRENT_CONSUMPTION));
	memset(&m_TestInfo_DIGITAL_WHEEL, 0x00, sizeof(TEST_INFO_DIGITAL_WHEEL));
	memset(&m_TestInfo_NOISE, 0x00, sizeof(TEST_INFO_NOISE));

	m_bCheckTrxShort = FALSE;
}

CTestOptionDlg::~CTestOptionDlg()
{
}

void CTestOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CScrollDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_TAB_OPTION, m_TabCtrl);
}


BEGIN_MESSAGE_MAP(CTestOptionDlg, CScrollDlg)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_OPTION, OnTcnSelchangeTabCtrl)
	END_MESSAGE_MAP()


// CTestOptionDlg 메시지 처리기입니다.

BOOL CTestOptionDlg::OnInitDialog()
{
	CString strTemp;

	CScrollDlg::OnInitDialog();

	m_strZtdExeDir = GetCurrentAppDirectory();

	m_btnOK.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	m_btnOK.SetIcon(IDI_ICON_OK, IDI_ICON_OK_GRAY);
	m_btnOK.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(125, 184, 56));
	m_btnOK.SetBtnCursor(IDC_CURSOR_HAND);

	m_btnCancel.SetShade(CShadeButtonST::SHS_HARDBUMP);
	m_btnCancel.SetIcon(IDI_ICON_CANCEL, IDI_ICON_CANCEL_GRAY);
	m_btnCancel.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(240, 26, 40));
	m_btnCancel.SetBtnCursor(IDC_CURSOR_HAND);

	m_TabCtrl.InsertItem(0, _T("    Default    "));
	m_TabCtrl.InsertItem(1, _T("    SPEC-1    "));
	m_TabCtrl.InsertItem(2, _T("    SPEC-2    "));

	CRect rect;
	m_TabCtrl.GetClientRect(&rect);

	m_Tab0.Create(IDD_TEST_OPTION_TAB_0, &m_TabCtrl);
	m_Tab0.SetWindowPos(NULL, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_SHOWWINDOW | SWP_NOZORDER);

	m_Tab1.Create(IDD_TEST_OPTION_TAB_1, &m_TabCtrl);
	m_Tab1.SetWindowPos(NULL, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_HIDEWINDOW | SWP_NOZORDER);

	m_Tab2.Create(IDD_TEST_OPTION_TAB_2, &m_TabCtrl);
	m_Tab2.SetWindowPos(NULL, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_HIDEWINDOW | SWP_NOZORDER);

	m_pTabWndShow = &m_Tab0;

	// dialog size
	GetClientRect(&rect);

	long cx, cy;
	cy = ::GetSystemMetrics(SM_CYSCREEN);
	cx = ::GetSystemMetrics(SM_CXSCREEN);

	if ((rect.Width() > cx) || (rect.Height() > cy))
	{
		if (cx <= 1000 || cy <= 800) {
			cy = cy * 9 / 10;
			cx = cx * 9 / 10;

		}
		else {
			cy = cy * 2 / 3;
			cx = cx * 3 / 4;
		}

		this->MoveWindow(0, 0, cx, cy);
	}

#if defined(D_USE_CHECK_BUTTON_TEST)
	if(CTestOptionDlg::m_bUseStartIndividualTest)
	{
		CTestOptionDlg::m_bCheckGpioButton = FALSE;
		CTestOptionDlg::m_bCheckTouchButton = FALSE;
	}
#endif

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CTestOptionDlg::CheckIfeException()
{
	if (CTestOptionDlg::m_bUseIFE == FALSE)
	{
		CTestModeView * pTestModeView = (CTestModeView*)g_pTestModeViewWnd;
		if (pTestModeView == NULL)
			return;

		if (pTestModeView->GetModuleVendorCode() != MODULE_VENDOR_TRULY)
			CTestOptionDlg::m_bAdditionalLOG = FALSE;

		CTestOptionDlg::m_TestInfo_SELF_SATURATION.bCheckData = FALSE;
		CTestOptionDlg::m_TestInfo_HW_CALIBRATION.bCheckData = FALSE;

		if(m_TestInfo_JITTER.bCheckDataByMpztd == FALSE)		//add 2021.9.17
			CTestOptionDlg::m_TestInfo_JITTER.bCheckData = FALSE;

		CTestOptionDlg::m_TestInfo_HIGH_VOLTAGE.bCheckData = FALSE;
	}
}

void CTestOptionDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	m_Tab0.UpdateData(TRUE);
	m_Tab1.UpdateData(TRUE);
	m_Tab2.UpdateData(TRUE);

	if( m_bUseStartup && m_strStartupPassword.IsEmpty() )
	{
		AfxMessageBox("Please. Check the STARTUP PASSWORD.");
		return;
	}

	if( m_nBarcode_Length > 255 )
	{
		AfxMessageBox("Please. Enter an barcode length between 1 and 255");
		return;
	}

	// Measure Delta
	if (m_TestInfo_MEASURE_DELTA.bCheckData == 1 &&
		(m_TestInfo_MEASURE_DELTA.nCount <= 0 || m_TestInfo_MEASURE_DELTA.nCount > MAX_MEASURE_CNT))
	{
		AfxMessageBox("Setting value is not valid (Measure delta frame count)");
		return;
	}
	else if( m_TestInfo_CURRENT_CONSUMPTION.Active.nAvgMs > 1000 )
	{
		AfxMessageBox("Please. Enter an current consumption delay time\nbetween 1 and 1000");
		return;
	}

	// Project ID
	if (m_strProjectID.IsEmpty())
		m_strProjectID = _T("0000");

	if (m_strProjectID.GetLength() != 4 ||
		CheckHexValue(m_strProjectID) == FALSE)
	{
		AfxMessageBox("Please. Check the PROJECT ID.");
		return;
	}

	// Device ID
	if (m_strDeviceID.IsEmpty())
		m_strDeviceID = _T("0000000000000000");

	if (m_strDeviceID.GetLength() != 16 ||
		CheckHexValue(m_strDeviceID) == FALSE)
	{
		AfxMessageBox("Please. Check the DEVICE ID.");
		return;
	}

	// unsupported test item in CHINA
	CheckIfeException();

	if( m_nMultiDeviceCount <= 0 )
		m_nMultiDeviceCount = 1;
	else if( m_nMultiDeviceCount >= MAX_DEVICE_COUNT )
		m_nMultiDeviceCount = MAX_DEVICE_COUNT;

	if(m_bUseMultiDevice == FALSE)
		m_bUseStartIndividualTest = FALSE;

	if(m_bUseStartIndividualTest)
	{
		m_bCheckGpioButton = FALSE;
		m_bCheckTouchButton = FALSE;
	}

	m_Tab0.GetControlValue();

	OnOK();
}

BOOL CTestOptionDlg::CheckHexValue(CString str)
{
	BOOL bValid = TRUE;

	int nCount = str.GetLength();
	char szTemp[24];

	sprintf(szTemp, "%s", str.GetBuffer());

	for (int i = 0; i < nCount; i++)
	{
		if ( ! ((szTemp[i]>='0' && szTemp[i]<='9') ||
				(szTemp[i]>='A' && szTemp[i]<='F') ||
				(szTemp[i]>='a' && szTemp[i]<='f')) )
		{
			bValid = FALSE;
			break;
		}
	}

	return bValid;
}

void CTestOptionDlg::OnBnClickedCancel()
{
	CScrollDlg::OnCancel();
}

void CTestOptionDlg::OnTcnSelchangeTabCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_pTabWndShow != NULL)
	{
		m_pTabWndShow->ShowWindow(SW_HIDE);
		m_pTabWndShow = NULL;
	}

	int tabIndex = m_TabCtrl.GetCurSel();

	switch (tabIndex)
	{
	case 0:
		m_Tab0.UpdateData(FALSE);
		m_Tab0.EnableDialogControls(0xFFFFFFFF);

		m_Tab0.ShowWindow(SW_SHOW);
		m_pTabWndShow = &m_Tab0;
		break;

	case 1:
		m_Tab1.UpdateData(FALSE);
		m_Tab1.EnableDialogControls(0xFFFFFFFF);

		m_Tab1.ShowWindow(SW_SHOW);
		m_pTabWndShow = &m_Tab1;
		break;

	case 2:
		m_Tab2.UpdateData(FALSE);
		m_Tab2.EnableDialogControls(0xFFFFFFFF);

		m_Tab2.ShowWindow(SW_SHOW);
		m_pTabWndShow = &m_Tab2;
		break;
	}

	*pResult = 0;
}

void CTestOptionDlg::OnBnClickedButtonResetCount()
{
	CTestModeView * pParent = (CTestModeView*)m_pParentWnd;

	pParent->ResetTestCount();
}

