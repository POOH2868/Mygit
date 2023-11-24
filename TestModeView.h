#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "global.h"
#include ".\Hid\UsbHid.h"
#include ".\Hid\UsbHidList.h"
#include ".\CustomUI\CustomButton\BtnST.h"
#include ".\CustomUI\CustomButton\ShadeButtonST.h"
#include ".\CustomUI\ListCtrlEx.h"
#include ".\CustomUI\RichEditEx.h"
#include ".\CustomUI\DimTabCtrl.h"

#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
#include "ConnectInfoDlg.h"
#endif

#include <sys/timeb.h>
#include <vector>

#include "GpioButtonDlg.h"
#include "TouchButtonDlg.h"
#include "BarcodeSettingDlg.h"

#include "TestModeView_Test.h"
#include "TestModeView_TestInfo.h"

#include "TestResultDescDlg.h"


// CTestModeView 폼 뷰입니다.

class CTestResultData
{
public:
	CTestResultData()
	{
		m_item = "";
		m_info = "";
		m_image = 0;

		for (int i = 0; i<MAX_DEVICE_COUNT; i++)
		{
			nResult[i] = 0;
			strResult[i] = "";
		}
	}

	CString m_item;		// INDEX
	CString m_info;		// INFO
	int m_image;		// icon

	int nResult[MAX_DEVICE_COUNT];
	CString strResult[MAX_DEVICE_COUNT];
};

class CTestModeView : public CFormView
{
	DECLARE_DYNCREATE(CTestModeView)

protected:
	CTestModeView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CTestModeView();

public:
	enum { IDD = IDD_FORMVIEW_TESTMODE };

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:

	static CUsbHidList	* m_pUsbHidList;

protected:

	BOOL m_bCreatePane;

	CString m_strZtdExeDir;
	CString m_strConfigDir;

	int m_nTestAvailableCount;

	CFont  m_ListFont;
	CFont  m_StartButtonFont;

	CListCtrlEx m_listInfo;
	CImageList *m_imageListCtrl;

	CRect	  m_rect_list;

	int m_nMaxDeviceIndex;
	int m_nAvailableTestStep;
	int m_nEnableTestCount;
	int m_nMaxMpToolVersion;

	CUIntArray 		m_TestDevIndex;

	COleDateTime	m_StartOleDateTime;

	CStringArray	m_strArrTestInfo;

	// for progress bar
	int m_nTestStepMin;

	// info
	std::vector <CTestResultData> m_infoDatabase;

	BOOL m_bExistDevice;
	BOOL m_bAleadyTestEnter;
	BOOL m_bAleadyTestReady;
	BOOL m_bTestEnterByFwAutoDownload;
	BOOL m_bIsDefaultConfig;
	BOOL m_bEnableStartButton;

	int	m_nBarcodeIdx;
	int	m_nBarcodeChIdx;
	char m_cBarcode[1000];

	UINT64 m_BarcodeInputTime;

	BOOL m_bIsLogin;

	UINT64 m_nTestStartTime;

	CShadeButtonST m_btnOption;
	CShadeButtonST m_btnStartTestAll;

	CStatic m_staticPassCountTitle;
	CStatic m_staticPassCount;
	CStatic m_staticFailCountTitle;
	CStatic m_staticFailCount;
	CStatic m_staticTotalCountTitle;
	CStatic m_staticTotalCount;

	CStatic m_picTitle;

	CStatic m_picPass;
	CStatic m_picFail;
	CStatic m_picStandBy;

	static int m_Pass_Count;
	static int m_Fail_Count;
	static BOOL m_bResetCount;

	// Main
	CStatic m_staticCustomerTitle;
	CStatic m_staticCustomer;
	CStatic m_staticProjectNameTitle;
	CStatic m_staticProjectName;
	CStatic m_staticVersionTitle;
	CStatic m_staticVersion;

	CStatic m_staticBarcodeTitle;
	CStatic m_staticBarcode;

	BOOL m_bAdditionalLOG;
	CString m_strTestName;
	CString m_strTestStage;
	CString m_strModuleMaker;
	int m_nInOutType;

	// Option > Default
	BOOL m_bUseIFE;
	BOOL m_bSaveResult;
	BOOL m_bSaveFixedPosition;
	BOOL m_bCreatePassFailDirectory;
	int m_nStandBySec;
	BOOL m_bUseRetry;
	BOOL m_bUseMptoolver;
	int m_nAllRetryCount;
	int m_nMptoolCount;
	int m_nSolutionResultNotify;

	int m_nAllRetryCopyCount;	//OnBnClickedButtonStartTestAll (m_nAllRetryCopyCount = m_nAllRetryCount)
	int m_nStartTestIndex;
	e_AllRetryStatus m_eAllRetryStatus;

	// Option > Barcode
	BOOL m_bUseBarcode;
	BOOL m_bBarcodeDisplayInTest;
	BOOL m_bAutoStartTest;
	BOOL m_bFixedBarcodeLength;
	int m_nBarcode_Length;
	int m_nBarcode_AutoReset;

	// Option > Download
	BOOL m_bDownload;
	BOOL m_bDownloadFixedPosition;
	int m_nDownloadRetryCount;
	BOOL m_bSaveInfo;
	int	 m_nSaveInfoLocation;
	CString m_strDownloadErrorMsg;
	BOOL m_bVerifyFirmware;
	BOOL m_bVerifyAfterDownload;
	BOOL m_bBurstUpgrade;
	BOOL m_bBurstUpgradeReal;
	BOOL m_bForceUpgrade;
	BOOL m_bChangeNvmTiming;
	int m_nIUMFormat;
	int m_nI2CFormat;
	
	BOOL m_bCheckEepromBit;
	BOOL m_bUseNvmPgmVerify;

	CString m_strProjectID;
	CString m_strDeviceID;

	CString m_strFirmFileName;
	CString m_strSaveResultDir;
	CString m_strCustomer;
	CString m_strProjectName;
	CString m_strFirmwareDir;
	CString m_strLoggingFileName;

	// Check Version
	BOOL m_bCheckVersion;
	BOOL m_bCheckAutoFwDownload;
	BOOL m_bCheckIUM;

	BOOL m_bCheckI2C_Level;
	// check VIH or I2c
	//BOOL m_bI2C_Check;
	//short int m_nI2C_Value;
	//short int m_I2C_Value;
	
	//
	short int m_nIcReivsion;
	short int m_nHwID;
	short int m_nMajorVersion;
	short int m_nMinorVersion;
	short int m_nRegisterVersion;

	CString m_strIcRevision;
	CString m_strHwID;
	CString m_strMajorVersion;
	CString m_strMinorVersion;
	CString m_strRegisterVersion;

	CString m_strModuleID;

	BOOL m_bCheckIntPin;
	BOOL m_bCheckReset;
	BOOL m_bCheckResetPin;

	// password
	CString m_strUserAdminPassword;
	BOOL m_bUseStartup;
	CString m_strStartupPassword;

	// TEST INFO
	TEST_INFO_DND					m_TestInfo_DND;
	TEST_INFO_DND					m_TestInfo_HF_DND;

	TEST_INFO_CND					m_TestInfo_CND;					// CND	//add 2022.11.14 for ZTM730 CND

	TEST_INFO_HIGH_VOLTAGE			m_TestInfo_HIGH_VOLTAGE;		// HighVoltage Test

	TEST_INFO_SELF					m_TestInfo_SELF_DND;
	TEST_INFO_ONLYSELF				m_TestInfo_ONLYSELF_DND;		//for only self raw data
	TEST_INFO_SELF					m_TestInfo_SELF_SATURATION;

	TEST_INFO_MEASURE_DELTA			m_TestInfo_MEASURE_DELTA;
	TEST_INFO_HW_CALIBRATION		m_TestInfo_HW_CALIBRATION;
	TEST_INFO_JITTER				m_TestInfo_JITTER;
	TEST_INFO_CURRENT_CONSUMPTION 	m_TestInfo_CURRENT_CONSUMPTION;
	TEST_INFO_DIGITAL_WHEEL			m_TestInfo_DIGITAL_WHEEL;
	TEST_INFO_NOISE					m_TestInfo_NOISE;

	BOOL m_bCheckTrxShort;

	BOOL m_bUseMultiDevice;
	int  m_nMultiDeviceCount;
	BOOL m_bUseStartIndividualTest;

	BOOL m_bCheckUserNumber;
	CString m_strUserNumber;

	BOOL m_bHidMode;

	int m_nModuleVendorCode;

#if defined(D_USE_CHECK_BUTTON_TEST)
	BOOL m_bCheckGpioButton;

	BOOL m_bCheckTouchButton;
	int m_nTouchButton_Threshold;
#endif

	// ADDITIONAL ITEM
	BOOL m_bPowerOffWhenUsbDetect;
	int m_nPowerCtrlDelayTime;
	BOOL m_bStartAllTestByStartPin;

	BOOL m_bCheckVirtualDrawTest;

	// DEV_TOOL
	BOOL m_bMeasureLdo;

public :

	HWND m_parentHWnd;

	BOOL m_nNeedResearch;
	UINT m_nCheckHidDeviceCount;

	CString m_strIniErroMsg;

	CBarcodeSettingDlg * m_pBarcodeSettingDlg;

	#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
	CConnectInfoDlg     *	m_pConnectInfoDlg;
	#endif

#if defined(D_MEASURE_ELAPSED_TIME)
	UINT64 m_dt;
#endif

// Operation

public :
	void * GetUsdHidList() { return m_pUsbHidList; }

	int GetAvailableTestStep()	{ return m_nAvailableTestStep; }
	BOOL IsAleadyTestEnter()		{ return m_bAleadyTestEnter; }
	BOOL IsCreatePane()		    	{ return m_bCreatePane; }

	BOOL IsUseStartIndividualTest()	{ return m_bUseStartIndividualTest; }

	TEST_INFO_DND * GetTestInfo_DND()							{ return &m_TestInfo_DND; }
	TEST_INFO_DND *	GetTestInfo_HF_DND()						{ return &m_TestInfo_HF_DND; }

	TEST_INFO_CND* GetTestInfo_CND()							{ return &m_TestInfo_CND; }		// CND	//add 2022.11.14 for ZTM730 CND

	TEST_INFO_SELF * GetTestInfo_SELF_DND()					{ return &m_TestInfo_SELF_DND; }
	TEST_INFO_SELF * GetTestInfo_SELF_SATURATION()			{ return &m_TestInfo_SELF_SATURATION; }

	TEST_INFO_MEASURE_DELTA * GetTestInfo_MEASURE_DELTA()	{ return &m_TestInfo_MEASURE_DELTA; }

	int GetModuleVendorCode() { return m_nModuleVendorCode; }

	void EnterTestModeView();
	void ResetTestCount();

	void DisplayResultToListEx(CUsbHid * pUsbHid, int nResult, LPCTSTR pstrFormat, ...);
	void TSM_MainTest(CUsbHid * pUsbHid);

	BOOL ConfirmStartupPassword();

	CUsbHid * GetUsbHidByIndex(int nIndex);
	CUsbHid * GetUsbHidByDevIndex(int nIndex);

	void PostMessageEx(UINT msg, WPARAM wParam, LPARAM lParam);

	BOOL ReadASICID(CUsbHid * pUsbHid);

	void MpTool_Init(CUsbHid * pUsbHid);
	int MpTool_PowerControl(CUsbHid * pUsbHid, BOOL bOn, BOOL bJumpRom = FALSE);
	int MpTool_PowerSequenceAfterReset(CUsbHid * pUsbHid, BOOL bChecksum = TRUE, BOOL bJumpRom = FALSE);

	void UsbHidDevice_StartSurveyTimer(BOOL bSoon = FALSE);
	void UsbHidDevice_KillSurveyTimer();
	void KillAllTimer(BOOL bTestStart = FALSE);

	BOOL GetUseIFE()	{ return m_bUseIFE; }

protected:

	#define	IFE_MODE_ON		(m_bUseIFE)			// 중국 vs 비중국
	#define	IFE_MODE_OFF	(!IFE_MODE_ON)

	#define	INDIVIDUAL_TEST_START_ON		(m_bUseStartIndividualTest)
	#define	INDIVIDUAL_TEST_START_OFF		(!INDIVIDUAL_TEST_START_ON)

	void EnableStartButton(BOOL bEnable);
	void EnableOptionButton(BOOL bEnable);

	void GotoStandBy();
	void ExitSolutionResetNotify();

	void DeleteTempLoggingFile();

	//-----------------------------------------------
	// List Control

	void ListCtrl_Initialize();

	void AddMessageToList(int nIndex, CString strInfo, int image);
	//void AddMessageOrModifyToList(CUsbHid * pUsbHid, int index, CString strIndex, CString strInfo, int image);
	void DisplayResultToList(CUsbHid * pUsbHid, int nResult);
	void DisplayResultFailToList(CUsbHid * pUsbHid, LPCTSTR pstrFormat, ...);
	void DisplayResultByTestStop(CUsbHid * pUsbHid);
	void ClearTsmTestMessageOnList(int nDeviceIndex);

	int GetImageListIndex(int bCheckTestEnable);
	void DisplayTotalTestStep();

	//-----------------------------------------------
	// INI

	void TestInfo_LoadData();
	void TestInfo_SaveData();

	void TestInfo_LoadTestIni();
	void TestInfo_SaveTestIni();
	void TestInfo_SaveTestIniEx(int nType);

	void BarcodeDisplayInTestVisible();

	BOOL TestInfo_LoadModuleIdList();
	void TestInfo_SaveModuleIdList();

	void TestInfo_LoadTestSpec();
	void TestInfo_SaveTestSpec();

	void TestInfo_Load_Check_PrintPpVariation();

	void LoadSpecData(const char * pSpecFile, int(*pData)[DEF_MAX_Y_LINE_NUM], int nDefaltValue);
	void SaveSpecData(const char * pSpecFile, int(*pData)[DEF_MAX_Y_LINE_NUM]);

	void LoadSpecDataEx(const char * pSpecFile, int * pData, int nDefaltValue);
	void SaveSpecDataEx(const char * pSpecFile, int * pData);

	//-----------------------------------------------
	// TSM TEST

	void TSM_StartTest(int nDeviceIndex);
	void TSM_StartTest_Init(int nDeviceIndex);
	void TSM_StartTest_Individual(int nDeviceIndex);
	void TSM_StartTest_ByBarcode();
	BOOL TSM_StartTest_ByStartPin();

	void TSM_SetUpgradeInfo();
	BOOL TSM_LoadTouchFirmware(CString * strRet);
	BOOL TSM_UpgradeTouchFirmware(CUsbHid * pUsbHid);
	BOOL TSM_VerifyTouchFirmware(CUsbHid * pUsbHid);
	BOOL TSM_CheckEepromBit(CUsbHid * pUsbHid, BOOL bUseNvmPgmVerify);

	BOOL TSM_SubTest(CUsbHid * pUsbHid);
	void TSM_SubTestEnd(CUsbHid * pTestEndUsbHid);
	void TSM_TestEnd();

	BOOL ReadConfigRegister(CUsbHid * pUsbHid);

	BOOL Check_Ic_Signature(CUsbHid * pUsbHid);
	BOOL Check_Interrupt_Pin(CUsbHid * pUsbHid);
	BOOL Check_Reset_Pin(CUsbHid * pUsbHid);

	void TSM_AddData_InLog(CUsbHid * pUsbHid, short int * pRawData, int nTestStep = TEST_STEP_CHECK_DATA);
	void TSM_AddData_InLog_Self(CUsbHid * pUsbHid, short int * pRxRawData, short int * pTxRawData);
	void TSM_AddData_InLog_MeasureDelta(CUsbHid * pUsbHid, short int * pRawData);
	void TSM_AddData_InLog_Noise(CUsbHid * pUsbHid, short int * sNoise);

	//-----------------------------------------------
	// Read/Check Register and raw data

	BOOL ChangeTouchMode(CUsbHid * pUsbHid, int nMode);
	BOOL ReadGarbageData(CUsbHid * pUsbHid, short int *sRawData);

	int  GetTouchMode(CUsbHid * pUsbHid);
	BOOL PrepareToReadRawData(CUsbHid * pUsbHid);
	BOOL PrepareToReadNoise(CUsbHid * pUsbHid, char num);

	void CalculateAfeSetting(CUsbHid * pUsbHid, int nTestStep);
	BOOL ChangeAfeSetting(CUsbHid * pUsbHid);

	// DND
	BOOL ReadRawData(CUsbHid * pUsbHid, short int *sRawData);
	BOOL CheckDNDRawDataRange(CUsbHid * pUsbHid, short int *sRawData);
	BOOL CheckRawDataHDiff(CUsbHid * pUsbHid, short int *sRawData, short int *sDiffData);
	BOOL CheckRawDataVDiff(CUsbHid * pUsbHid, short int *sRawData, short int *sDiffData);

	//CND
	BOOL CheckCNDRawDataRange(CUsbHid* pUsbHid, short int* sRawData);		//add 2022.11.14 for ZTM730 CND

	// HIGH VOLTAGE
	BOOL ReadHighVoltage(CUsbHid* pUsbHid, short int* sRawData, int nTestSubStep);
	BOOL CheckHighVoltageRange(CUsbHid* pUsbHid, s16 *sRawData);

	// HF DND
	BOOL ReadRawData_HfDnd(CUsbHid * pUsbHid, short int *sRawData);
	BOOL CheckRawDataRange_HfDnd(CUsbHid * pUsbHid, short int *sRawData);
	BOOL CheckRawDataHDiff_HfDnd(CUsbHid * pUsbHid, short int *sRawData, short int *sDiffData);
	BOOL CheckRawDataVDiff_HfDnd(CUsbHid * pUsbHid, short int *sRawData, short int *sDiffData);

	// JITTER
	BOOL ReadRawData_JitterN(CUsbHid * pUsbHid, short int *sRawData);
	BOOL ReadRawData_JitterN_ByMPztd(CUsbHid * pUsbHid, short int * pResultData);
#if defined(D_PRODUCT_SOLUTION_ALLROUNDER)
	BOOL ReadRawData_JitterN_AllRounder(CUsbHid * pUsbHid, short int *sRawData);
#endif
	BOOL CheckRawData_JitterMax(CUsbHid * pUsbHid, short int *sRawData);

	// ONLY_SELF_DND
	BOOL ReadRawData_OnlySelfDnd(CUsbHid* pUsbHid, short int* sRawData, int nTestSubStep);
	BOOL CheckRawDataRange_OnlySelfDnd(CUsbHid* pUsbHid, short int* sRawData);

	// SELF_DND
	BOOL ReadRawData_SelfDnd(CUsbHid * pUsbHid, short int *sRawData, int nTestSubStep);
	BOOL CheckRawDataRange_SelfDnd(CUsbHid * pUsbHid, short int *sRxRawData, short int *sTxRawData);
	BOOL CheckRawDataHDiff_SelfDnd(CUsbHid * pUsbHid, short int *sRxRawData, short int *sTxRawData, short int *sDiffData);

	// SELF_SATURATION
	BOOL ReadRawData_SelfSaturation(CUsbHid * pUsbHid, short int *sRawData);
	BOOL CheckRawDataRange_SelfSaturation(CUsbHid * pUsbHid, short int *sRawData);

	// TRX SHORT
	BOOL ReadRawData_TrxShort(CUsbHid * pUsbHid, short int *sRawData);
	BOOL CheckRawDataRange_TrxShort(CUsbHid * pUsbHid, short int *sRawData);

	// Current Consumption
	int TSM_CurrentConsumption(CUsbHid * pUsbHid);

	// HW Calibration
	BOOL TSM_HwCalibration(CUsbHid * pUsbHid);
	BOOL ReadRawData_HwCalibration(CUsbHid * pUsbHid, short int *sRawData);
	BOOL CheckRawDataRange_HwCalibration(CUsbHid * pUsbHid, short int *sRawData);

	// MEASURE DELTA
	BOOL ReadRawData_MeasureDelta(CUsbHid * pUsbHid, short int *sRawData);
	BOOL CheckRawDataRange_MeasureDelta_Condition_AutoStart(CUsbHid * pUsbHid, short int *sRawData_Average);
	BOOL CheckRawDataRange_MeasureDelta_Condition_1(CUsbHid * pUsbHid, short int *sRawData_Average);
	BOOL CheckRawDataRange_MeasureDelta_Condition_2(CUsbHid * pUsbHid, short int *sRawData_SY);
	BOOL CheckRawDataRange_MeasureDelta_Condition_3(CUsbHid * pUsbHid, short int *sRawData_SX);

	BOOL ReadRawData_DigitalWheel(CUsbHid * pUsbHid, short int *sRawData);
	BOOL CheckRawDataRange_DigitalWheel(CUsbHid * pUsbHid, short int *sRawData);

	//-----------------------------------------------
	// NOISE
	BOOL Noise_RequestTest(CUsbHid * pUsbHid, short int *sNoise, char num);
	BOOL CheckNoise(CUsbHid * pUsbHid, short int *sNoise);

	BOOL CheckVirtualDrawTest(CUsbHid* pUsbHid);

	//-----------------------------------------------
	// Button Test
#if defined(D_USE_CHECK_BUTTON_TEST)
	// GPIO BUTTON
	CGpioButtonDlg * m_pGpioButtonDlg;
	BUTTON_STATUS	m_GpioButtonStatus;

	void GpioButton_ShowDialog(BOOL bShow);
	BOOL GpioButton_RequestTest(CUsbHid * pUsbHid);
	BOOL GpioButton_WaitTest(CUsbHid * pUsbHid);

	// TOUCH BUTTON
	CTouchButtonDlg * m_pTouchButtonDlg;
	BUTTON_STATUS	m_TouchButtonStatus;

	void TouchButton_ShowDialog(BOOL bShow);
	BOOL TouchButton_RequestTest(CUsbHid * pUsbHid);
	BOOL TouchButton_WaitTest(CUsbHid * pUsbHid);
	BOOL TouchButton_ReadRawData(CUsbHid * pUsbHid, short int *sRawData);

public:

	BUTTON_STATUS * GetGpioButtonStatus() { return &m_GpioButtonStatus; }
	BUTTON_STATUS * GetTouchButtonStatus() { return &m_TouchButtonStatus; };
#endif

	//-----------------------------------------------
	// Test Result Description
	void TestResultDesc_Initialize();
	int TestResultDesc_GetActiveTabCtrl() { return m_TabCtrl.GetCurSel(); }
	void TestResultDesc_ClearAll();
	void TestResultDesc_SetCurSel(int nIndex);
	CTestResultDescDlg * TestResultDesc_GetHandle(int nDevIndex);

	static int TestResultDesc_GetListState(int nDevIndex);


	CDimTabCtrl 		m_TabCtrl;

	CWnd			*	m_pTabWndShow;

	CTestResultDescDlg			m_TestResultDescDlg[MAX_DEVICE_COUNT];

protected:

	//-----------------------------------------------
	// Save Info

	// EEPROM
	BOOL TSM_SaveEeprom(CUsbHid * pUsbHid);

	// IUM
	BOOL TSM_SaveIUM(CUsbHid * pUsbHid);
	BOOL TSM_SaveIUM_Erase(CUsbHid * pUsbHid);
	BOOL TSM_SaveIUM_Exit(CUsbHid * pUsbHid);
	BOOL TSM_SaveIUM_Enter(CUsbHid * pUsbHid);
	BOOL TSM_SaveIUM_Save(CUsbHid * pUsbHid);
	BOOL TSM_SaveIUM_Verify(CUsbHid * pUsbHid);

	BOOL TSM_VerifyIUM(CUsbHid * pUsbHid);

	void TSM_SaveIUM_SaveFileAll();
	void TSM_SaveIUM_SaveFile(CUsbHid* pUsbHid, CString strDate );

	//-----------------------------------------------
	// LOG & LOGGING FILE
	void TSM_SaveLogFile(CUsbHid * pTestEndUsbHid);
	void TSM_SaveLoggingFile();

	void CheckSameLogFile(CString FileDir, CString TEST_NAME, CString ASIC_ID, CString strLogFile);
	void Rename(CString strOldName, CString strNewName, BOOL bUnlock);

	void UpdateProgressBar(CUsbHid * pCurrUsbHid);
	void UpdateStatusBar(CString * strText);

	BOOL CheckModuleID(CUsbHid * pUsbHid, int nModuleID);

	void Login();

	//BOOL ReadAllRegister(CUsbHid * pUsbHid);
	BOOL ReadTotalLineNumber(s16 * pTotalNumX, s16 * pTotalNumY, s16 * pSelfNumX, s16 * pSelfNumY, u8 * pMaxLineX, u8 * pMaxLineY);
	void SetMaxLineNumber(CUsbHid * pUsbHid, u8 * pLineX, u8 * pLineY);

	//-----------------------------------------------
	// USB HID devices

	HANDLE GetHidHandle(CUsbHid * pUsbHid, char * pDevicePath);
	int GetTestAvailableCount();

	BOOL SurveyUsbHidDevices();
	BOOL FindUsbHid();
	BOOL IsExistUsbHid(CUsbHid * pUsbHid, BOOL bGetHandle = FALSE);
	BOOL UpdateUsbHidDevices(CUsbHid * pUsbHid, int nDevSerial, int nDevIndex, int nMaxDeviceCount, char * pDevicePath);

	void SetListState(CUsbHid * pUsbHid, int nIndex, int nState);

	#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)
	BOOL CheckIcSignature(CUsbHid * pUsbHid);
	void CheckSignatureError(CUsbHid * pUsbHid);
	#endif

	void RemoveHidDevice(CUsbHid * pUsbHid);
	void RemoveAllHidDevice();

	void GetTouchVoltage(CUsbHid * pUsbHid);

	int  GetMaxDeviceCount();
	void CalculateMaxDeviceCount();
	void ChangeDeviceStatus();

	void DisplayMpToolVersion(CUsbHid * pUsbHid);

	// Test Error Description - for LOG and LOGGING file
	void Log_Append(CUsbHid * pUsbHid, int nResult, BOOL bIsLoggingData = FALSE);
	void Log_AppendDiff(CUsbHid * pUsbHid, int nResult, BOOL bIsLoggingData, CString strItem);
	void Log_AppendDesc(CUsbHid * pUsbHid, int nResult, const CString & pAdditional);
	void Log_AppendDescIUM(CUsbHid* pUsbHid, int nResult, const CString& pAdditional, BOOL bTitle);
	void Log_AppendEx(CUsbHid * pUsbHid, LPCTSTR pstrFormat, ...);

	void Logging_AppendEx(CUsbHid * pUsbHid, LPCTSTR pstrFormat, ...);

	// Test Error Description - for display in Test Result Description
	void TestResultDesc_AddMessage(int nDevIndex, COLORREF color, LPCTSTR pstrFormat, ...);
	void TestResultDesc_ClearMessage(int nDevIndex);
	void TestResultDesc_AddTestStep(int nDevIndex, int nTestStep);
	void TestResultDesc_AddResult(int nTestStep);
	void TestResultDesc_AddResultEx(CUsbHid * pUsbHid, int nTestStep);

	int GetBarcodeLength();
	void ProcessKeyToBarcode(MSG* pMsg);
	void BarcodeSetting_ShowDialog(BOOL bShow);
	void BarcodeSetting_TestCompleted(int nIndex, BOOL bError);

	BOOL IsAutoFwDownload_Condition();

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMCustomdrawListInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

	afx_msg void OnBnClickedButtonStartTestAll();
	afx_msg void OnBnClickedButtonOption();

	afx_msg void OnLvnGetdispinfoListInfo(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg LRESULT OnStartTest_ByFwAutoDownload(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnListCtrlColumnClick(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnExecuteAsicIdMode(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnExecuteGraphMode(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnExecuteBarcodeSetting(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCallbackByBarcodeSetting(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnUsbHidDeviceDisconnect(WPARAM wParam, LPARAM lParam);

	afx_msg void TestResultDesc_OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void TestResultDesc_OnSelchangingTab(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg LRESULT OnSetFocusToList(WPARAM wParam, LPARAM lParam);

#ifdef _DEBUG
	afx_msg LRESULT OnMyTestEvent(WPARAM wParam, LPARAM lParam);
#endif
};

