#pragma once

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_dx11.h"
#include "ImGui\imgui_impl_win32.h"

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CImguiManager final
	:public CBase
{
public:
	CImguiManager();
	virtual ~CImguiManager() = default;

public:
	HRESULT NativeConstruct(HWND hWnd, ID3D11Device * pDevice, ID3D11DeviceContext * pDevice_Context);
	void Tick(_double TimeDelta);
	HRESULT Render();

private:
	CGameInstance* m_pGameInstance = nullptr;

public:
	static CImguiManager* Create(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDevice_Context);
	virtual void Free() override;
};

END


