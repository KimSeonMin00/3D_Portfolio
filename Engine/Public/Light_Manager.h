#pragma once
#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHTDESC* Get_LightDesc(_uint iIndex) const;

public:
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);
	HRESULT Render_Lights(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader);

private:
	vector<class CLight*>				m_Lights;
	typedef vector<class CLight*>		LIGHTS;

public:
	virtual void Free() override;
};

END

