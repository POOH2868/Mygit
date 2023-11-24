// TestModeView_Test.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "TestModeView.h"

#include "TestModeView_Test.h"
#include "TestModeView_TestInfo.h"

#include "util.h"

#include "UserNumberDlg.h"
#include "popup_noti.h"

#include <share.h>				// _SH_DENYWR

#define	RETRY_COUNT							100000

// IDC_BUTTON_START_TEST_ALL event handler
void CTestModeView::OnBnClickedButtonStartTestAll()
{
	if (m_bUseBarcode == TRUE)
	{
		if (m_bAutoStartTest == TRUE)
		{
			CString strMsg;
			strMsg.Format("Barcode Mode&Auto Start Mode Enable.");
			AfxMessageBox(strMsg);
			return;
		}
		else
		{
			// start test for only 1st device
			CUsbHid * pUsbHid = GetUsbHidByDevIndex(0);
			if (pUsbHid && pUsbHid->IsTestAvailable())
			{
				// set barcode
				pUsbHid->m_strBarcode.Format("%s", m_cBarcode);

				TSM_StartTest(0);
			}
			return;
		}
	}

	TSM_StartTest(-1);
}

LRESULT CTestModeView::OnStartTest_ByFwAutoDownload(WPARAM wParam, LPARAM lParam)
{
	if (m_bTestEnterByFwAutoDownload == FALSE)
		return 0L;

	TSM_StartTest(-1);

	return 1L;
}

LRESULT CTestModeView::OnCallbackByBarcodeSetting(WPARAM wParam, LPARAM lParam)
{
	int nEventID = (int)wParam;

	if (m_bUseBarcode == FALSE || nEventID == E_BARCODE_SETTING_HIDE_DIALOG)
	{
		// hide dialog
		BarcodeSetting_ShowDialog(FALSE);

		UsbHidDevice_StartSurveyTimer(TRUE);
	}
	else if (nEventID == E_BARCODE_SETTING_START_TEST)
	{
		int nIndex = (int)lParam;

		if (nIndex == -1)
		{// start TSM test - ALL
			TSM_StartTest(-1);
		}
		else
		{// start TSM test - PARTIAL
			// start test for only 1st device
			CUsbHid * pUsbHid = GetUsbHidByDevIndex(nIndex);
			if (pUsbHid == NULL || pUsbHid->IsTestAvailable() == FALSE)
			{
				BarcodeSetting_TestCompleted(nIndex, TRUE);	// ERROR
				return 0L;
			}

			if (m_bAleadyTestEnter)
				TSM_StartTest_Individual(nIndex);
			else
				TSM_StartTest(nIndex);
		}
	}

	return 1L;
}

BOOL CTestModeView::IsAutoFwDownload_Condition()
{
	if( (m_bTestEnterByFwAutoDownload == FALSE)
		&& (m_bDownload == FALSE)
		&& m_bCheckVersion
		&& m_bCheckAutoFwDownload)
	{
		return TRUE;
	}

	return FALSE;
}

// If nDeviceIndex is -1, START_ALL. Otherwise, nDeviceIndex  // ----> 테스트 최초 실행 시 한번 호출됨.
void CTestModeView::TSM_StartTest(int nDeviceIndex)
{
	if (m_bAleadyTestReady == TRUE)		return;
	if (m_bAleadyTestEnter == TRUE)		return;

	m_bAleadyTestReady = TRUE;

	m_nStartTestIndex = nDeviceIndex;

	if (m_bUseRetry && m_bUseIFE)
	{
		switch (m_eAllRetryStatus)
		{
		case eAllRetryStatus_Nothing:
			m_nAllRetryCopyCount = m_nAllRetryCount;
			m_eAllRetryStatus = eAllRetryStatus_Running;
			break;

		case eAllRetryStatus_Running:
			//Running Log Message
			break;
		}
	}

	TSM_StartTest_Init(nDeviceIndex);

	m_bAleadyTestReady = FALSE;
}

void CTestModeView::TSM_StartTest_Init(int nDeviceIndex)	// MP TooL 3.0
{
	CString strFileDir;
	BOOL bFoundDevice = FALSE;
	int n;

	if (m_bAleadyTestEnter == TRUE)	return;

	if (m_bCheckUserNumber == TRUE && m_bExistDevice == TRUE && m_strUserNumber.IsEmpty())
	{
		BOOL bUserNumber = FALSE;

		CUserNumberDlg dlg;

		if (dlg.DoModal() == IDOK)
		{
			m_strUserNumber = dlg.m_strUserNumber;

			if (m_strUserNumber.IsEmpty() == FALSE)
			{
				bUserNumber = TRUE;
			}
		}

		if (bUserNumber == FALSE)
		{
			AfxMessageBox("Please. Check the user number.");

			if (m_bUseBarcode == TRUE)
			{
				m_nBarcodeIdx = 0;
				m_nBarcodeChIdx = 0;
			}
			return;
		}
	}

	if( IFE_MODE_OFF && m_bIsDefaultConfig )
	{
		if (AfxMessageBox(_T("Do you want to testing using the default configuration file?"), MB_OKCANCEL) != IDOK)
			return;
	}

	KillAllTimer(TRUE);

	MySleep(10);

	m_nIcReivsion = char2dec((char*)m_strIcRevision.GetBuffer());
	m_nHwID = char2dec((char*)m_strHwID.GetBuffer());
	m_nMajorVersion = char2dec((char*)m_strMajorVersion.GetBuffer());
	m_nMinorVersion = char2dec((char*)m_strMinorVersion.GetBuffer());
	m_nRegisterVersion = char2dec((char*)m_strRegisterVersion.GetBuffer());
	
	SemaWait(g_hSemaphore);

	if (m_bDownload || m_bTestEnterByFwAutoDownload || m_bVerifyFirmware)
		m_infoDatabase[TEST_FW_DOWNLOAD].m_image = IMAGE_LIST_ENABLE;
	else
		m_infoDatabase[TEST_FW_DOWNLOAD].m_image = IMAGE_LIST_DISABLE;

	m_picPass.ShowWindow(FALSE);
	m_picFail.ShowWindow(FALSE);
	m_picStandBy.ShowWindow(FALSE);

	SemaWait(g_hInfoSemaphore);
	m_TestDevIndex.RemoveAll();
	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);

	// Find HID devices
	if (nDeviceIndex == -1)
	{
		int retryTime = 1;	// 10

		MyDelayMs(1);
		while (retryTime-- > 0)
		{
			if (FindUsbHid() == TRUE)
			{
				bFoundDevice = TRUE;
				break;
			}
			MyDelayMs(200);
		};
	}
	else
	{
		CUsbHid * pUsbHid = GetUsbHidByDevIndex(nDeviceIndex);
		if (pUsbHid)
		{
			bFoundDevice = TRUE;
		}
	}

	if (bFoundDevice == FALSE)
	{
		TSM_TestEnd();
		AfxMessageBox("CANNOT FIND TOUCH DEVICE");
		MyDelayMs(1);
		return;
	}

	m_bAleadyTestEnter = TRUE;

	m_nTestStartTime = GetTimeInMillis();

	m_StartOleDateTime = COleDateTime::GetCurrentTime();

	// Create log directory when first test start
	if (m_bAdditionalLOG)
	{
		strFileDir.Format("%s\\SAVE_LOG", m_strSaveResultDir);
	}
	else
	{
		strFileDir.Format("%s\\DATA", m_strSaveResultDir);

		if (PathFileExists(strFileDir) == FALSE)
			CreateDirectory((LPCTSTR)strFileDir, NULL);

		strFileDir.Format("%s\\DATA\\%02d%02d%02d", m_strSaveResultDir, m_StartOleDateTime.GetYear() % 100, m_StartOleDateTime.GetMonth(), m_StartOleDateTime.GetDay());
	}

	if (PathFileExists(strFileDir) == FALSE && CreateDirectory((LPCTSTR)strFileDir, NULL) == FALSE)
	{
		CString strTemp = "";
		strTemp.Format("Cannot create directory (%s)", strFileDir);
		AfxMessageBox(strTemp);

		TSM_TestEnd();
		return;
	}

	EnableStartButton(FALSE);
	EnableOptionButton(FALSE);

	m_nTestStepMin = 0;

#if defined(D_USE_CHECK_BUTTON_TEST)
	memset(&m_GpioButtonStatus, 0x00, sizeof(BUTTON_STATUS));
	memset(&m_TouchButtonStatus, 0x00, sizeof(BUTTON_STATUS));
#endif

	if(INDIVIDUAL_TEST_START_OFF)
		TestResultDesc_ClearAll();

	// Verify detected devices
	for (n = 0; n < m_nMaxDeviceIndex; n++)
	{
		CUsbHid * pUsbHid = GetUsbHidByDevIndex(n);
		if (pUsbHid == NULL)	continue;

		pUsbHid->m_bTesting = FALSE;
		pUsbHid->m_bTestResult = FALSE;
		pUsbHid->m_bDownloadResult = FALSE;
		pUsbHid->m_nResult = RESULT_NONE;
		pUsbHid->m_strResult = "";

		if (pUsbHid->IsTestAvailable() == FALSE)
			continue;
		else if (nDeviceIndex != -1 && pUsbHid->m_nDevIndex != nDeviceIndex)
			continue;

		// check version check OK when previous TEST
		if (m_bTestEnterByFwAutoDownload && (pUsbHid->m_bErrorVersionCheck == FALSE))
			continue;

		pUsbHid->m_nTestStep = 0;
		pUsbHid->m_bErrorVersionCheck = FALSE;

		pUsbHid->m_bBurstUpgrade = m_bBurstUpgrade;

		// add testing device list
		SemaWait(g_hInfoSemaphore);
		m_TestDevIndex.Add(n);
		::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);

		pUsbHid->m_bTesting = TRUE;
		pUsbHid->m_bSingleTest = (nDeviceIndex == -1) ? FALSE: TRUE;
		pUsbHid->m_OleDateTime = m_StartOleDateTime;

#if defined(D_USE_CHECK_BUTTON_TEST)
		m_GpioButtonStatus.nStatus[n] = E_BUTTON_TEST_ON;
		m_TouchButtonStatus.nStatus[n] = E_BUTTON_TEST_ON;
#endif

		ClearTsmTestMessageOnList(pUsbHid->m_nDevIndex);

		if(INDIVIDUAL_TEST_START_ON)
			TestResultDesc_ClearMessage(pUsbHid->m_nDevIndex);

		SetListState(pUsbHid, n + FIRST_DEVICE_INDEX, ITEM_STATE_ENABLE);

		if (nDeviceIndex != -1)
			break;
	}

	TestResultDesc_SetCurSel(nDeviceIndex);

	MyDelayMs(100);

	if(INDIVIDUAL_TEST_START_OFF)
	{
		TestResultDesc_AddMessage(0, ZT_COLOR_BLACK, "TEST START : %04d%02d%02d_%02d%02d%02d\r\n", m_StartOleDateTime.GetYear(), m_StartOleDateTime.GetMonth(), m_StartOleDateTime.GetDay(), m_StartOleDateTime.GetHour(), m_StartOleDateTime.GetMinute(), m_StartOleDateTime.GetSecond());
		UpdateProgressBar(NULL);
	}

	TSM_SetUpgradeInfo();

	if (m_bDownload || m_bTestEnterByFwAutoDownload || m_bVerifyFirmware)
	{
		g_FwDownloadInfo.bFwBinaryLoad = TSM_LoadTouchFirmware(&m_strDownloadErrorMsg);
	}

	// Request TSM Test to individual thread
	for (n = 0; n < m_nMaxDeviceIndex; n++)
	{
		CUsbHid * pUsbHid = GetUsbHidByDevIndex(n);

		if (pUsbHid == NULL || pUsbHid->IsTestAvailable() == FALSE || pUsbHid->m_bTesting == FALSE)
			continue;

		if(INDIVIDUAL_TEST_START_ON)
		{
			TestResultDesc_AddMessage(pUsbHid->m_nDevIndex, ZT_COLOR_BLACK, "TEST START : %04d%02d%02d_%02d%02d%02d\r\n", m_StartOleDateTime.GetYear(), m_StartOleDateTime.GetMonth(), m_StartOleDateTime.GetDay(), m_StartOleDateTime.GetHour(), m_StartOleDateTime.GetMinute(), m_StartOleDateTime.GetSecond());
			UpdateProgressBar(pUsbHid);
		}

		pUsbHid->StartTsmTest();
	}
}

// Test 중 장치별로 개별 테스트 시작을 위한 함수
void CTestModeView::TSM_StartTest_Individual(int nDeviceIndex)
{
	CUsbHid * pUsbHid = GetUsbHidByDevIndex(nDeviceIndex);

	if (pUsbHid == NULL || INDIVIDUAL_TEST_START_OFF || pUsbHid->IsTestAvailable() == FALSE )
		return;

	if (pUsbHid->m_bTesting)
		return;

	// add testing device list
	SemaWait(g_hInfoSemaphore);
	m_TestDevIndex.Add(nDeviceIndex);
	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);

	SetListState(pUsbHid, pUsbHid->m_nDevIndex + FIRST_DEVICE_INDEX, ITEM_STATE_ENABLE);

	TestResultDesc_SetCurSel(nDeviceIndex);

	pUsbHid->m_nResult = RESULT_NONE;
	pUsbHid->m_strResult = "";

	pUsbHid->m_nTestStep = 0;
	pUsbHid->m_bErrorVersionCheck = FALSE;

	pUsbHid->m_bTesting = TRUE;
	pUsbHid->m_bSingleTest = TRUE;

	// clear previous test result
	ClearTsmTestMessageOnList(nDeviceIndex);

	TestResultDesc_ClearMessage(pUsbHid->m_nDevIndex);

	COleDateTime OleDateTime = COleDateTime::GetCurrentTime();

	TestResultDesc_AddMessage(pUsbHid->m_nDevIndex, ZT_COLOR_BLACK, "TEST START : %04d%02d%02d_%02d%02d%02d\r\n", OleDateTime.GetYear(), OleDateTime.GetMonth(), OleDateTime.GetDay(), OleDateTime.GetHour(), OleDateTime.GetMinute(), OleDateTime.GetSecond());
	UpdateProgressBar(pUsbHid);

	pUsbHid->StartTsmTest();
}

void CTestModeView::TSM_StartTest_ByBarcode()
{
	int nBarcodeLength = GetBarcodeLength();

	if (m_nBarcodeChIdx == nBarcodeLength)       //Key 입력수와 Length가 동일한 경우
	{
		m_cBarcode[m_nBarcodeIdx] = '\0';

		if (m_bUseBarcode == TRUE && m_bAutoStartTest == TRUE)
		{
			if (m_bAleadyTestEnter == FALSE)
			{
#if 0
				OnBnClickedButtonStartTestAll();
#else
				// start test for only 1st device
				CUsbHid * pUsbHid = GetUsbHidByDevIndex(0);
				if (pUsbHid && pUsbHid->IsTestAvailable())
				{
					pUsbHid->m_strBarcode.Format("%s", m_cBarcode);

					TSM_StartTest(0);
				}
#endif
			}
		}
	}
	else
	{											//Key 입력수와 Length가 동일하지 않은 경우
		CString strMsg;

		if (m_bUseBarcode == FALSE)
		{
			strMsg.Format("Barcode Mode Disable");
			AfxMessageBox(strMsg);
		}
		else
		{
			if (nBarcodeLength == 0)
			{
				strMsg.Format("The length of the barcode is different from the set value. \nSet Max Length = %d / Key Input Length = %d", m_nBarcode_Length, m_nBarcodeIdx);
				AfxMessageBox(strMsg);
			}
			else
			{
				strMsg.Format("The length of the barcode is different from the set value. \nSet Length = %d / Key Input Length = %d", nBarcodeLength, m_nBarcodeIdx);
				AfxMessageBox(strMsg);
			}

			m_nBarcodeChIdx = 0;
			m_nBarcodeIdx = 0;
		}
	}
}

BOOL CTestModeView::TSM_StartTest_ByStartPin()
{
	BOOL bStartTest = FALSE;

	if (GetTestAvailableCount() == 0)
		return FALSE;

	// Check START_PIN - for TSM TEST by START button in MP TOOL
	for (int n = 0; n < m_nMaxDeviceIndex; n++)
	{
		CUsbHid * pUsbHid = GetUsbHidByDevIndex(n);
		if (pUsbHid == NULL)	continue;

		if (pUsbHid->IsTestAvailable() == FALSE)
			continue;

		if (pUsbHid->m_bTesting)
			continue;

		if (pUsbHid->ReadStartPinStatus())
		{
			if (m_bAleadyTestEnter)
			{
				TSM_StartTest_Individual(n);
			}
			else
			{
				bStartTest = TRUE;

				if(m_bStartAllTestByStartPin)
					TSM_StartTest(-1);
				else
					TSM_StartTest(n);
			}

			break;
		}
	}

	return bStartTest;
}


//==============================================================================================
// 각 장치마다 TSM TEST 를 시작하는 함수. 장치별 쓰레드에서 호출
//==============================================================================================

void CTestModeView::TSM_MainTest(CUsbHid * pUsbHid)
{
	CString strRet;

	int nRetry = m_nDownloadRetryCount;

	if (pUsbHid == NULL)		return;

	// Initialzie test related variables
	pUsbHid->m_strResult.Format("");
	pUsbHid->m_strLog.Format("");
	pUsbHid->m_strLogging.Format("");

	pUsbHid->TestErrDesc_RemoveAll();
	pUsbHid->TestResultDesc_RemoveAll();

	pUsbHid->m_bTestResult = TRUE;
	pUsbHid->m_bDownloadResult = TRUE;
	pUsbHid->m_bErrorBeforeTest = TRUE;
	pUsbHid->m_bButtonTest_FirstDevice = FALSE;

	pUsbHid->m_nResult = RESULT_PASS;

	pUsbHid->m_nTestStep = 0;

	if (m_bAdditionalLOG)
	{
		sprintf(pUsbHid->m_cAsicID_0, "%s", STRING_INVALID_ASIC_ID);
		sprintf(pUsbHid->m_cAsicID_1, "%s", STRING_INVALID_ASIC_ID);
	}

	// Save start time
	if (pUsbHid->m_bSingleTest == FALSE)
	{
		pUsbHid->m_nTestStartTime = m_nTestStartTime;
	}
	else
	{
		pUsbHid->m_OleDateTime = COleDateTime::GetCurrentTime();

		pUsbHid->m_nTestStartTime = GetTimeInMillis();
	}

	u16 usbJigHwVerion, usbJigSwVerion;
	if (pUsbHid->Jig_GetVersion(&usbJigHwVerion, &usbJigSwVerion) != 0)
	{
		// apply delay time after CloseHidHandle()
		MyDelayMs(1000);

		// when TSM test is started, handle of HID device should re-create.
		if (IsExistUsbHid(pUsbHid, TRUE) == FALSE)
		{
			pUsbHid->m_bTestResult = FALSE;
			Log_Append(pUsbHid, RESULT_FAIL);
			DisplayResultFailToList(pUsbHid, "FAIL : HID DEVICE COMMUNICATION ERROR");

			DisplayResultByTestStop(pUsbHid);

			// Test End
			TSM_SubTestEnd(pUsbHid);
			return;
		}
	}

	// ALL LED OFF
	pUsbHid->SuccessLedControl(FALSE);
	pUsbHid->RunLedControl(FALSE);
	pUsbHid->FailLedControl(FALSE);
	pUsbHid->PowerControl(FALSE);
	pUsbHid->ControlGpioPb15(FALSE);

	//-----------------------------------------------
	// 01_DETECTED
	int nError = MpTool_PowerControl(pUsbHid, TRUE, m_bForceUpgrade && m_bDownload);
	if (nError != Z_HID_OK)
	{
		pUsbHid->m_bTestResult = FALSE;
		Log_Append(pUsbHid, RESULT_FAIL);

		if (nError == Z_ERR_POWER_CONTROL)
		{
			DisplayResultFailToList(pUsbHid, "FAIL : POWER CONTROL");
		}
		else
		{
			if (GetUseIFE())
			{ // IFE
				if (pUsbHid->m_nChecksum == CHECKSUM_PATTERN_PASS)
					DisplayResultFailToList(pUsbHid, "FAIL : CHECK POWER SEQUENCE (0x%04X)", pUsbHid->GetLastError());
				else
					DisplayResultFailToList(pUsbHid, "FAIL : CHECK POWER SEQUENCE - CHECKSUM ERROR (0x%04X)", pUsbHid->GetLastError());
			}
			else
			{
				DisplayResultFailToList(pUsbHid, "FAIL : CHECK POWER SEQUENCE (0x%04X)", pUsbHid->GetLastError());
			}
		}

		UpdateProgressBar(pUsbHid);
		goto exit_tsm_test;
	}
	MyDelayMs(100);

	//-----------------------------------------------
	if (pUsbHid->GetProtocolMode() != 0)
	{
		AfxMessageBox("Cannot read ""REG_PROTOCOL_MODE""!");
		goto exit_tsm_test;;
	}
	//-----------------------------------------------

	// CHECK IC SIGNATURE
	if (Check_Ic_Signature(pUsbHid) == FALSE)
	{
		pUsbHid->m_bTestResult = FALSE;
		Log_Append(pUsbHid, RESULT_FAIL);
		DisplayResultFailToList(pUsbHid, "IC SIGNATURE [0x%04X] IS NOT [0x%04X]", pUsbHid->m_CapInfo.icSignature, gCapInfo.icSignature);

		UpdateProgressBar(pUsbHid);
		goto exit_tsm_test;
	}

	// Do not remove comment
	//Log_Append(pUsbHid, RESULT_PASS);
	//DisplayResultToList(pUsbHid, RESULT_PASS);

	//-----------------------------------------------
	// 02_Firmware Download
	if (m_bDownload || m_bTestEnterByFwAutoDownload || m_bVerifyFirmware)
	{
		// display "01_DETECTED" result
		Log_Append(pUsbHid, RESULT_PASS);
		DisplayResultToList(pUsbHid, RESULT_PASS);

		pUsbHid->m_nTestStep = 1;
		UpdateProgressBar(pUsbHid);

HID_RETRY_FIRMWARE_DOWNLOAD:

		if ( g_FwDownloadInfo.bFwBinaryLoad == FALSE )
		{// Load firmware is failed
			pUsbHid->m_bDownloadResult = FALSE;

			DisplayResultFailToList(pUsbHid, m_strDownloadErrorMsg);
		}
		else if (m_bVerifyFirmware)
		{
			pUsbHid->m_bDownloadResult = TSM_VerifyTouchFirmware(pUsbHid);

			if (pUsbHid->m_bDownloadResult && (m_bSaveInfo && m_nSaveInfoLocation == SAVE_INFO_IUM))
			{
				if (TSM_VerifyIUM(pUsbHid) == FALSE)
				{
					pUsbHid->m_bDownloadResult = FALSE;

					DisplayResultFailToList(pUsbHid, "FAIL : Verify IUM FAIL");
				}
			}
		}
		else
		{
			if (m_bCheckEepromBit)
			{
				pUsbHid->m_bDownloadResult = TSM_CheckEepromBit(pUsbHid, m_bUseNvmPgmVerify);
			}

			if (pUsbHid->m_bDownloadResult)//
			{
				pUsbHid->m_bDownloadResult = TSM_UpgradeTouchFirmware(pUsbHid);
			}
		}
		//MySleep(10);

		if (pUsbHid->m_bDownloadResult)
		{
			//Log_Append(pUsbHid, RESULT_PASS);
		}
		else
		{// firmware upgrade failed
			pUsbHid->m_bTestResult = FALSE;
			Log_Append(pUsbHid, RESULT_FAIL);

			pUsbHid->SuccessLedControl(FALSE);
			pUsbHid->FailLedControl(TRUE);

			pUsbHid->m_nResult = RESULT_FAIL;
			goto exit_tsm_test;
		}

		// when only FW download, do not power control because of mismatching of i2c address.
		if (!(m_nEnableTestCount == 2 && (m_bDownload && !m_bSaveInfo)))
		{
			BOOL bRelVerError = FALSE;

			//-----------------------------------------------
			// CHECK POWER CONTROL & POWER SEQUENCE
			nError = MpTool_PowerSequenceAfterReset(pUsbHid);

			if(m_bHidMode && nError == Z_HID_OK)
			{
				if (pUsbHid->GetTouchConfigEx(REG_DATA_VERSION, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_DATA_VERSION]) != 0)
					nError = Z_HID_FAIL;
				else if (pUsbHid->m_CapInfo.icRegData[REG_DATA_VERSION] != m_nRegisterVersion)
				{
					nError = Z_HID_FAIL;
					bRelVerError = TRUE;
				}
			}

			if (nError != Z_HID_OK)
			{
				if(m_bHidMode)
				{
					if(GetUseIFE()
					&& m_bDownload
					&& g_FwDownloadInfo.bFwBinaryLoad == TRUE
					&& m_bVerifyFirmware == FALSE
					&& m_bCheckEepromBit == FALSE
					&& nRetry > 0)
					{
						nRetry--;
						goto HID_RETRY_FIRMWARE_DOWNLOAD;
					}
				}

				pUsbHid->m_bTestResult = FALSE;
				Log_Append(pUsbHid, RESULT_FAIL);

				if (nError == Z_ERR_POWER_CONTROL)
				{
					DisplayResultFailToList(pUsbHid, "FAIL : POWER CONTROL");
				}
				else
				{
					if (GetUseIFE())
					{ // IFE
						if (bRelVerError)
							DisplayResultFailToList(pUsbHid, "REG_REGISTER_VERSION [0x%04X] IS NOT REF[0x%04X]", pUsbHid->m_CapInfo.icRegData[REG_DATA_VERSION] & 0xFFFF, m_nRegisterVersion & 0xFFFF);
						else if (pUsbHid->m_nChecksum == CHECKSUM_PATTERN_PASS)
							DisplayResultFailToList(pUsbHid, "FAIL : CHECK POWER SEQUENCE (0x%04X)", pUsbHid->GetLastError());
						else
							DisplayResultFailToList(pUsbHid, "FAIL : CHECK POWER SEQUENCE - CHECKSUM ERROR (0x%04X)", pUsbHid->GetLastError());
					}
					else
					{
						DisplayResultFailToList(pUsbHid, "FAIL : CHECK POWER SEQUENCE (0x%04X)", pUsbHid->GetLastError());
					}
				}
				goto exit_tsm_test;
			}

			//-----------------------------------------------
			// Save Info
			if (m_bDownload && m_bSaveInfo)
			{
				if (m_nSaveInfoLocation == SAVE_INFO_EEPROM)
					nError = TSM_SaveEeprom(pUsbHid);
				else
					nError = TSM_SaveIUM(pUsbHid);

				if (nError == FALSE)
				{
					pUsbHid->m_bTestResult = FALSE;
					Log_Append(pUsbHid, RESULT_FAIL);
					DisplayResultFailToList(pUsbHid, "FAIL : SAVE INFO");
					goto exit_tsm_test;
				}

				// check MODULE ID
				if (m_nSaveInfoLocation == SAVE_INFO_EEPROM)
				{
					int nModuleID = char2dec((char*)m_strModuleID.GetBuffer());

					if (CheckModuleID(pUsbHid, nModuleID) == FALSE)
					{
						pUsbHid->m_bTestResult = FALSE;
						Log_Append(pUsbHid, RESULT_FAIL);
						DisplayResultFailToList(pUsbHid, "MODULE_ID [0x%04X] IS NOT [0x%04X]", pUsbHid->m_nModuleID & 0xFFFF, nModuleID);
						goto exit_tsm_test;
					}
				}
			}
		}
	}

	pUsbHid->m_strResult = "";

	if (m_nEnableTestCount == 2 && (m_bVerifyFirmware || (m_bDownload && !m_bSaveInfo)))
	{
		// Download only mode.
		Log_Append(pUsbHid, RESULT_PASS);
		DisplayResultToList(pUsbHid, RESULT_PASS);

		DisplayResultByTestStop(pUsbHid);
	}
	else
	{
		pUsbHid->m_bErrorBeforeTest = FALSE;

		//-----------------------------------------------------------
		// TSM_SubTest
		pUsbHid->m_bTestResult = TSM_SubTest(pUsbHid);
		//-----------------------------------------------------------
	}

	GetDlgItem(IDC_LIST_INFO)->SetFocus();

exit_tsm_test:

	if (pUsbHid->m_bTestResult == TRUE)
	{
		pUsbHid->SuccessLedControl(TRUE);
		pUsbHid->FailLedControl(FALSE);
	}
	else
	{
		pUsbHid->SuccessLedControl(FALSE);
		pUsbHid->FailLedControl(TRUE);
	}

	MpTool_PowerControl(pUsbHid, FALSE);

	if (pUsbHid->m_bErrorBeforeTest)
	{
		DisplayResultByTestStop(pUsbHid);
	}

	// Test End
	TSM_SubTestEnd(pUsbHid);
}

void CTestModeView::TSM_SetUpgradeInfo()
{
	ZT_FREE(g_FwDownloadInfo.pFwBinaryData);

	memset(&g_FwDownloadInfo, 0x00, sizeof(T_FW_DOWNLOAD_INFO));

	// set page size, fuzing delay
	switch( IC_SIGNATURE[g_nIcType] )
	{
		case 0xE700:	//ZT7554
			g_FwDownloadInfo.icPageSize    = 128;
			g_FwDownloadInfo.icFuzingDelay = 8;			// ZTD : 10
			break;
		case 0xE548:	//ZT7548
		case 0xE545:	//ZTL545
			g_FwDownloadInfo.icPageSize    = 64;
			g_FwDownloadInfo.icFuzingDelay = 6;			// ZTD : 10
			break;
		case 0xE538:	//ZT7538
		case 0xE638:	//ZT7538S
			g_FwDownloadInfo.icPageSize    = 64;
			g_FwDownloadInfo.icFuzingDelay = 6;			// ZTD : 10
			break;
		case 0xE532:	//ZT7532S = ZT7528 = ZTW522
			g_FwDownloadInfo.icPageSize    = 64;
			g_FwDownloadInfo.icFuzingDelay = 8;			// ZTD : 10
			break;
		case 0xE628:	//ZTW523 = ZT2628 = ZTM620
			g_FwDownloadInfo.icPageSize    = 64;
			g_FwDownloadInfo.icFuzingDelay = 10;		// ZTD : 20
			break;
		case 0xE622:	//ZTW622 = ZT2717
		case 0xE623:	//ZTW623
			g_FwDownloadInfo.icPageSize    = 128;
			g_FwDownloadInfo.icFuzingDelay = 15;		// ZTD : 20
			break;
		case 0xE630:	//ZTM630
		case 0xE730:	//ZTM730
		case 0xE722:	//ZTW722
		case 0xE137:  //GTY137
			g_FwDownloadInfo.icPageSize    = 128;
			g_FwDownloadInfo.icFuzingDelay = 20;
			break;
		case 0x650E:	//ZT7650M
		case 0xE760:	//ZTL760
			g_FwDownloadInfo.icPageSize    = 128;
			g_FwDownloadInfo.icFuzingDelay = 20;
			break;
		case 0xE650:	//ZT7650 = ZTL650
			g_FwDownloadInfo.icPageSize    = 1024;
			g_FwDownloadInfo.icFuzingDelay = 30;
			break;
		case 0x650C:	//ZT7650C
			g_FwDownloadInfo.icPageSize    = 128;
			g_FwDownloadInfo.icFuzingDelay = 10;
			break;
		default:
			g_FwDownloadInfo.icPageSize    = 64;
			g_FwDownloadInfo.icFuzingDelay = 10;
			break;
	}

	// set burst(extended) upgrade packet size
	if(IC_SIGNATURE[g_nIcType] == 0xE730)
	{
		g_FwDownloadInfo.icBurstPacketSize = g_FwDownloadInfo.icPageSize;
	}

	// set NVM timing
	g_FwDownloadInfo.icDefaultOscFreq 		= 0;
	g_FwDownloadInfo.icNvmDelayRegister 	= 0;
	g_FwDownloadInfo.icNvmDelayTime 		= 0;

	switch( IC_SIGNATURE[g_nIcType] )
	{
	case 0xE622:	//ZTW622 = ZT2717
	case 0xE623:	//ZTW623
		g_FwDownloadInfo.icDefaultOscFreq 	= 128;
		g_FwDownloadInfo.icNvmDelayRegister = 0x001F002C;
		g_FwDownloadInfo.icNvmDelayTime    	= 0x00003BD0;			// 4.5msec
	break;

	case 0xE730:	//ZTM730
	case 0xE722:	//ZTW722
		g_FwDownloadInfo.icDefaultOscFreq 	= 128;
		g_FwDownloadInfo.icNvmDelayRegister = 0x001F001C;
		g_FwDownloadInfo.icNvmDelayTime    	= 0x00003BD0;			// 4.5msec
	break;

	case 0x650E:	//ZT7650M
	//case 0xE760:	//ZTL760
		g_FwDownloadInfo.icDefaultOscFreq 	= 128;
		g_FwDownloadInfo.icNvmDelayRegister = 0x001E002C;
		g_FwDownloadInfo.icNvmDelayTime     = 0x00004FC2;			// 4.5msec , 기존 48Mhz 에서 44Mhz로 주파수 변경으로 Delay 수정
	break;

	case 0xE137:	//GTY137
		g_FwDownloadInfo.icDefaultOscFreq 	= 128;
		g_FwDownloadInfo.icNvmDelayRegister = 0x001E001C;			// addr 받기 
		g_FwDownloadInfo.icNvmDelayTime    	= 0x00005706;			// 4.5msec
	break;
	}

	if (m_bSaveInfo && (m_nSaveInfoLocation == SAVE_INFO_EEPROM))
	{
		g_FwDownloadInfo.bDontEraseLastPage = TRUE;
	}

	g_FwDownloadInfo.icIumSize = g_FwDownloadInfo.icPageSize;

	// set IUM Size
	if (IC_SIGNATURE[g_nIcType] == 0xE650 || IC_SIGNATURE[g_nIcType] == 0x650E || IC_SIGNATURE[g_nIcType] == 0xE760)
	{
		g_FwDownloadInfo.icIumSize = 64;
	}

	if(g_FwDownloadInfo.icIumSize > 128) { g_FwDownloadInfo.icIumSize = 128; }
}

BOOL CTestModeView::TSM_LoadTouchFirmware(CString * strRet)
{
	FILE * fi = NULL;
	int		nFileSz;
	CString strFileName;

	char * binData = NULL;
	int nMemSz, nFwBinaryDataSize;

	// Find firmware binary file
	CString strImagePath;
	strImagePath.Format("%s\\*.bin", m_strFirmwareDir);

	CFileFind finder;
	BOOL bWorking = finder.FindFile(strImagePath);
	int nBinFileCnt = 0;

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		if ((FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(finder.GetFilePath())) == 0)
		{// 파일일 경우
			nBinFileCnt++;
			m_strFirmFileName = finder.GetFilePath();
		}
	}

	if (nBinFileCnt == 0)
	{
		strRet->Format("FAIL : Binary does not exist.");
		goto error_firmware_file;
	}
	else if (nBinFileCnt >= 2)
	{
		strRet->Format("FAIL : Binary is too much.");
		goto error_firmware_file;
	}

	strFileName = m_strFirmFileName;

	nFileSz = GetFileSize(strFileName);
	if (nFileSz == 0)
	{
		strRet->Format("FAIL : CANNOT OPEN THE FILE(%s).", strFileName);
		goto error_firmware_file;
	}

	nMemSz = nFileSz;

	if (nMemSz % READ_SECTOR_SIZE > 0) {
		nMemSz = (nMemSz / READ_SECTOR_SIZE)*READ_SECTOR_SIZE + READ_SECTOR_SIZE;
	}

	nFwBinaryDataSize = nMemSz + 1024;

	binData = (char*)malloc(nFwBinaryDataSize);
	if (binData == NULL)
	{
		strRet->Format("FAIL : CANNOT ALLOC MEMORY.");
		goto error_firmware_file;
	}

	memset(binData, 0xff, nFwBinaryDataSize);

	fi = fopen(strFileName, "r+b");
	if (fi == NULL)
	{
		strRet->Format("FAIL : CANNOT OPEN THE FILE(%s).", strFileName);
		goto error_firmware_file;
	}

	if (nFileSz != fread(binData, 1, nFileSz, fi))
	{
		strRet->Format("FAIL : CANNOT READ FILE(%s). : SIZE = %d.", strFileName, nFileSz);
		fclose(fi);
		goto error_firmware_file;
	}

	if (fi != NULL)
		fclose(fi);

	g_FwDownloadInfo.pFwBinaryData = binData;
	g_FwDownloadInfo.nFwBinaryDataSize = nFwBinaryDataSize;
	g_FwDownloadInfo.nMemSize = nMemSz;

	return TRUE;

error_firmware_file:

	ZT_FREE(binData);

	if (fi != NULL)
		fclose(fi);

	return FALSE;
}

BOOL CTestModeView::TSM_UpgradeTouchFirmware(CUsbHid * pUsbHid)
{
	BOOL	bRet;
	BOOL bDownloadResult = TRUE;
	BOOL bRealBurstUpgrade = FALSE;

	CString strTemp = "";
	CString strFailMsg = "";

	if (pUsbHid->IsTestAvailable() == FALSE)
		return TRUE;

	MyDelayMs(10);

	if(pUsbHid->m_bIfeModeOn
	&& pUsbHid->m_bBurstUpgrade
	&& pUsbHid->m_nMpToolVersion >= 10
	&& m_bBurstUpgradeReal)
	{
		bRealBurstUpgrade = TRUE;
	}

	bRet = pUsbHid->UpgradeTouchFirmware(&strFailMsg, m_parentHWnd, m_bVerifyAfterDownload, bRealBurstUpgrade, m_bChangeNvmTiming);

	if (IsExistUsbHid(pUsbHid) == FALSE)
	{
		bDownloadResult = FALSE;

		pUsbHid->m_nResult = RESULT_FAIL;
	}
	else
	{
		if (bRet == TRUE)
		{ // success
			bDownloadResult = TRUE;
		}
		else
		{ // fail - retry
			bDownloadResult = FALSE;

			for (int nRetry = 0; nRetry < m_nDownloadRetryCount; nRetry++)
			{
				bRet = pUsbHid->UpgradeTouchFirmware(&strFailMsg, m_parentHWnd, m_bVerifyAfterDownload, bRealBurstUpgrade, m_bChangeNvmTiming);

				if (bRet == TRUE)
				{
					bDownloadResult = TRUE;
					break;
				}
			}
		}
	}

	if (bDownloadResult)
	{
		//DisplayResultToList(pUsbHid, RESULT_PASS);
	}
	else
	{
		if(strFailMsg.IsEmpty())
			strTemp.Format("FAIL : F/W Download FAIL.\t");
		else
			strTemp.Format("FAIL : F/W Download FAIL.\t%s", strFailMsg);

		DisplayResultFailToList(pUsbHid, strTemp);
	}

	return bDownloadResult;
}

BOOL CTestModeView::TSM_VerifyTouchFirmware(CUsbHid * pUsbHid)
{
	BOOL bDownloadResult = TRUE;

	CString strTemp = "";
	CString strFailMsg;

	if (pUsbHid->IsTestAvailable() == FALSE)
		return TRUE;

	MyDelayMs(10);

	bDownloadResult = pUsbHid->VerifyTouchFirmware(&strFailMsg, m_parentHWnd);

	if (IsExistUsbHid(pUsbHid) == FALSE)
	{
		bDownloadResult = FALSE;

		pUsbHid->m_nResult = RESULT_FAIL;
	}

	if (bDownloadResult)
	{
		//DisplayResultToList(pUsbHid, RESULT_PASS);
	}
	else
	{
		if(strFailMsg.IsEmpty())
			strTemp.Format("FAIL : Verify Firmware FAIL");
		else
			strTemp.Format("FAIL : Verify Firmware FAIL.\t%s", strFailMsg);

		DisplayResultFailToList(pUsbHid, strTemp);
	}

	return bDownloadResult;
}

BOOL CTestModeView::TSM_CheckEepromBit(CUsbHid * pUsbHid, BOOL bUseNvmPgmVerify)
{
	BOOL bDownloadResult = TRUE;

	CString strTemp = "";
	CString strFailMsg;

	if (pUsbHid->IsTestAvailable() == FALSE)
		return TRUE;

	MyDelayMs(10);

	bDownloadResult = pUsbHid->CheckEepromBit(&strFailMsg, bUseNvmPgmVerify, m_parentHWnd);

	if (IsExistUsbHid(pUsbHid) == FALSE)
	{
		bDownloadResult = FALSE;

		pUsbHid->m_nResult = RESULT_FAIL;
	}

	if (bDownloadResult)
	{
		//DisplayResultToList(pUsbHid, RESULT_PASS);
	}
	else
	{
		if(strFailMsg.IsEmpty())
			strTemp.Format("FAIL : Check Eeprom Bit FAIL");
		else
			strTemp.Format("FAIL : Check Eeprom Bit FAIL.\t%s", strFailMsg);

		DisplayResultFailToList(pUsbHid, strTemp);
	}

	return bDownloadResult;
}

BOOL CTestModeView::TSM_SubTest(CUsbHid * pUsbHid)	// MP TooL 2.0
{
	BOOL bRet = TRUE;

	BOOL bErrorReadRawData_DND = FALSE;
	BOOL bErrorReadRawData_DND_HF = FALSE;
	BOOL bErrorReadRawData_SelfDnd = FALSE;
	BOOL bErrorReadHighVoltage = FALSE;

	BOOL bErrorReadRawData_CND = FALSE;

	s16	sRawData[MAX_RAWDATA_NUM];
	s16	sRawData2[MAX_RAWDATA_NUM];
	s16	sDiffData[MAX_RAWDATA_NUM];

	int i, j;
	CString strTemp;

	TRACE("%s [%d]\n", __FUNCTION__, pUsbHid->m_nDevIndex);

	pUsbHid->m_strResult = "";

	//MyDelayMs(10);

	//-----------------------------------------------
	// Read Config Register
	bRet = ReadConfigRegister(pUsbHid);
	if (bRet == FALSE)
	{
		Log_Append(pUsbHid, RESULT_FAIL);
		DisplayResultFailToList(pUsbHid, "FAIL : Read config register");
		goto fail_tsm_test;
	}

	//-----------------------------------------------
	if (pUsbHid->GetProtocolMode() != 0)
	{
		AfxMessageBox("Cannot read ""REG_PROTOCOL_MODE""!");
		goto exit_tsm_test;;
	}
	//-----------------------------------------------

	//-----------------------------------------------
	// Read ASIC ID
	if (m_bAdditionalLOG)
	{
		bRet = ReadASICID(pUsbHid);
		if (bRet == FALSE)
		{
			Log_Append(pUsbHid, RESULT_FAIL);
			DisplayResultFailToList(pUsbHid, "FAIL : Please check the 'ASIC ID' again.");
			goto fail_tsm_test;
		}
	}


#if defined(D_USE_FW_CHINA_FRAMEWORK)
	if (pUsbHid->IsCoreFrameWork() == FALSE)
	{ // CHINA. For power saving off
		if (pUsbHid->SetTouchConfigEx(REG_INTERNAL_FLAG_00, 0) != 0)
		{
			for (i = 0; i < 5; i++)
			{
				if (pUsbHid->SetTouchConfigEx(REG_INTERNAL_FLAG_00, 0) == 0)
					break;
			}

			if (i >= 5)
			{
				Log_Append(pUsbHid, RESULT_FAIL);
				DisplayResultFailToList(pUsbHid, "Fail Set REG_INTERNAL_FLAG_00");
				goto fail_tsm_test;
			}
		}
	}
#endif

	// 0 -> Detect Device, 1 -> Download
	Log_Append(pUsbHid, RESULT_PASS);
	DisplayResultToList(pUsbHid, RESULT_PASS);

	if(pUsbHid->m_nTestStep == 0)
	{
		pUsbHid->m_nTestStep = 1;			// disable download
		UpdateProgressBar(pUsbHid);

		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep = 2;
	UpdateProgressBar(pUsbHid);

	//-----------------------------------------------
	// "03_CHECK_PIN"
	if (m_bCheckIntPin || m_bCheckReset)
	{
		if (m_bCheckIntPin)
		{
			//-----------------------------------------------
			// CHECK INTERRUPT PIN TEST
			bRet = Check_Interrupt_Pin(pUsbHid);
			if (bRet == FALSE)
			{
				Log_Append(pUsbHid, RESULT_FAIL);
				DisplayResultFailToList(pUsbHid, "FAIL : INTERRUPT PIN FAIL");
				goto fail_tsm_test;
			}
		}

		if (m_bCheckReset)
		{
			//-----------------------------------------------
			// CHECK RESET PIN TEST
			if (m_bCheckResetPin && pUsbHid->m_nMpToolVersion >= 6)
			{
				bRet = Check_Reset_Pin(pUsbHid);
				if (bRet == FALSE)
				{
					Log_Append(pUsbHid, RESULT_FAIL);
					DisplayResultFailToList(pUsbHid, "FAIL : RESET PIN FAIL");
					goto fail_tsm_test;
				}
			}
		}

		Log_Append(pUsbHid, RESULT_PASS);
		DisplayResultToList(pUsbHid, RESULT_PASS);
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "04_CHECK_VERSION"
	if (m_bCheckVersion == TRUE)
	{
		BOOL bTestResult = TRUE;

 		if (pUsbHid->m_CapInfo.icRegData[REG_CHIP_REVISION] != m_nIcReivsion){		// CHECK IC REVISION
			bTestResult = FALSE;
			DisplayResultFailToList(pUsbHid, "REG_IC_REVISION [0x%04X] IS NOT REF[0x%04X]", pUsbHid->m_CapInfo.icRegData[REG_CHIP_REVISION]&0xFFFF, m_nIcReivsion&0xFFFF);
		}

		if (pUsbHid->m_CapInfo.icRegData[REG_HW_ID] != m_nHwID){					// CHECK HW ID
			bTestResult = FALSE;
			DisplayResultFailToList(pUsbHid, "REG_HW_ID [0x%04X] IS NOT REF[0x%04X]", pUsbHid->m_CapInfo.icRegData[REG_HW_ID]&0xFFFF, m_nHwID&0xFFFF);
		}

		if (pUsbHid->m_CapInfo.icRegData[REG_FIRMWARE_VERSION] != m_nMajorVersion){	// CHECK MAJOR VERSION
			bTestResult = FALSE;
			DisplayResultFailToList(pUsbHid, "REG_MAJOR_VERSION [0x%04X] IS NOT REF[0x%04X]", pUsbHid->m_CapInfo.icRegData[REG_FIRMWARE_VERSION]&0xFFFF, m_nMajorVersion&0xFFFF);
		}

		if (pUsbHid->m_CapInfo.icRegData[REG_MINOR_FW_VERSION] != m_nMinorVersion){	// CHECK MINOR VERSION
			bTestResult = FALSE;
			DisplayResultFailToList(pUsbHid, "REG_MINOR_VERSION [0x%04X] IS NOT REF[0x%04X]", pUsbHid->m_CapInfo.icRegData[REG_MINOR_FW_VERSION]&0xFFFF, m_nMinorVersion&0xFFFF);
		}

		if (pUsbHid->m_CapInfo.icRegData[REG_DATA_VERSION] != m_nRegisterVersion){	// CHECK REGISTER VERSION
			bTestResult = FALSE;
			DisplayResultFailToList(pUsbHid, "REG_REGISTER_VERSION [0x%04X] IS NOT REF[0x%04X]", pUsbHid->m_CapInfo.icRegData[REG_DATA_VERSION]&0xFFFF, m_nRegisterVersion&0xFFFF);
		}

		// check module ID
		strTemp.Format("0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t[IC_REV],\t\t[HW ID],\t\t[MAJOR VER],\t\t[MINOR VER],\t\t[RELEASE VER]\r\n",
			pUsbHid->m_CapInfo.icRegData[REG_CHIP_REVISION] & 0xFFFF, pUsbHid->m_CapInfo.icRegData[REG_HW_ID] & 0xFFFF,
			pUsbHid->m_CapInfo.icRegData[REG_FIRMWARE_VERSION] & 0xFFFF, pUsbHid->m_CapInfo.icRegData[REG_MINOR_FW_VERSION] & 0xFFFF, pUsbHid->m_CapInfo.icRegData[REG_DATA_VERSION] & 0xFFFF);

		//04_5 CHECK_VERSION_IUM
		CString strIUMResult , strIUMTitle;
		if (m_bCheckIUM == TRUE)
		{
			//pUsbHid->m_tIumCmdInfo = ZT_GET_IUM_CMD_INFO(pUsbHid->m_CapInfo.icSignature);
			strTemp = strTemp + "\r\n";

			unsigned short nGetChipId = 0;

			pUsbHid->GetTouchRegReadCmd(ZT_VCMD(VCMD_IUM_READ_DATA), (BYTE*)pUsbHid->m_IumData, g_FwDownloadInfo.icIumSize);

			pUsbHid->GetTouchConfigEx(REG_CHIP_CODE, (BYTE*)&nGetChipId);

			CString strGetASIC;
			CString strGetProjectID;
			CString strGetDeviceID;
			CString strGetChipID;
			CString strRegChipID;

			strGetASIC.Format("%04X%04X%04X%04X", pUsbHid->m_IumData[3], pUsbHid->m_IumData[2], pUsbHid->m_IumData[1], pUsbHid->m_IumData[0]);
			strGetChipID.Format("%04X", pUsbHid->m_IumData[5]);
			strRegChipID.Format("%04X", nGetChipId);

			strGetProjectID.Format("%04X", pUsbHid->m_IumData[6]);
			strGetDeviceID.Format("%04X%04X%04X%04X" , pUsbHid->m_IumData[7], pUsbHid->m_IumData[8], pUsbHid->m_IumData[9], pUsbHid->m_IumData[10]);

			if (strGetProjectID != m_strProjectID)
			{
				bTestResult = FALSE;
				DisplayResultFailToList(pUsbHid, "IUM_PROJECT_ID [%s] IS NOT REF[%s]", strGetProjectID, m_strProjectID);
			}

			if (strGetDeviceID != m_strDeviceID)
			{
				bTestResult = FALSE;
				DisplayResultFailToList(pUsbHid, "IUM_DEVICE_ID [%s] IS NOT REF[%s]", strGetDeviceID, m_strDeviceID);
			}

			if (strGetChipID != strRegChipID)
			{
				bTestResult = FALSE;
				DisplayResultFailToList(pUsbHid, "IUM_CHIP_ID [%s] IS NOT REG_CHIP_ID [%s]", strGetChipID , strRegChipID);
			}

			if (pUsbHid->m_IumData[4] != m_nHwID)
			{
				bTestResult = FALSE;
				DisplayResultFailToList(pUsbHid, "IUM_HW_ID [0x%04X] IS NOT REF[0x%04X]", pUsbHid->m_IumData[4]&0xFFFF, m_nHwID&0xFFFF);
			}

			strIUMTitle.Format(" ASIC0,\t\t ASIC1,\t\t ASIC2,\t\t ASIC3,\t\t HW_ID,\t\tCHIPID,\t\tPRJ_ID,\t\tDEVICE 0,\t\tDEVICE 1,\t\tDEVICE 2,\t\tDEVICE 3\r\n");

			strIUMResult.Format("0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X\r\n",
				pUsbHid->m_IumData[0], pUsbHid->m_IumData[1], pUsbHid->m_IumData[2], pUsbHid->m_IumData[3], pUsbHid->m_IumData[4], pUsbHid->m_IumData[5],
				pUsbHid->m_IumData[6], pUsbHid->m_IumData[7], pUsbHid->m_IumData[8], pUsbHid->m_IumData[9], pUsbHid->m_IumData[10]);
		}

		// Verify test result...
		if (bTestResult == FALSE)
		{
			Log_AppendDesc(pUsbHid, RESULT_FAIL, strTemp);
			if (m_bCheckIUM == TRUE)
			{
				Log_AppendDescIUM(pUsbHid, RESULT_FAIL, strIUMTitle, TRUE);
				Log_AppendDescIUM(pUsbHid, RESULT_FAIL, strIUMResult, FALSE);
			}

			if (IsAutoFwDownload_Condition())
			{// set version check error flag
				pUsbHid->m_bErrorVersionCheck = TRUE;
			}

			goto fail_tsm_test;
		}
		else
		{
			Log_AppendDesc(pUsbHid, RESULT_PASS, strTemp);
			if (m_bCheckIUM == TRUE)
			{
				Log_AppendDescIUM(pUsbHid, RESULT_PASS, strIUMTitle, TRUE);
				Log_AppendDescIUM(pUsbHid, RESULT_PASS, strIUMResult, FALSE);
			}

			DisplayResultToList(pUsbHid, RESULT_PASS);
		}
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);

	if (m_bCheckIUM == FALSE)
		Log_AppendEx(pUsbHid, "\r\n");

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "05_DND"
	if (m_TestInfo_DND.bCheckData == TRUE)
	{
		bRet = ReadRawData(pUsbHid, sRawData);

		if (bRet == FALSE)
		{
			bErrorReadRawData_DND = TRUE;

			Log_Append(pUsbHid, RESULT_FAIL);

			if (pUsbHid->m_InterruptTimeOut)
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA - Interrupt timeout");
			else
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA");
		}
		else
		{
			bRet = CheckDNDRawDataRange(pUsbHid, sRawData);

			if (bRet == FALSE)
			{
				Log_Append(pUsbHid, RESULT_FAIL, TRUE);
				DisplayResultToList(pUsbHid, RESULT_FAIL);
			}
			else
			{
				Log_Append(pUsbHid, RESULT_PASS, TRUE);
				DisplayResultToList(pUsbHid, RESULT_PASS);
			}

			TSM_AddData_InLog(pUsbHid, sRawData);
		}

		Log_AppendEx(pUsbHid, "\r\n");
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "06_DND_H_GAP_&_V_GAP"
	if (m_TestInfo_DND.bCheckData == TRUE && m_TestInfo_DND.bCheckDataDiff == TRUE && bErrorReadRawData_DND == FALSE)
	{
		BOOL bTestResult = TRUE;

		//-----------------------------------------------
		// "DND H GAP"

		memset(sDiffData, 0x00, sizeof(s16)*MAX_RAWDATA_NUM);

		bRet = CheckRawDataHDiff(pUsbHid, sRawData, sDiffData);

		if (bRet == FALSE)
		{
			bTestResult = FALSE;

			Log_AppendDiff(pUsbHid, RESULT_FAIL, TRUE, "DND_H_GAP");
		}
		else
		{
			Log_AppendDiff(pUsbHid, RESULT_PASS, TRUE, "DND_H_GAP");
		}

		TSM_AddData_InLog(pUsbHid, sDiffData, TEST_STEP_CHECK_H_DIFF);

		Log_AppendEx(pUsbHid, "\r\n");

		//-----------------------------------------------
		// "DND V GAP"

		memset(sDiffData, 0x00, sizeof(s16)*MAX_RAWDATA_NUM);

		bRet = CheckRawDataVDiff(pUsbHid, sRawData, sDiffData);

		if (bRet == FALSE)
		{
			bTestResult = FALSE;

			Log_AppendDiff(pUsbHid, RESULT_FAIL, TRUE, "DND_V_GAP");
		}
		else
		{
			Log_AppendDiff(pUsbHid, RESULT_PASS, TRUE, "DND_V_GAP");
		}

		TSM_AddData_InLog(pUsbHid, sDiffData, TEST_STEP_CHECK_V_DIFF);

		if( bTestResult )
			DisplayResultToList(pUsbHid, RESULT_PASS);
		else
			DisplayResultToList(pUsbHid, RESULT_FAIL);

		Log_AppendEx(pUsbHid, "\r\n");
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "07_HF DND"
	if (m_TestInfo_HF_DND.bCheckData == TRUE)
	{
		bRet = ReadRawData_HfDnd(pUsbHid, sRawData);

		if (bRet == FALSE)
		{
			bErrorReadRawData_DND_HF = TRUE;

			Log_Append(pUsbHid, RESULT_FAIL);

			if (pUsbHid->m_InterruptTimeOut)
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA - Interrupt timeout");
			else
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA");
		}
		else
		{
			bRet = CheckRawDataRange_HfDnd(pUsbHid, sRawData);

			if (bRet == FALSE)
			{
				Log_Append(pUsbHid, RESULT_FAIL, TRUE);
				DisplayResultToList(pUsbHid, RESULT_FAIL);
			}
			else
			{
				Log_Append(pUsbHid, RESULT_PASS, TRUE);
				DisplayResultToList(pUsbHid, RESULT_PASS);
			}

			TSM_AddData_InLog(pUsbHid, sRawData);
		}

		Log_AppendEx(pUsbHid, "\r\n");
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "08_HF_DND_H_GAP_&_V_GAP"
	if (m_TestInfo_HF_DND.bCheckData == TRUE && m_TestInfo_HF_DND.bCheckDataDiff == TRUE && bErrorReadRawData_DND_HF == FALSE)
	{
		BOOL bTestResult = TRUE;

		//-----------------------------------------------
		// HF DND H GAP

		memset(sDiffData, 0x00, sizeof(s16)*MAX_RAWDATA_NUM);

		bRet = CheckRawDataHDiff_HfDnd(pUsbHid, sRawData, sDiffData);

		if (bRet == FALSE)
		{
			bTestResult = FALSE;

			Log_AppendDiff(pUsbHid, RESULT_FAIL, TRUE, "HF_DND_H_GAP");
		}
		else
		{
			Log_AppendDiff(pUsbHid, RESULT_PASS, TRUE, "HF_DND_H_GAP");
		}

		TSM_AddData_InLog(pUsbHid, sDiffData, TEST_STEP_CHECK_H_DIFF);

		Log_AppendEx(pUsbHid, "\r\n");

		//-----------------------------------------------
		// HF DND V GAP

		memset(sDiffData, 0x00, sizeof(s16)*MAX_RAWDATA_NUM);

		bRet = CheckRawDataVDiff_HfDnd(pUsbHid, sRawData, sDiffData);

		if (bRet == FALSE)
		{
			bTestResult = FALSE;

			Log_AppendDiff(pUsbHid, RESULT_FAIL, TRUE, "HF_DND_V_GAP");
		}
		else
		{
			Log_AppendDiff(pUsbHid, RESULT_PASS, TRUE, "HF_DND_V_GAP");
		}

		TSM_AddData_InLog(pUsbHid, sDiffData, TEST_STEP_CHECK_V_DIFF);

		if( bTestResult )
			DisplayResultToList(pUsbHid, RESULT_PASS);
		else
			DisplayResultToList(pUsbHid, RESULT_FAIL);

		Log_AppendEx(pUsbHid, "\r\n");
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);

#if defined(D_USE_FW_CHINA_FRAMEWORK)
	if (pUsbHid->IsCoreFrameWork() == FALSE && (m_TestInfo_SELF_DND.bCheckData || m_bCheckTrxShort))
	{
		pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] = pUsbHid->m_IC_Max_X_Line_Num;
		pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] = pUsbHid->m_IC_Max_Y_Line_Num;

		pUsbHid->SetTouchConfigEx(REG_TOTAL_NUMBER_OF_X, pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X]);
		pUsbHid->SetTouchConfigEx(REG_TOTAL_NUMBER_OF_Y, pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]);
	}
#endif

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "09_SELF_DND"
	if (m_TestInfo_SELF_DND.bCheckData == TRUE)
	{
		s16	* pRxRawData = sRawData;
		s16	* pTxRawData = NULL;

		if (pUsbHid->IsCoreFrameWork() == FALSE)
			pTxRawData = sRawData2;
		else
			pTxRawData = &sRawData[pUsbHid->m_nSelfChannelCntY];

		bRet = ReadRawData_SelfDnd(pUsbHid, pRxRawData, 0);

		if (bRet && pUsbHid->IsCoreFrameWork() == FALSE)
		{
			MyDelayMs(100);
			bRet = ReadRawData_SelfDnd(pUsbHid, pTxRawData, 1);
		}

		if (bRet == FALSE)
		{
			bErrorReadRawData_SelfDnd = TRUE;

			Log_Append(pUsbHid, RESULT_FAIL);

			if (pUsbHid->m_InterruptTimeOut)
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA - Interrupt timeout");
			else
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA");
		}
		else
		{
			bRet = CheckRawDataRange_SelfDnd(pUsbHid, pRxRawData, pTxRawData);

			if (bRet == FALSE)
			{
				Log_Append(pUsbHid, RESULT_FAIL, TRUE);
				DisplayResultToList(pUsbHid, RESULT_FAIL);
			}
			else
			{
				Log_Append(pUsbHid, RESULT_PASS, TRUE);
				DisplayResultToList(pUsbHid, RESULT_PASS);
			}

			TSM_AddData_InLog_Self(pUsbHid, pRxRawData, pTxRawData);
		}

		Log_AppendEx(pUsbHid, "\r\n");
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "10_SELF_DND_GAP"
	if (m_TestInfo_SELF_DND.bCheckData == TRUE && m_TestInfo_SELF_DND.bCheckDataDiff == TRUE && bErrorReadRawData_SelfDnd == FALSE)
	{
		s16	* pRxRawData = sRawData;
		s16	* pTxRawData = NULL;

		if (pUsbHid->IsCoreFrameWork() == FALSE)
			pTxRawData = sRawData2;
		else
			pTxRawData = &sRawData[pUsbHid->m_nSelfChannelCntY];

		memset(sDiffData, 0x00, sizeof(s16)*MAX_RAWDATA_NUM);

		bRet = CheckRawDataHDiff_SelfDnd(pUsbHid, pRxRawData, pTxRawData, sDiffData);

		if (bRet == FALSE)
		{
			Log_Append(pUsbHid, RESULT_FAIL, TRUE);
			DisplayResultToList(pUsbHid, RESULT_FAIL);
		}
		else
		{
			Log_Append(pUsbHid, RESULT_PASS, TRUE);
			DisplayResultToList(pUsbHid, RESULT_PASS);
		}

		TSM_AddData_InLog_Self(pUsbHid, sDiffData, &sDiffData[pUsbHid->m_nSelfChannelCntY]);

		Log_AppendEx(pUsbHid, "\r\n");
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "11_SELF_SATURATION"
	if (m_TestInfo_SELF_SATURATION.bCheckData == TRUE)
	{
		bRet = ReadRawData_SelfSaturation(pUsbHid, sRawData);

		if (bRet == FALSE)
		{
			Log_Append(pUsbHid, RESULT_FAIL);

			if (pUsbHid->m_InterruptTimeOut)
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA - Interrupt timeout");
			else
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA");
		}
		else
		{
			bRet = CheckRawDataRange_SelfSaturation(pUsbHid, sRawData);

			if (bRet == FALSE)
			{
				Log_Append(pUsbHid, RESULT_FAIL, TRUE);
				DisplayResultToList(pUsbHid, RESULT_FAIL);
			}
			else
			{
				Log_Append(pUsbHid, RESULT_PASS, TRUE);
				DisplayResultToList(pUsbHid, RESULT_PASS);
			}

			TSM_AddData_InLog_Self(pUsbHid, sRawData, &sRawData[pUsbHid->m_nSelfChannelCntY]);
		}

		Log_AppendEx(pUsbHid, "\r\n");
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "12_TRX_SHORT"
	if (m_bCheckTrxShort == TRUE)
	{
		bRet = ReadRawData_TrxShort(pUsbHid, sRawData);

		if (bRet == FALSE)
		{
			Log_Append(pUsbHid, RESULT_FAIL);

			if (pUsbHid->m_InterruptTimeOut)
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA - Interrupt timeout");
			else
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA");
		}
		else
		{
			bRet = CheckRawDataRange_TrxShort(pUsbHid, sRawData);

			if (bRet == FALSE)
			{
				Log_Append(pUsbHid, RESULT_FAIL, TRUE);
				DisplayResultToList(pUsbHid, RESULT_FAIL);
			}
			else
			{
				Log_Append(pUsbHid, RESULT_PASS, TRUE);
				DisplayResultToList(pUsbHid, RESULT_PASS);
			}

			TSM_AddData_InLog(pUsbHid, sRawData);
		}

		Log_AppendEx(pUsbHid, "\r\n");
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);

#if defined(D_USE_FW_CHINA_FRAMEWORK)
	if (pUsbHid->IsCoreFrameWork() == FALSE && (m_TestInfo_SELF_DND.bCheckData || m_bCheckTrxShort))
	{
		pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] = pUsbHid->m_nTotalNumberX;
		pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] = pUsbHid->m_nTotalNumberY;

		pUsbHid->SetTouchConfigEx(REG_TOTAL_NUMBER_OF_X, pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X]);
		pUsbHid->SetTouchConfigEx(REG_TOTAL_NUMBER_OF_Y, pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]);
	}
#endif

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "13_Current_Consumption"
	if (m_TestInfo_CURRENT_CONSUMPTION.Active.bCheckData == TRUE && m_TestInfo_CURRENT_CONSUMPTION.bEnable == TRUE)
	{
		int nRet = TSM_CurrentConsumption(pUsbHid);

		BOOL bEnableIdleOption = m_TestInfo_CURRENT_CONSUMPTION.Idle.bCheckData || m_TestInfo_CURRENT_CONSUMPTION.Sleep.bCheckData || m_TestInfo_CURRENT_CONSUMPTION.DeepSleep.bCheckData;

		if (nRet != Z_TEST_OK)
		{// Fail
			if (nRet == Z_TEST_ERR_SPEC_OVER)
			{// range over
				if (bEnableIdleOption)
					strTemp.Format("%d uA,\t\t%d uA,\t\t%d uA,\t\t%d uA\r\n", pUsbHid->m_curCurrent[CURRENT_CONSUMPTION_ACTIVE], pUsbHid->m_curCurrent[CURRENT_CONSUMPTION_IDLE], pUsbHid->m_curCurrent[CURRENT_CONSUMPTION_SLEEP], pUsbHid->m_curCurrent[CURRENT_CONSUMPTION_DEEP_SLEEP]);
				else
					strTemp.Format("%d uA\r\n", pUsbHid->m_curCurrent[CURRENT_CONSUMPTION_ACTIVE]);

				Log_AppendDesc(pUsbHid, RESULT_FAIL, strTemp);
				DisplayResultToList(pUsbHid, RESULT_FAIL);
			}
			else
			{
				Log_Append(pUsbHid, RESULT_FAIL);
				DisplayResultFailToList(pUsbHid, pUsbHid->m_strRet);
			}
		}
		else
		{// Success
			strTemp.Format("%d uA", pUsbHid->m_curCurrent[CURRENT_CONSUMPTION_ACTIVE]);

			if(pUsbHid->m_curCurrent[CURRENT_CONSUMPTION_IDLE] == -1)
				strTemp.AppendFormat(",\t\tNotTest");
			else
				strTemp.AppendFormat(",\t\t%d uA", pUsbHid->m_curCurrent[CURRENT_CONSUMPTION_IDLE]);

			if(pUsbHid->m_curCurrent[CURRENT_CONSUMPTION_SLEEP] == -1)
				strTemp.AppendFormat(",\t\tNotTest");
			else
				strTemp.AppendFormat(",\t\t%d uA", pUsbHid->m_curCurrent[CURRENT_CONSUMPTION_SLEEP]);
			if(pUsbHid->m_curCurrent[CURRENT_CONSUMPTION_DEEP_SLEEP] == -1)
				strTemp.AppendFormat(",\t\tNotTest \r\n");
			else
				strTemp.AppendFormat(",\t\t%d uA \r\n", pUsbHid->m_curCurrent[CURRENT_CONSUMPTION_DEEP_SLEEP]);

			Log_AppendDesc(pUsbHid, RESULT_PASS, strTemp);

			DisplayResultToList(pUsbHid, RESULT_PASS);
		}

		if (bEnableIdleOption)
		{
			// Power Reset & Power Sequence
			MpTool_PowerSequenceAfterReset(pUsbHid);
		}
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "14_JITTER"
	if (m_TestInfo_JITTER.bCheckData == TRUE)
	{
#if defined(D_PRODUCT_SOLUTION_ALLROUNDER)
		bRet = ReadRawData_JitterN_AllRounder(pUsbHid, sRawData);
#else
		if(pUsbHid->IsCoreFrameWork() == FALSE && m_TestInfo_JITTER.bCheckDataByMpztd)
			bRet = ReadRawData_JitterN_ByMPztd(pUsbHid, sRawData);
		else
			bRet = ReadRawData_JitterN(pUsbHid, sRawData);
#endif

		if (bRet == FALSE)
		{
			Log_Append(pUsbHid, RESULT_FAIL);

			if (pUsbHid->m_InterruptTimeOut)
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA - Interrupt timeout");
			else
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA");
		}
		else
		{
			bRet = CheckRawData_JitterMax(pUsbHid, sRawData);

			if (bRet == FALSE)
			{
				Log_Append(pUsbHid, RESULT_FAIL, TRUE);
				DisplayResultToList(pUsbHid, RESULT_FAIL);
			}
			else
			{
				Log_Append(pUsbHid, RESULT_PASS, TRUE);
				DisplayResultToList(pUsbHid, RESULT_PASS);
			}

			TSM_AddData_InLog(pUsbHid, sRawData);
		}

		Log_AppendEx(pUsbHid, "\r\n");
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "15_HW_CALIBRATION"
	if (m_TestInfo_HW_CALIBRATION.bCheckData)
	{
		bRet = TSM_HwCalibration(pUsbHid);

		if (bRet == FALSE)
		{
			Log_Append(pUsbHid, RESULT_FAIL);
			DisplayResultFailToList(pUsbHid, pUsbHid->m_strRet);
		}
		else
		{
			bRet = ReadRawData_HwCalibration(pUsbHid, sRawData);

			if (bRet == FALSE)
			{
				Log_Append(pUsbHid, RESULT_FAIL);

				if (pUsbHid->m_InterruptTimeOut)
					DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA - Interrupt timeout");
				else
					DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA");
			}
			else
			{
				bRet = CheckRawDataRange_HwCalibration(pUsbHid, sRawData);

				if (bRet == FALSE)
				{
					Log_Append(pUsbHid, RESULT_FAIL, TRUE);
					DisplayResultToList(pUsbHid, RESULT_FAIL);
				}
				else
				{
					Log_Append(pUsbHid, RESULT_PASS, TRUE);
					DisplayResultToList(pUsbHid, RESULT_PASS);
				}

				TSM_AddData_InLog(pUsbHid, sRawData);
			}
		}

		Log_AppendEx(pUsbHid, "\r\n");
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "16_MEASURE DELTA"
	if (m_TestInfo_MEASURE_DELTA.bCheckData == TRUE)
	{
		s16	sRawData_Measure[MAX_MEASURE_CNT][MAX_MEASURE_DELTA_RAWDATA_NUM];
		s16	sRawData_Measure_Average[MAX_MEASURE_DELTA_RAWDATA_NUM];

		int nSelfYPos, nSelfXPos;
		nSelfYPos = (int)(pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]);	// Mutual Data
		nSelfYPos += (pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]);				// Null
		nSelfXPos = nSelfYPos + (pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]);	// Y Self

		if (m_TestInfo_MEASURE_DELTA.nCount > 0 && m_TestInfo_MEASURE_DELTA.nCount < MAX_MEASURE_CNT)
		{
			// change mode
			bRet = PrepareToReadRawData(pUsbHid);

			// Read Garbage Data
			if( bRet && ReadGarbageData(pUsbHid, sRawData_Measure[0]) != TRUE )
				bRet = FALSE;

			if (bRet == TRUE)
			{
				if (m_TestInfo_MEASURE_DELTA.bCheckAutoStart)
				{
					popup_noti * pNotiPopup = new popup_noti(TRUE);
					pNotiPopup->Create(IDD_DIALOG_POPUP_NOTI, NULL);
					pNotiPopup->ShowWindow(SW_SHOW);
					MyDelayMs(100);

					// Read dummy data
					for (i = 0; i < 3; i++)
					{
						ReadRawData_MeasureDelta(pUsbHid, sRawData_Measure[0]);
					}

					while(1)
					{
						if (pNotiPopup->IsDialogExit())
							break;

						bRet = ReadRawData_MeasureDelta(pUsbHid, sRawData_Measure[0]);

						if (bRet != TRUE)
							continue;

						bRet = CheckRawDataRange_MeasureDelta_Condition_AutoStart(pUsbHid, sRawData_Measure[0]);
						if (bRet == TRUE)
							break;

						MyDelayMs(100);
					};

					MyDelayMs(1000);		// 1 sec delay

					pNotiPopup->DestroyWindow();
					ZT_DELETE_OBJ(pNotiPopup);
				}
				else
				{
					// "PRESS TOUCH PANEL"
					popup_noti dlg(FALSE);
					if (dlg.DoModal() != IDOK)
						bRet = TRUE;
				}

				// Read dummy data
				for (i = 0; i < 3; i++)
				{
					ReadRawData_MeasureDelta(pUsbHid, sRawData_Measure[0]);
				}

				// Read Mutual Data - 10 times
				for (i = 0; i < m_TestInfo_MEASURE_DELTA.nCount; i++)
				{
					bRet = ReadRawData_MeasureDelta(pUsbHid, sRawData_Measure[i]);

					if (bRet != TRUE)
						break;
				}
			}

			if (bRet == TRUE)
			{
				///////////////////////////////
				//average 연산
				///////////////////////////////
				int sum_val;

				// Get average for Mutual Data
				for (i = 0; i < (int)(pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]); i++)
				{
					sum_val = 0;

					for (j = 0; j < m_TestInfo_MEASURE_DELTA.nCount; j++)
						sum_val += sRawData_Measure[j][i];

					sRawData_Measure_Average[i] = sum_val / m_TestInfo_MEASURE_DELTA.nCount;
				}

				// Get average for Self Y
				for (i = 0; i < (int)pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]; i++)
				{
					sum_val = 0;

					for (j = 0; j < m_TestInfo_MEASURE_DELTA.nCount; j++)
						sum_val += sRawData_Measure[j][nSelfYPos + i];

					sRawData_Measure_Average[nSelfYPos + i] = sum_val / m_TestInfo_MEASURE_DELTA.nCount;
				}

				// Get average for Self X
				for (i = 0; i < (int)pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X]; i++)
				{
					sum_val = 0;

					for (j = 0; j < m_TestInfo_MEASURE_DELTA.nCount; j++)
						sum_val += sRawData_Measure[j][nSelfXPos + i];

					sRawData_Measure_Average[nSelfXPos + i] = sum_val / m_TestInfo_MEASURE_DELTA.nCount;
				}
			}
		}
		else
		{
			bRet = FALSE;
		}

		///////////////////////////////
		if (bRet == FALSE)
		{
			Log_Append(pUsbHid, RESULT_FAIL);
			if (m_TestInfo_MEASURE_DELTA.nCount != 0)
			{
				if (pUsbHid->m_InterruptTimeOut)
					DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA - Interrupt timeout");
				else
					DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA");
			}
			else
				DisplayResultFailToList(pUsbHid, "FAIL : MEASURE DELTA FRAME COUNT IS 0");
		}
		else
		{
			// check condition 1
			BOOL bRet1 = CheckRawDataRange_MeasureDelta_Condition_1(pUsbHid, &sRawData_Measure_Average[0]);

			// check condition 2
			BOOL bRet2 = CheckRawDataRange_MeasureDelta_Condition_2(pUsbHid, &sRawData_Measure_Average[nSelfYPos]);

			// check condition 3
			BOOL bRet3 = CheckRawDataRange_MeasureDelta_Condition_3(pUsbHid, &sRawData_Measure_Average[nSelfXPos]);

			if (bRet1 == FALSE || bRet2 == FALSE || bRet3 == FALSE)
			{
				Log_Append(pUsbHid, RESULT_FAIL, TRUE);
				DisplayResultToList(pUsbHid, RESULT_FAIL);
			}
			else
			{
				Log_Append(pUsbHid, RESULT_PASS, TRUE);
				DisplayResultToList(pUsbHid, RESULT_PASS);
			}

			TSM_AddData_InLog_MeasureDelta(pUsbHid, sRawData_Measure_Average);
		}

		Log_AppendEx(pUsbHid, "\r\n");
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "17_DIGITAL_WHEEL"
	if (m_TestInfo_DIGITAL_WHEEL.bCheckData == TRUE)
	{
		bRet = ReadRawData_DigitalWheel(pUsbHid, sRawData);

		if (bRet == FALSE)
		{
			Log_Append(pUsbHid, RESULT_FAIL);

			if (pUsbHid->m_InterruptTimeOut)
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA - Interrupt timeout");
			else
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA");
		}
		else
		{
			bRet = CheckRawDataRange_DigitalWheel(pUsbHid, sRawData);

			if (bRet == FALSE)
			{
				Log_Append(pUsbHid, RESULT_FAIL, TRUE);
				DisplayResultToList(pUsbHid, RESULT_FAIL);
			}
			else
			{
				Log_Append(pUsbHid, RESULT_PASS, TRUE);
				DisplayResultToList(pUsbHid, RESULT_PASS);
			}

			TSM_AddData_InLog(pUsbHid, sRawData);
		}

		Log_AppendEx(pUsbHid, "\r\n");
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);

#if defined(D_USE_CHECK_BUTTON_TEST)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "18_GPIO_BUTTON"
	if (m_bCheckGpioButton)
	{
		bRet = GpioButton_RequestTest(pUsbHid);

		if (bRet == FALSE)
		{
			Log_Append(pUsbHid, RESULT_FAIL, FALSE);
			DisplayResultToList(pUsbHid, RESULT_FAIL);
		}
		else
		{
			Log_Append(pUsbHid, RESULT_PASS, FALSE);
			DisplayResultToList(pUsbHid, RESULT_PASS);
		}
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "19_TOUCH_BUTTON"
	if (m_bCheckTouchButton)
	{
		bRet = TouchButton_RequestTest(pUsbHid);

		if (bRet == FALSE)
		{
			Log_Append(pUsbHid, RESULT_FAIL, FALSE);
			DisplayResultToList(pUsbHid, RESULT_FAIL);
		}
		else
		{
			Log_Append(pUsbHid, RESULT_PASS, FALSE);
			DisplayResultToList(pUsbHid, RESULT_PASS);
		}
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);
#endif

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "20_NOISE_CHECK"
	if (m_TestInfo_NOISE.m_bCheckNoise)
	{
		s16 sNoise[100] = { 0, };
		bRet = TRUE;
		BOOL bNoiseResult;
		int i;

		if (ChangeTouchMode(pUsbHid, 0) != TRUE)
		{
			for (i = 0; i < 5; i++)
			{
				if (ChangeTouchMode(pUsbHid, 0) == TRUE)
					break;

				if (i == 4)
				{
					Log_Append(pUsbHid, RESULT_FAIL);
					DisplayResultFailToList(pUsbHid, "FAIL : CHANGE TOUCH MODE : 0");

					pUsbHid->m_nTestStep++;
					UpdateProgressBar(pUsbHid);
					goto exit_tsm_test;
				}
			}
		}

		pUsbHid->SetTouchConfig(0x0017, 0, FALSE);
		MySleep(10);

		for (i = 0; i < 5; i++)
		{
			if (i == 1 && m_TestInfo_NOISE.AfeInfo.m_bCheckFrequency1 == FALSE)
				continue;
			if (i == 2 && m_TestInfo_NOISE.AfeInfo.m_bCheckFrequency2 == FALSE)
				continue;
			if (i == 3 && m_TestInfo_NOISE.AfeInfo.m_bCheckFrequency3 == FALSE)
				continue;
			if (i == 4 && m_TestInfo_NOISE.AfeInfo.m_bCheckFrequency4 == FALSE)
				continue;

			bNoiseResult = Noise_RequestTest(pUsbHid, sNoise, i);
			bRet &= bNoiseResult;

			if (bNoiseResult == TRUE)
			{
				bNoiseResult = CheckNoise(pUsbHid, sNoise);
				bRet &= bNoiseResult;

				TestInfo_Load_Check_PrintPpVariation();

				TSM_AddData_InLog_Noise(pUsbHid, sNoise);
			}
		}

		if (bRet == FALSE)
		{
			Log_Append(pUsbHid, RESULT_FAIL, TRUE);
			DisplayResultToList(pUsbHid, RESULT_FAIL);
		}
		else
		{
			Log_Append(pUsbHid, RESULT_PASS, TRUE);
			DisplayResultToList(pUsbHid, RESULT_PASS);
		}
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "21_ONLY_SELF_DND"		//add 2022.09.19
	if (m_TestInfo_ONLYSELF_DND.bCheckData == TRUE)
	{
		bRet = ReadRawData_OnlySelfDnd(pUsbHid, sRawData, pUsbHid->m_nTestStep);

		if (bRet == FALSE)
		{
			Log_Append(pUsbHid, RESULT_FAIL);

			if (pUsbHid->m_InterruptTimeOut)
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA - Interrupt timeout");
			else
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA");
		}
		else
		{
			bRet = CheckRawDataRange_OnlySelfDnd(pUsbHid, sRawData);

			if (bRet == FALSE)
			{
				Log_Append(pUsbHid, RESULT_FAIL, TRUE);
				DisplayResultToList(pUsbHid, RESULT_FAIL);
			}
			else
			{
				Log_Append(pUsbHid, RESULT_PASS, TRUE);
				DisplayResultToList(pUsbHid, RESULT_PASS);
			}

			TSM_AddData_InLog_Self(pUsbHid, sRawData, &sRawData[pUsbHid->m_nSelfChannelCntY]);
		}

		Log_AppendEx(pUsbHid, "\r\n");
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "22_HIGH_VOLTAGE"		//add 2022.09.30
	if (m_TestInfo_HIGH_VOLTAGE.bCheckData == TRUE)
	{
		bRet = ReadHighVoltage(pUsbHid, sRawData, pUsbHid->m_nTestStep);

		pUsbHid->m_bErrorRead = bRet;

		if (bRet == FALSE)
		{
			bErrorReadHighVoltage = TRUE;

			Log_Append(pUsbHid, RESULT_FAIL);

			if (pUsbHid->m_InterruptTimeOut)
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA - Interrupt timeout");
			else
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA");
		}
		else
		{
			bRet = CheckHighVoltageRange(pUsbHid, sRawData);

			if (bRet == FALSE)
			{
				Log_Append(pUsbHid, RESULT_FAIL, TRUE);
				DisplayResultToList(pUsbHid, RESULT_FAIL);
			}
			else
			{
				Log_Append(pUsbHid, RESULT_PASS, TRUE);
				DisplayResultToList(pUsbHid, RESULT_PASS);
			}

			TSM_AddData_InLog(pUsbHid, sRawData);
		}

		Log_AppendEx(pUsbHid, "\r\n");
	}
	else
	{
		pUsbHid->m_bErrorRead = FALSE;

		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "23_CND"			//add 2022.11.14 for ZTM730 CND
	if (m_TestInfo_CND.bCheckData == TRUE)
	{
		bRet = ReadRawData(pUsbHid, sRawData);

		if (bRet == FALSE)
		{
			bErrorReadRawData_CND = TRUE;

			Log_Append(pUsbHid, RESULT_FAIL);

			if (pUsbHid->m_InterruptTimeOut)
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA - Interrupt timeout");
			else
				DisplayResultFailToList(pUsbHid, "FAIL : CANNOT READ RAW DATA");
		}
		else
		{
			bRet = CheckCNDRawDataRange(pUsbHid, sRawData);

			if (bRet == FALSE)
			{
				Log_Append(pUsbHid, RESULT_FAIL, TRUE);
				DisplayResultToList(pUsbHid, RESULT_FAIL);
			}
			else
			{
				Log_Append(pUsbHid, RESULT_PASS, TRUE);
				DisplayResultToList(pUsbHid, RESULT_PASS);
			}

			TSM_AddData_InLog(pUsbHid, sRawData);
		}

		Log_AppendEx(pUsbHid, "\r\n");
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "24_VIRTUAL_DRAW_TEST"
	if(m_bCheckVirtualDrawTest)
	{
		bRet = CheckVirtualDrawTest(pUsbHid);

		if (bRet == FALSE)
		{
			Log_Append(pUsbHid, RESULT_FAIL, FALSE);
			DisplayResultFailToList(pUsbHid, pUsbHid->m_strRet);
		}
		else
		{
			Log_Append(pUsbHid, RESULT_PASS, FALSE);
			DisplayResultToList(pUsbHid, RESULT_PASS);
		}
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// "25_CHECK_VIH"
	
	if (m_bCheckI2C_Level == TRUE)
	{
		BOOL bTestResult = TRUE;
		double i2cformat;
		
		if(m_nI2CFormat == 0){  i2cformat =1.10;}
		if(m_nI2CFormat == 1){  i2cformat =1.26;}
		if(m_nI2CFormat == 2){  i2cformat =1.36;}
		/*
		if(m_nI2CFormat == 3){  i2cformat =1.80;}
		if(m_nI2CFormat == 4){  i2cformat =1.90;}
		if(m_nI2CFormat == 5)
		{  
		i2cformat =g_vcca_ref;
		}
		*/
	 		if (i2cformat != m_vih_ref)
	 		{		// CHECK IC REVISION
				bTestResult = FALSE;
				DisplayResultFailToList(pUsbHid, "REG_IC_REVISION [0x%04X] IS NOT REF[0x%04X]", pUsbHid->m_CapInfo.icRegData[REG_CHIP_REVISION]&0xFFFF, m_nIcReivsion&0xFFFF);
			}


			if (bTestResult == FALSE)
			{
				Log_Append(pUsbHid, RESULT_FAIL, FALSE);
				DisplayResultFailToList(pUsbHid, pUsbHid->m_strRet);
			}
			else
			{
				Log_Append(pUsbHid, RESULT_PASS, FALSE);
				DisplayResultToList(pUsbHid, RESULT_PASS);
			}
	
	}
	else
	{
		Log_Append(pUsbHid, RESULT_NONE);
		DisplayResultToList(pUsbHid, RESULT_NONE);
	}

	pUsbHid->m_nTestStep++;
	UpdateProgressBar(pUsbHid);
	

	
	goto exit_tsm_test;

fail_tsm_test:

	pUsbHid->m_nResult = RESULT_FAIL;

	DisplayResultByTestStop(pUsbHid);

exit_tsm_test:

	if (g_bAppExit)
		return 0;

	pUsbHid->m_strResult = "";

	//::ReleaseSemaphore(g_hSemaphore, 1, NULL);

	return (pUsbHid->m_nResult == RESULT_FAIL) ? FALSE : TRUE;

}


void CTestModeView::TSM_SubTestEnd(CUsbHid * pTestEndUsbHid)
{
	BOOL bTestResult = TRUE;
	CString str;
	int n;

	if (pTestEndUsbHid == NULL || g_bAppExit)
		return;

#if defined(D_MEASURE_ELAPSED_TIME)
	TRACE("\t [STEP_%02d] %llu msec \n", pTestEndUsbHid->m_nTestStep, GetTimeInMillis()-m_dt);
#endif

	TRACE("%s [%d] \n", __FUNCTION__, pTestEndUsbHid->m_nDevIndex);

#if defined(D_USE_CHECK_BUTTON_TEST)
	SemaWait(g_hInfoSemaphore);
	m_GpioButtonStatus.nStatus[pTestEndUsbHid->m_nDevIndex] = E_BUTTON_TEST_OFF;
	m_TouchButtonStatus.nStatus[pTestEndUsbHid->m_nDevIndex] = E_BUTTON_TEST_OFF;
	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);
#endif

	if (pTestEndUsbHid->m_bTestResult == FALSE || pTestEndUsbHid->m_bDownloadResult == FALSE)
	{
		bTestResult = FALSE;
	}

	str.Format("%s,\t\t,\t\t%s,\t\t%d ms\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "ELAPSETIME"), pTestEndUsbHid->m_strDevName, (int)(GetTimeInMillis() - pTestEndUsbHid->m_nTestStartTime));
	pTestEndUsbHid->m_strLog.Append(str);
	pTestEndUsbHid->m_strLogging.Append(str);

	str.Format("%s,\t\t,\t\t%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "RESULT"), pTestEndUsbHid->m_strDevName, pTestEndUsbHid->m_bTestResult ? "PASS" : "FAIL");
	pTestEndUsbHid->m_strLog.Append(str);
	pTestEndUsbHid->m_strLogging.Append(str);

	// Save result Log
	if (m_bSaveResult == TRUE)
	{
		TSM_SaveLogFile(pTestEndUsbHid);
	}

	if (bTestResult == FALSE && IsExistUsbHid(pTestEndUsbHid) == FALSE)
	{
		// Device is lost by RESET button while FW downloading is in progress.
		// CloseHidHandle() function is called by HID communication error in CUsbHid.
		// Device will re-connect after TSM Test is stopped, then HID device handle will get.
		// If device is detached, device will remove by RemoveHidDevice().

		pTestEndUsbHid->m_bDevLost = TRUE;
		pTestEndUsbHid->m_bInit = FALSE;

		SetListState(pTestEndUsbHid, pTestEndUsbHid->m_nDevIndex + FIRST_DEVICE_INDEX, ITEM_STATE_DISABLE);

		m_listInfo.SetItemCount((int)m_infoDatabase.size());

		//RemoveHidDevice(pTestEndUsbHid);

		#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
		m_pConnectInfoDlg->AddLog("DIS-CONNECT", pTestEndUsbHid);
		#endif
	}

	if (bTestResult == FALSE && pTestEndUsbHid->m_bDevLost == FALSE)
	{
		SetListState(pTestEndUsbHid, pTestEndUsbHid->m_nDevIndex + FIRST_DEVICE_INDEX, ITEM_STATE_TEST_FAIL);

		TestResultDesc_SetCurSel(pTestEndUsbHid->m_nDevIndex);
	}
	else
	{
		SetListState(pTestEndUsbHid, pTestEndUsbHid->m_nDevIndex + FIRST_DEVICE_INDEX, ITEM_STATE_TEST_SUCCESS);
	}

	m_listInfo.Invalidate();

	BarcodeSetting_TestCompleted(pTestEndUsbHid->m_nDevIndex, FALSE);	// NO_ERR

	SemaWait(g_hInfoSemaphore);

	// 하단 total 결과
	if (bTestResult == TRUE) {
		m_Pass_Count++;
	}
	else {
		m_Fail_Count++;
	}

	str.Format("%d", m_Pass_Count);
	GetDlgItem(IDC_STATIC_PASS_COUNT)->SetWindowTextA(str);
	str.Format("%d", m_Fail_Count);
	GetDlgItem(IDC_STATIC_FAIL_COUNT)->SetWindowTextA(str);
	str.Format("%d", m_Pass_Count + m_Fail_Count);
	GetDlgItem(IDC_STATIC_TOTAL_COUNT)->SetWindowTextA(str);

	pTestEndUsbHid->m_bTesting = FALSE;
	pTestEndUsbHid->m_bSingleTest = FALSE;

	if(INDIVIDUAL_TEST_START_ON)
	{
		COleDateTime oletimeTime = COleDateTime::GetCurrentTime();
		TestResultDesc_AddMessage(pTestEndUsbHid->m_nDevIndex, ZT_COLOR_BLACK, "TEST END : %04d%02d%02d_%02d%02d%02d\r\n", oletimeTime.GetYear(), oletimeTime.GetMonth(), oletimeTime.GetDay(), oletimeTime.GetHour(), oletimeTime.GetMinute(), oletimeTime.GetSecond());
		TestResultDesc_AddMessage(pTestEndUsbHid->m_nDevIndex, ZT_COLOR_BLACK, "Total elapse time : %d ms\r\n", (int)(GetTimeInMillis() - m_nTestStartTime));
	}

	CUsbHid * pUsbHid;
	for (n = 0; n < m_nMaxDeviceIndex; n++)
	{
		pUsbHid = GetUsbHidByDevIndex(n);
		if (pUsbHid == NULL || pUsbHid->IsTestAvailable() == FALSE)	continue;

		if (pUsbHid->m_bTesting || pUsbHid->m_bSingleTest)
		{
			::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);
			goto skip_TsmTestEnd;							// All devices did not complete TSM TEST
		}

		pUsbHid->m_strResult = "";
	}

	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);

	// All devices is completed TSM TEST
	TSM_TestEnd();

	TSM_SaveLoggingFile();

	if(m_eAllRetryStatus == eAllRetryStatus_Nothing)
	{
		int nDeviceCount = m_TestDevIndex.GetCount();
		for (int n = 0; n < nDeviceCount; n++)
		{
			pUsbHid = GetUsbHidByDevIndex(m_TestDevIndex.GetAt(n));
			if (pUsbHid == NULL)	continue;

			pUsbHid->m_strBarcode = "";
		}
	}

skip_TsmTestEnd:

	if (pTestEndUsbHid->m_DeviceHandle == NULL)
	{
		((CTestModeView*)g_pTestModeViewWnd)->PostMessageEx(WM_USER_USB_HID_DEVICE_DISCONNECT, pTestEndUsbHid->m_nDevIndex, 0);
	}
}

void CTestModeView::TSM_TestEnd()
{
	BOOL bTestResult = TRUE;
	CUsbHid * pUsbHid;
	int nDeviceCount;

	nDeviceCount = m_TestDevIndex.GetCount();

	if (nDeviceCount > 0)
	{
		if(INDIVIDUAL_TEST_START_OFF)
		{
			COleDateTime oletimeTime = COleDateTime::GetCurrentTime();
			TestResultDesc_AddMessage(0, ZT_COLOR_BLACK, "TEST END : %04d%02d%02d_%02d%02d%02d\r\n", oletimeTime.GetYear(), oletimeTime.GetMonth(), oletimeTime.GetDay(), oletimeTime.GetHour(), oletimeTime.GetMinute(), oletimeTime.GetSecond());
			TestResultDesc_AddMessage(0, ZT_COLOR_BLACK, "Total elapse time : %d ms\r\n", (int)(GetTimeInMillis() - m_nTestStartTime));
		}

		// Check test result
		for (int n = 0; n < nDeviceCount; n++)
		{
			pUsbHid = GetUsbHidByDevIndex(m_TestDevIndex.GetAt(n));
			if (pUsbHid == NULL)	continue;

			if (pUsbHid->m_bTestResult == FALSE || pUsbHid->m_bDownloadResult == FALSE)
			{
				bTestResult = FALSE;
				break;
			}
		}

		// Display test result image
		if (bTestResult == TRUE)
		{
			m_picPass.ShowWindow(TRUE);
			m_picFail.ShowWindow(FALSE);
			m_picStandBy.ShowWindow(FALSE);
		}
		else
		{
			m_picPass.ShowWindow(FALSE);
			m_picFail.ShowWindow(TRUE);
			m_picStandBy.ShowWindow(FALSE);
		}

		m_TabCtrl.EnableDraw(BTC_TABS);

		// Check F/W auto download condition
		if ((bTestResult == FALSE) && IsAutoFwDownload_Condition())
		{
			for (int n = 0; n < nDeviceCount; n++)
			{
				pUsbHid = GetUsbHidByDevIndex(m_TestDevIndex.GetAt(n));
				if (pUsbHid == NULL)	continue;

				if (pUsbHid->m_bErrorVersionCheck)
				{
					m_bTestEnterByFwAutoDownload = TRUE;
					break;
				}
			}
		}
		else
		{
			m_bTestEnterByFwAutoDownload = FALSE;	// retry is 1 time
		}
	}

	m_bAleadyTestEnter = FALSE;
	::ReleaseSemaphore(g_hSemaphore, 1, NULL);

	if (m_bTestEnterByFwAutoDownload)
	{
		Sleep(1000);

		BOOL bRet = -1;
		do
		{
			PostMessage(WM_USER_TSM_TEST_START_BY_FW_AUTO_DOWNLOAD, 0, 0);
		}while( bRet == 0 );

		return;
	}

	//GetDlgItem(IDC_LIST_INFO)->SetFocus();
	((CTestModeView*)g_pTestModeViewWnd)->PostMessageEx(WM_USER_SET_FOCUS_TO_LIST, 0, 0);

	switch (m_eAllRetryStatus)
	{
	case eAllRetryStatus_Nothing:
		m_nAllRetryCopyCount = 0;
		break;

	case eAllRetryStatus_Running:
		m_nAllRetryCopyCount--;
		if (m_nAllRetryCopyCount > 0)
			SetTimer(TIMER_DEBUG_TEST_AGAIN, 1000, NULL);
		else
			m_eAllRetryStatus = eAllRetryStatus_Nothing;

		break;
	}

	if(m_eAllRetryStatus == eAllRetryStatus_Nothing)
	{
		m_nBarcodeChIdx = 0;
		m_nBarcodeIdx = 0;

		memset(m_cBarcode, 0x00, sizeof(m_cBarcode));

		BarcodeDisplayInTestVisible();

		if (bTestResult == TRUE && m_nStandBySec > 0)
		{
			SetTimer(TIMER_GOTO_STANDBY, m_nStandBySec * 1000, NULL);
		}

		if (bTestResult == TRUE && m_nSolutionResultNotify > 0)
		{
			for (int n = 0; n < nDeviceCount; n++)
			{
				pUsbHid = GetUsbHidByDevIndex(m_TestDevIndex.GetAt(n));
				if (pUsbHid != NULL)
				{
					pUsbHid->ControlGpioPb15(TRUE);
				}
			}

			SetTimer(TIMER_SOLUTION_RESULT_NOTIFY, m_nSolutionResultNotify * 1000, NULL);
		}

		EnableStartButton(TRUE);
		EnableOptionButton(TRUE);

		UsbHidDevice_StartSurveyTimer(TRUE);
	}

#if defined(D_DEBUG_FW_UPGRADE_TEST)
	if (nDeviceCount > 0)
	{
		SetTimer(TIMER_DEBUG_FW_UPGRADE_TEST, 2000, NULL);
	}
#endif
}

//==============================================================================================

void CTestModeView::TSM_AddData_InLog(CUsbHid * pUsbHid, short int *pRawData, int nTestStep)
{
	int i, j;

	if (pUsbHid->m_nTestStep == TEST_HIGH_VOLTAGE)	//add 2022.10.11
	{
		Logging_AppendEx(pUsbHid, ",\t%05d", pRawData[0]);
		Logging_AppendEx(pUsbHid, "\r\n");

		Log_AppendEx(pUsbHid, ",\t%05d", pRawData[0]);
		Log_AppendEx(pUsbHid, "\r\n");

	}
	else if(pUsbHid->m_nTestStep == TEST_DND || pUsbHid->m_nTestStep == TEST_HF_DND ||
		    pUsbHid->m_nTestStep == TEST_TRX_SHORT || pUsbHid->m_nTestStep == TEST_HW_CALIBRATION ||
		    pUsbHid->m_nTestStep == TEST_JITTER || pUsbHid->m_nTestStep == TEST_DIGITAL_WHEEL
		 || pUsbHid->m_nTestStep == TEST_CND
	)
	{
		int nTotalNumberX = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X];
		int nTotalNumberY = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y];

		//-----------------------------------------------
		// LOGGING
		for (j = 0; j < nTotalNumberX * nTotalNumberY; j++)
			Logging_AppendEx(pUsbHid, ",%05d", pRawData[j]);
		Logging_AppendEx(pUsbHid, "\r\n");

		//-----------------------------------------------
		// LOG
		Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\tCH___", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, m_strArrTestInfo.GetAt(pUsbHid->m_nTestStep)));
		for (i = 0; i < nTotalNumberY; i++)
			Log_AppendEx(pUsbHid, ",Y%04d", i);
		Log_AppendEx(pUsbHid, "\r\n");

		for (i = 0; i < nTotalNumberX; i++)
		{
			Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\tX%04d", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, m_strArrTestInfo.GetAt(pUsbHid->m_nTestStep)), i);
			for (j = 0; j < nTotalNumberY; j++)
				Log_AppendEx(pUsbHid, ",%05d", pRawData[i*nTotalNumberY + j]);
			Log_AppendEx(pUsbHid, "\r\n");
		}
	}
	else if (pUsbHid->m_nTestStep == TEST_DND_GAP || pUsbHid->m_nTestStep == TEST_HF_DND_GAP )
	{
		short int * sDiffData = pRawData;
		CString strTestTitle;

		if (nTestStep == TEST_STEP_CHECK_H_DIFF)
		{
			int nTotalNumberX = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X];
			int nTotalNumberY = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] - 1;
			int nPitchNumberY = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y];

			//-----------------------------------------------
			// LOGGING
			for (i = 0; i < nTotalNumberX; i++)
				for (j = 0; j < nTotalNumberY; j++)
					Logging_AppendEx(pUsbHid, ",%05d", sDiffData[i*nPitchNumberY + j]);
			Logging_AppendEx(pUsbHid, "\r\n");

			//-----------------------------------------------
			// LOG
			if(pUsbHid->m_nTestStep == TEST_DND_GAP)
				strTestTitle.Format("%s", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, "DND_H_GAP"));
			else
				strTestTitle.Format("%s", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, "HF_DND_H_GAP"));

			Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\tCH___", strTestTitle);
			for (i = 0; i < nTotalNumberY; i++)
				Log_AppendEx(pUsbHid, ",Y%02d%02d", i, i+1);
			Log_AppendEx(pUsbHid, "\r\n");

			for (i = 0; i < nTotalNumberX; i++)
			{
				Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\tX%04d", strTestTitle, i);
				for (j = 0; j < nTotalNumberY; j++) {
					Log_AppendEx(pUsbHid, ",%05d", sDiffData[i*nPitchNumberY + j]);
				}
				Log_AppendEx(pUsbHid, "\r\n");
			}
		}
		else if (nTestStep == TEST_STEP_CHECK_V_DIFF)
		{
			int nTotalNumberX = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] - 1;
			int nTotalNumberY = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y];

			//-----------------------------------------------
			// LOGGING
			for (i = 0; i < nTotalNumberX; i++)
				for (j = 0; j < nTotalNumberY; j++)
					Logging_AppendEx(pUsbHid, ",%05d", sDiffData[i*nTotalNumberY + j]);
			Logging_AppendEx(pUsbHid, "\r\n");

			//-----------------------------------------------
			// LOG
			if(pUsbHid->m_nTestStep == TEST_DND_GAP)
				strTestTitle.Format("%s", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, "DND_V_GAP"));
			else
				strTestTitle.Format("%s", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, "HF_DND_V_GAP"));

			Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\tCH___", strTestTitle);

			for (i = 0; i < nTotalNumberY; i++)
				Log_AppendEx(pUsbHid, ",Y%04d", i);
			Log_AppendEx(pUsbHid, "\r\n");

			for (i = 0; i < nTotalNumberX; i++)
			{
				Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\tX%02d%02d", strTestTitle, i, i+1);
				for (j = 0; j < nTotalNumberY; j++) {
					Log_AppendEx(pUsbHid, ",%05d", sDiffData[i*nTotalNumberY + j]);
				}
				Log_AppendEx(pUsbHid, "\r\n");
			}
		}
	}
}

void CTestModeView::TSM_AddData_InLog_Self(CUsbHid * pUsbHid, short int * pRxRawData, short int * pTxRawData)
{
	int i, j;

	if (pUsbHid->m_nTestStep == TEST_ONLY_SELF_DND)
	{
		int nTotalNumberX = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X];
		int nTotalNumberY = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y];

		//-----------------------------------------------
		// LOGGING
		for (j = 0; j < nTotalNumberY; j++)
			Logging_AppendEx(pUsbHid, ",%05d", pRxRawData[j]);

		for (j = 0; j < nTotalNumberX * nTotalNumberY; j++)
			Logging_AppendEx(pUsbHid, ",%05d", pTxRawData[j]);
		Logging_AppendEx(pUsbHid, "\r\n");

		//-----------------------------------------------
		// LOG
		Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\tCHY__", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, m_strArrTestInfo.GetAt(pUsbHid->m_nTestStep)));
		for (i = 0; i < nTotalNumberY; i++)
			Log_AppendEx(pUsbHid, ",%05d", pRxRawData[i]);
		Log_AppendEx(pUsbHid, "\r\n");
		Log_AppendEx(pUsbHid, "\r\n");

		for (i = 0; i < nTotalNumberX; i++)
		{
			if (i != 0)
				Log_AppendEx(pUsbHid, "\r\n");

			Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\tCHX%02d", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, m_strArrTestInfo.GetAt(pUsbHid->m_nTestStep)), i);

			for (j = 0; j < nTotalNumberY; j++)
			{
				Log_AppendEx(pUsbHid, ",%05d", pTxRawData[i * nTotalNumberY + j]);
			}
		}

		Log_AppendEx(pUsbHid, "\r\n");
	}
	else if (pUsbHid->m_nTestStep == TEST_SELF_DND || pUsbHid->m_nTestStep == TEST_SELF_SATURATION)
	{
		int nTotalNumberX = pUsbHid->m_nSelfRowCount;
		int nTotalNumberY = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y];

		if(pUsbHid->IsCoreFrameWork() == TRUE)
			nTotalNumberY = pUsbHid->m_nSelfChannelCntY;

		//-----------------------------------------------
		// LOGGING
		for (j = 0; j < nTotalNumberY; j++)
			Logging_AppendEx(pUsbHid, ",%05d", pRxRawData[j]);

		for (j = 0; j < nTotalNumberX * nTotalNumberY; j++)
			Logging_AppendEx(pUsbHid, ",%05d", pTxRawData[j]);
		Logging_AppendEx(pUsbHid, "\r\n");

		//-----------------------------------------------
		// LOG
		Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\tCHY__", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, m_strArrTestInfo.GetAt(pUsbHid->m_nTestStep)));
		for (i = 0; i < nTotalNumberY; i++)
			Log_AppendEx(pUsbHid, ",%05d", pRxRawData[i]);
		Log_AppendEx(pUsbHid, "\r\n");
		Log_AppendEx(pUsbHid, "\r\n");

		for(i = 0; i < nTotalNumberX; i++)
		{
			if (i != 0)
				Log_AppendEx(pUsbHid, "\r\n");

			Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\tCHX%02d", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, m_strArrTestInfo.GetAt(pUsbHid->m_nTestStep)), i);

			for(j = 0; j < nTotalNumberY; j++)
			{
				Log_AppendEx(pUsbHid, ",%05d", pTxRawData[i*nTotalNumberY + j]);
			}
		}

		Log_AppendEx(pUsbHid, "\r\n");
	}
	else if (pUsbHid->m_nTestStep == TEST_SELF_DND_GAP)
	{
		int nTotalNumberX = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X];
		int nTotalNumberY = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y];

		if(pUsbHid->IsCoreFrameWork() == TRUE)
		{
			nTotalNumberX = pUsbHid->m_nSelfChannelCntX;
			nTotalNumberY = pUsbHid->m_nSelfChannelCntY;
		}

		//-----------------------------------------------
		// LOGGING
		for (i = 0; i < nTotalNumberY-1; i++)
			Logging_AppendEx(pUsbHid, ",%05d", pRxRawData[i]);

		for(i = 0; i < nTotalNumberX-1; i++)
			Logging_AppendEx(pUsbHid, ",%05d", pTxRawData[i]);
		Logging_AppendEx(pUsbHid, "\r\n");

		//-----------------------------------------------
		// LOG
		Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\tCHY__", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, m_strArrTestInfo.GetAt(pUsbHid->m_nTestStep)));
		for (i = 0; i < nTotalNumberY-1; i++)
			Log_AppendEx(pUsbHid, ",%05d", pRxRawData[i]);
		Log_AppendEx(pUsbHid, "\r\n");

		Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\tCHX__", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, m_strArrTestInfo.GetAt(pUsbHid->m_nTestStep)));
		for(i = 0; i < nTotalNumberX-1; i++)
			Log_AppendEx(pUsbHid, ",%05d", pTxRawData[i]);
		Log_AppendEx(pUsbHid, "\r\n");
	}
}

void CTestModeView::TSM_AddData_InLog_MeasureDelta(CUsbHid * pUsbHid, short int * pRawData)
{
	int i, j;

	if (pUsbHid->m_nTestStep != TEST_MEASURE_DELTA)
		return;

	int nTotalNumberX = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X];
	int nTotalNumberY = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y];

	int nSelfYPos, nSelfXPos;
	nSelfYPos = (int)(nTotalNumberX * nTotalNumberY);	// Mutual Data
	nSelfYPos += nTotalNumberY;							// Null
	nSelfXPos = nSelfYPos + nTotalNumberY;				// Y Self

	//-----------------------------------------------
	// LOGGING
	for (j = 0; j < nTotalNumberX * nTotalNumberY; j++)
		Logging_AppendEx(pUsbHid, ",%05d", pRawData[j]);

	Logging_AppendEx(pUsbHid, ",\t");
	for (j = 0; j < nTotalNumberY; j++)
		Logging_AppendEx(pUsbHid, ",%05d", pRawData[nSelfYPos + j]);

	Logging_AppendEx(pUsbHid, ",\t");
	for (j = 0; j < nTotalNumberX; j++)
		Logging_AppendEx(pUsbHid, ",%05d", pRawData[nSelfXPos + j]);
	Logging_AppendEx(pUsbHid, "\r\n");

	//-----------------------------------------------
	// LOG
	Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\tCH___", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, m_strArrTestInfo.GetAt(pUsbHid->m_nTestStep)));
	for (i = 0; i < nTotalNumberY; i++)
		Log_AppendEx(pUsbHid, ",Y%04d", i);
	Log_AppendEx(pUsbHid, ",SX___");
	Log_AppendEx(pUsbHid, "\r\n");

	for (i = 0; i < nTotalNumberX; i++)
	{
		Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\tX%04d", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, m_strArrTestInfo.GetAt(pUsbHid->m_nTestStep)), i);
		for (j = 0; j < nTotalNumberY; j++) {
			Log_AppendEx(pUsbHid, ",%05d", pRawData[i*nTotalNumberY + j]);
		}

		// SX
		Log_AppendEx(pUsbHid, ",%05d", pRawData[nSelfXPos + i]);
		Log_AppendEx(pUsbHid, "\r\n");
	}

	// SY
	Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\tSY___", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, m_strArrTestInfo.GetAt(pUsbHid->m_nTestStep)));
	for (j = 0; j < nTotalNumberY; j++) {
		Log_AppendEx(pUsbHid, ",%05d", pRawData[nSelfYPos + j]);
	}
}

void CTestModeView::TSM_AddData_InLog_Noise(CUsbHid * pUsbHid, short int *sNoise)
{
	int i, j;

	//-----------------------------------------------
	// LOGGING
	int readTimes;

	readTimes = m_TestInfo_NOISE.m_nCheckNoise_ReadTimes < 99 ? m_TestInfo_NOISE.m_nCheckNoise_ReadTimes : 99;

	Logging_AppendEx(pUsbHid, "\r\n");

	if (m_TestInfo_NOISE.AfeInfo.bPrintPpVariation != 1)
	{
		Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\t Value:", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, ""));
		Log_AppendEx(pUsbHid, ",%05d", sNoise[99]);
		Log_AppendEx(pUsbHid, "\r\n");
	}
	else
	{
		Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\tTimes:", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, ""));
		for (i = 0; i < readTimes; i++)
			Log_AppendEx(pUsbHid, ",Time%d", (i + 1));
		Log_AppendEx(pUsbHid, "\r\n");


		//-----------------------------------------------
		// LOG
		Log_AppendEx(pUsbHid, "%s,\t\t,\t\t,\t\t,\t\t", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, ""));

		for (j = 0; j < readTimes; j++)
			Log_AppendEx(pUsbHid, ",%05d", sNoise[j]);
		Log_AppendEx(pUsbHid, "\r\n");
	}
}

//==============================================================================================

BOOL CTestModeView::ReadConfigRegister(CUsbHid * pUsbHid)
{
	if (pUsbHid->GetTouchConfigEx(REG_TOTAL_NUMBER_OF_X, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X]) != 0) return FALSE;
	if (pUsbHid->GetTouchConfigEx(REG_TOTAL_NUMBER_OF_Y, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]) != 0) return FALSE;
	if (pUsbHid->GetTouchConfigEx(REG_X_RESOLUTION, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_X_RESOLUTION]) != 0) return FALSE;
	if (pUsbHid->GetTouchConfigEx(REG_Y_RESOLUTION, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_Y_RESOLUTION]) != 0) return FALSE;

	if (pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] <= 0 || pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] >= DEF_MAX_X_LINE_NUM ||
		pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] <= 0 || pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] >= DEF_MAX_Y_LINE_NUM ||
		pUsbHid->m_CapInfo.icRegData[REG_X_RESOLUTION] <= 0 || pUsbHid->m_CapInfo.icRegData[REG_Y_RESOLUTION] <= 0)
	{
		return FALSE;
	}


	if (pUsbHid->GetTouchConfigEx(REG_CHIP_REVISION, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_CHIP_REVISION]) != 0) return FALSE;
	if (pUsbHid->GetTouchConfigEx(REG_HW_ID, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_HW_ID]) != 0) return FALSE;
	if (pUsbHid->GetTouchConfigEx(REG_FIRMWARE_VERSION, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_FIRMWARE_VERSION]) != 0) return FALSE;
	if (pUsbHid->GetTouchConfigEx(REG_MINOR_FW_VERSION, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_MINOR_FW_VERSION]) != 0) return FALSE;
	if (pUsbHid->GetTouchConfigEx(REG_DATA_VERSION, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_DATA_VERSION]) != 0) return FALSE;

	if (pUsbHid->GetTouchConfigEx(REG_PROTOCOL_MODE, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_PROTOCOL_MODE]) != 0) return FALSE;

	if (pUsbHid->GetTouchConfigEx(REG_JITTER_N_DATA_CNT, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_JITTER_N_DATA_CNT]) != 0) return FALSE;

			// AFE Setting
			if (pUsbHid->GetTouchConfigEx(REG_DND_FREQUENCY, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_DND_FREQUENCY]) != 0) return FALSE;
			if (pUsbHid->GetTouchConfigEx(REG_DND_N_COUNT, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_DND_N_COUNT]) != 0) return FALSE;
			if (pUsbHid->GetTouchConfigEx(REG_DND_U_COUNT, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_DND_U_COUNT]) != 0) return FALSE;

			if (pUsbHid->GetTouchConfigEx(REG_HF_DND_FREQUENCY, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_HF_DND_FREQUENCY]) != 0) return FALSE;
			if (pUsbHid->GetTouchConfigEx(REG_HF_DND_N_COUNT, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_HF_DND_N_COUNT]) != 0) return FALSE;
			if (pUsbHid->GetTouchConfigEx(REG_HF_DND_U_COUNT, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_HF_DND_U_COUNT]) != 0) return FALSE;

			if (pUsbHid->GetTouchConfigEx(REG_SELF_DND_FREQUENCY_A, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_SELF_DND_FREQUENCY_A]) != 0) return FALSE;
			if (pUsbHid->GetTouchConfigEx(REG_SELF_DND_N_COUNT_A, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_SELF_DND_N_COUNT_A]) != 0) return FALSE;
			if (pUsbHid->GetTouchConfigEx(REG_SELF_DND_U_COUNT_A, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_SELF_DND_U_COUNT_A]) != 0) return FALSE;
			if (pUsbHid->GetTouchConfigEx(REG_SELF_DND_FREQUENCY_B, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_SELF_DND_FREQUENCY_B]) != 0) return FALSE;
			if (pUsbHid->GetTouchConfigEx(REG_SELF_DND_N_COUNT_B, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_SELF_DND_N_COUNT_B]) != 0) return FALSE;
			if (pUsbHid->GetTouchConfigEx(REG_SELF_DND_U_COUNT_B, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_SELF_DND_U_COUNT_B]) != 0) return FALSE;

	if (pUsbHid->GetTouchConfigEx(REG_SELF_SATURATION_FREQUENCY_A, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_SELF_SATURATION_FREQUENCY_A]) != 0) return FALSE;
	if (pUsbHid->GetTouchConfigEx(REG_SELF_SATURATION_N_COUNT_A, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_SELF_SATURATION_N_COUNT_A]) != 0) return FALSE;
	if (pUsbHid->GetTouchConfigEx(REG_SELF_SATURATION_U_COUNT_A, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_SELF_SATURATION_U_COUNT_A]) != 0) return FALSE;
	if (pUsbHid->GetTouchConfigEx(REG_SELF_SATURATION_FREQUENCY_B, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_SELF_SATURATION_FREQUENCY_B]) != 0) return FALSE;
	if (pUsbHid->GetTouchConfigEx(REG_SELF_SATURATION_N_COUNT_B, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_SELF_SATURATION_N_COUNT_B]) != 0) return FALSE;
	if (pUsbHid->GetTouchConfigEx(REG_SELF_SATURATION_U_COUNT_B, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_SELF_SATURATION_U_COUNT_B]) != 0) return FALSE;

	if (pUsbHid->GetTouchConfigEx(REG_DEFAULT_FREQUENCY, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_DEFAULT_FREQUENCY]) != 0) return FALSE;
	if (pUsbHid->GetTouchConfigEx(REG_DEFAULT_U_COUNT, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_DEFAULT_U_COUNT]) != 0) return FALSE;
	if (pUsbHid->GetTouchConfigEx(REG_DEFAULT_N_COUNT, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_DEFAULT_N_COUNT]) != 0) return FALSE;

	if (pUsbHid->IsCoreFrameWork() == FALSE)
	{
		if (pUsbHid->GetTouchConfigEx(REG_DEFAULT_U_COUNT_CHINA, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_DEFAULT_U_COUNT_CHINA]) != 0) return FALSE;
		if (pUsbHid->GetTouchConfigEx(REG_DEFAULT_N_COUNT_CHINA, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_DEFAULT_N_COUNT_CHINA]) != 0) return FALSE;

		pUsbHid->m_nSelfChannelCntX = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X];
		pUsbHid->m_nSelfChannelCntY = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y];
	}
	else
	{
		if (pUsbHid->GetTouchConfigEx(REG_CUSTOM_BUTTON_CHANNEL, (BYTE*)&pUsbHid->m_CapInfo.icRegData[REG_CUSTOM_BUTTON_CHANNEL]) != 0) return FALSE;
		if (pUsbHid->GetTouchConfigEx(REG_CUSTOM_BUTTON_Y_CHANNEL, (BYTE*)&pUsbHid->m_CapInfo.icRegData[REG_CUSTOM_BUTTON_Y_CHANNEL]) != 0) return FALSE;

		pUsbHid->m_nSelfChannelCntX = pUsbHid->m_CapInfo.icRegData[REG_CUSTOM_BUTTON_CHANNEL];
		pUsbHid->m_nSelfChannelCntY = pUsbHid->m_CapInfo.icRegData[REG_CUSTOM_BUTTON_Y_CHANNEL];
	}

	if (pUsbHid->GetTouchConfigEx(REG_BUTTON_SUPPORTED_NUM, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_BUTTON_SUPPORTED_NUM]) != 0) return FALSE;

	if(pUsbHid->m_CapInfo.icRegData[REG_BUTTON_SUPPORTED_NUM] > 8)
		pUsbHid->m_CapInfo.icRegData[REG_BUTTON_SUPPORTED_NUM] = 8;

	if (pUsbHid->GetTouchConfigEx(REG_BUTTON_LINE_NUM, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_BUTTON_LINE_NUM]) != 0) return FALSE;
	if (pUsbHid->GetTouchConfigEx(REG_BUTTON_LINE_TYPE, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_BUTTON_LINE_TYPE]) != 0) return FALSE;

	// Calculate AFE Setting
	for (int nTestStep=0; nTestStep<TEST_MAX; nTestStep++)
	{
		CalculateAfeSetting(pUsbHid, nTestStep);
	}

#if defined(D_USE_FW_CHINA_FRAMEWORK)
	//-----------------------------------------------
	// Set maximum X, Y line number
	pUsbHid->m_nTotalNumberX = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X];
	pUsbHid->m_nTotalNumberY = pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y];

	// get self row count
	if (pUsbHid->IsCoreFrameWork() == FALSE)//(IFE_MODE_OFF)  //수정 22_10_12
	{ // CHINA
		// Set maximum X, Y line number to self-data (using CHINA)
		SetMaxLineNumber(pUsbHid, &pUsbHid->m_IC_Max_X_Line_Num, &pUsbHid->m_IC_Max_Y_Line_Num);

		if (pUsbHid->m_IC_Max_X_Line_Num == 0 || pUsbHid->m_IC_Max_Y_Line_Num == 0)	return FALSE;    // 22.10.27 채널 사이드 박현전 수석 요청

		pUsbHid->m_nSelfRowCount = pUsbHid->m_IC_Max_X_Line_Num / pUsbHid->m_IC_Max_Y_Line_Num;
		if ((pUsbHid->m_IC_Max_X_Line_Num % pUsbHid->m_IC_Max_Y_Line_Num) > 0)
			pUsbHid->m_nSelfRowCount++;
	}
	else
	{ // IFE
		pUsbHid->m_nTotalNumberX = pUsbHid->m_nSelfChannelCntX;
		pUsbHid->m_nTotalNumberX += pUsbHid->m_CapInfo.icRegData[REG_BUTTON_SUPPORTED_NUM] * (pUsbHid->m_CapInfo.icRegData[REG_BUTTON_LINE_TYPE] == 1 ? 1 : 0);
		pUsbHid->m_nTotalNumberY = pUsbHid->m_nSelfChannelCntY;

		if (pUsbHid->m_nTotalNumberX == 0 || pUsbHid->m_nTotalNumberY == 0)	return FALSE;

		pUsbHid->m_nSelfRowCount = pUsbHid->m_nTotalNumberX / pUsbHid->m_nTotalNumberY;
		if ((pUsbHid->m_nTotalNumberX % pUsbHid->m_nTotalNumberY) > 0)
			pUsbHid->m_nSelfRowCount++;
	}

#if 0	// SIM_TODO. CHINA
	pUsbHid->m_strLogInfo.AppendFormat("\tREG_TOTAL_NUMBER_OF_X : [%d], REG_TOTAL_NUMBER_OF_Y : [%d], IC_Max_X_Line_Num : [%d], IC_Max_Y_Line_Num : [%d]\n",
		pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X], pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y],
		pUsbHid->m_IC_Max_X_Line_Num, pUsbHid->m_IC_Max_Y_Line_Num);
#endif
#endif

	return TRUE;
}

BOOL CTestModeView::Check_Ic_Signature(CUsbHid * pUsbHid)
{
	if (pUsbHid->m_CapInfo.icSignature == 0 || pUsbHid->m_CapInfo.icSignature == 0xFFFF)
	{
		HV_TRACE("## ERROR ## %s() ## Get signatrure failed 0x%x\n", __FUNCTION__, pUsbHid->m_CapInfo.icSignature);
		return FALSE;
	}

	if (pUsbHid->m_CapInfo.icSignature != gCapInfo.icSignature)
	{
		HV_TRACE("## ERROR ## %s() ## UNSUPPORT IC SIGNATURE 0x%x IS NOT 0x%x\n", __FUNCTION__, pUsbHid->m_CapInfo.icSignature, gCapInfo.icSignature);
		return FALSE;
	}

	return TRUE;
}

// INT pin 이 High 를 유지하는지 확인
BOOL CTestModeView::Check_Interrupt_Pin(CUsbHid * pUsbHid)
{
	int nCount = 0;
	int nRet, i;
	stCoordinfo coordInfo;

	//pUsbHid->SetTouchConfigEx(REG_ESD_INT_INTERVAL, 0);
	//MyDelayMs(1);

	for(i = 0; i < 6; i++)									// 10 -> 6
	{
		pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
		MyDelayMs(10);										// 1 -> 10

		nRet = pUsbHid->ReadTouchGetPoint((BYTE*)&coordInfo, sizeof(stCoordinfo));

		if (nRet == 0) {				// 실제 INT 발생 시
			nCount++;
		}
		else if(nRet == 2) {			// HID 통신 에러 발생 시
			return FALSE;
		}
		else{							// INT Pin 이 High 일 경우
			nCount = 0;
		}
		//MyDelayMs(10);

		if(nCount > 3)
			return FALSE;
	}

	return TRUE;
}

BOOL CTestModeView::Check_Reset_Pin(CUsbHid * pUsbHid)
{
	s16 nCurVal, nInitVal;

	if (pUsbHid->GetTouchConfigEx(REG_CHECK_RESET_PIN, (BYTE *)&nInitVal) != 0)
		return FALSE;

	// bit NOT operation
	nCurVal = ~nInitVal;

	if (pUsbHid->SetTouchConfigEx(REG_CHECK_RESET_PIN, nCurVal) != 0)
		return FALSE;

	// reset
	pUsbHid->ResetPIN_Control();
	MyDelayMs(100);

	// power sequence
	if (pUsbHid->PowerSequenceEx() == FALSE)
	{
		Log_Append(pUsbHid, RESULT_FAIL);

		if (pUsbHid->m_nChecksum == CHECKSUM_PATTERN_PASS)
			DisplayResultFailToList(pUsbHid, "FAIL : CHECK RESET PIN - POWER SEQUENCE");
		else
			DisplayResultFailToList(pUsbHid, "FAIL : CHECK RESET PIN - POWER SEQUENCE - CHECKSUM ERROR");

		return FALSE;
	}

	if (pUsbHid->GetTouchConfigEx(REG_CHECK_RESET_PIN, (BYTE *)&nInitVal) != 0)
		return FALSE;

	if(nInitVal == nCurVal)
		return FALSE;

	return TRUE;
}

//==============================================================================================
// Read/Check Register and raw data
//==============================================================================================

BOOL CTestModeView::ChangeTouchMode(CUsbHid * pUsbHid, int nMode)
{
	MyDelayMs(20);

	if (pUsbHid->IsCoreFrameWork() == FALSE)
	{ // CHINA
		if (pUsbHid->SetTouchConfigEx(REG_TOUCH_MODE, TOUCH_POINT_MODE) != 0)
			return FALSE;

		MyDelayMs(20);
		if (pUsbHid->SetTouchCommandEx(CMD_SW_RESET) != 0)
			return FALSE;

		if (pUsbHid->SetTouchConfigEx(REG_TOUCH_MODE, nMode) != 0)
			return FALSE;
	}
	else
	{ // IFE
		if( nMode != TOUCH_POINT_MODE )
		{
			if(pUsbHid->SetTouchConfigEx(REG_TOUCH_MODE, TOUCH_POINT_MODE) != 0)
				return FALSE;
		}

		if (pUsbHid->SetTouchConfigEx(REG_TOUCH_MODE, nMode) != 0)
			return FALSE;
	}

	return TRUE;
}

BOOL CTestModeView::ReadGarbageData(CUsbHid * pUsbHid, short int *sRawData)
{
	int ret, nRetry;
	if (pUsbHid->IsCoreFrameWork() == FALSE)
	{ // CHINA
		nRetry = 0;
		for(int i = 0; i < 3; )		// 5 -> 3
		{
			ret = pUsbHid->ReadTouchRawData((BYTE*)&sRawData[0], 4);

			if(ret == 0)
			{
				pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
				i++;
			}
			else if(nRetry++ > RETRY_COUNT || ret == 2)
			{
				//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
				//UpdateStatusBar(&err_msg);
				return FALSE;
			}
		}
	}
	else
	{ // IFE
		pUsbHid->m_InterruptTimeOut = FALSE;

		// Read Garbage Data
		// 0x0200번지에서 4byte read 후 return 값에 문제 없으면, 0x0003 command 보냄. 문제있으면 fail처리.
		nRetry = 0;
		ret = pUsbHid->ReadTouchRawData((BYTE*)&sRawData[0], 4);

		if (ret == 0)
		{
			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
		}
		else if (nRetry++ > RETRY_COUNT || ret == 2)
		{
			//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
			//UpdateStatusBar(&err_msg);
			return FALSE;
		}

		// Interrupt low check block
		int nRet, nCount = 0;
		stCoordinfo temp;
		UINT64 dt = GetTimeInMillis();

		while( 1 )
		{
			nRet = pUsbHid->ReadTouchGetPoint((BYTE*)&temp, sizeof(stCoordinfo));

			if (nRet == 2)		break;
			else if (nRet == 0) nCount++;
			else				nCount = 0;

			if(nCount > 2)		break;

			if (GetTimeInMillis() - dt > 5000)
			{
				pUsbHid->m_InterruptTimeOut = TRUE;

				//err_msg.Format("[ERROR] FAILED TO READ RAW DATA. - Interrupt timeout");
				//UpdateStatusBar(&err_msg);
				AfxMessageBox("Please Check FW or Test List");
				return FALSE;
			}
		}
	}

	return TRUE;
}

int CTestModeView::GetTouchMode(CUsbHid * pUsbHid)
{
	int nTouchMode = TOUCH_POINT_MODE;

	if (pUsbHid->IsCoreFrameWork() == TRUE)
	{ // Core
		switch (pUsbHid->m_nTestStep)
		{
		case TEST_DND:							nTouchMode = TOUCH_MODE_DND;					break;
		case TEST_CND:							nTouchMode = TOUCH_MODE_CND;					break;
		case TEST_SELF_DND:						nTouchMode = TOUCH_MODE_SELF_DND;				break;
		case TEST_SELF_SATURATION:				nTouchMode = TOUCH_MODE_SELF_SATURATION;		break;
		case TEST_MEASURE_DELTA:				nTouchMode = TOUCH_MODE_HYBRID_BASELINED_DATA;	break;
		case TEST_HW_CALIBRATION:				nTouchMode = TOUCH_MODE_HW_CALIBRATION;			break;
		case TEST_JITTER:						nTouchMode = TOUCH_MODE_JITTER;					break;
		case TEST_DIGITAL_WHEEL:				nTouchMode = TOUCH_DIGITAL_WHEEL_DATA_MODE;		break;
		case TEST_ONLY_SELF_DND:				nTouchMode = TOUCH_MODE_ONLY_SELF_DND;			break;
		case TEST_HIGH_VOLTAGE:					nTouchMode = TOUCH_MODE_HIGH_VOLTAGE;			break;
		case TEST_HF_DND:
			if(pUsbHid->IsChinaCustomer())		nTouchMode = TOUCH_MODE_DND;
			else								nTouchMode = TOUCH_MODE_DND_HF;
		break;
		case TEST_TRX_SHORT:
			if(pUsbHid->IsChinaCustomer())		nTouchMode = TOUCH_RAW_CHECK_SHORT_MODE;
			else								nTouchMode = TOUCH_MODE_TRX_SHORT;
		break;
		case TEST_VIRTUAL_DRAW:					nTouchMode = TOUCH_MODE_VIRTUAL_DRAW;			break;

		case TEST_CURRENT_CONSUMPTION:
		default:
			nTouchMode = TOUCH_POINT_MODE;
		break;
		}
	}
	else
	{ // CHINA
		switch (pUsbHid->m_nTestStep)
		{
		case TEST_DND:
		case TEST_HF_DND:
			nTouchMode = TOUCH_MODE_DND;
		break;
		case TEST_SELF_DND:
			if (pUsbHid->m_nTestSubStep == 0)
				nTouchMode = TOUCH_PP_RX_SHORT_N_DATA_MODE;
			else
				nTouchMode = TOUCH_PP_TX_SHORT_N_DATA_MODE;
		break;
		case TEST_TRX_SHORT:
			nTouchMode = TOUCH_RAW_CHECK_SHORT_MODE;
		break;
		case TEST_MEASURE_DELTA:
			nTouchMode = TOUCH_MODE_HYBRID_BASELINED_DATA;
		break;
		case TEST_JITTER:
			nTouchMode = TOUCH_CAL_N_DATA_MODE;
		break;
		/*
		case TEST_SELF_SATURATION:		nTouchMode = TOUCH_MODE_SELF_SATURATION;		break;
		case TEST_HW_CALIBRATION:		nTouchMode = TOUCH_MODE_HW_CALIBRATION;			break;
		case TEST_DIGITAL_WHEEL:		nTouchMode = TOUCH_DIGITAL_WHEEL_DATA_MODE;		break;
		*/
		case TEST_CURRENT_CONSUMPTION:
			//nTouchMode = TOUCH_POINT_MODE;
		break;
		case TEST_VIRTUAL_DRAW:
			nTouchMode = TOUCH_MODE_VIRTUAL_DRAW;
		break;
		}
	}

	return nTouchMode;
}

void CTestModeView::CalculateAfeSetting(CUsbHid * pUsbHid, int nTestStep)
{
	// AFE setting is unnecessary
	if (nTestStep < TEST_DND ||
		nTestStep == TEST_TRX_SHORT ||
		nTestStep == TEST_CURRENT_CONSUMPTION ||
		nTestStep == TEST_VIRTUAL_DRAW)
	{
		return;
	}

	T_AFE_VALUE * pAfeValue = &pUsbHid->m_AfeValue[nTestStep];

	if (IFE_MODE_OFF)
	{ // CHINA
		pAfeValue->reg_Frequency = REG_DEFAULT_FREQUENCY;
		pAfeValue->reg_Ncount = REG_DEFAULT_N_COUNT_CHINA;
		pAfeValue->reg_Ucount = REG_DEFAULT_U_COUNT_CHINA;

		if (nTestStep == TEST_SELF_DND || nTestStep == TEST_SELF_SATURATION)
		{
			T_AFE_SETTING_MULTI	* pAfeInfo = NULL;

			if (nTestStep == TEST_SELF_DND)
				pAfeInfo = &m_TestInfo_SELF_DND.AfeInfo;
			else
				pAfeInfo = &m_TestInfo_SELF_SATURATION.AfeInfo;

			// set user setting value
			if ( (pAfeInfo != NULL) && pAfeInfo->bUseAfeSetting )
			{
				pAfeValue->val_Frequency = pAfeInfo->nFrequency_A;
				pAfeValue->val_Ncount = pAfeInfo->nNCount_A;
				pAfeValue->val_Ucont = pAfeInfo->nUCount_A;
			}
			else
			{
				pAfeValue->val_Frequency = pUsbHid->m_CapInfo.icRegData[pAfeValue->reg_Frequency];
				pAfeValue->val_Ncount = pUsbHid->m_CapInfo.icRegData[pAfeValue->reg_Ncount];
				pAfeValue->val_Ucont = pUsbHid->m_CapInfo.icRegData[pAfeValue->reg_Ucount];
			}
		}
		else
		{
			T_AFE_SETTING * pAfeInfo = NULL;

			switch (nTestStep)
			{
			case TEST_DND:				pAfeInfo = &m_TestInfo_DND.AfeInfo;					break;
			case TEST_HF_DND:			pAfeInfo = &m_TestInfo_HF_DND.AfeInfo;				break;
			case TEST_MEASURE_DELTA:	pAfeInfo = &m_TestInfo_MEASURE_DELTA.AfeInfo;		break;
			case TEST_HW_CALIBRATION:	pAfeInfo = &m_TestInfo_HW_CALIBRATION.AfeInfo;		break;
			case TEST_JITTER:			pAfeInfo = &m_TestInfo_JITTER.AfeInfo;				break;
			}

			// set user setting value
			if ( (pAfeInfo != NULL) && pAfeInfo->bUseAfeSetting )
			{
				pAfeValue->val_Frequency = pAfeInfo->nFrequency;
				pAfeValue->val_Ncount = pAfeInfo->nNCount;
				pAfeValue->val_Ucont = pAfeInfo->nUCount;
			}
			else
			{
				pAfeValue->val_Frequency = pUsbHid->m_CapInfo.icRegData[pAfeValue->reg_Frequency];
				pAfeValue->val_Ncount = pUsbHid->m_CapInfo.icRegData[pAfeValue->reg_Ncount];
				pAfeValue->val_Ucont = pUsbHid->m_CapInfo.icRegData[pAfeValue->reg_Ucount];
			}
		}
	}
	else
	{ // IFE
		if (nTestStep == TEST_SELF_DND || nTestStep == TEST_SELF_SATURATION)
		{
			//-----------------------------------------------
			// Assign register address and value [ MULTI ]

			T_AFE_SETTING_MULTI	* pAfeInfo = NULL;

			if (nTestStep == TEST_SELF_DND)
			{
				pAfeValue->reg_Frequency = REG_SELF_DND_FREQUENCY_A;
				pAfeValue->reg_Ncount = REG_SELF_DND_N_COUNT_A;
				pAfeValue->reg_Ucount = REG_SELF_DND_U_COUNT_A;

				pAfeValue->reg_Frequency_B = REG_SELF_DND_FREQUENCY_B;
				pAfeValue->reg_Ncount_B = REG_SELF_DND_N_COUNT_B;
				pAfeValue->reg_Ucount_B = REG_SELF_DND_U_COUNT_B;

				pAfeInfo = &m_TestInfo_SELF_DND.AfeInfo;
			}
			else
			{
				pAfeValue->reg_Frequency = REG_SELF_SATURATION_FREQUENCY_A;
				pAfeValue->reg_Ncount = REG_SELF_SATURATION_N_COUNT_A;
				pAfeValue->reg_Ucount = REG_SELF_SATURATION_U_COUNT_A;

				pAfeValue->reg_Frequency_B = REG_SELF_SATURATION_FREQUENCY_B;
				pAfeValue->reg_Ncount_B = REG_SELF_SATURATION_N_COUNT_B;
				pAfeValue->reg_Ucount_B = REG_SELF_SATURATION_U_COUNT_B;

				pAfeInfo = &m_TestInfo_SELF_SATURATION.AfeInfo;
			}

			// set user setting value
			if ( (pAfeInfo != NULL) && pAfeInfo->bUseAfeSetting )
			{
				pAfeValue->val_Frequency = pAfeInfo->nFrequency_A;
				pAfeValue->val_Ncount = pAfeInfo->nNCount_A;
				pAfeValue->val_Ucont = pAfeInfo->nUCount_A;

				pAfeValue->val_Frequency_B = pAfeInfo->nFrequency_B;
				pAfeValue->val_Ncount_B = pAfeInfo->nNCount_B;
				pAfeValue->val_Ucont_B = pAfeInfo->nUCount_B;
			}
			else
			{
				pAfeValue->val_Frequency = pUsbHid->m_CapInfo.icRegData[pAfeValue->reg_Frequency];
				pAfeValue->val_Ncount = pUsbHid->m_CapInfo.icRegData[pAfeValue->reg_Ncount];
				pAfeValue->val_Ucont = pUsbHid->m_CapInfo.icRegData[pAfeValue->reg_Ucount];

				pAfeValue->val_Frequency_B = pUsbHid->m_CapInfo.icRegData[pAfeValue->reg_Frequency_B];
				pAfeValue->val_Ncount_B = pUsbHid->m_CapInfo.icRegData[pAfeValue->reg_Ncount_B];
				pAfeValue->val_Ucont_B = pUsbHid->m_CapInfo.icRegData[pAfeValue->reg_Ucount_B];
			}
		}
		else
		{
			//-----------------------------------------------
			// Assign register address and value [ SINGLE ]

			T_AFE_SETTING * pAfeInfo = NULL;

			pAfeValue->reg_Frequency = REG_DEFAULT_FREQUENCY;
			pAfeValue->reg_Ncount = REG_DEFAULT_N_COUNT;
			pAfeValue->reg_Ucount = REG_DEFAULT_U_COUNT;

			switch (nTestStep)
			{
			case TEST_DND:
			{
				pAfeValue->reg_Frequency = REG_DND_FREQUENCY;
				pAfeValue->reg_Ncount = REG_DND_N_COUNT;
				pAfeValue->reg_Ucount = REG_DND_U_COUNT;

				pAfeInfo = &m_TestInfo_DND.AfeInfo;
			}
			break;

			case TEST_CND:
			{	//TEST_CND 에서는 TEST_DND 와는 다르게 reg_Frequency,reg_Ncount,reg_Ucount 부분 변경을 하지 않는다.
				pAfeValue->reg_Frequency = 0x03F5;	//0x03F5
				pAfeValue->reg_Ncount = 0;		//
				pAfeValue->reg_Ucount = 0;

				pAfeInfo = &m_TestInfo_CND.AfeInfo;
			}
			break;

			case TEST_HF_DND:
			{
				pAfeValue->reg_Frequency = REG_HF_DND_FREQUENCY;
				pAfeValue->reg_Ncount = REG_HF_DND_N_COUNT;
				pAfeValue->reg_Ucount = REG_HF_DND_U_COUNT;

				pAfeInfo = &m_TestInfo_HF_DND.AfeInfo;
			}
			break;

			case TEST_MEASURE_DELTA:
			{
				pAfeInfo = &m_TestInfo_MEASURE_DELTA.AfeInfo;
			}
			break;

			case TEST_HW_CALIBRATION:
			{
				pAfeInfo = &m_TestInfo_HW_CALIBRATION.AfeInfo;
			}
			break;

			case TEST_JITTER:
			{
				pAfeInfo = &m_TestInfo_JITTER.AfeInfo;
			}
			break;

			case TEST_NOISE:
			{
			    pAfeInfo = &m_TestInfo_NOISE.AfeInfo;
			}
			break;
			}

			// set user setting value
			if ( (pAfeInfo != NULL) && pAfeInfo->bUseAfeSetting )
			{
				pAfeValue->val_Frequency = pAfeInfo->nFrequency;
				pAfeValue->val_Ncount = pAfeInfo->nNCount;
				pAfeValue->val_Ucont = pAfeInfo->nUCount;
			}
			else
			{
				pAfeValue->val_Frequency = pUsbHid->m_CapInfo.icRegData[pAfeValue->reg_Frequency];
				pAfeValue->val_Ncount = pUsbHid->m_CapInfo.icRegData[pAfeValue->reg_Ncount];
				pAfeValue->val_Ucont = pUsbHid->m_CapInfo.icRegData[pAfeValue->reg_Ucount];
			}
		}
	}
}

BOOL CTestModeView::ChangeAfeSetting(CUsbHid * pUsbHid)
{
	int nTestStep = pUsbHid->m_nTestStep;

	// AFE setting is unnecessary
	if (nTestStep == TEST_TRX_SHORT ||
		nTestStep == TEST_CURRENT_CONSUMPTION ||
		nTestStep == TEST_CND
	)
	{
		return TRUE;
	}

	T_AFE_VALUE * pAfeValue = &pUsbHid->m_AfeValue[nTestStep];

	if (nTestStep == TEST_SELF_DND || nTestStep == TEST_SELF_SATURATION)
	{
		if (pUsbHid->SetTouchConfigEx(pAfeValue->reg_Frequency, pAfeValue->val_Frequency) != 0)			return FALSE;
		if (pUsbHid->SetTouchConfigEx(pAfeValue->reg_Ncount, pAfeValue->val_Ncount) != 0)				return FALSE;
		if (pUsbHid->SetTouchConfigEx(pAfeValue->reg_Ucount, pAfeValue->val_Ucont) != 0)				return FALSE;

		if (pUsbHid->IsCoreFrameWork() == TRUE)
		{ // IFE
			if (pUsbHid->SetTouchConfigEx(pAfeValue->reg_Frequency_B, pAfeValue->val_Frequency_B) != 0)		return FALSE;
			if (pUsbHid->SetTouchConfigEx(pAfeValue->reg_Ncount_B, pAfeValue->val_Ncount_B) != 0)			return FALSE;
			if (pUsbHid->SetTouchConfigEx(pAfeValue->reg_Ucount_B, pAfeValue->val_Ucont_B) != 0)			return FALSE;
		}
	}
	else
	{
		if (pUsbHid->SetTouchConfigEx(pAfeValue->reg_Frequency, pAfeValue->val_Frequency) != 0)		return FALSE;
		if (pUsbHid->SetTouchConfigEx(pAfeValue->reg_Ncount, pAfeValue->val_Ncount) != 0)			return FALSE;
		if (pUsbHid->SetTouchConfigEx(pAfeValue->reg_Ucount, pAfeValue->val_Ucont) != 0)			return FALSE;
	}

	return TRUE;
}


BOOL CTestModeView::PrepareToReadNoise(CUsbHid * pUsbHid, char num)
{
	// Change AFE setting
	if (num == 0){
		if (pUsbHid->SetTouchConfigEx(0x0100, m_TestInfo_NOISE.AfeInfo.nFrequency) != 0)				return FALSE;
		if (pUsbHid->SetTouchConfigEx(0x02F2, m_TestInfo_NOISE.AfeInfo.nUCount) != 0)				return FALSE;
		if (pUsbHid->SetTouchConfigEx(0x02F3, m_TestInfo_NOISE.AfeInfo.nNCount) != 0)				return FALSE;
		MyDelayMs(50);
	}
	else if (num == 1){
		if (pUsbHid->SetTouchConfigEx(0x0100, m_TestInfo_NOISE.AfeInfo.nFrequency1) != 0)			return FALSE;
	}
	else if (num == 2){
		if (pUsbHid->SetTouchConfigEx(0x0100, m_TestInfo_NOISE.AfeInfo.nFrequency2) != 0)			return FALSE;
	}
	else if (num == 3){
		if (pUsbHid->SetTouchConfigEx(0x0100, m_TestInfo_NOISE.AfeInfo.nFrequency3) != 0)			return FALSE;
	}
	else if (num == 4){
		if (pUsbHid->SetTouchConfigEx(0x0100, m_TestInfo_NOISE.AfeInfo.nFrequency4) != 0)			return FALSE;
	}
	MyDelayMs(20);

	pUsbHid->SetTouchCommandEx(CMD_SW_RESET);
	MyDelayMs(10);
	pUsbHid->SetTouchCommandEx(CMD_SW_RESET);
	MyDelayMs(10);
	pUsbHid->SetTouchCommandEx(CMD_SW_RESET);
	MyDelayMs(10);

	return TRUE;
}

BOOL CTestModeView::PrepareToReadRawData(CUsbHid * pUsbHid)
{
	//-----------------------------------------------
	// Power Reset
	if (pUsbHid->m_nTestStep == TEST_CURRENT_CONSUMPTION ||
		//pUsbHid->m_nTestStep == TEST_HW_CALIBRATION||
		pUsbHid->m_nTestStep == TEST_JITTER )
	{
		int nError = MpTool_PowerSequenceAfterReset(pUsbHid);
		if (nError != Z_HID_OK)
		{
			if (nError == Z_ERR_POWER_CONTROL)
				pUsbHid->m_strRet.Format("FAIL : POWER CONTROL");
			else
				pUsbHid->m_strRet.Format("FAIL : POWER SEQUENCE (0x%04X)", pUsbHid->GetLastError());
			return FALSE;
		}
	}

	//-----------------------------------------------
	// Set the additional setting
	switch (pUsbHid->m_nTestStep)
	{
	case TEST_TRX_SHORT:
	{
		if(pUsbHid->IsCoreFrameWork() == TRUE)
		{ // IFE
			if (pUsbHid->SetTouchConfigEx(REG_TM_SHORT_METHOD, 0) != 0)
			{
				//err_msg.Format("[ERROR] FAILED TO SET %04XH = %d.", REG_TM_SHORT_METHOD, 0);
				return FALSE;
			}
		}
	}
	break;

	case TEST_MEASURE_DELTA:
	{
		if(pUsbHid->IsCoreFrameWork() == FALSE)
		{ // CHINA
			for (int reyCount = 0; reyCount < 5; reyCount++)
			{
				if (pUsbHid->SetTouchConfigEx(REG_INTERNAL_FLAG_00, 0) == 0)
				{
					break;
				}
				MyDelayMs(10);
			}
		}
	}

	case TEST_JITTER:
	{
		if(pUsbHid->IsCoreFrameWork() == TRUE)
		{
			if (pUsbHid->SetTouchConfigEx(REG_JITTER_N_DATA_CNT, pUsbHid->m_CapInfo.icRegData[REG_JITTER_N_DATA_CNT]) != 0)
				return FALSE;
		}
	}
	break;
	}

	//-----------------------------------------------
	// Change AFE setting
	if( ChangeAfeSetting(pUsbHid) != TRUE )
		return FALSE;

	//-----------------------------------------------
	// change touch mode
	if( ChangeTouchMode(pUsbHid, GetTouchMode(pUsbHid)) != TRUE )
		return FALSE;

	if(pUsbHid->m_nTestStep == TEST_CURRENT_CONSUMPTION)
	{
		if(pUsbHid->IsCoreFrameWork() == FALSE)
		{ // CHINA
			if (pUsbHid->SetTouchRegBit(REG_INTERNAL_FLAG_00, DEF_USE_POWER_SAVING_MODE, FALSE) != 0)		// remove power saving mode
				return FALSE;
		}
		else
		{ // IFE
			if (pUsbHid->SetTouchConfigEx(REG_POWER_STATE_FLAG, 0x01) != 0)		// Set Active
			{
				pUsbHid->m_strRet.Format("FAIL: POWER ACTIVE FAIL");
				return FALSE;
			}
			MyDelayMs(5);

			if (pUsbHid->SetTouchRegBit(REG_INTERNAL_FLAG_00, DEF_USE_DEBUG_FORCE_NOISE_OFF, TRUE) != 0)
				return FALSE;
		}
		MyDelayMs(100);
	}

	//MyDelayMs(200);

	return TRUE;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// DND

BOOL CTestModeView::ReadRawData(CUsbHid * pUsbHid, short int *sRawData)
{
	// AFE setting, change Touch mode
	if( PrepareToReadRawData(pUsbHid) != TRUE )
		return FALSE;

	// Read Garbage Data
	if( ReadGarbageData(pUsbHid, sRawData) != TRUE )
		return FALSE;

	// Read Raw Data
	int ret, nRetry = 0;
	int nReadDataSize = (pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]) * 2;

	do
	{
		ret = pUsbHid->ReadTouchRawData((BYTE*)&sRawData[0], nReadDataSize);

		if(ret == 0)
		{
			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
			break;
		}
		else if(nRetry++ > RETRY_COUNT || ret == 2)
		{
			//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
			//UpdateStatusBar(&err_msg);
			return FALSE;
		}
	} while(1);

	return TRUE;
}

BOOL CTestModeView::CheckDNDRawDataRange(CUsbHid * pUsbHid, short int *sRawData)
{
	BOOL bRet = TRUE;
	int nDiff_Max, nDiff_Min;
	int i, j;

	for (i = 0; i < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X]; i++)
	{
		for (j = 0; j < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]; j++)
		{
			int nCurDiff = ((int)sRawData[i*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + j]);

			nDiff_Max = m_TestInfo_DND.pDataMax[i][j];
			nDiff_Min = m_TestInfo_DND.pDataMin[i][j];

			if (nCurDiff > nDiff_Max)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, j, nDiff_Max, (int)nCurDiff);
				bRet = FALSE;
			}
			if (nCurDiff < nDiff_Min)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Min, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, j, nDiff_Min, (int)nCurDiff);
				bRet = FALSE;
			}
		}
	}

	return bRet;
}

BOOL CTestModeView::CheckCNDRawDataRange(CUsbHid* pUsbHid, short int* sRawData)
{
	BOOL bRet = TRUE;
	int nDiff_Max, nDiff_Min;
	int i, j;

	for (i = 0; i < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X]; i++)
	{
		for (j = 0; j < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]; j++)
		{
			int nCurDiff = ((int)sRawData[i * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + j]);

			nDiff_Max = m_TestInfo_CND.pDataMax[i][j];
			nDiff_Min = m_TestInfo_CND.pDataMin[i][j];

			if (nCurDiff > nDiff_Max)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, j, nDiff_Max, (int)nCurDiff);
				bRet = FALSE;
			}
			if (nCurDiff < nDiff_Min)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Min, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, j, nDiff_Min, (int)nCurDiff);
				bRet = FALSE;
			}
		}
	}

	return bRet;
}

BOOL CTestModeView::ReadHighVoltage(CUsbHid* pUsbHid, short int* sRawData, int nTestSubStep)
{
	// AFE setting, change Touch mode
	if( PrepareToReadRawData(pUsbHid) != TRUE )
		return FALSE;

	// Read Garbage Data
	if( ReadGarbageData(pUsbHid, sRawData) != TRUE )
		return FALSE;

	// Read Raw Data
	int ret, nRetry = 0;
	int nReadDataSize = 2;

	do
	{
		ret = pUsbHid->ReadTouchRawData((BYTE*)&sRawData[0], nReadDataSize);

		if(ret == 0)
		{
			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
			break;
		}
		else if(nRetry++ > RETRY_COUNT || ret == 2)
		{
			//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
			//UpdateStatusBar(&err_msg);
			return FALSE;
		}
	} while(1);

	return TRUE;
}

BOOL CTestModeView::CheckHighVoltageRange(CUsbHid * pUsbHid, s16 *sRawData)
{
	BOOL bRet = TRUE;
	s16 nDiff_Max, nDiff_Min;
	int i =1 , j = 1;

	s16 nCurDiff = sRawData[0];

	nDiff_Max = m_TestInfo_HIGH_VOLTAGE.pDataMax;
	nDiff_Min = m_TestInfo_HIGH_VOLTAGE.pDataMin;

	if (nCurDiff > nDiff_Max)
	{
		pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
		pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, j, nDiff_Max, (int)nCurDiff);
		bRet = FALSE;
	}
	if (nCurDiff < nDiff_Min)
	{
		pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Min, 5), (int)nCurDiff);
		pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, j, nDiff_Min, (int)nCurDiff);
		bRet = FALSE;
	}


	return bRet;
}

BOOL CTestModeView::CheckRawDataHDiff(CUsbHid * pUsbHid, short int *sRawData, short int *sDiffData)
{
	BOOL bRet = TRUE;
	int nDiff;
	float nCurDiff;
	int x, y;

	for(x = 0; x < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] ; x++)
	{
		for(y = 0; y < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] - 1; y++)
		{
			nDiff = m_TestInfo_DND.pHDiff[x][y];

			s16 A = ((int)sRawData[x*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + y]);
			s16 B = ((int)sRawData[x*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + y + 1]);

			if (A == B)
			{
				nCurDiff = 0;
			}
			else if ((A > B && A == 0) || (A < B && B == 0))
			{
				nCurDiff = S16_PLUS_MAX;
			}

			else
			{
				if (A > B)
					nCurDiff = ((100.0 * B) / (100.0 * A));
				else
					nCurDiff = ((100.0 * A) / (100.0 * B));

				nCurDiff = (100 - (nCurDiff * 100));
				nCurDiff = (int)(nCurDiff + 0.5);		//Round Up
			}

			sDiffData[x*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + y] = (short int)nCurDiff;

			if (nCurDiff < 0)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%02d%02d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", x, y, y+1, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%02d%02d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", x, y, y+1, nDiff, (int)nCurDiff);
				bRet = FALSE;
			}

			if (nCurDiff > nDiff)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%02d%02d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", x, y, y+1, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%02d%02d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", x, y, y+1, nDiff, (int)nCurDiff);
				bRet = FALSE;
			}
		}
	}

	return bRet;
}

BOOL CTestModeView::CheckRawDataVDiff(CUsbHid * pUsbHid, short int *sRawData, short int *sDiffData)
{
	BOOL bRet = TRUE;
	int nDiff;
	float nCurDiff;
	int x, y;

	for (x = 0; x < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] -1; x++)
	{
		for (y = 0; y < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] ; y++)
		{
			nDiff = m_TestInfo_DND.pVDiff[x][y];

			s16 A = ((int)sRawData[x*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + y]);
			s16 B = ((int)sRawData[(x + 1)*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + y]);

			if (A == B)
			{
				nCurDiff = 0;
			}
			else if ((A > B && A == 0) || (A < B && B == 0))
			{
				nCurDiff = S16_PLUS_MAX;
			}
			else
			{
				if (A > B)
					nCurDiff = ((100.0 * B) / (100.0 * A));
				else
					nCurDiff = ((100.0 * A) / (100.0 * B));

				nCurDiff = (100 - (nCurDiff * 100));
				nCurDiff = (int)(nCurDiff + 0.5);		//Round Up
			}

			sDiffData[x*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + y] = (short int)nCurDiff;

			if (nCurDiff < 0)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%02d%02d_Y%04d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", x, x+1, y, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%02d%02d_Y%04d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", x, x+1, y, nDiff, (int)nCurDiff);
				bRet = FALSE;
			}

			if (nCurDiff > nDiff)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%02d%02d_Y%04d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", x, x+1, y, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%02d%02d_Y%04d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", x, x+1, y, nDiff, (int)nCurDiff);
				bRet = FALSE;
			}
		}
	}

	return bRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// HF DND

BOOL CTestModeView::ReadRawData_HfDnd(CUsbHid * pUsbHid, short int *sRawData)
{
	// AFE setting, change Touch mode
	if( PrepareToReadRawData(pUsbHid) != TRUE )
		return FALSE;

	// Read Garbage Data
	if( ReadGarbageData(pUsbHid, sRawData) != TRUE )
		return FALSE;

	// Read Raw Data
	int ret, nRetry = 0;
	int nReadDataSize = (pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]) * 2;

	do
	{
		ret = pUsbHid->ReadTouchRawData((BYTE*)&sRawData[0], nReadDataSize);

		if (ret == 0)
		{
			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
			break;
		}
		else if (nRetry++ > RETRY_COUNT || ret == 2)
		{
			//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
			//UpdateStatusBar(&err_msg);
			return FALSE;
		}
	} while (1);

	return TRUE;
}

BOOL CTestModeView::CheckRawDataRange_HfDnd(CUsbHid * pUsbHid, short int *sRawData)
{
	BOOL bRet = TRUE;
	int nDiff_Max, nDiff_Min;
	int i, j;

	for (i = 0; i < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X]; i++)
	{
		for (j = 0; j < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]; j++)
		{
			int nCurDiff = ((int)sRawData[i*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + j]);

			nDiff_Max = m_TestInfo_HF_DND.pDataMax[i][j];
			nDiff_Min = m_TestInfo_HF_DND.pDataMin[i][j];

			if (nCurDiff > nDiff_Max)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, j, nDiff_Max, (int)nCurDiff);
				bRet = FALSE;
			}
			if (nCurDiff < nDiff_Min)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Min, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, j, nDiff_Min, (int)nCurDiff);
				bRet = FALSE;
			}

		}
	}

	return bRet;
}

BOOL CTestModeView::CheckRawDataHDiff_HfDnd(CUsbHid * pUsbHid, short int *sRawData, short int *sDiffData)
{
	BOOL bRet = TRUE;
	int nDiff;
	float nCurDiff;
	int x, y;

	for(x = 0; x < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] ; x++)
	{
		for(y = 0; y < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] - 1; y++)
		{
			nDiff = m_TestInfo_HF_DND.pHDiff[x][y];

			s16 A = ((int)sRawData[x*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + y]);
			s16 B = ((int)sRawData[x*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + y + 1]);

			if (A == B)
			{
				nCurDiff = 0;
			}
			else if ((A > B && A == 0) || (A < B && B == 0))
			{
				nCurDiff = S16_PLUS_MAX;
			}

			else
			{
				if (A > B)
					nCurDiff = ((100.0 * B) / (100.0 * A));
				else
					nCurDiff = ((100.0 * A) / (100.0 * B));

				nCurDiff = (100 - (nCurDiff * 100));
				nCurDiff = (int)(nCurDiff + 0.5);		//Round Up
			}

			sDiffData[x*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + y] = (short int)nCurDiff;

			if (nCurDiff < 0)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%02d%02d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", x, y, y+1, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%02d%02d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", x, y, y+1, nDiff, (int)nCurDiff);
				bRet = FALSE;
			}

			if (nCurDiff > nDiff)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%02d%02d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", x, y, y+1, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%02d%02d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", x, y, y+1, nDiff, (int)nCurDiff);
				bRet = FALSE;
			}
		}
	}

	return bRet;
}
BOOL CTestModeView::CheckRawDataVDiff_HfDnd(CUsbHid * pUsbHid, short int *sRawData, short int *sDiffData)
{
	BOOL bRet = TRUE;
	int nDiff;
	float nCurDiff;
	int x, y;

	for (x = 0; x < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] -1; x++)
	{
		for (y = 0; y < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] ; y++)
		{
			nDiff =  m_TestInfo_HF_DND.pVDiff[x][y];

			s16 A = ((int)sRawData[x*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + y]);
			s16 B = ((int)sRawData[(x + 1)*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + y]);

			if (A == B)
			{
				nCurDiff = 0;
			}
			else if ((A > B && A == 0) || (A < B && B == 0))
			{
				nCurDiff = S16_PLUS_MAX;
			}
			else
			{
				if (A > B)
					nCurDiff = ((100.0 * B) / (100.0 * A));
				else
					nCurDiff = ((100.0 * A) / (100.0 * B));

				nCurDiff = (100 - (nCurDiff * 100));
				nCurDiff = (int)(nCurDiff + 0.5);		//Round Up
			}

			sDiffData[x*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + y] = (short int)nCurDiff;

			if (nCurDiff < 0)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%02d%02d_Y%04d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", x, x+1, y, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%02d%02d_Y%04d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", x, x+1, y, nDiff, (int)nCurDiff);
				bRet = FALSE;
			}

			if (nCurDiff > nDiff)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%02d%02d_Y%04d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", x, x+1, y, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%02d%02d_Y%04d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", x, x+1, y, nDiff, (int)nCurDiff);
				bRet = FALSE;
			}
		}
	}

	return bRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ONLY_SELF_DND
BOOL CTestModeView::ReadRawData_OnlySelfDnd(CUsbHid* pUsbHid, short int* sRawData, int nTestSubStep)
{
	pUsbHid->m_nTestSubStep = nTestSubStep;

	// AFE setting, change Touch mode
	if (PrepareToReadRawData(pUsbHid) != TRUE)
		return FALSE;

	// Read Garbage Data
	if (ReadGarbageData(pUsbHid, sRawData) == FALSE)
		return FALSE;

	// Read Raw Data
	int ret, nRetry = 0, nReadDataSize;

	//현재 7650M Backgammon 에서는 x : 24 , y : 12 로 되어있다.
	nReadDataSize = (int)(pUsbHid->m_nSelfChannelCntY + pUsbHid->m_nSelfChannelCntX) * 2;

	do
	{
		ret = pUsbHid->ReadTouchRawData((BYTE*)&sRawData[0], nReadDataSize);

		if (ret == 0)
		{
			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
			break;
		}
		else if (nRetry++ > RETRY_COUNT || ret == 2)
		{
			//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
			//UpdateStatusBar(&err_msg);
			return FALSE;
		}
	} while (1);

	return TRUE;
}

BOOL CTestModeView::CheckRawDataRange_OnlySelfDnd(CUsbHid* pUsbHid, short int* sRawData)
{
	BOOL bRet = TRUE;
	int nCurDiff, nDiff_Max, nDiff_Min;
	int i, nRow, nCol;

	short int* pRxRawData = &sRawData[0];
	short int* pTxRawData = &sRawData[pUsbHid->m_nSelfChannelCntY];

	// Check Y - First Line
	for (i = 0; i < pUsbHid->m_nSelfChannelCntY; i++)
	{
		nCurDiff = (int)pRxRawData[i];

		nDiff_Max = m_TestInfo_ONLYSELF_DND.pRxDataMax[0][i];
		nDiff_Min = m_TestInfo_ONLYSELF_DND.pRxDataMin[0][i];

		if (nCurDiff > nDiff_Max)
		{
			pUsbHid->TestErrDesc_AddDesc("[COL_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
			pUsbHid->TestResultDesc_AddDesc("[COL_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, nDiff_Max, (int)nCurDiff);
			bRet = FALSE;
		}
		if (nCurDiff < nDiff_Min)
		{
			pUsbHid->TestErrDesc_AddDesc("[COL_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Min, 5), (int)nCurDiff);
			pUsbHid->TestResultDesc_AddDesc("[COL_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, nDiff_Min, (int)nCurDiff);
			bRet = FALSE;
		}
	}

	// Check X - Second Line ~ ?
	for (i = 0; i < pUsbHid->m_nSelfChannelCntX; i++)
	{
		nCurDiff = (int)pTxRawData[i];

		nCol = i % pUsbHid->m_nSelfChannelCntY;
		nRow = i / pUsbHid->m_nSelfChannelCntY;

		nDiff_Max = m_TestInfo_ONLYSELF_DND.pTxDataMax[nRow][nCol];
		nDiff_Min = m_TestInfo_ONLYSELF_DND.pTxDataMin[nRow][nCol];

		if (nCurDiff > nDiff_Max)
		{
			pUsbHid->TestErrDesc_AddDesc("[ROW_X%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
			pUsbHid->TestResultDesc_AddDesc("[ROW_X%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, nDiff_Max, (int)nCurDiff);
			bRet = FALSE;
		}
		if (nCurDiff < nDiff_Min)
		{
			pUsbHid->TestErrDesc_AddDesc("[ROW_X%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Min, 5), (int)nCurDiff);
			pUsbHid->TestResultDesc_AddDesc("[ROW_X%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, nDiff_Min, (int)nCurDiff);
			bRet = FALSE;
		}
	}

	return bRet;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SELF_DND //ssy 221012

BOOL CTestModeView::ReadRawData_SelfDnd(CUsbHid * pUsbHid, short int *sRawData, int nTestSubStep)
{
	pUsbHid->m_nTestSubStep = nTestSubStep;

	// AFE setting, change Touch mode
	if( PrepareToReadRawData(pUsbHid) != TRUE )
		return FALSE;

	// Read Garbage Data
	if( ReadGarbageData(pUsbHid, sRawData) == FALSE )
		return FALSE;

	// Read Raw Data
	int ret, nRetry = 0, nReadDataSize;

	if (pUsbHid->IsCoreFrameWork() == FALSE)
	{ // CHINA
		nReadDataSize = (int)(pUsbHid->m_nSelfChannelCntX * pUsbHid->m_nSelfChannelCntY) * 2;
	}
	else
	{ // IFE
		nReadDataSize = (int)(pUsbHid->m_nSelfChannelCntX +
							+ pUsbHid->m_CapInfo.icRegData[REG_BUTTON_SUPPORTED_NUM] * (pUsbHid->m_CapInfo.icRegData[REG_BUTTON_LINE_TYPE] == 1 ? 1: 0)
							+ pUsbHid->m_nSelfChannelCntY) * 2;

		// clear buffer - consider dummy
		memset(sRawData, 0x00, (pUsbHid->m_nSelfChannelCntY + pUsbHid->m_nSelfRowCount * pUsbHid->m_nSelfChannelCntY) * 2);
	}

	do
	{
		ret = pUsbHid->ReadTouchRawData((BYTE*)&sRawData[0], nReadDataSize);

		if (ret == 0)
		{
			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
			break;
		}
		else if (nRetry++ > RETRY_COUNT || ret == 2)
		{
			//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
			//UpdateStatusBar(&err_msg);
			return FALSE;
		}
	} while (1);

	return TRUE;
}

BOOL CTestModeView::CheckRawDataRange_SelfDnd(CUsbHid* pUsbHid, short int* sRxRawData, short int* sTxRawData)
{
	BOOL bRet = TRUE;

	int nCurDiff, nDiff_Max, nDiff_Min;
	int i, nRow, nCol;

	// Check Y (IFE:First Line, CHINA:sRawData)
	if (pUsbHid->IsCoreFrameWork() == FALSE)
	{
		for (i = 0; i < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]; i++)
		{
			nCurDiff = (int)sRxRawData[i];
			nDiff_Max = m_TestInfo_SELF_DND.pRxDataMax[0][i];
			nDiff_Min = m_TestInfo_SELF_DND.pRxDataMin[0][i];

			if (nCurDiff > nDiff_Max)
			{
				pUsbHid->TestErrDesc_AddDesc("[COL_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[COL_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, nDiff_Max, (int)nCurDiff);
				bRet = FALSE;
			}
			if (nCurDiff < nDiff_Min)
			{
				pUsbHid->TestErrDesc_AddDesc("[COL_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Min, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[COL_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, nDiff_Min, (int)nCurDiff);
				bRet = FALSE;
			}
		}
	}
	else
	{
		for (i = 0; i < pUsbHid->m_nSelfChannelCntY; i++)
		{
			nCurDiff = (int)sRxRawData[i];
			nDiff_Max = m_TestInfo_SELF_DND.pRxDataMax[0][i];
			nDiff_Min = m_TestInfo_SELF_DND.pRxDataMin[0][i];

			if (nCurDiff > nDiff_Max)
			{
				pUsbHid->TestErrDesc_AddDesc("[COL_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[COL_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, nDiff_Max, (int)nCurDiff);
				bRet = FALSE;
			}
			if (nCurDiff < nDiff_Min)
			{
				pUsbHid->TestErrDesc_AddDesc("[COL_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Min, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[COL_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, nDiff_Min, (int)nCurDiff);
				bRet = FALSE;
			}
		}
	}

	if (pUsbHid->IsCoreFrameWork() == FALSE)
	{
		// Check X (IFE:Second Line ~ ?, CHINA:sDataData2)
		for (i = 0; i < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X]; i++)
		{
			nCurDiff = (int)sTxRawData[i];

			nCol = i % pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y];
			nRow = i / pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y];

			nDiff_Max = m_TestInfo_SELF_DND.pTxDataMax[nRow][nCol];
			nDiff_Min = m_TestInfo_SELF_DND.pTxDataMin[nRow][nCol];

			if (nCurDiff > nDiff_Max)
			{
				pUsbHid->TestErrDesc_AddDesc("[ROW_X%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[ROW_X%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, nDiff_Max, (int)nCurDiff);
				bRet = FALSE;
			}
			if (nCurDiff < nDiff_Min)
			{
				pUsbHid->TestErrDesc_AddDesc("[ROW_X%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Min, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[ROW_X%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, nDiff_Min, (int)nCurDiff);
				bRet = FALSE;
			}
		}
	}
	else
	{
		// Check X (IFE:Second Line ~ ?, CHINA:sDataData2)
		for (i = 0; i < pUsbHid->m_nSelfChannelCntX; i++)
		{
			nCurDiff = (int)sTxRawData[i];

			nCol = i % pUsbHid->m_nSelfChannelCntY;
			nRow = i / pUsbHid->m_nSelfChannelCntY;

			if (nCurDiff > nDiff_Max)
			{
				pUsbHid->TestErrDesc_AddDesc("[ROW_X%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[ROW_X%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, nDiff_Max, (int)nCurDiff);
				bRet = FALSE;
			}
			if (nCurDiff < nDiff_Min)
			{
				pUsbHid->TestErrDesc_AddDesc("[ROW_X%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Min, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[ROW_X%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, nDiff_Min, (int)nCurDiff);
				bRet = FALSE;
			}
		}
	}
	return bRet;
}


BOOL CTestModeView::CheckRawDataHDiff_SelfDnd(CUsbHid * pUsbHid, short int *sRxRawData, short int *sTxRawData, short int *sDiffData)
{
	BOOL bRet = TRUE;
	int i, nRow, nCol;
	int A, B, nDiff;
	float nCurDiff;

	// Check Y - First Line
	for(i = 0; i < pUsbHid->m_nSelfChannelCntY - 1; i++)
	{
		nDiff = m_TestInfo_SELF_DND.pRxHDiff[0][i];

		A = ((int)sRxRawData[i]);
		B = ((int)sRxRawData[i + 1]);

		if (A == B)
		{
			nCurDiff = 0;
		}
		else
		{
			if (A > B)
					nCurDiff = ((100.0 * B) / (100.0 * A));
				else
					nCurDiff = ((100.0 * A) / (100.0 * B));

			nCurDiff = (100 - (nCurDiff * 100));
			nCurDiff = (int)(nCurDiff + 0.5);		//Round Up
		}

		sDiffData[i] = (short int)nCurDiff;

		if (nCurDiff > nDiff)
		{
			pUsbHid->TestErrDesc_AddDesc("[Y%02d%02d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, i+1, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff, 5), (int)nCurDiff);
			pUsbHid->TestResultDesc_AddDesc("[Y%02d%02d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, i+1, nDiff, (int)nCurDiff);
			bRet = FALSE;
		}
	}

	// Check X - Second Line ~ ?
	for(i = 0; i < pUsbHid->m_nSelfChannelCntX - 1; i++)
	{
		nCol = i % pUsbHid->m_nSelfChannelCntY;
		nRow = i / pUsbHid->m_nSelfChannelCntY;

		nDiff = m_TestInfo_SELF_DND.pTxHDiff[nRow][nCol];

		A = ((int)sTxRawData[nRow*pUsbHid->m_nSelfChannelCntY + nCol]);
		B = ((int)sTxRawData[nRow*pUsbHid->m_nSelfChannelCntY + nCol + 1]);

		if (A == B)
		{
			nCurDiff = 0;
		}
		else
		{
			if (A > B)
				nCurDiff = ((100.0 * B) / (100.0 * A));
			else
				nCurDiff = ((100.0 * A) / (100.0 * B));

			nCurDiff = (100 - (nCurDiff * 100));
			nCurDiff = (int)(nCurDiff + 0.5);		//Round Up
		}

		sDiffData[pUsbHid->m_nSelfChannelCntY + i] = (short int)nCurDiff;

		if (nCurDiff > nDiff)
		{
			pUsbHid->TestErrDesc_AddDesc("[X%02d_Y%02d%02d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", nRow, nCol, nCol+1, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff, 5), (int)nCurDiff);
			pUsbHid->TestResultDesc_AddDesc("[X%02d_Y%02d%02d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", nRow, nCol, nCol+1, nDiff, (int)nCurDiff);
			bRet = FALSE;
		}
	}

	return bRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SELF_SATURATION

BOOL CTestModeView::ReadRawData_SelfSaturation(CUsbHid * pUsbHid, short int *sRawData)
{
	// AFE setting, change Touch mode
	if( PrepareToReadRawData(pUsbHid) != TRUE )
		return FALSE;

	// Read Garbage Data
	if( ReadGarbageData(pUsbHid, sRawData) == FALSE )
		return FALSE;

	// Read Raw Data
	int ret, nRetry = 0, nReadDataSize;

	if (pUsbHid->IsCoreFrameWork() == FALSE)
	{ // CHINA
		nReadDataSize = (int)(pUsbHid->m_nSelfChannelCntX * pUsbHid->m_nSelfChannelCntY) * 2;
	}
	else
	{ // IFE
		nReadDataSize = (int)(pUsbHid->m_nSelfChannelCntX +
							+ pUsbHid->m_CapInfo.icRegData[REG_BUTTON_SUPPORTED_NUM] * (pUsbHid->m_CapInfo.icRegData[REG_BUTTON_LINE_TYPE] == 1 ? 1: 0)
							+ pUsbHid->m_nSelfChannelCntY) * 2;

		// clear buffer - consider dummy
		memset(sRawData, 0x00, (pUsbHid->m_nSelfChannelCntY + pUsbHid->m_nSelfRowCount * pUsbHid->m_nSelfChannelCntY) * 2);
	}

	do
	{
		ret = pUsbHid->ReadTouchRawData((BYTE*)&sRawData[0], nReadDataSize);

		if (ret == 0)
		{
			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
			break;
		}
		else if (nRetry++ > RETRY_COUNT || ret == 2)
		{
			//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
			//UpdateStatusBar(&err_msg);
			return FALSE;
		}
	} while (1);

	return TRUE;
}

BOOL CTestModeView::CheckRawDataRange_SelfSaturation(CUsbHid * pUsbHid, short int *sRawData)
{
	BOOL bRet = TRUE;
	int nCurDiff, nDiff_Max, nDiff_Min;
	int i, nRow, nCol;

	short int * pRxRawData = &sRawData[0];
	short int * pTxRawData = &sRawData[pUsbHid->m_nSelfChannelCntY];

	// Check Y - First Line
	for(i = 0; i < pUsbHid->m_nSelfChannelCntY; i++)
	{
		nCurDiff = (int)pRxRawData[i];

		nDiff_Max = m_TestInfo_SELF_SATURATION.pRxDataMax[0][i];
		nDiff_Min = m_TestInfo_SELF_SATURATION.pRxDataMin[0][i];

		if (nCurDiff > nDiff_Max)
		{
			pUsbHid->TestErrDesc_AddDesc("[COL_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
			pUsbHid->TestResultDesc_AddDesc("[COL_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, nDiff_Max, (int)nCurDiff);
			bRet = FALSE;
		}
		if (nCurDiff < nDiff_Min)
		{
			pUsbHid->TestErrDesc_AddDesc("[COL_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Min, 5), (int)nCurDiff);
			pUsbHid->TestResultDesc_AddDesc("[COL_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, nDiff_Min, (int)nCurDiff);
			bRet = FALSE;
		}
	}

	// Check X - Second Line ~ ?
	for(i = 0; i < pUsbHid->m_nSelfChannelCntX; i++)
	{
		nCurDiff = (int)pTxRawData[i];

		nCol = i % pUsbHid->m_nSelfChannelCntY;
		nRow = i / pUsbHid->m_nSelfChannelCntY;

		nDiff_Max = m_TestInfo_SELF_SATURATION.pTxDataMax[nRow][nCol];
		nDiff_Min = m_TestInfo_SELF_SATURATION.pTxDataMin[nRow][nCol];

		if (nCurDiff > nDiff_Max)
		{
			pUsbHid->TestErrDesc_AddDesc("[ROW_X%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
			pUsbHid->TestResultDesc_AddDesc("[ROW_X%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, nDiff_Max, (int)nCurDiff);
			bRet = FALSE;
		}
		if (nCurDiff < nDiff_Min)
		{
			pUsbHid->TestErrDesc_AddDesc("[ROW_X%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Min, 5), (int)nCurDiff);
			pUsbHid->TestResultDesc_AddDesc("[ROW_X%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, nDiff_Min, (int)nCurDiff);
			bRet = FALSE;
		}
	}

	return bRet;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// TRX SHORT

BOOL CTestModeView::ReadRawData_TrxShort(CUsbHid * pUsbHid, short int *sRawData)
{
	// AFE setting, change Touch mode
	if( PrepareToReadRawData(pUsbHid) != TRUE )
		return FALSE;

	// Read Garbage Data
	if( ReadGarbageData(pUsbHid, sRawData) == FALSE )
		return FALSE;

	// Read Raw Data
	int ret, nRetry = 0;
	int nReadDataSize = (int)(pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]) * 2;

	do
	{
		ret = pUsbHid->ReadTouchRawData((BYTE*)&sRawData[0], nReadDataSize);

		if(ret == 0)
		{
			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
			break;
		}
		else if(nRetry++ > RETRY_COUNT || ret == 2)
		{
			//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
			//UpdateStatusBar(&err_msg);
			return FALSE;
		}
	} while(1);

	return TRUE;
}

BOOL CTestModeView::CheckRawDataRange_TrxShort(CUsbHid * pUsbHid, short int *sRawData)
{
	BOOL bRet = TRUE;
	int i, nCurDiff;
	int nCol, nRow;

	short int * pRxRawData = &sRawData[0];
	short int * pTxRawData = &sRawData[pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]];

	// Check Y - First Line
	for (i = 0; i < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]; i++)
	{
		nCurDiff = (int)pRxRawData[i];

		if(IFE_MODE_OFF)
		{ // CHINA
			if (nCurDiff < TRX_SHORT_REFERENCE)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", 0, i, hvItoaWithSpace(pUsbHid->m_nDevIndex, TRX_SHORT_REFERENCE, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", 0, i, TRX_SHORT_REFERENCE, (int)nCurDiff);
				bRet = FALSE;
			}
		}
		else
		{ // IFE
			if (nCurDiff > TRX_SHORT_REFERENCE)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", 0, i, hvItoaWithSpace(pUsbHid->m_nDevIndex, TRX_SHORT_REFERENCE, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", 0, i, TRX_SHORT_REFERENCE, (int)nCurDiff);
				bRet = FALSE;
			}
		}
	}

	// Check X - Second Line ~ ?
	for (i = 0; i < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X]; i++)
	{
		nCurDiff = (int)pTxRawData[i];

		nCol = i % pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y];
		nRow = (i / pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]) + 1;

		if(IFE_MODE_OFF)
		{ // CHINA
			if (nCurDiff < TRX_SHORT_REFERENCE)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", nRow, nCol, hvItoaWithSpace(pUsbHid->m_nDevIndex, TRX_SHORT_REFERENCE, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", nRow, nCol, TRX_SHORT_REFERENCE, (int)nCurDiff);
				bRet = FALSE;
			}
		}
		else
		{ // IFE
			if (nCurDiff > TRX_SHORT_REFERENCE)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", nRow, nCol, hvItoaWithSpace(pUsbHid->m_nDevIndex, TRX_SHORT_REFERENCE, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", nRow, nCol, TRX_SHORT_REFERENCE, (int)nCurDiff);
				bRet = FALSE;
			}
		}
	}

	return bRet;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// MEASURE DELTA
BOOL CTestModeView::ReadRawData_MeasureDelta(CUsbHid * pUsbHid, short int *sRawData)
{
	int ret, nRetry;
	int nReadLen;

	// Read Raw Data - TX(X), RX(Y)
	// TX CH * RX CH * 2(Mutual Data) + RX CH * 2(Null) + RX CH * 2(Y Self) + TX CH * 2(X Self) + Self Y dummy + Self X dummy
	nReadLen = (int)(pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]) * 2;	// Mutual Data
	nReadLen += (pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] * 2);	// Null
	nReadLen += (pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] * 2);	// Self Y
	nReadLen += (pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * 2);	// Self X

	if (pUsbHid->IsCoreFrameWork() == TRUE)
	{// IFE
		nReadLen += (pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] * 2);	// Self Y dummy
		nReadLen += (pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * 2);	// Self X dummy
	}

	nRetry = 0;
	do
	{
		ret = pUsbHid->ReadTouchRawData((BYTE*)&sRawData[0], nReadLen);

		if (ret == 0)
		{
			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
			break;
		}
		else if (nRetry++ > RETRY_COUNT || ret == 2)
		{
			//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
			//UpdateStatusBar(&err_msg);
			return FALSE;
		}
	} while (1);

	return TRUE;
}

BOOL CTestModeView::CheckRawDataRange_MeasureDelta_Condition_AutoStart(CUsbHid * pUsbHid, short int *sRawData_Average)
{
	BOOL bRet = FALSE;
	int i, j;

	int nSuccessCount = 0;

	for (i = 0; i < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X]; i++)
	{
		for (j = 0; j < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]; j++)
		{
			int nCurDiff = ((int)sRawData_Average[i*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + j]);

			if (nCurDiff > m_TestInfo_MEASURE_DELTA.nAutoStartThreshold)
			{
				nSuccessCount++;
			}
		}
	}

	// 전체 node 수의 50% 이상 성공 시 Auto Start
	int nTotalCount = (int)(pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]);
	float fAverage = (100) * nSuccessCount / nTotalCount;

	if (fAverage > 50)
		bRet = TRUE;

	return bRet;
}

BOOL CTestModeView::CheckRawDataRange_MeasureDelta_Condition_1(CUsbHid * pUsbHid, short int *sRawData_Average)
{
	BOOL bRet = TRUE;
	int nDiff_Max, nDiff_Min;
	int i, j;

	for (i = 0; i < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X]; i++)
	{
		for (j = 0; j < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]; j++)
		{
			int nCurDiff = ((int)sRawData_Average[i*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + j]);

			nDiff_Max = m_TestInfo_MEASURE_DELTA.pDataMax[i][j];
			nDiff_Min = m_TestInfo_MEASURE_DELTA.pDataMin[i][j];

			if (nCurDiff > nDiff_Max)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, j, nDiff_Max, (int)nCurDiff);
				bRet = FALSE;
			}
			if (nCurDiff < nDiff_Min)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Min, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, j, nDiff_Min, (int)nCurDiff);
				bRet = FALSE;
			}
		}
	}

	return bRet;
}

BOOL CTestModeView::CheckRawDataRange_MeasureDelta_Condition_2(CUsbHid * pUsbHid, short int *sRawData_SY)
{
	BOOL bRet = TRUE;
	int nDiff_Max, nDiff_Min;
	int j;

	for (j = 0; j < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]; j++)
	{
		int nCurDiff = ((int)sRawData_SY[j]);

		nDiff_Max = m_TestInfo_MEASURE_DELTA.pSelfRxDataMax[j];
		nDiff_Min = m_TestInfo_MEASURE_DELTA.pSelfRxDataMin[j];

		if (nCurDiff > nDiff_Max)
		{
			pUsbHid->TestErrDesc_AddDesc("[SY%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
			pUsbHid->TestResultDesc_AddDesc("[SY%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", j, nDiff_Max, (int)nCurDiff);
			bRet = FALSE;
		}
		if (nCurDiff < nDiff_Min)
		{
			pUsbHid->TestErrDesc_AddDesc("[SY%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Min, 5), (int)nCurDiff);
			pUsbHid->TestResultDesc_AddDesc("[SY%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", j, nDiff_Min, (int)nCurDiff);
			bRet = FALSE;
		}
	}

	return bRet;
}

BOOL CTestModeView::CheckRawDataRange_MeasureDelta_Condition_3(CUsbHid * pUsbHid, short int *sRawData_SX)
{
	BOOL bRet = TRUE;
	int nDiff_Max, nDiff_Min;
	int j;

	for (j = 0; j < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]; j++)
	{
		int nCurDiff = ((int)sRawData_SX[j]);

		nDiff_Max = m_TestInfo_MEASURE_DELTA.pSelfTxDataMax[j];
		nDiff_Min = m_TestInfo_MEASURE_DELTA.pSelfTxDataMin[j];

		if (nCurDiff > nDiff_Max)
		{
			pUsbHid->TestErrDesc_AddDesc("[SX%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
			pUsbHid->TestResultDesc_AddDesc("[SX%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", j, nDiff_Max, (int)nCurDiff);
			bRet = FALSE;
		}
		if (nCurDiff < nDiff_Min)
		{
			pUsbHid->TestErrDesc_AddDesc("[SX%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Min, 5), (int)nCurDiff);
			pUsbHid->TestResultDesc_AddDesc("[SX%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", j, nDiff_Min, (int)nCurDiff);
			bRet = FALSE;
		}
	}

	return bRet;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Current Consumption

int CTestModeView::TSM_CurrentConsumption(CUsbHid * pUsbHid)
{
	int nRet = Z_TEST_OK;
	int curCurrent;
	short int rawCurrent;
	CString strTestStep;

	TEST_CURRENT_CONSUMPTION_ITEM * pCurrentItem = NULL;

	pUsbHid->m_strRet.Format("");

	memset(&pUsbHid->m_curCurrent[0], -1, sizeof(int)*4);

	// AFE setting, change Touch mode
	if( PrepareToReadRawData(pUsbHid) != TRUE )
		goto error_current_consumption_setting;

	for (int nIndex=0; nIndex<CURRENT_CONSUMPTION_MAX; nIndex++)
	{
		BOOL bSetCommandFail = TRUE;

		//-----------------------------------------------
		// set test condition

		if (nIndex == CURRENT_CONSUMPTION_IDLE)
		{
			MyDelayMs(50);
			pCurrentItem = &m_TestInfo_CURRENT_CONSUMPTION.Idle;

			if (pCurrentItem->bCheckData == FALSE)
				continue;

			for (int reyCount = 0; reyCount < 5; reyCount++)
			{
				if(pUsbHid->IsCoreFrameWork() == TRUE)
				{// ISCoreFrameWork 일 경우 작동 => Core F.W 사용
					if (pUsbHid->SetTouchCommandEx(CMD_GOTO_SLEEP) == 0)
					{
						bSetCommandFail = FALSE;
						break;
					}
				}
				else
				{// 그렇치 않고 Core F.W 이 아니면
					if (pUsbHid->SetTouchCommandEx(CMD_GOTO_IDLE_CHINA) == 0)
					{
						bSetCommandFail = FALSE;
						break;
					}
				}
				MyDelayMs(1);
			}

			if (bSetCommandFail) {
				pUsbHid->m_strRet.Format("[ERROR] FAILED TO SET IDLE COMMAND.");
				goto error_current_consumption_setting;
			}
			MyDelayMs(200);

			strTestStep.Format("IDLE");
		}
		else if (nIndex == CURRENT_CONSUMPTION_SLEEP )
		{
			MyDelayMs(50);
			pCurrentItem = &m_TestInfo_CURRENT_CONSUMPTION.Sleep;

			if (pCurrentItem->bCheckData == FALSE)
				continue;

			for (int reyCount = 0; reyCount < 5; reyCount++)
			{
				if(pUsbHid->IsCoreFrameWork() == TRUE)
				{// IFE ON 이고, China 과제가 아닐 경우 CMD_GOTO_DEEP_SLEEP 이용
					if (pUsbHid->SetTouchCommandEx(CMD_GOTO_DEEP_SLEEP) == 0)
					{
						bSetCommandFail = FALSE;
						break;
					}
				}
				else
				{// 그렇치 않으면 CMD_GOTO_SLEEP 이용
					if (pUsbHid->SetTouchCommandEx(CMD_GOTO_SLEEP) == 0)
					{
						bSetCommandFail = FALSE;
						break;
					}
				}
				MyDelayMs(1);
			}

			if (bSetCommandFail) {
				pUsbHid->m_strRet.Format("[ERROR] FAILED TO SET SLEEP COMMAND.");
				goto error_current_consumption_setting;
			}
			MyDelayMs(200);

			strTestStep.Format("SLEEP");
		}
		else if (nIndex == CURRENT_CONSUMPTION_DEEP_SLEEP )
		{
			MyDelayMs(50);
			pCurrentItem = &m_TestInfo_CURRENT_CONSUMPTION.DeepSleep;

			if (pCurrentItem->bCheckData == FALSE)
				continue;

			if(pUsbHid->IsCoreFrameWork() == TRUE)
				continue;

			for (int reyCount = 0; reyCount < 5; reyCount++)
			{
				if (pUsbHid->SetTouchCommandEx(CMD_GOTO_DEEP_SLEEP_CHINA) == 0)
				{
					bSetCommandFail = FALSE;
					break;
				}
				MyDelayMs(1);
			}

			if (bSetCommandFail) {
				pUsbHid->m_strRet.Format("[ERROR] FAILED TO SET DEEP SLEEP COMMAND.");
				goto error_current_consumption_setting;
			}
			MyDelayMs(200);

			strTestStep.Format("DEEP_SLEEP");
		}
		else
		{
			MyDelayMs(50);
			pCurrentItem = &m_TestInfo_CURRENT_CONSUMPTION.Active;

			strTestStep.Format("ACTIVE");
		}

		//-----------------------------------------------
		// get 1sec average

		if (pUsbHid->GetTouchCurrentConsumption(pCurrentItem->nAvgMs, (BYTE*)&rawCurrent) == 2)
		{
			pUsbHid->m_strRet.Format("FAIL : FAILED TO GET CURRENT CONSUMPTION.");
			goto error_current_consumption_setting;
		}

		curCurrent = (s16)((float)rawCurrent * 2.5f / 2.0f);

		//-----------------------------------------------
		// check current value

		if ((nIndex == CURRENT_CONSUMPTION_ACTIVE) && (curCurrent <= 50))
		{// FAIL if the current is less than 50 uA
			nRet = Z_TEST_ERR;
			pUsbHid->m_strRet.Format("CHECKING MPTOOL");
			break;
		}

		if (pCurrentItem->nThreshould_Max == 0)
		{// If 0, Just measure the current consumption
			pUsbHid->m_curCurrent[nIndex] = curCurrent;
		}
		else
		{
			pUsbHid->m_curCurrent[nIndex] = curCurrent;

			if (curCurrent > pCurrentItem->nThreshould_Max)
			{
				nRet = Z_TEST_ERR_SPEC_OVER;
				pUsbHid->TestErrDesc_AddDesc("%s : %d uA > %d uA", strTestStep, curCurrent, pCurrentItem->nThreshould_Max);
				pUsbHid->TestResultDesc_AddDesc("%s : %d uA > %d uA", strTestStep, curCurrent, pCurrentItem->nThreshould_Max);
			}
			else if (nIndex == CURRENT_CONSUMPTION_ACTIVE && curCurrent < pCurrentItem->nThreshould_Min)
			{
				nRet = Z_TEST_ERR_SPEC_OVER;
				pUsbHid->TestErrDesc_AddDesc("%s : %d uA < %d uA", strTestStep, curCurrent, pCurrentItem->nThreshould_Min);
				pUsbHid->TestResultDesc_AddDesc("%s : %d uA < %d uA", strTestStep, curCurrent, pCurrentItem->nThreshould_Min);
			}
			else if (curCurrent < 0)
			{
				nRet = Z_TEST_ERR_SPEC_OVER;
				pUsbHid->TestErrDesc_AddDesc("%s : %d uA < 0 uA", strTestStep, curCurrent);
				pUsbHid->TestResultDesc_AddDesc("%s : %d uA < 0 uA", strTestStep, curCurrent);
			}
		}

		if(nIndex == CURRENT_CONSUMPTION_ACTIVE)
		{
			if (pUsbHid->SetTouchConfigEx(REG_POWER_STATE_FLAG, 0x00) != 0)		// Set Auto
			{
				pUsbHid->m_strRet.Format("[ERROR] FAILED TO SET POWER STATE AUTO.");
				goto error_current_consumption_setting;
			}
			MyDelayMs(5);
		}
	}

	return nRet;

error_current_consumption_setting:

	return Z_TEST_ERR;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// HW Calibration
BOOL CTestModeView::TSM_HwCalibration(CUsbHid * pUsbHid)
{
	//unsigned int CalDelayMs;
	//int nBakMode;

#if 1
	// Power Reset & Power Sequence
	int nError = MpTool_PowerSequenceAfterReset(pUsbHid);
	if (nError != Z_HID_OK)
	{
		if (nError == Z_ERR_POWER_CONTROL)
			pUsbHid->m_strRet.Format("FAIL : POWER CONTROL");
		else
			pUsbHid->m_strRet.Format("FAIL : POWER SEQUENCE (0x%04X)", pUsbHid->GetLastError());
		goto error_hw_calibration;
	}
#endif

	//SemaWait(g_hSemaphore);

	//pUsbHid->m_strRet.Format("SEND HW CALIBRATION SEQUENCE COMMAND.");

	if(pUsbHid->SetTouchCommandEx(CMD_SW_RESET) != 0) {
		pUsbHid->m_strRet.Format("FAIL :  WRITE SEND RESET COMMAND");
		goto error_hw_calibration;
	}
	MySleep(100);
	if (pUsbHid->SetTouchCommandEx(CMD_SW_RESET) != 0) {
		pUsbHid->m_strRet.Format("FAIL :  WRITE SEND RESET COMMAND");
		goto error_hw_calibration;
	}
	MySleep(100);

	// change mode
	//nBakMode = pUsbHid->m_CapInfo.icRegData[REG_TOUCH_MODE];

	if( ChangeTouchMode(pUsbHid, TOUCH_CAL_N_DATA_MODE) == FALSE ){
		pUsbHid->m_strRet.Format("FAIL :  WRITE SEND SET TOUCH MODE");
		goto error_hw_calibration;
	}
	pUsbHid->m_CapInfo.icRegData[REG_TOUCH_MODE] = TOUCH_CAL_N_DATA_MODE;

	if(pUsbHid->SetTouchCommandEx(CMD_SW_RESET) != 0) {
		pUsbHid->m_strRet.Format("FAIL :  WRITE SEND RESET COMMAND");
		goto error_hw_calibration;
	}
	MySleep(100);

	pUsbHid->m_strRet.Format("START HW CALIBRATION.");

	if (pUsbHid->SetTouchCommandEx(CMD_GET_CALIBRATION_DATA) != 0) {
		pUsbHid->m_strRet.Format("FAIL :  WRITE SEND CALIBRATION COMMAND");
		goto error_hw_calibration;
	}
	MySleep(100);
	if(pUsbHid->SetTouchCommandEx(CMD_SW_RESET) != 0) {
		pUsbHid->m_strRet.Format("[ERROR] FAILED TO SEND RESET COMMAND");
		goto error_hw_calibration;
	}

	pUsbHid->m_strRet.Format("WAITING FOR HW CALIBRATION.");

	//CalDelayMs = 0;
	//pUsbHid->m_strRet.Format("WAITING FOR CALIBRATION...(%d) Sec", CalDelayMs++);
	MySleep(1000);
	//pUsbHid->m_strRet.Format("WAITING FOR CALIBRATION...(%d) Sec", CalDelayMs++);
	MySleep(1000);
	//pUsbHid->m_strRet.Format("WAITING FOR CALIBRATION...(%d) Sec", CalDelayMs++);
	MySleep(1000);

	do{
		if(pUsbHid->GetTouchConfig(REG_EEPROM_INFO, (BYTE*)&pUsbHid->m_CapInfo.icRegData[REG_EEPROM_INFO]) != 0) {
			pUsbHid->m_strRet.Format("FAIL :  WRITE  GET EEPROM INFO");
			goto error_hw_calibration;
		}
		if(!BIT_TEST(pUsbHid->m_CapInfo.icRegData[REG_EEPROM_INFO], 0))
			break;
		//pUsbHid->m_strRet.Format("WAITING FOR CALIBRATION...(%d) Sec", CalDelayMs++);
		MySleep(1000);
	}while(1);

	if (pUsbHid->m_CapInfo.icSignature == 0xE654)
		pUsbHid->SetTouchConfig(0x13F0, 0x1);
	else
		pUsbHid->SetTouchConfig(ZT_VCMD(VCMD_NVM_WRITE_ENABLE), 0x1);
	MySleep(10);

	if(pUsbHid->SetTouchCommandEx(CMD_SAVE_CALIBRATION_DATA) != 0) {
		pUsbHid->m_strRet.Format("[ERROR] FAIL TO RESET");
		goto error_hw_calibration;
	}

	//pUsbHid->m_strRet.Format("WAITING FOR SAVE EEPROM... 1 Sec");

	MySleep(1000);

	if (pUsbHid->m_CapInfo.icSignature == 0xE654)
		pUsbHid->SetTouchConfig(0x13F0, 0x0);
	else
		pUsbHid->SetTouchConfig(ZT_VCMD(VCMD_NVM_WRITE_ENABLE), 0x0);
	MySleep(10);

	if(pUsbHid->SetTouchCommandEx(CMD_SW_RESET) != 0) {
		pUsbHid->m_strRet.Format("[ERROR] FAILED TO SEND RESET COMMAND");
		goto error_hw_calibration;
	}

	if (pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT) != 0) {
		pUsbHid->m_strRet.Format("[ERROR] FAILED TO SEND RESET COMMAND");
		goto error_hw_calibration;
	}

	//if( ChangeTouchMode(pUsbHid, nBakMode) == FALSE ){
	//	pUsbHid->m_strRet.Format("FAIL :  WRITE SEND SET TOUCH MODE");
	//	goto error_hw_calibration;
	//}
	//pUsbHid->m_CapInfo.icRegData[REG_TOUCH_MODE] = nBakMode;

	//::ReleaseSemaphore(g_hSemaphore, 1, NULL);
	//AfxMessageBox("FINISHED H/W CALIBRATION.");
	return TRUE;

error_hw_calibration:

	//if( ChangeTouchMode(pUsbHid, nBakMode) == FALSE )
	//	pUsbHid->m_strRet.Format("FAIL :  WRITE SEND SET TOUCH MODE");
	//else
	//	pUsbHid->m_CapInfo.icRegData[REG_TOUCH_MODE] = nBakMode;

	//AfxMessageBox(err_msg);
	//::ReleaseSemaphore(g_hSemaphore, 1, NULL);
	return FALSE;
}

BOOL CTestModeView::ReadRawData_HwCalibration(CUsbHid * pUsbHid, short int *sRawData)
{
	int ret, nRetry = 0;

	if( PrepareToReadRawData(pUsbHid) != TRUE )
		return FALSE;

	// Read Garbage Data
	if( ReadGarbageData(pUsbHid, sRawData) == FALSE )
		return FALSE;

	// Read Raw Data
	do
	{
		ret = pUsbHid->ReadTouchRawData((BYTE*)&sRawData[0], (int)(pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]) * 2);

		if (ret == 0)
		{
			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
			break;
		}
		else if (nRetry++ > RETRY_COUNT || ret == 2)
		{
			//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
			//UpdateStatusBar(&err_msg);
			return FALSE;
		}
	} while (1);

	return TRUE;
}

BOOL CTestModeView::CheckRawDataRange_HwCalibration(CUsbHid * pUsbHid, short int *sRawData)
{
#if(1)
	//솔루션 이재덕 선임 요청사항.
	BOOL bRet = TRUE;
	int nDiff_Max;
	int i, j;

	float fAverage = (100) * m_TestInfo_HW_CALIBRATION.nHwCalibrationMax / 100;

	nDiff_Max = fAverage;

	for (i = 0; i < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X]; i++)
	{
		for (j = 0; j < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]; j++)
		{
			int nCurDiff = ((int)sRawData[i*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + j]);

			if (nCurDiff > nDiff_Max) {
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, j, nDiff_Max, (int)nCurDiff);
				bRet = FALSE;
			}
		}
	}
	return bRet;
#else
	BOOL bRet = TRUE;
	int nDiff_Max, nDiff_Min;
	int i, j;

	float fAverage = (100) * m_TestInfo_HW_CALIBRATION.nHwCalibrationMax / 100;

	nDiff_Max =  100 + fAverage;
	nDiff_Min = 100 - fAverage;

	for (i = 0; i < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X]; i++)
	{
		for (j = 0; j < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]; j++)
		{
			int nCurDiff = ((int)sRawData[i*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + j]);

			if (nCurDiff > nDiff_Max) {
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, j, nDiff_Max, (int)nCurDiff);
				bRet = FALSE;
			}
			if (nCurDiff < nDiff_Min) {
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Min, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, j, nDiff_Min, (int)nCurDiff);
				bRet = FALSE;
			}
		}
	}
	return bRet;
#endif

}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// JITTER

BOOL CTestModeView::ReadRawData_JitterN(CUsbHid * pUsbHid, short int *sRawData)
{
	// AFE setting, change Touch mode
	if( PrepareToReadRawData(pUsbHid) != TRUE )
		return FALSE;

	// Read Garbage Data
	if( ReadGarbageData(pUsbHid, sRawData) != TRUE )
		return FALSE;

	// Read Raw Data
	int ret, nRetry = 0;
	int nReadDataSize = (int)(pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]) * 2;

	do
	{
		ret = pUsbHid->ReadTouchRawData((BYTE*)&sRawData[0], nReadDataSize);

		if (ret == 0)
		{
			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
			break;
		}
		else if (nRetry++ > RETRY_COUNT || ret == 2)
		{
			//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
			//UpdateStatusBar(&err_msg);
			return FALSE;
		}
	} while (1);

	return TRUE;
}

BOOL CTestModeView::ReadRawData_JitterN_ByMPztd(CUsbHid * pUsbHid, short int * pResultData)
{
	int ret, nRetry, idx, n;
	int nTotalNodeCount = (int)(pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]);
	int nReadDataSize = nTotalNodeCount * 2;

	s16	RawData[MAX_RAWDATA_NUM];
	s16	BaselineData[MAX_RAWDATA_NUM];
	s16	NormalData[MAX_RAWDATA_NUM];
	s16	PlusIiredData[MAX_RAWDATA_NUM];

	// AFE setting, change Touch mode
	if( PrepareToReadRawData(pUsbHid) != TRUE )
		return FALSE;

	// Read Garbage Data
	if( ReadGarbageData(pUsbHid, RawData) != TRUE )
		return FALSE;

	// Read Garbage Data - for baseline
	if( ReadGarbageData(pUsbHid, RawData) != TRUE )
		return FALSE;

	// Read Raw Data - for baseline
	nRetry = 0;
	do
	{
		ret = pUsbHid->ReadTouchRawData((BYTE*)&RawData[0], nReadDataSize);

		if (ret == 0)
		{
			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
			break;
		}
		else if (nRetry++ > RETRY_COUNT || ret == 2)
		{
			//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
			//UpdateStatusBar(&err_msg);
			return FALSE;
		}
	} while (1);

	memcpy(&BaselineData[0], &RawData[0], nReadDataSize);
	memcpy(&NormalData[0], &RawData[0], nReadDataSize);

	memset((u8*)&pResultData[0], 0x00, nReadDataSize);

	for(idx = 0; idx < m_TestInfo_JITTER.nJitterReadCount; idx++)
	{
		// Read Raw Data
		nRetry = 0;
		do
		{
			ret = pUsbHid->ReadTouchRawData((BYTE*)&RawData[0], nReadDataSize);

			if (ret == 0)
			{
				pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
				break;
			}
			else if (nRetry++ > RETRY_COUNT || ret == 2)
			{
				//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
				//UpdateStatusBar(&err_msg);
				return FALSE;
			}
		} while (1);

		for(n = 0; n < nTotalNodeCount; n++)
		{
			//NormalData[n] = (NormalData[n] + RawData[n])/2;
			NormalData[n] = RawData[n];
		}

		for(n = 0; n < nTotalNodeCount; n++)
		{
			PlusIiredData[n] = abs(NormalData[n] - BaselineData[n]);
			BaselineData[n] = NormalData[n];
		}

		//XlineFilter(S16_PLUS_MAX, 120, (s16*)&PlusIiredData[0], pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X], pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]);

		for(n = 0; n < nTotalNodeCount; n++)
		{
			if(pResultData[n] < PlusIiredData[n])
				pResultData[n] = PlusIiredData[n];
		}
	}

	return TRUE;
}

#if defined(D_PRODUCT_SOLUTION_ALLROUNDER)
BOOL CTestModeView::ReadRawData_JitterN_AllRounder(CUsbHid * pUsbHid, short int *sRawData)
{
	int ret, nRetry;

#if 1
	// Power Reset & Power Sequence
	int nError = MpTool_PowerSequenceAfterReset(pUsbHid);
	if (nError != Z_HID_OK)
	{
		if (nError == Z_ERR_POWER_CONTROL)
			pUsbHid->m_strRet.Format("FAIL : POWER CONTROL");
		else
			pUsbHid->m_strRet.Format("FAIL : POWER SEQUENCE (0x%04X)", pUsbHid->GetLastError());
		return FALSE;
	}
#endif

	// 중요
	if (pUsbHid->SetTouchConfigEx(REG_JITTER_N_DATA_CNT, 1) != 0) {
		//err_msg.Format("[ERROR] FAILED TO SET %04XH = %d.", REG_JITTER_N_DATA_CNT, 1);
		//UpdateStatusBar(&err_msg);
		return FALSE;
	}

	// Change AFE setting
	if( ChangeAfeSetting(pUsbHid) != TRUE )
		return FALSE;

	if( ChangeTouchMode(pUsbHid, TOUCH_MODE_JITTER) == FALSE )
		return FALSE;

#if 0	// Modify_by_solution_dev
	// Read Garbage Data
	if( ReadGarbageData(pUsbHid, sRawData) == FALSE )
		return FALSE;
#endif

	// Read Raw Data
	nRetry = 0;
	int nReadDataSize = (int)(pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]) * 2;

	do
	{
		ret = pUsbHid->ReadTouchRawData((BYTE*)&sRawData[0], nReadDataSize);

		if (ret == 0)
		{
			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
			break;
		}
		else if (nRetry++ > RETRY_COUNT || ret == 2)
		{
			//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
			//UpdateStatusBar(&err_msg);
			return FALSE;
		}
	} while (1);

	if (pUsbHid->SetTouchConfigEx(REG_JITTER_N_DATA_CNT, 1) != 0) {
		//err_msg.Format("[ERROR] FAILED TO SET %04XH = %d.", REG_JITTER_N_DATA_CNT, 1);
		//UpdateStatusBar(&err_msg);
		return FALSE;
	}

	nRetry = 0;
	do
	{
		ret = pUsbHid->ReadTouchRawData((BYTE*)&sRawData[0], (int)(pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]) * 2);

		if (ret == 0)
		{
			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
			break;
		}
		else if (nRetry++ > RETRY_COUNT || ret == 2)
		{
			//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
			//UpdateStatusBar(&err_msg);
			return FALSE;
		}
	} while (1);


	if (pUsbHid->SetTouchConfigEx(REG_JITTER_N_DATA_CNT, pUsbHid->m_CapInfo.icRegData[REG_JITTER_N_DATA_CNT]) != 0) {
		//err_msg.Format("[ERROR] FAILED TO SET %04XH = %d.", REG_JITTER_N_DATA_CNT, pUsbHid->m_CapInfo.icRegData[REG_JITTER_N_DATA_CNT]);
		//UpdateStatusBar(&err_msg);
		return FALSE;
	}

	// Read Raw Data
	nRetry = 0;
	do
	{
		ret = pUsbHid->ReadTouchRawData((BYTE*)&sRawData[0], (int)(pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]) * 2);

		if (ret == 0)
		{
			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
			break;
		}
		else if (nRetry++ > RETRY_COUNT || ret == 2)
		{
			//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
			//UpdateStatusBar(&err_msg);
			return FALSE;
		}
	} while (1);

	return TRUE;
}
#endif

BOOL CTestModeView::CheckRawData_JitterMax(CUsbHid * pUsbHid, short int *sRawData)
{
	BOOL bRet = TRUE;
	int i;

	int nReadDataSize = (int)(pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]);

	for (i = 0; i<nReadDataSize; i++)
	{
		if ((int)sRawData[i] > m_TestInfo_JITTER.nJitterDataMax)
		{
			pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d",
				i%pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y], i / pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y], hvItoaWithSpace(pUsbHid->m_nDevIndex, m_TestInfo_JITTER.nJitterDataMax, 5), (int)sRawData[i]);
			pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER,\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d",
				i%pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y], i / pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y], m_TestInfo_JITTER.nJitterDataMax, (int)sRawData[i]);
			bRet = FALSE;
		}
	}

	return bRet;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// DIGITAL WHEEL

BOOL CTestModeView::ReadRawData_DigitalWheel(CUsbHid * pUsbHid, short int *sRawData)
{
	// change touch mode
	if( ChangeTouchMode(pUsbHid, GetTouchMode(pUsbHid)) != TRUE )
		return FALSE;

	// Read Garbage Data
	if( ReadGarbageData(pUsbHid, sRawData) != TRUE )
		return FALSE;

	// Read Raw Data
	int ret, nRetry = 0;
	int nReadDataSize = (pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]) * 2;

	do
	{
		ret = pUsbHid->ReadTouchRawData((BYTE*)&sRawData[0], nReadDataSize);

		if(ret == 0)
		{
			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
			break;
		}
		else if(nRetry++ > RETRY_COUNT || ret == 2)
		{
			//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
			//UpdateStatusBar(&err_msg);
			return FALSE;
		}
	} while(1);

	return TRUE;
}

BOOL CTestModeView::CheckRawDataRange_DigitalWheel(CUsbHid * pUsbHid, short int *sRawData)
{
	BOOL bRet = TRUE;
	int nDiff_Max, nDiff_Min;
	int i, j;

	for (i = 0; i < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X]; i++)
	{
		for (j = 0; j < pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]; j++)
		{
			int nCurDiff = ((int)sRawData[i*pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y] + j]);

			nDiff_Max = m_TestInfo_DIGITAL_WHEEL.pDataMax[i][j];
			nDiff_Min = m_TestInfo_DIGITAL_WHEEL.pDataMin[i][j];

			if (nCurDiff == 0)
				continue;

			if (nCurDiff > nDiff_Max)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Max, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MAX),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, j, nDiff_Max, (int)nCurDiff);
				bRet = FALSE;
			}
			if (nCurDiff < nDiff_Min)
			{
				pUsbHid->TestErrDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%s,\t\tCUR_VALUE:%d", i, j, hvItoaWithSpace(pUsbHid->m_nDevIndex, nDiff_Min, 5), (int)nCurDiff);
				pUsbHid->TestResultDesc_AddDesc("[X%04d_Y%04d],\t\tSPEC_OVER(MIN),\t\tSPEC_VALUE:%05d,\t\tCUR_VALUE:%05d", i, j, nDiff_Min, (int)nCurDiff);
				bRet = FALSE;
			}
		}
	}

	return bRet;
}


#if defined(D_USE_CHECK_BUTTON_TEST)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// GPIO_BUTTON
void CTestModeView::GpioButton_ShowDialog(BOOL bShow)
{
	if (!m_pGpioButtonDlg)
		return;

	BOOL bRet = -1;

	do
	{
		if (bShow)
			bRet = m_pGpioButtonDlg->PostMessage(WM_USER_SHOW_BUTTON_TEST, 1, 0);
		else
			bRet = m_pGpioButtonDlg->PostMessage(WM_USER_SHOW_BUTTON_TEST, 0, 0);
	}while( bRet == 0 );
}

BOOL CTestModeView::GpioButton_RequestTest(CUsbHid * pUsbHid)
{
	BOOL bShowDialog = TRUE;
	BOOL bButtonResult = FALSE;
	BOOL bButtonTest_FirstDevice = TRUE;

	stCoordinfo coordInfo;
	u16 nBtnReg;
	int nDetectCount = 0;

	SemaWait(g_hInfoSemaphore);
	for (int n = 0; n < MAX_DEVICE_COUNT; n++)
	{
		if (m_GpioButtonStatus.nStatus[n] == E_BUTTON_TEST_REQUEST)
		{
			bButtonTest_FirstDevice = FALSE;
			break;
		}
	}
	pUsbHid->m_bButtonTest_FirstDevice = bButtonTest_FirstDevice;
	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);

	do
	{
		bShowDialog = GpioButton_WaitTest(pUsbHid);
	}while(bShowDialog == FALSE);

	if (pUsbHid->SetTouchConfigEx(REG_TOUCH_MODE, TOUCH_POINT_MODE) != 0) {
		//pUsbHid->m_strRet.Format("FAIL :  WRITE SEND SET TOUCH MODE");
		return FALSE;
	}
	MyDelayMs(20);

	// wait for dialog to be closed...
	while (1)
	{
		//Sleep(100);

		if (m_GpioButtonStatus.bShowDialog == FALSE)
			break;

		if (bButtonResult)
			continue;

		if (pUsbHid->ReadTouchGetPoint((BYTE*)&coordInfo, sizeof(stCoordinfo)) == 0)
		{
			if (BIT_TEST(coordInfo.status, BIT_BUTTON_EVENT) && !BIT_TEST(coordInfo.status, BIT_MUST_ZERO))
			{
				if (pUsbHid->GetTouchConfig(REG_BUTTON, (BYTE *)&nBtnReg) == 0 && BIT_TEST(nBtnReg, 0))
				{
					nDetectCount++;

					if (nDetectCount > 0)
					{
						bButtonResult = TRUE;

						m_GpioButtonStatus.nStatus[pUsbHid->m_nDevIndex] = E_BUTTON_TEST_OK;

						BOOL bRet = -1;

						do
						{
							bRet = m_pGpioButtonDlg->PostMessage(WM_USER_UPDATE_BUTTON_STATUS, pUsbHid->m_nDevIndex, E_BUTTON_TEST_OK);
						} while (bRet == 0);
					}
				}
			}

			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
		}
	}

	return bButtonResult;
}

BOOL CTestModeView::GpioButton_WaitTest(CUsbHid * pUsbHid)
{
	BOOL bShowDialog = TRUE;
	UINT64 dtStartTime;

	SemaWait(g_hInfoSemaphore);
	m_GpioButtonStatus.nStatus[pUsbHid->m_nDevIndex] = E_BUTTON_TEST_REQUEST;

	for (int n = 0; n < MAX_DEVICE_COUNT; n++)
	{
		if (m_GpioButtonStatus.nStatus[n] == E_BUTTON_TEST_ON)
		{
			bShowDialog = FALSE;
			break;
		}
	}
	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);

	if (bShowDialog)
	{
		GpioButton_ShowDialog(TRUE);
		Sleep(1000);
		return TRUE;
	}

	dtStartTime = GetTimeInMillis();

	// wait for dialog to be showed...
	while (1)
	{
		Sleep(1000);

		if (m_GpioButtonStatus.bShowDialog)
		{
			bShowDialog = TRUE;
			break;
		}

		if (pUsbHid->m_bButtonTest_FirstDevice && (GetTimeInMillis() - dtStartTime > 3000))
		{
			// if timeout, retry to check test condition.
			// The test conition can set E_BUTTON_TEST_OFF while test.
			bShowDialog = FALSE;
			break;
		}
	}

	return bShowDialog;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// TOUCH_BUTTON
void CTestModeView::TouchButton_ShowDialog(BOOL bShow)
{
	if (!m_pTouchButtonDlg)
		return;

	BOOL bRet = -1;

	do
	{
		if (bShow)
			bRet = m_pTouchButtonDlg->PostMessage(WM_USER_SHOW_BUTTON_TEST, 1, 0);
		else
			bRet = m_pTouchButtonDlg->PostMessage(WM_USER_SHOW_BUTTON_TEST, 0, 0);
	}while( bRet == 0 );
}

BOOL CTestModeView::TouchButton_RequestTest(CUsbHid * pUsbHid)
{
	BOOL bShowDialog = TRUE;
	BOOL bButtonResult = FALSE;
	BOOL bSensitivityError = FALSE;
	BOOL bButtonTest_FirstDevice = TRUE;

	int nDetectCount = 0, nIndex;

	int nButtonCount = pUsbHid->m_CapInfo.icRegData[REG_BUTTON_SUPPORTED_NUM];
	int nButtonLineNum = pUsbHid->m_CapInfo.icRegData[REG_BUTTON_LINE_NUM];
	int nButtonNode = pUsbHid->m_CapInfo.icRegData[REG_BUTTON_NODE];
	int nButtonDataPos = nButtonLineNum * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y];

	s16	sRawData[MAX_RAWDATA_NUM];
	s16 curButtonSensitivity;

	memset(&pUsbHid->m_ButtonSensitivity[0], 0, sizeof(s16)*8);

	if (pUsbHid->IsCoreFrameWork() == FALSE)
	{ // CHINA
		// Change AFE setting
		if( ChangeAfeSetting(pUsbHid) != TRUE )
			return FALSE;
	}

	// change Touch mode
	if (pUsbHid->SetTouchConfigEx(REG_TOUCH_MODE, TOUCH_MODE_STRENGTH_DATA) != 0)
		return FALSE;

	MyDelayMs(20);

	SemaWait(g_hInfoSemaphore);
	for (int n = 0; n < MAX_DEVICE_COUNT; n++)
	{
		if (m_TouchButtonStatus.nStatus[n] == E_BUTTON_TEST_REQUEST)
		{
			bButtonTest_FirstDevice = FALSE;
			break;
		}
	}
	pUsbHid->m_bButtonTest_FirstDevice = bButtonTest_FirstDevice;
	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);

	do
	{
		bShowDialog = TouchButton_WaitTest(pUsbHid);
	}while(bShowDialog == FALSE);

	// wait for dialog to be closed...
	while (1)
	{
		Sleep(100);

		if (m_TouchButtonStatus.bShowDialog == FALSE)
			break;

		if (bButtonResult)
			continue;

		if (TouchButton_ReadRawData(pUsbHid, sRawData) == FALSE)
			continue;

		bSensitivityError = FALSE;

		for(nIndex=0; nIndex<nButtonCount; nIndex++)
		{
			curButtonSensitivity = sRawData[nButtonDataPos + nIndex];

			if( curButtonSensitivity > pUsbHid->m_ButtonSensitivity[nIndex] )
			{
				pUsbHid->m_ButtonSensitivity[nIndex] = curButtonSensitivity;
			}

#ifdef _DEBUG
			TRACE("[%d] %d\n", nIndex, curButtonSensitivity);

			BOOL bRet = -1;

			do
			{
				bRet = m_pTouchButtonDlg->PostMessage(WM_USER_UPDATE_BUTTON_VALUE, curButtonSensitivity, 0);
			}while( bRet == 0 );
#endif

			if(curButtonSensitivity < m_nTouchButton_Threshold)
			{
				bSensitivityError = TRUE;
				break;
			}
		}

		if (bSensitivityError == TRUE)
			continue;

		nDetectCount++;

		if (nDetectCount > 0)
		{
			bButtonResult = TRUE;

			m_TouchButtonStatus.nStatus[pUsbHid->m_nDevIndex] = E_BUTTON_TEST_OK;

			BOOL bRet = -1;

			do
			{
				bRet = m_pTouchButtonDlg->PostMessage(WM_USER_UPDATE_BUTTON_STATUS, pUsbHid->m_nDevIndex, E_BUTTON_TEST_OK);
			}while( bRet == 0 );
		}
	}

	if (bButtonResult == FALSE)
	{
		pUsbHid->TestErrDesc_AddDesc("%d < %d", pUsbHid->m_ButtonSensitivity[0], m_nTouchButton_Threshold);
		pUsbHid->TestResultDesc_AddDesc("%d < %d", pUsbHid->m_ButtonSensitivity[0], m_nTouchButton_Threshold);
	}

	return bButtonResult;
}

BOOL CTestModeView::TouchButton_WaitTest(CUsbHid * pUsbHid)
{
	BOOL bShowDialog = TRUE;
	UINT64 dtStartTime;

	SemaWait(g_hInfoSemaphore);
	m_TouchButtonStatus.nStatus[pUsbHid->m_nDevIndex] = E_BUTTON_TEST_REQUEST;

	for (int n = 0; n < MAX_DEVICE_COUNT; n++)
	{
		if (m_TouchButtonStatus.nStatus[n] == E_BUTTON_TEST_ON)
		{
			bShowDialog = FALSE;
			break;
		}
	}
	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);

	if (bShowDialog)
	{
		TouchButton_ShowDialog(TRUE);
		Sleep(1000);
		return TRUE;
	}

	dtStartTime = GetTimeInMillis();

	// wait for dialog to be showed...
	while (1)
	{
		Sleep(1000);

		if (m_TouchButtonStatus.bShowDialog)
		{
			bShowDialog = TRUE;
			break;
		}

		if (pUsbHid->m_bButtonTest_FirstDevice && (GetTimeInMillis() - dtStartTime > 3000))
		{
			// if timeout, retry to check test condition.
			// The test conition can set E_BUTTON_TEST_OFF while test.
			bShowDialog = FALSE;
			break;
		}
	}

	return bShowDialog;
}

BOOL CTestModeView::TouchButton_ReadRawData(CUsbHid * pUsbHid, short int *sRawData)
{
	// Read Garbage Data
	if( ReadGarbageData(pUsbHid, sRawData) != TRUE )
		return FALSE;

	// Read Raw Data
	int ret, nRetry = 0;
	int nReadDataSize = (pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_X] * pUsbHid->m_CapInfo.icRegData[REG_TOTAL_NUMBER_OF_Y]) * 2;

	do
	{
		ret = pUsbHid->ReadTouchRawData((BYTE*)&sRawData[0], nReadDataSize);

		if(ret == 0)
		{
			pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT);
			break;
		}
		else if(nRetry++ > RETRY_COUNT || ret == 2)
		{
			//err_msg.Format("[ERROR] FAILED TO READ RAW DATA.");
			//UpdateStatusBar(&err_msg);
			return FALSE;
		}
	} while(1);

	return TRUE;
}
#endif


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// NOISE
BOOL CTestModeView::CheckNoise(CUsbHid * pUsbHid, short int *sNoise)
{
	BOOL bRet = TRUE;
	int i;
	int checkTimes;

	checkTimes = (m_TestInfo_NOISE.m_nCheckNoise_ReadTimes) < 99 ? (m_TestInfo_NOISE.m_nCheckNoise_ReadTimes) : 99;

	for (i = 0; i < checkTimes; i++)
	{
		if (sNoise[i] > m_TestInfo_NOISE.m_nCheckNoise_Threshold)
		{
			if (m_TestInfo_NOISE.AfeInfo.bPrintPpVariation != 1)
			{
				if (sNoise[i] > m_TestInfo_NOISE.m_nCheckNoise_Threshold + 10)
				{
					sNoise[i] = m_TestInfo_NOISE.m_nCheckNoise_Threshold + 10;
				}
				pUsbHid->TestErrDesc_AddDesc("%d > %d", sNoise[i], m_TestInfo_NOISE.m_nCheckNoise_Threshold);
				pUsbHid->TestResultDesc_AddDesc("%d > %d", sNoise[i], m_TestInfo_NOISE.m_nCheckNoise_Threshold);
				bRet = FALSE;
			}
			else
			{
				pUsbHid->TestErrDesc_AddDesc("%d > %d", sNoise[i], m_TestInfo_NOISE.m_nCheckNoise_Threshold);
				pUsbHid->TestResultDesc_AddDesc("%d > %d", sNoise[i], m_TestInfo_NOISE.m_nCheckNoise_Threshold);
				bRet = FALSE;
			}
		}
		else
		{
			//pUsbHid->TestErrDesc_AddDesc("%d <= %d", sNoise[i], m_TestInfo_NOISE.m_nCheckNoise_Threshold);
			//pUsbHid->TestResultDesc_AddDesc("%d <= %d", sNoise[i], m_TestInfo_NOISE.m_nCheckNoise_Threshold);
		}
	}

	return bRet;
}

BOOL CTestModeView::Noise_RequestTest(CUsbHid * pUsbHid, short int *sNoise, char num)
{
	int ret,i;
	int readTimes;
	int nMaxVariation;

	readTimes = (m_TestInfo_NOISE.m_nCheckNoise_ReadTimes) < 99 ? (m_TestInfo_NOISE.m_nCheckNoise_ReadTimes) : 99;

	ret = PrepareToReadNoise(pUsbHid,num);
	if (ret != TRUE)
	{
		return FALSE;
	}
	MySleep(10);

	if (readTimes > 5)
		ret = 5;
	else
		ret = readTimes;

	for (i = 0; i < ret; i++)
	{
		MySleep(m_TestInfo_NOISE.m_nCheckNoise_Interval);
		pUsbHid->GetTouchPpVariationMax((BYTE*)(&sNoise[i]), FALSE);
	}

	nMaxVariation = -1;
	for (i = 0; i < readTimes; i++)
	{
		MySleep(m_TestInfo_NOISE.m_nCheckNoise_Interval);

		ret = pUsbHid->GetTouchPpVariationMax((BYTE*)(&sNoise[i]), FALSE);

		if (ret != 0)
		{// error
			if (i > 0)
				sNoise[99] = nMaxVariation;
			else
				sNoise[99] = 0;
			return FALSE;
		}

		if (sNoise[i] > nMaxVariation)
			nMaxVariation = sNoise[i];
	}

	sNoise[99] = nMaxVariation;

	return TRUE;
}


BOOL CTestModeView::ReadASICID(CUsbHid * pUsbHid)
{
	// 처음 2개의 IUM 영역을 비교한다
	char * pAsicID_0 = pUsbHid->m_cAsicID_0;
	char * pAsicID_1 = pUsbHid->m_cAsicID_1;

	memset(pAsicID_0, 0x00, sizeof(pUsbHid->m_cAsicID_0));
	memset(pAsicID_1, 0x00, sizeof(pUsbHid->m_cAsicID_1));

	if (pUsbHid->GetTouchRegReadCmd(ZT_VCMD(VCMD_IUM_READ_DATA), (BYTE*)pUsbHid->m_IumData, g_FwDownloadInfo.icIumSize) != 0)
	{
		sprintf(pAsicID_0, "%s", STRING_INVALID_ASIC_ID);
		sprintf(pAsicID_1, "%s", STRING_INVALID_ASIC_ID);
		return FALSE;
	}

	char * pIumData = (char*)pUsbHid->m_IumData;

	CString strAsicID_0 = "", strAsicID_1 = "";

	if (m_nIUMFormat == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			//add 2021-10-14 김극래 책임 다르게 ASIC ID 읽히는 부분 디버깅.
			//## CTestModeView::ReadASICID ##
			//0x3F8A 0x010C 0x3310 0x534B 0x0014 0xE622 0x0000 0x0000
			//## CTestModeView::ReadASICID ##
			//strAsicID_0:3FFFFFFF8A010C3310534B

	        strAsicID_0.AppendFormat(_T("%02X"), pIumData[(i * 2) + 1] & 0x00FF);
	        strAsicID_0.AppendFormat(_T("%02X"), pIumData[(i * 2) + 0] & 0x00FF);

	        strAsicID_1.AppendFormat(_T("%02X"), pIumData[(i * 2) + 1 + 0x20] & 0x00FF);
	        strAsicID_1.AppendFormat(_T("%02X"), pIumData[(i * 2) + 0 + 0x20] & 0x00FF);
		}
	}
	else if (m_nIUMFormat == 1)
	{
		for (int i = 0; i < 7; i++)
		{
			//add 2021-10-14 김극래 책임 다르게 ASIC ID 읽히는 부분 디버깅.
			//## CTestModeView::ReadASICID ##
			//0x3F8A 0x010C 0x3310 0x534B 0x0014 0xE622 0x0000 0x0000
			//## CTestModeView::ReadASICID ##
			//strAsicID_0:3FFFFFFF8A010C3310534B

	        strAsicID_0.AppendFormat(_T("%02X"), pIumData[(i * 2) + 1] & 0x00FF);
	        strAsicID_0.AppendFormat(_T("%02X"), pIumData[(i * 2) + 0] & 0x00FF);

	        strAsicID_1.AppendFormat(_T("%02X"), pIumData[(i * 2) + 1 + 0x20] & 0x00FF);
	        strAsicID_1.AppendFormat(_T("%02X"), pIumData[(i * 2) + 0 + 0x20] & 0x00FF);
		}
	}

	if (strAsicID_0.Compare(strAsicID_1) != 0)
	{// mismatching
		sprintf(pAsicID_0, "%s", STRING_INVALID_ASIC_ID);
		sprintf(pAsicID_1, "%s", STRING_INVALID_ASIC_ID);
		return FALSE;
	}

	if (m_nIUMFormat == 0)
	{
		memcpy(&pAsicID_0[0], strAsicID_0.GetBuffer(), 16);
		memcpy(&pAsicID_1[0], strAsicID_1.GetBuffer(), 16);
	}
	else
	{
		memcpy(&pAsicID_0[0], strAsicID_0.GetBuffer(), 28);
		memcpy(&pAsicID_1[0], strAsicID_1.GetBuffer(), 28);
	}

	return TRUE;
}

BOOL CTestModeView::CheckModuleID(CUsbHid * pUsbHid, int nModuleID)
{
	BOOL bRet = TRUE;

	char * Buff = (char*)malloc(128);
	int retry_cnt = 0;

	if (Buff == NULL)
		return FALSE;

	if (pUsbHid->GetTouchConfigEx(REG_TSM_MODULE_ID, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_TSM_MODULE_ID]) != 0) return FALSE;

	while (1)
	{
		if (pUsbHid->m_CapInfo.icRegData[REG_TSM_MODULE_ID] == 0)
		{
			//WRITE Data(Module id)
			if (pUsbHid->SetTouchConfigEx(REG_TSM_MODULE_ID, 0xFEFE) != 0)
				return FALSE;
		}

		if (pUsbHid->SetTouchCommand(ZT_VCMD(VCMD_FW_UPGRADE_INIT), FALSE) != 0)								return FALSE;

		//WRITE VCMD_ENABLE
		if (pUsbHid->SetTouchConfigEx(ZT_VCMD(VCMD_ENABLE), 1) != 0)											return FALSE;

		//WRITE Data(Module id)	// 49,152 / 128 = 384
		if (pUsbHid->SetTouchConfigEx(ZT_VCMD(VCMD_FW_UPGRADE_START_PAGE), ZT_EEPROM_LAST_PAGE) != 0)			return FALSE;
		if (pUsbHid->ReadFirmwareData(ZT_VCMD(VCMD_FW_UPGRADE_READ), 0xffff, (char *)&Buff[0], 128) != 0)		return FALSE;

		pUsbHid->m_nModuleID = ((Buff[1] << 8 & 0xFF00) | (Buff[0] & 0x00FF));

		if (pUsbHid->m_nModuleID == nModuleID)
		{
			break;
		}
		else
		{
			retry_cnt++;
			if (retry_cnt > 3)
			{
				// ERROR
				bRet = FALSE;
				break;
			}
		}
	}

	ZT_FREE(Buff);

	return bRet;
}

BOOL CTestModeView::CheckVirtualDrawTest(CUsbHid* pUsbHid)
{
	BOOL bRet = FALSE;
	s16 nPrevValue;
	s16 nTestResult;
	u8 nRetryCount = 10;

	// get previous value
	if (pUsbHid->GetTouchConfigEx(REG_INTERRUPT_FLAG, (BYTE *)&nPrevValue) != 0)
	{
		pUsbHid->m_strRet.Format("[ERROR] FAIL READ REG_INTERRUPT_FLAG");
		return FALSE;
	}

	// disable INT PIN
	pUsbHid->m_CapInfo.icRegData[REG_INTERRUPT_FLAG] = nPrevValue;
	BIT_SET(pUsbHid->m_CapInfo.icRegData[REG_INTERRUPT_FLAG], DEF_BIT_INT_PIN_DISABLE);
	if (pUsbHid->SetTouchConfigEx(REG_INTERRUPT_FLAG, pUsbHid->m_CapInfo.icRegData[REG_INTERRUPT_FLAG]) != 0)
	{
		pUsbHid->m_strRet.Format("[ERROR] FAIL DISABLE INT PIN");
		return FALSE;
	}

	// change touch mode
	if( ChangeTouchMode(pUsbHid, GetTouchMode(pUsbHid)) != TRUE )
	{
		pUsbHid->m_strRet.Format("[ERROR] FAIL CHANGE VIRTUAL DRAW MODE");
		return FALSE;
	}
	MySleep(30);

	// clear interrupt pin
	if (pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT) != 0)
	{
		pUsbHid->m_strRet.Format("[ERROR] FAIL CMD_CLEAR_INTERRUPT");
		return FALSE;
	}
	MySleep(20);

	// waiting...
	MySleep(500);

	// wait until test is completed
	while(1)
	{
		// read result register
		if(pUsbHid->GetTouchConfigEx(REG_TM_DRAW_MODE_TEST_RESULT, (BYTE *)&nTestResult) == 0)
		{
			if(nTestResult != (s16)0x0011			// Test 진행 중...
			&& nTestResult != (s16)0x0000)			// Test 종료 또는 유효하지 않은 값
			{
				if(nTestResult == (s16)0x900D)
				{
					bRet = TRUE;
				}
				else
				{
					if(nTestResult == (s16)0xFA11)
						pUsbHid->m_strRet.Format("[ERROR] COORDINATES ARE MISMATCHING");
					else
						pUsbHid->m_strRet.Format("[ERROR] RESULT IS NOT VALID");

					bRet = FALSE;
				}

				break;
			}
		}

		nRetryCount--;
		if(nRetryCount == 0)
		{
			// timeout
			pUsbHid->m_strRet.Format("[ERROR] CANNOT READ RESULT - TIMEOUT");
			bRet = FALSE;
			break;
		}

		MySleep(300);
	}

	// restore previous value
	pUsbHid->SetTouchConfigEx(REG_TOUCH_MODE, TOUCH_POINT_MODE);

	pUsbHid->m_CapInfo.icRegData[REG_INTERRUPT_FLAG] = nPrevValue;
	pUsbHid->SetTouchConfigEx(REG_INTERRUPT_FLAG, nPrevValue);

	return bRet;
}

//==============================================================================================
// SAVE INFO
//==============================================================================================

BOOL CTestModeView::TSM_SaveEeprom(CUsbHid * pUsbHid)
{
	BOOL bRet = TRUE;
	int nRetry = 0;
	int nModuleID;

	// Fitbit - save by F/W
	if (m_bAdditionalLOG)
	{
SET_MODULE_ID_FITBIT_RETRY:

		MySleep(10);

		bRet = (pUsbHid->SetTouchConfigEx(REG_TSM_MODULE_ID, 0xFEFE) != 0) ? FALSE : TRUE;
		if (bRet == FALSE)
		{
			if (nRetry < 5)
			{
				nRetry++;
				goto SET_MODULE_ID_FITBIT_RETRY;
			}
			pUsbHid->m_strRet.Format("[ERROR] REG_TSM_MODULE_ID write (0xFEFE) ");
			bRet = FALSE;
		}

		return bRet;
	}

	//------------------------------------------------------------------------
	// [CAUTION] 현재 MPztd 에서 아래와 같은 방법으로 EEPROM save 하는 과제는 없음.
	//------------------------------------------------------------------------

	nModuleID = char2dec((char*)m_strModuleID.GetBuffer());

	if (pUsbHid->SetTouchConfigEx(REG_EEPROM_INFO, 0xFFAA) != 0)
	{
		pUsbHid->m_strRet.Format("[ERROR] REG_EEPROM_INFO write (0xFFAA) ");
		bRet = FALSE;
	}

	pUsbHid->SetTouchCommand(CMD_SW_RESET);
	MySleep(100);
	pUsbHid->SetTouchCommand(CMD_SW_RESET);
	MySleep(100);
	pUsbHid->SetTouchCommand(CMD_SW_RESET);
	MySleep(100);

	// ZT75XX_VCMD_WRITE_ENABLE(0x01)
	if (pUsbHid->SetTouchConfigEx(ZT_VCMD(VCMD_NVM_WRITE_ENABLE), 0x1) != 0)
	{
		pUsbHid->m_strRet.Format("[ERROR] FAIL ZT75XX_VCMD_WRITE_ENABLE(0x1)");
		bRet = FALSE;
	}

	nRetry = 0;

SET_MODULE_ID_RETRY:

	MySleep(10);

	bRet = (pUsbHid->SetTouchConfigEx(REG_TSM_MODULE_ID, nModuleID) != 0) ? FALSE : TRUE;
	if (bRet == FALSE)
	{
		if (nRetry < 5)
		{
			nRetry++;
			goto SET_MODULE_ID_RETRY;
		}
		pUsbHid->m_strRet.Format("[ERROR] FAIL REG_TSM_MODULE_ID(0x1E)");
		return FALSE;
	}

	nRetry = 0;

SAVE_STATUS_CMD_RETRY:

	MySleep(10);

	bRet = (pUsbHid->SetTouchCommandEx(CMD_SAVE_REGISTER) != 0) ? FALSE : TRUE;
	if (bRet == FALSE)
	{
		if (nRetry < 5)
		{
			nRetry++;
			goto SAVE_STATUS_CMD_RETRY;
		}
		pUsbHid->m_strRet.Format("[ERROR] FAIL CMD_SAVE_REGISTER");
		return FALSE;
	}

	MySleep(1500);

	// ZT75XX_VCMD_WRITE_ENABLE(0x00)
	if (pUsbHid->SetTouchConfigEx(ZT_VCMD(VCMD_NVM_WRITE_ENABLE), 0x0) != 0)
	{
		pUsbHid->m_strRet.Format("[ERROR] FAIL  ZT75XX_VCMD_WRITE_ENABLE(0x0)");
		bRet = FALSE;
	}
	MySleep(10);
	if (pUsbHid->SetTouchCommandEx(CMD_CLEAR_INTERRUPT) != 0)
	{
		pUsbHid->m_strRet.Format("[ERROR] FAIL CMD_CLEAR_INTERRUPT");
		bRet = FALSE;
	}

	return bRet;
}

BOOL CTestModeView::TSM_SaveIUM_Erase(CUsbHid * pUsbHid)
{
#if 0 // Erase command가 동작하지 안음. 확인 필요.
	MySleep(100);

	pUsbHid->SetTouchConfigEx(0xc000, 0x0001);
	MySleep(100);

	pUsbHid->SetTouchConfigEx(0xc003, 0x0001);
	MySleep(100);

	if (m_Comm.SetTouchCommand(ZT_VCMD(VCMD_IUM_ERASE)) != 0) {
		err_msg.Format("[ERROR] FAIL TO WRITE SEND SAVE IUM COMMAND");
		goto error_send_EraseIUM;
	}
	MySleep(300);

	err_msg.Format("[WARNING] Erase IUM Buffer.");
	SendMessage(WM_USER + 300, (WPARAM)0, (LPARAM)&err_msg);
	for (int i = 0; i < 10; i++) {
		MySleep(100);
		SendMessage(WM_USER + 300, (WPARAM)i * 10 / 100, (LPARAM)&err_msg);
	}
#else
	for (int i = 0; i < g_FwDownloadInfo.icIumSize / 2; i++) // 128byte 모두 클리어 필요. 2byte 통신이기 때문에 /2 필요.
	{
		if (pUsbHid->SetTouchConfigEx(pUsbHid->m_tIumCmdInfo.nIumCmdCustomAddr + i, 0) != 0) {
			goto error_send_EraseIUM;
		}
	}
#endif

	return TRUE;

error_send_EraseIUM:
	return FALSE;
}

BOOL CTestModeView::TSM_SaveIUM_Save(CUsbHid * pUsbHid)
{
	char strDeviceID[128];
	u16 i, nCustomerDataPos;

	if (m_nIUMFormat == 0)
		nCustomerDataPos = 4;
	else
		nCustomerDataPos = 7;

#if (0)//#ifdef _DEBUG for IUM
	memset(&pUsbHid->m_IumData[0], 0x00, sizeof(unsigned short int)*64);

	if (m_nIUMFormat == 0)
	{
		BYTE info[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
		memcpy(&pUsbHid->m_IumData[0], info, 8);
	}
	else
	{
		BYTE info[14] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
		memcpy(&pUsbHid->m_IumData[0], info, 14);
	}
	#else
	// Set IUM data
	pUsbHid->m_IumData[nCustomerDataPos++] = pUsbHid->m_CapInfo.icRegData[REG_HW_ID];
	pUsbHid->m_IumData[nCustomerDataPos++] = pUsbHid->m_CapInfo.icRegData[REG_CHIP_CODE];

	pUsbHid->m_IumData[nCustomerDataPos++] = Str_to_Hex((unsigned char*)m_strProjectID.GetBuffer(), 4);

	sprintf(strDeviceID, "%s", m_strDeviceID.GetBuffer());

	for (i = 0; i < 4; i++) //point1
	{
		pUsbHid->m_IumData[nCustomerDataPos++] = Str_to_Hex((unsigned char*)&strDeviceID[i * 4], 4);
	}
	#endif

	memcpy(&pUsbHid->m_IumData[0x20 / 2], &pUsbHid->m_IumData[0], 0x20);
	memcpy(&pUsbHid->m_IumData[0x40 / 2], &pUsbHid->m_IumData[0], 0x20);
	memcpy(&pUsbHid->m_IumData[0x60 / 2], &pUsbHid->m_IumData[0], 0x20);
	//-----------------------------------------
	// don't delete. for DMA_RX_ENABLE
	//if (TSM_SaveIUM_Enter(pUsbHid) == FALSE)
	//	return FALSE;
	//MyDelayMs(1);
	//-----------------------------------------

	// Save IUM
	for (i = 0; i < g_FwDownloadInfo.icIumSize / 2; i++) // 128byte 모두 클리어 필요. 2byte 통신이기 때문에 /2 필요.
	{
		if (pUsbHid->SetTouchConfigEx(pUsbHid->m_tIumCmdInfo.nIumCmdCustomAddr + (i * 2), pUsbHid->m_IumData[i]) != 0) {
			return FALSE;
		}
	}
	MyDelayMs(100);

	if (pUsbHid->SetTouchConfigEx(ZT_VCMD(VCMD_NVM_WRITE_ENABLE), 0x1) != 0){
		return FALSE;
	}
	MyDelayMs(10);

	//-----------------------------------------
	//Erase IUM for ZTM730
	if (pUsbHid->SetTouchCommand(ZT_VCMD(VCMD_IUM_ERASE)) != 0) {
		return FALSE;
	}
	//-----------------------------------------

	if (pUsbHid->SetTouchCommandEx(pUsbHid->m_tIumCmdInfo.nIumCmdSave) != 0) {
		return FALSE;
	}
	MyDelayMs(1);

	if (pUsbHid->SetTouchConfigEx(ZT_VCMD(VCMD_NVM_WRITE_ENABLE), 0x0) != 0){
		return FALSE;
	}
	MyDelayMs(10);
	return TRUE;
}

BOOL CTestModeView::TSM_SaveIUM_Enter(CUsbHid * pUsbHid)
{
	if (pUsbHid->SetTouchCommandEx(pUsbHid->m_tIumCmdInfo.nIumCmdEnter) != 0)
	{
		goto error_send_enter_ium;
	}

	return TRUE;

error_send_enter_ium:
	return FALSE;
}

BOOL CTestModeView::TSM_SaveIUM_Exit(CUsbHid * pUsbHid)
{
	if (pUsbHid->SetTouchCommandEx(pUsbHid->m_tIumCmdInfo.nIumCmdExit) != 0)
	{
		goto error_send_exit_ium;
	}

	return TRUE;

error_send_exit_ium:
	return FALSE;
}

BOOL CTestModeView::TSM_SaveIUM_Verify(CUsbHid * pUsbHid)
{
	// Read IUM
	if (pUsbHid->GetTouchRegReadCmd(ZT_VCMD(VCMD_IUM_READ_DATA), (BYTE*)pUsbHid->m_IumData_Verify, g_FwDownloadInfo.icIumSize) != 0)
	{
		return FALSE;
	}
	//SSY
	if (memcmp((BYTE*)pUsbHid->m_IumData, (BYTE*)pUsbHid->m_IumData_Verify, g_FwDownloadInfo.icIumSize) != 0)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CTestModeView::TSM_SaveIUM(CUsbHid * pUsbHid)
{
	BOOL bRet = FALSE;

	pUsbHid->m_tIumCmdInfo = ZT_GET_IUM_CMD_INFO(pUsbHid->m_CapInfo.icSignature);

	//SemaWait(g_hSemaphore);

	pUsbHid->SetTouchConfigEx(ZT_VCMD(VCMD_ENABLE), 0x0001);
	MySleep(100);

	// Read IUM
	if (pUsbHid->GetTouchRegReadCmd(ZT_VCMD(VCMD_IUM_READ_DATA), (BYTE*)pUsbHid->m_IumData, g_FwDownloadInfo.icIumSize) != 0)
	{
		goto error_SaveInformation;
	}
	MyDelayMs(100);

	// Read IC Info
	if (pUsbHid->GetTouchConfigEx(REG_HW_ID, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_HW_ID]) != 0)
		return FALSE;
	MyDelayMs(1);

	if (pUsbHid->GetTouchConfigEx(REG_CHIP_CODE, (BYTE *)&pUsbHid->m_CapInfo.icRegData[REG_CHIP_CODE]) != 0)
		return FALSE;
	MyDelayMs(1);

	if (TSM_SaveIUM_Enter(pUsbHid) == FALSE)
	{
		goto error_SaveInformation;
	}
	MyDelayMs(1);

	if (TSM_SaveIUM_Erase(pUsbHid) == FALSE)
	{
		goto error_SaveInformation;
	}
	MyDelayMs(1);

	if (TSM_SaveIUM_Save(pUsbHid) == FALSE)
	{
		goto error_SaveInformation;
	}
	MyDelayMs(1);

	if (TSM_SaveIUM_Exit(pUsbHid) == FALSE)
	{
		goto error_SaveInformation;
	}
	MyDelayMs(10);

	if (TSM_SaveIUM_Verify(pUsbHid) == FALSE)
	{
		goto error_SaveInformation;
	}
	MyDelayMs(1);

	//::ReleaseSemaphore(g_hSemaphore, 1, NULL);
	//	AfxMessageBox("FINISHED SAVE INFORMATION.");

	bRet = TRUE;

error_SaveInformation:

	TSM_SaveIUM_Exit(pUsbHid);

	pUsbHid->SetTouchConfigEx(ZT_VCMD(VCMD_ENABLE), 0x0000);
	MySleep(100);

	//::ReleaseSemaphore(g_hSemaphore, 1, NULL);
	return bRet;
}

BOOL CTestModeView::TSM_VerifyIUM(CUsbHid * pUsbHid)
{
	char strDeviceID[128];
	u16 i, nCustomerDataPos;

	memset(&pUsbHid->m_IumData_Verify[0], 0x00, sizeof(unsigned short int)*64);

	// Read IUM
	if (pUsbHid->GetTouchRegReadCmd(ZT_VCMD(VCMD_IUM_READ_DATA), (BYTE*)pUsbHid->m_IumData, g_FwDownloadInfo.icIumSize) != 0)
	{
		return FALSE;
	}

	// Set verify IUM data
	memcpy(&pUsbHid->m_IumData_Verify[0], &pUsbHid->m_IumData[0], 8);

	if (m_nIUMFormat == 0)
		nCustomerDataPos = 4;
	else
		nCustomerDataPos = 7;

	// Set IUM data
	pUsbHid->m_IumData_Verify[nCustomerDataPos++] = pUsbHid->m_CapInfo.icRegData[REG_HW_ID];
	pUsbHid->m_IumData_Verify[nCustomerDataPos++] = pUsbHid->m_CapInfo.icRegData[REG_CHIP_CODE];

	pUsbHid->m_IumData_Verify[nCustomerDataPos++] = Str_to_Hex((unsigned char*)m_strProjectID.GetBuffer(), 4);

	sprintf(strDeviceID, "%s", m_strDeviceID.GetBuffer());

	for (i = 0; i < 4; i++) //point2
	{
		pUsbHid->m_IumData_Verify[nCustomerDataPos++] = Str_to_Hex((unsigned char*)&strDeviceID[i * 4], 4);
	}

	memcpy(&pUsbHid->m_IumData_Verify[0x20 / 2], &pUsbHid->m_IumData_Verify[0], 0x20);
	memcpy(&pUsbHid->m_IumData_Verify[0x40 / 2], &pUsbHid->m_IumData_Verify[0], 0x20);
	memcpy(&pUsbHid->m_IumData_Verify[0x60 / 2], &pUsbHid->m_IumData_Verify[0], 0x20);

	if (memcmp((BYTE*)pUsbHid->m_IumData, (BYTE*)pUsbHid->m_IumData_Verify, g_FwDownloadInfo.icIumSize) != 0)
	{
		return FALSE;
	}

	return TRUE;
}

//==============================================================================================
// LOG & LOGGING FILE
//==============================================================================================

void CTestModeView::TSM_SaveLogFile(CUsbHid * pTestEndUsbHid)
{
	FILE * pfLog = NULL;

	CString strLogFile;
	CString strFileDir;
	CString strFileDate;
	CString str;
	CString Barcode_Name;

	if (pTestEndUsbHid == NULL)
		return;

#if defined(D_DEBUG_FW_UPGRADE_TEST)
	if (pTestEndUsbHid->m_bTestResult)
		return;
#endif

	int nYear = pTestEndUsbHid->m_OleDateTime.GetYear();
	int nMonth = pTestEndUsbHid->m_OleDateTime.GetMonth();
	int nDay = pTestEndUsbHid->m_OleDateTime.GetDay();
	int nHour = pTestEndUsbHid->m_OleDateTime.GetHour();
	int nMinute = pTestEndUsbHid->m_OleDateTime.GetMinute();
	int nSecond = pTestEndUsbHid->m_OleDateTime.GetSecond();

	if (pTestEndUsbHid->IsCoreFrameWork() == TRUE && m_nModuleVendorCode == MODULE_VENDOR_VISIONOX)
	{
		//2021.11.17 김극래 책임 VISIONOX 일경우 Log Namming rule 변경.
		CString strDirPass , strDirFail;
		// Create Directory
		strDirPass.Format("%s\\Pass", m_strSaveResultDir);
		if (PathFileExists(strDirPass) == FALSE && CreateDirectory((LPCTSTR)strDirPass, NULL) == FALSE)
		{
			str.Format("Cannot create directory (%s)", strDirPass);
			AfxMessageBox(str);
			return;
		}
		strDirPass.AppendFormat("\\%04d-%02d-%02d", nYear, nMonth, nDay);
		if (PathFileExists(strDirPass) == FALSE && CreateDirectory((LPCTSTR)strDirPass, NULL) == FALSE)
		{
			str.Format("Cannot create directory (%s)", strDirPass);
			AfxMessageBox(str);
			return;
		}

		strDirFail.Format("%s\\Fail", m_strSaveResultDir);
		if (PathFileExists(strDirFail) == FALSE && CreateDirectory((LPCTSTR)strDirFail, NULL) == FALSE)
		{
			str.Format("Cannot create directory (%s)", strDirFail);
			AfxMessageBox(str);
			return;
		}
		strDirFail.AppendFormat("\\%04d-%02d-%02d", nYear, nMonth, nDay);
		if (PathFileExists(strDirFail) == FALSE && CreateDirectory((LPCTSTR)strDirFail, NULL) == FALSE)
		{
			str.Format("Cannot create directory (%s)", strDirFail);
			AfxMessageBox(str);
			return;
		}

		strFileDir = (pTestEndUsbHid->m_bTestResult) ? strDirPass : strDirFail;
	}
	else if (pTestEndUsbHid->IsCoreFrameWork() == TRUE && m_nModuleVendorCode == MODULE_VENDOR_AUO && m_bAdditionalLOG && m_bCreatePassFailDirectory)
	{
		CString strDirPass , strDirFail;
		// Create Directory
		strFileDir.Format("%s\\SAVE_LOG", m_strSaveResultDir);
		if (PathFileExists(strFileDir) == FALSE && CreateDirectory((LPCTSTR)strFileDir, NULL) == FALSE)
		{
			str.Format("Cannot create directory (%s)", strFileDir);
			AfxMessageBox(str);
			return;
		}

		strFileDir.AppendFormat("\\%04d-%02d-%02d", nYear, nMonth, nDay);
		if (PathFileExists(strFileDir) == FALSE && CreateDirectory((LPCTSTR)strFileDir, NULL) == FALSE)
		{
			str.Format("Cannot create directory (%s)", strFileDir);
			AfxMessageBox(str);
			return;
		}

		if(pTestEndUsbHid->m_bTestResult)
		{
			strDirPass.Format("%s\\PASS", strFileDir);
			if (PathFileExists(strDirPass) == FALSE && CreateDirectory((LPCTSTR)strDirPass, NULL) == FALSE)
			{
				str.Format("Cannot create directory (%s)", strDirPass);
				AfxMessageBox(str);
				return;
			}

			strFileDir = strDirPass;
		}
		else
		{
			strDirFail.Format("%s\\FAIL", strFileDir);
			if (PathFileExists(strDirFail) == FALSE && CreateDirectory((LPCTSTR)strDirFail, NULL) == FALSE)
			{
				str.Format("Cannot create directory (%s)", strDirFail);
				AfxMessageBox(str);
				return;
			}

			strFileDir = strDirFail;
		}
	}
	else
	{
		// Create Directory
		if (m_bAdditionalLOG)
			strFileDir.Format("%s\\SAVE_LOG", m_strSaveResultDir);
		else
			strFileDir.Format("%s\\DATA\\%02d%02d%02d", m_strSaveResultDir, nYear % 100, nMonth, nDay);

		if (PathFileExists(strFileDir) == FALSE && CreateDirectory((LPCTSTR)strFileDir, NULL) == FALSE)
		{
			str.Format("Cannot create directory (%s)", strFileDir);
			AfxMessageBox(str);
			return;
		}

		if (m_bCreatePassFailDirectory)
		{
			if(m_nModuleVendorCode == MODULE_VENDOR_AUO)
				strFileDir.AppendFormat("\\%s", (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
			else
				strFileDir.AppendFormat("\\%s", (pTestEndUsbHid->m_bTestResult) ? "PassLog" : "FailLog");

			if (PathFileExists(strFileDir) == FALSE && CreateDirectory((LPCTSTR)strFileDir, NULL) == FALSE)
			{
				str.Format("Cannot create directory (%s)", strFileDir);
				AfxMessageBox(str);
				return;
			}
		}
	}

	//---------------------------------------------------------------------------------------------------
	// Create LOG file - 중국이 사용하는 모든 case
	if (pTestEndUsbHid->IsChinaCustomer())
	{
		Barcode_Name = pTestEndUsbHid->m_strBarcode;
		Barcode_Name.Replace('*', '#');
		// CHINA
		strFileDate.Format("%02d%02d%02d-%02d%02d-%02d", nYear % 100, nMonth, nDay, nHour, nMinute, nSecond);

		if (m_nModuleVendorCode == MODULE_VENDOR_TRULY)
		{
			if (m_bAdditionalLOG)
			{
				strFileDate.Format("%02d%02d%02d_%02d%02d%02d", nYear % 100, nMonth, nDay, nHour, nMinute, nSecond);

				if (m_strProjectName.IsEmpty())
					m_strProjectName = "EMPTY";

				if (m_bUseBarcode == TRUE)
				{
					strLogFile.Format("%s\\%s-%s-%s-%s-%s-%s-%s.csv", (LPCTSTR)strFileDir, Barcode_Name , (LPCTSTR)strFileDate, pTestEndUsbHid->m_cAsicID_0,
					(LPCTSTR)m_strCustomer, (LPCTSTR)m_strProjectName, (LPCTSTR)m_strTestStage, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
				}
				else
					strLogFile.Format("%s\\%s-%s-%s-%s-%s-%s.csv", (LPCTSTR)strFileDir, (LPCTSTR)strFileDate, pTestEndUsbHid->m_cAsicID_0,
					(LPCTSTR)m_strCustomer, (LPCTSTR)m_strProjectName, (LPCTSTR)m_strTestStage, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
					CheckSameLogFile(strFileDir, m_strTestName, pTestEndUsbHid->m_cAsicID_0, strLogFile);
			}
			else
			{
				if (m_bUseBarcode == TRUE)
				{
					strLogFile.Format("%s\\%s-%s-DEV %d-(%s).csv", (LPCTSTR)strFileDir, Barcode_Name , (LPCTSTR)strFileDate, pTestEndUsbHid->m_nDevIndex, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
				}
				else
					strLogFile.Format("%s\\%s-DEV %d-(%s).csv", (LPCTSTR)strFileDir, (LPCTSTR)strFileDate, pTestEndUsbHid->m_nDevIndex, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
			}
		}
		else if (m_nModuleVendorCode == MODULE_VENDOR_JING_ZHUO)
		{
			if (m_bUseBarcode == TRUE)
			{
				strLogFile.Format("%s\\%s-%s-DEV %d-%s.csv", (LPCTSTR)strFileDir, Barcode_Name, (LPCTSTR)strFileDate, pTestEndUsbHid->m_nDevIndex, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
			}
			else
				strLogFile.Format("%s\\%s-DEV %d-%s.csv", (LPCTSTR)strFileDir, (LPCTSTR)strFileDate, pTestEndUsbHid->m_nDevIndex, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
		}
		else
		{
			if (m_bUseBarcode == TRUE)
			{
				strLogFile.Format("%s\\%s-%s-DEV %d-(%s).csv", (LPCTSTR)strFileDir, Barcode_Name, (LPCTSTR)strFileDate, pTestEndUsbHid->m_nDevIndex, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
			}
			else
				strLogFile.Format("%s\\%s-DEV %d-(%s).csv", (LPCTSTR)strFileDir, (LPCTSTR)strFileDate, pTestEndUsbHid->m_nDevIndex, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
		}
	}
	else
	{	// IFE
		if(m_nModuleVendorCode == MODULE_VENDOR_VISIONOX)
		{
#if(1)
			/*
			* 2021.11.17 김극래 책임 VISIONOX 일경우 Log Naming rule 변경.
			* VISIONOX 일 경우 m_strProjectName.IsEmpty() 이부분을 무시 하도록 한다.
			*
			- MODULE VENDOR 가 Visionox 일 경우에만 적용
				- Additional Log IFE 와 상관 없이 강제 활성화.

				- [CREATE PASS/FAIL DIRECTORY] 체크 시 폴더 구조 변경
					- 강제로 Checked = 1 로 만든다.

				- [Additional Log Content]
					- 강제로 Checked = 1 로 만든다.

				- PASS/년월일/file_name_rule, FAIL/년월일/file_name_rule

				- FIXED POSITION 체크 여부에 따라서 달라지는 사양은 없음.

				- [Additional Log Content] 체크 여부에 따른 file naming rule
					- 강제로 Checked = 1 됨 : Barcode_time_TestName(can be edit)-Test Stage(can be edit)-MouleMaker(can be edit)_PASS/Fail.CSV
			*/
			//strFileDate.Format("%02d%02d%02d-%02d%02d%02d", nYear % 100, nMonth, nDay, nHour, nMinute, nSecond);
			strFileDate.Format("%04d%02d%02d%02d%02d%02d", nYear, nMonth, nDay, nHour, nMinute, nSecond);

			if (m_bAdditionalLOG)	//m_bAdditionalLOG = true 강제로 앞에서 설정된다.
			{// FOR_customer
				if (m_bUseBarcode == TRUE)
					strLogFile.Format("%s\\%s_%s_%s_%s_%s_%s.csv", (LPCTSTR)strFileDir, pTestEndUsbHid->m_strBarcode, (LPCTSTR)strFileDate,
									(LPCTSTR)m_strTestName ,(LPCTSTR)m_strTestStage, (LPCTSTR)m_strModuleMaker , (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
				else
					strLogFile.Format("%s\\%s_%s_%s_%s_%s.csv", (LPCTSTR)strFileDir, (LPCTSTR)strFileDate,
									(LPCTSTR)m_strTestName ,(LPCTSTR)m_strTestStage, (LPCTSTR)m_strModuleMaker , (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
			}
#else
			if (m_strProjectName.IsEmpty())
			{
				if (m_bUseBarcode == TRUE)
					strLogFile.Format("%s\\%s-%s-#%d-%s.csv", (LPCTSTR)strFileDir, pTestEndUsbHid->m_strBarcode, (LPCTSTR)strFileDate, pTestEndUsbHid->m_nDevIndex, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
				else
					strLogFile.Format("%s\\LOG-%s-#%d-%s.csv", (LPCTSTR)strFileDir, (LPCTSTR)strFileDate, pTestEndUsbHid->m_nDevIndex, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
			}
			else
			{
				if (m_bAdditionalLOG)
				{// FOR_customer
					if (m_bUseBarcode == TRUE)
						strLogFile.Format("%s\\%s-%s-%s-%s-%s-%s-%s.csv", (LPCTSTR)strFileDir, pTestEndUsbHid->m_strBarcode, (LPCTSTR)strFileDate, pTestEndUsbHid->m_cAsicID_0,
										(LPCTSTR)m_strCustomer, (LPCTSTR)m_strProjectName, (LPCTSTR)m_strTestStage, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
					else
						strLogFile.Format("%s\\%s-%s-%s-%s-%s-%s.csv", (LPCTSTR)strFileDir, (LPCTSTR)strFileDate, pTestEndUsbHid->m_cAsicID_0,
										(LPCTSTR)m_strCustomer, (LPCTSTR)m_strProjectName, (LPCTSTR)m_strTestStage, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
				}
				else
				{
					if (m_bUseBarcode == TRUE)
						strLogFile.Format("%s\\%s-%s-%s-#%d-%s.csv", (LPCTSTR)strFileDir, pTestEndUsbHid->m_strBarcode, (LPCTSTR)m_strProjectName, (LPCTSTR)strFileDate, pTestEndUsbHid->m_nDevIndex, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
					else
						strLogFile.Format("%s\\LOG-%s-%s-#%d-%s.csv", (LPCTSTR)strFileDir, (LPCTSTR)m_strProjectName, (LPCTSTR)strFileDate, pTestEndUsbHid->m_nDevIndex, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
				}
			}
#endif
		}
		else
		{
			strFileDate.Format("%02d%02d%02d_%02d%02d%02d", nYear % 100, nMonth, nDay, nHour, nMinute, nSecond);

			if (m_strProjectName.IsEmpty())
			{
				if (m_bUseBarcode == TRUE)
					strLogFile.Format("%s\\%s_%s_#%d_%s.csv", (LPCTSTR)strFileDir, pTestEndUsbHid->m_strBarcode, (LPCTSTR)strFileDate, pTestEndUsbHid->m_nDevIndex, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
				else
					strLogFile.Format("%s\\LOG_%s_#%d_%s.csv", (LPCTSTR)strFileDir, (LPCTSTR)strFileDate, pTestEndUsbHid->m_nDevIndex, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
			}
			else
			{
				if (m_bAdditionalLOG)
				{// FOR_customer
					if (m_bUseBarcode == TRUE && m_nModuleVendorCode == MODULE_VENDOR_TRULY)
		           {
						strLogFile.Format("%s\\%s_%s_%s_%s_%s_%s_%s_%s_%s_%s.csv", (LPCTSTR)strFileDir, pTestEndUsbHid->m_strBarcode, (LPCTSTR)m_strTestName, pTestEndUsbHid->m_cAsicID_0,
							(LPCTSTR)m_strCustomer, (LPCTSTR)m_strProjectName, (LPCTSTR)m_strTestStage, (LPCTSTR)m_strModuleMaker, (LPCTSTR)strFileDate,
							(m_nInOutType == 0) ? "IN": "OUT", (pTestEndUsbHid->m_bTestResult) ? "PASS": "FAIL");
		           }
					else if (m_bUseBarcode == TRUE &&  m_nModuleVendorCode == MODULE_VENDOR_AUO)
		           {
						strLogFile.Format("%s\\%s_%s_%s_%s_%s_%s_%s_%s_%s_%s.csv", (LPCTSTR)strFileDir, pTestEndUsbHid->m_strBarcode, (pTestEndUsbHid->m_bTestResult) ? "PASS": "FAIL" ,(LPCTSTR)strFileDate,
							(LPCTSTR)m_strTestName, pTestEndUsbHid->m_cAsicID_0,  (LPCTSTR)m_strCustomer, (LPCTSTR)m_strProjectName, (LPCTSTR)m_strTestStage, (LPCTSTR)m_strModuleMaker,
							(m_nInOutType == 0) ? "IN": "OUT");
		           }
		           else
		           { //p1
						strLogFile.Format("%s\\%s_%s_%s_%s_%s_%s_%s_%s_%s.csv", (LPCTSTR)strFileDir, (LPCTSTR)m_strTestName, pTestEndUsbHid->m_cAsicID_0,
							(LPCTSTR)m_strCustomer, (LPCTSTR)m_strProjectName, (LPCTSTR)m_strTestStage, (LPCTSTR)m_strModuleMaker, (LPCTSTR)strFileDate,
							(m_nInOutType == 0) ? "IN" : "OUT", (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
           			}
				}
				else
				{
					if (m_bUseBarcode == TRUE)
						strLogFile.Format("%s\\%s_%s_%s_#%d_%s.csv", (LPCTSTR)strFileDir, pTestEndUsbHid->m_strBarcode, (LPCTSTR)m_strProjectName, (LPCTSTR)strFileDate, pTestEndUsbHid->m_nDevIndex, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
					else
						strLogFile.Format("%s\\LOG_%s_%s_#%d_%s.csv", (LPCTSTR)strFileDir, (LPCTSTR)m_strProjectName, (LPCTSTR)strFileDate, pTestEndUsbHid->m_nDevIndex, (pTestEndUsbHid->m_bTestResult) ? "PASS" : "FAIL");
				}
			}
		}

		if (m_bAdditionalLOG)
		{
			CheckSameLogFile(strFileDir, m_strTestName, pTestEndUsbHid->m_cAsicID_0, strLogFile);
		}
	}

	pfLog = fopen(strLogFile, "a+b");
	if (pfLog == NULL)
	{
		str.Format("Cannot create Log File (%s)", strLogFile);
		AfxMessageBox(str);
		return;
	}

	if (m_bAdditionalLOG)
	{
		fputs((LPCTSTR)"\r\n", pfLog);
		fputs((LPCTSTR)"\r\n", pfLog);
		fputs((LPCTSTR)"========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t\r\n", pfLog);
	}

	str.Format("%s,\t\t%04d/%02d/%02d-%02d:%02d:%02d\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "FILE_DATE"), nYear, nMonth, nDay, nHour, nMinute, nSecond);
	fputs((LPCTSTR)str, pfLog);
	str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "ZTD_VERSION"), g_strZtdVersion);
	fputs((LPCTSTR)str, pfLog);

	if (m_bCheckUserNumber == TRUE)
	{
		str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "USER_NUMBER"), m_strUserNumber);
		fputs((LPCTSTR)str, pfLog);
	}

	if (m_strCustomer.IsEmpty())
		str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "CUSTOMER"), "NULL");
	else
		str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "CUSTOMER"), m_strCustomer);
	fputs((LPCTSTR)str, pfLog);

	if (m_strProjectName.IsEmpty())
		str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "PROJECT_NAME"), "NULL");
	else
		str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "PROJECT_NAME"), m_strProjectName);
	fputs((LPCTSTR)str, pfLog);

	if (m_bAdditionalLOG)
	{
		if (m_strTestName.IsEmpty())
			str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "TEST_NAME"), "NULL");
		else
			str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "TEST_NAME"), m_strTestName);
		fputs((LPCTSTR)str, pfLog);

		if (m_strTestStage.IsEmpty())
			str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "TEST_STAGE"), "NULL");
		else
			str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "TEST_STAGE"), m_strTestStage);
		fputs((LPCTSTR)str, pfLog);

		if (m_strModuleMaker.IsEmpty())
			str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "MODULE_MAKER"), "NULL");
		else
			str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "MODULE_MAKER"), m_strModuleMaker);
		fputs((LPCTSTR)str, pfLog);

		if (m_nInOutType == 0)
			str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "SELECT_IQC_OQC"), "IN");
		else
			str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "SELECT_IQC_OQC"), "OUT");
		fputs((LPCTSTR)str, pfLog);

		if (strcmp(pTestEndUsbHid->m_cAsicID_0, STRING_INVALID_ASIC_ID) == 0)
			str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "ASIC_ID"), "NULL");
		else
			str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "ASIC_ID"), pTestEndUsbHid->m_cAsicID_0);
		fputs((LPCTSTR)str, pfLog);
	}

	if (m_bUseBarcode)
	{
		if (pTestEndUsbHid->m_strBarcode.IsEmpty())
			str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "BARCODE"), "ERROR");
		else
			str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "BARCODE"), pTestEndUsbHid->m_strBarcode);

		fputs((LPCTSTR)str, pfLog);
	}

	fputs((LPCTSTR)"\r\n", pfLog);

	//=============================================================
	// Test Result
	fputs((LPCTSTR)pTestEndUsbHid->m_strLog, pfLog);

	fputs((LPCTSTR)"\r\n", pfLog);

	//-----------------------------------------------
	// Line number
	if (pTestEndUsbHid->IsCoreFrameWork() == FALSE)
	{ // CHINA
		str.Format("%s,\t\t,\t\t%s,\t\t%d,\t\t%d,\t\t%d,\t\t%d,\t\t%d\r\n",
					hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "LINE_NUMBER"), pTestEndUsbHid->m_strDevName,
					pTestEndUsbHid->m_nTotalNumberX, pTestEndUsbHid->m_nTotalNumberY, pTestEndUsbHid->m_nSelfRowCount,
					pTestEndUsbHid->m_IC_Max_X_Line_Num, pTestEndUsbHid->m_IC_Max_Y_Line_Num);
	}
	else
	{ // IFE
		str.Format("%s,\t\t,\t\t%s,\t\t%d,\t\t%d,\t\t%d\r\n",
					hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "LINE_NUMBER"), pTestEndUsbHid->m_strDevName,
					pTestEndUsbHid->m_nTotalNumberX, pTestEndUsbHid->m_nTotalNumberY, pTestEndUsbHid->m_nSelfRowCount);
	}

	fputs((LPCTSTR)str, pfLog);
	fputs((LPCTSTR)"\r\n", pfLog);

		//LDO OUT
		/*
	str.Format("%s,\t\t,\t\t%s,\t\t%d,\t\t%d,\t\t%d\r\n",
					hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "voltage"), pTestEndUsbHid->m_strDevName,
					m_vih_ref,g_vcca_ref,g_voltage);

		fputs((LPCTSTR)str, pfLog);
	fputs((LPCTSTR)"\r\n", pfLog);
	*/

	str.Format("%s,\t\t,\t\tDEV_%d,\t\t,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X",
		hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "AFE_FREQ"), pTestEndUsbHid->m_nDevIndex,
		pTestEndUsbHid->m_AfeValue[TEST_DND].val_Frequency,
		pTestEndUsbHid->m_AfeValue[TEST_HF_DND].val_Frequency,
		pTestEndUsbHid->m_AfeValue[TEST_SELF_DND].val_Frequency,
		pTestEndUsbHid->m_AfeValue[TEST_SELF_DND].val_Frequency_B,
		pTestEndUsbHid->m_AfeValue[TEST_SELF_SATURATION].val_Frequency,
		pTestEndUsbHid->m_AfeValue[TEST_SELF_SATURATION].val_Frequency_B,
		pTestEndUsbHid->m_AfeValue[TEST_JITTER].val_Frequency,
		pTestEndUsbHid->m_AfeValue[TEST_HW_CALIBRATION].val_Frequency,
		pTestEndUsbHid->m_AfeValue[TEST_MEASURE_DELTA].val_Frequency);
	str.AppendFormat(",\t\t,\t\t[DND],\t\t[HF_DND],\t\t[SELF_DND_A],\t\t[SELF_DND_B],\t\t[SELF_SATURATION_A],\t\t[SELF_SATURATION_B],\t\t[JITTER],\t\t[HW_CALIBRATION],\t\t[MEASURE_DELTA]\r\n");
	fputs((LPCTSTR)str, pfLog);

	str.Format("%s,\t\t,\t\tDEV_%d,\t\t,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X",
		hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "AFE_U_CNT"), pTestEndUsbHid->m_nDevIndex,
		pTestEndUsbHid->m_AfeValue[TEST_DND].val_Ucont,
		pTestEndUsbHid->m_AfeValue[TEST_HF_DND].val_Ucont,
		pTestEndUsbHid->m_AfeValue[TEST_SELF_DND].val_Ucont,
		pTestEndUsbHid->m_AfeValue[TEST_SELF_DND].val_Ucont_B,
		pTestEndUsbHid->m_AfeValue[TEST_SELF_SATURATION].val_Ucont,
		pTestEndUsbHid->m_AfeValue[TEST_SELF_SATURATION].val_Ucont_B,
		pTestEndUsbHid->m_AfeValue[TEST_JITTER].val_Ucont,
		pTestEndUsbHid->m_AfeValue[TEST_HW_CALIBRATION].val_Ucont,
		pTestEndUsbHid->m_AfeValue[TEST_MEASURE_DELTA].val_Ucont);
	str.AppendFormat(",\t\t,\t\t[DND],\t\t[HF_DND],\t\t[SELF_DND_A],\t\t[SELF_DND_B],\t\t[SELF_SATURATION_A],\t\t[SELF_SATURATION_B],\t\t[JITTER],\t\t[HW_CALIBRATION],\t\t[MEASURE_DELTA]\r\n");
	fputs((LPCTSTR)str, pfLog);

	str.Format("%s,\t\t,\t\tDEV_%d,\t\t,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X",
		hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "AFE_N_CNT"), pTestEndUsbHid->m_nDevIndex,
		pTestEndUsbHid->m_AfeValue[TEST_DND].val_Ncount,
		pTestEndUsbHid->m_AfeValue[TEST_HF_DND].val_Ncount,
		pTestEndUsbHid->m_AfeValue[TEST_SELF_DND].val_Ncount,
		pTestEndUsbHid->m_AfeValue[TEST_SELF_DND].val_Ncount_B,
		pTestEndUsbHid->m_AfeValue[TEST_SELF_SATURATION].val_Ncount,
		pTestEndUsbHid->m_AfeValue[TEST_SELF_SATURATION].val_Ncount_B,
		pTestEndUsbHid->m_AfeValue[TEST_JITTER].val_Ncount,
		pTestEndUsbHid->m_AfeValue[TEST_HW_CALIBRATION].val_Ncount,
		pTestEndUsbHid->m_AfeValue[TEST_MEASURE_DELTA].val_Ncount);
	str.AppendFormat(",\t\t,\t\t[DND],\t\t[HF_DND],\t\t[SELF_DND_A],\t\t[SELF_DND_B],\t\t[SELF_SATURATION_A],\t\t[SELF_SATURATION_B],\t\t[JITTER],\t\t[HW_CALIBRATION],\t\t[MEASURE_DELTA]\r\n");
	fputs((LPCTSTR)str, pfLog);

	if(m_bMeasureLdo || m_bCheckI2C_Level == TRUE)
	{
		fputs((LPCTSTR)"\r\n", pfLog);

		str.Format("%s,\t\t,\t\tDEV_%d,\t\t,\t\t%.2fV,\t\t%.2fV,\t\t%.2fV",
			hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, "LDO_VOLTAGE"), pTestEndUsbHid->m_nDevIndex, pTestEndUsbHid->m_fVoltageVcc, pTestEndUsbHid->m_fVoltageVi2c, pTestEndUsbHid->m_fVIH_1);
		str.AppendFormat(",\t\t,\t\t[VCC],\t\t[VIC2],\t\t[VIH]\r\n");
		fputs((LPCTSTR)str, pfLog);
	}

	fputs((LPCTSTR)"\r\n", pfLog);

	//=============================================================
	// Display error description
	if (pTestEndUsbHid->m_nResult == RESULT_FAIL)
	{
		BOOL bTestFail = FALSE;

		fputs((LPCTSTR)"\r\n", pfLog);
		fputs((LPCTSTR)"DESC\r\n", pfLog);

		SemaWait(g_hInfoSemaphore);

		int nCount = (int)m_infoDatabase.size();
		for(int i=0; i<nCount; i++)
		{
			if (m_infoDatabase[i].nResult[pTestEndUsbHid->m_nDevIndex] == RESULT_FAIL)
			{
				if (bTestFail)
					fputs((LPCTSTR)"\r\n", pfLog);			// DESC 바로 다음 칸은 개행 없음.
				else
					bTestFail = TRUE;

				str.Format("ERROR_%s", m_infoDatabase[i].m_info);
				str.Format("%s\r\n", hvFillBlankSpaceInString_0(pTestEndUsbHid->m_nDevIndex, str));
				fputs((LPCTSTR)str, pfLog);

				CStringArray & pErrDesc = pTestEndUsbHid->TestErrDesc_GetDesc(i);
				int nErrCnt = pErrDesc.GetSize();
				for(int j=0; j<nErrCnt; j++){
					fputs((LPCTSTR)",\t\t,\t\t,\t\t,\t\t", pfLog);
					fputs((LPCTSTR)pErrDesc.GetAt(j), pfLog);
					fputs((LPCTSTR)"\r\n", pfLog);
				}
			}
		}

		::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);
	}

	if (pfLog != NULL)
		fclose(pfLog);
}

void CTestModeView::TSM_SaveLoggingFile()
{
#if defined(D_DEBUG_FW_UPGRADE_TEST)
	return;
#endif

	FILE * pfLogging = NULL;

	CString str, strDesc;
	int nPos;

	int nDeviceCount = m_TestDevIndex.GetCount();

	//BOOL bIsFileExist = PathFileExists(m_strLoggingFileName) ? TRUE : FALSE;

	// Create a LOGGING file
	pfLogging = _fsopen(m_strLoggingFileName, "a+b", _SH_DENYNO);
	if (pfLogging == NULL)
	{
		str.Format("Cannot create Logging File (%s)", m_strLoggingFileName);
		AfxMessageBox(str);
		return;
	}

	fputs((LPCTSTR)"\r\n", pfLogging);
	fputs((LPCTSTR)"\r\n", pfLogging);

	str.Format("%s\t", hvFillBlankSpaceInString(0, "", m_nLogLimitNumOfChar_0, '='));
	fputs((LPCTSTR)str, pfLogging);
	fputs((LPCTSTR)",\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t========,\t\t\r\n", pfLogging);

	str.Format("%s,\t\t%04d/%02d/%02d-%02d:%02d:%02d\r\n", hvFillBlankSpaceInString_0(0, "FILE_DATE"), m_StartOleDateTime.GetYear(), m_StartOleDateTime.GetMonth(), m_StartOleDateTime.GetDay(), m_StartOleDateTime.GetHour(), m_StartOleDateTime.GetMinute(), m_StartOleDateTime.GetSecond());
	fputs((LPCTSTR)str, pfLogging);
	str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(0, "ZTD_VERSION"), g_strZtdVersion);
	fputs((LPCTSTR)str, pfLogging);

	if (m_bCheckUserNumber == TRUE)
	{
		str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(0, "USER_NUMBER"), m_strUserNumber);
		fputs((LPCTSTR)str, pfLogging);
	}

	if (m_strCustomer.IsEmpty())
		str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(0, "CUSTOMER"), "NULL");
	else
		str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(0, "CUSTOMER"), m_strCustomer);
	fputs((LPCTSTR)str, pfLogging);

	if (m_strProjectName.IsEmpty())
		str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(0, "PROJECT_NAME"), "NULL");
	else
		str.Format("%s,\t\t%s\r\n", hvFillBlankSpaceInString_0(0, "PROJECT_NAME"), m_strProjectName);
	fputs((LPCTSTR)str, pfLogging);

	str.Format("%s,\t\t%d\r\n", hvFillBlankSpaceInString_0(0, "DEVICE_CONNECT_NUM"), nDeviceCount);
	fputs((LPCTSTR)str, pfLogging);

	fputs((LPCTSTR)"/\r\n", pfLogging);

	//=============================================================
	// Test Result
	CUsbHid * pUsbHid;
	for (int n = 0; n < nDeviceCount; n++)
	{
		pUsbHid = GetUsbHidByDevIndex(m_TestDevIndex.GetAt(n));
		if (pUsbHid == NULL)	continue;

		if (m_bUseBarcode)
		{
			if (pUsbHid->m_strBarcode.IsEmpty())
				str.Format("%s,\t\t,\t\tDEV_%d,\t\t%s\r\n", hvFillBlankSpaceInString_0(0, "BARCODE"), pUsbHid->m_nDevIndex, "ERROR");
			else
				str.Format("%s,\t\t,\t\tDEV_%d,\t\t%s\r\n", hvFillBlankSpaceInString_0(0, "BARCODE"), pUsbHid->m_nDevIndex, pUsbHid->m_strBarcode);

			fputs((LPCTSTR)str, pfLogging);
		}

		fputs((LPCTSTR)pUsbHid->m_strLogging, pfLogging);
		fputs((LPCTSTR)"/\r\n", pfLogging);

		// display Freq, U Cnt, N Cnt of AFE
		str.Format("%s,\t\t,\t\tDEV_%d,\t\t,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X",
			hvFillBlankSpaceInString_0(0, "AFE_FREQ"), pUsbHid->m_nDevIndex,
			pUsbHid->m_AfeValue[TEST_DND].val_Frequency,
			pUsbHid->m_AfeValue[TEST_HF_DND].val_Frequency,
			pUsbHid->m_AfeValue[TEST_SELF_DND].val_Frequency,
			pUsbHid->m_AfeValue[TEST_SELF_DND].val_Frequency_B,
			pUsbHid->m_AfeValue[TEST_SELF_SATURATION].val_Frequency,
			pUsbHid->m_AfeValue[TEST_SELF_SATURATION].val_Frequency_B,
			pUsbHid->m_AfeValue[TEST_JITTER].val_Frequency,
			pUsbHid->m_AfeValue[TEST_HW_CALIBRATION].val_Frequency,
			pUsbHid->m_AfeValue[TEST_MEASURE_DELTA].val_Frequency);
		str.AppendFormat(",\t\t,\t\t[DND],\t\t[HF_DND],\t\t[SELF_DND_A],\t\t[SELF_DND_B],\t\t[SELF_SATURATION_A],\t\t[SELF_SATURATION_B],\t\t[JITTER],\t\t[HW_CALIBRATION],\t\t[MEASURE_DELTA]\r\n");
		fputs((LPCTSTR)str, pfLogging);

		str.Format("%s,\t\t,\t\tDEV_%d,\t\t,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X",
			hvFillBlankSpaceInString_0(0, "AFE_U_CNT"), pUsbHid->m_nDevIndex,
			pUsbHid->m_AfeValue[TEST_DND].val_Ucont,
			pUsbHid->m_AfeValue[TEST_HF_DND].val_Ucont,
			pUsbHid->m_AfeValue[TEST_SELF_DND].val_Ucont,
			pUsbHid->m_AfeValue[TEST_SELF_DND].val_Ucont_B,
			pUsbHid->m_AfeValue[TEST_SELF_SATURATION].val_Ucont,
			pUsbHid->m_AfeValue[TEST_SELF_SATURATION].val_Ucont_B,
			pUsbHid->m_AfeValue[TEST_JITTER].val_Ucont,
			pUsbHid->m_AfeValue[TEST_HW_CALIBRATION].val_Ucont,
			pUsbHid->m_AfeValue[TEST_MEASURE_DELTA].val_Ucont);
		str.AppendFormat(",\t\t,\t\t[DND],\t\t[HF_DND],\t\t[SELF_DND_A],\t\t[SELF_DND_B],\t\t[SELF_SATURATION_A],\t\t[SELF_SATURATION_B],\t\t[JITTER],\t\t[HW_CALIBRATION],\t\t[MEASURE_DELTA]\r\n");
		fputs((LPCTSTR)str, pfLogging);

		str.Format("%s,\t\t,\t\tDEV_%d,\t\t,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X,\t\t0x%04X",
			hvFillBlankSpaceInString_0(0, "AFE_N_CNT"), pUsbHid->m_nDevIndex,
			pUsbHid->m_AfeValue[TEST_DND].val_Ncount,
			pUsbHid->m_AfeValue[TEST_HF_DND].val_Ncount,
			pUsbHid->m_AfeValue[TEST_SELF_DND].val_Ncount,
			pUsbHid->m_AfeValue[TEST_SELF_DND].val_Ncount_B,
			pUsbHid->m_AfeValue[TEST_SELF_SATURATION].val_Ncount,
			pUsbHid->m_AfeValue[TEST_SELF_SATURATION].val_Ncount_B,
			pUsbHid->m_AfeValue[TEST_JITTER].val_Ncount,
			pUsbHid->m_AfeValue[TEST_HW_CALIBRATION].val_Ncount,
			pUsbHid->m_AfeValue[TEST_MEASURE_DELTA].val_Ncount);
		str.AppendFormat(",\t\t,\t\t[DND],\t\t[HF_DND],\t\t[SELF_DND_A],\t\t[SELF_DND_B],\t\t[SELF_SATURATION_A],\t\t[SELF_SATURATION_B],\t\t[JITTER],\t\t[HW_CALIBRATION],\t\t[MEASURE_DELTA]\r\n");
		fputs((LPCTSTR)str, pfLogging);
		fputs((LPCTSTR)"/\r\n", pfLogging);

		if(m_bMeasureLdo)
		{
			str.Format("%s,\t\t,\t\tDEV_%d,\t\t,\t\t%.2fV,\t\t%.2fV,\t\t%.2fV",hvFillBlankSpaceInString_0(0, "LDO_VOLTAGE"),
				pUsbHid->m_nDevIndex, pUsbHid->m_fVoltageVcc, pUsbHid->m_fVoltageVi2c, pUsbHid->m_fVIH_1);
			str.AppendFormat(",\t\t,\t\t[VCC],\t\t[VIC2],\t\t[VIH]\r\n");
			fputs((LPCTSTR)str, pfLogging);
			fputs((LPCTSTR)"/\r\n", pfLogging);
		}
	}

	// Display error description
	SemaWait(g_hInfoSemaphore);

	BOOL bTestFail = FALSE;				// for "DESC" display

	int nCount = (int)m_infoDatabase.size();
	for(int nTestStep=0; nTestStep<nCount; nTestStep++)
	{
		for (int n = 0; n < nDeviceCount; n++)
		{
			pUsbHid = GetUsbHidByDevIndex(m_TestDevIndex.GetAt(n));

			if (pUsbHid != NULL && pUsbHid->m_nResult == RESULT_FAIL)
			{
				if (bTestFail == FALSE)
				{
					fputs((LPCTSTR)"/\r\n", pfLogging);
					fputs((LPCTSTR)"DESC\r\n", pfLogging);
					bTestFail = TRUE;
				}

				if (m_infoDatabase[nTestStep].nResult[pUsbHid->m_nDevIndex] == RESULT_FAIL)
				{
					str.Format("ERROR_%s", m_infoDatabase[nTestStep].m_info);
					strDesc.Format("%s,\t\t,\t\t%s,\t\tFAIL", hvFillBlankSpaceInString_0(0, str), pUsbHid->m_strDevName);

					CStringArray & pErrDesc = pUsbHid->TestErrDesc_GetDesc(nTestStep);
					int nErrCnt = pErrDesc.GetSize();
					for(int j=0; j<nErrCnt; j++)
					{
						str = pErrDesc.GetAt(j);

						if (nTestStep >= TEST_CHECK_VERSION && nTestStep <= TEST_DIGITAL_WHEEL)
						{
							if (nTestStep == TEST_CHECK_VERSION || nTestStep == TEST_CURRENT_CONSUMPTION)
								nPos = str.Find(" ");																				// IC_REVISION MAJOR_VERSION
							else
								nPos = str.Find(",\t\t");																			// [X0000_Y0007] [X0000_Y0008],

							if (nPos == -1)	continue;

							strDesc.AppendFormat(",\t\t%s", str.Left(nPos));
						}
						else
						{
							strDesc.AppendFormat(",\t\t%s", str);
							break;
						}
					}
					strDesc.AppendFormat("\r\n");
					fputs((LPCTSTR)strDesc, pfLogging);
				}
			}
		}
	}

	fputs((LPCTSTR)"/\r\n", pfLogging);
	fputs((LPCTSTR)"/\r\n", pfLogging);

	::ReleaseSemaphore(g_hInfoSemaphore, 1, NULL);
	//=============================================================

	if (pfLogging != NULL)
		fclose(pfLogging);
}

void CTestModeView::CheckSameLogFile(CString FileDir, CString TEST_NAME, CString ASIC_ID, CString strNewFile)
{
	CString strFindPath, strName;
	CFileFind m_finder;
	CString del_file_name;

	del_file_name.Format("%s_%s", TEST_NAME, ASIC_ID);	//원하는 File name 선택

	strFindPath.Format("%s\\*.*", FileDir);

	BOOL bFound = m_finder.FindFile(strFindPath);
	while (bFound)
	{
		bFound = m_finder.FindNextFile();

		if (m_finder.IsHidden() || m_finder.IsSystem() || m_finder.IsDirectory())
			continue;

		strName = m_finder.GetFileName();

		if (strName.Find(del_file_name) == 0)
		{
			CString strOldFile;
			strOldFile.Format("%s\\%s", FileDir, strName);
			Rename(strOldFile, strNewFile, TRUE);
			break;
		}
	}
	m_finder.Close();
}

void CTestModeView::Rename(CString strOldName, CString strNewName, BOOL bUnlock)
{
	try
	{
		CFile::Rename(strOldName, strNewName);
	}
	catch (CFileException* pEx)
	{
		//TRACE(_T("File %20s not found, cause = %d\n"), strOldFile, pEx->m_cause);
		pEx->Delete();

		if (GetLastError() != ERROR_SHARING_VIOLATION)
			bUnlock = FALSE;

#if defined(D_HV_FILE_UNLOCKER)
		if (bUnlock)
		{
			FileUnlocker_Unlock(strOldName);

			CopyFile(strOldName, strNewName, FALSE);

			int retry_cnt = 0;

			do
			{
				if (DeleteFile(strOldName))
					break;

				retry_cnt++;

				Sleep(1000);
			} while (retry_cnt < 3);
		}
		else
		{
			CopyFile(strOldName, strNewName, FALSE);
		}
#else
		if (bUnlock)
		{
			AfxMessageBox("SaveLog File is Open. Close it, Please.");
		}

		// 파일이 사용 중일 경우 LOGGING 파일을 저장하지 않는다.
		//CopyFile(strOldName, strNewName, FALSE);
#endif
	}
}

//==============================================================================================
// Test Error Description - for LOG and LOGGING file

void CTestModeView::Log_Append(CUsbHid * pUsbHid, int nResult, BOOL bIsLoggingData)
{
	CString strItem, strResult;

	if (nResult == RESULT_PASS)
		strResult = "PASS";
	else if (nResult == RESULT_FAIL)
		strResult = "FAIL";
	else
		strResult = STRING_NOT_TEST;

#if defined(D_DISPLAY_NOT_USE_TEST_TO_RESERVED)
	if (nResult == RESULT_NONE)
		strItem = STRING_RESERVED_ITEM;
	else
#endif
		strItem = m_strArrTestInfo.GetAt(pUsbHid->m_nTestStep);

	strItem.Format("%s", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, strItem));

	if(pUsbHid->m_nTestStep == TEST_HIGH_VOLTAGE && pUsbHid->m_bErrorRead == TRUE)
		pUsbHid->m_strLog.AppendFormat("%s,\t\t,\t\t%s,\t\t%s", strItem, pUsbHid->m_strDevName, strResult);			//2022.10.11 염프로 요청사항. Log 에서도 value 표현을 위함.
	else
		pUsbHid->m_strLog.AppendFormat("%s,\t\t,\t\t%s,\t\t%s\r\n", strItem, pUsbHid->m_strDevName, strResult);


	// save logging
	if (bIsLoggingData)
		pUsbHid->m_strLogging.AppendFormat("%s,\t\t,\t\t%s,\t\t%s", strItem, pUsbHid->m_strDevName, strResult);		// Carriage return OFF
	else
		pUsbHid->m_strLogging.AppendFormat("%s,\t\t,\t\t%s,\t\t%s\r\n", strItem, pUsbHid->m_strDevName, strResult);	// Carriage return ON
}

void CTestModeView::Log_AppendDiff(CUsbHid * pUsbHid, int nResult, BOOL bIsLoggingData, CString strItem)
{
	CString strResult;

	if (nResult == RESULT_PASS)
		strResult = "PASS";
	else if (nResult == RESULT_FAIL)
		strResult = "FAIL";
	else
		strResult = STRING_NOT_TEST;

	//strItem = m_strArrTestInfo.GetAt(pUsbHid->m_nTestStep);
	strItem.Format("%s", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, strItem));

	pUsbHid->m_strLog.AppendFormat("%s,\t\t,\t\t%s,\t\t%s\r\n", strItem, pUsbHid->m_strDevName, strResult);

	// save logging
	if (bIsLoggingData)
		pUsbHid->m_strLogging.AppendFormat("%s,\t\t,\t\t%s,\t\t%s", strItem, pUsbHid->m_strDevName, strResult);		// Carriage return OFF
	else
		pUsbHid->m_strLogging.AppendFormat("%s,\t\t,\t\t%s,\t\t%s\r\n", strItem, pUsbHid->m_strDevName, strResult);	// Carriage return ON
}

void CTestModeView::Log_AppendDesc(CUsbHid * pUsbHid, int nResult, const CString & pAdditional)
{
	CString strItem, strResult, str;

	if (nResult == RESULT_PASS)
		strResult = "PASS";
	else if (nResult == RESULT_FAIL)
		strResult = "FAIL";
	else
		strResult = STRING_NOT_TEST;

	strItem = m_strArrTestInfo.GetAt(pUsbHid->m_nTestStep);
	strItem.Format("%s", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, strItem));

	str.Format("%s,\t\t,\t\t%s,\t\t%s,\t\t%s", strItem, pUsbHid->m_strDevName, strResult, pAdditional);

	pUsbHid->m_strLog.AppendFormat(str);

	pUsbHid->m_strLogging.AppendFormat(str);
}

void CTestModeView::Log_AppendDescIUM(CUsbHid * pUsbHid, int nResult, const CString & pAdditional , BOOL bTitle)
{
	CString strItem, strResult, str;

	if (bTitle == TRUE)
	{
		strResult = STRING_NOT_TEST;

		strItem = "__";
		strItem.Format("%s", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, strItem));
		str.Format("%s,\t\t,\t\t%s,\t\t%s,\t\t%s", strItem, "     ", "     ", pAdditional);
	}
	else
	{
		if (nResult == RESULT_PASS)
			strResult = "PASS";
		else if (nResult == RESULT_FAIL)
			strResult = "FAIL";
		else
			strResult = STRING_NOT_TEST;

		strItem = "IUM_INFO";
		strItem.Format("%s", hvFillBlankSpaceInString_0(pUsbHid->m_nDevIndex, strItem));

		str.Format("%s,\t\t,\t\t%s,\t\t%s,\t\t%s", strItem, pUsbHid->m_strDevName, strResult, pAdditional);
	}

	pUsbHid->m_strLog.AppendFormat(str);
	pUsbHid->m_strLogging.AppendFormat(str);
}

void CTestModeView::Log_AppendEx(CUsbHid * pUsbHid, LPCTSTR pstrFormat, ...)
{
	CString str;

	va_list args;
	va_start(args, pstrFormat);
	str.FormatV(pstrFormat, args);
	va_end(args);

	pUsbHid->m_strLog.AppendFormat("%s", str);

	//pUsbHid->m_strLogging.AppendFormat(str);
}

void CTestModeView::Logging_AppendEx(CUsbHid * pUsbHid, LPCTSTR pstrFormat, ...)
{
	CString str;

	va_list args;
	va_start(args, pstrFormat);
	str.FormatV(pstrFormat, args);
	va_end(args);

	pUsbHid->m_strLogging.AppendFormat(str);
}

//==============================================================================================
// Test Error Description - for display in Test Result Description

void CTestModeView::TestResultDesc_AddMessage(int nDevIndex, COLORREF color, LPCTSTR pstrFormat, ...)
{
	CTestResultDescDlg * pTestResultDesc = TestResultDesc_GetHandle(nDevIndex);
	if(pTestResultDesc == NULL)
		return;

	CString str;

	va_list args;
	va_start(args, pstrFormat);
	str.FormatV(pstrFormat, args);
	va_end(args);

	pTestResultDesc->AddMessage(color, str);
}

void CTestModeView::TestResultDesc_ClearMessage(int nDevIndex)
{
	CTestResultDescDlg * pTestResultDesc = TestResultDesc_GetHandle(nDevIndex);
	if(pTestResultDesc == NULL)
		return;

	pTestResultDesc->ClearMessage();
}

void CTestModeView::TestResultDesc_AddTestStep(int nDevIndex, int nTestStep)
{
#if defined(D_MEASURE_ELAPSED_TIME)
	if(nTestStep == 0)
		m_dt = GetTimeInMillis();
#endif

	if (nTestStep >= (int)m_infoDatabase.size())
	{
		::SendMessage(m_parentHWnd, WM_USER_PROGRESS_UPDATE, (WPARAM)nTestStep, (LPARAM)0);
		return;
	}

#if defined(D_DISPLAY_NOT_USE_TEST_TO_RESERVED)
	if( m_infoDatabase[nTestStep].m_image == IMAGE_LIST_DISABLE )
		TestResultDesc_AddMessage(nDevIndex, ZT_COLOR_GRAY, "[%02d:%s]\r\n", nTestStep+1, STRING_RESERVED_ITEM);
	else
#endif
		TestResultDesc_AddMessage(nDevIndex, ZT_COLOR_BLACK, "[%02d:%s]\r\n", nTestStep + 1, m_strArrTestInfo.GetAt(nTestStep));

	::SendMessage(m_parentHWnd, WM_USER_PROGRESS_UPDATE, (WPARAM)nTestStep, (LPARAM)0);

#if defined(D_MEASURE_ELAPSED_TIME)
	TRACE("\t [STEP_%02d] %llu msec \n", nTestStep, GetTimeInMillis()-m_dt);
	m_dt = GetTimeInMillis();
#endif
}

// Integrated display
void CTestModeView::TestResultDesc_AddResult(int nTestStep)
{
	CUsbHid * pUsbHid = NULL;
	BOOL bFoundPassDev = FALSE;
	BOOL bFoundNotTestDev = FALSE;
	BOOL bFoundFailDev = FALSE;
	CString str;
	int i, nPos, nDevIndex;

	int nDeviceCount = m_TestDevIndex.GetCount();

	if( nTestStep > 0 )
		nTestStep--;

	if (nTestStep >= (int)m_infoDatabase.size())
		return;

	//초기 zTD 켰을 때 Desc창에 Not Test가 출력됨 확인.
	//TEST 중일 때만 Not Test 문구가 desc box에 출력되도록 내용 추가.(Paul)
	if (m_infoDatabase[nTestStep].m_image == IMAGE_LIST_DISABLE && m_bAleadyTestEnter == TRUE)
	{
		TestResultDesc_AddMessage(0, ZT_COLOR_GRAY, "\tNot Test\r\n");
		return;
	}

	// PASS
	for(i=0; i<nDeviceCount; i++)
	{
		nDevIndex = m_TestDevIndex.GetAt(i);
		pUsbHid = GetUsbHidByDevIndex(nDevIndex);
		if (pUsbHid == NULL)	continue;

		if (m_infoDatabase[nTestStep].nResult[nDevIndex] == RESULT_PASS)
		{
			if (bFoundPassDev == FALSE)
			{
				bFoundPassDev = TRUE;

				if (nTestStep == TEST_CURRENT_CONSUMPTION)
					str.Format("[PASS] : Device %d(%d uA)", nDevIndex, pUsbHid->m_curCurrent[CURRENT_CONSUMPTION_ACTIVE]);
				else
					str.Format("[PASS] : Device %d", nDevIndex);
			}
			else
			{
				if (nTestStep == TEST_CURRENT_CONSUMPTION)
					str.AppendFormat(" , %d(%d uA)", nDevIndex, pUsbHid->m_curCurrent[CURRENT_CONSUMPTION_ACTIVE]);
				else
					str.AppendFormat(" , %d", nDevIndex);
			}
		}
	}

	if (bFoundPassDev)
		TestResultDesc_AddMessage(0, ZT_COLOR_BLACK, "\t%s\r\n", str);		// [PASS] : Device 0 , 2

	// Not Test
	for(i=0; i<nDeviceCount; i++)
	{
		nDevIndex = m_TestDevIndex.GetAt(i);
		pUsbHid = GetUsbHidByDevIndex(nDevIndex);
		if (pUsbHid == NULL)	continue;

		if (m_infoDatabase[nTestStep].nResult[nDevIndex] == RESULT_NONE)
		{
			if (bFoundNotTestDev == FALSE)
			{
				bFoundNotTestDev = TRUE;
				str.Format("[%s] : Device %d", STRING_NOT_TEST, nDevIndex);
			}
			else
			{
				str.AppendFormat(" , %d", nDevIndex);
			}
		}
	}

	if (bFoundNotTestDev)
		TestResultDesc_AddMessage(0, ZT_COLOR_GRAY, "\t%s\r\n", str);		// [Not Test] : Device 1 , 6

	// FAIL
	for(i=0; i<nDeviceCount; i++)
	{
		nDevIndex = m_TestDevIndex.GetAt(i);
		pUsbHid = GetUsbHidByDevIndex(nDevIndex);
		if (pUsbHid == NULL)	continue;

		if (m_infoDatabase[nTestStep].nResult[nDevIndex] == RESULT_FAIL)
		{
			if (bFoundFailDev == FALSE)
			{
				bFoundFailDev = TRUE;
				str.Format("[FAIL] : Device %d", nDevIndex);
			}
			else
			{
				str.AppendFormat(" , %d", nDevIndex);
			}
		}
	}

	if (bFoundFailDev == FALSE)
		return;

	// FAIL Description
	BOOL bDispMaxMin = FALSE;
	BOOL bDispSpecOver = FALSE;

	if (nTestStep == TEST_DND || nTestStep == TEST_HF_DND ||
		nTestStep == TEST_SELF_DND || nTestStep == TEST_SELF_SATURATION ||
		nTestStep == TEST_HW_CALIBRATION || nTestStep == TEST_MEASURE_DELTA || nTestStep == TEST_DIGITAL_WHEEL ||
		nTestStep == TEST_CND	//add 2022.11.14 for ZTM730 CND
	)
	{
		bDispMaxMin = TRUE;
	}
	else if (nTestStep == TEST_DND_GAP || nTestStep == TEST_HF_DND_GAP ||
			 nTestStep == TEST_JITTER || nTestStep == TEST_TRX_SHORT )
	{
		bDispSpecOver = TRUE;
	}

	TestResultDesc_AddMessage(0, ZT_COLOR_RED, "\t%s\r\n", str);		// [FAIL] : Device 3 , 5 , 7

	for(i=0; i<nDeviceCount; i++)
	{
		nDevIndex = m_TestDevIndex.GetAt(i);
		pUsbHid = GetUsbHidByDevIndex(nDevIndex);
		if (pUsbHid == NULL)	continue;

		if (m_infoDatabase[nTestStep].nResult[nDevIndex] != RESULT_FAIL)
			continue;

		CStringArray & pErrDesc = pUsbHid->TestResultDesc_GetDesc(nTestStep);
		int nErrCnt = pErrDesc.GetSize();
		if (nErrCnt <= 0)
			continue;

		if (nTestStep == TEST_CHECK_VERSION)
		{
			for(int j=0; j<nErrCnt; j++)
			{
				TestResultDesc_AddMessage(0, ZT_COLOR_RED, "\t\t[Device %d] : %s\r\n", nDevIndex, pErrDesc.GetAt(j));
			}
		}
		else if (nTestStep == TEST_CURRENT_CONSUMPTION || nTestStep == TEST_TOUCH_BUTTON)
		{
			str = "";

			for(int j=0; j<nErrCnt; j++)
			{
				if (j>0)
					str.AppendFormat("%s", ", ");

				str.AppendFormat("%s", pErrDesc.GetAt(j));
			}

			TestResultDesc_AddMessage(0, ZT_COLOR_RED, "\t\t[Device %d] : %s\r\n", nDevIndex, str);
		}
		else if (nTestStep == TEST_SELF_DND_GAP)
		{// dispaly all error node
			BOOL bFoundTx = FALSE, bFoundRx = FALSE;
			int nXvalue, nYvalue;
			CString strColY = "", strRowX = "";
			CString strValue;

			for(int j=0; j<nErrCnt; j++)
			{
				str = pErrDesc.GetAt(j);

				nYvalue = nXvalue = -1;

				// Get COL_Y
				nPos = str.Find("COL_Y");
				if (nPos != -1) {
					strValue = str.Mid(nPos + 5, 4);
					nYvalue = atoi(strValue);
				}else{
					// Get ROW_X
					nPos = str.Find("ROW_X");
					if (nPos == -1)	continue;
					strValue = str.Mid(nPos + 5, 4);
					nXvalue = atoi(strValue);
				}

				if (nYvalue != -1)	// RX - Y
				{
					if (bFoundRx == FALSE){
						strColY.Format("COL_Y   %04d", nYvalue);
						bFoundRx = TRUE;
					}else{
						strColY.AppendFormat(" , %04d", nYvalue);
					}
				}
				else if (nXvalue != -1)	// TX - X
				{
					if (bFoundTx == FALSE){
						strRowX.Format("ROW_X   %04d", nXvalue);
						bFoundTx = TRUE;
					}else{
						strRowX.AppendFormat(" , %04d", nXvalue);
					}
				}
			}

			if (strColY.IsEmpty() == FALSE)
				TestResultDesc_AddMessage(0, ZT_COLOR_RED, "\t\t[Device %d] : %s\r\n", nDevIndex, strColY);
			if (strRowX.IsEmpty() == FALSE)
				TestResultDesc_AddMessage(0, ZT_COLOR_RED, "\t\t[Device %d] : %s\r\n", nDevIndex, strRowX);
		}
		else if (bDispMaxMin == TRUE)
		{
			// MAX
			for(int j=0; j<nErrCnt; j++)
			{
				str = pErrDesc.GetAt(j);

				if( str.Find("SPEC_OVER(MAX)") != -1 ){
					str.Replace(",\t\t", "\t");
					TestResultDesc_AddMessage(0, ZT_COLOR_RED, "\t\t[Device %d] : %s\r\n", nDevIndex, str);
					break;
				}
			}

			// MIN
			for(int j=0; j<nErrCnt; j++)
			{
				str = pErrDesc.GetAt(j);

				if( str.Find("SPEC_OVER(MIN)") != -1 ){
					str.Replace(",\t\t", "\t");
					TestResultDesc_AddMessage(0, ZT_COLOR_RED, "\t\t[Device %d] : %s\r\n", nDevIndex, str);
					break;
				}
			}
		}
		else if (bDispSpecOver == TRUE)
		{
			str = pErrDesc.GetAt(0);
			str.Replace(",\t\t", "\t");
			TestResultDesc_AddMessage(0, ZT_COLOR_RED, "\t\t[Device %d] : %s\r\n", nDevIndex, str);
		}
		else
		{
			str = pErrDesc.GetAt(0);
			str.Replace(",\t\t", "\t");
			TestResultDesc_AddMessage(0, ZT_COLOR_RED, "\t\t[Device %d] : %s\r\n", nDevIndex, str);
		}
	}
}

void CTestModeView::TestResultDesc_AddResultEx(CUsbHid * pUsbHid, int nTestStep)
{
	if(pUsbHid == NULL)
		return;

	CString str;
	int nPos;
	int nDevIndex = pUsbHid->m_nDevIndex;

	if( nTestStep > 0 )
		nTestStep--;

	if (nTestStep >= (int)m_infoDatabase.size())
		return;

	//초기 zTD 켰을 때 Desc창에 Not Test가 출력됨 확인.
	//TEST 중일 때만 Not Test 문구가 desc box에 출력되도록 내용 추가.(Paul)
	if (m_infoDatabase[nTestStep].m_image == IMAGE_LIST_DISABLE && m_bAleadyTestEnter == TRUE)
	{
		TestResultDesc_AddMessage(nDevIndex, ZT_COLOR_GRAY, "\tNot Test\r\n");
		return;
	}

	// PASS
	if (m_infoDatabase[nTestStep].nResult[nDevIndex] == RESULT_PASS)
	{
		if (nTestStep == TEST_CURRENT_CONSUMPTION)
			str.Format("[PASS](%d uA)", pUsbHid->m_curCurrent[CURRENT_CONSUMPTION_ACTIVE]);
		else
			str.Format("[PASS]");

		TestResultDesc_AddMessage(nDevIndex, ZT_COLOR_BLACK, "\t%s\r\n", str);
		return;
	}

	// Not Test
	if (m_infoDatabase[nTestStep].nResult[nDevIndex] == RESULT_NONE)
	{
		str.Format("[%s]", STRING_NOT_TEST);

		TestResultDesc_AddMessage(nDevIndex, ZT_COLOR_GRAY, "\t%s\r\n", str);
		return;
	}

	// FAIL
	if (m_infoDatabase[nTestStep].nResult[nDevIndex] == RESULT_FAIL)
	{
		str.Format("[FAIL]");

		TestResultDesc_AddMessage(nDevIndex, ZT_COLOR_RED, "\t%s\r\n", str);		// [FAIL] : Device 3 , 5 , 7
	}

	// FAIL Description
	BOOL bDispMaxMin = FALSE;
	BOOL bDispSpecOver = FALSE;

	if (nTestStep == TEST_DND || nTestStep == TEST_HF_DND ||
		nTestStep == TEST_SELF_DND || nTestStep == TEST_SELF_SATURATION ||
		nTestStep == TEST_HW_CALIBRATION || nTestStep == TEST_MEASURE_DELTA || nTestStep == TEST_DIGITAL_WHEEL)
	{
		bDispMaxMin = TRUE;
	}
	else if (nTestStep == TEST_DND_GAP || nTestStep == TEST_HF_DND_GAP ||
			 nTestStep == TEST_JITTER || nTestStep == TEST_TRX_SHORT )
	{
		bDispSpecOver = TRUE;
	}

	CStringArray & pErrDesc = pUsbHid->TestResultDesc_GetDesc(nTestStep);
	int nErrCnt = pErrDesc.GetSize();
	if (nErrCnt <= 0)
		return;

	if (nTestStep == TEST_CHECK_VERSION)
	{
		for(int j=0; j<nErrCnt; j++)
		{
			TestResultDesc_AddMessage(nDevIndex, ZT_COLOR_RED, "\t\t%s\r\n", pErrDesc.GetAt(j));
		}
	}
	else if (nTestStep == TEST_CURRENT_CONSUMPTION || nTestStep == TEST_TOUCH_BUTTON)
	{
		str = "";

		for(int j=0; j<nErrCnt; j++)
		{
			if (j>0)
				str.AppendFormat("%s", ", ");

			str.AppendFormat("%s", pErrDesc.GetAt(j));
		}

		TestResultDesc_AddMessage(nDevIndex, ZT_COLOR_RED, "\t\t%s\r\n", str);
	}
	else if (nTestStep == TEST_SELF_DND_GAP)
	{// dispaly all error node
		BOOL bFoundTx = FALSE, bFoundRx = FALSE;
		int nXvalue, nYvalue;
		CString strColY = "", strRowX = "";
		CString strValue;

		for(int j=0; j<nErrCnt; j++)
		{
			str = pErrDesc.GetAt(j);

			nYvalue = nXvalue = -1;

			// Get COL_Y
			nPos = str.Find("COL_Y");
			if (nPos != -1) {
				strValue = str.Mid(nPos + 5, 4);
				nYvalue = atoi(strValue);
			}else{
				// Get ROW_X
				nPos = str.Find("ROW_X");
				if (nPos == -1)	continue;
				strValue = str.Mid(nPos + 5, 4);
				nXvalue = atoi(strValue);
			}

			if (nYvalue != -1)	// RX - Y
			{
				if (bFoundRx == FALSE){
					strColY.Format("COL_Y   %04d", nYvalue);
					bFoundRx = TRUE;
				}else{
					strColY.AppendFormat(" , %04d", nYvalue);
				}
			}
			else if (nXvalue != -1)	// TX - X
			{
				if (bFoundTx == FALSE){
					strRowX.Format("ROW_X   %04d", nXvalue);
					bFoundTx = TRUE;
				}else{
					strRowX.AppendFormat(" , %04d", nXvalue);
				}
			}
		}

		if (strColY.IsEmpty() == FALSE)
			TestResultDesc_AddMessage(nDevIndex, ZT_COLOR_RED, "\t\t%s\r\n", strColY);
		if (strRowX.IsEmpty() == FALSE)
			TestResultDesc_AddMessage(nDevIndex, ZT_COLOR_RED, "\t\t%s\r\n", strRowX);
	}
	else if (bDispMaxMin == TRUE)
	{
		// MAX
		for(int j=0; j<nErrCnt; j++)
		{
			str = pErrDesc.GetAt(j);

			if( str.Find("SPEC_OVER(MAX)") != -1 ){
				str.Replace(",\t\t", "\t");
				TestResultDesc_AddMessage(nDevIndex, ZT_COLOR_RED, "\t\t%s\r\n", str);
				break;
			}
		}

		// MIN
		for(int j=0; j<nErrCnt; j++)
		{
			str = pErrDesc.GetAt(j);

			if( str.Find("SPEC_OVER(MIN)") != -1 ){
				str.Replace(",\t\t", "\t");
				TestResultDesc_AddMessage(nDevIndex, ZT_COLOR_RED, "\t\t%s\r\n", str);
				break;
			}
		}
	}
	else if (bDispSpecOver == TRUE)
	{
		str = pErrDesc.GetAt(0);
		str.Replace(",\t\t", "\t");
		TestResultDesc_AddMessage(nDevIndex, ZT_COLOR_RED, "\t\t%s\r\n", str);
	}
	else
	{
		str = pErrDesc.GetAt(0);
		str.Replace(",\t\t", "\t");
		TestResultDesc_AddMessage(nDevIndex, ZT_COLOR_RED, "\t\t%s\r\n", str);
	}
}

