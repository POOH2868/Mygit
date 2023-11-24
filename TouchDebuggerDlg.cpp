// TouchDebuggerDlg.cpp : 구현 파일
#include "stdafx.h"

#include "TouchDebugger.h"
#include "TouchDebuggerDlg.h"

#include "util.h"

#include <dbt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDC_HAND            MAKEINTRESOURCE(32649)

#define SKIP_COUNT	100

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CTouchDebuggerDlg 대화 상자

CTouchDebuggerDlg::CTouchDebuggerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTouchDebuggerDlg::IDD, pParent)
{
	// to find for a source of leak {xxx}
	//_CrtSetBreakAlloc(454);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CTouchDebuggerDlg::~CTouchDebuggerDlg()
{
	CloseHandle(g_hSemaphore);
	CloseHandle(g_hInfoSemaphore);

	// unregister
	UnregisterDeviceNotification(m_DeviceNotificationHandle);
}

void CTouchDebuggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PROGRESS_MAIN, m_progressBar);
}

BEGIN_MESSAGE_MAP(CTouchDebuggerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_DEVICECHANGE()
	ON_WM_QUERYDRAGICON()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_USER_PROGRESS_UPDATE, CTouchDebuggerDlg::OnUMProgressbar)	// progress bar
	
	ON_BN_CLICKED(IDC_BUTTON_TEST_MODE, CTouchDebuggerDlg::OnBnClickedButtonTestMode)

	END_MESSAGE_MAP()


LRESULT CTouchDebuggerDlg::OnUMProgressbar(WPARAM wParam, LPARAM lParam)
{
	int nTestStep = (int)wParam;
	CString *str = (CString *) lParam;

	if (g_bAppExit)
		return 1;

	if (str != NULL)
	{
		((CStatusBarView*)m_statusBarView)->SetStatusText((LPCTSTR)(CString)(*str));
		return 1;
	}

	if (nTestStep < 0)
	{
		m_progressBar.SetPos(0);

		m_progressBar.SetBkColor(RGB(255, 192, 192));
	}
	else
	{
		int nTotalTestStep = ((CTestModeView*)m_testModeView)->GetAvailableTestStep();

		if (nTestStep == 0 || nTotalTestStep == 0)
			m_progressBar.SetPos(0);
		else
			m_progressBar.SetPos((nTestStep * 100) / nTotalTestStep);

		m_progressBar.SetBkColor(RGB(186, 223, 253));
	}
	return 1;
}

// CTouchDebuggerDlg 메시지 처리기

BOOL CTouchDebuggerDlg::OnInitDialog()
{
	static BOOL bAlreadyInit = FALSE;
	CDialog::OnInitDialog();

	if (bAlreadyInit == TRUE) {	
		return TRUE;
	}
	
	bAlreadyInit = TRUE;
	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	RegisterForDeviceNotifications();

	CString strTitle;
	strTitle.Format("%s [HID MODE]", GetTargetName());
	this->SetWindowText((LPCTSTR)strTitle);
	
	g_hSemaphore = ::CreateSemaphore(NULL, 1, 1, "ZTouch Debugger Hid");
	g_hInfoSemaphore = ::CreateSemaphore(NULL, 1, 1, "ZTouch Debugger Hid Info");

	TRACE("SEMA HANDLE : %d\n", g_hSemaphore);
	TRACE("SEMA INFO HANDLE : %d\n", g_hInfoSemaphore);

	CRect rect;
	GetClientRect(&rect);
	rect.DeflateRect(20, 20); // rect defines the area of the main splitter pane	

	// formview
	CCreateContext cc;
	CView *pView = (CView*)RUNTIME_CLASS(CTestModeView)->CreateObject();
	::ZeroMemory(&cc, sizeof(cc));
	pView->Create(NULL, NULL, WS_CHILD, rect, this, IDD_FORMVIEW_TESTMODE, &cc);
	((CTestModeView*)pView)->m_parentHWnd = this->m_hWnd;
	pView->OnInitialUpdate();
	m_testModeView = pView;

	((CTestModeView*)m_testModeView)->MoveWindow(&rect);
	((CTestModeView*)m_testModeView)->ShowWindow(SW_HIDE);

	CView *pView2 = (CView*)RUNTIME_CLASS(CStatusBarView)->CreateObject();	
	pView2->Create(NULL, NULL, WS_CHILD, rect, this, IDD_FORMVIEW_STATUSBAR, &cc);
	pView2->OnInitialUpdate();
	m_statusBarView = pView2;
	m_statusBarView->MoveWindow(&rect);
	m_statusBarView->ShowWindow(SW_SHOW);	
	((CStatusBarView*)m_statusBarView)->m_parentHWnd = this->m_hWnd;	

	// dialog size
	long cx, cy;
	cy = ::GetSystemMetrics(SM_CYSCREEN);
	cx = ::GetSystemMetrics(SM_CXSCREEN);

#if 1
	cy = cy * 9 / 10;
	cx = cx * 9 / 10;
#else
	if (cx <= 1000 || cy <= 800)
	{
		cy = cy * 9 / 10;
		cx = cx * 9 / 10;
	}
	else
	{
		cy = cy * 2 / 3;
		cx = cx * 3 / 4;
	}
#endif
	this->MoveWindow(0, 0, cx, cy);

	m_progressBar.SetRange( 0, 100 );	
	m_progressBar.SetBkColor(RGB(186, 223, 253) );

	gCapInfo.authorityDepth = USER_AUTHORITY;

	g_bPowerState = TRUE;

	if( ((CTestModeView*)m_testModeView)->ConfirmStartupPassword() == FALSE )
	{
		AfxMessageBox("Please. Check the Password. \nThis tool will Exit.");
		EndDialog(TRUE);
		return TRUE;
	}

	BOOL bRet = -1;

	do
	{
		bRet = PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_TEST_MODE, BN_CLICKED), (LPARAM)this->m_hWnd);
	}while( bRet == 0 );

	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CTouchDebuggerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;		
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CTouchDebuggerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();	
	}	
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CTouchDebuggerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTouchDebuggerDlg::OnBnClickedButtonExit()
{
	CTestModeView * pTestModeView = ((CTestModeView*)m_testModeView);

	if (pTestModeView->IsAleadyTestEnter())
	{
		if (AfxMessageBox(_T("TSM Testing is currently in progress...\nDo you really want to quit the program?"), MB_OKCANCEL) != IDOK)
		{
			return;
		}
	}

	g_bAppExit = TRUE;

	pTestModeView->KillAllTimer();
	CDialog::OnCancel();
}

#define	LEFT_X	10
#define	LEFT_Y	5
#define	BOTTOM_STATIC_HEIGHT 25
#define	PIC_HEIGHT	2
#define	PIC_OFFSET	2

void CTouchDebuggerDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	RECT	mainRect;
	int	nBtnY;

	CTestModeView * pTestModeView = (CTestModeView*)m_testModeView;

	CDialog::OnWindowPosChanged(lpwndpos);
		
	if (IsIconic())	return;

	if (pTestModeView == NULL || pTestModeView->IsCreatePane() == FALSE)	{
		return;
	}
	
	this->GetClientRect(&mainRect);	

	if(m_MainRect.bottom == mainRect.bottom && m_MainRect.left == mainRect.left &&
		m_MainRect.right == mainRect.right && m_MainRect.top == mainRect.top)
		return;

	m_MainRect = mainRect;	
	
	{	
		m_lSpliteTopOffset = PIC_OFFSET*3 + PIC_HEIGHT*2;
		m_lSpliteBottomOffset = BOTTOM_STATIC_HEIGHT + PIC_HEIGHT * 3;
		m_lSpliteSideOffset = LEFT_X;
	}

	m_PaneRect = mainRect;
	m_PaneRect.left += m_lSpliteSideOffset;
	m_PaneRect.right -= m_lSpliteSideOffset;
	m_PaneRect.top += m_lSpliteTopOffset;
	m_PaneRect.bottom -= m_lSpliteBottomOffset;
	
	CRect rectTmp;

	pTestModeView->MoveWindow(&m_PaneRect);

	if (m_statusBarView != NULL){
		nBtnY = m_PaneRect.bottom;
		m_progressBar.MoveWindow(LEFT_X, nBtnY, mainRect.right - (LEFT_X * 2), PIC_HEIGHT * 3);

		nBtnY += PIC_HEIGHT * 3;
		((CStatusBarView*)m_statusBarView)->MoveWindow(LEFT_X, nBtnY, mainRect.right - (LEFT_X * 2), BOTTOM_STATIC_HEIGHT);
	}
}

void CTouchDebuggerDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanging(lpwndpos);	

	if(lpwndpos->cx < 500) 	lpwndpos->cx = 550; 
	if(lpwndpos->cy < 250)	lpwndpos->cy = 300;		
	
}

void CTouchDebuggerDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString strErr;
	static BOOL bEnter = FALSE;

	if (bEnter == TRUE)	return;

	bEnter = TRUE;
	
	if (nIDEvent == 1) {
		KillTimer(1);
	}

	bEnter = FALSE;

	CDialog::OnTimer(nIDEvent);
}

void CTouchDebuggerDlg::OnClose()
{
#if 0
	FILE			*fi;
	char	chTmp[1024];
	int GroupCount = 0, cnt;

	fi = fopen("reg.txt", "w+b");
	fputs("tsRegNameInfo g5thRegNameInfo[] = {\n", fi);

	for (int totalCount = 0; totalCount < 0x800;) {
		sprintf(chTmp, "\t// %04XH ~ %04XH \n", totalCount, totalCount + 16); 
		fputs(chTmp, fi);
		for (cnt = 0; cnt < 16; cnt++) {
			sprintf(chTmp, "\t{VALID, RESERVED_GROUP_%02d, %d, \"RESERVED_%04XH\", \"\", EDITABLE, HEX_FORMAT, NORMAL_AUTHORITY}, //%04XH\n", GroupCount, cnt, totalCount, totalCount);
			fputs(chTmp, fi);			
			totalCount++;
		}
		sprintf(chTmp, "\t// %04XH ~ %04XH \n", totalCount, totalCount + 16);
		fputs(chTmp, fi);
		for (cnt = 0; cnt < 16; cnt++) {
			sprintf(chTmp, "\t{VALID, RESERVED_GROUP_%02d, %d, \"RESERVED_%04XH\", \"\", EDITABLE, HEX_FORMAT, NORMAL_AUTHORITY}, //%04XH\n", GroupCount, cnt + 16, totalCount, totalCount);
			fputs(chTmp, fi);
			totalCount++;
		}
		GroupCount++;
	}
	fputs("\t{INVALID, ENDGROUP, 0, \"\", \"\", EDITABLE, SIGNED_FORMAT, NORMAL_AUTHORITY },\n", fi); 
	fputs("};\n", fi);
	fclose(fi);
#endif

	OnBnClickedButtonExit();
}

int CTouchDebuggerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

BOOL CTouchDebuggerDlg::PreTranslateMessage(MSG* pMsg)
{
	CString	str;
	UINT nRepCnt = (UINT)((pMsg->lParam)&0x0000FFFF);
	UINT nFlags = (UINT)((pMsg->lParam)>>16);
	//if(pMsg->lParam != 0 && pMsg->wParam != 0) {
	//	str.Format("%x, %x", pMsg->lParam, pMsg->wParam);
	//	AfxMessageBox(str);
	//}	

	if (pMsg->message == WM_KEYDOWN) {		
		// previous key statue
		//if(nFlags & 0x4000) {
		// 눌려진 상태
		//} else {
		// 한번 눌림 처리
		//}

		switch(pMsg->wParam) {
			case VK_RETURN:
			case VK_ESCAPE:				
				return TRUE;

			default:
				break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CTouchDebuggerDlg::PreCreateWindow(CREATESTRUCT& cs)
{	
	//cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
    //    | WS_SYSMENU | WS_MINIMIZEBOX  | WS_MINIMIZE; 
    //cs.style &= ~FWS_ADDTOTITLE;  
 	
	// Size the window to 1/2 screen size and center it
    //cs.cy = ::GetSystemMetrics(SM_CYSCREEN) * 3 / 4;
    //cs.cx = ::GetSystemMetrics(SM_CXSCREEN) * 3 / 4;
    //cs.y = cs.cy/2;
    //cs.x = cs.cx/2; 

	return CDialog::PreCreateWindow(cs);
}

BOOL CTouchDebuggerDlg::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}

void CTouchDebuggerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
}

void CTouchDebuggerDlg::OnBnClickedButtonTestMode()
{
	CTestModeView * pTestModeView = (CTestModeView*)m_testModeView;

	((CStatusBarView*)m_statusBarView)->SetStatusText("TEST MODE");

	//SemaWait(g_hSemaphore);
	//pTestModeView->FindUsbHid();
	//pTestModeView->m_nNeedResearch = 3;
	//::ReleaseSemaphore(g_hSemaphore, 1, NULL);

	pTestModeView->EnterTestModeView();
}

void CTouchDebuggerDlg::OnDestroy()
{	
	((CTestModeView*)m_testModeView)->KillAllTimer();

	CDialog::OnDestroy();
}

//========================================================================

void CTouchDebuggerDlg::RegisterForDeviceNotifications()

{
	// Register GUID 
	GUID hidGuid;
	HidD_GetHidGuid(&hidGuid);
   
    DEV_BROADCAST_DEVICEINTERFACE DevBroadcastDeviceInterface;

	ZeroMemory( &DevBroadcastDeviceInterface, sizeof(DevBroadcastDeviceInterface) );
    DevBroadcastDeviceInterface.dbcc_size = sizeof(DevBroadcastDeviceInterface);
    DevBroadcastDeviceInterface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    DevBroadcastDeviceInterface.dbcc_classguid = hidGuid;

    m_DeviceNotificationHandle = RegisterDeviceNotification(this->m_hWnd, &DevBroadcastDeviceInterface, DEVICE_NOTIFY_WINDOW_HANDLE);
	if ( m_DeviceNotificationHandle == NULL )
	{
		TRACE("ERROR\n");
	}
}

// WM_DEVICECHANGE 이벤트 핸들러
BOOL CTouchDebuggerDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwEventData)
{
	BOOL bDeviceChanged = FALSE;

	PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)dwEventData;

	switch (nEventType)
	{
	case DBT_DEVICEARRIVAL: // Attach
	{
		if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
		{
			PDEV_BROADCAST_DEVICEINTERFACE lpdb = (PDEV_BROADCAST_DEVICEINTERFACE)dwEventData;

			if ( !(StrStrI(lpdb->dbcc_name, "vid_14E5") && StrStrI(lpdb->dbcc_name, "pid_003a") && StrStrI(lpdb->dbcc_name, "mi_00")) )
				break;

			bDeviceChanged = TRUE;

			TRACE("##### MPTool inserted [%s] #####\n", lpdb->dbcc_name);
   		}
	}
	break;

	case DBT_DEVICEREMOVECOMPLETE:	// Detach
	{
		if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
		{
			PDEV_BROADCAST_DEVICEINTERFACE lpdb = (PDEV_BROADCAST_DEVICEINTERFACE)dwEventData;

			if ( !(StrStrI(lpdb->dbcc_name, "vid_14E5") && StrStrI(lpdb->dbcc_name, "pid_003a") && StrStrI(lpdb->dbcc_name, "mi_00")) )
				break;

			bDeviceChanged = TRUE;

			TRACE("##### MPTool removed [%s] #####\n", lpdb->dbcc_name);
   		}
	}
	break;
	}

	if (bDeviceChanged)
	{
		CTestModeView * pTestModeView = ((CTestModeView*)m_testModeView);
		if (pTestModeView == NULL)
			return TRUE;	

		pTestModeView->m_nNeedResearch = 3;

		pTestModeView->UsbHidDevice_StartSurveyTimer(TRUE);
	}

	return TRUE;
}


