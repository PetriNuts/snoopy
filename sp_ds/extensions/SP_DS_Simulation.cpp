//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/05/27 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/SP_DS_Simulation.h"
#include "sp_gui/dialogs/SP_DLG_SimulationProperties.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/SP_DS_Metadata.h"

BEGIN_EVENT_TABLE(SP_DS_Simulation, wxEvtHandler)

END_EVENT_TABLE()

SP_DS_Simulation::SP_DS_Simulation(SP_DS_Graph* p_pcGraph)
:m_pcGraph(p_pcGraph),
m_pcSimulationResultsDlg(NULL),
m_bFlagSimulationRunning(false),
m_nIntervalStart(0),
m_nIntervalEnd(0),
m_nIntervalSize(0),
m_nResultPointCount(0),
m_nSimulationRunCount(1),
m_nGeneratedResultPointsCount(0),
m_nCurrentSimulationRunNumber(0),
m_nCurrentIterationNumber(0),
m_nIterationCount(0),
m_nRounds(0),
m_nLastUpdateTime(0),
m_nCurrentTime(0),
m_nLastTime(0),
m_nPlaceCount(0),
m_nTransitionCount(0),
m_eExportType(SP_SIM_NO_EXPORT),
m_aanDeltaIncMatrix( 0), m_aanResetIncMatrix(0), m_aanPreIncMatrix( 0)
{
	m_cSimulationStopWatch.Pause();
}

SP_DS_Simulation::~SP_DS_Simulation()
{
}

bool SP_DS_Simulation::AddToSimulationProperties(SP_DLG_SimulationProperties* p_pcDlg, wxSizer* p_pcSizer)
{
	wxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(new wxStaticText( p_pcDlg, -1, wxT("Simulation run count:") ), 1, wxALL | wxEXPAND, 5);
	m_pcSimRunCountTextCtrl = new wxTextCtrl( p_pcDlg, -1, wxString::Format(wxT("%d"), m_nSimulationRunCount), wxDefaultPosition, wxDefaultSize, 0 );
	l_pcRowSizer->Add(m_pcSimRunCountTextCtrl, 0, wxALL, 5);
	p_pcSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL, 5);
	return true;
}

bool SP_DS_Simulation::OnSimulationPropertiesOk()
{
	long l_nValue = 0;

	if(m_pcSimRunCountTextCtrl->GetValue().ToLong(&l_nValue) && l_nValue > 0)
	{
		m_nSimulationRunCount = l_nValue;
/*
		SP_DS_Metadata* l_pcSimProp = *(m_pcGraph->GetMetadataclass(wxT("Simulation Properties"))->GetElements()->begin());
		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcSimProp->GetAttribute(wxT("simulator properties")));
		bool l_bFound = false;
		for(unsigned int i = 0; i < l_pcAttr->GetRowCount(); ++i)
		{
			wxString l_sKey = l_pcAttr->GetCell(i,0);
			if(l_sKey == wxT("simulation run count"))
			{
				l_bFound = true;
				l_pcAttr->SetCell(i,1, m_pcSimRunCountTextCtrl->GetValue());
			}
		}
		if(!l_bFound)
		{
			unsigned int l_nRow = l_pcAttr->AppendEmptyRow();
			l_pcAttr->SetCell(l_nRow, 0, wxT("simulation run count"));
			l_pcAttr->SetCell(l_nRow, 1, m_pcSimRunCountTextCtrl->GetValue());
		}
*/
	}
	else
	{
		SP_MESSAGEBOX(wxT("Simulation run count has to be a number greater than zero!"), wxT("Error"));
		return false;
	}

	return true;
}

void SP_DS_Simulation::UpdateSimulationDialog(unsigned long p_nSimulationRun)
{
	bool l_bUpdate = false;
	long l_nUpdateTime = m_cSimulationStopWatch.Time();
	if((m_nLastUpdateTime + 5000) < l_nUpdateTime
		|| m_nLastUpdateTime > l_nUpdateTime)
	{
		l_bUpdate = true;
		m_nLastUpdateTime = l_nUpdateTime;
	}

	while(m_nCurrentIterationNumber < ((m_nCurrentTime+(m_nIntervalEnd*p_nSimulationRun))/(m_nIntervalEnd*m_nSimulationRunCount) * 100))
	{
		if((m_nCurrentIterationNumber % 10) == 0)
		{
			l_bUpdate = true;
		}
		++m_nCurrentIterationNumber;
//		m_nRounds = 0;
	}

	if(l_bUpdate)
	{
		m_pcSimulationResultsDlg->SetSimulationProgressGauge(m_nCurrentIterationNumber);
		m_pcSimulationResultsDlg->SetSimulationStopWatch(l_nUpdateTime);

		CHECK_EVENTS(IsSimulationRunning(), return);
	}
/*
	if(m_nRounds == 10000000)
	{
		int l_Return = SP_MESSAGEBOX(wxT("Possible time-deadlock detected! To Abort the simulation press YES."), wxT("Warning"), wxYES_NO | wxYES_DEFAULT);
		if(l_Return	== wxYES)
		{
			AbortSimulation();
		}
		m_nRounds = 0;
	}
	++m_nRounds;
*/
}

void SP_DS_Simulation::SetUnFoldingInformation()
{
		m_mnPlaceArrayPos = *(m_pcUnfoldedNet->GetPlaceArrayPos());
		m_msPlaceNames = *(m_pcUnfoldedNet->GetPlaceNames());
		m_msPlaceNamesById = *(m_pcUnfoldedNet->GetPlaceNamesById());
		m_anNetMarkings = *(m_pcUnfoldedNet->GetNetMarkings());
		m_anNetMarkings = *(m_pcUnfoldedNet->GetNetMarkings());
		m_msTransitionNames = *(m_pcUnfoldedNet->GetTransitionNames());
		m_mnTransitionArrayPos = *(m_pcUnfoldedNet->GetTransitionArrayPos());
		m_msTransitionNamesById = *(m_pcUnfoldedNet->GetTransitionNamesById());
		m_anNetFunctions = *(m_pcUnfoldedNet->GetNetFunctions());
		m_mmPrePlaceArrayPosOfTransitionNode = *(m_pcUnfoldedNet->GetPrePlace());
		//m_nsTransitionCountByNodeClass = *(m_pcUnfoldedNet->GetTransitionCountByNodeClass());
		

		//long l_nTransitionCount = m_mnTransitionArrayPos.size();
		//long l_nPlaceCount = m_mnPlaceArrayPos.size();

		//m_aanDeltaIncMatrix = new SP_MatrixDouble(l_nTransitionCount, l_nPlaceCount, 0);
		//m_aanResetIncMatrix = new SP_MatrixBool(l_nTransitionCount, l_nPlaceCount, false);
		//m_aanPreIncMatrix = new SP_MatrixPairDouble(l_nTransitionCount, l_nPlaceCount, SP_PairDouble(0,LONG_MAX));

		//*m_aanDeltaIncMatrix = *(*(m_pcUnfoldedNet->GetDeltaIncMatrix()));
		//*m_aanResetIncMatrix = *(*(m_pcUnfoldedNet->GetResetIncMatrix()));
		//*m_aanPreIncMatrix = *(*(m_pcUnfoldedNet->GetPreIncMatrix()));

		//m_abNonConsumingMode = *(m_pcUnfoldedNet->GetNonConsumingMode());
}
