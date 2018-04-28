#include "stdafx.h"
#include "TCPSocket.h"


DWORD WINAPI TCPListenProc(LPVOID lpParam)
{
	TCPSocket* pSocket = (TCPSocket*)lpParam;
	if(nullptr != pSocket){
		fd_set reads;
		fd_set writes;
		while (pSocket->isValidHandle())
		{
			select(0, &reads, &writes, nullptr, time);
		}

		return 0;
	}
	return 0;
}

DWORD WINAPI TCPReceiveProc(LPVOID lpParam)
{
	TCPSocket* pSocket = (TCPSocket*)lpParam;
	if(nullptr != pSocket){
		while (pSocket->isValidHandle())
		{
			TRACE0("Receiving...");
		}

		return 0;
	}
	return 0;
}


TCPSocket::TCPSocket(ISocketManager* pManager/* = nullptr*/)
{
	m_pManager = pManager;
	m_handle = INVALID_SOCKET;
	m_eAddrFamily = EAddressFamily::UNKNOWN;
	m_eType = ESocketType::RAW;
	m_eProtocol = EProtocol::TCP;
	m_hListenThread = nullptr;
	m_hReceiveThread = nullptr;
	m_eRole = ESocketRole::eUnspecified;
	memset(&m_stAddress, 0, sizeof(sockaddr_in));
}

TCPSocket::TCPSocket(SOCKET handle, ISocketManager* pManager, EAddressFamily eFamily, ESocketType eType, EProtocol eProto, ESocketRole eRole, const sockaddr_in* remoteAddress)
{
	m_pManager = pManager;
	m_eAddrFamily = eFamily;
	m_eType = eType;
	m_eProtocol = eProto;
	m_eRole = eRole;
	m_handle = handle;

	if(nullptr != remoteAddress) {
		memcpy(&m_stAddress, remoteAddress, sizeof(m_stAddress));
	}

	if(isValidHandle()) {
		//Start receive thread
		DWORD dwThreadId = 0;
		m_hReceiveThread = ::CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)TCPReceiveProc, this, CREATE_SUSPENDED, &dwThreadId);

		//Insert addition operation here

		//resume receive thread
		ResumeThread(m_hReceiveThread);
	}
}

TCPSocket::~TCPSocket(void)
{
	this->Close();
}

bool TCPSocket::setOption(long opt, unsigned long *val)
{
	if(!isValidHandle()) {
		return false;
	}

	int nErr = ioctlsocket(m_handle, opt, val);
	if(SOCKET_ERROR == nErr) {
		return false;
	}

	return true;
}

bool TCPSocket::setNonBlocking(bool bNonBlocking)
{
	unsigned long val = bNonBlocking?1:0;
	return this->setOption(FIONBIO, &val);
}

bool TCPSocket::Create(EAddressFamily eFamily, ESocketType eType, EProtocol eProto)
{
	m_handle = socket(static_cast<int>(eFamily), static_cast<int>(eType), static_cast<int>(eProto));

	if(INVALID_SOCKET != m_handle) {
		m_eAddrFamily = eFamily;
		m_eType = eType;
		m_eProtocol = eProto;

		return true;
	}

	return false;
}

bool TCPSocket::Bind(const char* strIPAddress, unsigned short uPortNo)
{
	if(!isValidHandle()){
		return false;
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_addr.s_addr = inet_addr(strIPAddress);
	addr.sin_family = static_cast<ADDRESS_FAMILY>( m_eAddrFamily );
	addr.sin_port = htons(uPortNo);
	
	int nErr = bind(m_handle, (SOCKADDR*)&addr, sizeof(addr));
	if(SOCKET_ERROR == nErr) {
		return false;
	}

	//Save bound address
	m_stAddress.sin_addr.s_addr = addr.sin_addr.s_addr;
	m_stAddress.sin_family = addr.sin_family;
	m_stAddress.sin_port = addr.sin_port;

	return true;
}

bool TCPSocket::Listen(int nMaxConn/* = SOMAXCONN*/)
{
	if(ESocketRole::eUnspecified != getRole()) {
		//Already connected or listened
		return false;
	}

	
	if(!isValidHandle()) {
		return false;
	}

	int nErr = listen(m_handle, nMaxConn);
	if(SOCKET_ERROR == nErr) {
		return false;
	}

	//Start listen thread
	DWORD dwThreadId = 0;
	m_hListenThread = ::CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)TCPListenProc, this, CREATE_SUSPENDED, &dwThreadId);

	m_eRole = ESocketRole::eServerRole;

	::ResumeThread(m_hListenThread);

	return true;
}

bool TCPSocket::Connect(const char* strRemoteIP, unsigned short uRemotePort)
{
	if(ESocketRole::eUnspecified != getRole()) {
		//Already connected or listened
		return false;
	}

	if(!isValidHandle()) {
		return false;
	}

	sockaddr_in remoteAddr;
	remoteAddr.sin_family = static_cast<ADDRESS_FAMILY>( getFamily() );
	remoteAddr.sin_addr.s_addr = inet_addr(strRemoteIP);
	remoteAddr.sin_port = htons(uRemotePort);

	int nErr = connect(m_handle, (SOCKADDR*)&remoteAddr, sizeof(remoteAddr));
	if(SOCKET_ERROR == nErr) {
		return false;
	}

	//Save connected address
	m_stAddress.sin_addr.s_addr = remoteAddr.sin_addr.s_addr;
	m_stAddress.sin_port = remoteAddr.sin_port;

	DWORD dwThreadId = 0;
	m_hReceiveThread = ::CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)TCPReceiveProc, this, CREATE_SUSPENDED, &dwThreadId);

	m_eRole = ESocketRole::eClientRole;

	ResumeThread(m_hReceiveThread);

	return true;
}

TCPSocket* TCPSocket::Accept()
{
	if(!isValidHandle()) {
		return nullptr;
	}

	sockaddr_in incoming_addr;
	int addrlen = sizeof(incoming_addr);

	SOCKET sock = accept(m_handle, (SOCKADDR*)&incoming_addr, &addrlen);
	if(INVALID_SOCKET == sock) {
		return nullptr;
	}

	if(!OnAccepting(sock, &incoming_addr)) {
		closesocket(sock);
		return nullptr;
	}

	TCPSocket* newSocket = new TCPSocket(sock, m_pManager, m_eAddrFamily, m_eType, m_eProtocol, m_eRole, &incoming_addr);
	
	if(nullptr != m_pManager){
		if(!m_pManager->AddSocket(newSocket)) {
			delete newSocket;
			newSocket = nullptr;
			return nullptr;
		}
	}

	return newSocket;
}

bool TCPSocket::Close()
{
	if(!isValidHandle()) {
		return true;
	}

	OnClosing();

	int nErr = closesocket(m_handle);
	if(SOCKET_ERROR == nErr) {
		return false;
	}

	m_handle = INVALID_SOCKET;

	if(nullptr != m_hListenThread) {
		WaitForSingleObject(m_hListenThread, INFINITE);
		CloseHandle(m_hListenThread);
		m_hListenThread = nullptr;
	}

	if(nullptr != m_hReceiveThread) {
		WaitForSingleObject(m_hReceiveThread, INFINITE);
		CloseHandle(m_hReceiveThread);
		m_hReceiveThread = nullptr;
	}

	OnClosed();

	return true;
}