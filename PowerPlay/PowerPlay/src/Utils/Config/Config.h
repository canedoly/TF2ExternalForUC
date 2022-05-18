#pragma once
#include "json.hpp"
#include <fstream>
#include <iomanip>
#include <any>

#include "../Color/Color.h"

using json = nlohmann::json;

namespace CFG
{
	enum struct EVarType {
		_unk, _bool, _int, _float, _color4
	};

	struct Var_t
	{
		const char *m_pszKey;
		std::any m_Value;
		EVarType m_Type;
		Var_t(const char *k, const std::any &v);
		template <typename T>
		inline T &Get() { return std::any_cast<T &>(m_Value); }
	};

	inline std::vector<Var_t *> VarPtrs;

	inline Var_t::Var_t(const char *k, const std::any &v)
	{
		m_pszKey = k; m_Value = v;

		if (m_Value.type() == typeid(bool))
			m_Type = EVarType::_bool;

		else if (m_Value.type() == typeid(int))
			m_Type = EVarType::_int;

		else if (m_Value.type() == typeid(float))
			m_Type = EVarType::_float;

		else if (m_Value.type() == typeid(Color_t))
			m_Type = EVarType::_color4;

		else m_Type = EVarType::_unk;

		VarPtrs.push_back(this);
	}

	inline void Save(const char *name)
	{
		std::ofstream file(name);

		if (file.is_open())
		{
			json j;

			for (auto pVar : VarPtrs)
			{
				switch (pVar->m_Type)
				{
					case EVarType::_bool: {
						j[pVar->m_pszKey] = pVar->Get<bool>();
						break;
					}

					case EVarType::_int: {
						j[pVar->m_pszKey] = pVar->Get<int>();
						break;
					}

					case EVarType::_float: {
						j[pVar->m_pszKey] = pVar->Get<float>();
						break;
					}

					case EVarType::_color4: {
						auto col = pVar->Get<Color_t>();
						j[pVar->m_pszKey] = { col.r, col.g, col.b, col.a };
						break;
					}

					default: break;
				}
			}

			file << std::setw(4) << j;
			file.close();
		}
	}

	inline void Load(const char *name)
	{
		std::ifstream file(name);

		if (file.is_open())
		{
			json j;
			file >> j;

			for (auto pVar : VarPtrs)
			{
				if (j.find(pVar->m_pszKey) != j.end())
				{
					switch (pVar->m_Type)
					{
						case EVarType::_bool: {
							pVar->m_Value = std::make_any<bool>(j[pVar->m_pszKey]);
							break;
						}

						case EVarType::_int: {
							pVar->m_Value = std::make_any<int>(j[pVar->m_pszKey]);
							break;
						}

						case EVarType::_float: {
							pVar->m_Value = std::make_any<float>(j[pVar->m_pszKey]);
							break;
						}

						case EVarType::_color4: {
							Color_t clr = { j[pVar->m_pszKey][0], j[pVar->m_pszKey][1], j[pVar->m_pszKey][2], j[pVar->m_pszKey][3] };
							pVar->m_Value = clr;
							break;
						}

						default: break;
					}
				}
			}

			file.close();
		}
	}
}

#define CFGVAR(name, val) inline CFG::Var_t name{ #name, val }