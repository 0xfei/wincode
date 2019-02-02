// sethc_doorDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "sethc_door.h"
#include "sethc_doorDlg.h"
#include "setDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSethc_doorDlg dialog

CSethc_doorDlg::CSethc_doorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSethc_doorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSethc_doorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	ID = FALSE;
	ID2 = FALSE;
}

void CSethc_doorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSethc_doorDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSethc_doorDlg, CDialog)
	//{{AFX_MSG_MAP(CSethc_doorDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSethc_doorDlg message handlers

BOOL CSethc_doorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSethc_doorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSethc_doorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSethc_doorDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CSethc_doorDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	//CDialog::OnCancel();
	if(ID2)
	{
		ID =true;
	    m_MyEdit.CreateEx(WS_EX_CLIENTEDGE, // 3D-border外观
                     _T("EDIT"),
				   NULL,
                   ES_AUTOHSCROLL | ES_LEFT | ES_NOHIDESEL | WS_CHILD,
                   CRect(280,80,350,100), this,
                   ID_EDT);//生成edit控件

        m_MyEdit.ShowWindow(SW_SHOW); //显示控件
        m_MyEdit.SetFocus(); //设置焦点
		ID2 = FALSE;

	}
	else
	{
		CDialog::OnCancel();
	}


}

void CSethc_doorDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	char pass[16];
	if (ID)
	{
		GetDlgItem(ID_EDT)->GetWindowText(pass,16);
		if (strcmp(pass, password)==0)
		{
			CsetDialog dialog;
			dialog.DoModal();
		} else
		{
			MessageBox("Wrong password");
		}
	} else
	{
		ID2 = true;
	}
}
