#pragma once

#include <iphlpapi.h>
#include "TCPObject.h"

class CNetworkInterfaces : public TCPObject
{
private:
	DWORD m_dwTypeFilter;
	MIB_IFTABLE *m_pIfTable;
	MIB_IPADDRTABLE *m_pAddrTable;
	CMap<SIZE_T,SIZE_T,MIB_IFROW*,MIB_IFROW*> m_arrNICs;
	CMap<SIZE_T,SIZE_T,MIB_IPADDRROW*,MIB_IPADDRROW*> m_arrIPs;
public:
	CNetworkInterfaces(DWORD dwTypeFilter = 0);
	virtual ~CNetworkInterfaces(void);
	void Refresh(bool bUpdate = true);
	SIZE_T getCount() { return m_arrNICs.GetCount(); }
	CString getInterfaceName(SIZE_T index);
	CString getMACAddress(SIZE_T index);
	CString getIPAddress(SIZE_T index);
	DWORD getType(SIZE_T index);
	CString getTypeName(SIZE_T index);
};

