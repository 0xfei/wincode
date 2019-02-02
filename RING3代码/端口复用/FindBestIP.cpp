#include <windows.h>
#include <Iprtrmib.h>
#include <stdio.h>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#define DEFAULT_DESTINATION "115.239.210.26"

DWORD GetBestIP()
{
	PMIB_IPADDRTABLE pAddrTable = NULL;
	PMIB_IPADDRROW   pAddrRow = NULL;
	ULONG ulSize = 0;
	DWORD ret = -1;
	char* pBuffer = NULL;
	DWORD dwBestIndex = -1;
	DWORD dwResult;
	
	dwResult = GetBestInterface(inet_addr(DEFAULT_DESTINATION), &dwBestIndex);
	if (dwResult != NO_ERROR)
		return ret;

	GetIpAddrTable((PMIB_IPADDRTABLE)(char*)pBuffer, &ulSize, TRUE);
	pBuffer = new char[ulSize];
	dwResult = GetIpAddrTable((PMIB_IPADDRTABLE)(char*)pBuffer, &ulSize, TRUE);

	if (dwResult == NO_ERROR)
	{
		pAddrTable = (PMIB_IPADDRTABLE)(char*)pBuffer;
		for (DWORD x = 0; x < pAddrTable->dwNumEntries; x++)
		{
			pAddrRow = (PMIB_IPADDRROW)&(pAddrTable->table[x]);
			if (pAddrRow->dwIndex == dwBestIndex)
			{
				ret = pAddrRow->dwAddr;
				delete [] pBuffer;
				return ret;
			}
		}
	}
	delete [] pBuffer;
	return -1;
}


int main()
{
	DWORD ip = GetBestIP();
	in_addr a;
	a.S_un.S_addr =ip;
	printf("%s\n",inet_ntoa(a));
	return 0;
}
