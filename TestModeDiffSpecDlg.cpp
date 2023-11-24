// TestModeDiffSpecDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "TestModeDiffSpecDlg.h"
#include "afxdialogex.h"
#include "util.h"


// CTestModeDiffSpecDlg 대화 상자입니다.

//IMPLEMENT_DYNAMIC(CTestModeDiffSpecDlg, CResizableDialog)

CTestModeDiffSpecDlg::CTestModeDiffSpecDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CTestModeDiffSpecDlg::IDD, pParent)
{
	bHDiff = FALSE;
	bVDiff = FALSE;
	bSelf = FALSE;
}

CTestModeDiffSpecDlg::~CTestModeDiffSpecDlg()
{
}

void CTestModeDiffSpecDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_NODE, m_ctrlNodeList);
	DDX_Text(pDX, IDC_EDIT_APPLY_ALL_NODE_VALUE, m_nApplyValue);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BUTTON_APPLY_ALL_NODE, m_btnApplyAllNode);
}


BEGIN_MESSAGE_MAP(CTestModeDiffSpecDlg, CResizableDialog)
	ON_BN_CLICKED(IDOK, &CTestModeDiffSpecDlg::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_NODE, &CTestModeDiffSpecDlg::OnNMDblclkListNode)
	ON_NOTIFY(NM_CLICK, IDC_LIST_NODE, &CTestModeDiffSpecDlg::OnNMClickListNode)
	ON_BN_CLICKED(IDC_BUTTON_APPLY_ALL_NODE, &CTestModeDiffSpecDlg::OnBnClickedButtonApplyAllNode)
END_MESSAGE_MAP()


// CTestModeDiffSpecDlg 메시지 처리기입니다.


void CTestModeDiffSpecDlg::OnBnClickedOk()
{
	CResizableDialog::OnOK();
}


BOOL CTestModeDiffSpecDlg::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	this->SetWindowTextA("SPEC [After edit in cell, press the enter key]");		

	m_btnOK.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	m_btnOK.SetIcon(IDI_ICON_OK, IDI_ICON_OK_GRAY);
	m_btnOK.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(125, 184, 56));
	m_btnOK.SetBtnCursor(IDC_CURSOR_HAND);

	m_btnCancel.SetShade(CShadeButtonST::SHS_HARDBUMP);
	m_btnCancel.SetIcon(IDI_ICON_CANCEL, IDI_ICON_CANCEL_GRAY);
	m_btnCancel.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(240, 26, 40));
	m_btnCancel.SetBtnCursor(IDC_CURSOR_HAND);

	SetButtonConfig(&m_btnApplyAllNode);

	GetDlgItem(IDC_EDIT_MOD)->SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW);
	InitNodeList();
	UpdateNodeList();

	AddAnchor(IDC_LIST_NODE, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDOK, BOTTOM_LEFT);
	AddAnchor(IDCANCEL, BOTTOM_RIGHT);
	AddAnchor(IDC_BUTTON_APPLY_ALL_NODE, BOTTOM_CENTER);
	AddAnchor(IDC_EDIT_APPLY_ALL_NODE_VALUE, BOTTOM_CENTER);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CTestModeDiffSpecDlg::InitNodeList()
{
	LV_COLUMN	lvcolumn;
	LV_ITEM	lvitem;
	RECT	cRect;
	char	chstr[255];
	CString	str;
	int i, n;

	m_ctrlNodeList.SetExtendedStyle(LVS_EX_GRIDLINES);
	m_ctrlNodeList.DeleteAllItems();

	if (m_totalY >= DEF_MAX_Y_LINE_NUM || m_totalY <= 0)
		m_totalY = DEF_MAX_Y_LINE_NUM - 1;
	
	m_ctrlNodeList.GetClientRect(&cRect);
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_CENTER;
	lvcolumn.pszText = "IDX";
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = 50;
	m_ctrlNodeList.InsertColumn(0, &lvcolumn);

	//lvcolumn.cx = (cRect.right - 80) / (m_totalY);
	lvcolumn.cx = 70;

	// Header - Column 
	for (i = 0; i < (int)m_totalY; i++)
	{
		if (bHDiff)
			sprintf(chstr, "[%02d-%02d]", i, i+1);
		else
			sprintf(chstr, "%02d", i);

		lvcolumn.pszText = chstr;

		lvcolumn.iSubItem = i + 1;
		m_ctrlNodeList.InsertColumn(i + 1, &lvcolumn);
	}
		
	lvitem.mask = LVIF_TEXT;
	lvitem.iItem = m_ctrlNodeList.GetItemCount();

	if (bSelf)
	{
		m_totalX = 1;
	}
	else
	{
		if (m_totalX >= DEF_MAX_X_LINE_NUM || m_totalX <= 0)
			m_totalX = DEF_MAX_X_LINE_NUM - 1;
	}

	// Header - Row
	for (i = 0; i < (int)m_totalX; i++)
	{
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = m_ctrlNodeList.GetItemCount();
		lvitem.iSubItem = 0;

		if (bVDiff)
			sprintf(chstr, "[%02d-%02d]", i, i + 1);
		else
			sprintf(chstr, "%02d", i);

		lvitem.pszText = chstr;

		n = m_ctrlNodeList.InsertItem(&lvitem);

	}
}


void CTestModeDiffSpecDlg::UpdateNodeList()
{
	int x, y;
	LV_ITEM	lvitem;
	CString strInfo;

	lvitem.mask = LVIF_TEXT;

	m_ctrlNodeList.SetRedraw(FALSE);

	for (x = 0; x < (int)m_totalX; x++)
	{
		lvitem.iItem = x;
		for (y = 0; y < (int)m_totalY; y++)
		{
			strInfo.Format("%05d", nDiff[x][y]);
			lvitem.pszText = (LPSTR)(LPCTSTR)strInfo;
			lvitem.iSubItem = y + 1;
			m_ctrlNodeList.SetItem(&lvitem);

			//m_ctrlNodeList.SetColumnWidth(y+1, LVSCW_AUTOSIZE);
		}
	}

	m_ctrlNodeList.SetRedraw(TRUE);	
}

void CTestModeDiffSpecDlg::OnNMDblclkListNode(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
#if 0
	LPNMITEMACTIVATE pNMITEM = (LPNMITEMACTIVATE)pNMHDR;
	iSavedItem = pNMItemActivate->iItem;
	iSavedSubitem = pNMItemActivate->iSubItem;
#endif

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	LVHITTESTINFO ht;
	ht.pt = pNMListView->ptAction;
	m_ctrlNodeList.SubItemHitTest(&ht);
	
	if (ht.iItem == -1) {
		*pResult = 0;
		return;
	}

	// 전역변수에 저장합니다. 구조체 배열의 위치겠죠?
	iSavedItem = ht.iItem;
	iSavedSubitem = ht.iSubItem;
	
	if (iSavedItem != -1 && iSavedSubitem > 0) {
		CRect rect;
		
		m_ctrlNodeList.GetSubItemRect(iSavedItem, iSavedSubitem, LVIR_BOUNDS, rect);
		m_ctrlNodeList.ClientToScreen(rect);
		this->ScreenToClient(rect);
		GetDlgItem(IDC_EDIT_MOD)->SetWindowText(m_ctrlNodeList.GetItemText(iSavedItem, iSavedSubitem));
		GetDlgItem(IDC_EDIT_MOD)->SetWindowPos(NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
		GetDlgItem(IDC_EDIT_MOD)->SetFocus();
	}	
	*pResult = 0;
}


void CTestModeDiffSpecDlg::OnNMClickListNode(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	GetDlgItem(IDC_EDIT_MOD)->SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW);
	*pResult = 0;
}


BOOL CTestModeDiffSpecDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			if (pMsg->hwnd == GetDlgItem(IDC_EDIT_MOD)->GetSafeHwnd())
			{
				CString str;
				GetDlgItemText(IDC_EDIT_MOD, str);		

				nDiff[iSavedItem][iSavedSubitem - 1] = char2dec(str.GetBuffer());
				str.Format("%04d", nDiff[iSavedItem][iSavedSubitem - 1]);
				m_ctrlNodeList.SetItemText(iSavedItem, iSavedSubitem, str);

				GetDlgItem(IDC_EDIT_MOD)->SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW);
			}
			return TRUE;
		}
		else if (pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
	}

	return CResizableDialog::PreTranslateMessage(pMsg);
}


void CTestModeDiffSpecDlg::OnBnClickedButtonApplyAllNode()
{
	CString strInfo;
	int x, y;

	UpdateData(TRUE);

	if (bHDiff || bVDiff)
	{
		if (m_nApplyValue < 0 || m_nApplyValue > 100)
		{
			AfxMessageBox("[ERROR] This Register's MIN VALUE : 0 / MAX VALUE : 100");
			return;
		}
	}

	if (bSelf)
	{
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++)
		{
			nDiff[0][y] = m_nApplyValue;
		}
	}
	else
	{
		for (x = 0; x < DEF_MAX_X_LINE_NUM; x++)
		{
			for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++)
			{
				nDiff[x][y] = m_nApplyValue;
			}
		}
	}
	
	LV_ITEM	lvitem;
	lvitem.mask = LVIF_TEXT;
	
	for (x = 0; x < (int)m_totalX; x++)
	{
		lvitem.iItem = x;
		for (y = 0; y < (int)m_totalY; y++)
		{
			strInfo.Format("%05d", nDiff[x][y]);
			lvitem.pszText = (LPSTR)(LPCTSTR)strInfo;
			lvitem.iSubItem = y + 1;
			m_ctrlNodeList.SetItem(&lvitem);

			//m_ctrlNodeList.SetColumnWidth(y+1, LVSCW_AUTOSIZE);
		}
	}

	m_ctrlNodeList.SetRedraw(TRUE);
}
