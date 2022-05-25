#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	explicit CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT NativeConstruct(LEVEL eNextLevelID);
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CLoader*			m_pLoader = nullptr;
	LEVEL					m_eNextLevelID = LEVEL_END;

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

END

