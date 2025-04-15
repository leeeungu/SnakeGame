#include "ASnake.h"
#include "InputHandleManager.h"
#include "SpriteManager.h"
#include "AMap.h"

C_ASnake::C_ASnake()
{
	m_bActive = true;
	m_bUpdate = true;
	m_bRender = false;
	SetRegisterRender(false);
	SetRegisterUpdate(true);
	CreateEventActorArray(E_EventActor::E_EnumMax);

	using namespace InputHandle::KeyBoard;
	InputHandleManager::RegisterKeyBoard(InputType::E_KEYDOWN, ValueType::E_UP, this);
	InputHandleManager::RegisterKeyBoard(InputType::E_KEYDOWN, ValueType::E_DOWN, this);
	InputHandleManager::RegisterKeyBoard(InputType::E_KEYDOWN, ValueType::E_LEFT, this);
	InputHandleManager::RegisterKeyBoard(InputType::E_KEYDOWN, ValueType::E_RIGHT, this);
}

C_ASnake::~C_ASnake()
{
	using namespace InputHandle::KeyBoard;
	InputHandleManager::UnRegisterKeyBoard(InputType::E_KEYDOWN, ValueType::E_UP, this);
	InputHandleManager::UnRegisterKeyBoard(InputType::E_KEYDOWN, ValueType::E_DOWN, this);
	InputHandleManager::UnRegisterKeyBoard(InputType::E_KEYDOWN, ValueType::E_LEFT, this);
	InputHandleManager::UnRegisterKeyBoard(InputType::E_KEYDOWN, ValueType::E_RIGHT, this);
}

int C_ASnake::GetAppleType()
{
	return m_eApplType;
}

void C_ASnake::SetDead()
{
	m_bDead = true;
	m_bUpdate = false; 
	RunEvent(E_EventActor::E_GameState);
}

void C_ASnake::Update(Uint32 fDeltaTick)
{
	if ((m_sDir.y == m_sPreDir.y && m_sPreDir.x * -1 == m_sDir.x )||
		(m_sDir.x == m_sPreDir.x && m_sPreDir.y * -1 == m_sDir.y) )
	{
		m_sDir = m_sPreDir;
	}

	if (m_bDecreases && m_pTail)
	{
		m_nBodyCount -= 1;
		S_Body* pTail = m_pTail;
		pTail = pTail->pPre;
		PopBody(m_pTail);
		SetSprite(m_pPreTail);
		SetSprite(m_pTail);
		SetSprite(pTail);
		m_nDecreaseSize--;
		if (m_nDecreaseSize <= 0)
			m_bDecreases = false;
		RunEvent(E_EventActor::E_GameState);
	}

	if (m_bDecreases || !m_pMap || (m_sDir.x == m_sDir.y && m_sDir.y == 0))
		return;

	using namespace Sprite::Block;
	SDL_Point sNextPos;
	sNextPos.x = m_sCurPos.x + m_sDir.x;
	sNextPos.y = m_sCurPos.y + m_sDir.y;
	C_ABlock* pNextBlock = m_pMap->GetBlock(sNextPos);
	if (!pNextBlock)
	{
		SetDead();
		return;
	}
	unsigned int nIndex = pNextBlock->GetSpriteIndex();
	S_Body* pNextBody = GetBody();
	S_Body* pHead = m_pHead;
	S_Body* pTail = m_pTail;
	if (pTail->pBlock)
		pTail->pBlock->SetSpriteIndex(0);
	PopBody(m_pTail);
	PushBody(pNextBody);
	pNextBody->pBlock = pNextBlock;
	pNextBody->sCoord = sNextPos;
	pNextBlock->OverlapEventActor(this);
	OverlapEventActor(pNextBlock);
	SetSprite(pNextBody);
	SetSprite(pHead);
	SetSprite(m_pTail);
	SetSprite(pTail);
	SetSprite(m_pPreTail);

	if (nIndex != Sprite::Block::E_None && nIndex < Sprite::Block::E_RedApple)
	{
		SetDead();
	}
	m_sCurPos = sNextPos;
	m_sPreDir = m_sDir;
}

void C_ASnake::HandleEvent()
{
	using namespace InputHandle::KeyBoard::ValueType;
	E_TypeID eType = InputHandleManager::GetCurrentKeyBoardValue();
	void (C_ASnake:: * pFunc[E_TypeID::E_EnumMax])() {};
	pFunc[E_TypeID::E_DOWN] = &C_ASnake::Down;
	pFunc[E_TypeID::E_UP] = &C_ASnake::Up;
	pFunc[E_TypeID::E_LEFT] = &C_ASnake::Left;
	pFunc[E_TypeID::E_RIGHT] = &C_ASnake::Right;
	if (pFunc[eType])
	{
		(this->*pFunc[eType])();
	}
}

bool C_ASnake::OverlapEvent(C_Actor* pActor)
{
	C_ABlock* pBlock = dynamic_cast<C_ABlock*>(pActor);
	if (!pBlock)
		return false;

	using namespace Block::Apple;
	m_eApplType = pBlock->GetAppleType();
	if (m_eApplType == E_TypeID::E_Red)
	{
		if (m_pTail && m_pPreTail)
		{
			m_pTail->pNext = m_pPreTail;
			if (m_pPreTail)
				m_pPreTail->pPre = m_pTail;
			SetSprite(m_pTail);
			SetSprite(m_pPreTail);
			m_pTail = m_pPreTail;
			m_nBodyCount++;
			RunEvent(E_EventActor::E_GameState);
		}
	}
	else if (m_eApplType == E_TypeID::E_Gold)
	{
		if (m_pTail && m_pPreTail)
		{
			m_nDecreaseSize = m_nBodyCount / 2;
			if (m_nDecreaseSize <= 1)
			{
				m_nDecreaseSize = 0;
			}
			m_bDecreases = m_nDecreaseSize != 0;
		}
	}
	else if (m_eApplType == E_TypeID::E_Blue)
	{
		RunEvent(E_EventActor::E_GameState);
	}
	return true;
}

void C_ASnake::Reset()
{
	C_Actor* pActor = GetEventActor(E_EventActor::E_Map);
	if (!m_pMap && pActor)
	{
		m_pMap = dynamic_cast<C_AMap*>(pActor);
	}
	if (!m_pMap)
		return;
	for (size_t i = 0; i < E_BodySize::E_Size; i++)
	{
		arBody->sCoord = { 0,0 };
		arBody->pBlock = nullptr;
		arBody->pNext = nullptr;
		arBody->pPre = nullptr;
	}
	m_nBodyIndex = 0;
	m_nBodyCount = 2;
	m_sCurPos = { 4,5 };
	m_pHead = nullptr;
	m_pTail = nullptr;
	m_bDead = false;
	m_bUpdate = true;
	S_Body* pBody = GetBody();
	pBody->sCoord = m_sCurPos;
	pBody->pBlock = m_pMap->GetBlock(m_sCurPos);
	PushBody(pBody);
	pBody = GetBody();
	m_sCurPos = { 5,5 };
	pBody->sCoord = m_sCurPos;
	pBody->pBlock = m_pMap->GetBlock(m_sCurPos);
	PushBody(pBody);
	SetSprite(m_pHead);
	SetSprite(m_pTail);
	m_sDir = { 0,0 };
	m_sPreDir = { 0,1 };
}

C_ASnake::S_Body* C_ASnake::GetBody()
{
	int nLast = m_nBodyIndex;
	m_nBodyIndex++;
	while (nLast != m_nBodyIndex && arBody[m_nBodyIndex].pBlock)
	{
		m_nBodyIndex++;
		m_nBodyIndex = m_nBodyIndex % E_BodySize::E_Size;
	}
	arBody[m_nBodyIndex].pNext = nullptr;
	arBody[m_nBodyIndex].pPre = nullptr;
	arBody[m_nBodyIndex].pBlock = nullptr;
	return &arBody[m_nBodyIndex];
}

void C_ASnake::PushBody(S_Body* pBody)
{
	if (!pBody)
		return;
	S_Body* pNext{};
	if (m_pHead)
		pNext = m_pHead->pNext;
	pBody->pNext = m_pHead;
	if (m_pHead)
		m_pHead->pPre = pBody;
	if (m_pHead == nullptr)
	{
		m_pTail = pBody;
	}
	m_pHead = pBody;
}

void C_ASnake::PopBody(S_Body* pBody)
{
	if (!pBody)
		return;
	S_Body* pNext = pBody->pNext;
	S_Body* pPre = pBody->pPre;
	if (pNext)
		pNext->pPre = pPre;
	if (pPre)
		pPre->pNext = pNext;
	pBody->pNext = nullptr;
	pBody->pPre = nullptr;
	if (m_pPreTail)
	{
		m_pPreTail->pBlock = nullptr;
		m_pPreTail->sCoord = { 0,0 };
	}
	m_pPreTail = m_pTail;
	if (pBody == m_pTail)
		m_pTail = pPre;
	
}	

void C_ASnake::Up()
{
	m_sDir = { 0,-1 };
}

void C_ASnake::Down()
{
	m_sDir = { 0,1 };
}

void C_ASnake::Left()
{
	m_sDir = { -1,0 };
}

void C_ASnake::Right()
{
	m_sDir = { 1,0 };
}

void C_ASnake::SetSprite(S_Body* pBody)
{
	if (!pBody)
		return;
	using namespace Sprite::Block;
	S_Body* pPre = pBody->pPre;
	S_Body* pNext = pBody->pNext;
	E_SpriteID eSprite = E_SpriteID::E_None;
	if (!pNext)
	{
		if (pPre)
		{
			eSprite = E_SpriteID::E_DownTial;
			if (pPre->sCoord.y == pBody->sCoord.y)
			{
				eSprite = E_SpriteID::E_LeftTial;
				if (pPre->sCoord.x > pBody->sCoord.x)
				{
					eSprite = E_SpriteID::E_RightTial;
				}
			}
			else if (pPre->sCoord.y < pBody->sCoord.y)
			{
				eSprite = E_SpriteID::E_UpTial;
			}
		}
		else
		{
			eSprite = E_SpriteID::E_None;
		}
	}
	else if (pNext)
	{
		if (!pPre)
		{
			eSprite = E_SpriteID::E_UpHead;
			if (pNext->sCoord.y == pBody->sCoord.y)
			{
					eSprite = E_SpriteID::E_RightHead;
				if (pNext->sCoord.x > pBody->sCoord.x)
				{
				eSprite = E_SpriteID::E_LeftHead;
				}
			}
			else if (pNext->sCoord.y < pBody->sCoord.y)
			{
				eSprite = E_SpriteID::E_DownHead;
			}
		}
		else
		{
			eSprite = E_SpriteID::E_CurveRightDown;
			if (pPre->sCoord.y == pBody->sCoord.y) { // 가로로 
				if (pBody->sCoord.y == pNext->sCoord.y)
					eSprite = E_SpriteID::E_Row;
				else if (pNext->sCoord.y > pBody->sCoord.y) { // 꼬리가 왼족 방향	ㄱ	or 반전 ㄱ ?
					if (pPre->sCoord.x < pNext->sCoord.x) // ㄱ
						eSprite = E_SpriteID::E_CurveLeftDown;
					else // 반전 ㄱ
						eSprite = E_SpriteID::E_CurveRightDown;
				}
				else {  // ㄴ or 반전 ㄴ
					if (pNext->sCoord.x > pPre->sCoord.x) // 반전 ㄴ
						eSprite = E_SpriteID::E_CurveLeftUp;
					else // ㄴ
						eSprite = E_SpriteID::E_CurveRightUp;
				}
			}
			else {  // 세로
				if (pBody->sCoord.x == pNext->sCoord.x)
					eSprite = E_SpriteID::E_Col;
				else if (pNext->sCoord.x < pBody->sCoord.x) {// 꼬리가 위 ㄱ or 반전 ㄴ
					if (pPre->sCoord.y < pNext->sCoord.y) // ㄱ
						eSprite = E_SpriteID::E_CurveLeftUp;
					else // 반전 ㄴ
						eSprite = E_SpriteID::E_CurveLeftDown;
				}
				else { // ㄴ or 반전 ㄱ 
					if (pPre->sCoord.y > pNext->sCoord.y)  // 반전 ㄱ
						eSprite = E_SpriteID::E_CurveRightDown;
					else // ㄴ
						eSprite = E_SpriteID::E_CurveRightUp;
				}
			}
		}
	}
	if (pBody->pBlock)
		pBody->pBlock->SetSpriteIndex(eSprite);
}
//	else
//		snakeBody_index.push_back(head_index);
//}