#pragma once
#include "afxwin.h"

// CGetIpAddrDlg ��ȭ �����Դϴ�.

class CGetIpAddrDlg : public CDialog
{
	DECLARE_DYNAMIC(CGetIpAddrDlg)

public:
	CGetIpAddrDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CGetIpAddrDlg();
	int m_nPort;
	CString m_strServerIp;
	void AddIpAddress(CString strIP);

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SEL_IPADDRESS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	CComboBox m_comIP;	
public:
	afx_msg void OnBnClickedOk();
};
