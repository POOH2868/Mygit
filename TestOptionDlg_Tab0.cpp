// DlgTab4.cpp : implementation file
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "TestModeView.h"

#include "TestOptionDlg.h"
#include "TestOptionDlg_Tab0.h"

#include "util.h"

enum
{
	CTRL_UPDATE_IFE			 	= (1 << 0),
	CTRL_UPDATE_SAVE_RESULT 	= (1 << 1),
	CTRL_UPDATE_ADD_LOG 		= (1 << 2),
	CTRL_UPDATE_DOWNLOAD		= (1 << 3),
	CTRL_UPDATE_CHECK_VERSION 	= (1 << 4),
	CTRL_UPDATE_MULTI_DEVICE	= (1 << 5),
	CTRL_UPDATE_USE_STARTUP 	= (1 << 6),
	CTRL_UPDATE_CHECK_RESET 	= (1 << 7),
	CTRL_UPDATE_CHECK_RETRY 	= (1 << 8),
	CTRL_UPDATE_CHECK_MPTOOL_VER= (1 << 9),
	CTRL_UPDATE_CHECK_I2C_LEVEL = (1 << 10)
};

// CTestOptionDlg_Tab0 dialog

IMPLEMENT_DYNAMIC(CTestOptionDlg_Tab0, CDialog)

CTestOptionDlg_Tab0::CTestOptionDlg_Tab0(CWnd* pParent /*=NULL*/)
	: CDialog(CTestOptionDlg_Tab0::IDD, pParent)
{

}

CTestOptionDlg_Tab0::~CTestOptionDlg_Tab0()
{

}

void CTestOptionDlg_Tab0::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_USE_IFE, CTestOptionDlg::m_bUseIFE);
	DDX_Check(pDX, IDC_CHECK_USE_COUNT, CTestOptionDlg::m_bUseRetry);
	DDX_Check(pDX, IDC_CHECK_USE_MPTOOL_VER, CTestOptionDlg::m_bUseMptoolver);

	DDX_Radio(pDX, IDC_RADIO_IC_TYPE_1, CTestOptionDlg::m_nIcType);
	DDX_Radio(pDX, IDC_RADIO_SLAVE_ADDR_1, CTestOptionDlg::m_nSlaveAddress);
	DDX_Check(pDX, IDC_CHECK_SAVE_RESULT, CTestOptionDlg::m_bSaveResult);
	DDX_Check(pDX, IDC_CHECK_FIXED_POSITION, CTestOptionDlg::m_bSaveFixedPosition);
	DDX_Check(pDX, IDC_CHECK_CREATE_PASS_FAIL_DIR, CTestOptionDlg::m_bCreatePassFailDirectory);
	DDX_Text(pDX, IDC_EDIT_SAVE_RESULT_DIR, CTestOptionDlg::m_strSaveResultDir);
	DDX_Check(pDX, IDC_CHECK_DOWNLOAD_FIRMWARE, CTestOptionDlg::m_bDownload);
	DDX_Check(pDX, IDC_CHECK_DOWNLOAD_FIXED_POSITION, CTestOptionDlg::m_bDownloadFixedPosition);
	DDX_Text(pDX, IDC_EDIT_DOWNLOAD_RETRY_COUNT, CTestOptionDlg::m_nDownloadRetryCount);

	DDX_Text(pDX, IDC_ALL_RETRY_COUNT, CTestOptionDlg::m_nAllRetryCount);
	DDX_Text(pDX, IDC_EDIT_MPTOOL_VER, CTestOptionDlg::m_nMptoolCount);

	DDX_Check(pDX, IDC_CHECK_VERIFY_FIRMWARE, CTestOptionDlg::m_bVerifyFirmware);
	DDX_Check(pDX, IDC_CHECK_BURST_UPGRADE, CTestOptionDlg::m_bBurstUpgrade);
	DDX_Check(pDX, IDC_CHECK_SAVE_INFO, CTestOptionDlg::m_bSaveInfo);
	DDX_Check(pDX, IDC_CHECK_INTERRUPT_PIN, CTestOptionDlg::m_bCheckIntPin);
	DDX_Check(pDX, IDC_CHECK_RESET, CTestOptionDlg::m_bCheckReset);
	DDX_Check(pDX, IDC_CHECK_RESET_PIN, CTestOptionDlg::m_bCheckResetPin);
	DDX_Check(pDX, IDC_CHECK_VERSION, CTestOptionDlg::m_bCheckVersion);
	DDX_Check(pDX, IDC_CHECK_AUTO_FW_DOWNLOAD, CTestOptionDlg::m_bCheckAutoFwDownload);
	DDX_Text(pDX, IDC_EDIT_IC_REVISION, CTestOptionDlg::m_strIcRevision);
	DDX_Text(pDX, IDC_EDIT_HW_ID, CTestOptionDlg::m_strHwID);
	DDX_Text(pDX, IDC_EDIT_MAJOR_VERSION, CTestOptionDlg::m_strMajorVersion);
	DDX_Text(pDX, IDC_EDIT_MINOR_VERSION, CTestOptionDlg::m_strMinorVersion);
	DDX_Text(pDX, IDC_EDIT_REGISTER_VERSION, CTestOptionDlg::m_strRegisterVersion);
	DDX_Text(pDX, IDC_EDIT_CUSTOMER, CTestOptionDlg::m_strCustomer);
	DDX_Text(pDX, IDC_EDIT_PROJECT_NAME, CTestOptionDlg::m_strProjectName);
	DDX_Check(pDX, IDC_CHECK_MULTI_DEVICE, CTestOptionDlg::m_bUseMultiDevice);
	DDX_Text(pDX, IDC_EDIT_MAX_DEVICE_COUNT, CTestOptionDlg::m_nMultiDeviceCount);
	DDX_Check(pDX, IDC_CHECK_START_INDIVIDUAL_TEST, CTestOptionDlg::m_bUseStartIndividualTest);
	DDX_Check(pDX, IDC_CHECK_USER_NUMBER, CTestOptionDlg::m_bCheckUserNumber);
	DDX_Text(pDX, IDC_EDIT_STANDBY, CTestOptionDlg::m_nStandBySec);

	//IUM
	DDX_Check(pDX, IDC_CHECK_VERSION_IUM, CTestOptionDlg::m_bCheckIUM);

	// password
	DDX_Text(pDX, IDC_EDIT_USER_ADMIN_PASSWORD, CTestOptionDlg::m_strUserAdminPassword);
	DDX_Check(pDX, IDC_CHECK_STARTUP, CTestOptionDlg::m_bUseStartup);
	DDX_Text(pDX, IDC_EDIT_STARTUP_PASSWORD, CTestOptionDlg::m_strStartupPassword);

// by Paul {{{
	DDX_Check(pDX, IDC_CHECK_ADD_LOG, CTestOptionDlg::m_bAdditionalLOG);
	DDX_Text(pDX, IDC_EDIT_TEST_NAME,		CTestOptionDlg::m_strTestName);
	DDX_Text(pDX, IDC_EDIT_TEST_STAGE,		CTestOptionDlg::m_strTestStage);
	DDX_Text(pDX, IDC_EDIT_MODULE_MAKER,	CTestOptionDlg::m_strModuleMaker);
	DDX_Radio(pDX, IDC_RADIO_INOUT_TYPE_1, CTestOptionDlg::m_nInOutType);
// }}}

	DDX_Text(pDX, IDC_EDIT_DOWNLOAD_FIRMWARE_DIR, CTestOptionDlg::m_strFirmwareDir);
	DDX_Control(pDX, IDC_COMBO_MODULE_ID, CTestOptionDlg_Tab0::m_ComboModuleID);
	DDX_Control(pDX, IDC_COMBO_IUM_FORMAT, CTestOptionDlg_Tab0::m_ComboIumFormat);

	DDX_Control(pDX, IDC_COMBO_I2C_FORMAT, CTestOptionDlg_Tab0::m_ComboI2CFormat);

	DDX_Control(pDX, IDC_BUTTON_RESET_COUNT, m_btnResetCount);
	DDX_Control(pDX, IDC_BUTTON_SELECT_DOWNLOAD_FIRMWARE_DIR, m_btnDownloadBrowse);
	DDX_Control(pDX, IDC_BUTTON_SELECT_SAVE_RESULT_DIR, m_btnSaveResultBrowse);

	DDX_Text(pDX, IDC_EDIT_PROJECT_ID,	CTestOptionDlg::m_strProjectID);
	DDX_Text(pDX, IDC_EDIT_DEVICE_ID,	CTestOptionDlg::m_strDeviceID);

	DDX_Check(pDX, IDC_CHECK_I2C_LEVEL, CTestOptionDlg::m_bCheckI2C_Level);
}


BEGIN_MESSAGE_MAP(CTestOptionDlg_Tab0, CDialog)
	ON_WM_CTLCOLOR()


	ON_BN_CLICKED(IDC_BUTTON_RESET_COUNT, &CTestOptionDlg::OnBnClickedButtonResetCount)

	ON_BN_CLICKED(IDC_BUTTON_SELECT_SAVE_RESULT_DIR, &CTestOptionDlg_Tab0::OnBnClickedButtonSelectSaveResultDir)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_DOWNLOAD_FIRMWARE_DIR, &CTestOptionDlg_Tab0::OnBnClickedButtonSelectFirwareDir)

	ON_BN_CLICKED(IDC_CHECK_USE_IFE, &CTestOptionDlg_Tab0::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_USE_COUNT, &CTestOptionDlg_Tab0::OnCheckBnClicked)

	ON_BN_CLICKED(IDC_CHECK_USE_MPTOOL_VER, &CTestOptionDlg_Tab0::OnCheckBnClicked)

	ON_BN_CLICKED(IDC_CHECK_MULTI_DEVICE, &CTestOptionDlg_Tab0::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_START_INDIVIDUAL_TEST, &CTestOptionDlg_Tab0::OnCheckBnClicked)

	ON_BN_CLICKED(IDC_CHECK_USER_NUMBER, &CTestOptionDlg_Tab0::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_ADD_LOG, &CTestOptionDlg_Tab0::OnCheckBnClicked)

	ON_BN_CLICKED(IDC_CHECK_DOWNLOAD_FIRMWARE, &CTestOptionDlg_Tab0::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_DOWNLOAD_FIXED_POSITION, &CTestOptionDlg_Tab0::OnBnClickedButtonDownloadFixedPosition)
	ON_BN_CLICKED(IDC_CHECK_SAVE_INFO, &CTestOptionDlg_Tab0::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_VERIFY_FIRMWARE, &CTestOptionDlg_Tab0::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_BURST_UPGRADE, &CTestOptionDlg_Tab0::OnCheckBnClicked)

	ON_BN_CLICKED(IDC_CHECK_VERSION, &CTestOptionDlg_Tab0::OnCheckBnClicked)
	
	ON_BN_CLICKED(IDC_CHECK_I2C_LEVEL, &CTestOptionDlg_Tab0::OnCheckBnClicked)
	
	ON_BN_CLICKED(IDC_CHECK_VERSION_IUM, &CTestOptionDlg_Tab0::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_AUTO_FW_DOWNLOAD, &CTestOptionDlg_Tab0::OnCheckBnClicked)

	ON_BN_CLICKED(IDC_CHECK_INTERRUPT_PIN, &CTestOptionDlg_Tab0::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_RESET, &CTestOptionDlg_Tab0::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_RESET_PIN, &CTestOptionDlg_Tab0::OnCheckBnClicked)

	ON_BN_CLICKED(IDC_CHECK_SAVE_RESULT, &CTestOptionDlg_Tab0::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_CREATE_PASS_FAIL_DIR, &CTestOptionDlg_Tab0::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_STARTUP, &CTestOptionDlg_Tab0::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_FIXED_POSITION, &CTestOptionDlg_Tab0::OnBnClickedButtonLogFixedPosition)

	ON_COMMAND_RANGE(IDC_RADIO_IC_TYPE_1, IDC_RADIO_IC_TYPE_14, &CTestOptionDlg_Tab0::OnRadioBnClicked)
	ON_COMMAND_RANGE(IDC_RADIO_SLAVE_ADDR_1, IDC_RADIO_SLAVE_ADDR_2, &CTestOptionDlg_Tab0::OnRadioBnClicked)
	ON_COMMAND_RANGE(IDC_RADIO_INOUT_TYPE_1, IDC_RADIO_INOUT_TYPE_2, &CTestOptionDlg_Tab0::OnRadioBnClicked)

	ON_CBN_SELCHANGE(IDC_COMBO_SAVE_INFO_LOCATION, &CTestOptionDlg_Tab0::OnCbnSelchangeComboSaveInfoLocation)
END_MESSAGE_MAP()

// CTestOptionDlg_Tab0 message handlers

BOOL CTestOptionDlg_Tab0::PreTranslateMessage(MSG* pMsg)
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

BOOL CTestOptionDlg_Tab0::OnInitDialog()
{
	CDialog::OnInitDialog();

	// module ID
	int nModuleID = char2dec((char*)CTestOptionDlg::m_strModuleID.GetBuffer());

	int nCount = g_strModuleIdList.GetCount();
	for (int i = 0; i < nCount; i++)
	{
		m_ComboModuleID.InsertString(m_ComboModuleID.GetCount(), g_strModuleIdList.GetAt(i));

		if (nModuleID == g_nModuleIdList.GetAt(i))
		{
			m_ComboModuleID.SetCurSel(i);
		}
	}

	m_ComboIumFormat.SetCurSel(CTestOptionDlg::m_nIUMFormat);
	m_ComboI2CFormat.SetCurSel(CTestOptionDlg::m_nI2CFormat);

	((CComboBox*)GetDlgItem(IDC_COMBO_SAVE_INFO_LOCATION))->SetCurSel(CTestOptionDlg::m_nSaveInfoLocation);

	EnableDialogControls(0xFFFFFFFF);

	InitUiContorl();

	m_nIcType_old = CTestOptionDlg::m_nIcType;


	fnIUM_Enable_Check();

	//2021.11.17 김극래 책임 VISIONOX 일경우 Log Namming rule 변경.
	fnCheck_VendorVisinox();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CTestOptionDlg_Tab0::fnIUM_Enable_Check(void)
{

	if ((CTestOptionDlg::m_nIcType == IC_TYPE_6	 || CTestOptionDlg::m_nIcType == IC_TYPE_10 || CTestOptionDlg::m_nIcType == IC_TYPE_1)//IC_TYPE_6,10			// ZT2717 / ZTW622	: 0xE622 ,GTW623 : 0xE623
		&& ((CButton*)GetDlgItem(IDC_CHECK_USE_IFE))->GetCheck() == 0)
	{
		GetDlgItem(IDC_CHECK_VERSION_IUM)->ShowWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_VERSION_IUM)->ShowWindow(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_VERSION_IUM))->SetCheck(0);
	}

	//IUM FORMAT
	CTestModeView * pTestModeView = (CTestModeView*)g_pTestModeViewWnd;

	if (CTestOptionDlg::m_nIcType == IC_TYPE_6	//IC_TYPE_6,			// ZT2717 / ZTW622				: 0xE622
		&& ((CButton*)GetDlgItem(IDC_CHECK_USE_IFE))->GetCheck() == 0
		&& pTestModeView->GetModuleVendorCode() == MODULE_VENDOR_XIAOMI)
	{
		GetDlgItem(IDC_COMBO_IUM_FORMAT)->ShowWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_COMBO_IUM_FORMAT)->ShowWindow(FALSE);
		((CButton*)GetDlgItem(IDC_COMBO_IUM_FORMAT))->SetCheck(0);
	}

	if ( ((CButton*)GetDlgItem(IDC_CHECK_USE_IFE))->GetCheck() == 1)
	{
			GetDlgItem(IDC_CHECK_USE_COUNT)->ShowWindow(TRUE);
				if ( ((CButton*)GetDlgItem(IDC_CHECK_USE_COUNT))->GetCheck() ==1)
				GetDlgItem(IDC_ALL_RETRY_COUNT)->ShowWindow(TRUE);
				else
				GetDlgItem(IDC_ALL_RETRY_COUNT)->ShowWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_USE_COUNT)->ShowWindow(FALSE);
		GetDlgItem(IDC_ALL_RETRY_COUNT)->ShowWindow(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_USE_COUNT))->SetCheck(0);
	}

	if ( ((CButton*)GetDlgItem(IDC_CHECK_USE_IFE))->GetCheck() == 0
		&& pTestModeView->GetModuleVendorCode() == MODULE_VENDOR_EDO )
	{
		GetDlgItem(IDC_CHECK_USE_MPTOOL_VER)->ShowWindow(TRUE);
		if ( ((CButton*)GetDlgItem(IDC_CHECK_USE_MPTOOL_VER))->GetCheck() ==1)
			GetDlgItem(IDC_EDIT_MPTOOL_VER)->ShowWindow(TRUE);
		else
			GetDlgItem(IDC_EDIT_MPTOOL_VER)->ShowWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_USE_MPTOOL_VER)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_MPTOOL_VER)->ShowWindow(FALSE);

	}
}

/*
//2021.11.17 김극래 책임 VISIONOX 일경우 Log Namming rule 변경.
    - MODULE VENDOR 가 Visionox 일 경우에만 적용
		- Additional Log IFE 와 상관 없이 강제 활성화.

        - [CREATE PASS/FAIL DIRECTORY] 체크 시 폴더 구조 변경
            - 강제로 Checked O 로 만든다.

        - [Additional Log Content]
            - 강제로 Checked O 로 만든다.

        - PASS/년월일/file_name_rule, FAIL/년월일/file_name_rule

        - FIXED POSITION 체크 여부에 따라서 달라지는 사양은 없음.

        - [Additional Log Content] 체크 여부에 따른 file naming rule
            - 강제 체크 됨 : Barcode_time_TestName(can be edit)-Test Stage(can be edit)-MouleMaker(can be edit)_PASS/Fail.CSV
*/
void CTestOptionDlg_Tab0::fnCheck_VendorVisinox(void)
{
	CTestModeView * pTestModeView = (CTestModeView*)g_pTestModeViewWnd;

	if (pTestModeView->GetModuleVendorCode() == MODULE_VENDOR_VISIONOX)
	{
		((CButton*)GetDlgItem(IDC_CHECK_CREATE_PASS_FAIL_DIR))->SetCheck(1);
		CTestOptionDlg::m_bCreatePassFailDirectory = true;	//강제 체크
		//GetDlgItem(IDC_CHECK_CREATE_PASS_FAIL_DIR)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CREATE_PASS_FAIL_DIR)->Invalidate();

		((CButton*)GetDlgItem(IDC_CHECK_ADD_LOG))->SetCheck(1);
		CTestOptionDlg::m_bAdditionalLOG = true;			//강제 체크
		GetDlgItem(IDC_CHECK_ADD_LOG)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_ADD_LOG)->Invalidate();

		//UpdateData(TRUE);
	}
}

void CTestOptionDlg_Tab0::OnRadioBnClicked(UINT nID)
{
	if (nID >= IDC_RADIO_IC_TYPE_1 && nID <= IDC_RADIO_IC_TYPE_14)
	{
		m_nIcType_old = CTestOptionDlg::m_nIcType;

		UpdateData(TRUE);

		GetDlgItem(IDC_RADIO_IC_TYPE_1 + CTestOptionDlg::m_nIcType)->Invalidate();
		GetDlgItem(IDC_RADIO_IC_TYPE_1 + m_nIcType_old)->Invalidate();


		fnIUM_Enable_Check();
	}
	else if(nID >= IDC_RADIO_SLAVE_ADDR_1 && nID <= IDC_RADIO_SLAVE_ADDR_2)
	{
		UpdateData(TRUE);

		GetDlgItem(IDC_RADIO_SLAVE_ADDR_1)->Invalidate();
		GetDlgItem(IDC_RADIO_SLAVE_ADDR_2)->Invalidate();
	}
	else if (nID >= IDC_RADIO_INOUT_TYPE_1 && nID <= IDC_RADIO_INOUT_TYPE_2)
	{
		UpdateData(TRUE);

		GetDlgItem(IDC_RADIO_INOUT_TYPE_1)->Invalidate();
		GetDlgItem(IDC_RADIO_INOUT_TYPE_2)->Invalidate();
	}
}

void CTestOptionDlg_Tab0::OnCheckBnClicked()
{
	int nID = GetFocus()->GetDlgCtrlID();
	int nUpdateID = 0xFFFF;

	if (nID == IDC_CHECK_USE_IFE)
	{
		nUpdateID = CTRL_UPDATE_IFE;
		fnIUM_Enable_Check();
	}
	else if (nID == IDC_CHECK_SAVE_RESULT || nID == IDC_CHECK_FIXED_POSITION || nID == IDC_CHECK_CREATE_PASS_FAIL_DIR)
		nUpdateID = CTRL_UPDATE_SAVE_RESULT;
	else if (nID == IDC_CHECK_ADD_LOG)
		nUpdateID = CTRL_UPDATE_ADD_LOG;
	else if (nID == IDC_CHECK_DOWNLOAD_FIRMWARE || nID == IDC_CHECK_DOWNLOAD_FIXED_POSITION ||
		nID == IDC_CHECK_SAVE_INFO || nID == IDC_CHECK_VERIFY_FIRMWARE || nID == IDC_CHECK_BURST_UPGRADE)
		nUpdateID = CTRL_UPDATE_DOWNLOAD;
	else if (nID == IDC_CHECK_MULTI_DEVICE || nID == IDC_CHECK_START_INDIVIDUAL_TEST)
		nUpdateID = CTRL_UPDATE_MULTI_DEVICE;
	else if (nID == IDC_CHECK_VERSION)
		nUpdateID = CTRL_UPDATE_CHECK_VERSION;
	else if (nID == IDC_CHECK_STARTUP)
		nUpdateID = CTRL_UPDATE_USE_STARTUP;
	else if (nID == IDC_CHECK_RESET)
		nUpdateID = CTRL_UPDATE_CHECK_RESET;
	else if (nID == IDC_CHECK_USE_COUNT)
		nUpdateID = CTRL_UPDATE_CHECK_RETRY;
	else if (nID == IDC_CHECK_USE_MPTOOL_VER)
		nUpdateID = CTRL_UPDATE_CHECK_MPTOOL_VER;
	else if (nID == IDC_CHECK_I2C_LEVEL)
		nUpdateID = CTRL_UPDATE_CHECK_I2C_LEVEL;

	EnableDialogControls(nUpdateID);

	fnCheck_VendorVisinox();

	GetDlgItem(nID)->Invalidate();
}

CString CTestOptionDlg_Tab0::SelectDirecotry()
{
	CString strSelectDir = "";
	TCHAR szPath[_MAX_PATH];
	CString csTitle = _T("Select the save directory");

	BROWSEINFO broweInfo;
	memset(&broweInfo, 0x00, sizeof(broweInfo));

	broweInfo.hwndOwner = m_hWnd;
	broweInfo.pidlRoot = NULL;
	broweInfo.lpszTitle = csTitle.GetBuffer(csTitle.GetLength());
	broweInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	broweInfo.lpfn = NULL;

	csTitle.ReleaseBuffer();

	LPITEMIDLIST lpBrowse = SHBrowseForFolder(&broweInfo);

	if (lpBrowse != NULL)
	{
		if (SHGetPathFromIDList(lpBrowse, szPath))
		{
			strSelectDir = szPath;
		}
	}

	return strSelectDir;
}

void CTestOptionDlg_Tab0::OnBnClickedButtonSelectSaveResultDir()
{
	CString strSelectDir = SelectDirecotry();

	if (strSelectDir.IsEmpty())
		return;

	CTestOptionDlg::m_strSaveResultDir = strSelectDir;
	UpdateData(FALSE);

	GetDlgItem(IDC_EDIT_SAVE_RESULT_DIR)->SetWindowText((LPCTSTR)CTestOptionDlg::m_strSaveResultDir);
}

void CTestOptionDlg_Tab0::OnBnClickedButtonSelectFirwareDir()
{
	CString strSelectDir = SelectDirecotry();

	if (strSelectDir.IsEmpty())
		return;

	CTestOptionDlg::m_strFirmwareDir = strSelectDir;
	UpdateData(FALSE);

	GetDlgItem(IDC_EDIT_DOWNLOAD_FIRMWARE_DIR)->SetWindowText((LPCTSTR)CTestOptionDlg::m_strFirmwareDir);
}

void CTestOptionDlg_Tab0::OnCbnSelchangeComboSaveInfoLocation()
{
	CTestOptionDlg::m_nSaveInfoLocation = ((CComboBox*)GetDlgItem(IDC_COMBO_SAVE_INFO_LOCATION))->GetCurSel();

	EnableDialogControls(CTRL_UPDATE_DOWNLOAD);
}

void CTestOptionDlg_Tab0::GetControlValue()
{
	if (g_nModuleIdList.GetCount() == 0)
		return;

	int nSel = m_ComboModuleID.GetCurSel();
	if (nSel == -1)
		return;

	int nModuleID_Sel = g_nModuleIdList.GetAt(nSel);

	CTestOptionDlg::m_strModuleID.Format("0x%X", nModuleID_Sel);

	CTestOptionDlg::m_nSaveInfoLocation = ((CComboBox*)GetDlgItem(IDC_COMBO_SAVE_INFO_LOCATION))->GetCurSel();

	CTestOptionDlg::m_nIUMFormat = m_ComboIumFormat.GetCurSel();
	CTestOptionDlg::m_nI2CFormat = m_ComboI2CFormat.GetCurSel();
}

void CTestOptionDlg_Tab0::InitUiContorl()
{
	SetButtonConfig(&m_btnResetCount);
	SetButtonConfig(&m_btnDownloadBrowse);
	SetButtonConfig(&m_btnSaveResultBrowse);

	SetWindowTheme(GetDlgItem(IDC_CHECK_USE_COUNT)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_CHECK_USE_IFE)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_MULTI_DEVICE)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_START_INDIVIDUAL_TEST)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_USER_NUMBER)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_ADD_LOG)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_CHECK_DOWNLOAD_FIRMWARE)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_DOWNLOAD_FIXED_POSITION)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_SAVE_INFO)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_VERIFY_FIRMWARE)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_BURST_UPGRADE)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_CHECK_VERSION)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_I2C_LEVEL)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_VERSION_IUM)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_AUTO_FW_DOWNLOAD)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_CHECK_INTERRUPT_PIN)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_RESET)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_RESET_PIN)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_CHECK_SAVE_RESULT)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_FIXED_POSITION)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_CREATE_PASS_FAIL_DIR)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_STARTUP)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_RADIO_SLAVE_ADDR_1)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_RADIO_SLAVE_ADDR_2)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_RADIO_INOUT_TYPE_1)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_RADIO_INOUT_TYPE_2)->m_hWnd, L"", L"");

	for (int i = 0; i < IC_TYPE_MAX; i++)
		SetWindowTheme(GetDlgItem(IDC_RADIO_IC_TYPE_1 + i)->m_hWnd, L"", L"");
}

// 변수의 상태에 따라서 컨트롤들의 표시 여부를 결정하는 함수임
void CTestOptionDlg_Tab0::EnableDialogControls(int nUpdateID)
{
	BOOL bChangeIFE = FALSE;

	CTestModeView * pTestModeView = (CTestModeView*)g_pTestModeViewWnd;
	if (pTestModeView == NULL)
		return;

	int nModuleVendorCode = pTestModeView->GetModuleVendorCode();

	UpdateData(TRUE);

	if( nUpdateID & CTRL_UPDATE_IFE )
	{
		if (nUpdateID == CTRL_UPDATE_IFE)
		{
			bChangeIFE = TRUE;
			nUpdateID |= CTRL_UPDATE_ADD_LOG;

			CTestOptionDlg::CheckIfeException();
		}
	}

	if( nUpdateID & CTRL_UPDATE_ADD_LOG )
	{
		if (nModuleVendorCode == MODULE_VENDOR_TRULY)
			GetDlgItem(IDC_CHECK_ADD_LOG)->EnableWindow(TRUE);
		else
			GetDlgItem(IDC_CHECK_ADD_LOG)->EnableWindow(CTestOptionDlg::m_bUseIFE);

		//2021.11.17 김극래 책임 요청 "Visionox 일경우 m_bAdditionalLOG 강제 체크
		if (pTestModeView->GetModuleVendorCode() == MODULE_VENDOR_VISIONOX)
			CTestOptionDlg::m_bAdditionalLOG = TRUE;

		BOOL bAddLog = CTestOptionDlg::m_bAdditionalLOG;

		GetDlgItem(IDC_STATIC_TEST_NAME)->EnableWindow(bAddLog);
		GetDlgItem(IDC_EDIT_TEST_NAME)->EnableWindow(bAddLog);
		GetDlgItem(IDC_STATIC_TEST_STAGE)->EnableWindow(bAddLog);
		GetDlgItem(IDC_EDIT_TEST_STAGE)->EnableWindow(bAddLog);
		GetDlgItem(IDC_STATIC_MODULE_MAKER)->EnableWindow(bAddLog);
		GetDlgItem(IDC_EDIT_MODULE_MAKER)->EnableWindow(bAddLog);
		GetDlgItem(IDC_STATIC_INOUT_TYPE)->EnableWindow(bAddLog);
		GetDlgItem(IDC_RADIO_INOUT_TYPE_1)->EnableWindow(bAddLog);
		GetDlgItem(IDC_RADIO_INOUT_TYPE_2)->EnableWindow(bAddLog);

		if( bChangeIFE )
		{
			GetDlgItem(IDC_CHECK_ADD_LOG)->Invalidate();

			UpdateData(FALSE);
		}
	}

	if( nUpdateID & CTRL_UPDATE_DOWNLOAD )
	{
		BOOL bDownload = CTestOptionDlg::m_bDownload;
		BOOL bDispDownloadDir = bDownload && (CTestOptionDlg::m_bDownloadFixedPosition ? FALSE : TRUE);
		BOOL bSaveInfo = bDownload && CTestOptionDlg::m_bSaveInfo;
		BOOL bSaveEeprom = bSaveInfo && (CTestOptionDlg::m_nSaveInfoLocation == SAVE_INFO_EEPROM);
		BOOL bSaveIum = bSaveInfo && (CTestOptionDlg::m_nSaveInfoLocation == SAVE_INFO_IUM);

		GetDlgItem(IDC_STATIC_DOWNLOAD_RETRY_COUNT)->EnableWindow(bDownload);
		GetDlgItem(IDC_EDIT_DOWNLOAD_RETRY_COUNT)->EnableWindow(bDownload);
		GetDlgItem(IDC_CHECK_DOWNLOAD_FIXED_POSITION)->EnableWindow(bDownload);

		GetDlgItem(IDC_EDIT_DOWNLOAD_FIRMWARE_DIR)->EnableWindow(bDispDownloadDir);
		GetDlgItem(IDC_BUTTON_SELECT_DOWNLOAD_FIRMWARE_DIR)->EnableWindow(bDispDownloadDir);

		GetDlgItem(IDC_CHECK_SAVE_INFO)->EnableWindow(bDownload);
		GetDlgItem(IDC_STATIC_LOCATION)->EnableWindow(bSaveInfo);
		GetDlgItem(IDC_COMBO_SAVE_INFO_LOCATION)->EnableWindow(bSaveInfo);

		GetDlgItem(IDC_STATIC_MODULE_ID)->EnableWindow(bSaveEeprom);
		GetDlgItem(IDC_COMBO_MODULE_ID)->EnableWindow(bSaveEeprom);

		GetDlgItem(IDC_EDIT_PROJECT_ID)->EnableWindow(bSaveIum);
		GetDlgItem(IDC_EDIT_DEVICE_ID)->EnableWindow(bSaveIum);
		GetDlgItem(IDC_STATIC_PROJECT_ID)->EnableWindow(bSaveIum);
		GetDlgItem(IDC_STATIC_DEVICE_ID)->EnableWindow(bSaveIum);

		if (bDownload)
		{
			CTestOptionDlg::m_bVerifyFirmware = FALSE;
			UpdateData(FALSE);
		}

		GetDlgItem(IDC_CHECK_VERIFY_FIRMWARE)->EnableWindow(bDownload ? FALSE : TRUE);
		GetDlgItem(IDC_CHECK_BURST_UPGRADE)->EnableWindow(bDownload);
	}

	if( nUpdateID & CTRL_UPDATE_CHECK_VERSION )
	{
		BOOL bCheckVersion = CTestOptionDlg::m_bCheckVersion;

		GetDlgItem(IDC_CHECK_AUTO_FW_DOWNLOAD)->EnableWindow(bCheckVersion);
		GetDlgItem(IDC_STATIC_IC_REVISION)->EnableWindow(bCheckVersion);
		GetDlgItem(IDC_EDIT_IC_REVISION)->EnableWindow(bCheckVersion);
		GetDlgItem(IDC_STATIC_HW_ID)->EnableWindow(bCheckVersion);
		GetDlgItem(IDC_EDIT_HW_ID)->EnableWindow(bCheckVersion);
		GetDlgItem(IDC_STATIC_MAJOR_VERSION)->EnableWindow(bCheckVersion);
		GetDlgItem(IDC_EDIT_MAJOR_VERSION)->EnableWindow(bCheckVersion);
		GetDlgItem(IDC_STATIC_MINOR_VERSION)->EnableWindow(bCheckVersion);
		GetDlgItem(IDC_EDIT_MINOR_VERSION)->EnableWindow(bCheckVersion);
		GetDlgItem(IDC_STATIC_REGISTER_VERSION)->EnableWindow(bCheckVersion);
		GetDlgItem(IDC_EDIT_REGISTER_VERSION)->EnableWindow(bCheckVersion);

		GetDlgItem(IDC_CHECK_VERSION_IUM)->EnableWindow(bCheckVersion);
	}

	if( nUpdateID & CTRL_UPDATE_CHECK_I2C_LEVEL )
	{
		BOOL bCheckI2C = CTestOptionDlg::m_bCheckI2C_Level;

		GetDlgItem(IDC_COMBO_I2C_FORMAT)->EnableWindow(bCheckI2C);
	}
	
	if( nUpdateID & CTRL_UPDATE_MULTI_DEVICE )
	{
		BOOL bUseMultiDevice = CTestOptionDlg::m_bUseMultiDevice;

		GetDlgItem(IDC_STATIC_MAX_DEVICE_COUNT)->EnableWindow(bUseMultiDevice);
		GetDlgItem(IDC_EDIT_MAX_DEVICE_COUNT)->EnableWindow(bUseMultiDevice);
		GetDlgItem(IDC_CHECK_START_INDIVIDUAL_TEST)->EnableWindow(bUseMultiDevice);

		if(bUseMultiDevice == FALSE)
		{
			CTestOptionDlg::m_bUseStartIndividualTest = FALSE;
			UpdateData(FALSE);
		}

#if defined(D_USE_CHECK_BUTTON_TEST)
		if(CTestOptionDlg::m_bUseStartIndividualTest)
		{
			CTestOptionDlg::m_bCheckGpioButton = FALSE;
			CTestOptionDlg::m_bCheckTouchButton = FALSE;
			UpdateData(FALSE);
		}
#endif
	}

	if( nUpdateID & CTRL_UPDATE_SAVE_RESULT )
	{
		BOOL bSaveResult = CTestOptionDlg::m_bSaveResult;
		BOOL bSaveFixedPosition = CTestOptionDlg::m_bSaveFixedPosition;

		GetDlgItem(IDC_CHECK_FIXED_POSITION)->EnableWindow(bSaveResult);
		GetDlgItem(IDC_CHECK_CREATE_PASS_FAIL_DIR)->EnableWindow(bSaveResult);
		GetDlgItem(IDC_EDIT_SAVE_RESULT_DIR)->EnableWindow(bSaveResult && (bSaveFixedPosition ? FALSE : TRUE));
		GetDlgItem(IDC_BUTTON_SELECT_SAVE_RESULT_DIR)->EnableWindow(bSaveResult && (bSaveFixedPosition ? FALSE : TRUE));
	}

	if( nUpdateID & CTRL_UPDATE_USE_STARTUP )
	{
		GetDlgItem(IDC_EDIT_STARTUP_PASSWORD)->EnableWindow(CTestOptionDlg::m_bUseStartup ? TRUE : FALSE);
	}

	if( nUpdateID & CTRL_UPDATE_CHECK_RESET )
	{
		GetDlgItem(IDC_CHECK_RESET_PIN)->EnableWindow(CTestOptionDlg::m_bCheckReset ? TRUE : FALSE);

		if(CTestOptionDlg::m_bCheckReset == FALSE)
		{
			CTestOptionDlg::m_bCheckResetPin = FALSE;
			UpdateData(FALSE);
		}
	}

	if( nUpdateID & CTRL_UPDATE_CHECK_RETRY )
	{
		if (((CButton*)GetDlgItem(IDC_CHECK_USE_IFE))->GetCheck() == TRUE
		&& ((CButton*)GetDlgItem(IDC_CHECK_USE_COUNT))->GetCheck() == TRUE)
			GetDlgItem(IDC_ALL_RETRY_COUNT)->ShowWindow(TRUE);
		else
			GetDlgItem(IDC_ALL_RETRY_COUNT)->ShowWindow(FALSE);
	}

	if (nUpdateID & CTRL_UPDATE_CHECK_MPTOOL_VER)
	{
		if (((CButton*)GetDlgItem(IDC_CHECK_USE_IFE))->GetCheck() == FALSE
			&& ((CButton*)GetDlgItem(IDC_CHECK_USE_MPTOOL_VER))->GetCheck() == TRUE)
			GetDlgItem(IDC_EDIT_MPTOOL_VER)->ShowWindow(TRUE);
		else
			GetDlgItem(IDC_EDIT_MPTOOL_VER)->ShowWindow(FALSE);
	}
}

HBRUSH CTestOptionDlg_Tab0::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor != CTLCOLOR_STATIC)
		return hbr;

	int nState = -1;

	switch (pWnd->GetDlgCtrlID())
	{
	case IDC_CHECK_USE_IFE:					nState = CTestOptionDlg::m_bUseIFE; 				break;
	case IDC_CHECK_USE_COUNT:				nState = CTestOptionDlg::m_bUseRetry; 				break;
	case IDC_CHECK_USE_MPTOOL_VER: 		nState = CTestOptionDlg::m_bUseMptoolver; 				break;
	case IDC_CHECK_MULTI_DEVICE:			nState = CTestOptionDlg::m_bUseMultiDevice;			break;
	case IDC_CHECK_START_INDIVIDUAL_TEST:	nState = CTestOptionDlg::m_bUseStartIndividualTest;	break;
	case IDC_CHECK_USER_NUMBER:				nState = CTestOptionDlg::m_bCheckUserNumber;		break;
	case IDC_CHECK_ADD_LOG:					nState = CTestOptionDlg::m_bAdditionalLOG;			break;

	case IDC_CHECK_DOWNLOAD_FIRMWARE:		nState = CTestOptionDlg::m_bDownload;				break;
	case IDC_CHECK_DOWNLOAD_FIXED_POSITION:	nState = CTestOptionDlg::m_bDownloadFixedPosition; 	break;
	case IDC_CHECK_SAVE_INFO:				nState = CTestOptionDlg::m_bSaveInfo;				break;
	case IDC_CHECK_VERIFY_FIRMWARE:			nState = CTestOptionDlg::m_bVerifyFirmware;			break;
	case IDC_CHECK_BURST_UPGRADE:			nState = CTestOptionDlg::m_bBurstUpgrade;			break;

	case IDC_CHECK_VERSION:					nState = CTestOptionDlg::m_bCheckVersion;			break;
	
	case IDC_CHECK_I2C_LEVEL:					nState = CTestOptionDlg::m_bCheckI2C_Level;			break;
	
	case IDC_CHECK_VERSION_IUM:				nState = CTestOptionDlg::m_bCheckIUM;				break;
	case IDC_CHECK_AUTO_FW_DOWNLOAD:		nState = CTestOptionDlg::m_bCheckAutoFwDownload;	break;

	case IDC_CHECK_INTERRUPT_PIN:			nState = CTestOptionDlg::m_bCheckIntPin;			break;
	case IDC_CHECK_RESET:					nState = CTestOptionDlg::m_bCheckReset;				break;
	case IDC_CHECK_RESET_PIN:				nState = CTestOptionDlg::m_bCheckResetPin;			break;

	case IDC_CHECK_SAVE_RESULT:				nState = CTestOptionDlg::m_bSaveResult;					break;
	case IDC_CHECK_FIXED_POSITION:			nState = CTestOptionDlg::m_bSaveFixedPosition;			break;
	case IDC_CHECK_CREATE_PASS_FAIL_DIR:	nState = CTestOptionDlg::m_bCreatePassFailDirectory;	break;
	case IDC_CHECK_STARTUP:					nState = CTestOptionDlg::m_bUseStartup;					break;
	
	case IDC_RADIO_IC_TYPE_1:				nState = (CTestOptionDlg::m_nIcType == 0) ? TRUE : FALSE; break;
	case IDC_RADIO_IC_TYPE_2:				nState = (CTestOptionDlg::m_nIcType == 1) ? TRUE : FALSE; break;
	case IDC_RADIO_IC_TYPE_3:				nState = (CTestOptionDlg::m_nIcType == 2) ? TRUE : FALSE; break;
	case IDC_RADIO_IC_TYPE_4:				nState = (CTestOptionDlg::m_nIcType == 3) ? TRUE : FALSE; break;
	case IDC_RADIO_IC_TYPE_5:				nState = (CTestOptionDlg::m_nIcType == 4) ? TRUE : FALSE; break;
	case IDC_RADIO_IC_TYPE_6:				nState = (CTestOptionDlg::m_nIcType == 5) ? TRUE : FALSE; break;
	case IDC_RADIO_IC_TYPE_7:				nState = (CTestOptionDlg::m_nIcType == 6) ? TRUE : FALSE; break;
	case IDC_RADIO_IC_TYPE_8:				nState = (CTestOptionDlg::m_nIcType == 7) ? TRUE : FALSE; break;
	case IDC_RADIO_IC_TYPE_9:				nState = (CTestOptionDlg::m_nIcType == 8) ? TRUE : FALSE; break;
	case IDC_RADIO_IC_TYPE_10:				nState = (CTestOptionDlg::m_nIcType == 9) ? TRUE : FALSE; break;
	case IDC_RADIO_IC_TYPE_11:				nState = (CTestOptionDlg::m_nIcType == 10) ? TRUE : FALSE; break;
	case IDC_RADIO_IC_TYPE_12:				nState = (CTestOptionDlg::m_nIcType == 11) ? TRUE : FALSE; break;
	case IDC_RADIO_IC_TYPE_13:				nState = (CTestOptionDlg::m_nIcType == 12) ? TRUE : FALSE; break;
	case IDC_RADIO_IC_TYPE_14:				nState = (CTestOptionDlg::m_nIcType == 13) ? TRUE : FALSE; break;

	case IDC_RADIO_SLAVE_ADDR_1:			nState = (CTestOptionDlg::m_nSlaveAddress == 0) ? TRUE : FALSE; break;
	case IDC_RADIO_SLAVE_ADDR_2:			nState = (CTestOptionDlg::m_nSlaveAddress == 1) ? TRUE : FALSE; break;

	case IDC_RADIO_INOUT_TYPE_1:			nState = (CTestOptionDlg::m_nInOutType == 0) ? TRUE : FALSE; break;
	case IDC_RADIO_INOUT_TYPE_2:			nState = (CTestOptionDlg::m_nInOutType == 1) ? TRUE : FALSE; break;

	
	}

	if (nState != -1)
	{
		ChangeCheckboxBgColor(pDC, nState);
	}

	return hbr;
}

void CTestOptionDlg_Tab0::OnBnClickedButtonDownloadFixedPosition()
{
	OnCheckBnClicked();

	if(CTestOptionDlg::m_bDownloadFixedPosition)
	{
		CTestOptionDlg::m_strFirmwareDir.Format("%s\\FIRMWARE", CTestOptionDlg::m_strZtdExeDir);
		UpdateData(FALSE);
	}
}

void CTestOptionDlg_Tab0::OnBnClickedButtonLogFixedPosition()
{
	OnCheckBnClicked();

	if(CTestOptionDlg::m_bSaveFixedPosition)
	{
		CTestOptionDlg::m_strSaveResultDir.Format("%s", CTestOptionDlg::m_strZtdExeDir);
		UpdateData(FALSE);
	}
}


