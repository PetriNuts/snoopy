
#include "sp_ds/extensions/SP_DS_Simulation.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColoredPNDirSimulation.h"

SP_DS_ColoredPNDirSimulation::SP_DS_ColoredPNDirSimulation(SP_DS_ColPN_Coloring* p_pcColoredNet)
	:m_pcColoredNet(p_pcColoredNet)
{
	if (!LoadColoringData())
		return;
}

SP_DS_ColoredPNDirSimulation::~SP_DS_ColoredPNDirSimulation()
{
	wxDELETE(m_pcColoredNet);
}

bool SP_DS_ColoredPNDirSimulation::LoadColoringData()
{
	
	m_vcolplaceNode.clear();
	m_msColoredPlaceNames.clear();
	m_msUncolPlaceNames.clear();
	m_msUncoloredPlaceNames.clear();
	m_anNetMarkings.clear();
	m_pnsPlaceCountById.clear();

	m_pnsTransitionCountById.clear();

	
	m_msTransitionNamesById.clear();
	
	m_msColoredTransitionNames.clear();

	m_anNetFunctions.clear();
	m_mmPrePlaceArrayPosOfTransitionNode.clear();

	if (m_pcColoredNet == NULL)
	{
		SP_LOGERROR(wxT("unexpected error, cannot locate the Colored Net"));

		return false;
	}

	
	m_msColoredPlaceNames = *(m_pcColoredNet->GetColoredPlaceNames());
	m_msUncolPlaceNames = *(m_pcColoredNet->GetPlaceNames());
	m_msuncoloredplaces = *(m_pcColoredNet->GetUncolorplaces());
	m_msColSetPlaceNames = *(m_pcColoredNet->GetColorSetOfPlace());
	m_msUncoloredPlaceNames = *(m_pcColoredNet->GetColorSetOfPlaceUpdate());
	m_anNetMarkings = *(m_pcColoredNet->GetmarkingDouble());
	m_pnsPlaceCountById = *(m_pcColoredNet->GetPlaceCountById());
	m_msColorsOfPlace = *(m_pcColoredNet->GetColorSetOfPlace());
	m_vcolplaceNode = *(m_pcColoredNet->GetplaceNode());
	m_pnsTransitionCountById = *(m_pcColoredNet->GetTransitionCountById());

	
	m_msColoredTransitionNames = *(m_pcColoredNet->GetColoredTransitionNames());


	m_anNetFunctions = *(m_pcColoredNet->GetColTransRatFun());

	m_mmPrePlaceArrayPosOfTransitionNode = *(m_pcColoredNet->GetPrePlace());


	return true;
}


void SP_DS_ColoredPNDirSimulation::GetColPlaceResults(const SP_Vector2DDouble& p_aanResults, SP_Vector2DDouble& p_aanColPlaceResults)
{
	p_aanColPlaceResults.clear();

	for (unsigned long l_nRow = 0; l_nRow < p_aanResults.size(); l_nRow++)
	{
		vector<double> l_vOneRowResult;
		for (unsigned int l_nPos = 0; l_nPos < m_msColoredPlaceNames.size(); l_nPos++)
		{
			double l_dResult = 0;
			for (unsigned int l_nCol = m_pnsPlaceCountById[l_nPos].m_nLow; l_nCol <= m_pnsPlaceCountById[l_nPos].m_nUp; l_nCol++)
			{
				l_dResult = l_dResult + p_aanResults[l_nRow][l_nCol];
			}
			l_vOneRowResult.push_back(l_dResult);
		}
		p_aanColPlaceResults.push_back(l_vOneRowResult);
	}
}


void SP_DS_ColoredPNDirSimulation::GetColTransResults(const SP_Vector2DDouble& p_aanResults, SP_Vector2DDouble& p_aanColTransResults)
{
	//TODO: use assign instead
	p_aanColTransResults.clear();

	for (unsigned long l_nRow = 0; l_nRow < p_aanResults.size(); l_nRow++)
	{
		vector<double> l_vOneRowResult;
		for (unsigned int l_nPos = 0; l_nPos < m_msColoredTransitionNames.size(); l_nPos++)
		{
			double l_dResult = 0;
			for (unsigned int l_nCol = m_pnsTransitionCountById[l_nPos].m_nLow; l_nCol <= m_pnsTransitionCountById[l_nPos].m_nUp; l_nCol++)
			{
				l_dResult = l_dResult + p_aanResults[l_nRow][l_nCol];
			}
			l_vOneRowResult.push_back(l_dResult);
		}

		p_aanColTransResults.push_back(l_vOneRowResult);
	}

}




