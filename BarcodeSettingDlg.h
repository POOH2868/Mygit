#pragma once

#include ".\CustomUI\Title.h"
#include ".\CustomUI\ColorListCtrl.h"
#include ".\CustomUI\CustomButton\ShadeButtonST.h"
#include "Global.h"

// CBarcodeSettingDlg dialog

enum
{
	E_BARCODE_SETTING_START_TEST 	= 0,
	E_BARCODE_SETTING_HIDE_DIALOG
};

class CBarcodeSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBarcodeSettingDlg)

public:
	CBarcodeSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBarcodeSettingDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_BARCODE };

protected:

	CWnd *					m_pParent;

	CColorListCtrl			m_ListCtrl;

	int						m_nListSelectionItem;

	BOOL 					m_bAutoStartTest;
	BOOL 					m_bFixedBarcodeLength;
	int 					m_nBarcode_Length;

	int						m_nBarcodeIdx;
	int						m_nBarcodeChIdx;

	char					m_cBarcode[1000];

	//UINT64				m_BarcodeInputTime;

	CTitle					m_Title;
	CFont					m_ListFont;

	CShadeButtonST			m_btnStartTestAll;
	CShadeButtonST			m_btnPartialStart[MAX_DEVICE_COUNT];

	BOOL					m_bBarcodeAutoClear[MAX_DEVICE_COUNT];
	BOOL 					m_bPartialTestOn;
	int						m_nPartialTestButtonPosX;
	int						m_nPartialTestButtonWidth;

public:

	BOOL 					m_bShowDialog;

	BOOL IsShowDialog() { return m_bShowDialog; }
	void SetBarcodeInfo(BOOL bAutoStart, BOOL bFixedLength, int nLength);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg LRESULT OnClickListBarcode(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClicked_StartTestAll();
	afx_msg void OnBnClicked_StartPartialTest(UINT nID);
	afx_msg void OnCheckBnClicked_PartialTest();

	LRESULT OnShowDialog(WPARAM wParam, LPARAM lParam);
	LRESULT OnDeviceChanged(WPARAM wParam, LPARAM lParam);
	LRESULT OnTestCompleted(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	void ShowDialog(int nShow);

	void UiCtrl_Initialize(BOOL bChangeFocus);
	void UiCtrl_SetListFocus(int nIndex);
	int UiCtrl_GetListFocus();
	void UiCtrl_EnableButton();

	void CompletedBarcodeInput();
	void ClearBarcodeInput();
	void EraseBarcodeInfo(int nIndex, BOOL bForce = FALSE);
	
	BOOL CheckBarcodeEmpty(int nIndex);
	BOOL CheckBarcodeLength(int nIndex);

	int GetNextSelectItem(int nCurItem);
};
