#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

BOOL HttpGetFile(char url_main[],char url_last[], char savepath[MAX_PATH])
{
	HMODULE hDll;
	LPVOID hSession, hConnect, hHttpFile;
	hDll = LoadLibrary("wininet.dll");
	if (!hDll) return FALSE;

	typedef LPVOID (WINAPI *pInternetOpen)(LPCTSTR,DWORD,LPCSTR,LPCTSTR,DWORD);
	typedef BOOL (WINAPI *pInternetCloseHandle)(LPVOID);
	typedef BOOL (WINAPI *pInternetReadFile)(LPVOID,LPVOID,DWORD,LPDWORD);
	typedef BOOL (WINAPI *pInternetQueryInfo)(LPVOID,DWORD,LPVOID,LPDWORD,LPDWORD);
	typedef LPVOID (WINAPI *pHttpQueryInfo)(LPVOID,DWORD,LPVOID,LPDWORD,LPDWORD);
	typedef LPVOID (WINAPI *pHttpOpenRequest)(LPVOID,
		LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR,DWORD,
		DWORD);
	typedef BOOL (WINAPI *pHttpSendRequest)(LPVOID,LPCTSTR,DWORD,LPVOID,DWORD);
	typedef LPVOID (WINAPI *pInternetConnect)(LPVOID,
		LPCTSTR,char,LPCTSTR,LPCTSTR,LPCTSTR,DWORD,
		DWORD);

	pInternetOpen InternetOpen = (pInternetOpen)GetProcAddress(hDll, "InternetOpenA");
	pInternetCloseHandle InternetCloseHandle = (pInternetCloseHandle)GetProcAddress(hDll, "InternetCloseHandle");
	pInternetReadFile InternetReadFile = (pInternetReadFile)GetProcAddress(hDll, "InternetReadFile");
	pInternetQueryInfo InternetQueryInfo = (pInternetQueryInfo)GetProcAddress(hDll, "InternetQueryInfoA");
	pHttpQueryInfo HttpQueryInfo = (pHttpQueryInfo)GetProcAddress(hDll, "HttpQueryInfoA");
	pHttpOpenRequest HttpOpenRequest = (pHttpOpenRequest)GetProcAddress(hDll, "HttpOpenRequestA");
	pHttpSendRequest HttpSendRequest = (pHttpSendRequest)GetProcAddress(hDll, "HttpSendRequestA");
	pInternetConnect InternetConnect = (pInternetConnect)GetProcAddress(hDll, "InternetConnectA");

	hSession = InternetOpen("lyyer", 0, NULL, NULL, 0);
	if (hSession == NULL) return FALSE;

	hConnect = InternetConnect(hSession, url_main, 0, NULL, NULL, 0, 0, 0);
	if (hConnect == NULL) return FALSE;

	return TRUE;
}

int main()
{
	return 0;
}
