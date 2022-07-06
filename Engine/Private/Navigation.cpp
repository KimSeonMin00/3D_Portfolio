#include "..\Public\Navigation.h"
#include "Cell.h"
#include "Shader.h"

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_iCurrentCellIndex(rhs.m_iCurrentCellIndex)
	, m_Cells(rhs.m_Cells)
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
#endif // _DEBUG
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif // _DEBUG
}

HRESULT CNavigation::NativeConstruct_Prototype(const _tchar * pNavigationDataFilePath)
{
	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(pNavigationDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		_float3		vPoints[3];

		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		if (dwByte == 0)
			break;

		CCell*			pCell = CCell::Create(m_pDevice, m_pDeviceContext, vPoints, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	if (FAILED(SetUp_Neighbor()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Ready_Debug()))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

HRESULT CNavigation::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
	{
		m_iCurrentCellIndex = ((NAVIGATIONDESC*)pArg)->iCurrentIndex;
	}

	return S_OK;
}

_bool CNavigation::Move_OnNavigation(const _float3 * pPosition, _float3* pDir)
{
	_int		iNeighborIndex = -1;
	if (pDir == nullptr)
		return false;

	/* 셀안에서 움직였다. */
	if (true == m_Cells[m_iCurrentCellIndex]->isIn(pPosition, &iNeighborIndex, pDir))
		return true;

	/* 셀 밖으로 움직였다. */
	else
	{
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(pPosition, &iNeighborIndex, pDir))
				{
					m_iCurrentCellIndex = iNeighborIndex;
					break;
				}
			}
			return true;
		}
		else
			return false;

	}


	return S_OK;
}

_bool CNavigation::Check_isIn_Navigation(const _float3 * pPosition)
{
	if (m_iCurrentCellIndex == -1)
		return false;

	else
	{
		_int iCellIndex = 0;
		_int iNeighborIndex = -1;

		for (auto& pCell : m_Cells)
		{
			if (pCell->isIn(pPosition, &iNeighborIndex, nullptr))
			{
	s			m_iCurrentCellIndex = iCellIndex;
				return true;
			}

			else
			{
				iCellIndex++;
			}
		}

		return false;
	}
}

HRESULT CNavigation::SetUp_Neighbor()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
				pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDestCell->Get_Index());

			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
				pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDestCell->Get_Index());

			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
				pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDestCell->Get_Index());
		}
	}

	return S_OK;
}
#ifdef _DEBUG

HRESULT CNavigation::Ready_Debug()
{
	m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXCOL_DECLARATION::Elements, VTXCOL_DECLARATION::iNumElement);
	if (nullptr == m_pShader)
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigation::Render()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (-1 == m_iCurrentCellIndex)
	{
		m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pShader->Set_RawValue("g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4));

		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
				pCell->Render(m_pShader);
		}
	}

	else
	{
		WorldMatrix._42 = 0.1f;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
		m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));

		m_pShader->Set_RawValue("g_vColor", &_float4(1.f, 0.f, 0.f, 1.f), sizeof(_float4));

		m_Cells[m_iCurrentCellIndex]->Render(m_pShader);
	}



	return S_OK;
}
#endif // _DEBUG

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pNavigationDataFilePath)
{
	CNavigation*	pInstance = new CNavigation(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pNavigationDataFilePath)))
	{
		MSGBOX(TEXT("Failed to Created : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{

	CNavigation*	pInstance = new CNavigation(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif // _DEBUG
}

