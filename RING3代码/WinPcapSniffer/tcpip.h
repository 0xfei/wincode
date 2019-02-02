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
	u_char 		ver_ihl;  			//4位首部长度,4位IP版本号 
	u_char 		tos;	  				//8位服务类型TOS 
	u_short 	     tlen;	  			//16位总长度（字节） 
	u_short 	     identification; 		//16位标识 
	u_short 	     flags_fo;			//3位标志位,13位偏移
	u_char 		ttl;	 				//8位生存时间 TTL 
	u_char        proto;				//8位协议 (TCP, UDP 或其他) 
	u_short 	     crc;	   				//16位IP首部校验和 
	ip_address 	saddr;				//32位源IP地址 
	ip_address 	daddr; 				//32位目的IP地址
	u_int 		op_pad;	 			//32位选项
}ip_header;


typedef struct tcp_header 
{
	u_short			sport;		//16位源端口 
	u_short			dport;		//16位目的端口 
	u_int			seq;			//32位序列号 
	u_int			ack;			//32位确认号 
	u_char			lenres;		//4位首部长度/6位保留字 
	u_char			flag;		//6位标志位 
	u_short			win;			//16位窗口大小 
	u_short		     sum;			//16位校验和 
	u_short			urp;			//16位紧急数据偏移量
	u_int 			op_pad;	 	//32位选项
}tcp_header;


typedef struct udp_header
{
	u_short sport;   	//16位源端口
	u_short dport;   	//16位目的端口
	u_short len;		//16位长度 
	u_short crc;		//16位校验和
}udp_header;
