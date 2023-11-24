// TestOptionDlg_Tab2.cpp : implementation file
//

#include "stdafx.h"
#include "TouchDebugger.h"

#include "TestOptionDlg.h"
#include "TestOptionDlg_Tab2.h"
#include "afxdialogex.h"

#include "TestModeDiffSpecDlg.h"
#include "AfeSettingDlg.h"
#include "AfeNoiseSettingDlg.h"

enum
{
	CTRL_UPDATE_USE_BARCODE					= (1 << 0),
	CTRL_UPDATE_USE_BARCODE_AUTO_START		= (1 << 1),
	CTRL_UPDATE_MEASURE_DELTA 				= (1 << 2),
	CTRL_UPDATE_HW_CALIBRATION				= (1 << 3),
	CTRL_UPDATE_JITTER						= (1 << 4),
	CTRL_UPDATE_TOUCH_BUTTON				= (1 << 5),
	CTRL_UPDATE_DIGITAL_WHEEL				= (1 << 6),
	CTRL_UPDATE_HID_MODE	 				= (1 << 7),
	CTRL_UPDATE_NOISE_TEST					= (1 << 8),
	CTRL_UPDATE_BARCODE_DISPLAY_IN_TEST		= (1 << 9),
};

// CTestOptionDlg_Tab2 dialog

IMPLEMENT_DYNAMIC(CTestOptionDlg_Tab2, CDialog)

CTestOptionDlg_Tab2::CTestOptionDlg_Tab2(CWnd* pParent /*=NULL*/)
	: CDialog(CTestOptionDlg_Tab2::IDD, pParent)
{

}

CTestOptionDlg_Tab2::~CTestOptionDlg_Tab2()
{
}

void CTestOptionDlg_Tab2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	// HID
	DDX_Check(pDX, IDC_CHECK_HID_MODE, CTestOptionDlg::m_bHidMode);

	// BARCODE
	DDX_Check(pDX, IDC_CHECK_USE_BARCODE, CTestOptionDlg::m_bUseBarcode);
	DDX_Check(pDX, IDC_CHECK_AUTO_START_TEST, CTestOptionDlg::m_bAutoStartTest);
	DDX_Check(pDX, IDC_CHECK_BARCODE_DISPLAY_IN_TEST, CTestOptionDlg::m_bBarcodeDisplayInTest);
	DDX_Check(pDX, IDC_CHECK_FIXED_BARCODE_LENGTH, CTestOptionDlg::m_bFixedBarcodeLength);
	DDX_Text(pDX, IDC_BARCODE_LENGTH, CTestOptionDlg::m_nBarcode_Length);
	//DDV_MinMaxInt(pDX, CTestOptionDlg::m_nBarcode_Length, 1, 255);
	DDX_Text(pDX, IDC_EDIT_BARCODE_AUTO_RESET_TIME, CTestOptionDlg::m_nBarcode_AutoReset);

	// MEASURE DELTA
	DDX_Check(pDX, IDC_CHECK_MEASURE_DELTA, CTestOptionDlg::m_TestInfo_MEASURE_DELTA.bCheckData);
	DDX_Check(pDX, IDC_CHECK_MEASURE_DELTA_AUTO_START, CTestOptionDlg::m_TestInfo_MEASURE_DELTA.bCheckAutoStart);
	DDX_Text(pDX, IDC_EDIT_MEASURE_DELTA_AUTO_START_THRESHOLD, CTestOptionDlg::m_TestInfo_MEASURE_DELTA.nAutoStartThreshold);
	DDX_Check(pDX, IDC_CHECK_MEASURE_DELTA_USE_USER_AFE, CTestOptionDlg::m_TestInfo_MEASURE_DELTA.AfeInfo.bUseAfeSetting);
	DDX_Control(pDX, IDC_BUTTON_MEASURE_DELTA_MAX_SPEC, m_btnMeasureDeltaMax);
	DDX_Control(pDX, IDC_BUTTON_MEASURE_DELTA_MIN_SPEC, m_btnMeasureDeltaMin);
	DDX_Control(pDX, IDC_BUTTON_MEASURE_DELTA_SELF_RX_MAX_SPEC, m_btnMeasureDeltaSelfRxMax);
	DDX_Control(pDX, IDC_BUTTON_MEASURE_DELTA_SELF_RX_MIN_SPEC, m_btnMeasureDeltaSelfRxMin);
	DDX_Control(pDX, IDC_BUTTON_MEASURE_DELTA_SELF_TX_MAX_SPEC, m_btnMeasureDeltaSelfTxMax);
	DDX_Control(pDX, IDC_BUTTON_MEASURE_DELTA_SELF_TX_MIN_SPEC, m_btnMeasureDeltaSelfTxMin);

	// HW CALIBRATION
	DDX_Check(pDX, IDC_CHECK_HW_CALIBRATION, CTestOptionDlg::m_TestInfo_HW_CALIBRATION.bCheckData);
	DDX_Text(pDX, IDC_EDIT_HW_CALIBRATION_MAX, CTestOptionDlg::m_TestInfo_HW_CALIBRATION.nHwCalibrationMax);
	DDX_Check(pDX, IDC_CHECK_HW_CALIBRATION_USE_USER_AFE, CTestOptionDlg::m_TestInfo_HW_CALIBRATION.AfeInfo.bUseAfeSetting);

	// JITTER
	DDX_Check(pDX, IDC_CHECK_JITTER, CTestOptionDlg::m_TestInfo_JITTER.bCheckData);
	DDX_Text(pDX, IDC_EDIT_JITTER_DATA_MAX, CTestOptionDlg::m_TestInfo_JITTER.nJitterDataMax);
	DDX_Check(pDX, IDC_CHECK_JITTER_USE_USER_AFE, CTestOptionDlg::m_TestInfo_JITTER.AfeInfo.bUseAfeSetting);

	// Change AFE SEtting
	DDX_Control(pDX, IDC_BUTTON_MEASURE_DELTA_CHANGE_USER_AFE, m_btnMeasureDeltaAfe);
	DDX_Control(pDX, IDC_BUTTON_HW_CALIBRATION_CHANGE_USER_AFE, m_btnHwCalAfe);
	DDX_Control(pDX, IDC_BUTTON_JITTER_CHANGE_USER_AFE, m_btnJitterAfe);

	// DIGITAL WHEEL
	DDX_Check(pDX, IDC_CHECK_DIGITAL_WHEEL, CTestOptionDlg::m_TestInfo_DIGITAL_WHEEL.bCheckData);
	DDX_Control(pDX, IDC_BUTTON_DIGITAL_WHEEL_MAX_SPEC, m_btnDigitalWheelDeltaMax);
	DDX_Control(pDX, IDC_BUTTON_DIGITAL_WHEEL_MIN_SPEC, m_btnDigitalWheelDeltaMin);

#if defined(D_USE_CHECK_BUTTON_TEST)
	DDX_Check(pDX, IDC_CHECK_GPIO_BUTTON_TEST, CTestOptionDlg::m_bCheckGpioButton);

	DDX_Check(pDX, IDC_CHECK_TOUCH_BUTTON_TEST, CTestOptionDlg::m_bCheckTouchButton);
	DDX_Text(pDX, IDC_EDIT_TOUCH_BUTTON_THRESOLD, CTestOptionDlg::m_nTouchButton_Threshold);
#endif

	// NOISE CHECK
	DDX_Check(pDX, IDC_CHECK_NOISE_TEST, CTestOptionDlg::m_TestInfo_NOISE.m_bCheckNoise);
	DDX_Check(pDX, IDC_CHECK_NOISE_USE_USER_AFE, CTestOptionDlg::m_TestInfo_NOISE.m_bCheckNoiseUserAFE);
	DDX_Text(pDX, IDC_EDIT_NOISE_THRESOLD, CTestOptionDlg::m_TestInfo_NOISE.m_nCheckNoise_Threshold);
	DDX_Text(pDX, IDC_EDIT_NOISE_TIMES, CTestOptionDlg::m_TestInfo_NOISE.m_nCheckNoise_ReadTimes);
	DDX_Text(pDX, IDC_EDIT_NOISE_INTERVAL, CTestOptionDlg::m_TestInfo_NOISE.m_nCheckNoise_Interval);
	DDX_Control(pDX, IDC_BUTTON_NOISE_USER_AFE, m_btnNoiseAfe);

	// VIRTUAL_DRAW_TEST
	DDX_Check(pDX, IDC_CHECK_VIRTUAL_DRAW_TEST, CTestOptionDlg::m_bCheckVirtualDrawTest);
}


BEGIN_MESSAGE_MAP(CTestOptionDlg_Tab2, CDialog)
	ON_WM_CTLCOLOR()

	// HID
	ON_BN_CLICKED(IDC_CHECK_HID_MODE, &CTestOptionDlg_Tab2::OnCheckBnClicked)

	// BARCODE
	ON_BN_CLICKED(IDC_CHECK_USE_BARCODE, &CTestOptionDlg_Tab2::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_AUTO_START_TEST, &CTestOptionDlg_Tab2::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_FIXED_BARCODE_LENGTH, &CTestOptionDlg_Tab2::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_BARCODE_DISPLAY_IN_TEST, &CTestOptionDlg_Tab2::OnCheckBnClicked)

	// MEASURE DELTA
	ON_BN_CLICKED(IDC_CHECK_MEASURE_DELTA, &CTestOptionDlg_Tab2::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_MEASURE_DELTA_AUTO_START, &CTestOptionDlg_Tab2::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_MEASURE_DELTA_USE_USER_AFE, &CTestOptionDlg_Tab2::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_BUTTON_MEASURE_DELTA_MIN_SPEC, &CTestOptionDlg_Tab2::OnBnClicked_MeasureDelta_ModifyMinSpec)
	ON_BN_CLICKED(IDC_BUTTON_MEASURE_DELTA_MAX_SPEC, &CTestOptionDlg_Tab2::OnBnClicked_MeasureDelta_ModifyMaxSpec)
	ON_BN_CLICKED(IDC_BUTTON_MEASURE_DELTA_SELF_RX_MIN_SPEC, &CTestOptionDlg_Tab2::OnBnClicked_MeasureDelta_ModifySelfRxMinSpec)
	ON_BN_CLICKED(IDC_BUTTON_MEASURE_DELTA_SELF_RX_MAX_SPEC, &CTestOptionDlg_Tab2::OnBnClicked_MeasureDelta_ModifySelfRxMaxSpec)
	ON_BN_CLICKED(IDC_BUTTON_MEASURE_DELTA_SELF_TX_MIN_SPEC, &CTestOptionDlg_Tab2::OnBnClicked_MeasureDelta_ModifySelfTxMinSpec)
	ON_BN_CLICKED(IDC_BUTTON_MEASURE_DELTA_SELF_TX_MAX_SPEC, &CTestOptionDlg_Tab2::OnBnClicked_MeasureDelta_ModifySelfTxMaxSpec)

	// HW CALIBRATION
	ON_BN_CLICKED(IDC_CHECK_HW_CALIBRATION, &CTestOptionDlg_Tab2::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_HW_CALIBRATION_USE_USER_AFE, &CTestOptionDlg_Tab2::OnCheckBnClicked)

	// JITTER
	ON_BN_CLICKED(IDC_CHECK_JITTER, &CTestOptionDlg_Tab2::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_JITTER_USE_USER_AFE, &CTestOptionDlg_Tab2::OnCheckBnClicked)

	ON_BN_CLICKED(IDC_BUTTON_MEASURE_DELTA_CHANGE_USER_AFE, &CTestOptionDlg_Tab2::OnBnClicked_ChangeUserAFE)
	ON_BN_CLICKED(IDC_BUTTON_HW_CALIBRATION_CHANGE_USER_AFE, &CTestOptionDlg_Tab2::OnBnClicked_ChangeUserAFE)
	ON_BN_CLICKED(IDC_BUTTON_JITTER_CHANGE_USER_AFE, &CTestOptionDlg_Tab2::OnBnClicked_ChangeUserAFE)

	// DIGITAL WHEEL
	ON_BN_CLICKED(IDC_CHECK_DIGITAL_WHEEL, &CTestOptionDlg_Tab2::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_BUTTON_DIGITAL_WHEEL_MAX_SPEC, &CTestOptionDlg_Tab2::OnBnClicked_DigitalWheel_ModifyMaxSpec)
	ON_BN_CLICKED(IDC_BUTTON_DIGITAL_WHEEL_MIN_SPEC, &CTestOptionDlg_Tab2::OnBnClicked_DigitalWheel_ModifyMinSpec)

#if defined(D_USE_CHECK_BUTTON_TEST)
	ON_BN_CLICKED(IDC_CHECK_GPIO_BUTTON_TEST, &CTestOptionDlg_Tab2::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_TOUCH_BUTTON_TEST, &CTestOptionDlg_Tab2::OnCheckBnClicked)
#endif

	// NOISE CHECK
	ON_BN_CLICKED(IDC_CHECK_NOISE_TEST, &CTestOptionDlg_Tab2::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_NOISE_USE_USER_AFE, &CTestOptionDlg_Tab2::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_BUTTON_NOISE_USER_AFE, &CTestOptionDlg_Tab2::OnBnClickedButtonNoiseUserAfe)

	// SRAM TEST
	ON_BN_CLICKED(IDC_CHECK_VIRTUAL_DRAW_TEST, &CTestOptionDlg_Tab2::OnCheckBnClicked)
END_MESSAGE_MAP()


// CTestOptionDlg_Tab2 message handlers

BOOL CTestOptionDlg_Tab2::PreTranslateMessage(MSG* pMsg)
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

BOOL CTestOptionDlg_Tab2::OnInitDialog()
{
	CDialog::OnInitDialog();

	EnableDialogControls(0xFFFFFFFF);

	InitUiContorl();

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CTestOptionDlg_Tab2::InitUiContorl()
{
	SetButtonConfig(&m_btnMeasureDeltaMax);
	SetButtonConfig(&m_btnMeasureDeltaMin);
	SetButtonConfig(&m_btnMeasureDeltaSelfRxMax);
	SetButtonConfig(&m_btnMeasureDeltaSelfRxMin);
	SetButtonConfig(&m_btnMeasureDeltaSelfTxMax);
	SetButtonConfig(&m_btnMeasureDeltaSelfTxMin);
	SetButtonConfig(&m_btnMeasureDeltaAfe);

	SetButtonConfig(&m_btnHwCalAfe);
	SetButtonConfig(&m_btnJitterAfe);

	SetButtonConfig(&m_btnDigitalWheelDeltaMax);
	SetButtonConfig(&m_btnDigitalWheelDeltaMin);

	SetButtonConfig(&m_btnNoiseAfe);

	SetWindowTheme(GetDlgItem(IDC_CHECK_HID_MODE)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_CHECK_MEASURE_DELTA)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_MEASURE_DELTA_AUTO_START)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_MEASURE_DELTA_USE_USER_AFE)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_CHECK_HW_CALIBRATION)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_CHECK_JITTER)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_JITTER_USE_USER_AFE)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_CHECK_USE_BARCODE)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_AUTO_START_TEST)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_FIXED_BARCODE_LENGTH)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_CHECK_DIGITAL_WHEEL)->m_hWnd, L"", L"");

#if defined(D_USE_CHECK_BUTTON_TEST)
	SetWindowTheme(GetDlgItem(IDC_CHECK_GPIO_BUTTON_TEST)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_TOUCH_BUTTON_TEST)->m_hWnd, L"", L"");
#endif

	SetWindowTheme(GetDlgItem(IDC_CHECK_NOISE_TEST)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_NOISE_USE_USER_AFE)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_CHECK_VIRTUAL_DRAW_TEST)->m_hWnd, L"", L"");
}

void CTestOptionDlg_Tab2::OnCheckBnClicked()
{
	UpdateData(TRUE);

	int nID = GetFocus()->GetDlgCtrlID();
	int nUpdateID = 0xFFFF;

	if( nID == IDC_CHECK_USE_BARCODE || nID == IDC_CHECK_FIXED_BARCODE_LENGTH )
	{
		nUpdateID = CTRL_UPDATE_USE_BARCODE;
	}
	else if (nID == IDC_CHECK_BARCODE_DISPLAY_IN_TEST)
	{
		nUpdateID = CTRL_UPDATE_BARCODE_DISPLAY_IN_TEST;
	}
	else if( nID == IDC_CHECK_AUTO_START_TEST )
	{
		nUpdateID = CTRL_UPDATE_USE_BARCODE_AUTO_START;
	}
	else if( nID == IDC_CHECK_MEASURE_DELTA || nID == IDC_CHECK_MEASURE_DELTA_USE_USER_AFE || nID == IDC_CHECK_MEASURE_DELTA_AUTO_START )
	{
		nUpdateID = CTRL_UPDATE_MEASURE_DELTA;

		if( nID == IDC_CHECK_MEASURE_DELTA && CTestOptionDlg::m_TestInfo_MEASURE_DELTA.bCheckData == FALSE)
		{
			CTestOptionDlg::m_TestInfo_MEASURE_DELTA.AfeInfo.bUseAfeSetting = FALSE;
			UpdateData(FALSE);
		}
	}
	else if( nID == IDC_CHECK_HW_CALIBRATION || nID == IDC_CHECK_HW_CALIBRATION_USE_USER_AFE )
	{
		nUpdateID = CTRL_UPDATE_HW_CALIBRATION;

		if( nID == IDC_CHECK_HW_CALIBRATION && CTestOptionDlg::m_TestInfo_HW_CALIBRATION.bCheckData == FALSE)
		{
			CTestOptionDlg::m_TestInfo_HW_CALIBRATION.AfeInfo.bUseAfeSetting = FALSE;
			UpdateData(FALSE);
		}
	}
	else if( nID == IDC_CHECK_JITTER || nID == IDC_CHECK_JITTER_USE_USER_AFE )
	{
		nUpdateID = CTRL_UPDATE_JITTER;

		if( nID == IDC_CHECK_JITTER && CTestOptionDlg::m_TestInfo_JITTER.bCheckData == FALSE)
		{
			CTestOptionDlg::m_TestInfo_JITTER.AfeInfo.bUseAfeSetting = FALSE;
			UpdateData(FALSE);
		}
	}
	else if( nID == IDC_CHECK_TOUCH_BUTTON_TEST )
	{
		nUpdateID = CTRL_UPDATE_TOUCH_BUTTON;
	}
	else if( nID == IDC_CHECK_DIGITAL_WHEEL )
	{
		nUpdateID = CTRL_UPDATE_DIGITAL_WHEEL;
	}
	else if( nID == IDC_CHECK_HID_MODE )
	{
		nUpdateID = CTRL_UPDATE_HID_MODE;
	}
	else if (nID == IDC_CHECK_NOISE_TEST || nID == IDC_CHECK_NOISE_USE_USER_AFE)
	{
		nUpdateID = CTRL_UPDATE_NOISE_TEST;
		if (nID == IDC_CHECK_NOISE_TEST && CTestOptionDlg::m_TestInfo_NOISE.m_bCheckNoise == FALSE)
		{
			CTestOptionDlg::m_TestInfo_NOISE.m_bCheckNoiseUserAFE = FALSE;
			UpdateData(FALSE);
		}
	}

	EnableDialogControls(nUpdateID);

	GetDlgItem(nID)->Invalidate();
}

// MEASURE DELTA
void CTestOptionDlg_Tab2::OnBnClicked_MeasureDelta_ModifyMaxSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_MEASURE_DELTA.pDataMax[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_MEASURE_DELTA.nDataMax;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY;

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_MEASURE_DELTA.pDataMax[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_MEASURE_DELTA.nDataMax = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab2::OnBnClicked_MeasureDelta_ModifyMinSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_MEASURE_DELTA.pDataMin[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_MEASURE_DELTA.nDataMin;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY;

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_MEASURE_DELTA.pDataMin[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_MEASURE_DELTA.nDataMin = dlg.m_nApplyValue;
}

// MEASURE DELTA - SELF
void CTestOptionDlg_Tab2::OnBnClicked_MeasureDelta_ModifySelfRxMaxSpec()
{
	CTestModeDiffSpecDlg dlg;
	int y;

	for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
		dlg.nDiff[0][y] = CTestOptionDlg::m_TestInfo_MEASURE_DELTA.pSelfRxDataMax[y];
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_MEASURE_DELTA.m_nSelfRxDataMax;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY;

	dlg.bSelf = TRUE;

	if (dlg.DoModal() != IDOK)
		return;

	for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
		CTestOptionDlg::m_TestInfo_MEASURE_DELTA.pSelfRxDataMax[y] = dlg.nDiff[0][y];
	}

	CTestOptionDlg::m_TestInfo_MEASURE_DELTA.m_nSelfRxDataMax = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab2::OnBnClicked_MeasureDelta_ModifySelfRxMinSpec()
{
	CTestModeDiffSpecDlg dlg;
	int y;

	for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
		dlg.nDiff[0][y] = CTestOptionDlg::m_TestInfo_MEASURE_DELTA.pSelfRxDataMin[y];
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_MEASURE_DELTA.m_nSelfRxDataMin;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY;

	dlg.bSelf = TRUE;

	if (dlg.DoModal() != IDOK)
		return;

	for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
		CTestOptionDlg::m_TestInfo_MEASURE_DELTA.pSelfRxDataMin[y] = dlg.nDiff[0][y];
	}

	CTestOptionDlg::m_TestInfo_MEASURE_DELTA.m_nSelfRxDataMin = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab2::OnBnClicked_MeasureDelta_ModifySelfTxMaxSpec()
{
	CTestModeDiffSpecDlg dlg;
	int y;

	for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
		dlg.nDiff[0][y] = CTestOptionDlg::m_TestInfo_MEASURE_DELTA.pSelfTxDataMax[y];
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_MEASURE_DELTA.m_nSelfTxDataMax;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY;

	dlg.bSelf = TRUE;

	if (dlg.DoModal() != IDOK)
		return;

	for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
		CTestOptionDlg::m_TestInfo_MEASURE_DELTA.pSelfTxDataMax[y] = dlg.nDiff[0][y];
	}

	CTestOptionDlg::m_TestInfo_MEASURE_DELTA.m_nSelfTxDataMax = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab2::OnBnClicked_MeasureDelta_ModifySelfTxMinSpec()
{
	CTestModeDiffSpecDlg dlg;
	int y;

	for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
		dlg.nDiff[0][y] = CTestOptionDlg::m_TestInfo_MEASURE_DELTA.pSelfTxDataMin[y];
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_MEASURE_DELTA.m_nSelfTxDataMin;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY;

	dlg.bSelf = TRUE;

	if (dlg.DoModal() != IDOK)
		return;

	for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
		CTestOptionDlg::m_TestInfo_MEASURE_DELTA.pSelfTxDataMin[y] = dlg.nDiff[0][y];
	}

	CTestOptionDlg::m_TestInfo_MEASURE_DELTA.m_nSelfTxDataMin = dlg.m_nApplyValue;
}

// DIGITAL WHEEL
void CTestOptionDlg_Tab2::OnBnClicked_DigitalWheel_ModifyMaxSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_DIGITAL_WHEEL.pDataMax[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_DIGITAL_WHEEL.nDataMax;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY;

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_DIGITAL_WHEEL.pDataMax[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_DIGITAL_WHEEL.nDataMax = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab2::OnBnClicked_DigitalWheel_ModifyMinSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_DIGITAL_WHEEL.pDataMin[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_DIGITAL_WHEEL.nDataMin;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY;

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_DIGITAL_WHEEL.pDataMin[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_DIGITAL_WHEEL.nDataMin = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab2::OnBnClicked_ChangeUserAFE()
{
	int nID = GetFocus()->GetDlgCtrlID();

	if (nID == IDC_BUTTON_MEASURE_DELTA_CHANGE_USER_AFE )
	{
		T_AFE_SETTING * pAfeSetting = &CTestOptionDlg::m_TestInfo_MEASURE_DELTA.AfeInfo;

		CAfeSettingDlg dlg;

		dlg.m_nFrequency 	= pAfeSetting->nFrequency;
		dlg.m_nNCount 		= pAfeSetting->nNCount;
		dlg.m_nUCount 		= pAfeSetting->nUCount;

		if (dlg.DoModal() != IDOK)
			return;

		pAfeSetting->nFrequency = dlg.m_nFrequency;
		pAfeSetting->nNCount 	= dlg.m_nNCount;
		pAfeSetting->nUCount 	= dlg.m_nUCount;
	}
	else
	{
		T_AFE_SETTING * pAfeSetting = NULL;

		if (nID == IDC_BUTTON_HW_CALIBRATION_CHANGE_USER_AFE )
		{
			pAfeSetting = &CTestOptionDlg::m_TestInfo_HW_CALIBRATION.AfeInfo;
		}
		else if (nID == IDC_BUTTON_JITTER_CHANGE_USER_AFE )
		{
			pAfeSetting = &CTestOptionDlg::m_TestInfo_JITTER.AfeInfo;
		}

		if (pAfeSetting == NULL)
			return;

		CAfeSettingDlg dlg;

		dlg.m_nFrequency 	= pAfeSetting->nFrequency;
		dlg.m_nNCount 		= pAfeSetting->nNCount;
		dlg.m_nUCount 		= pAfeSetting->nUCount;

		if (dlg.DoModal() != IDOK)
			return;

		pAfeSetting->nFrequency = dlg.m_nFrequency;
		pAfeSetting->nNCount 	= dlg.m_nNCount;
		pAfeSetting->nUCount 	= dlg.m_nUCount;
	}
}

void CTestOptionDlg_Tab2::EnableDialogControls(int nUpdateID)
{
	UpdateData(TRUE);

	BOOL bEnableIFE = CTestOptionDlg::m_bUseIFE;

	if( nUpdateID & CTRL_UPDATE_USE_BARCODE )
	{
		BOOL bUseBarcode = CTestOptionDlg::m_bUseBarcode;

		GetDlgItem(IDC_CHECK_AUTO_START_TEST)->EnableWindow(bUseBarcode);
		GetDlgItem(IDC_CHECK_FIXED_BARCODE_LENGTH)->EnableWindow(bUseBarcode);
		GetDlgItem(IDC_STATIC_BARCODE_LENGTH)->EnableWindow(bUseBarcode);
		GetDlgItem(IDC_BARCODE_LENGTH)->EnableWindow(bUseBarcode);
		GetDlgItem(IDC_CHECK_BARCODE_DISPLAY_IN_TEST)->EnableWindow(bUseBarcode);
		GetDlgItem(IDC_STATIC_BARCODE_AUTO_RESET_TIME)->EnableWindow(bUseBarcode);
		GetDlgItem(IDC_EDIT_BARCODE_AUTO_RESET_TIME)->EnableWindow(bUseBarcode);
		GetDlgItem(IDC_STATIC_BARCODE_AUTO_RESET_TIME_SEC)->EnableWindow(bUseBarcode);

		if( bUseBarcode && CTestOptionDlg::m_bAutoStartTest )
		{
			CTestOptionDlg::m_bFixedBarcodeLength = TRUE;
			UpdateData(FALSE);

			GetDlgItem(IDC_CHECK_FIXED_BARCODE_LENGTH)->Invalidate();
			GetDlgItem(IDC_CHECK_FIXED_BARCODE_LENGTH)->EnableWindow(FALSE);
		}

		GetDlgItem(IDC_STATIC_BARCODE_AUTO_RESET_TIME)->EnableWindow(bUseBarcode && CTestOptionDlg::m_bFixedBarcodeLength);
		GetDlgItem(IDC_EDIT_BARCODE_AUTO_RESET_TIME)->EnableWindow(bUseBarcode && CTestOptionDlg::m_bFixedBarcodeLength);
		GetDlgItem(IDC_STATIC_BARCODE_AUTO_RESET_TIME_SEC)->EnableWindow(bUseBarcode && CTestOptionDlg::m_bFixedBarcodeLength);
	}

	if( nUpdateID & CTRL_UPDATE_USE_BARCODE_AUTO_START )
	{
		if(CTestOptionDlg::m_bAutoStartTest)
		{
			CTestOptionDlg::m_bFixedBarcodeLength = TRUE;
			UpdateData(FALSE);

			GetDlgItem(IDC_CHECK_FIXED_BARCODE_LENGTH)->Invalidate();
			GetDlgItem(IDC_CHECK_FIXED_BARCODE_LENGTH)->EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_CHECK_FIXED_BARCODE_LENGTH)->EnableWindow(CTestOptionDlg::m_bUseBarcode);
		}
	}

	if( nUpdateID & CTRL_UPDATE_MEASURE_DELTA )
	{
		BOOL bCheckMeasureDelta = CTestOptionDlg::m_TestInfo_MEASURE_DELTA.bCheckData;
		BOOL bCheckMeasureDeltaAutoStart = bCheckMeasureDelta && CTestOptionDlg::m_TestInfo_MEASURE_DELTA.bCheckAutoStart;

		GetDlgItem(IDC_CHECK_MEASURE_DELTA_AUTO_START)->EnableWindow(bCheckMeasureDelta);

		GetDlgItem(IDC_STATIC_MEASURE_DELTA_AUTO_START_THRESHOLD)->EnableWindow(bCheckMeasureDeltaAutoStart);
		GetDlgItem(IDC_EDIT_MEASURE_DELTA_AUTO_START_THRESHOLD)->EnableWindow(bCheckMeasureDeltaAutoStart);

		GetDlgItem(IDC_BUTTON_MEASURE_DELTA_MAX_SPEC)->EnableWindow(bCheckMeasureDelta);
		GetDlgItem(IDC_BUTTON_MEASURE_DELTA_MIN_SPEC)->EnableWindow(bCheckMeasureDelta);

		GetDlgItem(IDC_BUTTON_MEASURE_DELTA_SELF_RX_MAX_SPEC)->EnableWindow(bCheckMeasureDelta);
		GetDlgItem(IDC_BUTTON_MEASURE_DELTA_SELF_RX_MIN_SPEC)->EnableWindow(bCheckMeasureDelta);

		GetDlgItem(IDC_BUTTON_MEASURE_DELTA_SELF_TX_MAX_SPEC)->EnableWindow(bCheckMeasureDelta);
		GetDlgItem(IDC_BUTTON_MEASURE_DELTA_SELF_TX_MIN_SPEC)->EnableWindow(bCheckMeasureDelta);

		GetDlgItem(IDC_CHECK_MEASURE_DELTA_USE_USER_AFE)->EnableWindow(bEnableIFE && bCheckMeasureDelta);
		GetDlgItem(IDC_BUTTON_MEASURE_DELTA_CHANGE_USER_AFE)->EnableWindow(bEnableIFE && bCheckMeasureDelta && CTestOptionDlg::m_TestInfo_MEASURE_DELTA.AfeInfo.bUseAfeSetting);
	}

	if( nUpdateID & CTRL_UPDATE_HW_CALIBRATION )
	{
		GetDlgItem(IDC_CHECK_HW_CALIBRATION)->EnableWindow(bEnableIFE);

		BOOL bCheckHwCal = bEnableIFE && CTestOptionDlg::m_TestInfo_HW_CALIBRATION.bCheckData;

		GetDlgItem(IDC_CHECK_HW_CALIBRATION_USE_USER_AFE)->EnableWindow(bCheckHwCal);
		GetDlgItem(IDC_BUTTON_HW_CALIBRATION_CHANGE_USER_AFE)->EnableWindow(bCheckHwCal && CTestOptionDlg::m_TestInfo_HW_CALIBRATION.AfeInfo.bUseAfeSetting);

		GetDlgItem(IDC_STATIC_HW_CALIBRATION_MAX)->EnableWindow(bCheckHwCal);
		GetDlgItem(IDC_EDIT_HW_CALIBRATION_MAX)->EnableWindow(bCheckHwCal);
	}

	if( nUpdateID & CTRL_UPDATE_JITTER )
	{
		BOOL bCheckJitter = CTestOptionDlg::m_TestInfo_JITTER.bCheckData;

		if (CTestOptionDlg::m_bUseIFE == FALSE && CTestOptionDlg::m_TestInfo_JITTER.bCheckDataByMpztd == FALSE)
		{
			GetDlgItem(IDC_CHECK_JITTER)->EnableWindow(FALSE);

			bCheckJitter = FALSE;
			CTestOptionDlg::m_TestInfo_JITTER.bCheckData = FALSE;
			UpdateData(FALSE);
		}
		else
			GetDlgItem(IDC_CHECK_JITTER)->EnableWindow(TRUE);

		GetDlgItem(IDC_CHECK_JITTER_USE_USER_AFE)->EnableWindow(bCheckJitter);
		GetDlgItem(IDC_BUTTON_JITTER_CHANGE_USER_AFE)->EnableWindow(bCheckJitter && CTestOptionDlg::m_TestInfo_JITTER.AfeInfo.bUseAfeSetting);

		GetDlgItem(IDC_STATIC_JITTER_SPEC_MAX)->EnableWindow(bCheckJitter);
		GetDlgItem(IDC_EDIT_JITTER_DATA_MAX)->EnableWindow(bCheckJitter);
	}

	if( nUpdateID & CTRL_UPDATE_TOUCH_BUTTON )
	{
		if(CTestOptionDlg::m_bUseStartIndividualTest)
		{
			CTestOptionDlg::m_bCheckGpioButton = FALSE;
			CTestOptionDlg::m_bCheckTouchButton = FALSE;
			UpdateData(FALSE);
		}

		GetDlgItem(IDC_CHECK_GPIO_BUTTON_TEST)->EnableWindow(CTestOptionDlg::m_bUseStartIndividualTest ? FALSE : TRUE);
		GetDlgItem(IDC_CHECK_TOUCH_BUTTON_TEST)->EnableWindow(CTestOptionDlg::m_bUseStartIndividualTest ? FALSE : TRUE);

		BOOL bCheckTouchButton = CTestOptionDlg::m_bCheckTouchButton;

		GetDlgItem(IDC_STATIC_TOUCH_BUTTON_THRESOLD)->EnableWindow(bCheckTouchButton);
		GetDlgItem(IDC_EDIT_TOUCH_BUTTON_THRESOLD)->EnableWindow(bCheckTouchButton);
	}

	if( nUpdateID & CTRL_UPDATE_DIGITAL_WHEEL )
	{
		GetDlgItem(IDC_CHECK_DIGITAL_WHEEL)->EnableWindow(bEnableIFE);

		BOOL bCheckDigitalWheel = bEnableIFE && CTestOptionDlg::m_TestInfo_DIGITAL_WHEEL.bCheckData;

		GetDlgItem(IDC_BUTTON_DIGITAL_WHEEL_MAX_SPEC)->EnableWindow(bCheckDigitalWheel);
		GetDlgItem(IDC_BUTTON_DIGITAL_WHEEL_MIN_SPEC)->EnableWindow(bCheckDigitalWheel);
	}

	if( nUpdateID & CTRL_UPDATE_HID_MODE )
	{
		// Nothing to do...
	}

	if (nUpdateID & CTRL_UPDATE_NOISE_TEST)
	{
		BOOL bCheckNoise = CTestOptionDlg::m_TestInfo_NOISE.m_bCheckNoise;
		BOOL bCheckUserAFE = bCheckNoise && CTestOptionDlg::m_TestInfo_NOISE.m_bCheckNoiseUserAFE;

		GetDlgItem(IDC_STATIC_NOISE_THRESOLD)->EnableWindow(bCheckNoise);
		GetDlgItem(IDC_EDIT_NOISE_THRESOLD)->EnableWindow(bCheckNoise);
		GetDlgItem(IDC_STATIC_NOISE_READ_TIMES)->EnableWindow(bCheckNoise);
		GetDlgItem(IDC_EDIT_NOISE_TIMES)->EnableWindow(bCheckNoise);
		GetDlgItem(IDC_STATIC_NOISE_INTERVAL)->EnableWindow(bCheckNoise);
		GetDlgItem(IDC_EDIT_NOISE_INTERVAL)->EnableWindow(bCheckNoise);
		GetDlgItem(IDC_STATIC_NOISE_INTERVAL_UNIT)->EnableWindow(bCheckNoise);

		GetDlgItem(IDC_CHECK_NOISE_USE_USER_AFE)->EnableWindow(bCheckNoise);
		GetDlgItem(IDC_BUTTON_NOISE_USER_AFE)->EnableWindow(bCheckUserAFE);
	}
}

HBRUSH CTestOptionDlg_Tab2::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor != CTLCOLOR_STATIC)
		return hbr;

	int nState = -1;

	switch (pWnd->GetDlgCtrlID())
	{
	case IDC_CHECK_HID_MODE:					nState = CTestOptionDlg::m_bHidMode; break;
	case IDC_CHECK_USE_BARCODE:					nState = CTestOptionDlg::m_bUseBarcode;	break;
	case IDC_CHECK_BARCODE_DISPLAY_IN_TEST:		nState = CTestOptionDlg::m_bUseBarcode && CTestOptionDlg::m_bBarcodeDisplayInTest;	break;
	case IDC_CHECK_AUTO_START_TEST:				nState = CTestOptionDlg::m_bUseBarcode && CTestOptionDlg::m_bAutoStartTest;	break;
	case IDC_CHECK_FIXED_BARCODE_LENGTH:		nState = CTestOptionDlg::m_bUseBarcode && CTestOptionDlg::m_bFixedBarcodeLength;	break;

	case IDC_CHECK_MEASURE_DELTA:				nState = CTestOptionDlg::m_TestInfo_MEASURE_DELTA.bCheckData;	break;
	case IDC_CHECK_MEASURE_DELTA_AUTO_START:	nState = CTestOptionDlg::m_TestInfo_MEASURE_DELTA.bCheckAutoStart;	break;
	case IDC_CHECK_MEASURE_DELTA_USE_USER_AFE:	nState = CTestOptionDlg::m_TestInfo_MEASURE_DELTA.bCheckData && CTestOptionDlg::m_TestInfo_MEASURE_DELTA.AfeInfo.bUseAfeSetting;	break;

	case IDC_CHECK_HW_CALIBRATION:				nState = CTestOptionDlg::m_TestInfo_HW_CALIBRATION.bCheckData;	break;
	case IDC_CHECK_HW_CALIBRATION_USE_USER_AFE: nState = CTestOptionDlg::m_TestInfo_HW_CALIBRATION.bCheckData && CTestOptionDlg::m_TestInfo_HW_CALIBRATION.AfeInfo.bUseAfeSetting;	break;

	case IDC_CHECK_JITTER:						nState = CTestOptionDlg::m_TestInfo_JITTER.bCheckData;	break;
	case IDC_CHECK_JITTER_USE_USER_AFE:			nState = CTestOptionDlg::m_TestInfo_JITTER.AfeInfo.bUseAfeSetting;	break;

	case IDC_CHECK_DIGITAL_WHEEL:				nState = CTestOptionDlg::m_TestInfo_DIGITAL_WHEEL.bCheckData;	break;

#if defined(D_USE_CHECK_BUTTON_TEST)
	case IDC_CHECK_GPIO_BUTTON_TEST:			nState = CTestOptionDlg::m_bCheckGpioButton;	break;
	case IDC_CHECK_TOUCH_BUTTON_TEST:			nState = CTestOptionDlg::m_bCheckTouchButton;	break;
#endif

	case IDC_CHECK_NOISE_TEST:					nState = CTestOptionDlg::m_TestInfo_NOISE.m_bCheckNoise;	break;
	case IDC_CHECK_NOISE_USE_USER_AFE:			nState = CTestOptionDlg::m_TestInfo_NOISE.m_bCheckNoise && CTestOptionDlg::m_TestInfo_NOISE.m_bCheckNoiseUserAFE;	break;

	case IDC_CHECK_VIRTUAL_DRAW_TEST:			nState = CTestOptionDlg::m_bCheckVirtualDrawTest;	break;
	}

	if (nState != -1)
	{
		ChangeCheckboxBgColor(pDC, nState);
	}

	return hbr;
}

void CTestOptionDlg_Tab2::OnBnClickedButtonNoiseUserAfe()
{
	T_AFE_SETTING * pAfeSetting = &CTestOptionDlg::m_TestInfo_NOISE.AfeInfo;

	CAfeNoiseSettingDlg dlg;

	dlg.m_nFrequency = pAfeSetting->nFrequency;
	dlg.m_nNCount = pAfeSetting->nNCount;
	dlg.m_nUCount = pAfeSetting->nUCount;

	dlg.bCheckFrequency1 = pAfeSetting->m_bCheckFrequency1;
	dlg.bCheckFrequency2 = pAfeSetting->m_bCheckFrequency2;
	dlg.bCheckFrequency3 = pAfeSetting->m_bCheckFrequency3;
	dlg.bCheckFrequency4 = pAfeSetting->m_bCheckFrequency4;

	dlg.m_nFrequency1 = pAfeSetting->nFrequency1;
	dlg.m_nFrequency2 = pAfeSetting->nFrequency2;
	dlg.m_nFrequency3 = pAfeSetting->nFrequency3;
	dlg.m_nFrequency4 = pAfeSetting->nFrequency4;

	if (dlg.DoModal() != IDOK)
		return;

	pAfeSetting->nFrequency = dlg.m_nFrequency;
	pAfeSetting->nNCount = dlg.m_nNCount;
	pAfeSetting->nUCount = dlg.m_nUCount;

	pAfeSetting->m_bCheckFrequency1 = dlg.bCheckFrequency1;
	pAfeSetting->m_bCheckFrequency2 = dlg.bCheckFrequency2;
	pAfeSetting->m_bCheckFrequency3 = dlg.bCheckFrequency3;
	pAfeSetting->m_bCheckFrequency4 = dlg.bCheckFrequency4;

	pAfeSetting->nFrequency1 = dlg.m_nFrequency1;
	pAfeSetting->nFrequency2 = dlg.m_nFrequency2;
	pAfeSetting->nFrequency3 = dlg.m_nFrequency3;
	pAfeSetting->nFrequency4 = dlg.m_nFrequency4;
}

