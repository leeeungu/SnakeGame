#include "AOpponent.h"
#include "AMap.h"
#include "AText.h"
#include "DebugMessageManager.h"

C_AOpponent::C_AOpponent()
{
	using namespace FontData;
	m_pOpponentMap = new C_AMap{};
	m_pOpponentMap->SetCanReset(false);
	m_pOpponentMap->SetRegisterRender(true);
	m_pOpponentMap->SetMapPosition({ Map::E_RectSize * Map::E_MapSize + 25, 30.f });

	m_pText[E_Score] = new C_AText{ E_FontID::E_cutetat , 30, {Map::E_RectSize * Map::E_MapSize + 25 + 100,0} };
	m_pText[E_Length] = new C_AText{ E_FontID::E_cutetat , 30, {Map::E_RectSize * Map::E_MapSize + 25 + 300,0} };
	m_pText[E_Speed] = new C_AText{ E_FontID::E_cutetat , 30, {Map::E_RectSize * Map::E_MapSize + 25 + 500,0} };

	SetCanReset(false);
	SetRegisterRender(true);
	{
		std::string Temp = "Score : " + std::to_string(0);
		m_pText[E_Score]->SetText(Temp.c_str());
	}

	{
		std::string Temp = "Speed : " + std::to_string(1);
		m_pText[E_Speed]->SetText(Temp.c_str());
	}
	{
		std::string Temp = "Length : " + std::to_string(2);
		m_pText[E_Length]->SetText(Temp.c_str());
	}
}

C_AOpponent::~C_AOpponent()
{
	for (int i = 0; i < E_EnumMax; i++)
	{
		if (m_pText[i])
			delete m_pText[i];
		m_pText[i] = nullptr;
	}
	if (m_pOpponentMap)
		delete m_pOpponentMap;
	m_pOpponentMap = nullptr;
}

void C_AOpponent::SetState(int nSpeed, int nScore, int nLength)
{
	{
		std::string Temp = "Score : " + std::to_string(nScore);
		m_pText[E_Score]->SetText(Temp.c_str());
	}

	{
		std::string Temp = "Speed : " + std::to_string(nSpeed);
		m_pText[E_Speed]->SetText(Temp.c_str());
	}
	{
		std::string Temp = "Length : " + std::to_string(nLength);
		m_pText[E_Length]->SetText(Temp.c_str());
	}
}

void C_AOpponent::SetMap(void* pMessage, int nMessageLength)
{
	if (m_pOpponentMap)
		m_pOpponentMap->RecvUDPMapMessage(pMessage, nMessageLength);
}

void C_AOpponent::SendAll()
{
	if (m_pOpponentMap)
		m_pOpponentMap->SendAll();
}
