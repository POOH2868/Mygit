// GetIpAddrDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TouchDebugger.h"
#include "GetIpAddrDlg.h"


// CGetIpAddrDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CGetIpAddrDlg, CDialog)
CGetIpAddrDlg::CGetIpAddrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGetIpAddrDlg::IDD, pParent)
{
}

CGetIpAddrDlg::~CGetIpAddrDlg()
{
}

void CGetIpAddrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SEL_IPADDRESS, m_comIP);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
}


BEGIN_MESSAGE_MAP(CGetIpAddrDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CGetIpAddrDlg 메시지 처리기입니다.
void CGetIpAddrDlg::AddIpAddress(CString strIP)
{
	m_comIP.AddString(strIP);
}
void CGetIpAddrDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	int nIdx = 0;
	CString	strSel;	
	nIdx = m_comIP.GetCurSel();			
	m_comIP.GetLBText(nIdx, m_strServerIp);	 
	OnOK();
}
