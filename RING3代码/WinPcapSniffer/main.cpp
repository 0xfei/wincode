#include <pcap.h>
#include "tcpip.h"
#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "ws2_32.lib")

int main(int argc, char **argv)
{
	pcap_if_t* alldevs;
	pcap_if_t* d;
	int inum;
	int i = 0;
	pcap_t* fp;
	char *ofilename = "sniffer.txt";
	char errbuf[PCAP_ERRBUF_SIZE];

	u_int netmask;
	int res;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;

	char packet_filter[] = "ip and tcp and udp";
	pcap_dumper_t *dumpfile;
	struct bpf_program fcode;

	ip_header* ih;
	tcp_header* th;
	udp_header* uh;
	u_int ip_len;
	u_short sport, dport;

	// 获得网络设备指针
	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		fprintf(stderr, "pcap_findalldevs出错: %s\n", errbuf);
		exit(1);
	}

	// 枚举网卡信息
	for (d = alldevs; d; d = d->next)
	{
		printf("%d. %s", ++ i, d->name);
		if (d->description)
		{
			printf(" (%s)\n", d->description);
		}
		else
		{
			printf(" (无描述信息)\n");
		}
	}

	if (i == 0)
	{
		printf("\n没发现任何接口，请确认是否已经安装WinPcap库.\n");
		return -1;
	}

	printf("请输入网卡接口号 (1 - %d):", i);
	scanf("%d", &inum);

	if (inum < 1 || inum > i)
	{
		printf("\n接口号超出范围.\n");
		// 释放alldevs资源
		pcap_freealldevs(alldevs);
		return -1;
	}

	// 跳到选择的网卡
	for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++);

	// 打开网卡
	if ((fp = pcap_open_live(d->name,  	// 设备名称
	65536,	   
	0, 		// 混杂模式
	1000,	   					// 远程认证，本程序因是本地嗅探，不需要设置
	errbuf	   
	)) == NULL)
	{
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by Winpcap\n");
		// 释放alldevs资源
		pcap_freealldevs(alldevs);
		return -1;
	}

	// 检查链路层，本程序只简单的支持以太网
	if (pcap_datalink(fp) != DLT_EN10MB)
	{
		fprintf(stderr, "\n本程序只简单的支持以太网.\n");
		// 释放alldevs资源
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (d->addresses != NULL)
	{
		// 取得一个网络接口的子网掩码
		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	}
	else
	{
		netmask = 0xffffffff;
	}

	// 编译过滤器
	if (pcap_compile(fp, &fcode, packet_filter, 1, netmask) < 0)
	{
		fprintf(stderr, "\nU不能编译包过滤，请检查正则表达式.\n");
		// 释放alldevs资源
		pcap_freealldevs(alldevs);
		return -1;
	}

	// 设置过滤器
	if (pcap_setfilter(fp, &fcode) < 0)
	{
		fprintf(stderr, "\n设置过滤器失败.\n");
		// 释放alldevs资源
		pcap_freealldevs(alldevs);
		return -1;
	}

	printf("\nlistening on %s ...\n", d->description);

	// 释放alldevs资源，因为不再需要它
	pcap_freealldevs(alldevs);

	// 打开输出文件
	dumpfile= pcap_dump_open(fp, ofilename);
	if (dumpfile == NULL)
	{
		fprintf(stderr,"\n打开输出文件错误\n");
		pcap_close(fp);
		return -1;
	}

	// 开始捕获数据包
	while((res = pcap_next_ex( fp, &header, &pkt_data)) >= 0)
	{
		if(res == 0)
		/* 超时继续 */
		continue;

		// 将捕获的数据包存入文件
		pcap_dump((unsigned char *) dumpfile, header, pkt_data);

		// 取得IP头，14为以太网头长度
		ih = (ip_header*)(pkt_data + 14);
		ip_len = (ih->ver_ihl & 0xf) * 4;

		if (ih->proto == 6)	//TCP 
		{
			//取TCP头
			th = (tcp_header*)((u_char*)ih + ip_len);

			/* convert from network byte order to host byte order */
			sport = ntohs(th->sport);	//网络字节序转主机字节序
			dport = ntohs(th->dport);	//网络字节序转主机字节序

			/* 打印IP地址和端口号 */
			printf("%d.%d.%d.%d:%d-> %d.%d.%d.%d:%d\n",
			ih->saddr.byte1,
			ih->saddr.byte2,
			ih->saddr.byte3,
			ih->saddr.byte4,
			sport,
			ih->daddr.byte1,
			ih->daddr.byte2,
			ih->daddr.byte3,
			ih->daddr.byte4,
			dport);	
		}
		else if (ih->proto == 17)	//UDP
		{
			//取UDP头
			uh = (udp_header*)((u_char*)ih + ip_len);

			/* convert from network byte order to host byte order */
			sport = ntohs(uh->sport);	//网络字节序转主机字节序
			dport = ntohs(uh->dport);	//网络字节序转主机字节序

			/* 打印IP地址和端口号 */
			printf("%d.%d.%d.%d:%d-> %d.%d.%d.%d:%d\n",
			ih->saddr.byte1,
			ih->saddr.byte2,
			ih->saddr.byte3,
			ih->saddr.byte4,
			sport,
			ih->daddr.byte1,
			ih->daddr.byte2,
			ih->daddr.byte3,
			ih->daddr.byte4,
			dport);	
		}
	}

	pcap_close(fp);
	pcap_dump_close(dumpfile);

	return 0;
}
