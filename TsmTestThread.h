#if !defined(AFX_TSM_TEST_THREAD_H_)
#define AFX_TSM_TEST_THREAD_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UsbHidThread.h : header file
//

#include ".\Hid\UsbHid.h"

/////////////////////////////////////////////////////////////////////////////
// CTsmTestThread thread

class CTsmTestThread : public CWinThread
{
	DECLARE_DYNCREATE(CTsmTestThread)

protected:
	CTsmTestThread();           // protected constructor used by dynamic creation

// Attributes
private:

	CUsbHid *	m_pUsbHid;

	int	m_ThreadState;

// Operations
public:

	enum
	{
		THREAD_STATE_NONE = 0,
		THREAD_STATE_RUNNING
	};

	void SetUsbHidInfo(CUsbHid * pUsbHid);
	void SendThreadExitEvent();
	void StartTsmTest();
	int	 GetThreadState()	{ return m_ThreadState; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTsmTestThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTsmTestThread();

	// Generated message map functions
	//{{AFX_MSG(CTsmTestThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	afx_msg void OnTsmTest(WPARAM wParam, LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TSM_TEST_THREAD_H_)
