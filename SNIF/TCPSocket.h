#pragma once

#include "TCPObject.h"

class TCPSocket : public TCPObject,  public ISocket
{
private:
	ISocketManager* m_pManager;
	SOCKET	m_handle;
	HANDLE m_hListenThread;
	HANDLE m_hReceiveThread;
	EAddressFamily m_eAddrFamily;
	ESocketType m_eType;
	EProtocol m_eProtocol;
	sockaddr_in m_stAddress;
	ESocketRole m_eRole;
public:
	TCPSocket(ISocketManager* pManager = nullptr);
	virtual ~TCPSocket(void);
public:
	inline SOCKET getHandle(){ return m_handle; }
	inline EAddressFamily getFamily() { return m_eAddrFamily; }
	inline ESocketType getType() { return m_eType; }
	inline EProtocol getProtocol() { return m_eProtocol; }
	inline const char* getIP() { if(!isValidHandle()) return nullptr; return  (char*)&m_stAddress.sin_addr.s_addr; }
	inline const unsigned short getPort() { if(!isValidHandle()) return 0; return m_stAddress.sin_port; }
	inline bool isValidHandle() { return INVALID_SOCKET != m_handle; }
	inline ESocketRole getRole() { return m_eRole; }
	inline void setManager(ISocketManager* pManager) { m_pManager = pManager; }
public:
	bool Create(EAddressFamily eFamily, ESocketType eType, EProtocol eProto);
	bool Bind(const char* strIPAddress, unsigned short uPortNo);
	bool Connect(const char* strRemoteIP, unsigned short uRemotePort);
	bool Listen(int nMaxConn = SOMAXCONN);
	TCPSocket* Accept();
	bool Close();
	bool setOption(long opt, unsigned long *val);
	bool setNonBlocking(bool bNonBlocking);
protected:
	virtual bool OnAccepting(SOCKET incomingSocket, sockaddr_in *address) { return true; }
	virtual void OnAccepted(){ ; }
	virtual void OnClosing() { ; }
	virtual void OnClosed() { ; }
private:
	TCPSocket(SOCKET handle, ISocketManager* pManager, EAddressFamily eFamily, ESocketType eType, EProtocol eProto, ESocketRole eRole, const sockaddr_in* remoteAddress);
};

