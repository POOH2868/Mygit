#pragma once

#include ".\CustomUI\CustomButton\ShadeButtonST.h"

// CTestOptionDlg_Tab0 dialog

class CTestOptionDlg_Tab0 : public CDialog
{
	DECLARE_DYNAMIC(CTestOptionDlg_Tab0)

public:
	CTestOptionDlg_Tab0(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestOptionDlg_Tab0();

// Dialog Data
	enum { IDD = IDD_TEST_OPTION_TAB_0 };

// operations

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	void fnIUM_Enable_Check();

	//2021.11.17 ±è±Ø·¡ Ã¥ÀÓ VISIONOX ÀÏ°æ¿ì Log Namming rule º¯°æ.
	void fnCheck_VendorVisinox();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnRadioBnClicked(UINT nID);
	afx_msg void OnCheckBnClicked();

	afx_msg void OnCbnSelchangeComboSaveInfoLocation();

	afx_msg void OnBnClickedButtonSelectSaveResultDir();
	afx_msg void OnBnClickedButtonSelectFirwareDir();

	afx_msg void OnBnClickedButtonDownloadFixedPosition();
	afx_msg void OnBnClickedButtonLogFixedPosition();

	DECLARE_MESSAGE_MAP()

	CString SelectDirecotry();

	void InitUiContorl();


public:

	void EnableDialogControls(int nUpdateID);
	void GetControlValue();

// attributes

protected:

	CComboBox m_ComboModuleID;
	CComboBox m_ComboIumFormat;
	CComboBox m_ComboI2CFormat;

	CShadeButtonST m_btnResetCount;
	CShadeButtonST m_btnDownloadBrowse;
	CShadeButtonST m_btnSaveResultBrowse;

	int m_nIcType_old;
};
