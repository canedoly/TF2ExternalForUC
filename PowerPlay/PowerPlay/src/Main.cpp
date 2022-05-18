#include "SDK/SDK.h"

namespace Vars
{
	CFGVAR(Aimbot_Active, true);
	CFGVAR(Aimbot_Key, VK_SHIFT);
	CFGVAR(Aimbot_Auto_Shoot, false);
	CFGVAR(Aimbot_Aim_FOV, 180.0f);
}

struct Target_t
{
	CBaseEntity *m_pEntity = nullptr;
	Vec3 m_vAngleTo = {};
	float m_flFOVTo = 0.0f;
};

void Aimbot()
{
	if (!Vars::Aimbot_Active.Get<bool>())
		return;

	if (!(GetAsyncKeyState(Vars::Aimbot_Key.Get<int>()) & 0x8000))
		return;

	auto pLocal = g_EntityCache.GetLocal();

	if (!pLocal || pLocal->deadflag() || pLocal->InCond(TF_COND_TAUNTING))
		return;

	Vec3 vLocalPos = pLocal->GetShootPos() + (pLocal->m_vecVelocity() * 0.015000f);
	Vec3 vLocalAngles = I::EngineClient.GetViewAngles();

	std::vector<Target_t> vecTargets = {};

	for (auto pEntity : g_EntityCache.GetGroup(EEntGroup::PLAYERS_ENEMIES))
	{
		auto pPlayer = pEntity->As<CTFPlayer>();

		if (!pPlayer || pPlayer->deadflag() || pPlayer->IsInvulnerable())
			continue;
		
		Vec3 vPos = [&]()
		{
			switch (pLocal->m_iClass())
			{
				case CLASS_SCOUT:
				case CLASS_SOLDIER:
				case CLASS_PYRO:
				case CLASS_HEAVY:
				case CLASS_MEDIC:
				case CLASS_SNIPER:
				case CLASS_SPY: return pPlayer->GetBonePos(6) + Vec3(0.0f, 0.0f, 4.5f);
				case CLASS_DEMOMAN: return pPlayer->GetBonePos(16) + Vec3(0.0f, 0.0f, 4.5f);
				case CLASS_ENGINEER: return pPlayer->GetBonePos(8) + Vec3(0.0f, 0.0f, 4.5f);
				default: return Vec3();
			}
		}();

		Vec3 vAngleTo = Math::CalcAngle(vLocalPos, vPos);
		float flFOVTo = Math::CalcFov(vLocalAngles, vAngleTo);

		if (flFOVTo > Vars::Aimbot_Aim_FOV.Get<float>())
			continue;

		vecTargets.emplace_back(Target_t{ pPlayer, vAngleTo, flFOVTo });
	}

	if (vecTargets.empty())
		return;

	const auto &Target = *std::min_element(vecTargets.begin(), vecTargets.end(), [&](const Target_t &a, const Target_t &b) -> bool {
		return a.m_flFOVTo < b.m_flFOVTo;
	});

	I::EngineClient.SetViewAngles(Target.m_vAngleTo);

	if (Vars::Aimbot_Auto_Shoot.Get<bool>())
		I::BaseClientDLL.SetAttack(6);
}

int main()
{
	SetConsoleTitleA("PowerPlay");

	g_Memory.Initialize("hl2.exe");

	Offsets::Initialize();

	while (!(GetAsyncKeyState(VK_END) & 0x8000))
	{
		g_EntityCache.Fill();

		Aimbot();

		g_EntityCache.Clear();

		Sleep(0x1);
	}

	g_Memory.Free();

	return 0;
}