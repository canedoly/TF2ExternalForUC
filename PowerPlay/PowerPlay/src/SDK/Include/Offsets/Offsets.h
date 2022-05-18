#pragma once
#include "../../../Utils/Memory/Memory.h"

namespace Offsets
{
	inline DWORD dwClient			= 0x0;
	inline DWORD dwEngine			= 0x0;
	inline DWORD dwLocalPlayer		= 0x0;
	inline DWORD dwEntityList		= 0x0;
	inline DWORD dwGetAllClasses	= 0x0;
	inline DWORD dwJump				= 0xC6E460; //TODO: sig
	inline DWORD dwAttack			= 0xC6E46C; //TODO: sig
	inline DWORD dwAttack2			= 0xC6E478; //TODO: sig
	inline DWORD dwViewAngles		= 0x467474; //TODO: sig

	inline void Initialize()
	{
		while (!dwClient)
			dwClient = reinterpret_cast<DWORD>(g_Memory.FindModule("client.dll").hModule);

		while (!dwEngine)
			dwEngine = reinterpret_cast<DWORD>(g_Memory.FindModule("engine.dll").hModule);

		while (!dwLocalPlayer)
		{
			if (auto dwAddress = g_Memory.FindPattern("client.dll", "8B 3D ? ? ? ? 8B F1 3B FE 75 07 5F 5E E9 ? ? ? ?"))
				dwLocalPlayer = g_Memory.RPM<DWORD>(dwAddress + 0x2) - dwClient;
		}

		while (!dwEntityList)
		{
			if (auto dwAddress = g_Memory.FindPattern("client.dll", "A1 ? ? ? ? A8 01 75 51 83 C8 01 C7 05 ? ? ? ? ? ? ? ? 68 ? ? ? ?"))
				dwEntityList = (g_Memory.RPM<DWORD>(dwAddress + 0x1) - dwClient) + 0x18;
		}

		while (!dwGetAllClasses)
		{
			if (auto dwAddress = g_Memory.FindPattern("client.dll", "A1 ? ? ? ? A3 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C3"))
				dwGetAllClasses = g_Memory.RPM<DWORD>(g_Memory.RPM<DWORD>(dwAddress + 0x1));
		}

		printf("dwClient 0x%X\n", dwClient);
		printf("dwEngine 0x%X\n", dwEngine);
		printf("dwLocalPlayer 0x%X\n", dwLocalPlayer);
		printf("dwEntityList 0x%X\n", dwEntityList);
		printf("dwGetAllClasses 0x%X\n", dwGetAllClasses);
	}
}