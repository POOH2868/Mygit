
// ZDataAnalyzerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "TestModeView.h"
#include "TestModeView_TestInfo.h"
#include "ZDataAnalyzerDlg.h"
#include "util.h"
#include <share.h>				// _SH_DENYWR

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define BUFFER_SIZE 4096

#define	STRING_SAVE_DIRECTORY	"GRAPH MODE"

#define	STRING_CH_START_CODE		"CH___,"
#define	STRING_ROW_START_CODE		"ROW__,"
#define	STRING_COL_START_CODE		"COL__,"

#define	STRING_PREFIX_CH			"CH___"
#define	STRING_PREFIX_SELF_CH_Y		"CHY__"
#define	STRING_PREFIX_SELF_CH_X		"CHX"

// CZDataAnalyzerDlg dialog

CZDataAnalyzerDlg::CZDataAnalyzerDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CZDataAnalyzerDlg::IDD, pParent)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pTestModeView = NULL;

	m_bCheckAll = FALSE;
	m_bIgnoreItemChangedEvent = FALSE;
}

void CZDataAnalyzerDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILES, m_ListCtrlFiles);
	DDX_Control(pDX, IDC_COMBO_DATA_TYPE, m_ComboDataType);
}

BEGIN_MESSAGE_MAP(CZDataAnalyzerDlg, CResizableDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILES, OnDblclkList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FILES, OnItemchangedList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_FILES, OnColumnClick)
	ON_CBN_SELCHANGE(IDC_COMBO_DATA_TYPE, OnCbnSelchangeComboDataType)
	ON_MESSAGE(WM_USER_GRAPH_SELECTED, OnGraphSelected)
	ON_BN_CLICKED(IDC_BUTTON_GRAPH_DRAW, OnButtonGraphDraw)
	ON_BN_CLICKED(IDC_BUTTON_DATA_ANALYSIS_DIR_BROWSE, OnButtonDataAnalysisDirBrowse)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
END_MESSAGE_MAP()


// CZDataAnalyzerDlg message handlers

BOOL CZDataAnalyzerDlg::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//Get program file path
	m_strZtdExeDir = GetCurrentAppDirectory();

	//Create "GRAPH MODE" directory
	if (m_strZtdExeDir.Right(1) != '\\')
		m_strSaveDir.Format("%s\\%s", m_strZtdExeDir, STRING_SAVE_DIRECTORY);
	else
		m_strSaveDir.Format("%s%s", m_strZtdExeDir, STRING_SAVE_DIRECTORY);

	CreateDirectory((LPCTSTR)m_strSaveDir, NULL);

	InitUiControls();

	LinePlot_Init();

	FileList_Initialize();

	FileList_Find(m_strDataAnalysisDir);

	AddAnchor(IDC_LIST_FILES, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(ID_CTRL_LINE_PLOT, TOP_LEFT, BOTTOM_RIGHT);

	AddAnchor(IDC_BUTTON_SAVE, BOTTOM_LEFT);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CZDataAnalyzerDlg::OnDestroy()
{
	FileList_RemoveAllData();

	CDialog::OnDestroy();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CZDataAnalyzerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CResizableDialog::OnPaint();
	}
}

BOOL CZDataAnalyzerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	}
	else if (pMsg->message == WM_KEYUP)
	{
		if (GetDlgItem(IDC_EDIT_PERCENTAGE)->m_hWnd == pMsg->hwnd)
		{
			if (pMsg->wParam == VK_RETURN)
			{
				OnButtonGraphDraw();
			}
		}
		else if (GetDlgItem(IDC_LIST_FILES)->m_hWnd == pMsg->hwnd)
		{
			int nSelectRow = m_ListCtrlFiles.GetNextItem(-1, LVNI_SELECTED);
			if (nSelectRow < 0){
				return CResizableDialog::PreTranslateMessage(pMsg);
			}

			CString strPath = GetSelectedFilePath(nSelectRow);

			if (pMsg->wParam == VK_RETURN)
			{
				if (nSelectRow == 0 || PathIsDirectory(strPath)){
					FileList_Find(m_strCurDir);
				}
				else{
					m_ListCtrlFiles.SetCheck(nSelectRow, !m_ListCtrlFiles.GetCheck(nSelectRow));
				}
			}
			else if (pMsg->wParam == VK_DELETE)
			{
				if (nSelectRow > 0 && PathIsDirectory(strPath) == FALSE){
					m_ListCtrlFiles.SetCheck(nSelectRow, FALSE);
				}
			}
		}
	}

	return CResizableDialog::PreTranslateMessage(pMsg);
}

void CZDataAnalyzerDlg::InitUiControls()
{
	CTestModeView * pTestModeView = m_pTestModeView;

	CComboBox * pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_DATA_TYPE);

	// set data type
	DATA_TYPE dataType;

	// DND
	dataType.m_strTypeName = "DND";
	dataType.m_strLogPrefix = "DND";
	dataType.m_DataType = DATA_TYPE_AVG_MAX_MIN;
	m_DataTypeArray.Add(dataType);
	pComboBox->InsertString(pComboBox->GetCount(), dataType.m_strTypeName);

	dataType.m_strTypeName = "DND_H_GAP";
	dataType.m_strLogPrefix = "DND_H_GAP";
	dataType.m_DataType = DATA_TYPE_INCREASE;
	m_DataTypeArray.Add(dataType);
	pComboBox->InsertString(pComboBox->GetCount(), dataType.m_strTypeName);

	dataType.m_strTypeName = "DND_V_GAP";
	dataType.m_strLogPrefix = "DND_V_GAP";
	dataType.m_DataType = DATA_TYPE_INCREASE;
	m_DataTypeArray.Add(dataType);
	pComboBox->InsertString(pComboBox->GetCount(), dataType.m_strTypeName);

	// HF_DND
	dataType.m_strTypeName = "HF_DND";
	dataType.m_strLogPrefix = "HF_DND";
	dataType.m_DataType = DATA_TYPE_AVG_MAX_MIN;
	m_DataTypeArray.Add(dataType);
	pComboBox->InsertString(pComboBox->GetCount(), dataType.m_strTypeName);

	dataType.m_strTypeName = "HF_DND_H_GAP";
	dataType.m_strLogPrefix = "HF_DND_H_GAP";
	dataType.m_DataType = DATA_TYPE_INCREASE;
	m_DataTypeArray.Add(dataType);
	pComboBox->InsertString(pComboBox->GetCount(), dataType.m_strTypeName);

	dataType.m_strTypeName = "HF_DND_V_GAP";
	dataType.m_strLogPrefix = "HF_DND_V_GAP";
	dataType.m_DataType = DATA_TYPE_INCREASE;
	m_DataTypeArray.Add(dataType);
	pComboBox->InsertString(pComboBox->GetCount(), dataType.m_strTypeName);

	// SELF_DND
	dataType.m_strTypeName = "SELF_DND_RX";
	dataType.m_strLogPrefix = "SELF_DND";
	dataType.m_DataType = DATA_TYPE_AVG_MAX_MIN;
	m_DataTypeArray.Add(dataType);
	pComboBox->InsertString(pComboBox->GetCount(), dataType.m_strTypeName);

	dataType.m_strTypeName = "SELF_DND_TX";
	dataType.m_strLogPrefix = "SELF_DND";
	dataType.m_DataType = DATA_TYPE_AVG_MAX_MIN;
	m_DataTypeArray.Add(dataType);
	pComboBox->InsertString(pComboBox->GetCount(), dataType.m_strTypeName);

	if (pTestModeView->GetUseIFE())
	{
		dataType.m_strTypeName = "SELF_DND_RX_H_GAP";
		dataType.m_strLogPrefix = "SELF_DND_GAP";
		dataType.m_DataType = DATA_TYPE_INCREASE;
		m_DataTypeArray.Add(dataType);
		pComboBox->InsertString(pComboBox->GetCount(), dataType.m_strTypeName);

		dataType.m_strTypeName = "SELF_DND_TX_H_GAP";
		dataType.m_strLogPrefix = "SELF_DND_GAP";
		dataType.m_DataType = DATA_TYPE_INCREASE;
		m_DataTypeArray.Add(dataType);
		pComboBox->InsertString(pComboBox->GetCount(), dataType.m_strTypeName);
	
		// SELF_SATURATION
		dataType.m_strTypeName = "SELF_SATURATION_RX";
		dataType.m_strLogPrefix = "SELF_SATURATION";
		dataType.m_DataType = DATA_TYPE_AVG_MAX_MIN;
		m_DataTypeArray.Add(dataType);
		pComboBox->InsertString(pComboBox->GetCount(), dataType.m_strTypeName);

		dataType.m_strTypeName = "SELF_SATURATION_TX";
		dataType.m_strLogPrefix = "SELF_SATURATION";
		dataType.m_DataType = DATA_TYPE_AVG_MAX_MIN;
		m_DataTypeArray.Add(dataType);
		pComboBox->InsertString(pComboBox->GetCount(), dataType.m_strTypeName);
	}

	m_ComboDataType.SetCurSel(0);
	GetDlgItem(IDC_STATIC_PERCENTAGE)->SetWindowText("PERCENTAGE");

	SetDlgItemInt(IDC_EDIT_PERCENTAGE, 30);
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// File List

void CZDataAnalyzerDlg::FileList_Initialize()
{
	char *list[2] = { "FILE NAME", "DATA EXIST" };

	m_ListCtrlFiles.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

	m_ListCtrlFiles.ModifyStyle(0, LVS_SHOWSELALWAYS);

	CRect rect;
	m_ListCtrlFiles.GetClientRect(rect);
	m_ListCtrlFiles.InsertColumn(0, _T(list[0]), LVCFMT_CENTER, (rect.right - rect.left) - 90 - 20, 0);
	m_ListCtrlFiles.InsertColumn(1, _T(list[1]), LVCFMT_CENTER, 90, 0);

	m_ListCtrlFiles.SetTextColor(RGB(0, 0, 0));
	m_ListCtrlFiles.SetBkColor(RGB(210, 230, 250));
	m_ListCtrlFiles.SetTextBkColor(RGB(210, 230, 250));
}

void CZDataAnalyzerDlg::FileList_Find(CString csDirPath, BOOL bBrowse)
{
	CString strFindPath, strExt;
	CString strCurDirName = "";
	CString strName;
	int nIndex = 0, nSelectedIndex = 0;

	HANDLE hFind;
	WIN32_FIND_DATA fd;	//파일 찾기 구조체
	BOOL bResult = TRUE;

	CUIntArray nArrTime;			//현재시간<=>파일수정시간 간격을 저장하는 배열
	CStringArray strArrFileName;	//파일명을 저장하는 배열

	m_bCheckAll = FALSE;

	m_ListCtrlFiles.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

	int nSelectRow = m_ListCtrlFiles.GetNextItem(-1, LVNI_SELECTED);
	if (bBrowse)
		nSelectRow = -1;

	if (nSelectRow == 0)
	{// [..]
		CString strDir = csDirPath;
		if (strDir.Right(1) != '\\')
			strDir.Append("\\");

		if (PathIsRoot(strDir))
			return;

		strCurDirName = csDirPath;
		PathStripPath(strCurDirName.GetBuffer());

		int nPos = csDirPath.ReverseFind('\\');
		if (nPos == 0)
			csDirPath = '\\';
		else
			csDirPath = csDirPath.Left(nPos);
	}
	else if (nSelectRow > 0)
	{// directory
		if (csDirPath.Right(1) != '\\')
			csDirPath += ("\\" + m_ListCtrlFiles.GetItemText(nSelectRow, 0));
		else
			csDirPath += m_ListCtrlFiles.GetItemText(nSelectRow, 0);
	}

	m_bIgnoreItemChangedEvent = TRUE;

	m_strCurDir.Format("%s", csDirPath);
	GetDlgItem(IDC_EDIT_CURRENT_DIR)->SetWindowText(m_strCurDir);

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_CURRENT_DIR);
	pEdit->LineScroll(pEdit->GetLineCount());
	pEdit->ShowScrollBar(SB_VERT, TRUE);

	FileList_RemoveAllData();

	// add [..]
	m_ListCtrlFiles.InsertItem(0, "[ .. ]");
	m_ListCtrlFiles.SetItemText(0, 1, "-");
	nIndex++;

	if (csDirPath.Right(1) != '\\')
		csDirPath.Append("\\");
	
	strFindPath.Format("%s*.*", csDirPath);
	
	hFind = FindFirstFile( strFindPath, &fd );
	if(hFind == INVALID_HANDLE_VALUE)
		bResult = FALSE;

	CTime currentTime = CTime::GetCurrentTime();
	
	while (bResult)
	{
		strName.Format("%s", fd.cFileName);
				
		if( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
		{
			if (strName.Compare(".") != 0 && 
				strName.Compare("..") != 0 &&
				strName.Compare("FIRMWARE") != 0 &&
				strName.Compare(STRING_SAVE_DIRECTORY) != 0)
			{
				if (strName.Compare(strCurDirName) == 0)
					nSelectedIndex = nIndex;

				//--------------------------------
				// add directory name to list
				FileList_Add(strName, FALSE);
				nIndex++;
			}
		}
		else
		{
			strExt = PathFindExtension(strName);
			strExt = strExt.Right(strExt.GetLength()-1);

			if (strExt.Compare("csv") == 0)
			{
				CTime time( fd.ftLastWriteTime );
				CTimeSpan ts = currentTime - time;

				nArrTime.Add( ts.GetTotalSeconds() );
				strArrFileName.Add(strName);
			}
		}

		//다음 파일을 찾는다.
		bResult = FindNextFile( hFind, &fd );
	}

 	// File Sorting - 오래된 파일부터 표시
	unsigned int nTemp;
	CString strTemp;
	int filecnt = strArrFileName.GetSize();

	for(int i = 0; i < filecnt; i++ )
	{
		for(int j = i; j < filecnt; j++)
		{
			if( nArrTime[i] < nArrTime[j] )
			{
				nTemp = nArrTime[i];
				strTemp = strArrFileName[i];
				
				nArrTime[i] = nArrTime[j];
				strArrFileName[i] = strArrFileName[j];
				nArrTime[j] = nTemp;
				strArrFileName[j] = strTemp;
			}
		}
	}

	//--------------------------------
	// Add file name to list
	for(int i = 0; i < filecnt; i++ )
	{
		FileList_Add(strArrFileName[i], TRUE);
	}

	m_ListCtrlFiles.SetItemState(nSelectedIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	FileList_UpdateDataExist();

	m_bIgnoreItemChangedEvent = FALSE;

	strArrFileName.RemoveAll();
	nArrTime.RemoveAll();
}

// bFile - TRUE : File, FALSE : Direcotry
void CZDataAnalyzerDlg::FileList_Add(CString strFile, BOOL bFile)
{
	if (strFile.IsEmpty()) 
		return;

	if (bFile && FileList_IsValidFile(strFile) == FALSE)
		return;

	LVFINDINFO info;
	info.flags = LVFI_PARTIAL | LVFI_STRING;
	info.psz = strFile;

	// check same file name
	if (m_ListCtrlFiles.FindItem(&info) != -1)
		return;
	
	int nRow = m_ListCtrlFiles.GetItemCount();
	m_ListCtrlFiles.InsertItem(nRow, strFile);
	m_ListCtrlFiles.SetItemText(nRow, 1, "-");

	if (bFile)
	{
		FileList_ReadData(nRow);
	}
}

void CZDataAnalyzerDlg::FileList_Delete()
{
	int iSelectedCount = m_ListCtrlFiles.GetSelectedCount();

	if (iSelectedCount == 0)
		return;

	m_ListCtrlFiles.GetItemText(m_ListCtrlFiles.GetSelectionMark(), 0);

	for (int iLoop = iSelectedCount - 1; iLoop > -1; iLoop--){
		m_ListCtrlFiles.DeleteItem(m_ListCtrlFiles.GetSelectionMark());
	}
}

BOOL CZDataAnalyzerDlg::FileList_IsValidFile(CString strFile)
{	
	char buffer[BUFFER_SIZE + 1];
	FILE *stream;
	char *ps = NULL;
	char * endptr;
	BOOL bFind = FALSE;

	// Ignore "LOGGING" file
	if (strFile.Find("LOGGING_") == 0)
		return FALSE;

	//--------------------------------
	// search ZTD version

	CString strParsingPrefix = hvFillBlankSpaceInString_0(0, "ZTD_VERSION");
	if (strParsingPrefix.IsEmpty())
		return FALSE;

	CString strFilePath;
	strFilePath.Format("%s\\%s", m_strCurDir, strFile);

	errno_t err = fopen_s(&stream, strFilePath.GetBuffer(), "rt");

	if (stream == NULL)
	{
		TRACE(_T("\r\n** %s :: Error = (%d)\r\n"), strFilePath.GetBuffer(), err);
		return FALSE;
	}

	while (fgets(buffer, BUFFER_SIZE, stream) != NULL)
	{
		ps = strstr(buffer, (const char*)strParsingPrefix.GetBuffer());

		if (ps == NULL)
			continue;

		bFind = TRUE;
		break;
	}

	fclose(stream);

	if (bFind == FALSE)
		return FALSE;

	// Parsing ZTD version
	char * pNextContext = NULL;
	char seps[] = ",\t\t";
	int nVersion = 0;

	ps = strtok_s(buffer, seps, &pNextContext);

	while (ps)
	{
		ps = strtok_s(NULL, seps, &pNextContext);

		if (ps == NULL)
			break;

		nVersion = strtol(ps+1, &endptr, 10);
	}

	if (nVersion < 2)
		return FALSE;

	return TRUE;
}

void CZDataAnalyzerDlg::FileList_ReadData(int nRow)
{
	BOOL bIsExistData = FALSE;
	CString strFileName = m_ListCtrlFiles.GetItemText(nRow, 0);

	CGraphData * pGraphData = (CGraphData*)m_ListCtrlFiles.GetItemData(nRow);

	if (pGraphData == NULL)
	{
		pGraphData = new CGraphData;
		if (pGraphData == NULL)
			return;

		m_ListCtrlFiles.SetItemData(nRow, (DWORD_PTR)pGraphData);
	}

	CString strFilePath;
	strFilePath.Format("%s\\%s", m_strCurDir, m_ListCtrlFiles.GetItemText(nRow, 0));

	int nDataTypeCount = m_DataTypeArray.GetCount();

	//--------------------------------
	// get line number
#if 1	// D_USE_LINE_NUMBER_IN_LOG
	if (ReadLineNumber(strFilePath, pGraphData) == FALSE)
		return;
#else
	// Get Line Number X, Y
	for (int i = 0; i < nDataTypeCount; i++)
	{
		bIsExistData = ReadData_GetLineNum(i, strFilePath, pGraphData);
		if (bIsExistData)
			break;
	}
#endif

	//--------------------------------'
	// Read Data
	for (int i = 0; i < nDataTypeCount; i++)
	{
		ReadFileParsing(i, strFilePath, pGraphData);
	}
}

void CZDataAnalyzerDlg::FileList_RemoveAllData()
{
	CGraphData * pGraphData = NULL;

	long lListCount = m_ListCtrlFiles.GetItemCount();
	if (lListCount <= 0) 	return;

	for (long nIndex = lListCount - 1; nIndex > -1; nIndex--)
	{
		pGraphData = (CGraphData*)m_ListCtrlFiles.GetItemData(nIndex);

		if (pGraphData != NULL)
		{
			delete pGraphData;
		}
	}

	m_ListCtrlFiles.DeleteAllItems();
}

void CZDataAnalyzerDlg::FileList_UpdateDataExist()
{
	CGraphData * pGraphData = NULL;

	int nDataType = m_ComboDataType.GetCurSel();

	void * pDataExist = NULL;

	long lListCount = m_ListCtrlFiles.GetItemCount();
	if (lListCount <= 0) 	return;

	m_LinePlot.Clear();

	for (long nIndex = 0; nIndex < lListCount; nIndex++)
	{
		if (nIndex == 0 || PathIsDirectory(GetSelectedFilePath(nIndex))){
			m_ListCtrlFiles.SetItemText(nIndex, 1, "");
			continue;
		}

		pGraphData = (CGraphData*)m_ListCtrlFiles.GetItemData(nIndex);

		if (pGraphData && LinePlot_GetDataPtr(pGraphData))
		{
			m_ListCtrlFiles.SetItemText(nIndex, 1, "OK");
		}
		else
		{
			m_ListCtrlFiles.SetCheck(nIndex, FALSE);
			m_ListCtrlFiles.SetItemText(nIndex, 1, "-");
		}
	}
}

void CZDataAnalyzerDlg::FileList_SetFocus(int nIndex)
{
	if (nIndex < 0)
		return;

	// set focus
	m_ListCtrlFiles.SetItemState(nIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	m_ListCtrlFiles.SetFocus();
	m_ListCtrlFiles.EnsureVisible(nIndex, TRUE);

	EnableSaveButton();
}

CString CZDataAnalyzerDlg::GetSelectedFilePath(int nRow)
{
	CString strPath;

	if (m_strCurDir.Right(1) != '\\')
		strPath.Format("%s\\%s", m_strCurDir, m_ListCtrlFiles.GetItemText(nRow, 0));
	else
		strPath.Format("%s%s", m_strCurDir, m_ListCtrlFiles.GetItemText(nRow, 0));

	return strPath;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Data parsing

HV_GRAPH_DATA * CZDataAnalyzerDlg::GetGraphData(CGraphData * pGraphData, int nDataType)
{
	HV_GRAPH_DATA * pHvGraphData = NULL;

	switch (nDataType)
	{
	case DATA_TYPE_DND:						pHvGraphData = &pGraphData->m_DataDnd;					break;
	case DATA_TYPE_DND_H_GAP:				pHvGraphData = &pGraphData->m_DataDndHGap;				break;
	case DATA_TYPE_DND_V_GAP:				pHvGraphData = &pGraphData->m_DataDndVGap;				break;

	case DATA_TYPE_HF_DND:					pHvGraphData = &pGraphData->m_DataHfDnd;				break;
	case DATA_TYPE_HF_DND_H_GAP:			pHvGraphData = &pGraphData->m_DataHfDndHGap;			break;
	case DATA_TYPE_HF_DND_V_GAP:			pHvGraphData = &pGraphData->m_DataHfDndVGap;			break;

	case DATA_TYPE_SELF_DND_RX:				pHvGraphData = &pGraphData->m_DataSelfDndRx;			break;
	case DATA_TYPE_SELF_DND_TX:				pHvGraphData = &pGraphData->m_DataSelfDndTx;			break;
	case DATA_TYPE_SELF_DND_RX_H_GAP:		pHvGraphData = &pGraphData->m_DataSelfDndRxHGap;		break;
	case DATA_TYPE_SELF_DND_TX_H_GAP:		pHvGraphData = &pGraphData->m_DataSelfDndTxHGap;		break;

	case DATA_TYPE_SELF_SATURATION_RX:		pHvGraphData = &pGraphData->m_DataSelfSaturationRx;		break;
	case DATA_TYPE_SELF_SATURATION_TX:		pHvGraphData = &pGraphData->m_DataSelfSaturationTx;		break;

	default:																						break;
	}

	return pHvGraphData;
}

#if 1	// D_USE_LINE_NUMBER_IN_LOG
BOOL CZDataAnalyzerDlg::ReadLineNumber(CString strFilePath, CGraphData * pGraphData)
{
	char buffer[BUFFER_SIZE + 1];
	FILE *stream;
	char *ps = NULL;
	char * endptr;
	BOOL bFind = FALSE;

	//--------------------------------
	// search line number

	CString strParsingPrefix = hvFillBlankSpaceInString_0(0, "LINE_NUMBER");
	if (strParsingPrefix.IsEmpty())
		return FALSE;

	errno_t err = fopen_s(&stream, strFilePath.GetBuffer(), "rt");

	if (stream == NULL)
	{
		TRACE(_T("\r\n** %s :: Error = (%d)\r\n"), strFilePath.GetBuffer(), err);
		return FALSE;
	}

	while (fgets(buffer, BUFFER_SIZE, stream) != NULL)
	{
		ps = strstr(buffer, (const char*)strParsingPrefix.GetBuffer());

		if (ps == NULL)
			continue;

		bFind = TRUE;
		break;
	}

	fclose(stream);

	if (bFind == FALSE)
		return FALSE;

	//--------------------------------
	// read line number

	BOOL bIFE_Mode = FALSE;
	bFind = FALSE;

	// Parsing line number
	BOOL bFindStartChar = FALSE;
	int nFindNumberCount = 0;
	char * pNextContext = NULL;
	char seps[] = ",\t\t";

	ps = strtok_s(buffer, seps, &pNextContext);

	while (ps)
	{
		ps = strtok_s(NULL, seps, &pNextContext);

		if (ps == NULL)
			break;

		if (bFindStartChar == FALSE)
		{
			if (StrStrI(ps, "DEV_") != NULL)
			{
				bFindStartChar = TRUE;
			}
		}
		else
		{
			if (nFindNumberCount == 0) {
				pGraphData->m_nLineNumX = strtol(ps, &endptr, 10);
			}
			else if (nFindNumberCount == 1)	{
				pGraphData->m_nLineNumY = strtol(ps, &endptr, 10);
			}
			else if (nFindNumberCount == 2)	{
				pGraphData->m_nSelfRowCount = strtol(ps, &endptr, 10);
				bIFE_Mode = TRUE;
				bFind = TRUE;
			}
			else if (nFindNumberCount == 3)	{
				pGraphData->m_nMaxLineNumX = strtol(ps, &endptr, 10);
				bIFE_Mode = FALSE;
				bFind = FALSE;
			}
			else if (nFindNumberCount == 4)	{
				pGraphData->m_nMaxLineNumY = strtol(ps, &endptr, 10);
				bIFE_Mode = FALSE;
				bFind = TRUE;
			}

			nFindNumberCount++;
		}
	}

	if (bFind && bIFE_Mode)
	{
		pGraphData->m_bUseIFE_mode = TRUE;
	}

	return bFind;
}
#else
BOOL CZDataAnalyzerDlg::ReadData_GetLineNum(int nDataType, CString strFilePath, CGraphData * pGraphData)
{
	char buffer[BUFFER_SIZE + 1];
	FILE *stream;
	char *ps = NULL;

	DATA_TYPE dataType;
	dataType = m_DataTypeArray.GetAt(nDataType);

	CString strParsingPrefix = hvFillBlankSpaceInString_0(0, dataType.m_strLogPrefix);
	if (strParsingPrefix.IsEmpty())
		return FALSE;

	stream = _fsopen(strFilePath.GetBuffer(), "rt", _SH_DENYNO);
	if (stream == NULL)
	{
		TRACE(_T("\r\n** %s :: Error\r\n"), strFilePath.GetBuffer());
		return FALSE;
	}

	HV_GRAPH_DATA * pHvGraphData = GetGraphData(pGraphData, nDataType);
	if (pHvGraphData == NULL)
		return FALSE;

	CString strResult = hvFillBlankSpaceInString_0(0, "RESULT");

	pGraphData->m_nLineNumX = 0;
	pGraphData->m_nLineNumY = 0;

	while (fgets(buffer, BUFFER_SIZE, stream) != NULL)
	{
		if (strstr(buffer, strResult) != NULL)
			break;

		ps = strstr(buffer, (const char*)strParsingPrefix.GetBuffer());

		if (ps == NULL || ps != buffer)
			continue;

		if (nDataType == DATA_TYPE_SELF_DND_TX || nDataType == DATA_TYPE_SELF_DND_RX || 
			nDataType == DATA_TYPE_SELF_DND_TX_H_GAP || nDataType == DATA_TYPE_SELF_DND_RX_H_GAP || 
			nDataType == DATA_TYPE_SELF_SATURATION_TX || nDataType == DATA_TYPE_SELF_SATURATION_RX)
		{
			// Get LineNumY
			if (fgets(buffer, BUFFER_SIZE, stream) == NULL)	break;

			if (fgets(buffer, BUFFER_SIZE, stream) != NULL)
			{
				if (strstr(buffer, (const char*)STRING_COL_START_CODE) == NULL)
					break;
				
				pGraphData->m_nLineNumY = ReadData_GetLineNumY(nDataType, buffer, STRING_PREFIX_SELF_CH_Y);
			}

			// Get LineNumX
			if (fgets(buffer, BUFFER_SIZE, stream) == NULL)	break;
			if (fgets(buffer, BUFFER_SIZE, stream) == NULL)	break;
			
			if (fgets(buffer, BUFFER_SIZE, stream) != NULL)
			{
				if (strstr(buffer, (const char*)STRING_ROW_START_CODE) == NULL)
					break;
				
				pGraphData->m_nLineNumX = ReadData_GetLineNumY(nDataType, buffer, STRING_PREFIX_SELF_CH_X);
			}
		}
		else
		{
			// Get LineNumY		
			if (fgets(buffer, BUFFER_SIZE, stream) != NULL)
			{
				if (strstr(buffer, (const char*)STRING_CH_START_CODE) == NULL)
					break;
				
				pGraphData->m_nLineNumY = ReadData_GetLineNumY(nDataType, buffer, STRING_PREFIX_CH);
			}

			// Get LineNumX
			while (fgets(buffer, BUFFER_SIZE, stream) != NULL)
			{
				// check whether the end of the data has been reached
				if (strcmp(buffer, (const char*)"\n") == 0 || strcmp(buffer, (const char*)"\r\n") == 0)
					break;
				
				pGraphData->m_nLineNumX++;
			}
		}

		break;
	}

	fclose(stream);

	if (pGraphData->m_nLineNumX == 0 || pGraphData->m_nLineNumY == 0)
		return FALSE;

	if (nDataType == DATA_TYPE_DND_H_GAP || nDataType == DATA_TYPE_HF_DND_H_GAP)
	{
		pGraphData->m_nLineNumY++;
	}
	else if (nDataType == DATA_TYPE_DND_V_GAP || nDataType == DATA_TYPE_HF_DND_V_GAP)
	{
		pGraphData->m_nLineNumX++;
	}
	else if (nDataType == DATA_TYPE_SELF_DND_TX_H_GAP || nDataType == DATA_TYPE_SELF_DND_RX_H_GAP)
	{
		pGraphData->m_nLineNumY++;
		pGraphData->m_nLineNumX++;
	}

	return TRUE;
}

int CZDataAnalyzerDlg::ReadData_GetLineNumY(int nDataType, char * pBuffer, char * pFindChar)
{
	BOOL bFindStartChar = FALSE;
	int nLineNumY = 0;
	char * pNextContext = NULL;
	char *ps = NULL;
	char seps[] = ",\t\t";

	ps = strtok_s(pBuffer, seps, &pNextContext);

	// Check LineNumY
	while (ps)
	{
		ps = strtok_s(NULL, seps, &pNextContext);

		if (ps == NULL)
			break;

		if (bFindStartChar)
		{
			nLineNumY++;
		}
		else if (strcmp(ps, pFindChar) == 0)
		{
			bFindStartChar = TRUE;
			continue;
		}
	}

	return nLineNumY;
}
#endif

BOOL CZDataAnalyzerDlg::ReadFileParsing(int nDataType, CString strFilePath, CGraphData * pGraphData)
{
	char buffer[BUFFER_SIZE + 1];
	FILE *stream;
	char *ps = NULL;
	CString strFind;
	int nLineNumY, nLineNumX;

	DATA_TYPE dataType;
	dataType = m_DataTypeArray.GetAt(nDataType);

	CString strParsingPrefix = hvFillBlankSpaceInString_0(0, dataType.m_strLogPrefix);
	if (strParsingPrefix.IsEmpty())
		return FALSE;

	stream = _fsopen(strFilePath.GetBuffer(), "rt", _SH_DENYNO);
	if (stream == NULL)
	{
		TRACE(_T("\r\n** %s :: Error = (%d)\r\n"), strFilePath.GetBuffer());
		return FALSE;
	}

	//--------------------------------
	// set line number
	nLineNumY = pGraphData->m_nLineNumY;	// COL
	nLineNumX = pGraphData->m_nLineNumX;	// ROW

	switch(nDataType)
	{
	case DATA_TYPE_DND_H_GAP:
	case DATA_TYPE_HF_DND_H_GAP:
		nLineNumY = (pGraphData->m_nLineNumY - 1);
	break;

	case DATA_TYPE_DND_V_GAP:
	case DATA_TYPE_HF_DND_V_GAP:
		nLineNumX = (pGraphData->m_nLineNumX - 1);
	break;

	case DATA_TYPE_SELF_DND_RX:
	case DATA_TYPE_SELF_DND_RX_H_GAP:
	case DATA_TYPE_SELF_SATURATION_RX:
	{
		nLineNumX = 1;

		if (pGraphData->m_bUseIFE_mode == FALSE)
			nLineNumY = pGraphData->m_nMaxLineNumY;

		if (nDataType == DATA_TYPE_SELF_DND_RX_H_GAP)
			nLineNumY--;
	}
	break;

	case DATA_TYPE_SELF_DND_TX:
	case DATA_TYPE_SELF_DND_TX_H_GAP:
	case DATA_TYPE_SELF_SATURATION_TX:
	{
		nLineNumX = pGraphData->m_nSelfRowCount;

		if (pGraphData->m_bUseIFE_mode == FALSE)
			nLineNumY = pGraphData->m_nMaxLineNumY;

		if (nDataType == DATA_TYPE_SELF_DND_TX_H_GAP)
			nLineNumY--;
	}
	break;
	}

	//--------------------------------
	// search data area
	CString strResult = hvFillBlankSpaceInString_0(0, "RESULT");
	
	while (fgets(buffer, BUFFER_SIZE, stream) != NULL)
	{
		if (strstr(buffer, strResult) != NULL)
			break;
		
		ps = strstr(buffer, (const char*)strParsingPrefix.GetBuffer());

		if (ps == NULL || ps != buffer)
			continue;

		//--------------------------------
		// allocate graph data

		HV_GRAPH_DATA * pHvGraphData = GetGraphData(pGraphData, nDataType);
		if (pHvGraphData == NULL)	break;

		pHvGraphData->nCount = nLineNumX * nLineNumY;
		
		FLOATPOINT * pDataPtr = new FLOATPOINT[pHvGraphData->nCount];
		if (pDataPtr == NULL)	break;

		pHvGraphData->pData = pDataPtr;

		//--------------------------------
		// read data

		if (nDataType == DATA_TYPE_SELF_DND_RX || nDataType == DATA_TYPE_SELF_DND_TX || 
			nDataType == DATA_TYPE_SELF_DND_RX_H_GAP || nDataType == DATA_TYPE_SELF_DND_TX_H_GAP || 
			nDataType == DATA_TYPE_SELF_SATURATION_RX || nDataType == DATA_TYPE_SELF_SATURATION_TX )
		{
			if (fgets(buffer, BUFFER_SIZE, stream) == NULL)								break;	// CHY__
			if (strstr(buffer, (const char*)STRING_PREFIX_SELF_CH_Y) == NULL)			break;

			if (nDataType == DATA_TYPE_SELF_DND_RX || 
				nDataType == DATA_TYPE_SELF_DND_RX_H_GAP ||
				nDataType == DATA_TYPE_SELF_SATURATION_RX )
			{
				ReadDataParsing(nDataType, buffer, pDataPtr, 0, STRING_PREFIX_SELF_CH_Y);
			}
			else
			{
				int nIndex = 0;

				if (fgets(buffer, BUFFER_SIZE, stream) == NULL)							break;	// \n
				if (fgets(buffer, BUFFER_SIZE, stream) == NULL)							break;	// CHX
				if (strstr(buffer, (const char*)STRING_PREFIX_SELF_CH_X) == NULL)		break;
				
				ReadDataParsing(nDataType, buffer, pDataPtr, nIndex, STRING_PREFIX_SELF_CH_X);

				do
				{
					if (fgets(buffer, BUFFER_SIZE, stream) == NULL)						break;	// _____,
					if (strstr(buffer, (const char*)STRING_PREFIX_SELF_CH_X) == NULL)	break;

					nIndex += nLineNumY;
					ReadDataParsing(nDataType, buffer, pDataPtr, nIndex, STRING_PREFIX_SELF_CH_X);
				}while(1);
			}
		}
		else
		{
			if (fgets(buffer, BUFFER_SIZE, stream) == NULL)								break;	// CH
			if (strstr(buffer, (const char*)STRING_CH_START_CODE) == NULL)				break;
			
			for(int i=0; i<pGraphData->m_nLineNumX; i++)
			{
				if (fgets(buffer, BUFFER_SIZE, stream) == NULL)							break;

				if (nDataType == DATA_TYPE_DND_V_GAP || nDataType == DATA_TYPE_HF_DND_V_GAP)
					strFind.Format("X%02d%02d", i, i+1);
				else
					strFind.Format("X%04d", i);
				
				ReadDataParsing(nDataType, buffer, pDataPtr, nLineNumY * i, strFind.GetBuffer());
			}
		}

		//--------------------------------
		// get MAX/MIN value
		ReadData_GetMinMax(nDataType, pGraphData);

		break;
	}

	fclose(stream);
			
	return TRUE;
}

void CZDataAnalyzerDlg::ReadDataParsing(int nDataType, char * pBuffer, FLOATPOINT * pDataPtr, int nIndex, char * pFindChar)
{
	BOOL bFindStartChar = FALSE;
	char * pNextContext = NULL;
	char *ps = NULL;
	char seps[] = ",\t\t";
	
	ps = strtok_s(pBuffer, seps, &pNextContext);

	// Check LineNumY
	while (ps)
	{
		ps = strtok_s(NULL, seps, &pNextContext);

		if (ps == NULL)
			break;

		if (bFindStartChar)
		{
			pDataPtr[nIndex].x = (float)nIndex;
			pDataPtr[nIndex].y = (float)atoi(ps);

			nIndex++;
		}
		else if (strncmp(ps, pFindChar, strlen(pFindChar)) == 0)
		{
			bFindStartChar = TRUE;
			continue;
		}
	}
}

void CZDataAnalyzerDlg::ReadData_GetMinMax(int nDataType, CGraphData * pGraphData)
{
	HV_GRAPH_DATA * pHvGraphData = GetGraphData(pGraphData, nDataType);
	if (pHvGraphData == NULL)
		return;

	FLOATPOINT * pDataPtr = pHvGraphData->pData;

	pHvGraphData->nMaxY = 0;
	pHvGraphData->nMinY = 999999;
			
	for (int i=0; i<pHvGraphData->nCount; i++)
	{
		if (pHvGraphData->nMaxY < pDataPtr[i].y)
			pHvGraphData->nMaxY = pDataPtr[i].y;
		if (pHvGraphData->nMinY > pDataPtr[i].y)
			pHvGraphData->nMinY = pDataPtr[i].y;
	}
}

void CZDataAnalyzerDlg::OnButtonDataAnalysisDirBrowse()
{
	CString strRet;
	TCHAR szPath[_MAX_PATH];
	CString csTitle = _T("Select the directory to data analysis");

	BROWSEINFO broweInfo;
	memset(&broweInfo, 0x00, sizeof(broweInfo));

	broweInfo.hwndOwner = m_hWnd;
	broweInfo.pidlRoot = NULL;
	broweInfo.lpszTitle = csTitle.GetBuffer(csTitle.GetLength());
	broweInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	broweInfo.lpfn = NULL;

	csTitle.ReleaseBuffer();

	LPITEMIDLIST lpBrowse = SHBrowseForFolder(&broweInfo);

	if (lpBrowse != NULL)
	{
		if (SHGetPathFromIDList(lpBrowse, szPath))
		{
			SetDataAnalysisDir(szPath);

			FileList_Find(m_strDataAnalysisDir, TRUE);
		}
	}
}

void CZDataAnalyzerDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)pNMHDR;

	int nSelectRow = lpnmitem->iItem;

	CString strPath = GetSelectedFilePath(nSelectRow);

	if (nSelectRow == 0 || PathIsDirectory(strPath))
	{
		FileList_Find(m_strCurDir);
	}

	*pResult = 0;
}

void CZDataAnalyzerDlg::OnCbnSelchangeComboDataType()
{
	int nDataType = m_ComboDataType.GetCurSel();
	
	DATA_TYPE dataType;
	dataType = m_DataTypeArray.GetAt(nDataType);

	if (dataType.m_DataType == DATA_TYPE_INCREASE)
		GetDlgItem(IDC_STATIC_PERCENTAGE)->SetWindowText("INCREASE"); 
	else
		GetDlgItem(IDC_STATIC_PERCENTAGE)->SetWindowText("PERCENTAGE");

	m_bIgnoreItemChangedEvent = TRUE;

	FileList_UpdateDataExist();

	OnButtonGraphDraw();

	m_bIgnoreItemChangedEvent = FALSE;

	EnableSaveButton();
}

void CZDataAnalyzerDlg::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// Did the user click on a valid column heading? 
	if ((pNMListView->hdr.code == LVN_COLUMNCLICK) && (pNMListView->iItem == -1) && (pNMListView->iSubItem == 0))
	{
		long lListCount = m_ListCtrlFiles.GetItemCount();
		if (lListCount <= 0) 	return;

		m_bCheckAll = !m_bCheckAll;

		m_bIgnoreItemChangedEvent = TRUE;

		CGraphData * pGraphData = NULL;

		for (long nIndex = 1; nIndex < lListCount; nIndex++)	// for [..] dir
		{
			pGraphData = (CGraphData*)m_ListCtrlFiles.GetItemData(nIndex);
			if (PathIsDirectory(GetSelectedFilePath(nIndex)) || pGraphData == NULL || LinePlot_GetDataCount(pGraphData) == 0)
			{
				m_ListCtrlFiles.SetCheck(nIndex, FALSE);
				continue;
			}

			m_ListCtrlFiles.SetCheck(nIndex, m_bCheckAll ? TRUE : FALSE);
		}

		OnButtonGraphDraw();

		m_bIgnoreItemChangedEvent = FALSE;
	}
}

void CZDataAnalyzerDlg::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int item = pNMListView->iItem; // idx of item that changed

	if (!(pNMListView->uChanged & LVIF_STATE)) // item state has been changed
		return;

	if (m_bIgnoreItemChangedEvent)
		return;

	if (item == 0)
	{
		m_ListCtrlFiles.SetCheck(item, FALSE);
		return;
	}

	BOOL bChangedSelect = FALSE;
	BOOL bChangedCheck = FALSE;

	BOOL wasSelected = (pNMListView->uOldState & LVIS_SELECTED) != 0;
	BOOL isSelected = (pNMListView->uNewState & LVIS_SELECTED) != 0;

	BOOL wasChecked = (pNMListView->uOldState & 0x3000) != 0x1000;
	BOOL isChecked = (pNMListView->uNewState & 0x3000) != 0x1000;

	if ((!wasSelected && isSelected))
		bChangedSelect = TRUE;
	if (wasChecked != isChecked)
		bChangedCheck = TRUE;

	if (bChangedSelect == FALSE && bChangedCheck == FALSE)
		return;

	if (bChangedCheck)
	{
		if (item == 0 || PathIsDirectory(GetSelectedFilePath(item)))
		{
			m_ListCtrlFiles.SetCheck(item, FALSE);
			return;
		}

		CGraphData * pGraphData = (CGraphData*)m_ListCtrlFiles.GetItemData(item);
		if (pGraphData == NULL || LinePlot_GetDataCount(pGraphData) == 0)
		{
			m_ListCtrlFiles.SetCheck(item, FALSE);
			return;
		}

		OnButtonGraphDraw();

		if (isChecked)
			FileList_SetFocus(item);
		return;
	}

	if (m_ListCtrlFiles.GetCheck(item) == FALSE)
	{
		EnableSaveButton();
		return;
	}

	CGraphData * pGraphData = (CGraphData*)m_ListCtrlFiles.GetItemData(item);
	if (pGraphData == NULL)
		return;

	if (pGraphData->m_GraphIndex == -1)
	{
		TRACE("### ERROR ### %s %d \n", __FUNCTION__, __LINE__);
		return;
	}

	m_LinePlot.SetSelected(pGraphData->m_GraphIndex);
	m_LinePlot.Invalidate();
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Draw graph

void CZDataAnalyzerDlg::OnButtonGraphDraw()
{
	int nDataType = m_ComboDataType.GetCurSel();

	CGraphData * pGraphData = NULL;
	FLOATPOINT * pRealDataPtr = NULL;
	int nDataCount = 0;
	int nMaxDataY = 0;
	int nMinDataY = 999999;

	float MaxPos_x = 0;
	float MaxPos_Y = 0;
	float MinPos_Y = 0;

	long lListCount = m_ListCtrlFiles.GetItemCount();
	if (lListCount <= 0) 	return;

	SetCursor(LoadCursor(0, IDC_WAIT));

	m_LinePlot.Clear();

	// add graph data
	for (long nIndex = 1; nIndex < lListCount; nIndex++)	// for [..] dir
	{
		if (m_ListCtrlFiles.GetCheck(nIndex) == FALSE)
			continue;

		pGraphData = (CGraphData*)m_ListCtrlFiles.GetItemData(nIndex);
		if (pGraphData == NULL)
			continue;

		pGraphData->m_GraphIndex = -1;

		pRealDataPtr = LinePlot_GetDataPtr(pGraphData);
		nDataCount = LinePlot_GetDataCount(pGraphData);

		if (pRealDataPtr == NULL || nDataCount == 0)
			continue;

		LinePlot_GetDataY_MinMax(pGraphData, &nMinDataY, &nMaxDataY);

		//TRACE(_T("\r\n## Graph Data Count [%d]\r\n"), nDataCount);
		if (nDataCount - 1 > MaxPos_x)
			MaxPos_x = (float)(nDataCount - 1);
		if (nMaxDataY > MaxPos_Y)
			MaxPos_Y = (float)nMaxDataY;
		if (nMinDataY < MinPos_Y)
			MinPos_Y = (float)nMinDataY;
		
		m_LinePlot.Add("", LINE_COLOR_NORMAL, CLinePlot::LpLine, pRealDataPtr, nDataCount, nIndex);

		pGraphData->m_GraphIndex = m_LinePlot.Count() - 1;
	}

	if (m_LinePlot.Count() <= 0){
		EnableSaveButton();
		SetCursor(LoadCursor(0, IDC_ARROW));
		return;
	}

	m_LinePlot.SetXMax(MaxPos_x);
	m_LinePlot.SetYMax(MaxPos_Y);
	m_LinePlot.SetYMin(MinPos_Y);

	DATA_TYPE dataType;
	dataType = m_DataTypeArray.GetAt(nDataType);

	m_LinePlot.AddAverage("", CLinePlot::LpLine, MaxPos_x, MinPos_Y, MaxPos_Y, GetDlgItemInt(IDC_EDIT_PERCENTAGE), dataType.m_DataType);
	
	m_LinePlot.Refresh();
	
	FileList_SetFocus(m_LinePlot.GetSelectedListIndex());

	SetCursor(LoadCursor(0, IDC_ARROW));
}

void CZDataAnalyzerDlg::LinePlot_Init(void)
{
	//  create the line plot control.
	m_LinePlot.Create(CRect(0, 0, 1, 1), WS_CHILD | WS_VISIBLE, this, ID_CTRL_LINE_PLOT);

	CRect rect;
	((CStatic*)GetDlgItem(IDC_STATIC_GRAPH_POSITION))->GetWindowRect(&rect);
	ScreenToClient(&rect);

	if (m_LinePlot.m_hWnd != NULL)
	{
		m_LinePlot.MoveWindow(&rect);
	}
}

FLOATPOINT * CZDataAnalyzerDlg::LinePlot_GetDataPtr(CGraphData * pGraphData)
{
	HV_GRAPH_DATA * pHvGraphData = GetGraphData(pGraphData, m_ComboDataType.GetCurSel());

	if (pHvGraphData)
	{
		return pHvGraphData->pData;
	}

	return NULL;
}

int CZDataAnalyzerDlg::LinePlot_GetDataCount(CGraphData * pGraphData)
{
	HV_GRAPH_DATA * pHvGraphData = GetGraphData(pGraphData, m_ComboDataType.GetCurSel());

	if (pHvGraphData)
	{
		return pHvGraphData->nCount;
	}

	return 0;
}

void CZDataAnalyzerDlg::LinePlot_GetDataY_MinMax(CGraphData * pGraphData, int *nMinY, int *nMaxY)
{
	HV_GRAPH_DATA * pHvGraphData = GetGraphData(pGraphData, m_ComboDataType.GetCurSel());

	*nMaxY = 0;
	*nMinY = 0;

	if (pHvGraphData)
	{
		*nMaxY = pHvGraphData->nMaxY;
		*nMinY = pHvGraphData->nMinY;
	}
}

LRESULT CZDataAnalyzerDlg::OnGraphSelected(WPARAM wParam, LPARAM lParam)
{
	int nSelected = (int)wParam;

	long lListCount = m_ListCtrlFiles.GetItemCount();
	if (lListCount <= 0) 	return 0L;
	if (nSelected >= lListCount) return 0L;

	FileList_SetFocus(nSelected);
	
	return 1L;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Save spec

void CZDataAnalyzerDlg::EnableSaveButton()
{
	int nDataType = m_ComboDataType.GetCurSel();

#if 0
	if (nDataType == DATA_TYPE_HW_CALIBRATION )
	{
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	}
	else
#endif
	{
		int nIndex = m_LinePlot.GetSelectedListIndex();

		if (nIndex < 0)
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
		else if ((CGraphData*)m_ListCtrlFiles.GetItemData(nIndex) != NULL)
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		else
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	}
}

void CZDataAnalyzerDlg::SaveSpecData(CString strFileName, int pSpecData[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM])
{
	FILE *Fi = NULL;
	int x, y;
	int lineLength = 0;
	CString strData;
	CString strFilePath;

	strFilePath.Format("%s\\%s", m_strSaveDir, strFileName);

	Fi = fopen(strFilePath.GetBuffer(), "wt");
	if (Fi == NULL)	{
		return;
	}

	for (x = 0; x < DEF_MAX_X_LINE_NUM; x++) {
		strData.Format("");
		for (y = 0; y < DEF_MAX_Y_LINE_NUM; y++) {
			if (y != DEF_MAX_Y_LINE_NUM - 1)
				strData.AppendFormat("%d\t", pSpecData[x][y]);
			else
				strData.AppendFormat("%d", pSpecData[x][y]);
		}
		strData.AppendFormat("\n");
		fwrite(LPCTSTR(strData), 1, strData.GetLength(), Fi);
	}

	fclose(Fi);
}

void CZDataAnalyzerDlg::OnButtonSave()
{
	CGraphData * pGraphData = NULL;

	if (m_LinePlot.Count() <= 0)
		return;

	pGraphData = (CGraphData*)m_ListCtrlFiles.GetItemData(m_LinePlot.GetSelectedListIndex());
	if (pGraphData == NULL)
		return;

	int nDataType = m_ComboDataType.GetCurSel();

	int nLineNumX, nLineNumY;

	nLineNumX = pGraphData->m_nLineNumX;
	nLineNumY = pGraphData->m_nLineNumY;

	if (pGraphData->m_bUseIFE_mode == FALSE)
	{
		if (nDataType == DATA_TYPE_SELF_DND_RX || nDataType == DATA_TYPE_SELF_DND_TX || 
			nDataType == DATA_TYPE_SELF_DND_RX_H_GAP || nDataType == DATA_TYPE_SELF_DND_TX_H_GAP || 
			nDataType == DATA_TYPE_SELF_SATURATION_RX || nDataType == DATA_TYPE_SELF_SATURATION_TX )
		{
			nLineNumX = pGraphData->m_nMaxLineNumX;
			nLineNumY = pGraphData->m_nMaxLineNumY;
		}
	}

	// Self TX
	if (nDataType == DATA_TYPE_SELF_DND_TX || 
		nDataType == DATA_TYPE_SELF_DND_TX_H_GAP || 
 		nDataType == DATA_TYPE_SELF_SATURATION_TX )
	{
		nLineNumX = pGraphData->m_nSelfRowCount;
	}

	// H diff
	if (nDataType == DATA_TYPE_DND_H_GAP || nDataType == DATA_TYPE_HF_DND_H_GAP ||
		nDataType == DATA_TYPE_SELF_DND_RX_H_GAP || nDataType == DATA_TYPE_SELF_DND_TX_H_GAP )
	{
		nLineNumY = nLineNumY - 1;
	}
	// V diff
	if (nDataType == DATA_TYPE_DND_V_GAP || nDataType == DATA_TYPE_HF_DND_V_GAP)
	{
		nLineNumX = nLineNumX - 1;
	}

	switch (nDataType)
	{
	case DATA_TYPE_DND:
	{
		TEST_INFO_DND * pTestInfo_dnd = m_pTestModeView->GetTestInfo_DND();

		// Check_PP_DND_Data_Max
		for (int row = 0; row < nLineNumX; row++) {
			for (int col = 0; col < nLineNumY; col++) {
				pTestInfo_dnd->pDataMax[row][col] = m_LinePlot.m_pAverageMax[col + row*nLineNumY].y;
			}
		}

		SaveSpecData(DND_MAX_SPEC_INI, pTestInfo_dnd->pDataMax);

		// Check_PP_DND_Data_Min
		for (int row = 0; row < nLineNumX; row++) {
			for (int col= 0; col < nLineNumY; col++) {
				pTestInfo_dnd->pDataMin[row][col] = m_LinePlot.m_pAverageMin[col + row*nLineNumY].y;
			}
		}

		SaveSpecData(DND_MIN_SPEC_INI, pTestInfo_dnd->pDataMin);
	}
	break;

	case DATA_TYPE_CND:
	{
		TEST_INFO_CND* pTestInfo_cnd = m_pTestModeView->GetTestInfo_CND();

		// Check_PP_DND_Data_Max
		for (int row = 0; row < nLineNumX; row++) {
			for (int col = 0; col < nLineNumY; col++) {
				pTestInfo_cnd->pDataMax[row][col] = m_LinePlot.m_pAverageMax[col + row * nLineNumY].y;
			}
		}

		SaveSpecData(CND_MAX_SPEC_INI, pTestInfo_cnd->pDataMax);

		// Check_PP_DND_Data_Min
		for (int row = 0; row < nLineNumX; row++) {
			for (int col = 0; col < nLineNumY; col++) {
				pTestInfo_cnd->pDataMin[row][col] = m_LinePlot.m_pAverageMin[col + row * nLineNumY].y;
			}
		}

		SaveSpecData(CND_MIN_SPEC_INI, pTestInfo_cnd->pDataMin);
	}
	break;

	case DATA_TYPE_DND_H_GAP:
	{
		TEST_INFO_DND * pTestInfo_dnd = m_pTestModeView->GetTestInfo_DND();

		for (int row = 0; row < nLineNumX; row++) {
			for (int col = 0; col < (nLineNumY); col++) {
				pTestInfo_dnd->pHDiff[row][col] = m_LinePlot.m_pAverageMax[col + row*(nLineNumY)].y;
			}
		}

		SaveSpecData(DND_H_GAP_SPEC_INI, pTestInfo_dnd->pHDiff);
	}
	break;

	case DATA_TYPE_DND_V_GAP:
	{
		TEST_INFO_DND * pTestInfo_dnd = m_pTestModeView->GetTestInfo_DND();

		for (int row = 0; row < nLineNumX; row++) {
			for (int col = 0; col < nLineNumY; col++) {
				pTestInfo_dnd->pVDiff[row][col] = m_LinePlot.m_pAverageMax[col + row*nLineNumY].y;
			}
		}

		SaveSpecData(DND_V_GAP_SPEC_INI, pTestInfo_dnd->pVDiff);
	}
	break;

	case DATA_TYPE_HF_DND:
	{
		TEST_INFO_DND * pTestInfo_hf_dnd = m_pTestModeView->GetTestInfo_HF_DND();
	
		// Check_PP_DND_Data_Max_HF
		for (int row = 0; row < nLineNumX; row++) {
			for (int col = 0; col < nLineNumY; col++) {
				pTestInfo_hf_dnd->pDataMax[row][col] = m_LinePlot.m_pAverageMax[col + row*nLineNumY].y;
			}
		}

		SaveSpecData(HF_DND_MAX_SPEC_INI, pTestInfo_hf_dnd->pDataMax);

		// Check_PP_DND_Data_Min_HF
		for (int row = 0; row < nLineNumX; row++) {
			for (int col = 0; col < nLineNumY; col++) {
				pTestInfo_hf_dnd->pDataMin[row][col] = m_LinePlot.m_pAverageMin[col + row*nLineNumY].y;
			}
		}

		SaveSpecData(HF_DND_MIN_SPEC_INI, pTestInfo_hf_dnd->pDataMin);
	}
	break;

	case DATA_TYPE_HF_DND_H_GAP:
	{
		TEST_INFO_DND * pTestInfo_hf_dnd = m_pTestModeView->GetTestInfo_HF_DND();

		for (int row = 0; row < nLineNumX; row++) {
			for (int col = 0; col < (nLineNumY); col++) {
				pTestInfo_hf_dnd->pHDiff[row][col] = m_LinePlot.m_pAverageMax[col + row*(nLineNumY)].y;
			}
		}

		SaveSpecData(HF_DND_H_GAP_SPEC_INI, pTestInfo_hf_dnd->pHDiff);
	}
	break;

	case DATA_TYPE_HF_DND_V_GAP:
	{
		TEST_INFO_DND * pTestInfo_hf_dnd = m_pTestModeView->GetTestInfo_HF_DND();

		for (int row = 0; row < nLineNumX; row++) {
			for (int col = 0; col < nLineNumY; col++) {
				pTestInfo_hf_dnd->pVDiff[row][col] = m_LinePlot.m_pAverageMax[col + row*nLineNumY].y;
			}
		}

		SaveSpecData(HF_DND_V_GAP_SPEC_INI, pTestInfo_hf_dnd->pVDiff);
	}
	break;

	case DATA_TYPE_SELF_DND_RX:
	{
		TEST_INFO_SELF * pTestInfo_self_dnd = m_pTestModeView->GetTestInfo_SELF_DND();

		// MAX
		for (int col = 0; col < nLineNumY; col++)
		{
			pTestInfo_self_dnd->pRxDataMax[0][col] = m_LinePlot.m_pAverageMax[col].y;
		}

		SaveSpecData(SELF_DND_RX_MAX_SPEC_INI, pTestInfo_self_dnd->pRxDataMax);

		// MIN
		for (int col = 0; col < nLineNumY; col++)
		{
			pTestInfo_self_dnd->pRxDataMin[0][col] = m_LinePlot.m_pAverageMin[col].y;
		}

		SaveSpecData(SELF_DND_RX_MIN_SPEC_INI, pTestInfo_self_dnd->pRxDataMin);
	}
	break;

	case DATA_TYPE_SELF_DND_TX:
	{
		TEST_INFO_SELF * pTestInfo_self_dnd = m_pTestModeView->GetTestInfo_SELF_DND();

		// MAX
		for (int row = 0; row < nLineNumX; row++)
		{
			for (int col = 0; col < nLineNumY; col++)
			{
				pTestInfo_self_dnd->pTxDataMax[row][col] = m_LinePlot.m_pAverageMax[col + row*nLineNumY].y;
			}
		}

		SaveSpecData(SELF_DND_TX_MAX_SPEC_INI, pTestInfo_self_dnd->pTxDataMax);

		// MIN
		for (int row = 0; row < nLineNumX; row++)
		{
			for (int col = 0; col < nLineNumY; col++)
			{
				pTestInfo_self_dnd->pTxDataMin[row][col] = m_LinePlot.m_pAverageMin[col + row*nLineNumY].y;
			}
		}

		SaveSpecData(SELF_DND_TX_MIN_SPEC_INI, pTestInfo_self_dnd->pTxDataMin);
	}
	break;

	case DATA_TYPE_SELF_DND_RX_H_GAP:
	{
		TEST_INFO_SELF * pTestInfo_self_dnd = m_pTestModeView->GetTestInfo_SELF_DND();

		for (int col = 0; col < nLineNumY; col++)
		{
			pTestInfo_self_dnd->pRxHDiff[0][col] = m_LinePlot.m_pAverageMax[col].y;
		}

		SaveSpecData(SELF_DND_RX_H_GAP_SPEC_INI, pTestInfo_self_dnd->pRxHDiff);
	}
	break;

	case DATA_TYPE_SELF_DND_TX_H_GAP:
	{
		TEST_INFO_SELF * pTestInfo_self_dnd = m_pTestModeView->GetTestInfo_SELF_DND();

		for (int row = 0; row < nLineNumX; row++)
		{
			for (int col = 0; col < nLineNumY; col++)
			{
				pTestInfo_self_dnd->pTxHDiff[row][col] = m_LinePlot.m_pAverageMax[col + row*nLineNumY].y;
			}
		}

		SaveSpecData(SELF_DND_TX_H_GAP_SPEC_INI, pTestInfo_self_dnd->pTxHDiff);
	}
	break;

	case DATA_TYPE_SELF_SATURATION_RX:
	{
		TEST_INFO_SELF * pTestInfo_self_saturation = m_pTestModeView->GetTestInfo_SELF_SATURATION();

		// MAX
		for (int col = 0; col < nLineNumY; col++)
		{
			pTestInfo_self_saturation->pRxDataMax[0][col] = m_LinePlot.m_pAverageMax[col].y;
		}

		SaveSpecData(SELF_SATURATION_RX_MAX_SPEC_INI, pTestInfo_self_saturation->pRxDataMax);

		// MIN
		for (int col = 0; col < nLineNumY; col++)
		{
			pTestInfo_self_saturation->pRxDataMin[0][col] = m_LinePlot.m_pAverageMin[col].y;
		}

		SaveSpecData(SELF_SATURATION_RX_MIN_SPEC_INI, pTestInfo_self_saturation->pRxDataMin);
	}
	break;

	case DATA_TYPE_SELF_SATURATION_TX:
	{
		TEST_INFO_SELF * pTestInfo_self_saturation = m_pTestModeView->GetTestInfo_SELF_SATURATION();

		// MAX
		for (int row = 0; row < nLineNumX; row++)
		{
			for (int col = 0; col < nLineNumY; col++)
			{
				pTestInfo_self_saturation->pTxDataMax[row][col] = m_LinePlot.m_pAverageMax[col + row*nLineNumY].y;
			}
		}

		SaveSpecData(SELF_SATURATION_TX_MAX_SPEC_INI, pTestInfo_self_saturation->pTxDataMax);

		// MIN
		for (int row = 0; row < nLineNumX; row++)
		{
			for (int col = 0; col < nLineNumY; col++)
			{
				pTestInfo_self_saturation->pTxDataMin[row][col] = m_LinePlot.m_pAverageMin[col + row*nLineNumY].y;
			}
		}

		SaveSpecData(SELF_SATURATION_TX_MIN_SPEC_INI, pTestInfo_self_saturation->pTxDataMin);
	}
	break;

	default:
		break;
	}
}

