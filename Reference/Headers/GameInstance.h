#pragma once

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Picking.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Pipeline.h"
#include "Light_Manager.h"


BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
public:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* 장치의 기능을 이용하기위하여 필수적으로 셋팅(초기화)되어야할 일들을 수행하낟. */
	HRESULT Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const CGraphic_Device::GRAPHICDEVDESC& GraphicDesc, ID3D11Device** ppOutDevice, ID3D11DeviceContext** ppOutDeviceContext);
	void Tick_Engine(_float fTimeDelta);
	HRESULT Render_Engine();
	HRESULT Clear(_uint iLevelIndex);

public: /* For.Graphic_Device */	
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public: /* For.Input_Device */
	_byte Get_DIKeyState(_ubyte byKeyID);
	_long Get_DIMMoveState(CInput_Device::DIMM eMouseMoveID);
	_byte Get_DIMButtonState(CInput_Device::DIMB eMouseButtonID);


public: /* For.Timer_Manager */
	HRESULT Add_Timer(const _tchar* pTimerTag);
	_float Compute_TimeDelta(const _tchar* pTimerTag);


public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelID, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_Layer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	_uint	Get_Layer_Size(_uint iLevelIndex, const _tchar* pLayerTag);

	class CComponent* Get_Component(_uint iLevelID, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);
	class CGameObject* Get_GameObjectPtr(_uint iLevelID, const _tchar* pLayerTag, _uint iIndex = 0);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);

public: /* For.PipeLine */
	_float4 Get_CamPositionFloat4() const;
	_vector Get_CamPositionVector() const;
	_matrix Get_TransformMatrix(CPipeline::TRANSFORMSTATETYPE eStateType);
	_float4x4 Get_TransformFloat4x4(CPipeline::TRANSFORMSTATETYPE eStateType);
	_float4x4 Get_TransformFloat4x4_TP(CPipeline::TRANSFORMSTATETYPE eStateType);

public: /* For.Light */
	const LIGHTDESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);
public: /* For.Picking */
	bool Picking(class CVIBuffer* pVIBuffer, class CTransform* pTransform, _float3* pOut);


private:
	CGraphic_Device*			m_pGraphic_Device = nullptr;
	CInput_Device*				m_pInput_Device = nullptr;
	CTimer_Manager*				m_pTimer_Manager = nullptr;
	CLevel_Manager*				m_pLevel_Manager = nullptr;
	CObject_Manager*			m_pObject_Manager = nullptr;
	CComponent_Manager*			m_pComponent_Manager = nullptr;
	CPipeline*						m_pPipeline = nullptr;
	CLight_Manager*				m_pLight_Manager = nullptr;
	CPicking*					m_pPicking = nullptr;

public:
	static void Release_Engine();
	virtual void Free() override;
};

END