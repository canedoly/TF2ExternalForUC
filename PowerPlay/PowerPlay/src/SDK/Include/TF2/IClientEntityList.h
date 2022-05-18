#pragma once
#include "basehandle.h"
#include "../Offsets/Offsets.h"

class IClientEntityList
{
public:
	CBaseEntity *GetClientEntity(int entnum) {
		return g_Memory.RPM<CBaseEntity *>(Offsets::dwClient + (Offsets::dwEntityList + (entnum * 0x10)));
	}

	CBaseEntity *GetClientEntityFromHandle(CBaseHandle hEnt) {
		return GetClientEntity((hEnt.ToInt() & 0xFFF) - 1);
	}

	CBaseEntity *GetClientEntity(const DWORD &dwEntity) {
		return reinterpret_cast<CBaseEntity *>(dwEntity);
	}

	int GetHighestEntityIndex() {
		return g_Memory.RPM<int>(Offsets::dwClient + Offsets::dwEntityList + 0x10024);
	}
};

namespace I { inline IClientEntityList ClientEntityList; }

inline IHandleEntity *CBaseHandle::Get() const
{
	return reinterpret_cast<IHandleEntity *>(I::ClientEntityList.GetClientEntityFromHandle(m_Index));
}

#define LOCAL_ADDRESS g_Memory.RPM<DWORD>(Offsets::dwClient + Offsets::dwLocalPlayer)