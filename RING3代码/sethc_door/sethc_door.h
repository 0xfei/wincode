// sethc_door.h : main header file for the SETHC_DOOR application
//

#if !defined(AFX_SETHC_DOOR_H__E4A1A6BF_9CEC_4240_9B94_B0958AF9F281__INCLUDED_)
#define AFX_SETHC_DOOR_H__E4A1A6BF_9CEC_4240_9B94_B0958AF9F281__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSethc_doorApp:
// See sethc_door.cpp for the implementation of this class
//

class CSethc_doorApp : public CWinApp
{
public:
	CSethc_doorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSethc_doorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSethc_doorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETHC_DOOR_H__E4A1A6BF_9CEC_4240_9B94_B0958AF9F281__INCLUDED_)
