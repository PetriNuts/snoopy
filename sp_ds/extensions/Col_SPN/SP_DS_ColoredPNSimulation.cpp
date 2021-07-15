//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2011/08/02 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/SP_DS_Simulation.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColoredPNSimulation.h"
#include "sp_core/tools/SP_NetUnfolder.h"

SP_DS_ColoredPNSimulation::SP_DS_ColoredPNSimulation(SP_DS_ColPN_Unfolding* p_pcUnfoldedNet)
:m_pcUnfoldedNet(p_pcUnfoldedNet)
{	
	if( !LoadUnfoldingData() )
			return;
}

SP_DS_ColoredPNSimulation::~SP_DS_ColoredPNSimulation()
{
   wxDELETE(m_pcUnfoldedNet);
}

bool SP_DS_ColoredPNSimulation::LoadUnfoldingData()
{
	m_mnPlaceArrayPos_FromUnfolding.clear();
	m_msPlaceNamesById_FromUnfolding.clear();	
	m_msPlaceNames.clear();
	m_msColoredPlaceNames.clear();
	m_anNetMarkings.clear();
	m_pnsPlaceCountById.clear();

	m_pnsTransitionCountById.clear();

	m_mnTransitionArrayPos.clear();
	m_msTransitionNamesById.clear();
	m_msTransitionNames.clear();
	m_msColoredTransitionNames.clear();

	m_anNetFunctions.clear();
	m_mmPrePlaceArrayPosOfTransitionNode.clear();

    if(m_pcUnfoldedNet==NULL)
    {
    	SP_LOGERROR(wxT("unexpected error, cannot locate the unfolding object"));

    	return false;
    }

	m_mnPlaceArrayPos_FromUnfolding = *(m_pcUnfoldedNet->GetPlaceArrayPos());
	m_msPlaceNamesById_FromUnfolding = *(m_pcUnfoldedNet->GetPlaceNamesById());	
	m_msPlaceNames = *(m_pcUnfoldedNet->GetPlaceNames());	
	m_msColoredPlaceNames =  *(m_pcUnfoldedNet->GetColoredPlaceNames());	
	m_anNetMarkings = *(m_pcUnfoldedNet->GetNetMarkings());
	m_pnsPlaceCountById = *(m_pcUnfoldedNet->GetPlaceCountById());	
	m_msColorsOfPlace = *(m_pcUnfoldedNet->GetColorsOfPlace());

	m_pnsTransitionCountById = *(m_pcUnfoldedNet->GetTransitionCountById());	

	//m_nsTransitionCountByNodeClass = *(m_pcUnfoldedNet->GetTransitionCountByNodeClass());	

	m_mnTransitionArrayPos = *(m_pcUnfoldedNet->GetTransitionArrayPos());
	m_msTransitionNamesById = *(m_pcUnfoldedNet->GetTransitionNamesById());
	m_msTransitionNames = *(m_pcUnfoldedNet->GetTransitionNames());
	m_msColoredTransitionNames = *(m_pcUnfoldedNet->GetColoredTransitionNames());


	m_anNetFunctions = *(m_pcUnfoldedNet->GetNetFunctions());

	m_mmPrePlaceArrayPosOfTransitionNode = *(m_pcUnfoldedNet->GetPrePlace());
	

	return true;
}


void SP_DS_ColoredPNSimulation::GetColPlaceResults(const SP_Vector2DDouble& p_aanResults,SP_Vector2DDouble& p_aanColPlaceResults)
{	
	p_aanColPlaceResults.clear();

	for (unsigned long l_nRow = 0; l_nRow < p_aanResults.size(); l_nRow++)	
	{
		vector<double> l_vOneRowResult;
		for( unsigned int  l_nPos = 0; l_nPos < m_msColoredPlaceNames.size(); l_nPos++ )
		{
			double l_dResult = 0;
			for(unsigned int l_nCol = m_pnsPlaceCountById[l_nPos].m_nLow;  l_nCol <= m_pnsPlaceCountById[l_nPos].m_nUp; l_nCol++)
			{
				l_dResult = l_dResult + p_aanResults[l_nRow][l_nCol];
			}
			l_vOneRowResult.push_back(l_dResult);
		}
		p_aanColPlaceResults.push_back(l_vOneRowResult);
	}
}


void SP_DS_ColoredPNSimulation::GetColTransResults(const SP_Vector2DDouble& p_aanResults,SP_Vector2DDouble& p_aanColTransResults)
{
	//TODO: use assign instead
	p_aanColTransResults.clear();

	for (unsigned long l_nRow = 0; l_nRow < p_aanResults.size(); l_nRow++)	
	{
		vector<double> l_vOneRowResult;
		for( unsigned int  l_nPos = 0; l_nPos < m_msColoredTransitionNames.size(); l_nPos++ )
		{
			double l_dResult = 0;
			for(unsigned int l_nCol = m_pnsTransitionCountById[l_nPos].m_nLow;  l_nCol <= m_pnsTransitionCountById[l_nPos].m_nUp; l_nCol++)
			{
				l_dResult = l_dResult + p_aanResults[l_nRow][l_nCol];
			}
			l_vOneRowResult.push_back(l_dResult);
		}

		p_aanColTransResults.push_back(l_vOneRowResult);
	}

}




bool SP_DS_ColoredPNSimulation::LoadUnfoldedPlaces()
{
	//if( ! m_pcUnfoldedNet->UnfoldPlace(true) )
	wxDELETE(m_pcUnfoldedNet);
	m_pcUnfoldedNet = new SP_DS_ColPN_Unfolding();
	if( ! m_pcUnfoldedNet->Unfolding() )
	{
		return false;
	}

	m_mnPlaceArrayPos_FromUnfolding = *(m_pcUnfoldedNet->GetPlaceArrayPos());
	m_msPlaceNamesById_FromUnfolding = *(m_pcUnfoldedNet->GetPlaceNamesById());	
	m_msPlaceNames = *(m_pcUnfoldedNet->GetPlaceNames());	
	m_msColoredPlaceNames =  *(m_pcUnfoldedNet->GetColoredPlaceNames());	
	m_anNetMarkings = *(m_pcUnfoldedNet->GetNetMarkings());
	m_pnsPlaceCountById = *(m_pcUnfoldedNet->GetPlaceCountById());	

	m_msColorsOfPlace = *(m_pcUnfoldedNet->GetColorsOfPlace());

	return true;

}


dssd::andl::Net_ptr SP_DS_ColoredPNSimulation::CreateANDLNet(SP_DS_Graph* p_pcGraph, bool p_bshowLog)
{
	if (p_pcGraph == NULL)
		return NULL;

//	SP_SimpleNetBuilder builder;
	SP_IddNetUnfolder unfolder;
	if (unfolder(p_pcGraph,false,false,p_bshowLog))

	 {
		return unfolder.GetUnfoldedNet();
	 }
	 return NULL;
}
