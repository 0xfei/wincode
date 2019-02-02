#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./Inc/pcap.h"

#pragma comment(lib,"./Lib/wpcap.lib")
#pragma comment(lib,"ws2_32.lib")

#define ETH_IP       0x0800//0x0008
#define ETH_ARP      0x0806//0x0608
#define ARP_REQUEST  0x0001//0x0100
#define ARP_REPLY    0x0002//0x0200
#define ARP_HARDWARE 0x0001//0x0100

#pragma pack(1)

typedef struct _ETHHDR
{
	unsigned char dst_mac[6];
	unsigned char src_mac[6];
	unsigned short eth_t;
} ETHHDR,*PETHHDR;

typedef struct arphdr
{
    unsigned short  arp_hdr;
    unsigned short  arp_pro;
    unsigned char   arp_hln;
    unsigned char   arp_pln;
    unsigned short  arp_opt;
    unsigned char   arp_sha[6];
    unsigned long   arp_spa;
    unsigned char   arp_tha[6];
    unsigned long   arp_tpa;
} ARPHDR,*PARPHDR;

typedef struct ARPPACK
{
	ETHHDR eth;
	ARPHDR arp;
} ARPPACK,*PARPPACK;

#pragma pack()


char errbuf[2048];			//错误缓冲区
pcap_if_t* device;			//设备名
pcap_if_t* alldev;			//全部设备
pcap_t*	ahandle;			//设备句柄
ARPPACK arp;
/*
char DestIP[100]={0};		//目标IP
char SurcIP[100]={0};		//源IP
char DestHard[6]={0};		//目标MAC;
char SurcHard[6]={0};		//源MAC;

char ethDest[6]={0};		//实际目标
char ethSrc[6]={0};			//实际源
*/
int Ip = 0;

void InitDevice();
void FreeDevice();
void InitPacket(char *DIP,char *DMac,char *SIP,char *SMac);
void WriteHard(unsigned char *Hard,char* Src);
char Hex(char Src);