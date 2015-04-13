//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/07/25 09:35:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "SP_DS_StSimImmediateTransitionList.h"
#include <math.h>
#include "sp_core/tools/MersenneTwister.h"

SP_DS_StSimImmediateTransitionList::SP_DS_StSimImmediateTransitionList():
m_Sum(0.0)
{
	m_pcMTRand = new MTRand(rand());
}

SP_DS_StSimImmediateTransitionList::~SP_DS_StSimImmediateTransitionList()
{
	delete m_pcMTRand;
	Clear();
}

void SP_DS_StSimImmediateTransitionList::InsertImmediateTransition(long p_nTransitionNumber, double p_nWeight)
{
	if(p_nWeight > 0)
	{
		SP_MapLong2Double::iterator it = m_mTrans.find(p_nTransitionNumber);
		if(it != m_mTrans.end())
		{
			m_Sum -= it->second;
			it->second = p_nWeight;
		}
		else
		{
			m_mTrans.insert(std::pair<long, double>(p_nTransitionNumber, p_nWeight));
		}
		m_Sum += p_nWeight;
	}
	else
	{
		RemoveElem(p_nTransitionNumber);
	}
}

void SP_DS_StSimImmediateTransitionList::Clear()
{
	m_Sum = 0.0;
	m_mTrans.clear();
}

size_t SP_DS_StSimImmediateTransitionList::GetElemCount()
{
	return m_mTrans.size();
}

void SP_DS_StSimImmediateTransitionList::RemoveElem(long p_nTransNumber)
{
	SP_MapLong2Double::iterator it = m_mTrans.find(p_nTransNumber);
	if(it != m_mTrans.end())
	{
		m_Sum -= it->second;
		m_mTrans.erase(it);
	}
}

long SP_DS_StSimImmediateTransitionList::ChooseTransitionByUniformDistribution()
{
	size_t l_nSize = m_mTrans.size();
	size_t l_nChoose = m_pcMTRand->randInt() % l_nSize;
	size_t i = 0;

	SP_MapLong2Double::iterator l_it =  m_mTrans.begin();
	SP_MapLong2Double::iterator l_end = m_mTrans.end();
	for(; l_it != l_end; ++l_it)
	{
		if(i == l_nChoose)
		{
			long l_nReturn = l_it->first;
			m_Sum -= l_it->second;
			m_mTrans.erase(l_it);
			return l_nReturn;
		}
		++i;
	}
	return -1;
}

long SP_DS_StSimImmediateTransitionList::ChooseTransitionByWeight()
{
	double l_nU = m_Sum * m_pcMTRand->randExc();
	double l_nRunningSum = 0;

	SP_MapLong2Double::iterator l_it =  m_mTrans.begin();
	SP_MapLong2Double::iterator l_end = m_mTrans.end();
	for(; l_it != l_end; ++l_it)
	{
		l_nRunningSum += l_it->second;

		if (l_nRunningSum > l_nU)
		{
			long l_nReturn = l_it->first;
			m_Sum -= l_it->second;
			m_mTrans.erase(l_it);
			return l_nReturn;
		}
	}
	return -1;
}

