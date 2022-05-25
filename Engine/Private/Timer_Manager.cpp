#include "..\Public\Timer_Manager.h"
#include "Timer.h"

IMPLEMENT_SINGLETON(CTimer_Manager)

CTimer_Manager::CTimer_Manager()
{
}

HRESULT CTimer_Manager::Add_Timer(const _tchar * pTimerTag)
{
	if (nullptr != Find_Timer(pTimerTag))
		return E_FAIL;

	CTimer*		pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;
	
	m_Timers.emplace(pTimerTag, pTimer);

	return S_OK;
}

_float CTimer_Manager::Compute_TimeDelta(const _tchar * pTimerTag)
{
	CTimer*	pTimer = Find_Timer(pTimerTag);

	if(nullptr == pTimer)
		return 0.0f;

	return pTimer->Compute_TimeDelta();	
}


CTimer * CTimer_Manager::Find_Timer(const _tchar * pTimerTag)
{


	/*auto	iter = find_if(m_Timers.begin(), m_Timers.end(), [&](TIMERS::value_type Pair)->_bool
	{
	return !lstrcmp(pTimerTag, Pair.first);
	});
	*/


	/*CTagFinder		Finder(pTimerTag);

	auto	iter = find_if(m_Timers.begin(), m_Timers.end(), Finder);*/

	auto	iter = find_if(m_Timers.begin(), m_Timers.end(), CTagFinder(pTimerTag));

	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;
}

void CTimer_Manager::Free()
{
	for (auto& Pair : m_Timers)
		Safe_Release(Pair.second);

	m_Timers.clear();

}

