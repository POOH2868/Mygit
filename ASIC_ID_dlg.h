#pragma once

#include "Global.h"

#include "afxwin.h"
#include "Hid\UsbHid.h"

#include ".\CustomUI\Title.h"
#include ".\CustomUI\ColorListCtrl.h"
#include ".\CustomUI\CustomButton\ShadeButtonST.h"

// ASIC_ID_dlg ��ȭ �����Դϴ�.

class ASIC_ID_dlg : public CDialog
{
	DECLARE_DYNAMIC(ASIC_ID_dlg)

public:
	ASIC_ID_dlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~ASIC_ID_dlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_ASIC_ID };

protected:

	CWnd *				m_pParent;
	
	CColorListCtrl		m_ListCtrl;

	CEdit				mEdit_ASIC_ID_0;
	CEdit				mEdit_ASIC_ID_1;

	CTitle				m_Title;
	CFont				m_ListFont;
	CShadeButtonST		m_btnOK;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	afx_msg void OnPaint();

	LRESULT OnSaveIum(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	void ListCtrl_Init();
	void ReadAsicID();
	void DisplayAsicID();
};
