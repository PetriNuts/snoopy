/*
 * SP_DS_ContinuousPlaceFormula.cpp
 * $Author: F. Liu
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 4.6.2010
 * Short Description:
 */
 //======================================================================================
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/Col_CPN/SP_DS_ColCPNPlaceFormula.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include<boost/spirit/home/classic/core/parser.hpp>

/*
 * Begin IL formula grammar definition
 */
using namespace rfdag;


bool
SP_DS_ColCPNPlaceFormula::Initialise(SP_DS_ColPN_Unfolding* p_pUnfoldedNet)
{
	
	m_pUnfoldedNet=p_pUnfoldedNet;
	LoadUnfoldedNet();

	LoadParameters();

	if( m_bFirstTime ) 
	{
		LoadColoredPlace();
	}
	else
	{
		LoadUnfoldedPlace();
	}

	m_bPreArcFlag = false;
	

	return true;
}




void
SP_DS_ColCPNPlaceFormula::SetNamesColored(RFDAG& p_Expression)
{
	switch(p_Expression.token)
	{
	case IDENT:
	case PLACE:
		SetPlaceName( p_Expression );
		break;
	case PARAMETER:		
		break;
	default:
		children_t::iterator it = p_Expression.children.begin();
		for(; it != p_Expression.children.end(); ++it)
		{
			SetNamesColored(*it);
		}
	}
}


void
SP_DS_ColCPNPlaceFormula::SetPlaceName(RFDAG& p_Expression)
{
	//SP_MapString2String l_Place2ColorMap;
	//l_Place2ColorMap = (*m_pmsPlace2Color)[m_nTransitionID];

	wxString l_sColoredPlaceName = m_Id2Placename[p_Expression.placeid];	
	
/*			
	if(l_Place2ColorMap.find(l_sColoredPlaceName) != l_Place2ColorMap.end())
	{	
		wxString l_sColor = l_Place2ColorMap.find(l_sColoredPlaceName)->second;
		wxString l_sUnfoldedPlaceName = l_sColoredPlaceName + wxT("_") + l_sColor; //l_Place2ColorMap[p_sVariable];
				
		if(m_pmnPlaceArrayPos->find(l_sUnfoldedPlaceName) != m_pmnPlaceArrayPos->end())
		{
			
			p_Expression.placename = l_sUnfoldedPlaceName.mb_str();
			p_Expression.placeid = m_pmnPlaceArrayPos->find(l_sUnfoldedPlaceName)->second;
					
		}
	}
*/	
	//for the special case of rate function definition like P1[c1]*p1[c2]
	if(m_pmnPlaceArrayPos->find(l_sColoredPlaceName) != m_pmnPlaceArrayPos->end())
	{
		p_Expression.placename = m_pmnPlaceArrayPos->find(l_sColoredPlaceName)->first.mb_str();
		p_Expression.placeid = m_pmnPlaceArrayPos->find(l_sColoredPlaceName)->second;
	}
}






SP_DS_ColCPNPlaceFormula::SP_DS_ColCPNPlaceFormula(bool p_bFirstTime, const wxString& l_sFormula,const wxString & p_sPlaceType)
:SP_DS_ContinuousPlaceFormula(l_sFormula,p_sPlaceType),
m_bFirstTime(p_bFirstTime)
{ 
	
}
SP_DS_ColCPNPlaceFormula::~SP_DS_ColCPNPlaceFormula()
{

}

 wxString SP_DS_ColCPNPlaceFormula::ReplacePatternInTransitionFormula(long p_nTransitionID)
 {
	 m_nTransitionID = p_nTransitionID;
	//for the first parser
	 //LoadColoredPlace();

	if( ! Parse() )
	{
		 SP_LOGERROR( wxT("Error in the transition's Formula: ")); //+  dynamic_cast<SP_DS_NameAttribute*>((p_pcTransNode)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue());
		 return(wxT("0"));
	 }

	//replace colored place names with unfolded place names

	SetNamesColored(*m_Expression);

	//eliminate patterns

	//reset these two variables to the unfolded information
	m_Id2Placename.clear();
	m_Placename2Id.clear();

	m_Placename2Id = *m_pmnPlaceArrayPos;
	m_Id2Placename = *m_pmsPlaceNames;


	 SP_MapString2Long l_mPrePlaces = (*m_pmmPrePlaceArrayPosOfTransitionNode)[ p_nTransitionID ];
	 SP_MapString2Long::iterator itMap;
	 for(itMap = l_mPrePlaces.begin(); itMap != l_mPrePlaces.end(); itMap++)
	 {
		 m_vsTransitionPrePlaces.push_back( itMap->first);
	 }
	
	m_mPlaceTransitionMultiplicity = (*m_pmPrePlaceToArcsOfTransitionNode)[ p_nTransitionID ];	


	 return (EliminatePattern(*m_Expression));

	 
	 
 }

 void
SP_DS_ColCPNPlaceFormula::LoadUnfoldedPlace()
{		
	
	ClearclientContinuousPlaces();
	m_Id2Placename.clear();
	m_Placename2Id.clear();

	m_Placename2Id = *m_pmnPlaceArrayPos;
	m_Id2Placename = *m_pmsPlaceNames;

	for(unsigned i = 0; i < m_Id2Placename.size(); i++)
	{
		wxString l_sPlaceName = m_Id2Placename[i];
		unsigned l_nPos = i;
		AddclientContinuousPlaces(l_sPlaceName.mb_str(), l_nPos);
	}
	

}

  void
SP_DS_ColCPNPlaceFormula::LoadColoredPlace()
{
	ClearclientContinuousPlaces();
	
	m_Id2Placename.clear();
	m_Placename2Id.clear();

	m_Id2Placename = *m_pmsColoredPlaceNames;
	for(unsigned i = 0; i < m_Id2Placename.size(); i++)
	{
		wxString l_sPlaceName = m_Id2Placename[i];
		unsigned l_nPos = i;
		m_Placename2Id[l_sPlaceName] = l_nPos;
		AddclientContinuousPlaces(l_sPlaceName.mb_str(), l_nPos);
	}

	//
	
	for(unsigned i = 0; i < (*m_pmsPlaceNames).size(); i++ )
	{
		wxString l_sPlaceName = (*m_pmsPlaceNames)[i];
		unsigned l_nPos = i + (*m_pmsColoredPlaceNames).size();
		m_Placename2Id[l_sPlaceName] = l_nPos;
		m_Id2Placename.push_back(l_sPlaceName);
		AddclientContinuousPlaces(l_sPlaceName.mb_str(), l_nPos);

	}

 
}

  void SP_DS_ColCPNPlaceFormula::LoadUnfoldedNet()
  {
	  m_pmmPrePlaceArrayPosOfTransitionNode = m_pUnfoldedNet->GetPrePlace();
	  //m_pmPrePlaceToArcsOfTransitionNode = m_pUnfoldedNet->GetPlaceToArcsOfTransitionNode();

	  //m_pmPostPlace2ArcsOfTransitionNode = m_pUnfoldedNet->GetPostPlace2ArcsOfTransitionNode();
	  m_pmnPlaceArrayPos = m_pUnfoldedNet->GetPlaceArrayPos();
	  m_pmsPlaceNames = m_pUnfoldedNet->GetPlaceNames();
	 // m_pmsPlace2Color = m_pUnfoldedNet->GetPlace2Color();
	  m_pmsColoredPlaceNames = m_pUnfoldedNet->GetColoredPlaceNames();
  
  }

/*
  bool
SP_DS_ColCPNPlaceFormula::Parse()
{
	bool l_Return = ParseforColCPN();
	
	
	return l_Return;
}
*/
