#ifndef __TCP_DEFINITION_HEADER__
#define __TCP_DEFINITION_HEADER__

#include <WinSock2.h>
#include <ws2tcpip.h>

typedef enum EAddressFamily_
{
	UNKNOWN = AF_UNSPEC,
	INET = AF_INET,
	INET6 = AF_INET6,
	IPX = AF_IPX,
	APPLETALK = AF_APPLETALK,
	NETBIOS = AF_NETBIOS,
	IRDA = AF_IRDA,
	BLUETOOTH = AF_BTH,
}EAddressFamily;

typedef enum ESocketType_
{
	STREAM = SOCK_STREAM,
	DGRAM = SOCK_DGRAM,
	RAW = SOCK_RAW,
	RDM = SOCK_RDM,
	SEQ = SOCK_SEQPACKET,
}ESocketType;

typedef enum EProtocol_
{
	HOPOPTS = IPPROTO_HOPOPTS,
	ICMP = IPPROTO_ICMP,
	IGMP = IPPROTO_IGMP,
	GGP = IPPROTO_GGP,
	ST = IPPROTO_ST,
	TCP = IPPROTO_TCP,
	CBT = IPPROTO_CBT,
	EGP = IPPROTO_EGP,
	IGP = IPPROTO_IGP,
	PUP = IPPROTO_PUP,
	IDP = IPPROTO_IDP,
	RDP = IPPROTO_RDP,
	IPV6 = IPPROTO_IPV6,
	ROUT = IPPROTO_ROUTING,
	FRAGMENT = IPPROTO_FRAGMENT,
	ESP = IPPROTO_ESP,
	AH = IPPROTO_AH,
	ICMP6 = IPPROTO_ICMPV6,
	NONE = IPPROTO_NONE,
	DSTOPTS = IPPROTO_DSTOPTS,
	ND = IPPROTO_ND,
	ICLFXBM = IPPROTO_ICLFXBM,
	PIM = IPPROTO_PIM,
	PGM = IPPROTO_PGM,
	UDP = IPPROTO_UDP,
	RM = IPPROTO_AH,
	L2TP = IPPROTO_L2TP,
	SCTP = IPPROTO_SCTP,
	RAWPROTO = IPPROTO_RAW
}EProtocol;

typedef enum ESocketRole_
{
	eUnspecified = 0,
	eClientRole,
	eServerRole,
}ESocketRole;

__interface ISocketManager;

__interface ISocket
{
	virtual SOCKET getHandle() = 0;
	virtual void setManager(ISocketManager*) = 0;
	virtual bool Close() = 0;
};

__interface ISocketManager
{
	virtual bool AddSocket(ISocket* pSock) = 0;
	virtual void SocketData(ISocket* pSocket, const BYTE* data) = 0;
};

#endif	//__TCP_DEFINITION_HEADER__
