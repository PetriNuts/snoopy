/*
 * SP_DS_PlaceODE.cpp
 * $Author: F. Liu
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 2.6.2010
 * Short Description:
 */
 //======================================================================================
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/Col_CPN/SP_DS_ColPlaceODE.h"
#include "sp_ds/extensions/continuous/SP_DS_ContinuousPlaceFormula.h"
#include <wx/defs.h>

#include "sp_ds/extensions/Col_CPN/SP_DS_ColCPNPlaceFormula.h"



SP_DS_ColPlaceODE::SP_DS_ColPlaceODE(SP_DS_Graph* p_pcGraph,SP_DS_ColPN_Unfolding* p_pUnfoldedNet)
:SP_DS_PlaceODE(p_pcGraph),
m_pUnfoldedNet(p_pUnfoldedNet)
{	

}


bool SP_DS_ColPlaceODE::LoadPlacesOfType(const wxString& p_sPlacesType)
{
	m_nPlaceCount = m_pUnfoldedNet->GetPlaceNames()->size();

	asPlaceNames.clear();
	asPlaceNames = *(m_pUnfoldedNet->GetPlaceNames());

	for(unsigned i = 0; i < m_nPlaceCount; i ++)
	{
		m_mPlacesPosByID[i] = i;
	}

	return true;
}

bool SP_DS_ColPlaceODE::LoadTransitions()
{

	m_nTransCount = m_pUnfoldedNet->GetTransitionNames()->size();

	if( SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass( SP_DS_CONTINUOUS_TRANS )->GetElements()->size() == 0)
		return false;

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass( SP_DS_CONTINUOUS_TRANS )->GetElements()->begin()))->GetAttribute(wxT("FunctionList")));
	unsigned int l_nCurrentColumn =	l_pcColList->GetActiveColumn();

	m_anTransPosition.resize(m_nTransCount);
	m_asTransRateFunction.resize(m_nTransCount);
	m_anTransitionsType.resize(m_nTransCount);

	for(unsigned i = 0; i < m_nTransCount; i ++)
	{
		m_anTransPosition[i] = i;
		m_mTransitionsPosByID[i] = i;
		m_anTransitionsType[i]=SP_CONTINUOUS_TRANS;

		SP_DS_ColCPNPlaceFormula l_Equation(true,(*(m_pUnfoldedNet->GetNetFunctions()))[i][l_nCurrentColumn]);
		l_Equation.Initialise( m_pUnfoldedNet );

		m_asTransRateFunction[i]=wxT("(")+ l_Equation.ReplacePatternInTransitionFormula( i )  +wxT(")");

	}


	return true;
}

bool SP_DS_ColPlaceODE::LoadConnections()
{
	map<long, vector<SP_CPN_ContinuousArc> >* l_pmvPlaceID_PreTransitionID; // = m_pUnfoldedNet->GetPlaceID_PreTransitionID();
	map<long, vector<SP_CPN_ContinuousArc> >* l_pmvPlaceID_PostTransitionID; // = m_pUnfoldedNet->GetPlaceID_PostTransitionID();
	
	m_a2DPlacePostTransitions.resize(m_nPlaceCount);
    m_a2DPlacePreTransitions.resize(m_nPlaceCount);

	for(unsigned long l_nPlacePos = 0; l_nPlacePos < m_nPlaceCount; l_nPlacePos++)
	{
		if( (*l_pmvPlaceID_PreTransitionID).find(l_nPlacePos) != (*l_pmvPlaceID_PreTransitionID).end() )
		{
			vector<SP_CPN_ContinuousArc> l_ContinuousArc = (*l_pmvPlaceID_PreTransitionID).find(l_nPlacePos)->second;
			for(unsigned i = 0; i < l_ContinuousArc.size(); i++)
			{
				PlaceTransitions l_Trans;
				l_Trans.nTransPos = l_ContinuousArc[i].m_nTranPos;
				l_Trans.sConWeight = l_ContinuousArc[i].m_sMultiplicity;
				l_Trans.ConType = GetConnectionType(l_ContinuousArc[i].m_sArcClass);

				m_a2DPlacePreTransitions[l_nPlacePos].push_back(l_Trans);
			}
		}
	}

	for(unsigned long l_nPlacePos = 0; l_nPlacePos < m_nPlaceCount; l_nPlacePos++)
	{
		if( (*l_pmvPlaceID_PostTransitionID).find(l_nPlacePos) != (*l_pmvPlaceID_PostTransitionID).end() )
		{
			vector<SP_CPN_ContinuousArc> l_ContinuousArc = (*l_pmvPlaceID_PostTransitionID).find(l_nPlacePos)->second;
			for(unsigned i = 0; i < l_ContinuousArc.size(); i++)
			{
				PlaceTransitions l_Trans;
				l_Trans.nTransPos = l_ContinuousArc[i].m_nTranPos;
				l_Trans.sConWeight = l_ContinuousArc[i].m_sMultiplicity;
				l_Trans.ConType = GetConnectionType(l_ContinuousArc[i].m_sArcClass);

				m_a2DPlacePostTransitions[l_nPlacePos].push_back(l_Trans);
			}
		}
	}

	return true;
}


ConnectionType SP_DS_ColPlaceODE::GetConnectionType(wxString p_sArcType)
{
  wxString l_sArcType=p_sArcType;

          //Return the appreciate type based on the arc type
          if(l_sArcType == SP_DS_EDGE)
          {
        	  return SP_STANDARD_ARC;
          }
          else if(l_sArcType==SP_DS_READ_EDGE)
          {
        	  return SP_READ_ARC;
          }
          else if(l_sArcType==SP_DS_INHIBITOR_EDGE)
		  {
		    return SP_INHIBITOR_ARC;
		  }
          else if(l_sArcType ==SP_DS_EQUAL_EDGE)
		  {
			return SP_EQUAL_ARC;
		  }
          else if(l_sArcType ==SP_DS_MODIFIER_EDGE)
		  {
			return SP_MODIFIER_ARC;
		  }
          else
          {
        	return SP_OTHER_CONNECTION;
          }
}


