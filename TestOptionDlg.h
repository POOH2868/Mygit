#pragma once

#include "afxwin.h"
#include "Global.h"

#include "util.h"

#include "TestOptionDlg_Tab0.h"
#include "TestOptionDlg_Tab1.h"
#include "TestOptionDlg_Tab2.h"

#include "ScrollDlg.h"

#include "TestModeView_Test.h"
#include "TestModeView_TestInfo.h"

// CTestOptionDlg 대화 상자입니다.

class CTestOptionDlg : public CScrollDlg
{
	DECLARE_DYNAMIC(CTestOptionDlg)

public:
	CTestOptionDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTestOptionDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEST_OPTION };

public:

	CShadeButtonST			m_btnOK;
	CShadeButtonST			m_btnCancel;

	CTabCtrl				m_TabCtrl;
	CTestOptionDlg_Tab0		m_Tab0;
	CTestOptionDlg_Tab1		m_Tab1;
	CTestOptionDlg_Tab2		m_Tab2;

	CWnd			*		m_pTabWndShow;

	static s16 m_icRegData_TotalNumOfX;
	static s16 m_icRegData_TotalNumOfY;

	static s16 m_icRegData_SelfNumOfX;
	static s16 m_icRegData_SelfNumOfY;

	static u8 m_IC_Max_X_Line_Num;
	static u8 m_IC_Max_Y_Line_Num;

	// default
	static BOOL m_bUseIFE;
	static BOOL m_bUseRetry;
	static BOOL m_bUseMptoolver;

	static int m_nIcType;
	static int m_nSlaveAddress;

	static BOOL m_bSaveResult;
	static BOOL m_bSaveFixedPosition;
	static BOOL m_bCreatePassFailDirectory;
	static CString m_strSaveResultDir;
	static int m_nStandBySec;

	static BOOL m_bDownload;
	static BOOL m_bDownloadFixedPosition;
	static int m_nDownloadRetryCount;
	static int m_nAllRetryCount;
	static int m_nMptoolCount;
	static CString m_strFirmwareDir;
	static BOOL m_bSaveInfo;
	static int	m_nSaveInfoLocation;
	static BOOL m_bVerifyFirmware;
	static BOOL m_bBurstUpgrade;

	static CString m_strProjectID;
	static CString m_strDeviceID;

	static int m_nIUMFormat;
	static int m_nI2CFormat;

	static BOOL m_bCheckIntPin;
	static BOOL m_bCheckReset;
	static BOOL m_bCheckResetPin;
	static BOOL m_bCheckVersion;
	static BOOL m_bCheckAutoFwDownload;
	static BOOL m_bCheckIUM;

	static BOOL m_bCheckI2C_Level;
	static CString m_strIcRevision;
	static CString m_strHwID;
	static CString m_strModuleID;
	static CString m_strMajorVersion;
	static CString m_strMinorVersion;
	static CString m_strRegisterVersion;
	static CString m_strCustomer;
	static CString m_strProjectName;

	// barcode
	static BOOL m_bUseBarcode;
	static BOOL m_bAutoStartTest;
	static BOOL m_bBarcodeDisplayInTest;
	static BOOL m_bFixedBarcodeLength;
	static int m_nBarcode_Length;
	static int m_nBarcode_AutoReset;

	// password
	static CString m_strUserAdminPassword;
	static BOOL m_bUseStartup;
	static CString m_strStartupPassword;

	static CString m_strZtdExeDir;

// by Paul {{{
	static BOOL m_bAdditionalLOG;

	static CString m_strTestName;
	static CString m_strTestStage;
	static CString m_strModuleMaker;

	static int m_nInOutType;
// }}}

	// TEST INFO
	static TEST_INFO_DND					m_TestInfo_DND;
	static TEST_INFO_DND					m_TestInfo_HF_DND;
	static TEST_INFO_CND					m_TestInfo_CND;					// CND	//add 2022.11.14 for ZTM730 CND
	static TEST_INFO_HIGH_VOLTAGE			m_TestInfo_HIGH_VOLTAGE;

	static TEST_INFO_SELF					m_TestInfo_SELF_DND;
	static TEST_INFO_ONLYSELF				m_TestInfo_ONLYSELF_DND;		//add 2022.09.16
	static TEST_INFO_SELF					m_TestInfo_SELF_SATURATION;

	static TEST_INFO_MEASURE_DELTA			m_TestInfo_MEASURE_DELTA;
	static TEST_INFO_HW_CALIBRATION			m_TestInfo_HW_CALIBRATION;
	static TEST_INFO_JITTER 				m_TestInfo_JITTER;
	static TEST_INFO_CURRENT_CONSUMPTION 	m_TestInfo_CURRENT_CONSUMPTION;
	static TEST_INFO_DIGITAL_WHEEL			m_TestInfo_DIGITAL_WHEEL;
	static TEST_INFO_NOISE 					m_TestInfo_NOISE;

	static BOOL 							m_bCheckTrxShort;

	static BOOL m_bUseMultiDevice;
	static int  m_nMultiDeviceCount;
	static BOOL m_bUseStartIndividualTest;

	static BOOL m_bCheckUserNumber;

	static BOOL m_bHidMode;

#if defined(D_USE_CHECK_BUTTON_TEST)
	static BOOL m_bCheckGpioButton;

	static BOOL m_bCheckTouchButton;
	static int m_nTouchButton_Threshold;
#endif

	static BOOL m_bCheckVirtualDrawTest;

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnTcnSelchangeTabCtrl(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

	BOOL CheckHexValue(CString str);

public:

	afx_msg void OnBnClickedButtonResetCount();

	static void CheckIfeException();
};

