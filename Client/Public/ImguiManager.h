#pragma once

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_dx11.h"
#include "ImGui\imgui_impl_win32.h"

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CTransform;
class CShader;
class CVIBuffer_Triangle;
END

BEGIN(Client)

class CImguiManager final
	:public CBase
{
public:
	enum ToolList {TOOL_UI, TOOL_OBJECT, TOOL_NAVIGATION, TOOL_END};

	typedef struct tagTriangle
	{
		_float3 vPos1;
		_float3 vPos2;
		_float3 vPos3;
	}TRIANGLE;

public:
	CImguiManager();
	virtual ~CImguiManager() = default;

public:
	HRESULT NativeConstruct(HWND hWnd, ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context);
	void Tick(_double TimeDelta);
	HRESULT Render();

private:
	void	UI_Tool();

	void	Object_Tool();

	void	Navigation_Tool(_float TimeDelta);
	_bool	Check_TrianglePoint(_float3& fPos);
	void	Sort_TrianglePoint();

private:
	CGameInstance* m_pGameInstance = nullptr;
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;

	ToolList			  m_eToolList = TOOL_END;
	//For UI Tool
	_uint				  m_iNumUI = 0;
	static const char* CurrentItem;
	_uint				   m_iCurrentItemIndex = 0;
	CTransform*		m_pTransform = nullptr;

	//For Object Tool
	_uint	m_iNumObject = 0;
	_int	m_iObjectIndex = 0;
	_uint	m_iNumObjectIndex = 175;

	//For Navigation Tool
	_float3					m_vPoint[3];
	_uint					m_iPointCount = 0;
	_float					m_fInputDelay = 0.f;

	vector<TRIANGLE*>		m_TrianglePoints;
	vector<CVIBuffer_Triangle*> m_Triangle;

	CShader*				m_pShader = nullptr; 

	_bool					m_bEnableImgui = true;

public:
	static CImguiManager* Create(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	virtual void Free() override;
};

END


