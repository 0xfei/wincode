#include <windows.h>
#include <cstdio>

int LookUp()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken)) {
		return -1;
		printf("ddddddddd");
	}
	LookupPrivilegeValue(NULL, SE_BACKUP_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes |= SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
}

int main()
{
	HKEY key;
	LPCTSTR data = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion";
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, data, 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
		/*
		*/
	}

	DWORD dwIndex = 0, NameSize, NameCnt, NameMaxLen, Type;
	DWORD KeySize, KeyCnt, KeyMaxLen, DataSize, MaxDataLen;
	
	if (RegQueryInfoKey(key, NULL, NULL, NULL,
		&KeyCnt, &KeyMaxLen, NULL, &NameCnt, &NameMaxLen, &MaxDataLen,
		NULL,NULL) == ERROR_SUCCESS) {
	}

	for (dwIndex = 0; dwIndex < KeyCnt; dwIndex++) {
		KeySize = KeyMaxLen + 1;
		char *szKeyName = (char*)malloc(KeySize);
		RegEnumKey(key, dwIndex, szKeyName, KeySize);
		printf("%s\n",szKeyName);
	}

	for (dwIndex = 0; dwIndex < NameCnt; dwIndex++) {
		DataSize = MaxDataLen + 1;
		NameSize = NameMaxLen + 1;
		char* szValueName = (char*)malloc(NameSize);
		unsigned char* szValueData = (unsigned char*)malloc(DataSize);
		RegEnumValue(key, dwIndex, szValueName, &NameSize, NULL, &Type, szValueData, &DataSize);

	}

	RegCreateKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\test", NULL);
	RegDeleteKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\test");

	RegSetValueEx(key, "test", 0, REG_DWORD, (const unsigned char*)10, 4);
	RegDeleteValue(key, "test");

	LookUp();

	RegSaveKey(HKEY_CURRENT_USER, "C:\\TEST.KEY", NULL);
	//RegRestoreKey(HKEY_CURRENT_USER, "C:\\TEST.KEY", NULL);
	RegCloseKey(key);
	return 0;
}
