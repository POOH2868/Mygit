#pragma once
#include "afxwin.h"

// CGetIpAddrDlg 대화 상자입니다.

class CGetIpAddrDlg : public CDialog
{
	DECLARE_DYNAMIC(CGetIpAddrDlg)

public:
	CGetIpAddrDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CGetIpAddrDlg();
	int m_nPort;
	CString m_strServerIp;
	void AddIpAddress(CString strIP);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SEL_IPADDRESS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CComboBox m_comIP;	
public:
	afx_msg void OnBnClickedOk();
};
