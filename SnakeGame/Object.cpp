#include "Object.h"
#include "ObjectManager.h"
#include "NetworkManager.h"

C_Object::C_Object()
{
	m_bActive = true;
	m_bActiveBein = true;
	m_bRegisterUpdate = false;
	m_bUpdate = false;
	m_bCanReset = true;
	ObjectManager::RegisterObject(this);
}

C_Object::~C_Object()
{
	ObjectManager::UnRegisterObject(this);
}

void C_Object::SetRegisterUpdate(bool bValue)
{
	if (bValue == m_bRegisterUpdate)
		return;
	m_bRegisterUpdate = bValue;
	if (bValue)
		ObjectManager::RegisterObject(this);
	else
		ObjectManager::UnRegisterObject(this);
}

void C_Object::BeginPlayObject()
{
	m_bActiveBein = false;
	InitObject();
	BeginPlay();
}

void C_Object::UpdateObject(Uint32 fDeltaTick)
{
	if (m_bUpdate)
		Update(fDeltaTick);
}

void C_Object::EndPlayObject()
{
	EndPlay();	
}

void C_Object::ResetObject()
{
	if(m_bCanReset)
		Reset();
}

void C_Object::HandleEventObject()
{
	HandleEvent();
}

bool C_Object::RecvObject(Network::Protocol::E_ProtocolType eSocketType,int nMessageType, void* pMessage, int nMessageLength)
{
	if (eSocketType == Network::Protocol::E_ProtocolType::E_TCP)
		return RecvTCPMessage(pMessage);
	if (eSocketType == Network::Protocol::E_ProtocolType::E_UDP)
		return RecvUDPMessage(pMessage, nMessageLength);
	return false;
}
