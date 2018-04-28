#include "stdafx.h"
#include "NetworkInterfaces.h"

#pragma comment(lib, "IPHLPAPI.lib")

CNetworkInterfaces::CNetworkInterfaces(DWORD dwTypeFiler/* = 0*/)
{
	m_dwTypeFilter = dwTypeFiler;
	m_pIfTable = nullptr;
	m_pAddrTable = nullptr;

	Refresh();
}


CNetworkInterfaces::~CNetworkInterfaces(void)
{
	Refresh(false);
}


void CNetworkInterfaces::Refresh(bool bUpdate /* = true */)
{
	m_arrIPs.RemoveAll();
	m_arrNICs.RemoveAll();
	HANDLE hHeap = GetProcessHeap();
	if(nullptr != m_pIfTable) {
		HeapFree(hHeap, 0, m_pIfTable);
		m_pIfTable = nullptr;
	}

	if(nullptr != m_pAddrTable) {
		HeapFree(hHeap, 0, m_pAddrTable);
		m_pAddrTable = nullptr;
	}

	if(bUpdate){
		ULONG uSize= sizeof(MIB_IFTABLE);
		m_pIfTable = (MIB_IFTABLE *)HeapAlloc(hHeap, 0, uSize);
		MIB_IFROW *pIfRow = nullptr;

		if(GetIfTable(m_pIfTable, &uSize, TRUE) == ERROR_INSUFFICIENT_BUFFER) {
			HeapFree(hHeap, 0, m_pIfTable);
			m_pIfTable = (MIB_IFTABLE *)HeapAlloc(hHeap, 0, uSize);
		}

		if(nullptr != m_pIfTable) {
			int index = 0;
			if(GetIfTable(m_pIfTable, &uSize, TRUE) == NO_ERROR) {
				for(DWORD i=0;i<m_pIfTable->dwNumEntries;i++) {
					pIfRow = (MIB_IFROW *) &m_pIfTable->table[i];
					if( (0 == m_dwTypeFilter) || (pIfRow->dwType == m_dwTypeFilter) ) {
						index = m_arrNICs.GetCount();
						m_arrNICs.SetAt(index, pIfRow);
					}
				}
			}
		}

		uSize = sizeof(MIB_IPADDRTABLE);
		MIB_IPADDRTABLE *pAddrTable = (MIB_IPADDRTABLE*)HeapAlloc(hHeap, 0, uSize);
		MIB_IPADDRROW *pAddrRow = nullptr;

		if(GetIpAddrTable(pAddrTable, &uSize, TRUE) == ERROR_INSUFFICIENT_BUFFER) {
			HeapFree(hHeap, 0, pAddrTable);
			pAddrTable = (MIB_IPADDRTABLE*)HeapAlloc(hHeap, 0, uSize);
		}

		if(nullptr != pAddrTable) {
			if(GetIpAddrTable(pAddrTable, &uSize, TRUE) == NO_ERROR) {
				for(DWORD j=0;j<pAddrTable->dwNumEntries;j++){
					pAddrRow = &pAddrTable->table[j];
					m_arrIPs.SetAt(pAddrRow->dwIndex,pAddrRow);
				}
			}
		}
	}
}

CString CNetworkInterfaces::getInterfaceName(SIZE_T index)
{
	if( (index < 0) || (index > (SIZE_T)m_arrNICs.GetCount()) ) {
		return CString();
	}

	MIB_IFROW* pIfRow = m_arrNICs[index];
	if(nullptr != pIfRow) {
		return CString(pIfRow->bDescr);
	}

	return CString();
}

CString CNetworkInterfaces::getMACAddress(SIZE_T index)
{
	if( (index < 0) || (index > (SIZE_T)m_arrNICs.GetCount()) ) {
		return CString();
	}

	MIB_IFROW* pIfRow = m_arrNICs[index];
	if(nullptr != pIfRow) {
		CString str;
		CString strMAC;
		for(DWORD i=0;i<pIfRow->dwPhysAddrLen;i++) {
			if(i == (pIfRow->dwPhysAddrLen-1))
				str.Format(_T("%.2X"), (int)pIfRow->bPhysAddr[i]);
			else
				str.Format(_T("%.2X-"), (int)pIfRow->bPhysAddr[i]);
			strMAC.Append(str);
		}
		return strMAC;
	}

	return CString();
}

CString CNetworkInterfaces::getIPAddress(SIZE_T index)
{
	if( (index < 0) || (index > (SIZE_T)m_arrNICs.GetCount()) ) {
		return CString();
	}

	MIB_IFROW* pIfRow = m_arrNICs[index];
	if(nullptr != pIfRow) {
		MIB_IPADDRROW *pAddRow = m_arrIPs[pIfRow->dwIndex];
		if(nullptr != pAddRow) {
			in_addr ip;
			ip.S_un.S_addr = (ULONG)pAddRow->dwAddr;
			return CString(inet_ntoa(ip));
		}
	}

	return CString();
}

DWORD CNetworkInterfaces::getType(SIZE_T index)
{
	if( (index < 0) || (index > (SIZE_T)m_arrNICs.GetCount()) ) {
		return 0;
	}

	MIB_IFROW* pIfRow = m_arrNICs[index];
	if(nullptr != pIfRow) {
		return pIfRow->dwType;
	}

	return 0;
}

CString CNetworkInterfaces::getTypeName(SIZE_T index)
{
	DWORD dwType = getType(index);

	switch (dwType)
	{
	case IF_TYPE_OTHER:
		return CString(_T("IF_TYPE_OTHER"));
	case IF_TYPE_REGULAR_1822:
		return CString(_T("IF_TYPE_REGULAR_1822"));
	case IF_TYPE_HDH_1822:
		return CString(_T("IF_TYPE_HDH_1822"));
	case IF_TYPE_DDN_X25:
		return CString(_T("IF_TYPE_DDN_X25"));
	case IF_TYPE_RFC877_X25:
		return CString(_T("IF_TYPE_RFC877_X25"));
	case IF_TYPE_ETHERNET_CSMACD:
		return CString(_T("IF_TYPE_ETHERNET_CSMACD"));
	case IF_TYPE_IS088023_CSMACD:
		return CString(_T("IF_TYPE_IS088023_CSMACD"));
	case IF_TYPE_ISO88024_TOKENBUS:
		return CString(_T("IF_TYPE_ISO88024_TOKENBUS"));
	case IF_TYPE_ISO88025_TOKENRING:
		return CString(_T("IF_TYPE_ISO88025_TOKENRING"));
	case IF_TYPE_ISO88026_MAN:
		return CString(_T("IF_TYPE_ISO88026_MAN"));
	case IF_TYPE_STARLAN:
		return CString(_T("IF_TYPE_STARLAN"));
	case IF_TYPE_PROTEON_10MBIT:
		return CString(_T("IF_TYPE_PROTEON_10MBIT"));
	case IF_TYPE_PROTEON_80MBIT:
		return CString(_T("IF_TYPE_PROTEON_80MBIT"));
	case IF_TYPE_HYPERCHANNEL:
		return CString(_T("IF_TYPE_HYPERCHANNEL"));
	case IF_TYPE_FDDI:
		return CString(_T("IF_TYPE_FDDI"));
	case IF_TYPE_LAP_B:
		return CString(_T("IF_TYPE_LAP_B"));
	case IF_TYPE_SDLC:
		return CString(_T("IF_TYPE_SDLC"));
	case IF_TYPE_DS1:
		return CString(_T("IF_TYPE_DS1"));
	case IF_TYPE_E1:
		return CString(_T("IF_TYPE_E1"));
	case IF_TYPE_BASIC_ISDN:
		return CString(_T("IF_TYPE_BASIC_ISDN"));
	case IF_TYPE_PRIMARY_ISDN:
		return CString(_T("IF_TYPE_PRIMARY_ISDN"));
	case IF_TYPE_PROP_POINT2POINT_SERIAL:
		return CString(_T("IF_TYPE_PROP_POINT2POINT_SERIAL"));
	case IF_TYPE_PPP:
		return CString(_T("IF_TYPE_PPP"));
	case IF_TYPE_SOFTWARE_LOOPBACK:
		return CString(_T("IF_TYPE_SOFTWARE_LOOPBACK"));
	case IF_TYPE_EON:
		return CString(_T("IF_TYPE_EON"));
	case IF_TYPE_ETHERNET_3MBIT:
		return CString(_T("IF_TYPE_ETHERNET_3MBIT"));
	case IF_TYPE_NSIP:
		return CString(_T("IF_TYPE_NSIP"));
	case IF_TYPE_SLIP:
		return CString(_T("IF_TYPE_SLIP"));
	case IF_TYPE_ULTRA:
		return CString(_T("IF_TYPE_ULTRA"));
	case IF_TYPE_DS3:
		return CString(_T("IF_TYPE_DS3"));
	case IF_TYPE_SIP:
		return CString(_T("IF_TYPE_SIP"));
	case IF_TYPE_FRAMERELAY:
		return CString(_T("IF_TYPE_FRAMERELAY"));
	case IF_TYPE_RS232:
		return CString(_T("IF_TYPE_RS232"));
	case IF_TYPE_PARA:
		return CString(_T("IF_TYPE_PARA"));
	case IF_TYPE_ARCNET:
		return CString(_T("IF_TYPE_ARCNET"));
	case IF_TYPE_ARCNET_PLUS:
		return CString(_T("IF_TYPE_ARCNET_PLUS"));
	case IF_TYPE_ATM:
		return CString(_T("IF_TYPE_ATM"));
	case IF_TYPE_MIO_X25:
		return CString(_T("IF_TYPE_MIO_X25"));
	case IF_TYPE_SONET:
		return CString(_T("IF_TYPE_SONET"));
	case IF_TYPE_X25_PLE:
		return CString(_T("IF_TYPE_X25_PLE"));
	case IF_TYPE_ISO88022_LLC:
		return CString(_T("IF_TYPE_ISO88022_LLC"));
	case IF_TYPE_LOCALTALK:
		return CString(_T("IF_TYPE_LOCALTALK"));
	case IF_TYPE_SMDS_DXI:
		return CString(_T("IF_TYPE_SMDS_DXI"));
	case IF_TYPE_FRAMERELAY_SERVICE:
		return CString(_T("IF_TYPE_FRAMERELAY_SERVICE"));
	case IF_TYPE_HSSI:
		return CString(_T("IF_TYPE_HSSI"));
	case IF_TYPE_HIPPI:
		return CString(_T("IF_TYPE_HIPPI"));
	case IF_TYPE_MODEM:
		return CString(_T("IF_TYPE_MODEM"));
	case IF_TYPE_AAL5:
		return CString(_T("IF_TYPE_AAL5"));
	case IF_TYPE_SONET_PATH:
		return CString(_T("IF_TYPE_SONET_PATH"));
	case IF_TYPE_SONET_VT:
		return CString(_T("IF_TYPE_SONET_VT"));
	case IF_TYPE_SMDS_ICIP:
		return CString(_T("IF_TYPE_SMDS_ICIP"));
	case IF_TYPE_PROP_VIRTUAL:
		return CString(_T("IF_TYPE_PROP_VIRTUAL"));
	case IF_TYPE_PROP_MULTIPLEXOR:
		return CString(_T("IF_TYPE_OTHER"));
	case IF_TYPE_IEEE80212:
		return CString(_T("IF_TYPE_IEEE80212"));
	case IF_TYPE_FIBRECHANNEL:
		return CString(_T("IF_TYPE_FIBRECHANNEL"));
	case IF_TYPE_HIPPIINTERFACE:
		return CString(_T("IF_TYPE_HIPPIINTERFACE"));
	case IF_TYPE_FRAMERELAY_INTERCONNECT:
		return CString(_T("IF_TYPE_FRAMERELAY_INTERCONNECT"));
	case IF_TYPE_AFLANE_8023:
		return CString(_T("IF_TYPE_AFLANE_8023"));
	case IF_TYPE_AFLANE_8025:
		return CString(_T("IF_TYPE_AFLANE_8025"));
	case IF_TYPE_CCTEMUL:
		return CString(_T("IF_TYPE_CCTEMUL"));
	case IF_TYPE_FASTETHER:
		return CString(_T("IF_TYPE_FASTETHER"));
	case IF_TYPE_ISDN:
		return CString(_T("IF_TYPE_ISDN"));
	case IF_TYPE_V11:
		return CString(_T("IF_TYPE_V11"));
	case IF_TYPE_V36:
		return CString(_T("IF_TYPE_V36"));
	case IF_TYPE_G703_64K:
		return CString(_T("IF_TYPE_G703_64K"));
	case IF_TYPE_G703_2MB:
		return CString(_T("IF_TYPE_G703_2MB"));
	case IF_TYPE_QLLC:
		return CString(_T("IF_TYPE_QLLC"));
	case IF_TYPE_FASTETHER_FX:
		return CString(_T("IF_TYPE_FASTETHER_FX"));
	case IF_TYPE_CHANNEL:
		return CString(_T("IF_TYPE_CHANNEL"));
	case IF_TYPE_IEEE80211:
		return CString(_T("IF_TYPE_IEEE80211"));
	case IF_TYPE_IBM370PARCHAN:
		return CString(_T("IF_TYPE_IBM370PARCHAN"));
	case IF_TYPE_ESCON:
		return CString(_T("IF_TYPE_ESCON"));
	case IF_TYPE_DLSW:
		return CString(_T("IF_TYPE_DLSW"));
	case IF_TYPE_ISDN_S:
		return CString(_T("IF_TYPE_ISDN_S"));
	case IF_TYPE_ISDN_U:
		return CString(_T("IF_TYPE_ISDN_U"));
	case IF_TYPE_LAP_D:
		return CString(_T("IF_TYPE_LAP_D"));
	case IF_TYPE_IPSWITCH:
		return CString(_T("IF_TYPE_IPSWITCH"));
	case IF_TYPE_RSRB:
		return CString(_T("IF_TYPE_RSRB"));
	case IF_TYPE_ATM_LOGICAL:
		return CString(_T("IF_TYPE_ATM_LOGICAL"));
	case IF_TYPE_DS0:
		return CString(_T("IF_TYPE_DS0"));
	case IF_TYPE_DS0_BUNDLE:
		return CString(_T("IF_TYPE_DS0_BUNDLE"));
	case IF_TYPE_BSC:
		return CString(_T("IF_TYPE_BSC"));
	case IF_TYPE_ASYNC:
		return CString(_T("IF_TYPE_ASYNC"));
	case IF_TYPE_CNR:
		return CString(_T("IF_TYPE_CNR"));
	case IF_TYPE_ISO88025R_DTR:
		return CString(_T("IF_TYPE_ISO88025R_DTR"));
	case IF_TYPE_EPLRS:
		return CString(_T("IF_TYPE_EPLRS"));
	case IF_TYPE_ARAP:
		return CString(_T("IF_TYPE_ARAP"));
	case IF_TYPE_PROP_CNLS:
		return CString(_T("IF_TYPE_PROP_CNLS"));
	case IF_TYPE_HOSTPAD:
		return CString(_T("IF_TYPE_HOSTPAD"));
	case IF_TYPE_TERMPAD:
		return CString(_T("IF_TYPE_TERMPAD"));
	case IF_TYPE_FRAMERELAY_MPI:
		return CString(_T("IF_TYPE_FRAMERELAY_MPI"));
	case IF_TYPE_X213:
		return CString(_T("IF_TYPE_X213"));
	case IF_TYPE_ADSL:
		return CString(_T("IF_TYPE_ADSL"));
	case IF_TYPE_RADSL:
		return CString(_T("IF_TYPE_RADSL"));
	case IF_TYPE_SDSL:
		return CString(_T("IF_TYPE_SDSL"));
	case IF_TYPE_VDSL:
		return CString(_T("IF_TYPE_VDSL"));
	case IF_TYPE_ISO88025_CRFPRINT:
		return CString(_T("IF_TYPE_ISO88025_CRFPRINT"));
	case IF_TYPE_MYRINET:
		return CString(_T("IF_TYPE_MYRINET"));
	case IF_TYPE_VOICE_EM:
		return CString(_T("IF_TYPE_VOICE_EM"));
	case IF_TYPE_VOICE_FXO:
		return CString(_T("IF_TYPE_VOICE_FXO"));
	case IF_TYPE_VOICE_FXS:
		return CString(_T("IF_TYPE_VOICE_FXS"));
	case IF_TYPE_VOICE_ENCAP:
		return CString(_T("IF_TYPE_VOICE_ENCAP"));
	case IF_TYPE_VOICE_OVERIP:
		return CString(_T("IF_TYPE_VOICE_OVERIP"));
	case IF_TYPE_ATM_DXI:
		return CString(_T("IF_TYPE_OTHER"));
	case IF_TYPE_ATM_FUNI:
		return CString(_T("IF_TYPE_ATM_FUNI"));
	case IF_TYPE_ATM_IMA:
		return CString(_T("IF_TYPE_ATM_IMA"));
	case IF_TYPE_PPPMULTILINKBUNDLE:
		return CString(_T("IF_TYPE_PPPMULTILINKBUNDLE"));
	case IF_TYPE_IPOVER_CDLC:
		return CString(_T("IF_TYPE_IPOVER_CDLC"));
	case IF_TYPE_IPOVER_CLAW:
		return CString(_T("IF_TYPE_IPOVER_CLAW"));
	case IF_TYPE_STACKTOSTACK:
		return CString(_T("IF_TYPE_STACKTOSTACK"));
	case IF_TYPE_VIRTUALIPADDRESS:
		return CString(_T("IF_TYPE_VIRTUALIPADDRESS"));
	case IF_TYPE_MPC:
		return CString(_T("IF_TYPE_MPC"));
	case IF_TYPE_IPOVER_ATM:
		return CString(_T("IF_TYPE_IPOVER_ATM"));
	case IF_TYPE_ISO88025_FIBER:
		return CString(_T("IF_TYPE_ISO88025_FIBER"));
	case IF_TYPE_TDLC:
		return CString(_T("IF_TYPE_TDLC"));
	case IF_TYPE_GIGABITETHERNET:
		return CString(_T("IF_TYPE_GIGABITETHERNET"));
	case IF_TYPE_HDLC:
		return CString(_T("IF_TYPE_HDLC"));
	case IF_TYPE_LAP_F:
		return CString(_T("IF_TYPE_LAP_F"));
	default:
		return CString();
	}
}