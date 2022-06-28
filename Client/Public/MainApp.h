#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT NativeConstruct();
	void Tick(_double TimeDelta);
	HRESULT Render();

private:
	CGameInstance*				m_pGameInstance = nullptr;
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;
	CRenderer*						m_pRenderer = nullptr;

	class CImguiManager*		m_pImgui_Manager = nullptr;
public:
	HRESULT Open_Level(LEVEL eLevel);
	HRESULT Ready_Prototype_Component_Static();
	HRESULT Ready_Gara();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END