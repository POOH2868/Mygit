// ASIC_ID_dlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "ASIC_ID_dlg.h"
#include "afxdialogex.h"

#include "TestModeView.h"
#include "hid\UsbHidList.h"
#include "util.h"
#include "InputDlg.h"

// ASIC_ID_dlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(ASIC_ID_dlg, CDialog)

ASIC_ID_dlg::ASIC_ID_dlg(CWnd* pParent /*=NULL*/)
	: CDialog(ASIC_ID_dlg::IDD, pParent)
{
	m_pParent = pParent;
}

ASIC_ID_dlg::~ASIC_ID_dlg()
{
	if (m_ListFont.GetSafeHandle())
		m_ListFont.DeleteObject();
}

void ASIC_ID_dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TITLE_ASIC_ID, m_Title);
	DDX_Control(pDX, IDC_LIST_ASIC_ID, m_ListCtrl);
	DDX_Control(pDX, IDOK, m_btnOK);
}


BEGIN_MESSAGE_MAP(ASIC_ID_dlg, CDialog)
	ON_WM_PAINT()
	ON_MESSAGE(WM_USER_DEV_SAVE_IUM_EVENT, &ASIC_ID_dlg::OnSaveIum)
END_MESSAGE_MAP()


// ASIC_ID_dlg 메시지 처리기입니다.

BOOL ASIC_ID_dlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Initialize ListCtrl
	ListCtrl_Init();

	m_Title.SetTitle("ASIC ID");
	m_Title.SetBkColor(80, 100, 120, 0, 0, 0);

	SetButtonConfig(&m_btnOK);

	ReadAsicID();
	DisplayAsicID();

	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL ASIC_ID_dlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
		else if (pMsg->wParam == VK_F1)
		{
			BOOL bRet;

			do
			{
				bRet = PostMessage(WM_USER_DEV_SAVE_IUM_EVENT, 0, 0);
			}while( bRet == 0 );
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void ASIC_ID_dlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	m_Title.SetBkColor(80, 100, 120, 0, 0, 0);
}

void ASIC_ID_dlg::ListCtrl_Init()
{
	m_ListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS); // 원하는 Row을 선택하기 위해서 중요

	CRect rect;
	m_ListCtrl.GetClientRect(rect);
	int nWidth = (rect.right - rect.left - 80) / 2;
	m_ListCtrl.InsertColumn(0, _T(""), LVCFMT_CENTER, 0, 0);
	m_ListCtrl.InsertColumn(1, _T("DEVICE"), LVCFMT_CENTER, 80, 0);
	m_ListCtrl.InsertColumn(2, _T("ASIC ID (0)"), LVCFMT_CENTER, nWidth, 0);
	m_ListCtrl.InsertColumn(3, _T("ASIC ID (1)"), LVCFMT_CENTER, nWidth, 0);

	// Font 글자 설정
	LOGFONT LogFont;
	GetDlgItem(IDC_LIST_ASIC_ID)->GetFont()->GetLogFont(&LogFont);
	LogFont.lfWeight = 1000;	// 굵기설정( MAX : 1000 )
	LogFont.lfHeight = 15;		// Font Size 설정
	m_ListFont.CreateFontIndirect(&LogFont);
	GetDlgItem(IDC_LIST_ASIC_ID)->SetFont(&m_ListFont);

	// set ListCtrl height
	HIMAGELIST hList = ImageList_Create(24, 24, ILC_COLOR8 | ILC_MASK, 8, 1);
	CImageList	ImageList;
	ImageList.Attach(hList);
	m_ListCtrl.SetImageList(&ImageList, LVSIL_SMALL);
	ImageList_Destroy(hList);
}

void ASIC_ID_dlg::ReadAsicID()
{
	CTestModeView * pTestModeView = (CTestModeView*)m_pParent;
	CUsbHidList	* pUsbHidList = (CUsbHidList*)pTestModeView->GetUsdHidList();

	int nCount = 0;

	CString strTemp;

	for (int n = 0; n < MAX_DEVICE_COUNT; n++)
	{		
		CUsbHid * pUsbHid = pUsbHidList->GetUsbHidPtrByDevIndex(n);

		if (pUsbHid == NULL)
			continue;

		pTestModeView->MpTool_PowerControl(pUsbHid, TRUE);

		if (pTestModeView->ReadASICID(pUsbHid) == FALSE)
		{
			TRACE("## ERROR ## %s() ## ReadASICID() fail \n", __FUNCTION__);
		}

		pTestModeView->MpTool_PowerControl(pUsbHid, FALSE);
	}
}

void ASIC_ID_dlg::DisplayAsicID()
{
	CTestModeView * pTestModeView = (CTestModeView*)m_pParent;
	CUsbHidList	* pUsbHidList = (CUsbHidList*)pTestModeView->GetUsdHidList();

	BOOL bFirstItem = FALSE;

	CString strTemp;

	m_ListCtrl.DeleteAllItems();

	int nCount = 0;

	for (int n = 0; n < MAX_DEVICE_COUNT; n++)
	{		
		CUsbHid * pUsbHid = pUsbHidList->GetUsbHidPtrByDevIndex(n);

		int nIndex = m_ListCtrl.InsertItem( nCount, _T("") );

		strTemp.Format(_T("DEVICE %d"), n);
		m_ListCtrl.SetItemText( nCount, 1, strTemp );

		if (pUsbHid)
		{
			m_ListCtrl.SetItemText(nCount, 2, pUsbHid->m_cAsicID_0);
			m_ListCtrl.SetItemText(nCount, 3, pUsbHid->m_cAsicID_1);

			m_ListCtrl.SetItemTextColor(nCount, ZT_COLOR_BLACK);

			if (bFirstItem == FALSE)
			{
				bFirstItem = TRUE;

				m_ListCtrl.SetItemState(nCount, LVIS_FOCUSED | LVIS_SELECTED, LVIS_SELECTED);
				m_ListCtrl.SetSelectionMark(nCount);
				m_ListCtrl.SetFocus();
			}
		}
		else
		{
			m_ListCtrl.SetItemText(nCount, 2, "-");
			m_ListCtrl.SetItemText(nCount, 3, "-");

			m_ListCtrl.SetItemTextColor(nCount, ZT_COLOR_GRAY);

			m_ListCtrl.SetItemDisable(nCount, TRUE);
		}

		nCount++;		
	}
}

LRESULT ASIC_ID_dlg::OnSaveIum(WPARAM wParam, LPARAM lParam)
{
#ifndef _DEBUG
	CInputDlg dlg;
	dlg.m_strTitle.Format("PASSWORD");
	dlg.m_bPassword = TRUE;

	if (dlg.DoModal() != IDOK ||
		dlg.m_strInput.CompareNoCase("ZINITIXSW1231") != 0)
	{
		return 0L;
	}
#endif

	CTestModeView * pTestModeView = (CTestModeView*)m_pParent;
	CUsbHidList	* pUsbHidList = (CUsbHidList*)pTestModeView->GetUsdHidList();

	CString strZtdExeDir = GetCurrentAppDirectory();
	PathRemoveBackslash(strZtdExeDir.GetBuffer());

	for (int n = 0; n < MAX_DEVICE_COUNT; n++)
	{		
		CUsbHid * pUsbHid = pUsbHidList->GetUsbHidPtrByDevIndex(n);

		if (pUsbHid == NULL)
			continue;

		pTestModeView->MpTool_PowerControl(pUsbHid, TRUE);
	
		if (pUsbHid->GetTouchRegReadCmd(ZT_VCMD(VCMD_IUM_READ_DATA), (BYTE*)pUsbHid->m_IumData, g_FwDownloadInfo.icIumSize) == 0)
		{
			CString strFile;
			strFile.Format("%s\\IUM_DEV_%d.bin", strZtdExeDir, n);

			CFile file;
			if ( file.Open(strFile, CFile::modeCreate | CFile::modeWrite) )
			{
				file.Write(pUsbHid->m_IumData, 128);
				file.Close();
			}
		}

		pTestModeView->MpTool_PowerControl(pUsbHid, FALSE);
	}

	return 1L;
}

