#if !defined(AFX_SETDIALOG_H__D80218E2_95F8_43B3_B865_9D73B4CFCD6D__INCLUDED_)
#define AFX_SETDIALOG_H__D80218E2_95F8_43B3_B865_9D73B4CFCD6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// setDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CsetDialog dialog

class CsetDialog : public CDialog
{
// Construction
public:
	CsetDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CsetDialog)
	enum { IDD = IDD_DIALOG1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CsetDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CsetDialog)
	afx_msg void OnAdduser();
	virtual void OnCancel();
	afx_msg void OnCmd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETDIALOG_H__D80218E2_95F8_43B3_B865_9D73B4CFCD6D__INCLUDED_)
