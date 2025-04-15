#pragma once


#include "SDL_stdinc.h"
#include "NetworkManager.h"
class ObjectManager;
//enum E_SocketType;
//enum Network::Socket::E_SocketType;

__interface I_Object
{
	void Init();
	void InitObject();
	void BeginPlay();
	void Update(Uint32 fDeltaTick);
	void EndPlay();
	void Reset();
	void HandleEvent();

	bool RecvTCPMessage(int nMessageType, void* pMessage, int nMessageLength);
	bool RecvUDPMessage(void* pMessage, int nMessageLength);

	// virtual void BeginPlay() override;
	// virtual void Update() override;
	// virtual void EndPlay() override;
};

namespace Object
{
	enum E_UpdateLayer
	{
		E_None,
		E_EnumMax
	};
}

class C_Object abstract : public I_Object
{
private:
	bool m_bRegisterUpdate;
protected:
	Object::E_UpdateLayer m_eUpdateLayer;
	bool m_bActive;
	bool m_bUpdate;
	bool m_bActiveBein;
	bool m_bCanReset;
	bool m_bTCPToggle;
	void* m_pTCPMessage;
public:
	C_Object() ;
	virtual ~C_Object();

	void SetUpdateLayer(Object::E_UpdateLayer bValue) { m_eUpdateLayer = bValue; }
	Object::E_UpdateLayer GetUpdateLayer() { return m_eUpdateLayer; }
	void SetCanReset(bool bValue) { m_bCanReset = bValue; }
	bool GetCanReset() { return m_bCanReset; }
	void SetActive(bool bValue) { m_bActive = bValue; }
	bool GetActive() { return m_bActive; }
	void SetActiveBegin(bool bValue) { m_bActiveBein = bValue; }
	bool GetActiveBegin() { return m_bActiveBein; }
	void SetUpdate(bool bValue) { m_bUpdate = bValue; }
	bool GetUpdate() { return m_bUpdate; }
	bool GetRegisterUpdate() { return m_bRegisterUpdate; }
	void SetRegisterUpdate(bool bValue);

	void BeginPlayObject();
	void UpdateObject(Uint32 fDeltaTick);
	void EndPlayObject();
	void ResetObject();
	void HandleEventObject();

	bool RecvObject(Network::Protocol::E_ProtocolType eSocketType, int nMessageType, void* pMessage, int nMessageLength);
protected:
	virtual void InitObject() override {};
	// I_Object을(를) 통해 상속됨
	virtual void Init() override {};	
	virtual void BeginPlay() override {};
	virtual void HandleEvent() override {};
	virtual void Update(Uint32 fDeltaTick) override {};
	virtual void EndPlay() override {};
	virtual void Reset() override {};
	virtual bool RecvTCPMessage(int nMessageType, void* pMessage, int nMessageLength)  override { return false;};
	virtual bool RecvUDPMessage(void* pMessage, int nMessageLength)  override { return false;};
};