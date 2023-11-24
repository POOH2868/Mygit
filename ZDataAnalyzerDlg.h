
// ZDataAnalyzerDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include ".\CustomUI\ResizableDialog.h"
#include ".\CustomUI\LinePlot.h"

class CTestModeView;

struct DATA_TYPE
{
	CString m_strTypeName;
	CString m_strLogPrefix;

	emDataType m_DataType;
};

struct HV_GRAPH_DATA
{
	FLOATPOINT * pData;
	int nCount;
	int nMaxY;
	int nMinY;
};

enum
{
	DATA_TYPE_DND,
	DATA_TYPE_DND_H_GAP,
	DATA_TYPE_DND_V_GAP,

	DATA_TYPE_HF_DND,
	DATA_TYPE_HF_DND_H_GAP,
	DATA_TYPE_HF_DND_V_GAP,

	DATA_TYPE_SELF_DND_RX,			// COL
	DATA_TYPE_SELF_DND_TX,			// ROW
	DATA_TYPE_SELF_DND_RX_H_GAP,	// COL
	DATA_TYPE_SELF_DND_TX_H_GAP,	// ROW

	DATA_TYPE_SELF_SATURATION_RX,	// COL
	DATA_TYPE_SELF_SATURATION_TX,	// ROW

	DATA_TYPE_CND,					// CND	//add 2022.11.14 for ZTM730 CND
};

struct GRAPHT_DATA
{
	HV_GRAPH_DATA	m_DataDnd;
	HV_GRAPH_DATA	m_DataDndHGap;
	HV_GRAPH_DATA	m_DataDndVGap;

	HV_GRAPH_DATA	m_DataHfDnd;
	HV_GRAPH_DATA	m_DataHfDndHGap;
	HV_GRAPH_DATA	m_DataHfDndVGap;

	HV_GRAPH_DATA	m_DataSelfDndRx;
	HV_GRAPH_DATA	m_DataSelfDndTx;
	HV_GRAPH_DATA	m_DataSelfDndRxHGap;
	HV_GRAPH_DATA	m_DataSelfDndTxHGap;

	HV_GRAPH_DATA	m_DataSelfSaturationRx;
	HV_GRAPH_DATA	m_DataSelfSaturationTx;
};

class CGraphData : public GRAPHT_DATA
{
public:
	CGraphData()
	{
		memset(&m_DataDnd, 0x00, sizeof(HV_GRAPH_DATA));
		memset(&m_DataDndHGap, 0x00, sizeof(HV_GRAPH_DATA));
		memset(&m_DataDndVGap, 0x00, sizeof(HV_GRAPH_DATA));

		memset(&m_DataHfDnd, 0x00, sizeof(HV_GRAPH_DATA));
		memset(&m_DataHfDndHGap, 0x00, sizeof(HV_GRAPH_DATA));
		memset(&m_DataHfDndVGap, 0x00, sizeof(HV_GRAPH_DATA));

		memset(&m_DataSelfDndRx, 0x00, sizeof(HV_GRAPH_DATA));
		memset(&m_DataSelfDndTx, 0x00, sizeof(HV_GRAPH_DATA));
		memset(&m_DataSelfDndRxHGap, 0x00, sizeof(HV_GRAPH_DATA));
		memset(&m_DataSelfDndTxHGap, 0x00, sizeof(HV_GRAPH_DATA));

		memset(&m_DataSelfSaturationRx, 0x00, sizeof(HV_GRAPH_DATA));
		memset(&m_DataSelfSaturationTx, 0x00, sizeof(HV_GRAPH_DATA));

		m_bUseIFE_mode = FALSE;

		m_GraphIndex = -1;

		m_nLineNumX = 0;
		m_nLineNumY = 0;
		m_nMaxLineNumX = 0;
		m_nMaxLineNumY = 0;
	}

	~CGraphData()
	{
		if (m_DataDnd.pData){ delete[] m_DataDnd.pData; m_DataDnd.pData = NULL; }
		if (m_DataDndHGap.pData){ delete[] m_DataDndHGap.pData; m_DataDndHGap.pData = NULL; }
		if (m_DataDndVGap.pData){ delete[] m_DataDndVGap.pData; m_DataDndVGap.pData = NULL; }

		if (m_DataHfDnd.pData){ delete[] m_DataHfDnd.pData; m_DataHfDnd.pData = NULL; }
		if (m_DataHfDndHGap.pData){ delete[] m_DataHfDndHGap.pData; m_DataHfDndHGap.pData = NULL; }
		if (m_DataHfDndVGap.pData){ delete[] m_DataHfDndVGap.pData; m_DataHfDndVGap.pData = NULL; }

		if (m_DataSelfDndTx.pData){ delete[] m_DataSelfDndTx.pData; m_DataSelfDndTx.pData = NULL; }
		if (m_DataSelfDndRx.pData){ delete[] m_DataSelfDndRx.pData; m_DataSelfDndRx.pData = NULL; }
		if (m_DataSelfDndTxHGap.pData){ delete[] m_DataSelfDndTxHGap.pData; m_DataSelfDndTxHGap.pData = NULL; }
		if (m_DataSelfDndRxHGap.pData){ delete[] m_DataSelfDndRxHGap.pData; m_DataSelfDndRxHGap.pData = NULL; }

		if (m_DataSelfSaturationRx.pData){ delete[] m_DataSelfSaturationRx.pData; m_DataSelfSaturationRx.pData = NULL; }
		if (m_DataSelfSaturationTx.pData){ delete[] m_DataSelfSaturationTx.pData; m_DataSelfSaturationTx.pData = NULL; }
	}

	void operator=(const CGraphData& cc)
	{
		(CGraphData)*this = cc;
	}

	BOOL m_bUseIFE_mode;

	int m_GraphIndex;

	int m_nLineNumX;
	int m_nLineNumY;
	int m_nMaxLineNumX;
	int m_nMaxLineNumY;

	int m_nSelfRowCount;
};

// CZDataAnalyzerDlg dialog
class CZDataAnalyzerDlg : public CResizableDialog
{
// Construction
public:
	CZDataAnalyzerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ZDATAANALYZER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
public:

	void SetTestModeViewPtr(CTestModeView * ptr) { m_pTestModeView = ptr; }
	void SetDataAnalysisDir(CString strDir) { m_strDataAnalysisDir = strDir; }

protected:

	HICON m_hIcon;

	CTestModeView * m_pTestModeView;

	CListCtrl m_ListCtrlFiles;
	CComboBox m_ComboDataType;

	CString m_strZtdExeDir;
	CString m_strDataAnalysisDir;
	CString m_strCurDir;
	CString m_strSaveDir;

	CArray<DATA_TYPE, DATA_TYPE> m_DataTypeArray;

	BOOL m_bCheckAll;
	BOOL m_bIgnoreItemChangedEvent;

	void InitUiControls();

	void FileList_Initialize();
	void FileList_Find(CString csDirPath, BOOL bBrowse = FALSE);
	void FileList_Add(CString strFile, BOOL bFile);
	void FileList_Delete();
	void FileList_ReadData(int nRow);
	void FileList_RemoveAllData();
	void FileList_UpdateDataExist();
	void FileList_SetFocus(int nIndex);
	BOOL FileList_IsValidFile(CString strFile);

	CString GetSelectedFilePath(int nRow);

	HV_GRAPH_DATA * GetGraphData(CGraphData * pGraphData, int nDataType);

#if 1	// D_USE_LINE_NUMBER_IN_LOG
	BOOL ReadLineNumber(CString strFilePath, CGraphData * pGraphData);
#else
	BOOL ReadData_GetLineNum(int nDataType, CString strFilePath, CGraphData * pGraphData);
	int ReadData_GetLineNumY(int nDataType, char * pBuffer, char * pFindChar);
#endif
	
	BOOL ReadFileParsing(int nDataType, CString strFilePath, CGraphData * pGraphData);
	void ReadDataParsing(int nDataType, char * pBuffer, FLOATPOINT * pDataPtr, int nIndex, char * pFindChar);

	void ReadData_GetMinMax(int nDataType, CGraphData * pGraphData);

	void SaveSpecData(CString strFileName, int pSpecData[DEF_MAX_X_LINE_NUM][DEF_MAX_Y_LINE_NUM]);

	void EnableSaveButton();

	// graph

	CLinePlot m_LinePlot;

	void LinePlot_Init(void);
	FLOATPOINT * LinePlot_GetDataPtr(CGraphData * pGraphData);
	int LinePlot_GetDataCount(CGraphData * pGraphData);
	void LinePlot_GetDataY_MinMax(CGraphData * pGraphData, int *nMinY, int *nMaxY);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCbnSelchangeComboDataType();
	afx_msg LRESULT OnGraphSelected(WPARAM wParam, LPARAM lParam);
	afx_msg void OnButtonGraphDraw();
	afx_msg void OnButtonDataAnalysisDirBrowse();
	afx_msg void OnButtonSave();
	DECLARE_MESSAGE_MAP()
};

