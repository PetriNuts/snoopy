//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/10/10 10:20:00 $
// Short Description: colored Placemultiset class
//////////////////////////////////////////////////////////////////////

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_PlaceMultiSet.h"
#include <algorithm>
#include "sp_core/SP_Core.h"

#include <stdlib.h>
#include <time.h>


SP_CPN_PlaceMultiSet::SP_CPN_PlaceMultiSet()
{
	m_MultiSetMap.clear();
}

SP_CPN_PlaceMultiSet::~SP_CPN_PlaceMultiSet()
{	
}


bool SP_CPN_PlaceMultiSet::IsEmpty()
{
	if(m_MultiSetMap.size()==0)
		return true;

	return false;
}

void SP_CPN_PlaceMultiSet::clear()
{
	m_MultiSetMap.clear();
}

int  SP_CPN_PlaceMultiSet::Size()
{
	int l_nNumber = 0;
	SP_MapString2Int::iterator itMap;

	for(itMap = m_MultiSetMap.begin(); itMap != m_MultiSetMap.end(); itMap++)
	{
		l_nNumber += (*itMap).second;
	}
	return l_nNumber;
}

void SP_CPN_PlaceMultiSet::AddMultiSet(wxString p_Color, int p_Appearance)
{
	auto it = m_MultiSetMap.find(p_Color);
	if( it != m_MultiSetMap.end() )
	{
		it->second += p_Appearance;
	}
	else
	{
		if(p_Appearance > 0)
			m_MultiSetMap[p_Color] = p_Appearance;
	}
}


bool SP_CPN_PlaceMultiSet::RemoveMultiSet(wxString p_Color, int p_Appearance)
{
	if( m_MultiSetMap.find(p_Color) != m_MultiSetMap.end() && m_MultiSetMap.find(p_Color)->second >= p_Appearance )
	{
		m_MultiSetMap[p_Color] = m_MultiSetMap.find(p_Color)->second - p_Appearance;
		if(m_MultiSetMap.find(p_Color)->second == 0)
		{
			m_MultiSetMap.erase(m_MultiSetMap.find(p_Color));
		}
		return true;
	}
	
	return false;
}

bool SP_CPN_PlaceMultiSet::IsMarking(wxString p_Color, int p_Appearance)
{
	if(p_Appearance == 0)
		return true;       // if the p_Appearance is 0, then it can be considered as a marking
	
	SP_MapString2Int::iterator itMap;
	
	if( m_MultiSetMap.find(p_Color) != m_MultiSetMap.end() && m_MultiSetMap.find(p_Color)->second >= p_Appearance )
	{
		return true;
	}
	
	return false;	
}

bool SP_CPN_PlaceMultiSet::IsMarking(map<wxString, int> p_mSubset)
{
	SP_MapString2Int::iterator itMap;

	for(itMap = p_mSubset.begin(); itMap != p_mSubset.end(); itMap++)
	{
		if( itMap->second == 0 )
			continue;		
		
		if( m_MultiSetMap.find( itMap->first ) == m_MultiSetMap.end() )
		{
			return false;
		}

		if( m_MultiSetMap.find( itMap->first ) != m_MultiSetMap.end() && m_MultiSetMap.find(itMap->first)->second < itMap->second )
		{
			return false;
		}		
	}
	
	return true;
}

bool SP_CPN_PlaceMultiSet::EqualArcTest( SP_MapString2Int p_mSubset )

{
	SP_MapString2Int::iterator itMap;

	for(itMap = p_mSubset.begin(); itMap != p_mSubset.end(); itMap++)
	{
		if( itMap->second == 0 )
			continue;

		if( m_MultiSetMap.find( itMap->first ) == m_MultiSetMap.end() )
		{
			return false;
		}

		if( m_MultiSetMap.find( itMap->first ) != m_MultiSetMap.end() && m_MultiSetMap.find(itMap->first)->second != itMap->second )
		{
			return false;
		}
	}
	
	return true;
}

bool SP_CPN_PlaceMultiSet::InhibitorArcTest( SP_MapString2Int p_mSubset )

{
	SP_MapString2Int::iterator itMap;

	for(itMap = p_mSubset.begin(); itMap != p_mSubset.end(); itMap++)
	{
		if( itMap->second == 0 )
			continue;

		//if( m_MultiSetMap.find( itMap->first ) == m_MultiSetMap.end() )
		//{
		//	return false;
		//}

		if( m_MultiSetMap.find( itMap->first ) != m_MultiSetMap.end() && m_MultiSetMap.find(itMap->first)->second >= itMap->second )
		{
			return false;
		}
	}
	
	return true;
}

void SP_CPN_PlaceMultiSet::SetColorSetName(wxString p_ColorSetName)
{
	m_sColorSetName = p_ColorSetName;
}

wxString SP_CPN_PlaceMultiSet::GetColorSetName()
{
	return m_sColorSetName;
}

SP_MapString2Int* SP_CPN_PlaceMultiSet::GetMultiSetMap()
{
	return &m_MultiSetMap;
}

bool SP_CPN_PlaceMultiSet::RemoveMultiSet(SP_MapString2Int p_ColorMultiset)
{
	SP_MapString2Int::iterator itMap;
	for(itMap = p_ColorMultiset.begin(); itMap != p_ColorMultiset.end(); itMap++)
	{
		m_MultiSetMap[itMap->first] = m_MultiSetMap.find(itMap->first)->second - itMap->second;
		if(m_MultiSetMap.find(itMap->first)->second == 0)
		{
			m_MultiSetMap.erase(m_MultiSetMap.find(itMap->first));
		}
	}
	return true;
}


void SP_CPN_PlaceMultiSet::SetMultiSet(SP_MapString2Int p_ColorMultiset)
{
	m_MultiSetMap.clear();
	m_MultiSetMap = p_ColorMultiset;
}

	
