#pragma once

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_dx11.h"
#include "ImGui\imgui_impl_win32.h"

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CTransform;
END

BEGIN(Client)

class CImguiManager final
	:public CBase
{
public:
	enum ToolList {TOOL_UI, TOOL_OBJECT, TOOL_END};

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

private:
	CGameInstance* m_pGameInstance = nullptr;

	ToolList			  m_eToolList = TOOL_END;
	//For UI Tool
	_uint				  m_iNumUI = 0;
	static const char* CurrentItem;
	_uint				   m_iCurrentItemIndex = 0;
	CTransform*		m_pTransform = nullptr;

	//For Object Tool
	_uint	m_iNumObject = 0;
	_int	m_iObjectIndex = 0;
	_uint	m_iNumObjectIndex = 6;

	_bool					m_bEnableImgui = true;

public:
	static CImguiManager* Create(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	virtual void Free() override;
};

END


