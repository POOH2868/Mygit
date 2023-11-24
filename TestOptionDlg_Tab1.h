#pragma once

#include ".\CustomUI\CustomButton\ShadeButtonST.h"

// CTestOptionDlg_Tab1 dialog

class CTestOptionDlg_Tab1 : public CDialog
{
	DECLARE_DYNAMIC(CTestOptionDlg_Tab1)

public:
	CTestOptionDlg_Tab1(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestOptionDlg_Tab1();

// Dialog Data
	enum { IDD = IDD_TEST_OPTION_TAB_1 };

protected:

	CShadeButtonST m_btnDndMax;
	CShadeButtonST m_btnDndMin;
	CShadeButtonST m_btnDndVDiff;
	CShadeButtonST m_btnDndHDiff;
	CShadeButtonST m_btnDndAfe;

	CShadeButtonST m_btnHfDndMax;
	CShadeButtonST m_btnHfDndMin;
	CShadeButtonST m_btnHfDndVDiff;
	CShadeButtonST m_btnHfDndHDiff;
	CShadeButtonST m_btnHfDndAfe;

	CShadeButtonST m_btnSelfDndTxMax;
	CShadeButtonST m_btnSelfDndTxMin;
	CShadeButtonST m_btnSelfDndTxHDiff;
	CShadeButtonST m_btnSelfDndRxMax;
	CShadeButtonST m_btnSelfDndRxMin;
	CShadeButtonST m_btnSelfDndRxHDiff;
	CShadeButtonST m_btnSelfDndAfe;

	CShadeButtonST m_btnSelfSaturationTxMax;
	CShadeButtonST m_btnSelfSaturationTxMin;
	CShadeButtonST m_btnSelfSaturationRxMax;
	CShadeButtonST m_btnSelfSaturationRxMin;
	CShadeButtonST m_btnSelfSaturationAfe;

	CShadeButtonST m_btnCurrentIdleOption;

	CShadeButtonST m_btnOnlySelfDndTxMax;
	CShadeButtonST m_btnOnlySelfDndTxMin;
	CShadeButtonST m_btnOnlySelfDndRxMax;
	CShadeButtonST m_btnOnlySelfDndRxMin;

	CShadeButtonST m_btnCndMax;
	CShadeButtonST m_btnCndMin;

	void InitUiContorl();

public:

	void EnableDialogControls(int nUpdateID);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnCheckBnClicked();

	// DND
	afx_msg void OnBnClicked_Dnd_ModifyMaxSpec();
	afx_msg void OnBnClicked_Dnd_ModifyMinSpec();
	afx_msg void OnBnClicked_Dnd_ModifyHDiff();
	afx_msg void OnBnClicked_Dnd_ModifyVDiff();

	// CND	//add 2022.11.14 for ZTM730 CND
	afx_msg void OnBnClicked_Cnd_ModifyMaxSpec();
	afx_msg void OnBnClicked_Cnd_ModifyMinSpec();

	// HF DND
	afx_msg void OnBnClicked_HfDnd_ModifyMaxSpec();
	afx_msg void OnBnClicked_HfDnd_ModifyMinSpec();
	afx_msg void OnBnClicked_HfDnd_ModifyHDiff();
	afx_msg void OnBnClicked_HfDnd_ModifyVDiff();

	// ONLY SELF_DND
	afx_msg void OnBnClickedButtonOnlyselfDndTxModifyMaxSpec();
	afx_msg void OnBnClickedButtonOnlyselfDndTxModifyMinSpec();
	afx_msg void OnBnClickedButtonOnlyselfDndRxModifyMaxSpec();
	afx_msg void OnBnClickedButtonOnlyselfDndRxModifyMinSpec();

	// SELF_DND
	afx_msg void OnBnClicked_SelfDnd_Tx_ModifyMaxSpec();
	afx_msg void OnBnClicked_SelfDnd_Tx_ModifyMinSpec();
	afx_msg void OnBnClicked_SelfDnd_Tx_ModifyHDiff();

	afx_msg void OnBnClicked_SelfDnd_Rx_ModifyMaxSpec();
	afx_msg void OnBnClicked_SelfDnd_Rx_ModifyMinSpec();
	afx_msg void OnBnClicked_SelfDnd_Rx_ModifyHDiff();

	// SELF_SATURATION
	afx_msg void OnBnClicked_SelfSaturation_Tx_ModifyMaxSpec();
	afx_msg void OnBnClicked_SelfSaturation_Tx_ModifyMinSpec();

	afx_msg void OnBnClicked_SelfSaturation_Rx_ModifyMaxSpec();
	afx_msg void OnBnClicked_SelfSaturation_Rx_ModifyMinSpec();

	afx_msg void OnBnClicked_ChangeUserAFE();
	afx_msg void OnBnClicked_ChangeUserAFE_Multi();

	afx_msg void OnBnClicked_CurrentIdleOption();

	DECLARE_MESSAGE_MAP()
};

