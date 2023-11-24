// Filename: ScrollDlg.h

#ifndef AFX_SCROLL_DLG_INCLUDED
#define AFX_SCROLL_DLG_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Forward class declarations.
class CScrollHelper;

// CScrollDlg is a resizable, scrollable dialog that uses CScrollHelper.
class CScrollDlg : public CDialog
{
public:
    CScrollDlg(UINT nIDTemplate, CWnd* pParent = NULL);
    virtual ~CScrollDlg();

    // Dialog data.
    //{{AFX_DATA(CScrollDlg)
    //}}AFX_DATA

protected:
    virtual void PostNcDestroy();

    // ClassWizard generated virtual function overrides.
    //{{AFX_VIRTUAL(CScrollDlg)
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    //}}AFX_VIRTUAL

    // Generated message map functions.
    //{{AFX_MSG(CScrollDlg)
    afx_msg int  OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
    afx_msg void OnPaint();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    void DisplayScrollInfo();
    void DisplayScrollPos();

    CScrollHelper* m_scrollHelper;

    bool     m_isInit;
};

#endif // AFX_SCROLL_DLG_INCLUDED

// END

