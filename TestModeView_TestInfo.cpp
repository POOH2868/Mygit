// TestModeView_TestInfo.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "TestModeView.h"

#include "TestModeView_TestInfo.h"

#include "util.h"

void CTestModeView::TestInfo_LoadData()
{
	// Initialize TEST_INFO
	memset(&m_TestInfo_DND, 0x00, sizeof(TEST_INFO_DND));
	memset(&m_TestInfo_HF_DND, 0x00, sizeof(TEST_INFO_DND));

	memset(&m_TestInfo_CND, 0x00, sizeof(TEST_INFO_CND));			//add 2022.11.14 for ZTM730 CND

	memset(&m_TestInfo_SELF_DND, 0x00, sizeof(TEST_INFO_SELF));
	memset(&m_TestInfo_SELF_SATURATION, 0x00, sizeof(TEST_INFO_SELF));

	memset(&m_TestInfo_MEASURE_DELTA, 0x00, sizeof(TEST_INFO_MEASURE_DELTA));
	memset(&m_TestInfo_DIGITAL_WHEEL, 0x00, sizeof(TEST_INFO_DIGITAL_WHEEL));

	// Load TEST_CONF.INI
	TestInfo_LoadTestIni();

	//2021.11.17 김극래 책임 VISIONOX 일경우 Log Namming rule 변경.
	if (m_nModuleVendorCode == MODULE_VENDOR_VISIONOX)
	{
		//MODULE_VENDOR_VISIONOX 일 경우 강제 활성화 시킨다.
		m_bCreatePassFailDirectory = true;
		m_bAdditionalLOG = true;

	}

	// Initialize Spec Buffers
	for (int x = 0; x < DEF_MAX_X_LINE_NUM; x++)
	{
		for (int y = 0; y < DEF_MAX_Y_LINE_NUM; y++)
		{
			m_TestInfo_DND.pDataMax[x][y] = 30000;
			m_TestInfo_DND.pDataMin[x][y] = 0;
			m_TestInfo_DND.pHDiff[x][y] = 100;
			m_TestInfo_DND.pVDiff[x][y] = 100;

			//add 2022.11.14 for ZTM730 CND
			m_TestInfo_CND.pDataMax[x][y] = 30000;
			m_TestInfo_CND.pDataMin[x][y] = 0;

			m_TestInfo_HF_DND.pDataMax[x][y] = 30000;
			m_TestInfo_HF_DND.pDataMin[x][y] = 0;
			m_TestInfo_HF_DND.pHDiff[x][y] = 100;
			m_TestInfo_HF_DND.pVDiff[x][y] = 100;

			m_TestInfo_ONLYSELF_DND.pTxDataMax[x][y] = 30000;
			m_TestInfo_ONLYSELF_DND.pTxDataMin[x][y] = 0;
			m_TestInfo_ONLYSELF_DND.pRxDataMax[x][y] = 30000;
			m_TestInfo_ONLYSELF_DND.pRxDataMin[x][y] = 0;

			m_TestInfo_SELF_DND.pTxDataMax[x][y] = 30000;
			m_TestInfo_SELF_DND.pTxDataMin[x][y] = 0;
			m_TestInfo_SELF_DND.pTxHDiff[x][y] = 100;

			m_TestInfo_SELF_DND.pRxDataMax[x][y] = 30000;
			m_TestInfo_SELF_DND.pRxDataMin[x][y] = 0;
			m_TestInfo_SELF_DND.pRxHDiff[x][y] = 100;

			m_TestInfo_SELF_SATURATION.pTxDataMax[x][y] = 30000;
			m_TestInfo_SELF_SATURATION.pTxDataMin[x][y] = 0;

			m_TestInfo_SELF_SATURATION.pRxDataMax[x][y] = 30000;
			m_TestInfo_SELF_SATURATION.pRxDataMin[x][y] = 0;

			m_TestInfo_MEASURE_DELTA.pDataMax[x][y] = 30000;
			m_TestInfo_MEASURE_DELTA.pDataMin[x][y] = 0;

			m_TestInfo_DIGITAL_WHEEL.pDataMax[x][y] = 30000;
			m_TestInfo_DIGITAL_WHEEL.pDataMin[x][y] = 100;
		}

		m_TestInfo_MEASURE_DELTA.pSelfRxDataMax[x] = 30000;
		m_TestInfo_MEASURE_DELTA.pSelfRxDataMin[x] = 0;
		m_TestInfo_MEASURE_DELTA.pSelfTxDataMax[x] = 30000;
		m_TestInfo_MEASURE_DELTA.pSelfTxDataMin[x] = 0;
	}

	// Load Spec file
	TestInfo_LoadTestSpec();
}

void CTestModeView::TestInfo_SaveData()
{
	CString strMsg;

	strMsg.Format("WAIT FOR SAVING TEST SPEC");
	UpdateStatusBar(&strMsg);

	//-------------------------------------------------
	// Save TEST_CONF.INI
	TestInfo_SaveTestIni();

	//-------------------------------------------------
	// Save Spec INI
	TestInfo_SaveTestSpec();

	strMsg.Format("END SAVING TEST SPEC");
	UpdateStatusBar(&strMsg);
}

void CTestModeView::TestInfo_Load_Check_PrintPpVariation()
{
	char chFile[1024];

	sprintf(chFile, "%s\\%s", m_strConfigDir.GetBuffer(), TEST_CONF_INI);

	if (PathFileExists(chFile) == FALSE)
	{
		m_TestInfo_NOISE.AfeInfo.bPrintPpVariation = 0;
	}
	else
	{
		m_TestInfo_NOISE.AfeInfo.bPrintPpVariation = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_PRINT_PP_VARIAITON, 0, chFile);
	}
}

void CTestModeView::BarcodeDisplayInTestVisible()
{
	if (GetDlgItem(IDC_STATIC_BARCODE_TITLE) == NULL)
		return;

	if (m_bUseBarcode && m_bBarcodeDisplayInTest)
	{
		GetDlgItem(IDC_STATIC_BARCODE_TITLE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_BARCODE_TITLE)->Invalidate();

		GetDlgItem(IDC_STATIC_BARCODE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_BARCODE)->Invalidate();

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
	}
	else
	{
		GetDlgItem(IDC_STATIC_BARCODE_TITLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_BARCODE_TITLE)->Invalidate();

		GetDlgItem(IDC_STATIC_BARCODE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_BARCODE)->Invalidate();
	}
}

//==============================================================================================
// TEST_CONF.INI

void CTestModeView::TestInfo_LoadTestIni()
{
	char chFile[1024];
	char chStr[512];

	BOOL bTestIniNotFound = FALSE;
	BOOL bTestIniReCreate = FALSE;

	m_strIniErroMsg = _T("");

	sprintf(chFile, "%s\\%s", m_strConfigDir.GetBuffer(), TEST_CONF_INI);

	if (PathFileExists(chFile) == FALSE)
	{
		bTestIniNotFound = TRUE;
		m_strIniErroMsg.Format("The configuration file does not exist.\nWe will create a default configuration file.");
	}

	//-------------------------------------------------
	// [TSM TEST]
	GetPrivateProfileString(INI_CATEGORY_TSM_TEST, INI_ITEM_TSM_TEST_MPZTD_VERSION, "NULL", chStr, sizeof(chStr), chFile);
	if (bTestIniNotFound == FALSE && g_strZtdVersion.Compare(chStr) != 0)
	{
		bTestIniReCreate = TRUE;
	}

	//-------------------------------------------------
	// [DEFAULT]
	m_bUseIFE = GetPrivateProfileInt(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_USE_IFE, 1, chFile);

	m_bUseMultiDevice = GetPrivateProfileInt(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_MULTI_DEVICE, 0, chFile);
	m_nMultiDeviceCount = GetPrivateProfileInt(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_MULTI_DEVICE_COUNT, 8, chFile);
	m_bUseStartIndividualTest = GetPrivateProfileInt(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_START_INDIVIDUAL_TEST, 0, chFile);

	if( m_nMultiDeviceCount <= 0 )
		m_nMultiDeviceCount = 1;
	else if( m_nMultiDeviceCount >= MAX_DEVICE_COUNT )
		m_nMultiDeviceCount = MAX_DEVICE_COUNT;

	g_nIcType = GetPrivateProfileInt(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_IC_TYPE, 3, chFile);

	g_nSlaveAddress = GetPrivateProfileInt(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_SLAVE_ADDRESS, 0, chFile);

	m_nStandBySec = GetPrivateProfileInt(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_STANDBY_SEC, 0, chFile);

	m_bHidMode = GetPrivateProfileInt(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_HID_MODE, 0, chFile);

	m_nModuleVendorCode = GetPrivateProfileInt(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_MODULE_VENDOR, 0, chFile);

	m_bUseRetry = GetPrivateProfileInt(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_USE_RETRY, 0, chFile);

	m_nAllRetryCount  = GetPrivateProfileInt(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_All_RETRY_COUNT, 1, chFile);

	m_bUseMptoolver	=	 GetPrivateProfileInt(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_USE_MPTOOL, 0, chFile);

	m_nMptoolCount =  GetPrivateProfileInt(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_MPTOOL_VER_COUNT, 0, chFile);

//


	//-------------------------------------------------
	// [LOG]
	m_bSaveResult = GetPrivateProfileInt(INI_CATEGORY_LOG, INI_ITEM_LOG_SAVE_RESULT, 1, chFile);
	m_bSaveFixedPosition = GetPrivateProfileInt(INI_CATEGORY_LOG, INI_ITEM_LOG_SAVE_FIXED_POSITION, 1, chFile);
	m_bCreatePassFailDirectory = GetPrivateProfileInt(INI_CATEGORY_LOG, INI_ITEM_LOG_CREATE_PASS_FAIL_DIR, 0, chFile);
	GetPrivateProfileString(INI_CATEGORY_LOG, INI_ITEM_LOG_SAVE_RESULT_PATH, m_strZtdExeDir, chStr, sizeof(chStr), chFile);
	m_strSaveResultDir.Format("%s", chStr);

	//-------------------------------------------------
	// [PASSWORD]
	GetPrivateProfileString(INI_CATEGORY_PASSWORD, INI_ITEM_PASSWORD_ADMINISTRATOR, "ZINITIX1", chStr, sizeof(chStr), chFile);
	m_strUserAdminPassword.Format("%s", chStr);

	m_bUseStartup = GetPrivateProfileInt(INI_CATEGORY_PASSWORD, INI_ITEM_PASSWORD_USE_STARTUP, 0, chFile);

	GetPrivateProfileString(INI_CATEGORY_PASSWORD, INI_ITEM_PASSWORD_STARTUP, "", chStr, sizeof(chStr), chFile);
	m_strStartupPassword.Format("%s", chStr);

	//-------------------------------------------------
	// [INFORMATION]
	GetPrivateProfileString(INI_CATEGORY_INFORMATION, INI_ITEM_INFORMATION_CUSTOMER, "NULL", chStr, sizeof(chStr), chFile);
	m_strCustomer.Format("%s", chStr);
	if (m_strCustomer.Compare("NULL") == 0)
		m_strCustomer = _T("");

	GetPrivateProfileString(INI_CATEGORY_INFORMATION, INI_ITEM_INFORMATION_PROJECT_NAME, "NULL", chStr, sizeof(chStr), chFile);
	m_strProjectName.Format("%s", chStr);
	if (m_strProjectName.Compare("NULL") == 0)
		m_strProjectName = _T("");

	m_bCheckUserNumber = GetPrivateProfileInt(INI_CATEGORY_INFORMATION, INI_ITEM_INFORMATION_USER_NUMBER, 0, chFile);

// by Paul {{{
	m_bAdditionalLOG = GetPrivateProfileInt(INI_CATEGORY_INFORMATION, INI_ITEM_INFORMATION_ADDITIONAL_LOG, 0, chFile);

	GetPrivateProfileString(INI_CATEGORY_INFORMATION, INI_ITEM_INFORMATION_TEST_NAME, "NULL", chStr, sizeof(chStr), chFile);
	m_strTestName.Format("%s", chStr);
	if (m_strTestName.Compare("NULL") == 0)
		m_strTestName = _T("");

	GetPrivateProfileString(INI_CATEGORY_INFORMATION, INI_ITEM_INFORMATION_TEST_STAGE, "NULL", chStr, sizeof(chStr), chFile);
	m_strTestStage.Format("%s", chStr);
	if (m_strTestStage.Compare("NULL") == 0)
		m_strTestStage = _T("");

	GetPrivateProfileString(INI_CATEGORY_INFORMATION, INI_ITEM_INFORMATION_MODULE_MAKER, "NULL", chStr, sizeof(chStr), chFile);
	m_strModuleMaker.Format("%s", chStr);
	if (m_strModuleMaker.Compare("NULL") == 0)
		m_strModuleMaker = _T("");

	m_nInOutType = GetPrivateProfileInt(INI_CATEGORY_INFORMATION, INI_ITEM_INFORMATION_IQC_OQC, 0, chFile);
// }}}

	//-------------------------------------------------
	// [BARCODE]
	m_bUseBarcode = GetPrivateProfileInt(INI_CATEGORY_BARCODE, INI_ITEM_BARCODE_USE_BARCODE, 0, chFile);
	m_bBarcodeDisplayInTest = GetPrivateProfileInt(INI_CATEGORY_BARCODE, INI_ITEM_BARCODE_DISPLAY_IN_TEST, 0, chFile);
	m_bAutoStartTest = GetPrivateProfileInt(INI_CATEGORY_BARCODE, INI_ITEM_BARCODE_Auto_Start_TEST, 0, chFile);
	m_bFixedBarcodeLength = GetPrivateProfileInt(INI_CATEGORY_BARCODE, INI_ITEM_BARCODE_FIXED_BARCODE_LENGTH, 0, chFile);
	m_nBarcode_Length = GetPrivateProfileInt(INI_CATEGORY_BARCODE, INI_ITEM_BARCODE_LENGTH, 30, chFile);
	m_nBarcode_AutoReset = GetPrivateProfileInt(INI_CATEGORY_BARCODE, INI_ITEM_BARCODE_AUTO_RESET_TIME, 0, chFile);

	//-------------------------------------------------
	// [FW DOWNLOAD]
	m_bDownload = GetPrivateProfileInt(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_CHECK_DOWNLOAD, 0, chFile);
	m_bDownloadFixedPosition = GetPrivateProfileInt(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_FIXED_POSITION, 1, chFile);
	GetPrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_PATH, m_strFirmwareDir, chStr, sizeof(chStr), chFile);
	m_strFirmwareDir.Format("%s", chStr);
	m_nDownloadRetryCount = GetPrivateProfileInt(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_RETRY_COUNT, 2, chFile);
	m_bSaveInfo = GetPrivateProfileInt(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_SAVE_INFO, 0, chFile);
	m_nSaveInfoLocation = GetPrivateProfileInt(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_SAVE_INFO_LOCATION, 0, chFile);
	m_bVerifyFirmware = GetPrivateProfileInt(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_VERIFY_FIRMWARE, 0, chFile);
	m_bVerifyAfterDownload = GetPrivateProfileInt(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_VERIFY_AFTER_DOWNLOAD, 1, chFile);
	m_bBurstUpgrade = GetPrivateProfileInt(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_BURST_UPGRADE, 0, chFile);
	m_bBurstUpgradeReal = GetPrivateProfileInt(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_BURST_UPGRADE_REAL, 0, chFile);
	m_bForceUpgrade = GetPrivateProfileInt(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_FORCE_UPGRADE, 0, chFile);
	m_bChangeNvmTiming = GetPrivateProfileInt(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_CHANGE_NVM_TIMING, 1, chFile);
	m_nIUMFormat = GetPrivateProfileInt(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_IUM_FORMAT, 0, chFile);

	m_bCheckEepromBit = GetPrivateProfileInt(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_CHECK_EEPROM_BIT, 0, chFile);
	m_bUseNvmPgmVerify = GetPrivateProfileInt(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_USE_NVM_PGM_VERIFY, 0, chFile);

	GetPrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_PROJECT_ID, "0000", chStr, sizeof(chStr), chFile);
	m_strProjectID.Format("%s", chStr);
	GetPrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_DEVICE_ID, "0000000000000000", chStr, sizeof(chStr), chFile);
	m_strDeviceID.Format("%s", chStr);

	//-------------------------------------------------
	// [CHECK PIN]
	m_bCheckIntPin = GetPrivateProfileInt(INI_CATEGORY_CHECK_PIN, INI_ITEM_CHECK_PIN_INTERRUPT, 0, chFile);
	m_bCheckReset = GetPrivateProfileInt(INI_CATEGORY_CHECK_PIN, INI_ITEM_CHECK_PIN_RESET, 0, chFile);
	m_bCheckResetPin = GetPrivateProfileInt(INI_CATEGORY_CHECK_PIN, INI_ITEM_CHECK_PIN_RESET_GPIO, 0, chFile);

	//-------------------------------------------------
	// [CHECK VERSION]
	m_bCheckVersion = GetPrivateProfileInt(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_CHECK_VERSION, 0, chFile);
	m_bCheckAutoFwDownload = GetPrivateProfileInt(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_CHECK_AUTO_FW_DOWNLOAD, 0, chFile);

	GetPrivateProfileString(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_IC_REVISION, "0x0000", chStr, sizeof(chStr), chFile);
	m_strIcRevision.Format("%s", chStr);

	GetPrivateProfileString(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_HW_ID, "0x0000", chStr, sizeof(chStr), chFile);
	m_strHwID.Format("%s", chStr);

	GetPrivateProfileString(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_MODULE_ID, "0xFFFF", chStr, sizeof(chStr), chFile);
	m_strModuleID.Format("%s", chStr);

	GetPrivateProfileString(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_MAJOR_VERSION, "0x0006", chStr, sizeof(chStr), chFile);
	m_strMajorVersion.Format("%s", chStr);

	GetPrivateProfileString(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_MINOR_VERSION, "0x0001", chStr, sizeof(chStr), chFile);
	m_strMinorVersion.Format("%s", chStr);

	GetPrivateProfileString(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_RELEASE_VERSION, "0x0000", chStr, sizeof(chStr), chFile);
	m_strRegisterVersion.Format("%s", chStr);

	m_bCheckIUM = GetPrivateProfileInt(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_IUM, 0, chFile);
		//-------------------------------------------------
	// [I2C]
	m_bCheckI2C_Level = GetPrivateProfileInt(INI_CATEGORY_I2C_CHECK, INI_ITEM_CHECK_I2C, 0, chFile);
	m_nI2CFormat = GetPrivateProfileInt(INI_CATEGORY_I2C_CHECK, INI_ITEM_CHECK_I2C_LEVEL, 0, chFile);

	//-------------------------------------------------
	// [DND]
	m_TestInfo_DND.bCheckData = GetPrivateProfileInt(INI_CATEGORY_DND, INI_ITEM_DND_CHECK, 0, chFile);
	m_TestInfo_DND.nDataMax = GetPrivateProfileInt(INI_CATEGORY_DND, INI_ITEM_DND_ALL_NODE_MAX_SPEC, 30000, chFile);
	m_TestInfo_DND.nDataMin = GetPrivateProfileInt(INI_CATEGORY_DND, INI_ITEM_DND_ALL_NODE_MIN_SPEC, 0, chFile);

	m_TestInfo_DND.bCheckDataDiff = GetPrivateProfileInt(INI_CATEGORY_DND, INI_ITEM_DND_CHECK_GAP, 0, chFile);
	m_TestInfo_DND.nHDiffCenter = GetPrivateProfileInt(INI_CATEGORY_DND, INI_ITEM_DND_ALL_NODE_H_GAP_SPEC, 100, chFile);
	m_TestInfo_DND.nVDiffCenter = GetPrivateProfileInt(INI_CATEGORY_DND, INI_ITEM_DND_ALL_NODE_V_GAP_SPEC, 100, chFile);

	m_TestInfo_DND.AfeInfo.bUseAfeSetting = GetPrivateProfileInt(INI_CATEGORY_DND, INI_ITEM_AFE_USE_USER_SETTING, 0, chFile);
	m_TestInfo_DND.AfeInfo.nFrequency = GetPrivateProfileInt(INI_CATEGORY_DND, INI_ITEM_AFE_FREQUENCY, 240, chFile);
	m_TestInfo_DND.AfeInfo.nNCount = GetPrivateProfileInt(INI_CATEGORY_DND, INI_ITEM_AFE_N_COUNT, 13, chFile);
	m_TestInfo_DND.AfeInfo.nUCount = GetPrivateProfileInt(INI_CATEGORY_DND, INI_ITEM_AFE_U_COUNT, 10, chFile);
	//-------------------------------------------------
	// [CND]	//add 2022.11.14 for ZTM730 CND
	m_TestInfo_CND.bCheckData = GetPrivateProfileInt(INI_CATEGORY_CND, INI_ITEM_CND_CHECK, 0, chFile);
	m_TestInfo_CND.nDataMax = GetPrivateProfileInt(INI_CATEGORY_CND, INI_ITEM_CND_ALL_NODE_MAX_SPEC, 30000, chFile);
	m_TestInfo_CND.nDataMin = GetPrivateProfileInt(INI_CATEGORY_CND, INI_ITEM_CND_ALL_NODE_MIN_SPEC, 0, chFile);

	//-------------------------------------------------
	// [HF DND]
	m_TestInfo_HF_DND.bCheckData = GetPrivateProfileInt(INI_CATEGORY_HF_DND, INI_ITEM_HF_DND_CHECK, 0, chFile);
	m_TestInfo_HF_DND.nDataMax = GetPrivateProfileInt(INI_CATEGORY_HF_DND, INI_ITEM_HF_DND_ALL_NODE_MAX_SPEC, 30000, chFile);
	m_TestInfo_HF_DND.nDataMin = GetPrivateProfileInt(INI_CATEGORY_HF_DND, INI_ITEM_HF_DND_ALL_NODE_MIN_SPEC, 0, chFile);

	m_TestInfo_HF_DND.bCheckDataDiff = GetPrivateProfileInt(INI_CATEGORY_HF_DND, INI_ITEM_HF_DND_CHECK_GAP, 0, chFile);
	m_TestInfo_HF_DND.nHDiffCenter = GetPrivateProfileInt(INI_CATEGORY_HF_DND, INI_ITEM_HF_DND_ALL_NODE_H_GAP_SPEC, 100, chFile);
	m_TestInfo_HF_DND.nVDiffCenter = GetPrivateProfileInt(INI_CATEGORY_HF_DND, INI_ITEM_HF_DND_ALL_NODE_V_GAP_SPEC, 100, chFile);

	m_TestInfo_HF_DND.AfeInfo.bUseAfeSetting = GetPrivateProfileInt(INI_CATEGORY_HF_DND, INI_ITEM_AFE_USE_USER_SETTING, 0, chFile);
	m_TestInfo_HF_DND.AfeInfo.nFrequency = GetPrivateProfileInt(INI_CATEGORY_HF_DND, INI_ITEM_AFE_FREQUENCY, 39, chFile);
	m_TestInfo_HF_DND.AfeInfo.nNCount = GetPrivateProfileInt(INI_CATEGORY_HF_DND, INI_ITEM_AFE_N_COUNT, 13, chFile);
	m_TestInfo_HF_DND.AfeInfo.nUCount = GetPrivateProfileInt(INI_CATEGORY_HF_DND, INI_ITEM_AFE_U_COUNT, 10, chFile);
	//-------------------------------------------------
	// [HIGH_VOLTAGE]
	m_TestInfo_HIGH_VOLTAGE.bCheckData = GetPrivateProfileInt(INI_CATEGORY_HIGH_VOLTAGE, INI_ITEM_HIGH_VOLTAGE_CHECK, 0, chFile);
	m_TestInfo_HIGH_VOLTAGE.pDataMax = GetPrivateProfileInt(INI_CATEGORY_HIGH_VOLTAGE, INI_ITEM_HIGH_VOLTAGE_MAX_SPEC, 1000, chFile);
	m_TestInfo_HIGH_VOLTAGE.pDataMin = GetPrivateProfileInt(INI_CATEGORY_HIGH_VOLTAGE, INI_ITEM_HIGH_VOLTAGE_MIN_SPEC, 0, chFile);
	//-------------------------------------------------
	// [ONLY_SELF_DND]
	m_TestInfo_ONLYSELF_DND.bCheckData = GetPrivateProfileInt(INI_CATEGORY_ONLYSELF_DND, INI_ITEM_ONLYSELF_DND_CHECK, 0, chFile);
	m_TestInfo_ONLYSELF_DND.nTxDataMax = GetPrivateProfileInt(INI_CATEGORY_ONLYSELF_DND, INI_ITEM_ONLYSELF_TX_ALL_NODE_MAX_SPEC, 30000, chFile);
	m_TestInfo_ONLYSELF_DND.nTxDataMin = GetPrivateProfileInt(INI_CATEGORY_ONLYSELF_DND, INI_ITEM_ONLYSELF_TX_ALL_NODE_MIN_SPEC, 0, chFile);
	m_TestInfo_ONLYSELF_DND.nRxDataMax = GetPrivateProfileInt(INI_CATEGORY_ONLYSELF_DND, INI_ITEM_ONLYSELF_RX_ALL_NODE_MAX_SPEC, 30000, chFile);
	m_TestInfo_ONLYSELF_DND.nRxDataMin = GetPrivateProfileInt(INI_CATEGORY_ONLYSELF_DND, INI_ITEM_ONLYSELF_RX_ALL_NODE_MIN_SPEC, 0, chFile);
	//-------------------------------------------------
	// [SELF_DND]
	m_TestInfo_SELF_DND.bCheckData = GetPrivateProfileInt(INI_CATEGORY_SELF_DND, INI_ITEM_SELF_DND_CHECK, 0, chFile);
	m_TestInfo_SELF_DND.nTxDataMax = GetPrivateProfileInt(INI_CATEGORY_SELF_DND, INI_ITEM_SELF_TX_ALL_NODE_MAX_SPEC, 30000, chFile);
	m_TestInfo_SELF_DND.nTxDataMin = GetPrivateProfileInt(INI_CATEGORY_SELF_DND, INI_ITEM_SELF_TX_ALL_NODE_MIN_SPEC, 0, chFile);
	m_TestInfo_SELF_DND.nRxDataMax = GetPrivateProfileInt(INI_CATEGORY_SELF_DND, INI_ITEM_SELF_RX_ALL_NODE_MAX_SPEC, 30000, chFile);
	m_TestInfo_SELF_DND.nRxDataMin = GetPrivateProfileInt(INI_CATEGORY_SELF_DND, INI_ITEM_SELF_RX_ALL_NODE_MIN_SPEC, 0, chFile);

	m_TestInfo_SELF_DND.bCheckDataDiff = GetPrivateProfileInt(INI_CATEGORY_SELF_DND, INI_ITEM_SELF_DND_CHECK_GAP, 0, chFile);
	m_TestInfo_SELF_DND.nTxHDiffCenter = GetPrivateProfileInt(INI_CATEGORY_SELF_DND, INI_ITEM_SELF_DND_TX_ALL_NODE_H_GAP_SPEC, 100, chFile);
	m_TestInfo_SELF_DND.nRxHDiffCenter = GetPrivateProfileInt(INI_CATEGORY_SELF_DND, INI_ITEM_SELF_DND_RX_ALL_NODE_H_GAP_SPEC, 100, chFile);

	m_TestInfo_SELF_DND.AfeInfo.bUseAfeSetting = GetPrivateProfileInt(INI_CATEGORY_SELF_DND, INI_ITEM_AFE_USE_USER_SETTING, 0, chFile);
	m_TestInfo_SELF_DND.AfeInfo.nFrequency_A = GetPrivateProfileInt(INI_CATEGORY_SELF_DND, INI_ITEM_AFE_FREQUENCY_A, 60, chFile);
	m_TestInfo_SELF_DND.AfeInfo.nNCount_A = GetPrivateProfileInt(INI_CATEGORY_SELF_DND, INI_ITEM_AFE_N_COUNT_A, 13, chFile);
	m_TestInfo_SELF_DND.AfeInfo.nUCount_A = GetPrivateProfileInt(INI_CATEGORY_SELF_DND, INI_ITEM_AFE_U_COUNT_A, 0, chFile);

	m_TestInfo_SELF_DND.AfeInfo.nFrequency_B = GetPrivateProfileInt(INI_CATEGORY_SELF_DND, INI_ITEM_AFE_FREQUENCY_B, 60, chFile);
	m_TestInfo_SELF_DND.AfeInfo.nNCount_B = GetPrivateProfileInt(INI_CATEGORY_SELF_DND, INI_ITEM_AFE_N_COUNT_B, 13, chFile);
	m_TestInfo_SELF_DND.AfeInfo.nUCount_B = GetPrivateProfileInt(INI_CATEGORY_SELF_DND, INI_ITEM_AFE_U_COUNT_B, 0, chFile);

	//-------------------------------------------------
	// [SELF_SATURATION]
	m_TestInfo_SELF_SATURATION.bCheckData = GetPrivateProfileInt(INI_CATEGORY_SELF_SATURATION, INI_ITEM_SELF_SATURATION_CHECK, 0, chFile);
	m_TestInfo_SELF_SATURATION.nTxDataMax = GetPrivateProfileInt(INI_CATEGORY_SELF_SATURATION, INI_ITEM_SELF_TX_ALL_NODE_MAX_SPEC, 30000, chFile);
	m_TestInfo_SELF_SATURATION.nTxDataMin = GetPrivateProfileInt(INI_CATEGORY_SELF_SATURATION, INI_ITEM_SELF_TX_ALL_NODE_MIN_SPEC, 0, chFile);
	m_TestInfo_SELF_SATURATION.nRxDataMax = GetPrivateProfileInt(INI_CATEGORY_SELF_SATURATION, INI_ITEM_SELF_RX_ALL_NODE_MAX_SPEC, 30000, chFile);
	m_TestInfo_SELF_SATURATION.nRxDataMin = GetPrivateProfileInt(INI_CATEGORY_SELF_SATURATION, INI_ITEM_SELF_RX_ALL_NODE_MIN_SPEC, 0, chFile);

	m_TestInfo_SELF_SATURATION.AfeInfo.bUseAfeSetting = GetPrivateProfileInt(INI_CATEGORY_SELF_SATURATION, INI_ITEM_AFE_USE_USER_SETTING, 0, chFile);
	m_TestInfo_SELF_SATURATION.AfeInfo.nFrequency_A = GetPrivateProfileInt(INI_CATEGORY_SELF_SATURATION, INI_ITEM_AFE_FREQUENCY_A, 240, chFile);
	m_TestInfo_SELF_SATURATION.AfeInfo.nNCount_A = GetPrivateProfileInt(INI_CATEGORY_SELF_SATURATION, INI_ITEM_AFE_N_COUNT_A, 13, chFile);
	m_TestInfo_SELF_SATURATION.AfeInfo.nUCount_A = GetPrivateProfileInt(INI_CATEGORY_SELF_SATURATION, INI_ITEM_AFE_U_COUNT_A, 10, chFile);

	m_TestInfo_SELF_SATURATION.AfeInfo.nFrequency_B = GetPrivateProfileInt(INI_CATEGORY_SELF_SATURATION, INI_ITEM_AFE_FREQUENCY_B, 240, chFile);
	m_TestInfo_SELF_SATURATION.AfeInfo.nNCount_B = GetPrivateProfileInt(INI_CATEGORY_SELF_SATURATION, INI_ITEM_AFE_N_COUNT_B, 13, chFile);
	m_TestInfo_SELF_SATURATION.AfeInfo.nUCount_B = GetPrivateProfileInt(INI_CATEGORY_SELF_SATURATION, INI_ITEM_AFE_U_COUNT_B, 10, chFile);

	//-------------------------------------------------
	// [TRX SHORT]
	m_bCheckTrxShort = GetPrivateProfileInt(INI_CATEGORY_TRX_SHORT, INI_ITEM_TRX_SHORT_CHECK, 0, chFile);

	//-------------------------------------------------
	// [CURRENT CONSUMPTION]
	m_TestInfo_CURRENT_CONSUMPTION.Active.bCheckData = GetPrivateProfileInt(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_CHECK_ACTIVE, 0, chFile);
	m_TestInfo_CURRENT_CONSUMPTION.Active.nThreshould_Max = GetPrivateProfileInt(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_THR_ACTIVE_MAX, 0, chFile);
	m_TestInfo_CURRENT_CONSUMPTION.Active.nThreshould_Min = GetPrivateProfileInt(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_THR_ACTIVE_MIN, 0, chFile);
	m_TestInfo_CURRENT_CONSUMPTION.Active.nAvgMs = GetPrivateProfileInt(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_AVG_MS_ACTIVE, 200, chFile);

	m_TestInfo_CURRENT_CONSUMPTION.Idle.bCheckData = GetPrivateProfileInt(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_CHECK_IDLE, 0, chFile);
	m_TestInfo_CURRENT_CONSUMPTION.Idle.nThreshould_Max = GetPrivateProfileInt(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_THR_IDLE, 0, chFile);
	m_TestInfo_CURRENT_CONSUMPTION.Idle.nAvgMs = GetPrivateProfileInt(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_AVG_MS_IDLE, 600, chFile);

	m_TestInfo_CURRENT_CONSUMPTION.Sleep.bCheckData = GetPrivateProfileInt(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_CHECK_SLEEP, 0, chFile);
	m_TestInfo_CURRENT_CONSUMPTION.Sleep.nThreshould_Max = GetPrivateProfileInt(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_THR_SLEEP, 0, chFile);
	m_TestInfo_CURRENT_CONSUMPTION.Sleep.nAvgMs = GetPrivateProfileInt(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_AVG_MS_SLEEP, 600, chFile);

	m_TestInfo_CURRENT_CONSUMPTION.DeepSleep.bCheckData = GetPrivateProfileInt(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_CHECK_DEEP_SLEEP, 0, chFile);
	m_TestInfo_CURRENT_CONSUMPTION.DeepSleep.nThreshould_Max = GetPrivateProfileInt(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_THR_DEEP_SLEEP, 0, chFile);
	m_TestInfo_CURRENT_CONSUMPTION.DeepSleep.nAvgMs = GetPrivateProfileInt(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_AVG_MS_DEEP_SLEEP, 200, chFile);

	//-------------------------------------------------
	// [MEASURE DELTA]
	m_TestInfo_MEASURE_DELTA.bCheckData = GetPrivateProfileInt(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_CHECK, 0, chFile);
	m_TestInfo_MEASURE_DELTA.bCheckAutoStart = GetPrivateProfileInt(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_CHECK_AUTO_START, 0, chFile);
	m_TestInfo_MEASURE_DELTA.nAutoStartThreshold = GetPrivateProfileInt(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_AUTO_START_THRESHOLD, 100, chFile);
	m_TestInfo_MEASURE_DELTA.nCount = GetPrivateProfileInt(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_FRAME_COUNT, 10, chFile);
	m_TestInfo_MEASURE_DELTA.nDataMax = GetPrivateProfileInt(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_ALL_NODE_MAX_SPEC, 30000, chFile);
	m_TestInfo_MEASURE_DELTA.nDataMin = GetPrivateProfileInt(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_ALL_NODE_MIN_SPEC, 0, chFile);
	m_TestInfo_MEASURE_DELTA.m_nSelfRxDataMax = GetPrivateProfileInt(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_ALL_NODE_SELF_RX_MAX_SPEC, 30000, chFile);
	m_TestInfo_MEASURE_DELTA.m_nSelfRxDataMin = GetPrivateProfileInt(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_ALL_NODE_SELF_RX_MIN_SPEC, 0, chFile);
	m_TestInfo_MEASURE_DELTA.m_nSelfTxDataMax = GetPrivateProfileInt(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_ALL_NODE_SELF_TX_MAX_SPEC, 30000, chFile);
	m_TestInfo_MEASURE_DELTA.m_nSelfTxDataMin = GetPrivateProfileInt(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_ALL_NODE_SELF_TX_MIN_SPEC, 0, chFile);

	m_TestInfo_MEASURE_DELTA.AfeInfo.bUseAfeSetting = GetPrivateProfileInt(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_AFE_USE_USER_SETTING, 0, chFile);
	m_TestInfo_MEASURE_DELTA.AfeInfo.nFrequency = GetPrivateProfileInt(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_AFE_FREQUENCY, 240, chFile);
	m_TestInfo_MEASURE_DELTA.AfeInfo.nNCount = GetPrivateProfileInt(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_AFE_N_COUNT, 13, chFile);
	m_TestInfo_MEASURE_DELTA.AfeInfo.nUCount = GetPrivateProfileInt(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_AFE_U_COUNT, 10, chFile);

	//-------------------------------------------------
	// [HW CALIBRATION]
	m_TestInfo_HW_CALIBRATION.bCheckData = GetPrivateProfileInt(INI_CATEGORY_HW_CALIBRATION, INI_ITEM_HW_CALIBRATION_CHECK, 0, chFile);
	m_TestInfo_HW_CALIBRATION.nHwCalibrationMax = GetPrivateProfileInt(INI_CATEGORY_HW_CALIBRATION, INI_ITEM_HW_CALIBRATION_SPEC, 0, chFile);

	m_TestInfo_HW_CALIBRATION.AfeInfo.bUseAfeSetting = GetPrivateProfileInt(INI_CATEGORY_HW_CALIBRATION, INI_ITEM_AFE_USE_USER_SETTING, 0, chFile);
	m_TestInfo_HW_CALIBRATION.AfeInfo.nFrequency = GetPrivateProfileInt(INI_CATEGORY_HW_CALIBRATION, INI_ITEM_AFE_FREQUENCY, 240, chFile);
	m_TestInfo_HW_CALIBRATION.AfeInfo.nNCount = GetPrivateProfileInt(INI_CATEGORY_HW_CALIBRATION, INI_ITEM_AFE_N_COUNT, 13, chFile);
	m_TestInfo_HW_CALIBRATION.AfeInfo.nUCount = GetPrivateProfileInt(INI_CATEGORY_HW_CALIBRATION, INI_ITEM_AFE_U_COUNT, 10, chFile);

	//-------------------------------------------------
	// [JITTER]
	m_TestInfo_JITTER.bCheckData = GetPrivateProfileInt(INI_CATEGORY_JITTER, INI_ITEM_JITTER_CHECK, 0, chFile);
	m_TestInfo_JITTER.nJitterDataMax = GetPrivateProfileInt(INI_CATEGORY_JITTER, INI_ITEM_JITTER_DATA_MAX, 0, chFile);
	m_TestInfo_JITTER.AfeInfo.bUseAfeSetting = GetPrivateProfileInt(INI_CATEGORY_JITTER, INI_ITEM_AFE_USE_USER_SETTING, 0, chFile);
	m_TestInfo_JITTER.AfeInfo.nFrequency = GetPrivateProfileInt(INI_CATEGORY_JITTER, INI_ITEM_AFE_FREQUENCY, 240, chFile);
	m_TestInfo_JITTER.AfeInfo.nNCount = GetPrivateProfileInt(INI_CATEGORY_JITTER, INI_ITEM_AFE_N_COUNT, 13, chFile);
	m_TestInfo_JITTER.AfeInfo.nUCount = GetPrivateProfileInt(INI_CATEGORY_JITTER, INI_ITEM_AFE_U_COUNT, 10, chFile);

	m_TestInfo_JITTER.bCheckDataByMpztd = GetPrivateProfileInt(INI_CATEGORY_JITTER, INI_ITEM_JITTER_CHECK_BY_MPZTD, 0, chFile);
	m_TestInfo_JITTER.nJitterReadCount = GetPrivateProfileInt(INI_CATEGORY_JITTER, INI_ITEM_JITTER_READ_FRAME_COUNT, 100, chFile);

	//-------------------------------------------------
	// [DIGITAL WHEEL]
	m_TestInfo_DIGITAL_WHEEL.bCheckData = GetPrivateProfileInt(INI_CATEGORY_DIGITAL_WHEEL, INI_ITEM_DIGITAL_WHEEL_CHECK, 0, chFile);

	//-------------------------------------------------
	// [BUTTON SWITCH]
#if defined(D_USE_CHECK_BUTTON_TEST)
	m_bCheckGpioButton = GetPrivateProfileInt(INI_CATEGORY_ADD_TEST_ITEM, INI_ITEM_GPIO_BUTTON_CHECK, 0, chFile);

	m_bCheckTouchButton = GetPrivateProfileInt(INI_CATEGORY_ADD_TEST_ITEM, INI_ITEM_TOUCH_BUTTON_CHECK, 0, chFile);
	m_nTouchButton_Threshold = GetPrivateProfileInt(INI_CATEGORY_ADD_TEST_ITEM, INI_ITEM_TOUCH_BUTTON_TH, 100, chFile);
#endif

	// ADDITIONAL ITEM
	m_bPowerOffWhenUsbDetect = GetPrivateProfileInt(INI_CATEGORY_ADD_TEST_ITEM, INI_ITEM_ADD_TEST_POWER_OFF_WHEN_USB_DETECT, 1, chFile);
	m_nPowerCtrlDelayTime = GetPrivateProfileInt(INI_CATEGORY_ADD_TEST_ITEM, INI_ITEM_ADD_TEST_POWER_CTRL_DELAY_TIME, 300, chFile);
	m_bStartAllTestByStartPin = GetPrivateProfileInt(INI_CATEGORY_ADD_TEST_ITEM, INI_ITEM_ADD_TEST_START_ALL_TEST_BY_START_PIN, 1, chFile);
	m_nSolutionResultNotify = GetPrivateProfileInt(INI_CATEGORY_ADD_TEST_ITEM, INI_ITEM_ADD_TEST_SOLUTION_RESULT_NOFITY, 0, chFile);

	m_bCheckVirtualDrawTest = GetPrivateProfileInt(INI_CATEGORY_ADD_TEST_ITEM, INI_ITEM_ADD_TEST_CHECK_VIRTUAL_DRAW_TEST, 0, chFile);

	// [CHECK NOISE]
	m_TestInfo_NOISE.m_bCheckNoise = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE, 0, chFile);
	m_TestInfo_NOISE.m_nCheckNoise_Threshold = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_THRESHOLD, 100, chFile);
	m_TestInfo_NOISE.m_nCheckNoise_Interval = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_INTERVAL, 100, chFile);
	m_TestInfo_NOISE.m_bCheckNoiseUserAFE = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_USEUSERAFE, 0, chFile);

	m_TestInfo_NOISE.m_nCheckNoise_ReadTimes = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_READTIMES, 30, chFile);
	m_TestInfo_NOISE.m_nCheckNoise_ReadTimes_ScalerCoef = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_READTIMES_SCALER_COEF, 1, chFile);

	m_TestInfo_NOISE.m_nCheckNoise_ReadTime_Org = m_TestInfo_NOISE.m_nCheckNoise_ReadTimes;
	if(m_TestInfo_NOISE.m_nCheckNoise_ReadTimes_ScalerCoef > 1
	&& m_TestInfo_NOISE.m_nCheckNoise_ReadTimes >= m_TestInfo_NOISE.m_nCheckNoise_ReadTimes_ScalerCoef)
	{
		m_TestInfo_NOISE.m_nCheckNoise_ReadTimes /= m_TestInfo_NOISE.m_nCheckNoise_ReadTimes_ScalerCoef;
	}

	m_TestInfo_NOISE.AfeInfo.m_bCheckFrequency1 = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_CHECK_FREQUENCY1, 0, chFile);
	m_TestInfo_NOISE.AfeInfo.m_bCheckFrequency2 = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_CHECK_FREQUENCY2, 0, chFile);
	m_TestInfo_NOISE.AfeInfo.m_bCheckFrequency3 = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_CHECK_FREQUENCY3, 0, chFile);
	m_TestInfo_NOISE.AfeInfo.m_bCheckFrequency4 = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_CHECK_FREQUENCY4, 0, chFile);

	m_TestInfo_NOISE.AfeInfo.nFrequency = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_FREQUENCY, 240, chFile);
	m_TestInfo_NOISE.AfeInfo.nFrequency1 = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_FREQUENCY1, 240, chFile);
	m_TestInfo_NOISE.AfeInfo.nFrequency2 = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_FREQUENCY2, 240, chFile);
	m_TestInfo_NOISE.AfeInfo.nFrequency3 = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_FREQUENCY3, 240, chFile);
	m_TestInfo_NOISE.AfeInfo.nFrequency4 = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_FREQUENCY4, 240, chFile);
	m_TestInfo_NOISE.AfeInfo.nNCount = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_NCOUNT, 10, chFile);
	m_TestInfo_NOISE.AfeInfo.nUCount = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_UCOUNT, 13, chFile);

	m_TestInfo_NOISE.AfeInfo.bPrintPpVariation = GetPrivateProfileInt(INI_CATEGORY_CHECK_NOISE, INI_ITEM_PRINT_PP_VARIAITON, 0, chFile);

	m_bMeasureLdo = GetPrivateProfileInt(INI_CATEGORY_DEV_TOOL, INI_ITEM_DEV_TOOL_MEASURE_LDO, 0, chFile);

#if defined(D_USE_CHECK_MPTOOL_REVISION)
	m_TestInfo_CURRENT_CONSUMPTION.bEnable = m_TestInfo_CURRENT_CONSUMPTION.Active.bCheckData;
#endif

	if (bTestIniNotFound)
	{
		TestInfo_SaveTestIni();
	}
	else if(bTestIniReCreate)
	{
		DeleteFile(chFile);
		TestInfo_SaveTestIni();
	}

	// set F/W directory
	if( m_bDownloadFixedPosition )
	{
		CString strFirmwareDir;
		strFirmwareDir.Format("%s\\FIRMWARE", m_strZtdExeDir);

		if (PathFileExists(strFirmwareDir) == FALSE)
			CreateDirectory((LPCTSTR)strFirmwareDir, NULL);

		if (strFirmwareDir.Compare(m_strFirmwareDir) != 0)
		{
			m_strFirmwareDir.Format("%s", strFirmwareDir);
			TestInfo_SaveTestIniEx(SAVE_TEST_INI_DOWNLOAD_FIXED_POSITION);
		}
	}

	// set result log directory
	BOOL bUseDefualtDir = TRUE;
	CString strSaveResultDir;

	BOOL bIsDirExist = PathFileExists(m_strSaveResultDir);

	if (m_bSaveFixedPosition || (bIsDirExist == FALSE))
	{// verify log directory
		if(m_bSaveFixedPosition == FALSE && bIsDirExist == FALSE)
		{
			bUseDefualtDir = FALSE;

			CString strCreateDir, strRootDir, strSaveDir, strTemp;
			strRootDir.Format("%s", m_strSaveResultDir);

			PathStripToRoot((LPSTR)(LPCTSTR)strRootDir);

			if(PathFileExists(strRootDir) == FALSE)
			{// root directory does not exist
				bUseDefualtDir = TRUE;
				m_strIniErroMsg.Format("\"%s\" directory does not exist.\nThe log file will be stored in \"%s\"", m_strSaveResultDir, m_strZtdExeDir);
			}
			else
			{// create directory according to INI
				strSaveDir = PathSkipRoot(m_strSaveResultDir);

				strCreateDir.Format("%s", strRootDir);

				do
				{
					if(strSaveDir.IsEmpty())
						break;

					int nPos = strSaveDir.Find("\\");
					if(nPos == -1)
						nPos = strSaveDir.GetLength();

					strCreateDir.AppendFormat("%s\\", strSaveDir.Left(nPos));
					if (PathFileExists(strCreateDir) == FALSE && CreateDirectory((LPCTSTR)strCreateDir, NULL) == FALSE)
					{
						bUseDefualtDir = TRUE;
						m_strIniErroMsg.Format("\"%s\" directory does not exist.\nThe log file will be stored in \"%s\"", m_strSaveResultDir, m_strZtdExeDir);
						break;
					}

					if(strSaveDir.GetLength() == nPos)
						break;

					strSaveDir = strSaveDir.Right(strSaveDir.GetLength() - 1 - nPos);
				}while(1);

				if(bUseDefualtDir == FALSE)
				{
					bIsDirExist = TRUE;
					strSaveResultDir.Format("%s", m_strSaveResultDir);
				}
			}
		}
		else if (bIsDirExist == FALSE)
		{
			m_strIniErroMsg.Format("\"%s\" directory does not exist.\nThe log file will be stored in \"%s\"", m_strSaveResultDir, m_strZtdExeDir);
		}

		// change log directory
		if(bUseDefualtDir)
		{
			strSaveResultDir.Format("%s", m_strZtdExeDir);
		}

		if ((strSaveResultDir.Compare(m_strSaveResultDir) != 0) || (bIsDirExist == FALSE))
		{
			m_strSaveResultDir.Format("%s", strSaveResultDir);
			TestInfo_SaveTestIniEx(SAVE_TEST_INI_LOG_SAVE_FIXED_POSITION);
		}
	}

	if (TestInfo_LoadModuleIdList() == FALSE)
	{
		TestInfo_SaveModuleIdList();
	}

	((GetDlgItem(IDC_STATIC_CUSTOMER)))->SetWindowText((LPCTSTR)m_strCustomer);
	((GetDlgItem(IDC_STATIC_PROJECT_NAME)))->SetWindowText((LPCTSTR)m_strProjectName);
	((GetDlgItem(IDC_STATIC_RELEASE_VERSION)))->SetWindowText((LPCTSTR)m_strRegisterVersion);

	Util_SetIcSignature(IC_SIGNATURE[g_nIcType]);
}

void CTestModeView::TestInfo_SaveTestIni()
{
	char	chFile[1024];
	char	chStr[1024];
	char	chSeperate[8];

	sprintf(chSeperate, "");

	sprintf(chFile, "%s\\%s", m_strConfigDir.GetBuffer(), TEST_CONF_INI);

	//-------------------------------------------------
	// [TSM TEST]
	sprintf(chStr, "%s", g_strZtdVersion.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_TSM_TEST, INI_ITEM_TSM_TEST_MPZTD_VERSION, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_TSM_TEST, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [DEFAULT]

	sprintf(chStr, "%d", m_bUseIFE);
	WritePrivateProfileString(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_USE_IFE, chStr, chFile);

	sprintf(chStr, "%d", m_bUseMultiDevice);
	WritePrivateProfileString(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_MULTI_DEVICE, chStr, chFile);

	sprintf(chStr, "%d", m_nMultiDeviceCount);
	WritePrivateProfileString(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_MULTI_DEVICE_COUNT, chStr, chFile);

	sprintf(chStr, "%d", m_bUseStartIndividualTest);
	WritePrivateProfileString(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_START_INDIVIDUAL_TEST, chStr, chFile);

	sprintf(chStr, "%d", g_nIcType);
	WritePrivateProfileString(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_IC_TYPE, chStr, chFile);

	sprintf(chStr, "%d", g_nSlaveAddress);
	WritePrivateProfileString(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_SLAVE_ADDRESS, chStr, chFile);

	sprintf(chStr, "%d", m_nStandBySec);
	WritePrivateProfileString(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_STANDBY_SEC, chStr, chFile);

	sprintf(chStr, "%d", m_bHidMode);
	WritePrivateProfileString(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_HID_MODE, chStr, chFile);

	sprintf(chStr, "%d", m_nModuleVendorCode);
	WritePrivateProfileString(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_MODULE_VENDOR, chStr, chFile);

	sprintf(chStr, "%d", m_nAllRetryCount);
	WritePrivateProfileString(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_All_RETRY_COUNT, chStr, chFile);

	sprintf(chStr, "%d", m_bUseRetry);
	WritePrivateProfileString(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_USE_RETRY, chStr, chFile);

	sprintf(chStr, "%d", m_bUseMptoolver);
	WritePrivateProfileString(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_USE_MPTOOL, chStr, chFile);

	sprintf(chStr, "%d", m_nMptoolCount);
	WritePrivateProfileString(INI_CATEGORY_DEFAULT, INI_ITEM_DEFAULT_MPTOOL_VER_COUNT, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_DEFAULT, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [LOG]
	sprintf(chStr, "%d", m_bSaveResult);
	WritePrivateProfileString(INI_CATEGORY_LOG, INI_ITEM_LOG_SAVE_RESULT, chStr, chFile);
	sprintf(chStr, "%d", m_bSaveFixedPosition);
	WritePrivateProfileString(INI_CATEGORY_LOG, INI_ITEM_LOG_SAVE_FIXED_POSITION, chStr, chFile);
	sprintf(chStr, "%d", m_bCreatePassFailDirectory);
	WritePrivateProfileString(INI_CATEGORY_LOG, INI_ITEM_LOG_CREATE_PASS_FAIL_DIR, chStr, chFile);
	sprintf(chStr, "%s", m_strSaveResultDir.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_LOG, INI_ITEM_LOG_SAVE_RESULT_PATH, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_LOG, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [PASSWORD]
	sprintf(chStr, "%s", m_strUserAdminPassword.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_PASSWORD, INI_ITEM_PASSWORD_ADMINISTRATOR, chStr, chFile);

	sprintf(chStr, "%d", m_bUseStartup);
	WritePrivateProfileString(INI_CATEGORY_PASSWORD, INI_ITEM_PASSWORD_USE_STARTUP, chStr, chFile);

	sprintf(chStr, "%s", m_strStartupPassword.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_PASSWORD, INI_ITEM_PASSWORD_STARTUP, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_PASSWORD, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [INFORMATION]
	if (m_strCustomer.IsEmpty())
		m_strCustomer = _T("NULL");
	sprintf(chStr, "%s", m_strCustomer.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_INFORMATION, INI_ITEM_INFORMATION_CUSTOMER, chStr, chFile);
	if (m_strCustomer.Compare("NULL") == 0)
		m_strCustomer = _T("");

	if (m_strProjectName.IsEmpty())
		m_strProjectName = _T("NULL");
	sprintf(chStr, "%s", m_strProjectName.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_INFORMATION, INI_ITEM_INFORMATION_PROJECT_NAME, chStr, chFile);
	if (m_strProjectName.Compare("NULL") == 0)
		m_strProjectName = _T("");

	sprintf(chStr, "%d", m_bCheckUserNumber);
	WritePrivateProfileString(INI_CATEGORY_INFORMATION, INI_ITEM_INFORMATION_USER_NUMBER, chStr, chFile);

// by Paul {{{
	sprintf(chStr, "%d", m_bAdditionalLOG);
	WritePrivateProfileString(INI_CATEGORY_INFORMATION, INI_ITEM_INFORMATION_ADDITIONAL_LOG, chStr, chFile);

	if (m_strTestName.IsEmpty())
		m_strTestName = _T("NULL");
	sprintf(chStr, "%s", m_strTestName.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_INFORMATION, INI_ITEM_INFORMATION_TEST_NAME, chStr, chFile);
	if (m_strTestName.Compare("NULL") == 0)
		m_strTestName = _T("");

	if (m_strTestStage.IsEmpty())
		m_strTestStage = _T("NULL");
	sprintf(chStr, "%s", m_strTestStage.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_INFORMATION, INI_ITEM_INFORMATION_TEST_STAGE, chStr, chFile);
	if (m_strTestStage.Compare("NULL") == 0)
		m_strTestStage = _T("");

	if (m_strModuleMaker.IsEmpty())
		m_strModuleMaker = _T("NULL");
	sprintf(chStr, "%s", m_strModuleMaker.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_INFORMATION, INI_ITEM_INFORMATION_MODULE_MAKER, chStr, chFile);
	if (m_strModuleMaker.Compare("NULL") == 0)
		m_strModuleMaker = _T("");

	sprintf(chStr, "%d", m_nInOutType);
	WritePrivateProfileString(INI_CATEGORY_INFORMATION, INI_ITEM_INFORMATION_IQC_OQC, chStr, chFile);
// }}}

	WritePrivateProfileString(INI_CATEGORY_INFORMATION, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [BARCODE]
	sprintf(chStr, "%d", m_bUseBarcode);
	WritePrivateProfileString(INI_CATEGORY_BARCODE, INI_ITEM_BARCODE_USE_BARCODE, chStr, chFile);
	sprintf(chStr, "%d", m_bBarcodeDisplayInTest);
	WritePrivateProfileString(INI_CATEGORY_BARCODE, INI_ITEM_BARCODE_DISPLAY_IN_TEST, chStr, chFile);
	sprintf(chStr, "%d", m_bAutoStartTest);
	WritePrivateProfileString(INI_CATEGORY_BARCODE, INI_ITEM_BARCODE_Auto_Start_TEST, chStr, chFile);
	sprintf(chStr, "%d", m_bFixedBarcodeLength);
	WritePrivateProfileString(INI_CATEGORY_BARCODE, INI_ITEM_BARCODE_FIXED_BARCODE_LENGTH, chStr, chFile);
	sprintf(chStr, "%d", m_nBarcode_Length);
	WritePrivateProfileString(INI_CATEGORY_BARCODE, INI_ITEM_BARCODE_LENGTH, chStr, chFile);
	sprintf(chStr, "%d", m_nBarcode_AutoReset);
	WritePrivateProfileString(INI_CATEGORY_BARCODE, INI_ITEM_BARCODE_AUTO_RESET_TIME, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_BARCODE, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [FW DOWNLOAD]
	sprintf(chStr, "%d", m_bDownload);
	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_CHECK_DOWNLOAD, chStr, chFile);
	sprintf(chStr, "%d", m_bDownloadFixedPosition);
	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_FIXED_POSITION, chStr, chFile);
	sprintf(chStr, "%s", m_strFirmwareDir.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_PATH, chStr, chFile);
	sprintf(chStr, "%d", m_nDownloadRetryCount);
	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_RETRY_COUNT, chStr, chFile);
	sprintf(chStr, "%d", m_bSaveInfo);
	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_SAVE_INFO, chStr, chFile);
	sprintf(chStr, "%d", m_nSaveInfoLocation);
	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_SAVE_INFO_LOCATION, chStr, chFile);
	sprintf(chStr, "%d", m_bVerifyFirmware);
	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_VERIFY_FIRMWARE, chStr, chFile);
	sprintf(chStr, "%d", m_bVerifyAfterDownload);
	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_VERIFY_AFTER_DOWNLOAD, chStr, chFile);
	sprintf(chStr, "%d", m_bBurstUpgrade);
	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_BURST_UPGRADE, chStr, chFile);
	sprintf(chStr, "%d", m_bBurstUpgradeReal);
	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_BURST_UPGRADE_REAL, chStr, chFile);
	sprintf(chStr, "%d", m_bForceUpgrade);
	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_FORCE_UPGRADE, chStr, chFile);
	sprintf(chStr, "%d", m_bChangeNvmTiming);
	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_CHANGE_NVM_TIMING, chStr, chFile);

	sprintf(chStr, "%d", m_nIUMFormat);
	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_IUM_FORMAT, chStr, chFile);

	sprintf(chStr, "%d", m_bCheckEepromBit);
	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_CHECK_EEPROM_BIT, chStr, chFile);
	sprintf(chStr, "%d", m_bUseNvmPgmVerify);
	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_USE_NVM_PGM_VERIFY, chStr, chFile);

	if (m_strProjectID.IsEmpty())
		m_strProjectID = _T("0000");
	sprintf(chStr, "%s", m_strProjectID.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_PROJECT_ID, chStr, chFile);

	if (m_strDeviceID.IsEmpty())
		m_strDeviceID = _T("0000000000000000");
	sprintf(chStr, "%s", m_strDeviceID.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_DEVICE_ID, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [CHECK PIN]
	sprintf(chStr, "%d", m_bCheckIntPin);
	WritePrivateProfileString(INI_CATEGORY_CHECK_PIN, INI_ITEM_CHECK_PIN_INTERRUPT, chStr, chFile);
	sprintf(chStr, "%d", m_bCheckReset);
	WritePrivateProfileString(INI_CATEGORY_CHECK_PIN, INI_ITEM_CHECK_PIN_RESET, chStr, chFile);
	sprintf(chStr, "%d", m_bCheckResetPin);
	WritePrivateProfileString(INI_CATEGORY_CHECK_PIN, INI_ITEM_CHECK_PIN_RESET_GPIO, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_CHECK_PIN, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [CHECK VERSION]
	sprintf(chStr, "%d", m_bCheckVersion);
	WritePrivateProfileString(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_CHECK_VERSION, chStr, chFile);

	sprintf(chStr, "%d", m_bCheckAutoFwDownload);
	WritePrivateProfileString(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_CHECK_AUTO_FW_DOWNLOAD, chStr, chFile);

	sprintf(chStr, "%s", m_strIcRevision.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_IC_REVISION, chStr, chFile);

	sprintf(chStr, "%s", m_strHwID.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_HW_ID, chStr, chFile);

	sprintf(chStr, "%s", m_strModuleID.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_MODULE_ID, chStr, chFile);

	sprintf(chStr, "%s", m_strMajorVersion.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_MAJOR_VERSION, chStr, chFile);

	sprintf(chStr, "%s", m_strMinorVersion.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_MINOR_VERSION, chStr, chFile);

	sprintf(chStr, "%s", m_strRegisterVersion.GetBuffer());
	WritePrivateProfileString(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_RELEASE_VERSION, chStr, chFile);

	sprintf(chStr, "%d", m_bCheckIUM);
	WritePrivateProfileString(INI_CATEGORY_CHECK_VERSION, INI_ITEM_CHECK_VERSION_IUM, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_CHECK_VERSION, chSeperate, "===============================================", chFile);
	

	//-------------------------------------------------
	// [I2C]
	sprintf(chStr, "%d", m_bCheckI2C_Level);
	WritePrivateProfileString(INI_CATEGORY_I2C_CHECK, INI_ITEM_CHECK_I2C, chStr, chFile);

	sprintf(chStr, "%d", m_nI2CFormat);
	WritePrivateProfileString(INI_CATEGORY_I2C_CHECK, INI_ITEM_CHECK_I2C_LEVEL, chStr, chFile);
	
	WritePrivateProfileString(INI_CATEGORY_CHECK_VERSION, chSeperate, "===============================================", chFile);
	//-------------------------------------------------
	// [DND]
	sprintf(chStr, "%d", m_TestInfo_DND.bCheckData);
	WritePrivateProfileString(INI_CATEGORY_DND, INI_ITEM_DND_CHECK, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_DND.nDataMax);
	WritePrivateProfileString(INI_CATEGORY_DND, INI_ITEM_DND_ALL_NODE_MAX_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_DND.nDataMin);
	WritePrivateProfileString(INI_CATEGORY_DND, INI_ITEM_DND_ALL_NODE_MIN_SPEC, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_DND.bCheckDataDiff);
	WritePrivateProfileString(INI_CATEGORY_DND, INI_ITEM_DND_CHECK_GAP, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_DND.nHDiffCenter);
	WritePrivateProfileString(INI_CATEGORY_DND, INI_ITEM_DND_ALL_NODE_H_GAP_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_DND.nVDiffCenter);
	WritePrivateProfileString(INI_CATEGORY_DND, INI_ITEM_DND_ALL_NODE_V_GAP_SPEC, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_DND.AfeInfo.bUseAfeSetting);
	WritePrivateProfileString(INI_CATEGORY_DND, INI_ITEM_AFE_USE_USER_SETTING, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_DND.AfeInfo.nFrequency);
	WritePrivateProfileString(INI_CATEGORY_DND, INI_ITEM_AFE_FREQUENCY, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_DND.AfeInfo.nNCount);
	WritePrivateProfileString(INI_CATEGORY_DND, INI_ITEM_AFE_N_COUNT, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_DND.AfeInfo.nUCount);
	WritePrivateProfileString(INI_CATEGORY_DND, INI_ITEM_AFE_U_COUNT, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_DND, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [CND]
	sprintf(chStr, "%d", m_TestInfo_CND.bCheckData);
	WritePrivateProfileString(INI_CATEGORY_CND, INI_ITEM_CND_CHECK, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_CND.nDataMax);
	WritePrivateProfileString(INI_CATEGORY_CND, INI_ITEM_CND_ALL_NODE_MAX_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_DND.nDataMin);
	WritePrivateProfileString(INI_CATEGORY_CND, INI_ITEM_CND_ALL_NODE_MIN_SPEC, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_CND, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [HF DND]
	sprintf(chStr, "%d", m_TestInfo_HF_DND.bCheckData);
	WritePrivateProfileString(INI_CATEGORY_HF_DND, INI_ITEM_HF_DND_CHECK, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_HF_DND.nDataMax);
	WritePrivateProfileString(INI_CATEGORY_HF_DND, INI_ITEM_HF_DND_ALL_NODE_MAX_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_HF_DND.nDataMin);
	WritePrivateProfileString(INI_CATEGORY_HF_DND, INI_ITEM_HF_DND_ALL_NODE_MIN_SPEC, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_HF_DND.bCheckDataDiff);
	WritePrivateProfileString(INI_CATEGORY_HF_DND, INI_ITEM_HF_DND_CHECK_GAP, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_HF_DND.nHDiffCenter);
	WritePrivateProfileString(INI_CATEGORY_HF_DND, INI_ITEM_HF_DND_ALL_NODE_H_GAP_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_HF_DND.nVDiffCenter);
	WritePrivateProfileString(INI_CATEGORY_HF_DND, INI_ITEM_HF_DND_ALL_NODE_V_GAP_SPEC, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_HF_DND.AfeInfo.bUseAfeSetting);
	WritePrivateProfileString(INI_CATEGORY_HF_DND, INI_ITEM_AFE_USE_USER_SETTING, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_HF_DND.AfeInfo.nFrequency);
	WritePrivateProfileString(INI_CATEGORY_HF_DND, INI_ITEM_AFE_FREQUENCY, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_HF_DND.AfeInfo.nNCount);
	WritePrivateProfileString(INI_CATEGORY_HF_DND, INI_ITEM_AFE_N_COUNT, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_HF_DND.AfeInfo.nUCount);
	WritePrivateProfileString(INI_CATEGORY_HF_DND, INI_ITEM_AFE_U_COUNT, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_HF_DND, chSeperate, "===============================================", chFile);
	//-------------------------------------------------
	// [HIGH_VOLTAGE]
	sprintf(chStr, "%d", m_TestInfo_HIGH_VOLTAGE.bCheckData);
	WritePrivateProfileString(INI_CATEGORY_HIGH_VOLTAGE, INI_ITEM_HIGH_VOLTAGE_CHECK, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_HIGH_VOLTAGE.pDataMax);
	WritePrivateProfileString(INI_CATEGORY_HIGH_VOLTAGE, INI_ITEM_HIGH_VOLTAGE_MAX_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_HIGH_VOLTAGE.pDataMin);
	WritePrivateProfileString(INI_CATEGORY_HIGH_VOLTAGE, INI_ITEM_HIGH_VOLTAGE_MIN_SPEC, chStr, chFile);
	//--------------------------------------------------
	// [ONLY SELF_DND]
	sprintf(chStr, "%d", m_TestInfo_ONLYSELF_DND.bCheckData);
	WritePrivateProfileString(INI_CATEGORY_ONLYSELF_DND, INI_ITEM_ONLYSELF_DND_CHECK, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_ONLYSELF_DND.nTxDataMax);
	WritePrivateProfileString(INI_CATEGORY_ONLYSELF_DND, INI_ITEM_ONLYSELF_TX_ALL_NODE_MAX_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_ONLYSELF_DND.nTxDataMin);
	WritePrivateProfileString(INI_CATEGORY_ONLYSELF_DND, INI_ITEM_ONLYSELF_TX_ALL_NODE_MIN_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_ONLYSELF_DND.nRxDataMax);
	WritePrivateProfileString(INI_CATEGORY_ONLYSELF_DND, INI_ITEM_ONLYSELF_RX_ALL_NODE_MAX_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_ONLYSELF_DND.nRxDataMin);
	WritePrivateProfileString(INI_CATEGORY_ONLYSELF_DND, INI_ITEM_ONLYSELF_RX_ALL_NODE_MIN_SPEC, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_ONLYSELF_DND, chSeperate, "===============================================", chFile);
	//-------------------------------------------------
	// [SELF_DND]
	sprintf(chStr, "%d", m_TestInfo_SELF_DND.bCheckData);
	WritePrivateProfileString(INI_CATEGORY_SELF_DND, INI_ITEM_SELF_DND_CHECK, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_DND.nTxDataMax);
	WritePrivateProfileString(INI_CATEGORY_SELF_DND, INI_ITEM_SELF_TX_ALL_NODE_MAX_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_DND.nTxDataMin);
	WritePrivateProfileString(INI_CATEGORY_SELF_DND, INI_ITEM_SELF_TX_ALL_NODE_MIN_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_DND.nRxDataMax);
	WritePrivateProfileString(INI_CATEGORY_SELF_DND, INI_ITEM_SELF_RX_ALL_NODE_MAX_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_DND.nRxDataMin);
	WritePrivateProfileString(INI_CATEGORY_SELF_DND, INI_ITEM_SELF_RX_ALL_NODE_MIN_SPEC, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_SELF_DND.bCheckDataDiff);
	WritePrivateProfileString(INI_CATEGORY_SELF_DND, INI_ITEM_SELF_DND_CHECK_GAP, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_DND.nTxHDiffCenter);
	WritePrivateProfileString(INI_CATEGORY_SELF_DND, INI_ITEM_SELF_DND_TX_ALL_NODE_H_GAP_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_DND.nRxHDiffCenter);
	WritePrivateProfileString(INI_CATEGORY_SELF_DND, INI_ITEM_SELF_DND_RX_ALL_NODE_H_GAP_SPEC, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_SELF_DND.AfeInfo.bUseAfeSetting);
	WritePrivateProfileString(INI_CATEGORY_SELF_DND, INI_ITEM_AFE_USE_USER_SETTING, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_DND.AfeInfo.nFrequency_A);
	WritePrivateProfileString(INI_CATEGORY_SELF_DND, INI_ITEM_AFE_FREQUENCY_A, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_DND.AfeInfo.nNCount_A);
	WritePrivateProfileString(INI_CATEGORY_SELF_DND, INI_ITEM_AFE_N_COUNT_A, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_DND.AfeInfo.nUCount_A);
	WritePrivateProfileString(INI_CATEGORY_SELF_DND, INI_ITEM_AFE_U_COUNT_A, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_SELF_DND.AfeInfo.nFrequency_B);
	WritePrivateProfileString(INI_CATEGORY_SELF_DND, INI_ITEM_AFE_FREQUENCY_B, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_DND.AfeInfo.nNCount_B);
	WritePrivateProfileString(INI_CATEGORY_SELF_DND, INI_ITEM_AFE_N_COUNT_B, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_DND.AfeInfo.nUCount_B);
	WritePrivateProfileString(INI_CATEGORY_SELF_DND, INI_ITEM_AFE_U_COUNT_B, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_SELF_DND, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [SELF_SATURATION]
	sprintf(chStr, "%d", m_TestInfo_SELF_SATURATION.bCheckData);
	WritePrivateProfileString(INI_CATEGORY_SELF_SATURATION, INI_ITEM_SELF_SATURATION_CHECK, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_SATURATION.nTxDataMax);
	WritePrivateProfileString(INI_CATEGORY_SELF_SATURATION, INI_ITEM_SELF_TX_ALL_NODE_MAX_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_SATURATION.nTxDataMin);
	WritePrivateProfileString(INI_CATEGORY_SELF_SATURATION, INI_ITEM_SELF_TX_ALL_NODE_MIN_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_SATURATION.nRxDataMax);
	WritePrivateProfileString(INI_CATEGORY_SELF_SATURATION, INI_ITEM_SELF_RX_ALL_NODE_MAX_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_SATURATION.nRxDataMin);
	WritePrivateProfileString(INI_CATEGORY_SELF_SATURATION, INI_ITEM_SELF_RX_ALL_NODE_MIN_SPEC, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_SELF_SATURATION.AfeInfo.bUseAfeSetting);
	WritePrivateProfileString(INI_CATEGORY_SELF_SATURATION, INI_ITEM_AFE_USE_USER_SETTING, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_SATURATION.AfeInfo.nFrequency_A);
	WritePrivateProfileString(INI_CATEGORY_SELF_SATURATION, INI_ITEM_AFE_FREQUENCY_A, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_SATURATION.AfeInfo.nNCount_A);
	WritePrivateProfileString(INI_CATEGORY_SELF_SATURATION, INI_ITEM_AFE_N_COUNT_A, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_SATURATION.AfeInfo.nUCount_A);
	WritePrivateProfileString(INI_CATEGORY_SELF_SATURATION, INI_ITEM_AFE_U_COUNT_A, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_SELF_SATURATION.AfeInfo.nFrequency_B);
	WritePrivateProfileString(INI_CATEGORY_SELF_SATURATION, INI_ITEM_AFE_FREQUENCY_B, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_SATURATION.AfeInfo.nNCount_B);
	WritePrivateProfileString(INI_CATEGORY_SELF_SATURATION, INI_ITEM_AFE_N_COUNT_B, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_SELF_SATURATION.AfeInfo.nUCount_B);
	WritePrivateProfileString(INI_CATEGORY_SELF_SATURATION, INI_ITEM_AFE_U_COUNT_B, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_SELF_SATURATION, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [TRX SHORT]
	sprintf(chStr, "%d", m_bCheckTrxShort);
	WritePrivateProfileString(INI_CATEGORY_TRX_SHORT, INI_ITEM_TRX_SHORT_CHECK, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_TRX_SHORT, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [CURRENT CONSUMPTION]
	sprintf(chStr, "%d", m_TestInfo_CURRENT_CONSUMPTION.Active.bCheckData);
	WritePrivateProfileString(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_CHECK_ACTIVE, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_CURRENT_CONSUMPTION.Active.nThreshould_Max);
	WritePrivateProfileString(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_THR_ACTIVE_MAX, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_CURRENT_CONSUMPTION.Active.nThreshould_Min);
	WritePrivateProfileString(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_THR_ACTIVE_MIN, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_CURRENT_CONSUMPTION.Active.nAvgMs);
	WritePrivateProfileString(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_AVG_MS_ACTIVE, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_CURRENT_CONSUMPTION.Idle.bCheckData);
	WritePrivateProfileString(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_CHECK_IDLE, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_CURRENT_CONSUMPTION.Idle.nThreshould_Max);
	WritePrivateProfileString(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_THR_IDLE, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_CURRENT_CONSUMPTION.Idle.nAvgMs);
	WritePrivateProfileString(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_AVG_MS_IDLE, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_CURRENT_CONSUMPTION.Sleep.bCheckData);
	WritePrivateProfileString(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_CHECK_SLEEP, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_CURRENT_CONSUMPTION.Sleep.nThreshould_Max);
	WritePrivateProfileString(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_THR_SLEEP, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_CURRENT_CONSUMPTION.Sleep.nAvgMs);
	WritePrivateProfileString(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_AVG_MS_SLEEP, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_CURRENT_CONSUMPTION.DeepSleep.bCheckData);
	WritePrivateProfileString(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_CHECK_DEEP_SLEEP, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_CURRENT_CONSUMPTION.DeepSleep.nThreshould_Max);
	WritePrivateProfileString(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_THR_DEEP_SLEEP, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_CURRENT_CONSUMPTION.DeepSleep.nAvgMs);
	WritePrivateProfileString(INI_CATEGORY_CURRENT_CONSUMPTION, INI_ITEM_CURRENT_CONSUMPTION_AVG_MS_DEEP_SLEEP, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_CURRENT_CONSUMPTION, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [MEASURE DELTA]
	sprintf(chStr, "%d", m_TestInfo_MEASURE_DELTA.bCheckData);
	WritePrivateProfileString(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_CHECK, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_MEASURE_DELTA.bCheckAutoStart);
	WritePrivateProfileString(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_CHECK_AUTO_START, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_MEASURE_DELTA.nAutoStartThreshold);
	WritePrivateProfileString(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_AUTO_START_THRESHOLD, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_MEASURE_DELTA.nCount);
	WritePrivateProfileString(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_FRAME_COUNT, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_MEASURE_DELTA.nDataMax);
	WritePrivateProfileString(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_ALL_NODE_MAX_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_MEASURE_DELTA.nDataMin);
	WritePrivateProfileString(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_ALL_NODE_MIN_SPEC, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_MEASURE_DELTA.m_nSelfRxDataMax);
	WritePrivateProfileString(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_ALL_NODE_SELF_RX_MAX_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_MEASURE_DELTA.m_nSelfRxDataMin);
	WritePrivateProfileString(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_ALL_NODE_SELF_RX_MIN_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_MEASURE_DELTA.m_nSelfTxDataMax);
	WritePrivateProfileString(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_ALL_NODE_SELF_TX_MAX_SPEC, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_MEASURE_DELTA.m_nSelfTxDataMin);
	WritePrivateProfileString(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_MEASURE_DELTA_ALL_NODE_SELF_TX_MIN_SPEC, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_MEASURE_DELTA.AfeInfo.bUseAfeSetting);
	WritePrivateProfileString(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_AFE_USE_USER_SETTING, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_MEASURE_DELTA.AfeInfo.nFrequency);
	WritePrivateProfileString(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_AFE_FREQUENCY, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_MEASURE_DELTA.AfeInfo.nNCount);
	WritePrivateProfileString(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_AFE_N_COUNT, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_MEASURE_DELTA.AfeInfo.nUCount);
	WritePrivateProfileString(INI_CATEGORY_MEASURE_DELTA, INI_ITEM_AFE_U_COUNT, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_MEASURE_DELTA, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [HW CALIBRATION]
	sprintf(chStr, "%d", m_TestInfo_HW_CALIBRATION.bCheckData);
	WritePrivateProfileString(INI_CATEGORY_HW_CALIBRATION, INI_ITEM_HW_CALIBRATION_CHECK, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_HW_CALIBRATION.nHwCalibrationMax);
	WritePrivateProfileString(INI_CATEGORY_HW_CALIBRATION, INI_ITEM_HW_CALIBRATION_SPEC, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_HW_CALIBRATION.AfeInfo.bUseAfeSetting);
	WritePrivateProfileString(INI_CATEGORY_HW_CALIBRATION, INI_ITEM_AFE_USE_USER_SETTING, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_HW_CALIBRATION.AfeInfo.nFrequency);
	WritePrivateProfileString(INI_CATEGORY_HW_CALIBRATION, INI_ITEM_AFE_FREQUENCY, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_HW_CALIBRATION.AfeInfo.nNCount);
	WritePrivateProfileString(INI_CATEGORY_HW_CALIBRATION, INI_ITEM_AFE_N_COUNT, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_HW_CALIBRATION.AfeInfo.nUCount);
	WritePrivateProfileString(INI_CATEGORY_HW_CALIBRATION, INI_ITEM_AFE_U_COUNT, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_HW_CALIBRATION, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [JITTER]
	sprintf(chStr, "%d", m_TestInfo_JITTER.bCheckData);
	WritePrivateProfileString(INI_CATEGORY_JITTER, INI_ITEM_JITTER_CHECK, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_JITTER.nJitterDataMax);
	WritePrivateProfileString(INI_CATEGORY_JITTER, INI_ITEM_JITTER_DATA_MAX, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_JITTER.AfeInfo.bUseAfeSetting);
	WritePrivateProfileString(INI_CATEGORY_JITTER, INI_ITEM_AFE_USE_USER_SETTING, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_JITTER.AfeInfo.nFrequency);
	WritePrivateProfileString(INI_CATEGORY_JITTER, INI_ITEM_AFE_FREQUENCY, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_JITTER.AfeInfo.nNCount);
	WritePrivateProfileString(INI_CATEGORY_JITTER, INI_ITEM_AFE_N_COUNT, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_JITTER.AfeInfo.nUCount);
	WritePrivateProfileString(INI_CATEGORY_JITTER, INI_ITEM_AFE_U_COUNT, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_JITTER.bCheckDataByMpztd);
	WritePrivateProfileString(INI_CATEGORY_JITTER, INI_ITEM_JITTER_CHECK_BY_MPZTD, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_JITTER.nJitterReadCount);
	WritePrivateProfileString(INI_CATEGORY_JITTER, INI_ITEM_JITTER_READ_FRAME_COUNT, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_JITTER, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [DIGITAL WHEEL]
	sprintf(chStr, "%d", m_TestInfo_DIGITAL_WHEEL.bCheckData);
	WritePrivateProfileString(INI_CATEGORY_DIGITAL_WHEEL, INI_ITEM_DIGITAL_WHEEL_CHECK, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_DIGITAL_WHEEL, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [Add Test Item]
#if defined(D_USE_CHECK_BUTTON_TEST)
	sprintf(chStr, "%d", m_bCheckGpioButton);
	WritePrivateProfileString(INI_CATEGORY_ADD_TEST_ITEM, INI_ITEM_GPIO_BUTTON_CHECK, chStr, chFile);

	sprintf(chStr, "%d", m_bCheckTouchButton);
	WritePrivateProfileString(INI_CATEGORY_ADD_TEST_ITEM, INI_ITEM_TOUCH_BUTTON_CHECK, chStr, chFile);
	sprintf(chStr, "%d", m_nTouchButton_Threshold);
	WritePrivateProfileString(INI_CATEGORY_ADD_TEST_ITEM, INI_ITEM_TOUCH_BUTTON_TH, chStr, chFile);
#endif

	// ADDITIONAL ITEM
	sprintf(chStr, "%d", m_bPowerOffWhenUsbDetect);
	WritePrivateProfileString(INI_CATEGORY_ADD_TEST_ITEM, INI_ITEM_ADD_TEST_POWER_OFF_WHEN_USB_DETECT, chStr, chFile);

	sprintf(chStr, "%d", m_nPowerCtrlDelayTime);
	WritePrivateProfileString(INI_CATEGORY_ADD_TEST_ITEM, INI_ITEM_ADD_TEST_POWER_CTRL_DELAY_TIME, chStr, chFile);

	sprintf(chStr, "%d", m_bStartAllTestByStartPin);
	WritePrivateProfileString(INI_CATEGORY_ADD_TEST_ITEM, INI_ITEM_ADD_TEST_START_ALL_TEST_BY_START_PIN, chStr, chFile);

	sprintf(chStr, "%d", m_nSolutionResultNotify);
	WritePrivateProfileString(INI_CATEGORY_ADD_TEST_ITEM, INI_ITEM_ADD_TEST_SOLUTION_RESULT_NOFITY, chStr, chFile);

	sprintf(chStr, "%d", m_bCheckVirtualDrawTest);
	WritePrivateProfileString(INI_CATEGORY_ADD_TEST_ITEM, INI_ITEM_ADD_TEST_CHECK_VIRTUAL_DRAW_TEST, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_ADD_TEST_ITEM, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [CHECK NOISE]
	sprintf(chStr, "%d", m_TestInfo_NOISE.m_bCheckNoise);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_NOISE.m_nCheckNoise_Threshold);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_THRESHOLD, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_NOISE.m_nCheckNoise_Interval);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_INTERVAL, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_NOISE.m_bCheckNoiseUserAFE);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_USEUSERAFE, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_NOISE.m_nCheckNoise_ReadTime_Org);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_READTIMES, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_NOISE.m_nCheckNoise_ReadTimes_ScalerCoef);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_READTIMES_SCALER_COEF, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_NOISE.AfeInfo.m_bCheckFrequency1);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_CHECK_FREQUENCY1, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_NOISE.AfeInfo.m_bCheckFrequency2);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_CHECK_FREQUENCY2, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_NOISE.AfeInfo.m_bCheckFrequency3);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_CHECK_FREQUENCY3, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_NOISE.AfeInfo.m_bCheckFrequency4);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_CHECK_FREQUENCY4, chStr, chFile);

	sprintf(chStr, "%d", m_TestInfo_NOISE.AfeInfo.nFrequency);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_FREQUENCY, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_NOISE.AfeInfo.nFrequency1);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_FREQUENCY1, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_NOISE.AfeInfo.nFrequency2);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_FREQUENCY2, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_NOISE.AfeInfo.nFrequency3);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_FREQUENCY3, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_NOISE.AfeInfo.nFrequency4);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_FREQUENCY4, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_NOISE.AfeInfo.nNCount);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_NCOUNT, chStr, chFile);
	sprintf(chStr, "%d", m_TestInfo_NOISE.AfeInfo.nUCount);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_CHECK_NOISE_UCOUNT, chStr, chFile);

	sprintf(chStr, "%d", 0);
	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, INI_ITEM_PRINT_PP_VARIAITON, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_CHECK_NOISE, chSeperate, "===============================================", chFile);

	//-------------------------------------------------
	// [DEV_TOOL]
	sprintf(chStr, "%d", m_bMeasureLdo);
	WritePrivateProfileString(INI_CATEGORY_DEV_TOOL, INI_ITEM_DEV_TOOL_MEASURE_LDO, chStr, chFile);

	WritePrivateProfileString(INI_CATEGORY_DEV_TOOL, chSeperate, "===============================================", chFile);
}

void CTestModeView::TestInfo_SaveTestIniEx(int nType)
{
	char	chFile[1024];
	char	chStr[1024];

	sprintf(chFile, "%s\\%s", m_strConfigDir.GetBuffer(), TEST_CONF_INI);

	if( nType == SAVE_TEST_INI_DOWNLOAD_FIXED_POSITION )
	{
		sprintf(chStr, "%s", m_strFirmwareDir.GetBuffer());
		WritePrivateProfileString(INI_CATEGORY_FW_DOWNLOAD, INI_ITEM_FW_DOWNLOAD_PATH, chStr, chFile);
	}
	else if( nType == SAVE_TEST_INI_LOG_SAVE_FIXED_POSITION )
	{
		sprintf(chStr, "%s", m_strSaveResultDir.GetBuffer());
		WritePrivateProfileString(INI_CATEGORY_LOG, INI_ITEM_LOG_SAVE_RESULT_PATH, chStr, chFile);
	}
}

BOOL CTestModeView::TestInfo_LoadModuleIdList()
{
	CString strFile;
	strFile.Format(_T("%s\\%s"), m_strConfigDir.GetBuffer(), TEST_CONF_INI);

	g_strModuleIdList.RemoveAll();
	g_nModuleIdList.RemoveAll();

	CStdioFile file;
	if( file.Open(strFile, CFile::modeRead) == FALSE )
	{
		return FALSE;
	}

	BOOL bIsExistModuleIdList = FALSE;
	CString strReadLine;

	// find
	while (file.ReadString(strReadLine))
	{
		if (strReadLine.Compare(_T("[MODULE ID LIST]")) == 0)
		{
			bIsExistModuleIdList = TRUE;
			break;
		}
	}

	if (bIsExistModuleIdList == FALSE)
	{
		return FALSE;
	}

	int nEqualPos;

	while (file.ReadString(strReadLine))
	{
		if ((strReadLine.Compare(_T("===============================================")) == 0) || strReadLine.IsEmpty())
			break;

		nEqualPos = strReadLine.Find("=");
		if (nEqualPos == -1)
			continue;

		g_strModuleIdList.Add(strReadLine.Left(nEqualPos));
		g_nModuleIdList.Add(char2dec((char*)(strReadLine.Mid(nEqualPos+1)).GetBuffer()));
	}

	file.Close();

	return TRUE;
}

void CTestModeView::TestInfo_SaveModuleIdList()
{
	CString strFile;
	strFile.Format(_T("%s\\%s"), m_strConfigDir.GetBuffer(), TEST_CONF_INI);

	CStdioFile file;
	if (file.Open(strFile, CFile::modeWrite) == FALSE)
		return;

	file.SeekToEnd();

	file.WriteString("[MODULE ID LIST]\n");

	file.WriteString("NULL=0xFFFF\n");
	file.WriteString("VISIONOX=0xA50A\n");
	file.WriteString("TRULY=0xA51A\n");

	g_strModuleIdList.Add("NULL");		g_nModuleIdList.Add(0xFFFF);
	g_strModuleIdList.Add("VISIONOX");	g_nModuleIdList.Add(0xA50A);
	g_strModuleIdList.Add("TRULY");		g_nModuleIdList.Add(0xA51A);

	file.WriteString("===============================================");

	file.Close();
}

//==============================================================================================
// Spec file

void CTestModeView::TestInfo_LoadTestSpec()
{
	LoadSpecData(DND_MIN_SPEC_INI, m_TestInfo_DND.pDataMin, 0);
	LoadSpecData(DND_MAX_SPEC_INI, m_TestInfo_DND.pDataMax, 30000);
	LoadSpecData(DND_H_GAP_SPEC_INI, m_TestInfo_DND.pHDiff, 100);
	LoadSpecData(DND_V_GAP_SPEC_INI, m_TestInfo_DND.pVDiff, 100);

	LoadSpecData(CND_MIN_SPEC_INI, m_TestInfo_CND.pDataMin, 0);			//add 2022.11.14 for ZTM730 CND
	LoadSpecData(CND_MAX_SPEC_INI, m_TestInfo_CND.pDataMax, 30000);		//add 2022.11.14 for ZTM730 CND

	LoadSpecData(HF_DND_MIN_SPEC_INI, m_TestInfo_HF_DND.pDataMin, 0);
	LoadSpecData(HF_DND_MAX_SPEC_INI, m_TestInfo_HF_DND.pDataMax, 30000);
	LoadSpecData(HF_DND_H_GAP_SPEC_INI, m_TestInfo_HF_DND.pHDiff, 100);
	LoadSpecData(HF_DND_V_GAP_SPEC_INI, m_TestInfo_HF_DND.pVDiff, 100);

	LoadSpecData(SELF_SATURATION_TX_MIN_SPEC_INI, m_TestInfo_SELF_SATURATION.pTxDataMin, 0);
	LoadSpecData(SELF_SATURATION_TX_MAX_SPEC_INI, m_TestInfo_SELF_SATURATION.pTxDataMax, 30000);
	LoadSpecData(SELF_SATURATION_RX_MIN_SPEC_INI, m_TestInfo_SELF_SATURATION.pRxDataMin, 0);
	LoadSpecData(SELF_SATURATION_RX_MAX_SPEC_INI, m_TestInfo_SELF_SATURATION.pRxDataMax, 30000);

	LoadSpecData(ONLY_SELF_DND_TX_MAX_SPEC_INI, m_TestInfo_ONLYSELF_DND.pTxDataMax, 30000);
	LoadSpecData(ONLY_SELF_DND_TX_MIN_SPEC_INI, m_TestInfo_ONLYSELF_DND.pTxDataMin, 0);
	LoadSpecData(ONLY_SELF_DND_RX_MAX_SPEC_INI, m_TestInfo_ONLYSELF_DND.pRxDataMax, 30000);
	LoadSpecData(ONLY_SELF_DND_RX_MIN_SPEC_INI, m_TestInfo_ONLYSELF_DND.pRxDataMin, 0);


	LoadSpecData(SELF_DND_TX_MIN_SPEC_INI, m_TestInfo_SELF_DND.pTxDataMin, 0);
	LoadSpecData(SELF_DND_TX_MAX_SPEC_INI, m_TestInfo_SELF_DND.pTxDataMax, 30000);
	LoadSpecData(SELF_DND_TX_H_GAP_SPEC_INI, m_TestInfo_SELF_DND.pTxHDiff, 100);

	LoadSpecData(SELF_DND_RX_MIN_SPEC_INI, m_TestInfo_SELF_DND.pRxDataMin, 0);
	LoadSpecData(SELF_DND_RX_MAX_SPEC_INI, m_TestInfo_SELF_DND.pRxDataMax, 30000);
	LoadSpecData(SELF_DND_RX_H_GAP_SPEC_INI, m_TestInfo_SELF_DND.pRxHDiff, 100);

	LoadSpecData(SELF_SATURATION_TX_MIN_SPEC_INI, m_TestInfo_SELF_SATURATION.pTxDataMin, 0);
	LoadSpecData(SELF_SATURATION_TX_MAX_SPEC_INI, m_TestInfo_SELF_SATURATION.pTxDataMax, 30000);
	LoadSpecData(SELF_SATURATION_RX_MIN_SPEC_INI, m_TestInfo_SELF_SATURATION.pRxDataMin, 0);
	LoadSpecData(SELF_SATURATION_RX_MAX_SPEC_INI, m_TestInfo_SELF_SATURATION.pRxDataMax, 30000);

	LoadSpecData(MEASURE_DELTA_MIN_SPEC_INI, m_TestInfo_MEASURE_DELTA.pDataMin, 0);
	LoadSpecData(MEASURE_DELTA_MAX_SPEC_INI, m_TestInfo_MEASURE_DELTA.pDataMax, 30000);

	LoadSpecDataEx(MEASURE_DELTA_SELF_RX_MIN_SPEC_INI, m_TestInfo_MEASURE_DELTA.pSelfRxDataMin, 0);
	LoadSpecDataEx(MEASURE_DELTA_SELF_RX_MAX_SPEC_INI, m_TestInfo_MEASURE_DELTA.pSelfRxDataMax, 30000);
	LoadSpecDataEx(MEASURE_DELTA_SELF_TX_MIN_SPEC_INI, m_TestInfo_MEASURE_DELTA.pSelfTxDataMin, 0);
	LoadSpecDataEx(MEASURE_DELTA_SELF_TX_MAX_SPEC_INI, m_TestInfo_MEASURE_DELTA.pSelfTxDataMax, 30000);

	LoadSpecData(DIGITAL_WHEEL_MIN_SPEC_INI, m_TestInfo_DIGITAL_WHEEL.pDataMin, 100);
	LoadSpecData(DIGITAL_WHEEL_MAX_SPEC_INI, m_TestInfo_DIGITAL_WHEEL.pDataMax, 30000);
}

void CTestModeView::TestInfo_SaveTestSpec()
{
	SaveSpecData(DND_MIN_SPEC_INI, m_TestInfo_DND.pDataMin);
	SaveSpecData(DND_MAX_SPEC_INI, m_TestInfo_DND.pDataMax);
	SaveSpecData(DND_H_GAP_SPEC_INI, m_TestInfo_DND.pHDiff);
	SaveSpecData(DND_V_GAP_SPEC_INI, m_TestInfo_DND.pVDiff);

	SaveSpecData(CND_MIN_SPEC_INI, m_TestInfo_CND.pDataMin);	// CND	//add 2022.11.14 for ZTM730 CND
	SaveSpecData(CND_MAX_SPEC_INI, m_TestInfo_CND.pDataMax);	// CND	//add 2022.11.14 for ZTM730 CND

	SaveSpecData(HF_DND_MIN_SPEC_INI, m_TestInfo_HF_DND.pDataMin);
	SaveSpecData(HF_DND_MAX_SPEC_INI, m_TestInfo_HF_DND.pDataMax);
	SaveSpecData(HF_DND_H_GAP_SPEC_INI, m_TestInfo_HF_DND.pHDiff);
	SaveSpecData(HF_DND_V_GAP_SPEC_INI, m_TestInfo_HF_DND.pVDiff);

	SaveSpecData(ONLY_SELF_DND_TX_MAX_SPEC_INI, m_TestInfo_ONLYSELF_DND.pTxDataMax);
	SaveSpecData(ONLY_SELF_DND_TX_MIN_SPEC_INI, m_TestInfo_ONLYSELF_DND.pTxDataMin);
	SaveSpecData(ONLY_SELF_DND_RX_MAX_SPEC_INI, m_TestInfo_ONLYSELF_DND.pRxDataMax);
	SaveSpecData(ONLY_SELF_DND_RX_MIN_SPEC_INI, m_TestInfo_ONLYSELF_DND.pRxDataMin);

	SaveSpecData(SELF_DND_TX_MIN_SPEC_INI, m_TestInfo_SELF_DND.pTxDataMin);
	SaveSpecData(SELF_DND_TX_MAX_SPEC_INI, m_TestInfo_SELF_DND.pTxDataMax);
	SaveSpecData(SELF_DND_TX_H_GAP_SPEC_INI, m_TestInfo_SELF_DND.pTxHDiff);

	SaveSpecData(SELF_DND_RX_MIN_SPEC_INI, m_TestInfo_SELF_DND.pRxDataMin);
	SaveSpecData(SELF_DND_RX_MAX_SPEC_INI, m_TestInfo_SELF_DND.pRxDataMax);
	SaveSpecData(SELF_DND_RX_H_GAP_SPEC_INI, m_TestInfo_SELF_DND.pRxHDiff);

	SaveSpecData(SELF_SATURATION_TX_MIN_SPEC_INI, m_TestInfo_SELF_SATURATION.pTxDataMin);
	SaveSpecData(SELF_SATURATION_TX_MAX_SPEC_INI, m_TestInfo_SELF_SATURATION.pTxDataMax);

	SaveSpecData(SELF_SATURATION_RX_MIN_SPEC_INI, m_TestInfo_SELF_SATURATION.pRxDataMin);
	SaveSpecData(SELF_SATURATION_RX_MAX_SPEC_INI, m_TestInfo_SELF_SATURATION.pRxDataMax);

	SaveSpecData(MEASURE_DELTA_MIN_SPEC_INI, m_TestInfo_MEASURE_DELTA.pDataMin);
	SaveSpecData(MEASURE_DELTA_MAX_SPEC_INI, m_TestInfo_MEASURE_DELTA.pDataMax);

	SaveSpecDataEx(MEASURE_DELTA_SELF_RX_MIN_SPEC_INI, m_TestInfo_MEASURE_DELTA.pSelfRxDataMin);
	SaveSpecDataEx(MEASURE_DELTA_SELF_RX_MAX_SPEC_INI, m_TestInfo_MEASURE_DELTA.pSelfRxDataMax);
	SaveSpecDataEx(MEASURE_DELTA_SELF_TX_MIN_SPEC_INI, m_TestInfo_MEASURE_DELTA.pSelfTxDataMin);
	SaveSpecDataEx(MEASURE_DELTA_SELF_TX_MAX_SPEC_INI, m_TestInfo_MEASURE_DELTA.pSelfTxDataMax);

	SaveSpecData(DIGITAL_WHEEL_MIN_SPEC_INI, m_TestInfo_DIGITAL_WHEEL.pDataMin);
	SaveSpecData(DIGITAL_WHEEL_MAX_SPEC_INI, m_TestInfo_DIGITAL_WHEEL.pDataMax);
}

void CTestModeView::LoadSpecData(const char * pSpecFile, int(*pData)[DEF_MAX_Y_LINE_NUM], int nDefaltValue)
{
	char chFile[1024];
	FILE *Fi = NULL;
	char chArray[255];
	char *FileBuf = NULL;
	int totalSz;
	int Line, Item;
	int ParsingSz;
	int nRet;

	sprintf(chFile, "%s\\%s", m_strConfigDir.GetBuffer(), pSpecFile);

	totalSz = GetFileSize(chFile);
	if (totalSz == 0) {
		goto Error_LoadSpecFile;
	}

	Fi = fopen(chFile, "rt");
	if (Fi == NULL)	{
		goto Error_LoadSpecFile;
	}

	FileBuf = (char *)malloc(totalSz);
	if (FileBuf == NULL) {
		goto Error_LoadSpecFile;
	}

	nRet = (int)fread(FileBuf, 1, totalSz, Fi);
	totalSz = nRet;
	fclose(Fi);

	Line = Item = 0;
	ParsingSz = 0;

	while (1) {
		if (ParsingSz >= totalSz)	break;

		nRet = buf_sscanf((char*)&FileBuf[ParsingSz], chArray);
		ParsingSz += nRet;

		while (1) {
			if (ParsingSz >= totalSz)	break;
			if (nRet == 0) {
				if (FileBuf[ParsingSz] == '\n') {
					if (Item > 0) {
						Item = 0;
						Line++;
					}
				}
				ParsingSz++;
				break;
			}

			if ((chArray[0] >= '0' && chArray[0] <= '9') || chArray[0] == '-')
				break;
		}

		if (nRet > 0) {
			if (buf_char2dec(chArray, &pData[Line][Item]) == false)
				ParsingSz = totalSz;
			else
				Item++;
		}
	}

	if (Fi != NULL)	fclose(Fi);
	ZT_FREE(FileBuf);
	return;

Error_LoadSpecFile:
	if (Fi != NULL)	fclose(Fi);
	ZT_FREE(FileBuf);

	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			pData[x][y] = nDefaltValue;
		}
	}

	SaveSpecData(pSpecFile, pData);
}

void CTestModeView::SaveSpecData(const char * pSpecFile, int(*pData)[DEF_MAX_Y_LINE_NUM])
{
	char chFile[1024];
	FILE *Fi = NULL;
	int x, y;
	CString strData;

	sprintf(chFile, "%s\\%s", m_strConfigDir.GetBuffer(), pSpecFile);

	Fi = fopen(chFile, "wt");
	if (Fi == NULL)	{
		return;
	}

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		strData.Format("");
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			if (y != DEF_MAX_Y_LINE_NUM - 1)
				strData.AppendFormat("%d\t", pData[x][y]);
			else
				strData.AppendFormat("%d", pData[x][y]);
		}
		strData.AppendFormat("\n");
		fwrite(LPCTSTR(strData), 1, strData.GetLength(), Fi);
	}

	if (Fi != NULL) fclose(Fi);
}

void CTestModeView::LoadSpecDataEx(const char * pSpecFile, int * pData, int nDefaltValue)
{
	char chFile[1024];
	FILE *Fi = NULL;
	char chArray[255];
	char *FileBuf = NULL;
	int totalSz;
	int Item;
	int ParsingSz;
	int nRet;

	sprintf(chFile, "%s\\%s", m_strConfigDir.GetBuffer(), pSpecFile);

	totalSz = GetFileSize(chFile);
	if (totalSz == 0) {
		goto err_LoadCheckData;
	}

	Fi = fopen(chFile, "rt");
	if (Fi == NULL) {
		goto err_LoadCheckData;
	}

	FileBuf = (char *)malloc(totalSz);
	if (FileBuf == NULL) {
		goto err_LoadCheckData;
	}

	nRet = (int)fread(FileBuf, 1, totalSz, Fi);
	totalSz = nRet;
	fclose(Fi);

	Item = 0;
	ParsingSz = 0;

	while (1) {
		if (ParsingSz >= totalSz)	break;

		nRet = buf_sscanf((char*)&FileBuf[ParsingSz], chArray);
		ParsingSz += nRet;

		while (1) {
			if (ParsingSz >= totalSz)	break;
			if (nRet == 0) {
				if (FileBuf[ParsingSz] == '\n') {
					if (Item > 0) {
						Item = 0;
						break;
					}
				}
				ParsingSz++;
				break;
			}

			if ((chArray[0] >= '0' && chArray[0] <= '9') || chArray[0] == '-')
				break;
		}

		if (nRet > 0) {
			if (buf_char2dec(chArray, &pData[Item]) == false)
				ParsingSz = totalSz;
			else
				Item++;
		}
	}

	if (Fi != NULL)	fclose(Fi);
	ZT_FREE(FileBuf);
	return;

err_LoadCheckData:
	if (Fi != NULL)	fclose(Fi);
	ZT_FREE(FileBuf);

	int y;
	for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
		pData[y] = nDefaltValue;
	}

	SaveSpecDataEx(pSpecFile, pData);
}

void CTestModeView::SaveSpecDataEx(const char * pSpecFile, int * pData)
{
	char chFile[1024];
	FILE *Fi = NULL;
	int y;
	CString strData;

	sprintf(chFile, "%s\\%s", m_strConfigDir.GetBuffer(), pSpecFile);

	Fi = fopen(chFile, "wt");
	if (Fi == NULL) {
		return;
	}

	strData.Format("");
	for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
		if (y != DEF_MAX_Y_LINE_NUM - 1)
			strData.AppendFormat("%d\t", pData[y]);
		else
			strData.AppendFormat("%d", pData[y]);
	}
	strData.AppendFormat("\n");
	fwrite(LPCTSTR(strData), 1, strData.GetLength(), Fi);

	if (Fi != NULL) fclose(Fi);
}


