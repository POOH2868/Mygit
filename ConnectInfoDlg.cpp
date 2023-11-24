// MessageBox.cpp : implementation file
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "ConnectInfoDlg.h"

#if defined(D_USE_CHECK_IC_SIGNATURE_AFTER_USB_CONNECT)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectInfoDlg dialog


CConnectInfoDlg::CConnectInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConnectInfoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pParentWnd  = pParent;

	m_nElapseTime = 2000;

	m_bDoModal    = FALSE;
}


void CConnectInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_LIST_LOG, m_LogList);
}


BEGIN_MESSAGE_MAP(CConnectInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CConnectInfoDlg)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_BUTTON_LOG_CLEAR, OnBnClickedButtonLogClear)
	ON_MESSAGE(WM_USER_LIST_CTRL_LOG_CLICK, OnClickListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectInfoDlg message handlers

BOOL CConnectInfoDlg::OnInitDialog()
{
    CDialog::OnInitDialog() ;

	CRect rect;
	GetClientRect(rect);

	int iEdge = GetSystemMetrics( SM_CYEDGE ); 				// 테투리 라인 굵기 
	int iHeight = GetSystemMetrics( SM_CYCAPTION ) + iEdge; // 실제 높이 

	//SetWindowPos(&wndTopMost, 0, 0, 610, 370, SWP_HIDEWINDOW);
	SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height() + iHeight, SWP_HIDEWINDOW);

	CenterWindow();

	Init();

	m_LogList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_LogList.ModifyStyle(0, LVS_SHOWSELALWAYS);
	
	m_LogList.GetClientRect(rect);

	m_LogList.InsertColumn(0, _T(""), LVCFMT_CENTER, 0, 0);
	m_LogList.InsertColumn(1, _T("Date"), LVCFMT_CENTER, 75);
	m_LogList.InsertColumn(2, _T("Connectivity"), LVCFMT_CENTER, 100);
	m_LogList.InsertColumn(3, _T("ID"), LVCFMT_CENTER, 75);
	m_LogList.InsertColumn(4, _T("ERR_Signature"), LVCFMT_CENTER, 100);
	m_LogList.InsertColumn(5, _T("ERR_ID"), LVCFMT_CENTER, 100);

	m_LogList.m_parentHWnd = this->m_hWnd;

	return TRUE;
}

void CConnectInfoDlg::OnTimer(UINT_PTR nIDEvent) 
{
	KillTimer(nIDEvent);
	OnCancel();

	CWnd::OnTimer(nIDEvent);
}

void CConnectInfoDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
}

void CConnectInfoDlg::OnCancel() 
{
	if(m_bDoModal)
		CDialog::OnCancel();
	else
		ShowMsgBox(SW_HIDE);
}

void CConnectInfoDlg::OnOK() 
{
	if(m_bDoModal)
		CDialog::OnOK();
	else
		ShowMsgBox(SW_HIDE);
}

void CConnectInfoDlg::Init()
{
    
}

void CConnectInfoDlg::AddLog(CString strConnectivity, CUsbHid * pUsbHid)
{
	BOOL bShow = FALSE;

	int nCount = m_LogList.GetItemCount();
	CString strTemp;

	while (nCount > 300)
	{
		m_LogList.DeleteItem(300);

		nCount = m_LogList.GetItemCount();
	}

	CTime curTime = CTime::GetCurrentTime();

	m_LogList.InsertItem(0, "");
	m_LogList.SetItemText(0, 1, curTime.Format("%H:%M:%S"));
	m_LogList.SetItemText(0, 2, strConnectivity);

	strTemp.Format("%d", pUsbHid->m_nDevIndex);
	m_LogList.SetItemText(0, 3, strTemp);

	if (pUsbHid->m_bSignatureError)
	{
		bShow = TRUE;
		m_LogList.SetItemBkColor(0, 4, ZT_COLOR_RED);

		if (pUsbHid->m_nSignatureErrorTimeout)
			m_LogList.SetItemText(0, 4, "TIMEOUT");
		else if (pUsbHid->m_bI2cNoResponse)
			m_LogList.SetItemText(0, 4, "No Response");
		else
		{
			strTemp.Format("0x%x", pUsbHid->m_CapInfo.icSignature);
			m_LogList.SetItemText(0, 4, strTemp);
		}
	}
	else
	{
		strTemp.Format("0x%x", pUsbHid->m_CapInfo.icSignature);
		m_LogList.SetItemText(0, 4, strTemp);
	}

	if (pUsbHid->m_bDuplicateError){
		bShow = TRUE;
		m_LogList.SetItemText(0, 5, "ERROR");
		m_LogList.SetItemBkColor(0, 5, ZT_COLOR_RED);
	}
	else{
		m_LogList.SetItemText(0, 5, "OK");
	}

	//m_LogList.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	//m_LogList.SetFocus();
	m_LogList.EnsureVisible(0, TRUE);

	if (bShow){
		ShowMsgBox(SW_SHOW);
	}
}

void CConnectInfoDlg::OnBnClickedButtonLogClear()
{
	m_LogList.DeleteAllItems();
}

void CConnectInfoDlg::ShowMsgBox(int nShow)
{
	ShowWindow(nShow);

	if( !m_bDoModal )
	{
		if(nShow == SW_SHOW && m_nElapseTime != 0)
			SetTimer(TIMER_SHOW_CONNECT_INFO, m_nElapseTime, NULL);
		else
			KillTimer(TIMER_SHOW_CONNECT_INFO);
	}
}

INT_PTR CConnectInfoDlg::DoModal() 
{
	m_bDoModal = TRUE;
	return CDialog::DoModal();
}

LRESULT CConnectInfoDlg::OnClickListCtrl(WPARAM wParam, LPARAM lParam)
{
	KillTimer(TIMER_SHOW_CONNECT_INFO);

	return 1L;
}

BOOL CConnectInfoDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_F5)
		{
			KillTimer(TIMER_SHOW_CONNECT_INFO);
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

#endif
