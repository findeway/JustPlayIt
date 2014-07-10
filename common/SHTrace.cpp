#include "StdAfx.h"
#include "SHTrace.h"

set<CSHTrace*>					CSHTrace::m_objset;
auto_ptr<CSHCriticalSection>	CSHTrace::m_objsec;

CSHTrace::CSHTrace(void)
{
	if(m_objsec.get() == NULL)
		m_objsec.reset(new CSHCriticalSection());
	CCSScopeLock lock((*m_objsec)());
	m_objset.insert(this);
}

CSHTrace::~CSHTrace(void)
{
	CCSScopeLock lock((*m_objsec)());
	m_objset.erase(this);
}

bool CSHTrace::IsValid()const
{
	CCSScopeLock lock((*m_objsec)());
	return m_objset.find((CSHTrace*)this) != m_objset.end();
}
