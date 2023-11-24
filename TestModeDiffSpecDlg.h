#pragma once
#include "afxwin.h"
#include ".\CustomUI\CustomButton\BtnST.h"
#include ".\CustomUI\CustomButton\ShadeButtonST.h"
#include ".\CustomUI\ResizableDialog.h"
#include "Global.h"
#include "afxcmn.h"

// CTestModeDiffSpecDlg 대화 상자입니다.

class CTestModeDiffSpecDlg : public CResizableDialog
{
	//DECLARE_DYNAMIC(CTestModeDiffSpecDlg)

public:
	CTestModeDiffSpecDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTestModeDiffSpecDlg();

	// 대화 상자 데이터입니다.
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
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
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

