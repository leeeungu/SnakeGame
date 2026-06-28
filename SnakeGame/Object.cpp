#include "Object.h"
#include "ObjectManager.h"
#include "UDPManager.h"

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
	if (bValue)
		ObjectManager::RegisterObject(this);
	else
		ObjectManager::UnRegisterObject(this);
	m_bRegisterUpdate = bValue;
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

bool C_Object::SendUDPMessage(void* pMessage, int nMessageLength)
{
	Network::Client::S_Client* pClient = (Network::Client::S_Client*)UDPManager::GetHost();
	if (!pClient)
		return false;
	memcpy(pMessage, &pClient->nClientID, sizeof(int));
	pClient->pUDPPacket->address = pClient->sUDPAddress;
	return UDPManager::UDPSend(*UDPManager::GetHost(), pMessage, nMessageLength);
}
