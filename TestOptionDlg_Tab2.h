#pragma once

#include ".\CustomUI\CustomButton\ShadeButtonST.h"

// CTestOptionDlg_Tab2 dialog

class CTestOptionDlg_Tab2 : public CDialog
{
	DECLARE_DYNAMIC(CTestOptionDlg_Tab2)

public:
	CTestOptionDlg_Tab2(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestOptionDlg_Tab2();

// Dialog Data
	enum { IDD = IDD_TEST_OPTION_TAB_2 };

protected:

	CShadeButtonST m_btnMeasureDeltaMax;
	CShadeButtonST m_btnMeasureDeltaMin;
	CShadeButtonST m_btnMeasureDeltaSelfRxMax;
	CShadeButtonST m_btnMeasureDeltaSelfRxMin;
	CShadeButtonST m_btnMeasureDeltaSelfTxMax;
	CShadeButtonST m_btnMeasureDeltaSelfTxMin;
	CShadeButtonST m_btnMeasureDeltaAfe;

	CShadeButtonST m_btnHwCalAfe;
	CShadeButtonST m_btnJitterAfe;

	CShadeButtonST m_btnDigitalWheelDeltaMax;
	CShadeButtonST m_btnDigitalWheelDeltaMin;

	CShadeButtonST m_btnNoiseAfe;

	void InitUiContorl();

public:

	void EnableDialogControls(int nUpdateID);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnCheckBnClicked();

	// MEASURE DELTA
	afx_msg void OnBnClicked_MeasureDelta_ModifyMaxSpec();
	afx_msg void OnBnClicked_MeasureDelta_ModifyMinSpec();

	// MEASURE DELTA - SELF
	afx_msg void OnBnClicked_MeasureDelta_ModifySelfRxMaxSpec();
	afx_msg void OnBnClicked_MeasureDelta_ModifySelfRxMinSpec();
	afx_msg void OnBnClicked_MeasureDelta_ModifySelfTxMaxSpec();
	afx_msg void OnBnClicked_MeasureDelta_ModifySelfTxMinSpec();

	afx_msg void OnBnClicked_ChangeUserAFE();

	// DIGITAL WHEEL
	afx_msg void OnBnClicked_DigitalWheel_ModifyMaxSpec();
	afx_msg void OnBnClicked_DigitalWheel_ModifyMinSpec();

	// NOISE
	afx_msg void OnBnClickedButtonNoiseUserAfe();

	DECLARE_MESSAGE_MAP()
};

