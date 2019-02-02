#pragma once
typedef struct ip_address
{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;


typedef struct ip_header
{
	u_char 		ver_ihl;  			//4λ�ײ�����,4λIP�汾�� 
	u_char 		tos;	  				//8λ��������TOS 
	u_short 	     tlen;	  			//16λ�ܳ��ȣ��ֽڣ� 
	u_short 	     identification; 		//16λ��ʶ 
	u_short 	     flags_fo;			//3λ��־λ,13λƫ��
	u_char 		ttl;	 				//8λ����ʱ�� TTL 
	u_char        proto;				//8λЭ�� (TCP, UDP ������) 
	u_short 	     crc;	   				//16λIP�ײ�У��� 
	ip_address 	saddr;				//32λԴIP��ַ 
	ip_address 	daddr; 				//32λĿ��IP��ַ
	u_int 		op_pad;	 			//32λѡ��
}ip_header;


typedef struct tcp_header 
{
	u_short			sport;		//16λԴ�˿� 
	u_short			dport;		//16λĿ�Ķ˿� 
	u_int			seq;			//32λ���к� 
	u_int			ack;			//32λȷ�Ϻ� 
	u_char			lenres;		//4λ�ײ�����/6λ������ 
	u_char			flag;		//6λ��־λ 
	u_short			win;			//16λ���ڴ�С 
	u_short		     sum;			//16λУ��� 
	u_short			urp;			//16λ��������ƫ����
	u_int 			op_pad;	 	//32λѡ��
}tcp_header;


typedef struct udp_header
{
	u_short sport;   	//16λԴ�˿�
	u_short dport;   	//16λĿ�Ķ˿�
	u_short len;		//16λ���� 
	u_short crc;		//16λУ���
}udp_header;
