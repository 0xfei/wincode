// sethc_doorDlg.h : header file
//

#if !defined(AFX_SETHC_DOORDLG_H__942A7333_CF27_4166_9166_8AB033CF2B38__INCLUDED_)
#define AFX_SETHC_DOORDLG_H__942A7333_CF27_4166_9166_8AB033CF2B38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSethc_doorDlg dialog
#define password "hhx"

class CSethc_doorDlg : public CDialog
{
// Construction
public:
	CSethc_doorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSethc_doorDlg)
	enum { IDD = IDD_SETHC_DOOR_DIALOG };
	CEdit m_MyEdit;
	BOOL ID ;
	BOOL ID2 ;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSethc_doorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSethc_doorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETHC_DOORDLG_H__942A7333_CF27_4166_9166_8AB033CF2B38__INCLUDED_)
