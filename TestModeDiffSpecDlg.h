#pragma once
#include "afxwin.h"
#include ".\CustomUI\CustomButton\BtnST.h"
#include ".\CustomUI\CustomButton\ShadeButtonST.h"
#include ".\CustomUI\ResizableDialog.h"
#include "Global.h"
#include "afxcmn.h"

// CTestModeDiffSpecDlg ��ȭ �����Դϴ�.

class CTestModeDiffSpecDlg : public CResizableDialog
{
	//DECLARE_DYNAMIC(CTestModeDiffSpecDlg)

public:
	CTestModeDiffSpecDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTestModeDiffSpecDlg();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_NODE_SHEET };

	int m_totalX;
	int m_totalY;

	int m_nApplyValue;

	// Diff Test
	int nDiff[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM];	

	BOOL bHDiff;
	BOOL bVDiff;
	BOOL bSelf;

protected:

	CListCtrl m_ctrlNodeList;

	int iSavedItem;
	int iSavedSubitem;

	CShadeButtonST m_btnOK;
	CShadeButtonST m_btnCancel;
	CShadeButtonST m_btnApplyAllNode;
	
protected:
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedOk();
	afx_msg void OnNMDblclkListNode(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListNode(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnBnClickedButtonApplyAllNode();

	void InitNodeList();
	void UpdateNodeList();
};

