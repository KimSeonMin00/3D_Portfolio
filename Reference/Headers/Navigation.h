#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_uint		iCurrentIndex = 0;
	}NAVIGATIONDESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pNavigationDataFilePath);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	_bool Move_OnNavigation(const _float3* pPosition);
	_bool Check_isIn_Navigation(const _float3* pPosition);

public:
	HRESULT SetUp_Neighbor();

#ifdef _DEBUG
public:
	HRESULT Ready_Debug();
	HRESULT Render();
#endif // _DEBUG

private:
	_int								m_iCurrentCellIndex = -1;
	vector<class CCell*>				m_Cells;
	typedef vector<class CCell*>		CELLS;


#ifdef _DEBUG
private:
	class CShader*					m_pShader = nullptr;
#endif // _DEBUG

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pNavigationDataFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;


};

END
