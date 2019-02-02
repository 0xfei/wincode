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

	// ��������豸ָ��
	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		fprintf(stderr, "pcap_findalldevs����: %s\n", errbuf);
		exit(1);
	}

	// ö��������Ϣ
	for (d = alldevs; d; d = d->next)
	{
		printf("%d. %s", ++ i, d->name);
		if (d->description)
		{
			printf(" (%s)\n", d->description);
		}
		else
		{
			printf(" (��������Ϣ)\n");
		}
	}

	if (i == 0)
	{
		printf("\nû�����κνӿڣ���ȷ���Ƿ��Ѿ���װWinPcap��.\n");
		return -1;
	}

	printf("�����������ӿں� (1 - %d):", i);
	scanf("%d", &inum);

	if (inum < 1 || inum > i)
	{
		printf("\n�ӿںų�����Χ.\n");
		// �ͷ�alldevs��Դ
		pcap_freealldevs(alldevs);
		return -1;
	}

	// ����ѡ�������
	for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++);

	// ������
	if ((fp = pcap_open_live(d->name,  	// �豸����
	65536,	   
	0, 		// ����ģʽ
	1000,	   					// Զ����֤�����������Ǳ�����̽������Ҫ����
	errbuf	   
	)) == NULL)
	{
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by Winpcap\n");
		// �ͷ�alldevs��Դ
		pcap_freealldevs(alldevs);
		return -1;
	}

	// �����·�㣬������ֻ�򵥵�֧����̫��
	if (pcap_datalink(fp) != DLT_EN10MB)
	{
		fprintf(stderr, "\n������ֻ�򵥵�֧����̫��.\n");
		// �ͷ�alldevs��Դ
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (d->addresses != NULL)
	{
		// ȡ��һ������ӿڵ���������
		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	}
	else
	{
		netmask = 0xffffffff;
	}

	// ���������
	if (pcap_compile(fp, &fcode, packet_filter, 1, netmask) < 0)
	{
		fprintf(stderr, "\nU���ܱ�������ˣ�����������ʽ.\n");
		// �ͷ�alldevs��Դ
		pcap_freealldevs(alldevs);
		return -1;
	}

	// ���ù�����
	if (pcap_setfilter(fp, &fcode) < 0)
	{
		fprintf(stderr, "\n���ù�����ʧ��.\n");
		// �ͷ�alldevs��Դ
		pcap_freealldevs(alldevs);
		return -1;
	}

	printf("\nlistening on %s ...\n", d->description);

	// �ͷ�alldevs��Դ����Ϊ������Ҫ��
	pcap_freealldevs(alldevs);

	// ������ļ�
	dumpfile= pcap_dump_open(fp, ofilename);
	if (dumpfile == NULL)
	{
		fprintf(stderr,"\n������ļ�����\n");
		pcap_close(fp);
		return -1;
	}

	// ��ʼ�������ݰ�
	while((res = pcap_next_ex( fp, &header, &pkt_data)) >= 0)
	{
		if(res == 0)
		/* ��ʱ���� */
		continue;

		// ����������ݰ������ļ�
		pcap_dump((unsigned char *) dumpfile, header, pkt_data);

		// ȡ��IPͷ��14Ϊ��̫��ͷ����
		ih = (ip_header*)(pkt_data + 14);
		ip_len = (ih->ver_ihl & 0xf) * 4;

		if (ih->proto == 6)	//TCP 
		{
			//ȡTCPͷ
			th = (tcp_header*)((u_char*)ih + ip_len);

			/* convert from network byte order to host byte order */
			sport = ntohs(th->sport);	//�����ֽ���ת�����ֽ���
			dport = ntohs(th->dport);	//�����ֽ���ת�����ֽ���

			/* ��ӡIP��ַ�Ͷ˿ں� */
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
			//ȡUDPͷ
			uh = (udp_header*)((u_char*)ih + ip_len);

			/* convert from network byte order to host byte order */
			sport = ntohs(uh->sport);	//�����ֽ���ת�����ֽ���
			dport = ntohs(uh->dport);	//�����ֽ���ת�����ֽ���

			/* ��ӡIP��ַ�Ͷ˿ں� */
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
