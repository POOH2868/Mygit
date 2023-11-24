#if !defined(AFX_CONNECT_INFO_DLG_H__134B5EBA_42CA_43B3_B0AB_96FB3A6E5E5A__INCLUDED_)
#define AFX_CONNECT_INFO_DLG_H__134B5EBA_42CA_43B3_B0AB_96FB3A6E5E5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MessageBox.h : header file
//

#include ".\Hid\UsbHid.h"
#include ".\CustomUI\ColorListCtrl.h"

#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)

/////////////////////////////////////////////////////////////////////////////
// CConnectInfoDlg dialog

class CConnectInfoDlg : public CDialog
{
// Construction
public:
	CConnectInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConnectInfoDlg)
	enum { IDD = IDD_DIALOG_CONNECT_INFO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectInfoDlg)
public:
	virtual INT_PTR DoModal();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConnectInfoDlg)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBnClickedButtonLogClear();
	LRESULT OnClickListCtrl(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	void  Init();

public:
	
	void AddLog(CString strConnectivity, CUsbHid * pUsbHid);

	/*
	void  SetTimerType( UINT nIDEvent=0, int nTime = 3);
	void  SetMessage(   const char* strMsg=NULL);
	void  SetTitle(     const char* strTitle);
	*/
	void  ShowMsgBox(int nShow);

protected:

	int        m_nElapseTime;

	BOOL       m_bDoModal;

	CColorListCtrl		m_LogList;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

#endif // !defined(AFX_CONNECT_INFO_DLG_H__134B5EBA_42CA_43B3_B0AB_96FB3A6E5E5A__INCLUDED_)
