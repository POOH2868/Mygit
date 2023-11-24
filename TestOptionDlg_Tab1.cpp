// DlgTab4.cpp : implementation file
//

#include "stdafx.h"
#include "TouchDebugger.h"

#include "TestOptionDlg.h"
#include "TestOptionDlg_Tab1.h"

#include "TestModeDiffSpecDlg.h"
#include "AfeSettingDlg.h"
#include "AfeMultiSettingDlg.h"
#include "CurrentIdleOptionDlg.h"

enum
{
	CTRL_UPDATE_DND					= (1 << 0),
	CTRL_UPDATE_HF_DND 				= (1 << 1),
	CTRL_UPDATE_SELF_DND			= (1 << 2),
	CTRL_UPDATE_SELF_SATURATION		= (1 << 3),
	CTRL_UPDATE_CURRENT_CONSUMPTION	= (1 << 4),
	CTRL_UPDATE_ONLYSELF_DND		= (1 << 5),		//add 2022.09.16
	CTRL_UPDATE_HIGH_VOLTAGE		= (1 << 6),		//add 2022.09.30
	CTRL_UPDATE_CND					= (1 << 7),		// CND	//add 2022.11.14 for ZTM730 CND
	CTRL_UPDATE_I2C					= (1 << 8),		// I2C add 2023.05.15
};

// CTestOptionDlg_Tab1 dialog

IMPLEMENT_DYNAMIC(CTestOptionDlg_Tab1, CDialog)

CTestOptionDlg_Tab1::CTestOptionDlg_Tab1(CWnd* pParent /*=NULL*/)
	: CDialog(CTestOptionDlg_Tab1::IDD, pParent)
{

}

CTestOptionDlg_Tab1::~CTestOptionDlg_Tab1()
{

}

void CTestOptionDlg_Tab1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	// DND
	DDX_Check(pDX, IDC_CHECK_DND, CTestOptionDlg::m_TestInfo_DND.bCheckData);
	DDX_Check(pDX, IDC_CHECK_DND_DIFF, CTestOptionDlg::m_TestInfo_DND.bCheckDataDiff);
	DDX_Check(pDX, IDC_CHECK_DND_USE_USER_AFE, CTestOptionDlg::m_TestInfo_DND.AfeInfo.bUseAfeSetting);

	// CND	//add 2022.11.14 for ZTM730 CND
	DDX_Check(pDX, IDC_CHECK_CND, CTestOptionDlg::m_TestInfo_CND.bCheckData);

	//HIGH VOLTAGE
	DDX_Check(pDX, IDC_CHECK_HIGH_VOLTAGE, CTestOptionDlg::m_TestInfo_HIGH_VOLTAGE.bCheckData);
	DDX_Text(pDX, IDC_EDIT_HIGHVOLTAGE_MAX, CTestOptionDlg::m_TestInfo_HIGH_VOLTAGE.pDataMax);
	DDX_Text(pDX, IDC_EDIT_HIGHVOLTAGE_MIN, CTestOptionDlg::m_TestInfo_HIGH_VOLTAGE.pDataMin);
	/*
	DDX_Text(pDX, IDC_EDIT_HIGHVOLTAGE_MAX, CTestOptionDlg::m_TestInfo_HIGH_VOLTAGE.Active.nDataMax);
	DDX_Text(pDX, IDC_EDIT_HIGHVOLTAGE_MIN, CTestOptionDlg::m_TestInfo_HIGH_VOLTAGE.Active.nDataMin);
	*/
	// HF DND
	DDX_Check(pDX, IDC_CHECK_HF_DND, CTestOptionDlg::m_TestInfo_HF_DND.bCheckData);
	DDX_Check(pDX, IDC_CHECK_HF_DND_DIFF, CTestOptionDlg::m_TestInfo_HF_DND.bCheckDataDiff);
	DDX_Check(pDX, IDC_CHECK_HF_DND_USE_USER_AFE, CTestOptionDlg::m_TestInfo_HF_DND.AfeInfo.bUseAfeSetting);

	// SELF_DND
	DDX_Check(pDX, IDC_CHECK_SELF_DND, CTestOptionDlg::m_TestInfo_SELF_DND.bCheckData);
	DDX_Check(pDX, IDC_CHECK_SELF_DND_DIFF, CTestOptionDlg::m_TestInfo_SELF_DND.bCheckDataDiff);
	DDX_Check(pDX, IDC_CHECK_SELF_DND_USE_USER_AFE, CTestOptionDlg::m_TestInfo_SELF_DND.AfeInfo.bUseAfeSetting);

	// ONLY_SELF_DND
	DDX_Check(pDX, IDC_CHECK_ONLYSELF_DND, CTestOptionDlg::m_TestInfo_ONLYSELF_DND.bCheckData);


	// SELF_SATURATION
	DDX_Check(pDX, IDC_CHECK_SELF_SATURATION, CTestOptionDlg::m_TestInfo_SELF_SATURATION.bCheckData);
	DDX_Check(pDX, IDC_CHECK_SELF_SATURATION_USE_USER_AFE, CTestOptionDlg::m_TestInfo_SELF_SATURATION.AfeInfo.bUseAfeSetting);

	// TRX SHORT
	DDX_Check(pDX, IDC_CHECK_TRX_SHORT, CTestOptionDlg::m_bCheckTrxShort);

	// CURRENT CONSUMPTION
	DDX_Check(pDX, IDC_CHECK_CURRENT_CONSUMPTION, CTestOptionDlg::m_TestInfo_CURRENT_CONSUMPTION.Active.bCheckData);
	DDX_Text(pDX, IDC_EDIT_CURRENT_CONSUMPTION_MAX, CTestOptionDlg::m_TestInfo_CURRENT_CONSUMPTION.Active.nThreshould_Max);
	DDX_Text(pDX, IDC_EDIT_CURRENT_CONSUMPTION_MIN, CTestOptionDlg::m_TestInfo_CURRENT_CONSUMPTION.Active.nThreshould_Min);
	DDX_Text(pDX, IDC_EDIT_CURRENT_CONSUMPTION_AVG_MS, CTestOptionDlg::m_TestInfo_CURRENT_CONSUMPTION.Active.nAvgMs);

	DDX_Control(pDX, IDC_BUTTON_DND_MODIFY_MAX_SPEC, m_btnDndMax);
	DDX_Control(pDX, IDC_BUTTON_DND_MODIFY_MIN_SPEC, m_btnDndMin);
	DDX_Control(pDX, IDC_BUTTON_DND_MODIFY_VDIFF, m_btnDndVDiff);
	DDX_Control(pDX, IDC_BUTTON_DND_MODIFY_HDIFF, m_btnDndHDiff);
	DDX_Control(pDX, IDC_BUTTON_DND_CHANGE_USER_AFE, m_btnDndAfe);

	DDX_Control(pDX, IDC_BUTTON_CND_MODIFY_MAX_SPEC, m_btnCndMax);
	DDX_Control(pDX, IDC_BUTTON_CND_MODIFY_MIN_SPEC, m_btnCndMin);

	DDX_Control(pDX, IDC_BUTTON_HF_DND_MODIFY_MAX_SPEC, m_btnHfDndMax);
	DDX_Control(pDX, IDC_BUTTON_HF_DND_MODIFY_MIN_SPEC, m_btnHfDndMin);
	DDX_Control(pDX, IDC_BUTTON_HF_DND_MODIFY_VDIFF, m_btnHfDndVDiff);
	DDX_Control(pDX, IDC_BUTTON_HF_DND_MODIFY_HDIFF, m_btnHfDndHDiff);
	DDX_Control(pDX, IDC_BUTTON_HF_DND_CHANGE_USER_AFE, m_btnHfDndAfe);

	DDX_Control(pDX, IDC_BUTTON_SELF_DND_TX_MODIFY_MAX_SPEC, m_btnSelfDndTxMax);
	DDX_Control(pDX, IDC_BUTTON_SELF_DND_TX_MODIFY_MIN_SPEC, m_btnSelfDndTxMin);
	DDX_Control(pDX, IDC_BUTTON_SELF_DND_TX_MODIFY_HDIFF_SPEC, m_btnSelfDndTxHDiff);
	DDX_Control(pDX, IDC_BUTTON_SELF_DND_RX_MODIFY_MAX_SPEC, m_btnSelfDndRxMax);
	DDX_Control(pDX, IDC_BUTTON_SELF_DND_RX_MODIFY_MIN_SPEC, m_btnSelfDndRxMin);
	DDX_Control(pDX, IDC_BUTTON_SELF_DND_RX_MODIFY_HDIFF_SPEC, m_btnSelfDndRxHDiff);
	DDX_Control(pDX, IDC_BUTTON_SELF_DND_CHANGE_USER_AFE, m_btnSelfDndAfe);

	DDX_Control(pDX, IDC_BUTTON_SELF_SATURATION_TX_MODIFY_MAX_SPEC, m_btnSelfSaturationTxMax);
	DDX_Control(pDX, IDC_BUTTON_SELF_SATURATION_TX_MODIFY_MIN_SPEC, m_btnSelfSaturationTxMin);
	DDX_Control(pDX, IDC_BUTTON_SELF_SATURATION_RX_MODIFY_MAX_SPEC, m_btnSelfSaturationRxMax);
	DDX_Control(pDX, IDC_BUTTON_SELF_SATURATION_RX_MODIFY_MIN_SPEC, m_btnSelfSaturationRxMin);
	DDX_Control(pDX, IDC_BUTTON_SELF_SATURATION_CHANGE_USER_AFE, m_btnSelfSaturationAfe);

	DDX_Control(pDX, IDC_BUTTON_CURRENT_IDLE_OPTION, m_btnCurrentIdleOption);

	DDX_Control(pDX, IDC_BUTTON_ONLYSELF_DND_TX_MODIFY_MAX_SPEC, m_btnOnlySelfDndTxMax);
	DDX_Control(pDX, IDC_BUTTON_ONLYSELF_DND_TX_MODIFY_MIN_SPEC, m_btnOnlySelfDndTxMin);
	DDX_Control(pDX, IDC_BUTTON_ONLYSELF_DND_RX_MODIFY_MAX_SPEC, m_btnOnlySelfDndRxMax);
	DDX_Control(pDX, IDC_BUTTON_ONLYSELF_DND_RX_MODIFY_MIN_SPEC, m_btnOnlySelfDndRxMin);
}


BEGIN_MESSAGE_MAP(CTestOptionDlg_Tab1, CDialog)
	ON_WM_CTLCOLOR()

	// DND
	ON_BN_CLICKED(IDC_CHECK_DND, &CTestOptionDlg_Tab1::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_DND_DIFF, &CTestOptionDlg_Tab1::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_DND_USE_USER_AFE, &CTestOptionDlg_Tab1::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_BUTTON_DND_MODIFY_MIN_SPEC, &CTestOptionDlg_Tab1::OnBnClicked_Dnd_ModifyMinSpec)
	ON_BN_CLICKED(IDC_BUTTON_DND_MODIFY_MAX_SPEC, &CTestOptionDlg_Tab1::OnBnClicked_Dnd_ModifyMaxSpec)
	ON_BN_CLICKED(IDC_BUTTON_DND_MODIFY_VDIFF, &CTestOptionDlg_Tab1::OnBnClicked_Dnd_ModifyVDiff)
	ON_BN_CLICKED(IDC_BUTTON_DND_MODIFY_HDIFF, &CTestOptionDlg_Tab1::OnBnClicked_Dnd_ModifyHDiff)

	// CND	//add 2022.11.14 for ZTM730 CND
	ON_BN_CLICKED(IDC_CHECK_CND, &CTestOptionDlg_Tab1::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_BUTTON_CND_MODIFY_MIN_SPEC, &CTestOptionDlg_Tab1::OnBnClicked_Cnd_ModifyMinSpec)
	ON_BN_CLICKED(IDC_BUTTON_CND_MODIFY_MAX_SPEC, &CTestOptionDlg_Tab1::OnBnClicked_Cnd_ModifyMaxSpec)

	// HIGH VOLTAGE
	ON_BN_CLICKED(IDC_CHECK_HIGH_VOLTAGE, &CTestOptionDlg_Tab1::OnCheckBnClicked) //수정 필요 SSY 220930

	// HF DND
	ON_BN_CLICKED(IDC_CHECK_HF_DND, &CTestOptionDlg_Tab1::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_HF_DND_DIFF, &CTestOptionDlg_Tab1::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_HF_DND_USE_USER_AFE, &CTestOptionDlg_Tab1::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_BUTTON_HF_DND_MODIFY_MIN_SPEC, &CTestOptionDlg_Tab1::OnBnClicked_HfDnd_ModifyMinSpec)
	ON_BN_CLICKED(IDC_BUTTON_HF_DND_MODIFY_MAX_SPEC, &CTestOptionDlg_Tab1::OnBnClicked_HfDnd_ModifyMaxSpec)
	ON_BN_CLICKED(IDC_BUTTON_HF_DND_MODIFY_VDIFF, &CTestOptionDlg_Tab1::OnBnClicked_HfDnd_ModifyVDiff)
	ON_BN_CLICKED(IDC_BUTTON_HF_DND_MODIFY_HDIFF, &CTestOptionDlg_Tab1::OnBnClicked_HfDnd_ModifyHDiff)

	// SELF_DND
	ON_BN_CLICKED(IDC_CHECK_SELF_DND, &CTestOptionDlg_Tab1::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_SELF_DND_DIFF, &CTestOptionDlg_Tab1::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_SELF_DND_USE_USER_AFE, &CTestOptionDlg_Tab1::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_ONLYSELF_DND, &CTestOptionDlg_Tab1::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_BUTTON_SELF_DND_TX_MODIFY_MIN_SPEC, &CTestOptionDlg_Tab1::OnBnClicked_SelfDnd_Tx_ModifyMinSpec)
	ON_BN_CLICKED(IDC_BUTTON_SELF_DND_TX_MODIFY_MAX_SPEC, &CTestOptionDlg_Tab1::OnBnClicked_SelfDnd_Tx_ModifyMaxSpec)
	ON_BN_CLICKED(IDC_BUTTON_SELF_DND_TX_MODIFY_HDIFF_SPEC, &CTestOptionDlg_Tab1::OnBnClicked_SelfDnd_Tx_ModifyHDiff)
	ON_BN_CLICKED(IDC_BUTTON_SELF_DND_RX_MODIFY_MIN_SPEC, &CTestOptionDlg_Tab1::OnBnClicked_SelfDnd_Rx_ModifyMinSpec)
	ON_BN_CLICKED(IDC_BUTTON_SELF_DND_RX_MODIFY_MAX_SPEC, &CTestOptionDlg_Tab1::OnBnClicked_SelfDnd_Rx_ModifyMaxSpec)
	ON_BN_CLICKED(IDC_BUTTON_SELF_DND_RX_MODIFY_HDIFF_SPEC, &CTestOptionDlg_Tab1::OnBnClicked_SelfDnd_Rx_ModifyHDiff)

	// SELF_SATURATION
	ON_BN_CLICKED(IDC_CHECK_SELF_SATURATION, &CTestOptionDlg_Tab1::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_CHECK_SELF_SATURATION_USE_USER_AFE, &CTestOptionDlg_Tab1::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_BUTTON_SELF_SATURATION_TX_MODIFY_MIN_SPEC, &CTestOptionDlg_Tab1::OnBnClicked_SelfSaturation_Tx_ModifyMinSpec)
	ON_BN_CLICKED(IDC_BUTTON_SELF_SATURATION_TX_MODIFY_MAX_SPEC, &CTestOptionDlg_Tab1::OnBnClicked_SelfSaturation_Tx_ModifyMaxSpec)
	ON_BN_CLICKED(IDC_BUTTON_SELF_SATURATION_RX_MODIFY_MIN_SPEC, &CTestOptionDlg_Tab1::OnBnClicked_SelfSaturation_Rx_ModifyMinSpec)
	ON_BN_CLICKED(IDC_BUTTON_SELF_SATURATION_RX_MODIFY_MAX_SPEC, &CTestOptionDlg_Tab1::OnBnClicked_SelfSaturation_Rx_ModifyMaxSpec)

	// TRX SHORT
	ON_BN_CLICKED(IDC_CHECK_TRX_SHORT, &CTestOptionDlg_Tab1::OnCheckBnClicked)

	// CURRENT CONSUMPTION
	ON_BN_CLICKED(IDC_CHECK_CURRENT_CONSUMPTION, &CTestOptionDlg_Tab1::OnCheckBnClicked)
	ON_BN_CLICKED(IDC_BUTTON_CURRENT_IDLE_OPTION, &CTestOptionDlg_Tab1::OnBnClicked_CurrentIdleOption)

	// Change AFE SEtting
	ON_BN_CLICKED(IDC_BUTTON_DND_CHANGE_USER_AFE, &CTestOptionDlg_Tab1::OnBnClicked_ChangeUserAFE)
	ON_BN_CLICKED(IDC_BUTTON_HF_DND_CHANGE_USER_AFE, &CTestOptionDlg_Tab1::OnBnClicked_ChangeUserAFE)
	ON_BN_CLICKED(IDC_BUTTON_SELF_DND_CHANGE_USER_AFE, &CTestOptionDlg_Tab1::OnBnClicked_ChangeUserAFE_Multi)
	ON_BN_CLICKED(IDC_BUTTON_SELF_SATURATION_CHANGE_USER_AFE, &CTestOptionDlg_Tab1::OnBnClicked_ChangeUserAFE_Multi)
	ON_BN_CLICKED(IDC_BUTTON_ONLYSELF_DND_TX_MODIFY_MAX_SPEC, &CTestOptionDlg_Tab1::OnBnClickedButtonOnlyselfDndTxModifyMaxSpec)
	ON_BN_CLICKED(IDC_BUTTON_ONLYSELF_DND_TX_MODIFY_MIN_SPEC, &CTestOptionDlg_Tab1::OnBnClickedButtonOnlyselfDndTxModifyMinSpec)
	ON_BN_CLICKED(IDC_BUTTON_ONLYSELF_DND_RX_MODIFY_MAX_SPEC, &CTestOptionDlg_Tab1::OnBnClickedButtonOnlyselfDndRxModifyMaxSpec)
	ON_BN_CLICKED(IDC_BUTTON_ONLYSELF_DND_RX_MODIFY_MIN_SPEC, &CTestOptionDlg_Tab1::OnBnClickedButtonOnlyselfDndRxModifyMinSpec)
END_MESSAGE_MAP()


// CTestOptionDlg_Tab1 message handlers

BOOL CTestOptionDlg_Tab1::PreTranslateMessage(MSG* pMsg)
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

BOOL CTestOptionDlg_Tab1::OnInitDialog()
{
	CDialog::OnInitDialog();

	EnableDialogControls(0xFFFFFFFF);

	InitUiContorl();

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CTestOptionDlg_Tab1::OnCheckBnClicked()
{
	UpdateData(TRUE);

	int nID = GetFocus()->GetDlgCtrlID();
	int nUpdateID = 0xFFFF;

	if( nID == IDC_CHECK_DND || nID == IDC_CHECK_DND_DIFF || nID == IDC_CHECK_DND_USE_USER_AFE )
	{
		nUpdateID = CTRL_UPDATE_DND;

		if (CTestOptionDlg::m_TestInfo_DND.bCheckData == FALSE)
		{
			CTestOptionDlg::m_TestInfo_DND.bCheckDataDiff = FALSE;
			CTestOptionDlg::m_TestInfo_DND.AfeInfo.bUseAfeSetting = FALSE;
			UpdateData(FALSE);
		}
	}
	else if (nID == IDC_CHECK_CND)				// CND	//add 2022.11.14 for ZTM730 CND
	{
		nUpdateID = CTRL_UPDATE_CND;
		if (CTestOptionDlg::m_TestInfo_CND.bCheckData == FALSE)
		{
			UpdateData(FALSE);
		}
	}
	else if( nID == IDC_CHECK_HIGH_VOLTAGE)
	{
		nUpdateID = CTRL_UPDATE_HIGH_VOLTAGE;

		if (CTestOptionDlg::m_TestInfo_HIGH_VOLTAGE.bCheckData == FALSE)
		{
			//CTestOptionDlg::m_TestInfo_HF_DND.bCheckDataDiff = FALSE;
			//CTestOptionDlg::m_TestInfo_HF_DND.AfeInfo.bUseAfeSetting = FALSE;
			UpdateData(FALSE);
		}
	}
	else if( nID == IDC_CHECK_HF_DND || nID == IDC_CHECK_HF_DND_DIFF || nID == IDC_CHECK_HF_DND_USE_USER_AFE )
	{
		nUpdateID = CTRL_UPDATE_HF_DND;

		if (CTestOptionDlg::m_TestInfo_HF_DND.bCheckData == FALSE)
		{
			CTestOptionDlg::m_TestInfo_HF_DND.bCheckDataDiff = FALSE;
			CTestOptionDlg::m_TestInfo_HF_DND.AfeInfo.bUseAfeSetting = FALSE;
			UpdateData(FALSE);
		}
	}
	else if (nID == IDC_CHECK_ONLYSELF_DND)
	{
		nUpdateID = CTRL_UPDATE_ONLYSELF_DND;

		if (CTestOptionDlg::m_TestInfo_ONLYSELF_DND.bCheckData == TRUE)
		{
			CTestOptionDlg::m_TestInfo_SELF_DND.bCheckData = FALSE;
			CTestOptionDlg::m_TestInfo_SELF_DND.bCheckDataDiff = FALSE;
			CTestOptionDlg::m_TestInfo_SELF_DND.AfeInfo.bUseAfeSetting = FALSE;

			CTestOptionDlg::m_TestInfo_SELF_SATURATION.AfeInfo.bUseAfeSetting = FALSE;
			UpdateData(FALSE);
		}
	}
	else if( nID == IDC_CHECK_SELF_DND || nID == IDC_CHECK_SELF_DND_DIFF || nID == IDC_CHECK_SELF_DND_USE_USER_AFE)
	{
		nUpdateID = CTRL_UPDATE_SELF_DND;

		if (CTestOptionDlg::m_TestInfo_SELF_DND.bCheckData == FALSE)
		{
			CTestOptionDlg::m_TestInfo_SELF_DND.bCheckDataDiff = FALSE;
			CTestOptionDlg::m_TestInfo_SELF_DND.AfeInfo.bUseAfeSetting = FALSE;
			UpdateData(FALSE);
		}
		else
		{
			CTestOptionDlg::m_TestInfo_ONLYSELF_DND.bCheckData = FALSE;
			UpdateData(FALSE);
		}
	}
	else if( nID == IDC_CHECK_SELF_SATURATION || nID == IDC_CHECK_SELF_SATURATION_USE_USER_AFE )
	{
		nUpdateID = CTRL_UPDATE_SELF_SATURATION;

		if( nID == IDC_CHECK_SELF_SATURATION && CTestOptionDlg::m_TestInfo_SELF_SATURATION.bCheckData == FALSE)
		{
			CTestOptionDlg::m_TestInfo_SELF_SATURATION.AfeInfo.bUseAfeSetting = FALSE;
			UpdateData(FALSE);
		}
		else
		{
			CTestOptionDlg::m_TestInfo_ONLYSELF_DND.bCheckData = FALSE;
			UpdateData(FALSE);
		}
	}
	else if( nID == IDC_CHECK_CURRENT_CONSUMPTION )
	{
		nUpdateID = CTRL_UPDATE_CURRENT_CONSUMPTION;
	}

	EnableDialogControls(nUpdateID);

	GetDlgItem(nID)->Invalidate();
}

// DND
void CTestOptionDlg_Tab1::OnBnClicked_Dnd_ModifyMaxSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_DND.pDataMax[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_DND.nDataMax;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY;

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_DND.pDataMax[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_DND.nDataMax = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab1::OnBnClicked_Dnd_ModifyMinSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_DND.pDataMin[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_DND.nDataMin;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY;

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_DND.pDataMin[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_DND.nDataMin = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab1::OnBnClicked_Cnd_ModifyMinSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_CND.pDataMin[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_CND.nDataMin;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY;

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_CND.pDataMin[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_CND.nDataMin = dlg.m_nApplyValue;
}
void CTestOptionDlg_Tab1::OnBnClicked_Cnd_ModifyMaxSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_CND.pDataMax[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_CND.nDataMax;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY;

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_CND.pDataMax[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_CND.nDataMax = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab1::OnBnClicked_Dnd_ModifyHDiff()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] =CTestOptionDlg:: m_TestInfo_DND.pHDiff[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_DND.nHDiffCenter;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY - 1;

	dlg.bHDiff = TRUE;

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_DND.pHDiff[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_DND.nHDiffCenter = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab1::OnBnClicked_Dnd_ModifyVDiff()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_DND.pVDiff[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_DND.nVDiffCenter;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX - 1;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY;

	dlg.bVDiff = TRUE;

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_DND.pVDiff[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_DND.nVDiffCenter = dlg.m_nApplyValue;
}

// HF DND
void CTestOptionDlg_Tab1::OnBnClicked_HfDnd_ModifyMaxSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_HF_DND.pDataMax[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_HF_DND.nDataMax;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY;

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_HF_DND.pDataMax[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_HF_DND.nDataMax = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab1::OnBnClicked_HfDnd_ModifyMinSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_HF_DND.pDataMin[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_HF_DND.nDataMin;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY;

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_HF_DND.pDataMin[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_HF_DND.nDataMin = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab1::OnBnClicked_HfDnd_ModifyHDiff()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_HF_DND.pHDiff[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_HF_DND.nHDiffCenter;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY - 1;

	dlg.bHDiff = TRUE;

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_HF_DND.pHDiff[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_HF_DND.nHDiffCenter = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab1::OnBnClicked_HfDnd_ModifyVDiff()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_HF_DND.pVDiff[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_HF_DND.nVDiffCenter;

	dlg.m_totalX = CTestOptionDlg::m_icRegData_TotalNumOfX - 1;
	dlg.m_totalY = CTestOptionDlg::m_icRegData_TotalNumOfY;

	dlg.bVDiff = TRUE;

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_HF_DND.pVDiff[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_HF_DND.nVDiffCenter = dlg.m_nApplyValue;
}

// SELF_DND_TX
void CTestOptionDlg_Tab1::OnBnClicked_SelfDnd_Tx_ModifyMaxSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_SELF_DND.pTxDataMax[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_SELF_DND.nTxDataMax;

	if (CTestOptionDlg::m_bUseIFE)
	{ // IFE
		dlg.m_totalX = CTestOptionDlg::m_icRegData_SelfNumOfX;
		dlg.m_totalY = CTestOptionDlg::m_icRegData_SelfNumOfY;
	}
	else
	{ // CHINA
		dlg.m_totalX = CTestOptionDlg::m_IC_Max_X_Line_Num;
		dlg.m_totalY = CTestOptionDlg::m_IC_Max_Y_Line_Num;
	}

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_SELF_DND.pTxDataMax[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_SELF_DND.nTxDataMax = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab1::OnBnClicked_SelfDnd_Tx_ModifyMinSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_SELF_DND.pTxDataMin[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_SELF_DND.nTxDataMin;

	if (CTestOptionDlg::m_bUseIFE)
	{ // IFE
		dlg.m_totalX = CTestOptionDlg::m_icRegData_SelfNumOfX;
		dlg.m_totalY = CTestOptionDlg::m_icRegData_SelfNumOfY;
	}
	else
	{ // CHINA
		dlg.m_totalX = CTestOptionDlg::m_IC_Max_X_Line_Num;
		dlg.m_totalY = CTestOptionDlg::m_IC_Max_Y_Line_Num;
	}

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_SELF_DND.pTxDataMin[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_SELF_DND.nTxDataMin = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab1::OnBnClicked_SelfDnd_Tx_ModifyHDiff()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_SELF_DND.pTxHDiff[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_SELF_DND.nTxHDiffCenter;

	if (CTestOptionDlg::m_bUseIFE)
	{ // IFE
		dlg.m_totalX = CTestOptionDlg::m_icRegData_SelfNumOfX;
		dlg.m_totalY = CTestOptionDlg::m_icRegData_SelfNumOfY - 1;
	}
	else
	{ // CHINA
		dlg.m_totalX = CTestOptionDlg::m_IC_Max_X_Line_Num;
		dlg.m_totalY = CTestOptionDlg::m_IC_Max_Y_Line_Num - 1;
	}

	dlg.bHDiff = TRUE;

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_SELF_DND.pTxHDiff[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_SELF_DND.nTxHDiffCenter = dlg.m_nApplyValue;
}

// SELF_DND_RX
void CTestOptionDlg_Tab1::OnBnClicked_SelfDnd_Rx_ModifyMaxSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_SELF_DND.pRxDataMax[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_SELF_DND.nRxDataMax;

	if (CTestOptionDlg::m_bUseIFE)
	{ // IFE
		dlg.m_totalX = CTestOptionDlg::m_icRegData_SelfNumOfX;
		dlg.m_totalY = CTestOptionDlg::m_icRegData_SelfNumOfY;
	}
	else
	{ // CHINA
		dlg.m_totalX = CTestOptionDlg::m_IC_Max_X_Line_Num;
		dlg.m_totalY = CTestOptionDlg::m_IC_Max_Y_Line_Num;
	}

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_SELF_DND.pRxDataMax[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_SELF_DND.nRxDataMax = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab1::OnBnClicked_SelfDnd_Rx_ModifyMinSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_SELF_DND.pRxDataMin[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_SELF_DND.nRxDataMin;

	if (CTestOptionDlg::m_bUseIFE)
	{ // IFE
		dlg.m_totalX = CTestOptionDlg::m_icRegData_SelfNumOfX;
		dlg.m_totalY = CTestOptionDlg::m_icRegData_SelfNumOfY;
	}
	else
	{ // CHINA
		dlg.m_totalX = CTestOptionDlg::m_IC_Max_X_Line_Num;
		dlg.m_totalY = CTestOptionDlg::m_IC_Max_Y_Line_Num;
	}

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_SELF_DND.pRxDataMin[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_SELF_DND.nRxDataMin = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab1::OnBnClicked_SelfDnd_Rx_ModifyHDiff()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_SELF_DND.pRxHDiff[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_SELF_DND.nRxHDiffCenter;

	if (CTestOptionDlg::m_bUseIFE)
	{ // IFE
		dlg.m_totalX = CTestOptionDlg::m_icRegData_SelfNumOfX;
		dlg.m_totalY = CTestOptionDlg::m_icRegData_SelfNumOfY - 1;
	}
	else
	{ // CHINA
		dlg.m_totalX = CTestOptionDlg::m_IC_Max_X_Line_Num;
		dlg.m_totalY = CTestOptionDlg::m_IC_Max_Y_Line_Num - 1;
	}

	dlg.bHDiff = TRUE;

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_SELF_DND.pRxHDiff[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_SELF_DND.nRxHDiffCenter = dlg.m_nApplyValue;
}


// SELF_SATURATION_TX
void CTestOptionDlg_Tab1::OnBnClicked_SelfSaturation_Tx_ModifyMaxSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_SELF_SATURATION.pTxDataMax[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_SELF_SATURATION.nTxDataMax;

	if (CTestOptionDlg::m_bUseIFE)
	{ // IFE
		dlg.m_totalX = CTestOptionDlg::m_icRegData_SelfNumOfX;
		dlg.m_totalY = CTestOptionDlg::m_icRegData_SelfNumOfY;
	}
	else
	{ // CHINA
		dlg.m_totalX = CTestOptionDlg::m_IC_Max_X_Line_Num;
		dlg.m_totalY = CTestOptionDlg::m_IC_Max_Y_Line_Num;
	}

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_SELF_SATURATION.pTxDataMax[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_SELF_SATURATION.nTxDataMax = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab1::OnBnClicked_SelfSaturation_Tx_ModifyMinSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_SELF_SATURATION.pTxDataMin[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_SELF_SATURATION.nTxDataMin;

	if (CTestOptionDlg::m_bUseIFE)
	{ // IFE
		dlg.m_totalX = CTestOptionDlg::m_icRegData_SelfNumOfX;
		dlg.m_totalY = CTestOptionDlg::m_icRegData_SelfNumOfY;
	}
	else
	{ // CHINA
		dlg.m_totalX = CTestOptionDlg::m_IC_Max_X_Line_Num;
		dlg.m_totalY = CTestOptionDlg::m_IC_Max_Y_Line_Num;
	}

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_SELF_SATURATION.pTxDataMin[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_SELF_SATURATION.nTxDataMin = dlg.m_nApplyValue;
}

// SELF_SATURATION_RX
void CTestOptionDlg_Tab1::OnBnClicked_SelfSaturation_Rx_ModifyMaxSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_SELF_SATURATION.pRxDataMax[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_SELF_SATURATION.nRxDataMax;

	if (CTestOptionDlg::m_bUseIFE)
	{ // IFE
		dlg.m_totalX = CTestOptionDlg::m_icRegData_SelfNumOfX;
		dlg.m_totalY = CTestOptionDlg::m_icRegData_SelfNumOfY;
	}
	else
	{ // CHINA
		dlg.m_totalX = CTestOptionDlg::m_IC_Max_X_Line_Num;
		dlg.m_totalY = CTestOptionDlg::m_IC_Max_Y_Line_Num;
	}

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_SELF_SATURATION.pRxDataMax[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_SELF_SATURATION.nRxDataMax = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab1::OnBnClicked_SelfSaturation_Rx_ModifyMinSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_SELF_SATURATION.pRxDataMin[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_SELF_SATURATION.nRxDataMin;

	if (CTestOptionDlg::m_bUseIFE)
	{ // IFE
		dlg.m_totalX = CTestOptionDlg::m_icRegData_SelfNumOfX;
		dlg.m_totalY = CTestOptionDlg::m_icRegData_SelfNumOfY;
	}
	else
	{ // CHINA
		dlg.m_totalX = CTestOptionDlg::m_IC_Max_X_Line_Num;
		dlg.m_totalY = CTestOptionDlg::m_IC_Max_Y_Line_Num;
	}

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_SELF_SATURATION.pRxDataMin[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_SELF_SATURATION.nRxDataMin = dlg.m_nApplyValue;
}

void CTestOptionDlg_Tab1::OnBnClicked_ChangeUserAFE()
{
	int nID = GetFocus()->GetDlgCtrlID();
	T_AFE_SETTING * pAfeInfo = NULL;

	if (nID == IDC_BUTTON_DND_CHANGE_USER_AFE )
	{
		pAfeInfo = &CTestOptionDlg::m_TestInfo_DND.AfeInfo;
	}
	else if (nID == IDC_BUTTON_HF_DND_CHANGE_USER_AFE )
	{
		pAfeInfo = &CTestOptionDlg::m_TestInfo_HF_DND.AfeInfo;
	}

	if (pAfeInfo == NULL)
		return;

	CAfeSettingDlg dlg;

	dlg.m_nFrequency = pAfeInfo->nFrequency;
	dlg.m_nNCount = pAfeInfo->nNCount;
	dlg.m_nUCount = pAfeInfo->nUCount;

	if (dlg.DoModal() != IDOK)
		return;

	pAfeInfo->nFrequency = dlg.m_nFrequency;
	pAfeInfo->nNCount = dlg.m_nNCount;
	pAfeInfo->nUCount = dlg.m_nUCount;
}

void CTestOptionDlg_Tab1::OnBnClicked_ChangeUserAFE_Multi()
{
	T_AFE_SETTING_MULTI * pAfeInfo = NULL;

	int nID = GetFocus()->GetDlgCtrlID();

	if (nID == IDC_BUTTON_SELF_DND_CHANGE_USER_AFE )
	{
		pAfeInfo = &CTestOptionDlg::m_TestInfo_SELF_DND.AfeInfo;
	}
	else if (nID == IDC_BUTTON_SELF_SATURATION_CHANGE_USER_AFE )
	{
		pAfeInfo = &CTestOptionDlg::m_TestInfo_SELF_SATURATION.AfeInfo;
	}

	if (pAfeInfo == NULL)
		return;

	if (CTestOptionDlg::m_bUseIFE == FALSE)
	{ // CHINA.
		CAfeSettingDlg dlg;

		dlg.m_nFrequency = pAfeInfo->nFrequency_A;
		dlg.m_nNCount = pAfeInfo->nNCount_A;
		dlg.m_nUCount = pAfeInfo->nUCount_A;

		if (dlg.DoModal() != IDOK)
			return;

		pAfeInfo->nFrequency_A = dlg.m_nFrequency;
		pAfeInfo->nNCount_A = dlg.m_nNCount;
		pAfeInfo->nUCount_A = dlg.m_nUCount;
	}
	else
	{
		CAfeMultiSettingDlg dlg;

		dlg.m_nFrequency_A 	= pAfeInfo->nFrequency_A;
		dlg.m_nNCount_A 	= pAfeInfo->nNCount_A;
		dlg.m_nUCount_A 	= pAfeInfo->nUCount_A;

		dlg.m_nFrequency_B 	= pAfeInfo->nFrequency_B;
		dlg.m_nNCount_B 	= pAfeInfo->nNCount_B;
		dlg.m_nUCount_B 	= pAfeInfo->nUCount_B;

		if (dlg.DoModal() != IDOK)
			return;

		pAfeInfo->nFrequency_A 	= dlg.m_nFrequency_A;
		pAfeInfo->nNCount_A 	= dlg.m_nNCount_A;
		pAfeInfo->nUCount_A 	= dlg.m_nUCount_A;

		pAfeInfo->nFrequency_B 	= dlg.m_nFrequency_B;
		pAfeInfo->nNCount_B 	= dlg.m_nNCount_B;
		pAfeInfo->nUCount_B 	= dlg.m_nUCount_B;
	}
}

void CTestOptionDlg_Tab1::OnBnClicked_CurrentIdleOption()
{
	CCurrentIdleOptionDlg dlg;

	dlg.m_bUseIFE = CTestOptionDlg::m_bUseIFE;

	memcpy(&dlg.m_Idle, &CTestOptionDlg::m_TestInfo_CURRENT_CONSUMPTION.Idle, sizeof(TEST_CURRENT_CONSUMPTION_ITEM));
	memcpy(&dlg.m_Sleep, &CTestOptionDlg::m_TestInfo_CURRENT_CONSUMPTION.Sleep, sizeof(TEST_CURRENT_CONSUMPTION_ITEM));
	memcpy(&dlg.m_DeepSleep, &CTestOptionDlg::m_TestInfo_CURRENT_CONSUMPTION.DeepSleep, sizeof(TEST_CURRENT_CONSUMPTION_ITEM));

	if (dlg.DoModal() != IDOK)
		return;

	memcpy(&CTestOptionDlg::m_TestInfo_CURRENT_CONSUMPTION.Idle, &dlg.m_Idle, sizeof(TEST_CURRENT_CONSUMPTION_ITEM));
	memcpy(&CTestOptionDlg::m_TestInfo_CURRENT_CONSUMPTION.Sleep, &dlg.m_Sleep, sizeof(TEST_CURRENT_CONSUMPTION_ITEM));
	memcpy(&CTestOptionDlg::m_TestInfo_CURRENT_CONSUMPTION.DeepSleep, &dlg.m_DeepSleep, sizeof(TEST_CURRENT_CONSUMPTION_ITEM));
}

void CTestOptionDlg_Tab1::EnableDialogControls(int nUpdateID)
{
	UpdateData(TRUE);

	BOOL bEnableIFE = CTestOptionDlg::m_bUseIFE;

	if( nUpdateID & CTRL_UPDATE_DND )
	{
		BOOL bCheckDnd = CTestOptionDlg::m_TestInfo_DND.bCheckData;

		GetDlgItem(IDC_CHECK_DND_USE_USER_AFE)->EnableWindow(bCheckDnd);
		GetDlgItem(IDC_BUTTON_DND_CHANGE_USER_AFE)->EnableWindow(bCheckDnd && CTestOptionDlg::m_TestInfo_DND.AfeInfo.bUseAfeSetting);

		GetDlgItem(IDC_BUTTON_DND_MODIFY_MAX_SPEC)->EnableWindow(bCheckDnd);
		GetDlgItem(IDC_BUTTON_DND_MODIFY_MIN_SPEC)->EnableWindow(bCheckDnd);

		GetDlgItem(IDC_CHECK_DND_DIFF)->EnableWindow(bCheckDnd);

		BOOL bCheckDndDiff = bCheckDnd && CTestOptionDlg::m_TestInfo_DND.bCheckDataDiff;
		GetDlgItem(IDC_BUTTON_DND_MODIFY_HDIFF)->EnableWindow(bCheckDndDiff);
		GetDlgItem(IDC_BUTTON_DND_MODIFY_VDIFF)->EnableWindow(bCheckDndDiff);
	}

	if (nUpdateID & CTRL_UPDATE_CND)	// CND	//add 2022.11.14 for ZTM730 CND
	{
		BOOL bCheckCnd = CTestOptionDlg::m_TestInfo_CND.bCheckData;

		GetDlgItem(IDC_BUTTON_CND_MODIFY_MAX_SPEC)->EnableWindow(bCheckCnd);
		GetDlgItem(IDC_BUTTON_CND_MODIFY_MIN_SPEC)->EnableWindow(bCheckCnd);
	}

	if( nUpdateID & CTRL_UPDATE_HIGH_VOLTAGE )
	{
		GetDlgItem(IDC_CHECK_HIGH_VOLTAGE)->EnableWindow(bEnableIFE);

		BOOL bCheckHighVoltage = bEnableIFE && CTestOptionDlg::m_TestInfo_HIGH_VOLTAGE.bCheckData;

		GetDlgItem(IDC_EDIT_HIGHVOLTAGE_MAX)->EnableWindow(bCheckHighVoltage);
		GetDlgItem(IDC_EDIT_HIGHVOLTAGE_MIN)->EnableWindow(bCheckHighVoltage);
	}

	if( nUpdateID & CTRL_UPDATE_HF_DND )
	{
		BOOL bChecHfDnd = CTestOptionDlg::m_TestInfo_HF_DND.bCheckData;

		GetDlgItem(IDC_CHECK_HF_DND_USE_USER_AFE)->EnableWindow(bChecHfDnd);
		GetDlgItem(IDC_BUTTON_HF_DND_CHANGE_USER_AFE)->EnableWindow(bChecHfDnd && CTestOptionDlg::m_TestInfo_HF_DND.AfeInfo.bUseAfeSetting);

		GetDlgItem(IDC_BUTTON_HF_DND_MODIFY_MAX_SPEC)->EnableWindow(bChecHfDnd);
		GetDlgItem(IDC_BUTTON_HF_DND_MODIFY_MIN_SPEC)->EnableWindow(bChecHfDnd);

		//  HF DND H GAP & V GAP
		GetDlgItem(IDC_CHECK_HF_DND_DIFF)->EnableWindow(bChecHfDnd);

		BOOL bCheckHfDndDiff = bChecHfDnd && CTestOptionDlg::m_TestInfo_HF_DND.bCheckDataDiff;
		GetDlgItem(IDC_BUTTON_HF_DND_MODIFY_HDIFF)->EnableWindow(bCheckHfDndDiff);
		GetDlgItem(IDC_BUTTON_HF_DND_MODIFY_VDIFF)->EnableWindow(bCheckHfDndDiff);
	}

	if (nUpdateID & CTRL_UPDATE_ONLYSELF_DND)
	{
		BOOL bChecked = CTestOptionDlg::m_TestInfo_ONLYSELF_DND.bCheckData;

		GetDlgItem(IDC_BUTTON_ONLYSELF_DND_TX_MODIFY_MAX_SPEC)->EnableWindow(bChecked);
		GetDlgItem(IDC_BUTTON_ONLYSELF_DND_TX_MODIFY_MIN_SPEC)->EnableWindow(bChecked);
		GetDlgItem(IDC_BUTTON_ONLYSELF_DND_RX_MODIFY_MAX_SPEC)->EnableWindow(bChecked);
		GetDlgItem(IDC_BUTTON_ONLYSELF_DND_RX_MODIFY_MIN_SPEC)->EnableWindow(bChecked);

		bChecked = !bChecked;
		GetDlgItem(IDC_CHECK_SELF_DND)->EnableWindow(bChecked);
		GetDlgItem(IDC_CHECK_SELF_SATURATION)->EnableWindow(bChecked);
	}

	if( nUpdateID & CTRL_UPDATE_SELF_DND )
	{
		BOOL bCheckSelfDnd = CTestOptionDlg::m_TestInfo_SELF_DND.bCheckData;

		GetDlgItem(IDC_CHECK_ONLYSELF_DND)->EnableWindow(!bCheckSelfDnd);

		GetDlgItem(IDC_CHECK_SELF_DND_USE_USER_AFE)->EnableWindow(bCheckSelfDnd);
		GetDlgItem(IDC_BUTTON_SELF_DND_CHANGE_USER_AFE)->EnableWindow(bCheckSelfDnd && CTestOptionDlg::m_TestInfo_SELF_DND.AfeInfo.bUseAfeSetting);

		GetDlgItem(IDC_BUTTON_SELF_DND_TX_MODIFY_MAX_SPEC)->EnableWindow(bCheckSelfDnd);
		GetDlgItem(IDC_BUTTON_SELF_DND_TX_MODIFY_MIN_SPEC)->EnableWindow(bCheckSelfDnd);
		GetDlgItem(IDC_BUTTON_SELF_DND_RX_MODIFY_MAX_SPEC)->EnableWindow(bCheckSelfDnd);
		GetDlgItem(IDC_BUTTON_SELF_DND_RX_MODIFY_MIN_SPEC)->EnableWindow(bCheckSelfDnd);

		BOOL bEnableSelfDndDiff = bEnableIFE && bCheckSelfDnd;
		GetDlgItem(IDC_CHECK_SELF_DND_DIFF)->EnableWindow(bEnableSelfDndDiff);
		GetDlgItem(IDC_BUTTON_SELF_DND_TX_MODIFY_HDIFF_SPEC)->EnableWindow(bEnableSelfDndDiff && CTestOptionDlg::m_TestInfo_SELF_DND.bCheckDataDiff);
		GetDlgItem(IDC_BUTTON_SELF_DND_RX_MODIFY_HDIFF_SPEC)->EnableWindow(bEnableSelfDndDiff && CTestOptionDlg::m_TestInfo_SELF_DND.bCheckDataDiff);
	}

	if( nUpdateID & CTRL_UPDATE_SELF_SATURATION )
	{
		BOOL bEnableSelfSaturation = bEnableIFE && CTestOptionDlg::m_TestInfo_SELF_SATURATION.bCheckData;

		GetDlgItem(IDC_CHECK_ONLYSELF_DND)->EnableWindow(!bEnableSelfSaturation);

		GetDlgItem(IDC_CHECK_SELF_SATURATION)->EnableWindow(bEnableIFE);

		if (CTestOptionDlg::m_TestInfo_ONLYSELF_DND.bCheckData == TRUE)
			GetDlgItem(IDC_CHECK_SELF_SATURATION)->EnableWindow(FALSE);

		GetDlgItem(IDC_CHECK_SELF_SATURATION_USE_USER_AFE)->EnableWindow(bEnableSelfSaturation);
		GetDlgItem(IDC_BUTTON_SELF_SATURATION_CHANGE_USER_AFE)->EnableWindow(bEnableSelfSaturation && CTestOptionDlg::m_TestInfo_SELF_SATURATION.AfeInfo.bUseAfeSetting);

		GetDlgItem(IDC_BUTTON_SELF_SATURATION_TX_MODIFY_MAX_SPEC)->EnableWindow(bEnableSelfSaturation);
		GetDlgItem(IDC_BUTTON_SELF_SATURATION_TX_MODIFY_MIN_SPEC)->EnableWindow(bEnableSelfSaturation);
		GetDlgItem(IDC_BUTTON_SELF_SATURATION_RX_MODIFY_MAX_SPEC)->EnableWindow(bEnableSelfSaturation);
		GetDlgItem(IDC_BUTTON_SELF_SATURATION_RX_MODIFY_MIN_SPEC)->EnableWindow(bEnableSelfSaturation);
	}

	if( nUpdateID & CTRL_UPDATE_CURRENT_CONSUMPTION )
	{
#if defined(D_USE_CHECK_MPTOOL_REVISION)
		BOOL bEnableCurrent = CTestOptionDlg::m_TestInfo_CURRENT_CONSUMPTION.bEnable ? TRUE : FALSE;
		GetDlgItem(IDC_CHECK_CURRENT_CONSUMPTION)->EnableWindow(bEnableCurrent);
#else
		BOOL bEnableCurrent = CTestOptionDlg::m_TestInfo_CURRENT_CONSUMPTION.Active.bCheckData ? TRUE : FALSE;
#endif

		GetDlgItem(IDC_EDIT_CURRENT_CONSUMPTION_MAX)->EnableWindow(bEnableCurrent);
		GetDlgItem(IDC_EDIT_CURRENT_CONSUMPTION_MIN)->EnableWindow(bEnableCurrent);
		GetDlgItem(IDC_EDIT_CURRENT_CONSUMPTION_AVG_MS)->EnableWindow(bEnableCurrent);
		GetDlgItem(IDC_STATIC_CURRENT_CONSUMPTION_MAX)->EnableWindow(bEnableCurrent);
		GetDlgItem(IDC_STATIC_CURRENT_CONSUMPTION_MIN)->EnableWindow(bEnableCurrent);
		GetDlgItem(IDC_STATIC_CURRENT_CONSUMPTION)->EnableWindow(bEnableCurrent);
		GetDlgItem(IDC_STATIC_CURRENT_CONSUMPTION_AVG_MS_TITLE)->EnableWindow(bEnableCurrent);
		GetDlgItem(IDC_STATIC_CURRENT_CONSUMPTION_AVG_MS)->EnableWindow(bEnableCurrent);
		GetDlgItem(IDC_BUTTON_CURRENT_IDLE_OPTION)->EnableWindow(bEnableCurrent);
	}
}

void CTestOptionDlg_Tab1::InitUiContorl()
{
	SetButtonConfig(&m_btnDndMax);
	SetButtonConfig(&m_btnDndMin);
	SetButtonConfig(&m_btnDndVDiff);
	SetButtonConfig(&m_btnDndHDiff);
	SetButtonConfig(&m_btnDndAfe);

	SetButtonConfig(&m_btnCndMax);
	SetButtonConfig(&m_btnCndMin);

	SetButtonConfig(&m_btnHfDndMax);
	SetButtonConfig(&m_btnHfDndMin);
	SetButtonConfig(&m_btnHfDndVDiff);
	SetButtonConfig(&m_btnHfDndHDiff);
	SetButtonConfig(&m_btnHfDndAfe);

	SetButtonConfig(&m_btnSelfDndTxMax);
	SetButtonConfig(&m_btnSelfDndTxMin);
	SetButtonConfig(&m_btnSelfDndTxHDiff);
	SetButtonConfig(&m_btnSelfDndRxMax);
	SetButtonConfig(&m_btnSelfDndRxMin);
	SetButtonConfig(&m_btnSelfDndRxHDiff);
	SetButtonConfig(&m_btnSelfDndAfe);

	SetButtonConfig(&m_btnSelfSaturationTxMax);
	SetButtonConfig(&m_btnSelfSaturationTxMin);
	SetButtonConfig(&m_btnSelfSaturationRxMax);
	SetButtonConfig(&m_btnSelfSaturationRxMin);
	SetButtonConfig(&m_btnSelfSaturationAfe);

	SetButtonConfig(&m_btnCurrentIdleOption);

	SetButtonConfig(&m_btnOnlySelfDndTxMax);
	SetButtonConfig(&m_btnOnlySelfDndTxMin);
	SetButtonConfig(&m_btnOnlySelfDndRxMax);
	SetButtonConfig(&m_btnOnlySelfDndRxMin);

	SetWindowTheme(GetDlgItem(IDC_CHECK_DND)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_DND_DIFF)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_DND_USE_USER_AFE)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_CHECK_CND)->m_hWnd, L"", L"");				// CND	//add 2022.11.14 for ZTM730 CND

	SetWindowTheme(GetDlgItem(IDC_CHECK_HIGH_VOLTAGE)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_CHECK_HF_DND)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_HF_DND_DIFF)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_HF_DND_USE_USER_AFE)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_CHECK_SELF_DND)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_ONLYSELF_DND)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_SELF_DND_DIFF)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_SELF_DND_USE_USER_AFE)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_CHECK_SELF_SATURATION)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_SELF_SATURATION_USE_USER_AFE)->m_hWnd, L"", L"");

	SetWindowTheme(GetDlgItem(IDC_CHECK_TRX_SHORT)->m_hWnd, L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHECK_CURRENT_CONSUMPTION)->m_hWnd, L"", L"");
}

HBRUSH CTestOptionDlg_Tab1::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor != CTLCOLOR_STATIC)
		return hbr;

	int nState = -1;

	switch (pWnd->GetDlgCtrlID())
	{
	case IDC_CHECK_DND:							nState = CTestOptionDlg::m_TestInfo_DND.bCheckData;	break;
	case IDC_CHECK_DND_DIFF:					nState = CTestOptionDlg::m_TestInfo_DND.bCheckData && CTestOptionDlg::m_TestInfo_DND.bCheckDataDiff;	break;
	case IDC_CHECK_DND_USE_USER_AFE:			nState = CTestOptionDlg::m_TestInfo_DND.bCheckData && CTestOptionDlg::m_TestInfo_DND.AfeInfo.bUseAfeSetting;	break;

	case IDC_CHECK_CND:							nState = CTestOptionDlg::m_TestInfo_CND.bCheckData;	break;	// CND	//add 2022.11.14 for ZTM730 CND

	case IDC_CHECK_HIGH_VOLTAGE:				nState = CTestOptionDlg::m_TestInfo_HIGH_VOLTAGE.bCheckData;	break;

	case IDC_CHECK_HF_DND:						nState = CTestOptionDlg::m_TestInfo_HF_DND.bCheckData;	break;
	case IDC_CHECK_HF_DND_DIFF:					nState = CTestOptionDlg::m_TestInfo_HF_DND.bCheckData && CTestOptionDlg::m_TestInfo_HF_DND.bCheckDataDiff;	break;
	case IDC_CHECK_HF_DND_USE_USER_AFE:			nState = CTestOptionDlg::m_TestInfo_HF_DND.bCheckData && CTestOptionDlg::m_TestInfo_HF_DND.AfeInfo.bUseAfeSetting;	break;

	case IDC_CHECK_SELF_DND:					nState = CTestOptionDlg::m_TestInfo_SELF_DND.bCheckData;	break;
	case IDC_CHECK_ONLYSELF_DND:				nState = CTestOptionDlg::m_TestInfo_ONLYSELF_DND.bCheckData;	break;
	case IDC_CHECK_SELF_DND_DIFF:				nState = CTestOptionDlg::m_TestInfo_SELF_DND.bCheckData && CTestOptionDlg::m_TestInfo_SELF_DND.bCheckDataDiff;	break;
	case IDC_CHECK_SELF_DND_USE_USER_AFE:		nState = CTestOptionDlg::m_TestInfo_SELF_DND.bCheckData && CTestOptionDlg::m_TestInfo_SELF_DND.AfeInfo.bUseAfeSetting;	break;

	case IDC_CHECK_SELF_SATURATION:					nState = CTestOptionDlg::m_TestInfo_SELF_SATURATION.bCheckData;	break;
	case IDC_CHECK_SELF_SATURATION_USE_USER_AFE:	nState = CTestOptionDlg::m_TestInfo_SELF_SATURATION.bCheckData && CTestOptionDlg::m_TestInfo_SELF_SATURATION.AfeInfo.bUseAfeSetting;	break;

	case IDC_CHECK_TRX_SHORT:					nState = CTestOptionDlg::m_bCheckTrxShort;	break;
	case IDC_CHECK_CURRENT_CONSUMPTION:			nState = CTestOptionDlg::m_TestInfo_CURRENT_CONSUMPTION.Active.bCheckData;	break;
	}

	if (nState != -1)
	{
		ChangeCheckboxBgColor(pDC, nState);
	}

	return hbr;
}

void CTestOptionDlg_Tab1::OnBnClickedButtonOnlyselfDndTxModifyMaxSpec()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_ONLYSELF_DND.pTxDataMax[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_ONLYSELF_DND.nTxDataMax;

	if (CTestOptionDlg::m_bUseIFE)
	{ // IFE
		dlg.m_totalX = CTestOptionDlg::m_icRegData_SelfNumOfX;
		dlg.m_totalY = CTestOptionDlg::m_icRegData_SelfNumOfY;
	}
	else
	{ // CHINA
		dlg.m_totalX = CTestOptionDlg::m_IC_Max_X_Line_Num;
		dlg.m_totalY = CTestOptionDlg::m_IC_Max_Y_Line_Num;
	}

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_ONLYSELF_DND.pTxDataMax[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_ONLYSELF_DND.nTxDataMax = dlg.m_nApplyValue;
}


void CTestOptionDlg_Tab1::OnBnClickedButtonOnlyselfDndTxModifyMinSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_ONLYSELF_DND.pTxDataMin[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_ONLYSELF_DND.nTxDataMin;

	if (CTestOptionDlg::m_bUseIFE)
	{ // IFE
		dlg.m_totalX = CTestOptionDlg::m_icRegData_SelfNumOfX;
		dlg.m_totalY = CTestOptionDlg::m_icRegData_SelfNumOfY;
	}
	else
	{ // CHINA
		dlg.m_totalX = CTestOptionDlg::m_IC_Max_X_Line_Num;
		dlg.m_totalY = CTestOptionDlg::m_IC_Max_Y_Line_Num;
	}

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_ONLYSELF_DND.pTxDataMin[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_ONLYSELF_DND.nTxDataMin = dlg.m_nApplyValue;
}


void CTestOptionDlg_Tab1::OnBnClickedButtonOnlyselfDndRxModifyMaxSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_ONLYSELF_DND.pRxDataMax[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_ONLYSELF_DND.nRxDataMax;

	if (CTestOptionDlg::m_bUseIFE)
	{ // IFE
		dlg.m_totalX = CTestOptionDlg::m_icRegData_SelfNumOfX;
		dlg.m_totalY = CTestOptionDlg::m_icRegData_SelfNumOfY;
	}
	else
	{ // CHINA
		dlg.m_totalX = CTestOptionDlg::m_IC_Max_X_Line_Num;
		dlg.m_totalY = CTestOptionDlg::m_IC_Max_Y_Line_Num;
	}

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_ONLYSELF_DND.pRxDataMax[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_ONLYSELF_DND.nRxDataMax = dlg.m_nApplyValue;
}


void CTestOptionDlg_Tab1::OnBnClickedButtonOnlyselfDndRxModifyMinSpec()
{
	CTestModeDiffSpecDlg dlg;
	int x, y;
	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			dlg.nDiff[x][y] = CTestOptionDlg::m_TestInfo_ONLYSELF_DND.pRxDataMin[x][y];
		}
	}

	dlg.m_nApplyValue = CTestOptionDlg::m_TestInfo_ONLYSELF_DND.nRxDataMin;

	if (CTestOptionDlg::m_bUseIFE)
	{ // IFE
		dlg.m_totalX = CTestOptionDlg::m_icRegData_SelfNumOfX;
		dlg.m_totalY = CTestOptionDlg::m_icRegData_SelfNumOfY;
	}
	else
	{ // CHINA
		dlg.m_totalX = CTestOptionDlg::m_IC_Max_X_Line_Num;
		dlg.m_totalY = CTestOptionDlg::m_IC_Max_Y_Line_Num;
	}

	if (dlg.DoModal() != IDOK)
		return;

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			CTestOptionDlg::m_TestInfo_ONLYSELF_DND.pRxDataMin[x][y] = dlg.nDiff[x][y];
		}
	}

	CTestOptionDlg::m_TestInfo_ONLYSELF_DND.nRxDataMin = dlg.m_nApplyValue;
}
