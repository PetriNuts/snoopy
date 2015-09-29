/*
 * SP_DLG_ColHPNSimultionResults.h
 * $Author: F. Liu
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 6.12.2011
 * Short Description:
 */
 //==============================================================================

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/valgen.h>
#include <wx/gbsizer.h>
#include <math.h>
#include <wx/ffile.h>
#include <wx/busyinfo.h>
#include <wx/regex.h>
#include "sp_core/base/SP_Error.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_gui/dialogs/dia_ColHPN/SP_DLG_ColHPNSimultionResults.h"

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StParameterOverview.h"
#include "sp_gui/dialogs/SP_DLG_ColListOverview.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_core/SP_Core.h"

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "sp_ds/extensions/HybridPN/SP_DS_SimulatorThreadHybrid.h"

#include "spsim/spsim.h"


//Special for colored PN
#include "sp_ds/extensions/Col_CPN/SP_DS_ColPlaceODE.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStMarkingOverview.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStFunctionOverview.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColPlacesSelection.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColTraceAnalyzer.h"

IMPLEMENT_CLASS(SP_DLG_ColHPNSimultionResults, SP_DLG_HybridSimulationResults )

enum
{
   SP_ID_BUTTON_MODIFY_MARKING_SETS = SP_ID_LAST_ID + 200,
   SP_ID_BUTTON_MODIFY_FUNCTION_SETS,
   SP_ID_BUTTON_MODIFY_PARAMETER_SETS,
   SP_ID_BUTTON_MODIFY_STOCHASTIC_RATE_SETS,
   SP_ID_COMBOBOX_MARKING_SETS,
   SP_ID_COMBOBOX_FUNCTION_SETS,
   SP_ID_COMBOBOX_PARAMETER_SETS,
   SP_ID_COMBOBOX_TIME_SYNC,
   SP_ID_COMBOBOX_DELAY_SETS,
   SP_ID_COMBOBOX_IMMEDIATE_SETS,
   SP_ID_COMBOBOX_SCHEDULED_SETS,
   SP_ID_COMBOBOX_STOCHASTIC_RATE_SETS,
   SP_ID_COMBOBOX_SOLVER,
   SP_ID_BUTTON_SIMULATION_PROPERTIES,
   SP_ID_BUTTON_SAVE_ODE,
   SP_ID_COMBOBOX_STOCHASTIC_SIMULATOR,
   SP_ID_BUTTON_CONTINUOUS_SOLVER_PROPERTIES,
   SP_ID_BUTTON_MODIFY_DELAY_SETS,
   SP_ID_BUTTON_MODIFY_IMMDIATE_SETS,
   SP_ID_BUTTON_MODIFY_SCHEDULED_SETS

};
BEGIN_EVENT_TABLE( SP_DLG_ColHPNSimultionResults, SP_DLG_HybridSimulationResults )

EVT_BUTTON( SP_ID_BUTTON_MODIFY_MARKING_SETS, SP_DLG_ColHPNSimultionResults :: OnModifyMarkingSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_FUNCTION_SETS, SP_DLG_ColHPNSimultionResults :: OnModifyFunctionSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_PARAMETER_SETS, SP_DLG_ColHPNSimultionResults :: OnModifyParameterSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_STOCHASTIC_RATE_SETS, SP_DLG_ColHPNSimultionResults :: OnModifyStochasicSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_DELAY_SETS, SP_DLG_ColHPNSimultionResults :: OnModifyDelaySets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_IMMDIATE_SETS, SP_DLG_ColHPNSimultionResults :: OnModifyImmediateSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_SCHEDULED_SETS, SP_DLG_ColHPNSimultionResults :: OnModifyScheduledSets )

EVT_COMBOBOX( SP_ID_COMBOBOX_MARKING_SETS, SP_DLG_ColHPNSimultionResults::OnMarkingSetChanged )
EVT_COMBOBOX( SP_ID_COMBOBOX_FUNCTION_SETS, SP_DLG_ColHPNSimultionResults::OnFunctionSetChanged )
EVT_COMBOBOX( SP_ID_COMBOBOX_STOCHASTIC_RATE_SETS, SP_DLG_ColHPNSimultionResults::OnStochasticRateChanged )
EVT_COMBOBOX( SP_ID_COMBOBOX_IMMEDIATE_SETS, SP_DLG_ColHPNSimultionResults::OnImmediateSetChanged )
EVT_COMBOBOX( SP_ID_COMBOBOX_DELAY_SETS, SP_DLG_ColHPNSimultionResults::OnDelaySetChanged )
EVT_COMBOBOX( SP_ID_COMBOBOX_SCHEDULED_SETS, SP_DLG_ColHPNSimultionResults::OnScheduledSetChanged )

EVT_COMBOBOX( SP_ID_COMBOBOX_PARAMETER_SETS, SP_DLG_ColHPNSimultionResults::OnParameterSetChanged )
EVT_COMBOBOX( SP_ID_COMBOBOX_DELAY_SETS, SP_DLG_ColHPNSimultionResults::OnParameterSetChanged )
EVT_COMBOBOX( SP_ID_COMBOBOX_SOLVER, SP_DLG_ColHPNSimultionResults::OnSolverChanged )
EVT_COMBOBOX( SP_ID_COMBOBOX_TIME_SYNC, SP_DLG_ColHPNSimultionResults::OnTimeSyncTypeChanged )
EVT_BUTTON( SP_ID_BUTTON_SIMULATION_PROPERTIES, SP_DLG_ColHPNSimultionResults :: OnSimulationProperties )
EVT_BUTTON( SP_ID_BUTTON_CONTINUOUS_SOLVER_PROPERTIES, SP_DLG_ColHPNSimultionResults :: OnODESolverProperties )
EVT_BUTTON( SP_ID_BUTTON_SAVE_ODE, SP_DLG_ColHPNSimultionResults :: SaveODE )

EVT_SIMTHREAD(SP_SIMULATION_THREAD_EVENT,SP_DLG_HybridSimulationResults::OnSimulatorThreadEvent)

END_EVENT_TABLE()

SP_DLG_ColHPNSimultionResults::SP_DLG_ColHPNSimultionResults(
		SP_DS_Graph* p_pcGraph, SP_DS_ColPN_Unfolding* p_pcUnfoldedNet,wxWindow* p_pcParent,
			wxString p_sHelpText,  wxString p_sTitle, long p_nStyle)
:SP_DS_ColoredPNSimulation(p_pcUnfoldedNet),
SP_DLG_HybridSimulationResults( p_pcGraph,p_pcParent,p_sHelpText,p_sTitle,p_nStyle )
{
	m_pcXAxisChoices.clear();
	m_pcXAxisChoices[wxT("Place-colored")]=& m_msColoredPlaceNames;
	m_pcXAxisChoices[wxT("Transition-colored")]=&m_msColoredTransitionNames;
	m_pcXAxisChoices[wxT("Place-uncolored")]=&m_msPlaceNames;
	m_pcXAxisChoices[wxT("Transition-uncolored")]=&m_msTransitionNames;

	wxSizer* l_pcRowSizer = NULL;

	//Marking Set
	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add( new wxStaticText( m_pcPropertyWindowSetsSizer, -1, wxT("Marking set:") ), 1, wxALL | wxEXPAND, 5 );
	m_pcMarkingSetComboBox = new wxComboBox( m_pcPropertyWindowSetsSizer, SP_ID_COMBOBOX_MARKING_SETS, wxT(""), wxDefaultPosition, wxSize(100,-1), 0, NULL, wxCB_READONLY );
	l_pcRowSizer->Add( m_pcMarkingSetComboBox, 0, wxALL, 5 );
	l_pcRowSizer->Add( new wxButton( m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_MARKING_SETS, wxT("Modify") ), 0, wxALL, 5 );
	m_pcSetsSizer->Add( l_pcRowSizer, 1, wxEXPAND );

	//Function set
	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add( new wxStaticText( m_pcPropertyWindowSetsSizer, -1, wxT("Continuous rate:") ), 1, wxALL | wxEXPAND, 5 );
	m_pcFunctionSetComboBox = new wxComboBox( m_pcPropertyWindowSetsSizer, SP_ID_COMBOBOX_FUNCTION_SETS, wxT(""), wxDefaultPosition, wxSize(100,-1), 0, NULL, wxCB_READONLY );
	l_pcRowSizer->Add( m_pcFunctionSetComboBox, 1, wxALL, 5 );
	l_pcRowSizer->Add( new wxButton( m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_FUNCTION_SETS, wxT("Modify") ), 0, wxALL, 5 );
	m_pcSetsSizer->Add( l_pcRowSizer, 1, wxEXPAND );

	//Rate set
	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add( new wxStaticText( m_pcPropertyWindowSetsSizer, -1, wxT("Stochastic  rate:") ), 1, wxALL | wxEXPAND, 5 );
	m_pcStochasticRateSetComboBox = new wxComboBox( m_pcPropertyWindowSetsSizer, SP_ID_COMBOBOX_STOCHASTIC_RATE_SETS, wxT(""), wxDefaultPosition, wxSize(100,-1), 0, NULL, wxCB_READONLY );
	l_pcRowSizer->Add( m_pcStochasticRateSetComboBox, 1, wxALL, 5 );
	l_pcRowSizer->Add( new wxButton( m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_STOCHASTIC_RATE_SETS, wxT("Modify") ), 0, wxALL, 5 );
	m_pcSetsSizer->Add( l_pcRowSizer, 1, wxEXPAND );

	//immediate Set
	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add( new wxStaticText( m_pcPropertyWindowSetsSizer, -1, wxT("Immediate set:") ), 1, wxALL | wxEXPAND, 5 );
	m_pcImmediateSetComboBox = new wxComboBox( m_pcPropertyWindowSetsSizer, SP_ID_COMBOBOX_IMMEDIATE_SETS, wxT(""), wxDefaultPosition, wxSize(100,-1), 0, NULL, wxCB_READONLY );
	l_pcRowSizer->Add( m_pcImmediateSetComboBox, 1, wxALL, 5 );
	l_pcRowSizer->Add( new wxButton( m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_IMMDIATE_SETS, wxT("Modify") ), 0, wxALL, 5 );
	m_pcSetsSizer->Add( l_pcRowSizer, 1, wxEXPAND );

	//delay Set
	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add( new wxStaticText( m_pcPropertyWindowSetsSizer, -1, wxT("Delay set:") ), 1, wxALL | wxEXPAND, 5 );
	m_pcDelaySetComboBox = new wxComboBox( m_pcPropertyWindowSetsSizer, SP_ID_COMBOBOX_DELAY_SETS, wxT(""), wxDefaultPosition, wxSize(100,-1), 0, NULL, wxCB_READONLY );
	l_pcRowSizer->Add( m_pcDelaySetComboBox, 1, wxALL, 5 );
	l_pcRowSizer->Add( new wxButton( m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_DELAY_SETS, wxT("Modify") ), 0, wxALL, 5 );
	m_pcSetsSizer->Add( l_pcRowSizer, 1, wxEXPAND );


	//scheduled Set
	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add( new wxStaticText( m_pcPropertyWindowSetsSizer, -1, wxT("Scheduled set:") ), 1, wxALL | wxEXPAND, 5 );
	m_pcScheduledSetComboBox = new wxComboBox( m_pcPropertyWindowSetsSizer, SP_ID_COMBOBOX_SCHEDULED_SETS, wxT(""), wxDefaultPosition, wxSize(100,-1), 0, NULL, wxCB_READONLY );
	l_pcRowSizer->Add( m_pcScheduledSetComboBox, 1, wxALL, 5 );
	l_pcRowSizer->Add( new wxButton( m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_SCHEDULED_SETS, wxT("Modify") ), 0, wxALL, 5 );
	m_pcSetsSizer->Add( l_pcRowSizer, 1, wxEXPAND );

	//Parameter Set
	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add( new wxStaticText( m_pcPropertyWindowSetsSizer, -1, wxT("Parameter set:") ), 1, wxALL | wxEXPAND, 5 );
	m_pcParameterSetComboBox = new wxComboBox( m_pcPropertyWindowSetsSizer, SP_ID_COMBOBOX_PARAMETER_SETS, wxT(""), wxDefaultPosition, wxSize(100,-1), 0, NULL, wxCB_READONLY );
	l_pcRowSizer->Add( m_pcParameterSetComboBox, 1, wxALL, 5 );
	l_pcRowSizer->Add( new wxButton( m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_PARAMETER_SETS, wxT("Modify") ), 0, wxALL, 5 );
	m_pcSetsSizer->Add( l_pcRowSizer, 1, wxEXPAND );

	 //At the end call this function for alignment
	 SetSizerAndFit( m_pcMainSizer);

}


SP_DLG_ColHPNSimultionResults::~SP_DLG_ColHPNSimultionResults()
{
	    
}

bool SP_DLG_ColHPNSimultionResults::LoadViewerData(SP_DS_ResultViewer* p_pcViewer,SP_DS_Metadata*  p_pcView, wxArrayString& p_asPlaces)
{
	wxString l_sName;
	long l_nPos = 0;


	CHECK_POINTER(p_pcView,return false);

	//get the current nodeclass type
	SP_DS_Attribute* l_pcAttribute=p_pcView->GetAttribute(wxT("Nodeclass"));
	CHECK_POINTER(l_pcAttribute,return false);
	wxString l_sElementType=l_pcAttribute->GetValueString();

	l_nPos = 0;
	
	CHECK_POINTER(p_pcViewer,return false);

	//remove old curves
	p_pcViewer->ClearCurves();

	//remove shown curves
	p_pcViewer->ClearShownCurves();

	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*> (p_pcView->GetAttribute(wxT("CurveInfo")));
	CHECK_POINTER(l_pcCurveInfoList,return false);

	m_ArrayUnPlaces.Clear();
	m_ArrayUnTranstions.Clear();
	m_ArrayColPlaces.Clear();
	m_ArrayColTranstions.Clear();
	m_ArrayAuxPlaces.Clear();
	m_ArrayAuxtranstions.Clear();

	for (unsigned int i = 0; i < m_msPlaceNames.size(); i++) {
		m_ArrayUnPlaces.Add(m_msPlaceNames[i]);
	}
	for (unsigned int i = 0; i < m_msTransitionNames.size(); i++) {
		m_ArrayUnTranstions.Add(m_msTransitionNames[i]);
	}
	for (unsigned int i = 0; i < m_msColoredPlaceNames.size(); i++) {
		m_ArrayColPlaces.Add(m_msColoredPlaceNames[i]);
	}
	for (unsigned int i = 0; i < m_msColoredTransitionNames.size(); i++) {
		m_ArrayColTranstions.Add(m_msColoredTransitionNames[i]);
	}
	for (unsigned int i = 0; i < m_vAuxPLVars.size(); i++) {
		m_ArrayAuxPlaces.Add(m_vAuxPLVars[i]);
	}
	for (unsigned int i = 0; i < m_vAuxTRVars.size(); i++) {
		m_ArrayAuxtranstions.Add(m_vAuxTRVars[i]);
	}

/*	SP_LOGMESSAGE(wxString::Format(wxT("%d"), m_ArrayColPlaces.GetCount()));
	SP_LOGMESSAGE(wxString::Format(wxT("%d"), m_ArrayColTranstions.GetCount()));
	SP_LOGMESSAGE(wxString::Format(wxT("%d"), m_ArrayUnPlaces.GetCount()));
	SP_LOGMESSAGE(wxString::Format(wxT("%d"), m_ArrayUnTranstions.GetCount()));*/

	CreateViewerDataFromRegex(p_pcView);

	for(unsigned int l_nRow = 0; l_nRow < l_pcCurveInfoList->GetRowCount(); l_nRow++)
	{
		wxString l_sPosition = l_pcCurveInfoList->GetCell(l_nRow,0);
		unsigned long l_nPosition=0;
		if(!l_sPosition.ToULong(&l_nPosition))
		{
		  return false;
		}

		wxString l_sOutType = l_pcCurveInfoList->GetCell(l_nRow,1);


		if( l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Place")) && l_nPosition<m_msPlaceNames.size()) //unfolded place
		{
			l_sName=m_msPlaceNames[l_nPosition];

			p_pcViewer->AddCurve(l_sName,l_nPosition,&m_anResultMatrix);
		}
		else if( l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Transition")) && l_nPosition<m_msTransitionNames.size()) //unfolded transition
		{
			l_sName=m_msTransitionNames[l_nPosition];

			 p_pcViewer->AddCurve(l_sName,l_nPosition,&m_anResultMatrix);
		}
		else if( l_sOutType == wxT("Colored") && l_sElementType.IsSameAs(wxT("Place")) && l_nPosition<m_msColoredPlaceNames.size())//colored  place
		{
		   l_sName=m_msColoredPlaceNames[l_nPosition];

		   p_pcViewer->AddCurve(l_sName,l_nPosition,&m_aanColPlaceResults);
		}
		else if( l_sOutType == wxT("Colored") && l_sElementType.IsSameAs(wxT("Transition")) && l_nPosition<m_msColoredTransitionNames.size()) //colored transition
		{
			l_sName=m_msColoredTransitionNames[l_nPosition];

			p_pcViewer->AddCurve(l_sName,l_nPosition,&m_aanColTransResults);
		}
		else if( l_sOutType == wxT("Auxiliary variables") && l_sElementType.IsSameAs(wxT("Place")) && l_nPosition<m_vAuxPLVars.size()) //Auxiliary variables place
		{
			l_sName=m_vAuxPLVars[l_nPosition];

			p_pcViewer->AddCurve(l_sName,l_nPosition,&m_aanAuxPLVarsResults);
		}
		else if( l_sOutType == wxT("Auxiliary variables") && l_sElementType.IsSameAs(wxT("Transition")) && l_nPosition<m_vAuxTRVars.size()) //Auxiliary variables transition
		{
			l_sName=m_vAuxTRVars[l_nPosition];

			p_pcViewer->AddCurve(l_sName,l_nPosition,&m_aanAuxTRVarsResults);
		}
		else
		{
			SP_LOGERROR(wxT("Invalid node names, we stop loading the rest of the file"));

			//invalid row index, therefore we ignore the remaining rows
			l_pcCurveInfoList->RemoveRemainingRows(l_nRow);

			break;
		}

		wxString l_sOrignialName=l_pcCurveInfoList->GetCell(l_nRow,6);

		if(l_sOrignialName!=l_sName)
		{
			SP_LOGWARNING(wxT("Name for position (")+wxString::Format(wxT("%d"),l_nRow) +wxT(") is changed to ")+l_sName);
		}

		//update curve name
		l_pcCurveInfoList->SetCell(l_nRow,6,l_sName);

		p_asPlaces.Add(l_sName);
	}
	 
	return true;
}

void SP_DLG_ColHPNSimultionResults::LoadResults()
{
	//Load colored or auxiliary variables
	LoadColAuxResults();

	UpdateViewer();

	RefreshExternalWindows();
}


void SP_DLG_ColHPNSimultionResults::LoadColAuxResults()
{
	GetColPlaceResults(m_pcMainSimulator->GetResultMatrix(),m_aanColPlaceResults);
	GetColTransResults(m_pcMainSimulator->GetRateResultMatrix(),m_aanColTransResults);

	unsigned int l_nColCount=0;
	l_nColCount = m_msColoredPlaceNames.size();

	double l_nRunCount=(double) (m_pcWorkerThread->GetCurrentRunCount());

	//we need to account for the current run
	if(m_pcMainSimulator->IsSimulationRunning())
	{
		l_nRunCount+=1;
	}

	if(m_pcWorkerThread->GetRunCount()>1)
	{
	    //get the average values
		for(unsigned int l_nRow=0;l_nRow<m_aanColPlaceResults.size();l_nRow++)
			for(unsigned int l_nCol=0;l_nCol<l_nColCount;l_nCol++)
			{
				m_aanColPlaceResults[l_nRow][l_nCol]/=l_nRunCount;
			}
	 }
	
	l_nColCount = m_msColoredTransitionNames.size();
	if(m_pcWorkerThread->GetRunCount()>1)
	{
	    //get the average values
		for(unsigned int l_nRow=0;l_nRow<m_aanColTransResults.size();l_nRow++)
			for(unsigned int l_nCol=0;l_nCol<l_nColCount;l_nCol++)
			{
				m_aanColTransResults[l_nRow][l_nCol]/=l_nRunCount;
			}
	  }


	if(!m_bComAuxVarSingleRun)
	{
		ComputeAuxiliaryVars();
	}

	l_nColCount = m_vAuxPLVars.size();
	if(m_pcWorkerThread->GetRunCount()>1)
	{
	    //get the average values
		for(unsigned int l_nRow=0;l_nRow<m_aanAuxPLVarsResults.size();l_nRow++)
			for(unsigned int l_nCol=0;l_nCol<l_nColCount;l_nCol++)
			{
				m_aanAuxPLVarsResults[l_nRow][l_nCol]/=l_nRunCount;
			}
	  }

}

bool SP_DLG_ColHPNSimultionResults::ComputeAuxiliaryVars()
{
	m_aanAuxPLVarsResults.clear();
	m_vAuxPLVars.clear();
	SP_DS_ColTraceAnalyzer l_cTraceAnalyzer(m_pcUnfoldedNet, m_pcMainSimulator->GetResultMatrix(),m_aanAuxPLVarsResults, m_vAuxPLVars);

	if( ! l_cTraceAnalyzer.Compute() )
		return false;

	return true;
}


void SP_DLG_ColHPNSimultionResults::OnMarkingSetChanged( wxCommandEvent& p_cEvent )
{
	if(m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	//Get the Places Nodes
	unsigned int l_nCurrentMarkingSet = m_pcMarkingSetComboBox->GetSelection();
	const SP_ListNode* l_pcPlaces=m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements();

	//Set the current Active column	
	for(SP_ListNode::const_iterator l_itPlace=l_pcPlaces->begin();l_itPlace!=l_pcPlaces->end();l_itPlace++)
	{		
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itPlace)->GetAttribute(wxT("MarkingList"))))->SetActiveColumn(l_nCurrentMarkingSet);		
	}

	l_pcPlaces=m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE)->GetElements();

	//Set the current Active column	
	for(SP_ListNode::const_iterator l_itPlace=l_pcPlaces->begin();l_itPlace!=l_pcPlaces->end();l_itPlace++)
	{		
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itPlace)->GetAttribute(wxT("MarkingList"))))->SetActiveColumn(l_nCurrentMarkingSet);		
	}

	
	//Todo: load only the current marking
	LoadPlaces();		
    
	m_bIsSimulatorInitialized=false;
}
//
void SP_DLG_ColHPNSimultionResults::OnParameterSetChanged( wxCommandEvent& p_cEvent )
{
	if(m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	unsigned int l_nCurrentParameterSet = m_pcParameterSetComboBox->GetSelection();
	const SP_ListNode* l_pcParams=m_pcGraph->GetNodeclass(SP_DS_PARAM)->GetElements();

		//Set the current Active List
    for(SP_ListNode::const_iterator l_itParam=l_pcParams->begin();l_itParam!=l_pcParams->end();l_itParam++)
	  (dynamic_cast<SP_DS_ColListAttribute*>((*l_itParam)->GetAttribute(wxT("ParameterList"))))->SetActiveList(l_nCurrentParameterSet);
    
	 LoadParameters();

     m_bIsSimulatorInitialized=false;
}
//
void SP_DLG_ColHPNSimultionResults::OnFunctionSetChanged( wxCommandEvent& p_cEvent )
{
	if(m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	unsigned int l_nCurrentFunctionSet = m_pcFunctionSetComboBox->GetSelection();
	const SP_ListNode* l_pcTransitions=m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements();

	//Set the current Active List
	for(SP_ListNode::const_iterator l_itTrans=l_pcTransitions->begin();l_itTrans!=l_pcTransitions->end();l_itTrans++)
	{		
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itTrans)->GetAttribute(wxT("FunctionList"))))->SetActiveColumn(l_nCurrentFunctionSet);
	}
		
	 m_bIsSimulatorInitialized=false;
}

void SP_DLG_ColHPNSimultionResults::OnStochasticRateChanged( wxCommandEvent& p_cEvent )
{
	if(m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	unsigned int l_nCurrentFunctionSet = m_pcStochasticRateSetComboBox->GetSelection();
	const SP_ListNode* l_pcTransitions=m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements();

	//Set the current Active List
	for(SP_ListNode::const_iterator l_itTrans=l_pcTransitions->begin();l_itTrans!=l_pcTransitions->end();l_itTrans++)
	{		
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itTrans)->GetAttribute(wxT("FunctionList"))))->SetActiveColumn(l_nCurrentFunctionSet);
	}
		
	 m_bIsSimulatorInitialized=false;
}

void SP_DLG_ColHPNSimultionResults::OnImmediateSetChanged( wxCommandEvent& p_cEvent )
{
	if(m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	unsigned int l_nCurrentFunctionSet = m_pcImmediateSetComboBox->GetSelection();
	const SP_ListNode* l_pcTransitions=m_pcGraph->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements();

	//Set the current Active List
	for(SP_ListNode::const_iterator l_itTrans=l_pcTransitions->begin();l_itTrans!=l_pcTransitions->end();l_itTrans++)
	{		
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itTrans)->GetAttribute(wxT("FunctionList"))))->SetActiveColumn(l_nCurrentFunctionSet);
	}
		
	 m_bIsSimulatorInitialized=false;
}

void SP_DLG_ColHPNSimultionResults::OnDelaySetChanged( wxCommandEvent& p_cEvent )
{
	if(m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	unsigned int l_nCurrentFunctionSet = m_pcDelaySetComboBox->GetSelection();
	const SP_ListNode* l_pcTransitions=m_pcGraph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements();

	//Set the current Active List
	for(SP_ListNode::const_iterator l_itTrans=l_pcTransitions->begin();l_itTrans!=l_pcTransitions->end();l_itTrans++)
	{		
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itTrans)->GetAttribute(wxT("DelayList"))))->SetActiveColumn(l_nCurrentFunctionSet);
	}
		
	 m_bIsSimulatorInitialized=false;
}


void SP_DLG_ColHPNSimultionResults::OnScheduledSetChanged( wxCommandEvent& p_cEvent )
{
	if(m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	unsigned int l_nCurrentFunctionSet = m_pcScheduledSetComboBox->GetSelection();
	const SP_ListNode* l_pcTransitions=m_pcGraph->GetNodeclass(SP_DS_SCHEDULED_TRANS)->GetElements();

	//Set the current Active List
	for(SP_ListNode::const_iterator l_itTrans=l_pcTransitions->begin();l_itTrans!=l_pcTransitions->end();l_itTrans++)
	{		
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itTrans)->GetAttribute(wxT("PeriodicList"))))->SetActiveColumn(l_nCurrentFunctionSet);
	}
		
	 m_bIsSimulatorInitialized=false;
}




//
void SP_DLG_ColHPNSimultionResults::OnModifyMarkingSets(wxCommandEvent& p_cEvent)
{
	if(m_pcMainSimulator->IsSimulationRunning())
		{
			return;
		}

	SP_DLG_ColStMarkingOverview l_cDlg(this);
	if (l_cDlg.ShowModal() == wxID_OK)
	{
		LoadSets();

		//to be changed. partial unfolding
		if( ! LoadUnfoldedPlaces() )
		{
			SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
			return;
		}

	}

	m_bIsSimulatorInitialized=false;
}

void SP_DLG_ColHPNSimultionResults::OnModifyFunctionSets(wxCommandEvent& p_cEvent)
{
	if(m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	SP_DLG_ColStFunctionOverview* l_pcDlg = new SP_DLG_ColStFunctionOverview(SP_DS_CONTINUOUS_TRANS,this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

		if( LoadUnfoldedPlaces() )
		{
			if( LoadUnfoldingData() )
			{
				m_bIsSimulatorInitialized=false;
				return;
			}
		}
		SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
	}
}


void SP_DLG_ColHPNSimultionResults::OnModifyStochasicSets(wxCommandEvent& p_cEvent)
{
	if(m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	SP_DLG_ColStFunctionOverview* l_pcDlg = new SP_DLG_ColStFunctionOverview(SP_DS_STOCHASTIC_TRANS,this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

		if( LoadUnfoldedPlaces() )
		{
			if( LoadUnfoldingData() )
			{
				m_bIsSimulatorInitialized=false;
				return;
			}
		}
		SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
	}
}


void SP_DLG_ColHPNSimultionResults::OnModifyImmediateSets(wxCommandEvent& p_cEvent)
{
	if(m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	SP_DLG_ColStFunctionOverview* l_pcDlg = new SP_DLG_ColStFunctionOverview(SP_DS_IMMEDIATE_TRANS,this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

		if( LoadUnfoldedPlaces() )
		{
			if( LoadUnfoldingData() )
			{
				m_bIsSimulatorInitialized=false;
				return;
			}
		}
		SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
	}
}


void SP_DLG_ColHPNSimultionResults::OnModifyDelaySets(wxCommandEvent& p_cEvent)
{

	if(m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	SP_DLG_ColListOverview* l_pcDlg = new SP_DLG_ColListOverview(wxT("DelayList"), this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
		
		if( LoadUnfoldedPlaces() )
		{
			if( LoadUnfoldingData() )
			{
				m_bIsSimulatorInitialized=false;
				return;
			}
		}
		SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
	}
}


void SP_DLG_ColHPNSimultionResults::OnModifyScheduledSets(wxCommandEvent& p_cEvent)
{

	if(m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	SP_DLG_ColListOverview* l_pcDlg = new SP_DLG_ColListOverview(wxT("PeriodicList"), this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
		
		if( LoadUnfoldedPlaces() )
		{
			if( LoadUnfoldingData() )
			{
				m_bIsSimulatorInitialized=false;
				return;
			}
		}
		SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
	}
}


void SP_DLG_ColHPNSimultionResults::OnModifyParameterSets(wxCommandEvent& p_cEvent)
{
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		return;
	}

	SP_DLG_StParameterOverview* l_pcDlg = new SP_DLG_StParameterOverview(this);
	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}

	m_bIsSimulatorInitialized = false;
}
//=============================================================================================
void SP_DLG_ColHPNSimultionResults::LoadSets()
{
	m_pcMarkingSetComboBox->Clear();
	m_pcParameterSetComboBox->Clear();
	m_pcFunctionSetComboBox->Clear();
	m_pcStochasticRateSetComboBox->Clear();
	m_pcImmediateSetComboBox->Clear();
	m_pcDelaySetComboBox->Clear();
	m_pcScheduledSetComboBox->Clear();


	const SP_ListNode *l_pcNodesList;
	SP_DS_Node *l_pcFstNode;
	SP_DS_Attribute *l_pcAttr;
	SP_DS_ColListAttribute* l_pcColListAttr;

	//load Marking set
	l_pcNodesList=(SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements());
    if(!l_pcNodesList->empty())
    {
    	l_pcFstNode=l_pcNodesList->front();
		l_pcAttr=l_pcFstNode->GetAttribute(wxT("MarkingList"));
		l_pcColListAttr=dynamic_cast<SP_DS_ColListAttribute *>(l_pcAttr);

		for (unsigned int i = 1; i < l_pcColListAttr->GetColCount(); i++)
		{
			wxString l_sSetName = l_pcColListAttr->GetColLabel(i);
			l_sSetName = l_sSetName.BeforeFirst(wxT(':'));
			m_pcMarkingSetComboBox->Append(l_sSetName);
			i++;
		}

		//Select the first marking
		if(m_pcMarkingSetComboBox->GetCount() <= l_pcColListAttr->GetActiveColumn())
			m_pcMarkingSetComboBox->SetSelection(0);
		else
			m_pcMarkingSetComboBox->SetSelection(l_pcColListAttr->GetActiveColumn());			
    }

	//load parameter set
	l_pcNodesList=(SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_PARAM)->GetElements());
    if(!l_pcNodesList->empty())
    {
		 l_pcFstNode=l_pcNodesList->front();
		 l_pcAttr=l_pcFstNode->GetAttribute(wxT("ParameterList"));
		 l_pcColListAttr=dynamic_cast<SP_DS_ColListAttribute *>(l_pcAttr);
		 for(unsigned int i=0;i<l_pcColListAttr->GetRowCount();i++)
		 {
			m_pcParameterSetComboBox->Append(l_pcColListAttr->GetCell(i,0));
		 }
		 //Select the first Parameter
		 m_pcParameterSetComboBox->SetSelection(l_pcColListAttr->GetActiveList());
    }

	//load continusou function set
	l_pcNodesList=(SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements());
	if(!l_pcNodesList->empty())
	{
		l_pcFstNode=l_pcNodesList->front();
		l_pcAttr=l_pcFstNode->GetAttribute(wxT("FunctionList"));
		l_pcColListAttr=dynamic_cast<SP_DS_ColListAttribute *>(l_pcAttr);
		for (unsigned int i = 1; i < l_pcColListAttr->GetColCount(); i++)
		{
			wxString l_sSetName = l_pcColListAttr->GetColLabel(i);
			l_sSetName = l_sSetName.BeforeFirst(wxT(':'));
			m_pcFunctionSetComboBox->Append(l_sSetName);
		}
		//select the first function	  
		if(m_pcFunctionSetComboBox->GetCount() <= l_pcColListAttr->GetActiveColumn())
			m_pcFunctionSetComboBox->SetSelection(0);
		else
			m_pcFunctionSetComboBox->SetSelection(l_pcColListAttr->GetActiveColumn());
	}

	//load stochastic function set
	l_pcNodesList=(SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements());
	if(!l_pcNodesList->empty())
	{
		l_pcFstNode=l_pcNodesList->front();
		l_pcAttr=l_pcFstNode->GetAttribute(wxT("FunctionList"));
		l_pcColListAttr=dynamic_cast<SP_DS_ColListAttribute *>(l_pcAttr);
		for (unsigned int i = 1; i < l_pcColListAttr->GetColCount(); i++)
		{
			wxString l_sSetName = l_pcColListAttr->GetColLabel(i);
			l_sSetName = l_sSetName.BeforeFirst(wxT(':'));
			m_pcStochasticRateSetComboBox->Append(l_sSetName);
		}
		//select the first function	  
		if(m_pcStochasticRateSetComboBox->GetCount() <= l_pcColListAttr->GetActiveColumn())
			m_pcStochasticRateSetComboBox->SetSelection(0);
		else
			m_pcStochasticRateSetComboBox->SetSelection(l_pcColListAttr->GetActiveColumn());
	}

	//load immediate function set
	l_pcNodesList=(SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements());
	if(!l_pcNodesList->empty())
	{
		l_pcFstNode=l_pcNodesList->front();
		l_pcAttr=l_pcFstNode->GetAttribute(wxT("FunctionList"));
		l_pcColListAttr=dynamic_cast<SP_DS_ColListAttribute *>(l_pcAttr);
		for (unsigned int i = 1; i < l_pcColListAttr->GetColCount(); i++)
		{
			wxString l_sSetName = l_pcColListAttr->GetColLabel(i);
			l_sSetName = l_sSetName.BeforeFirst(wxT(':'));
			m_pcImmediateSetComboBox->Append(l_sSetName);
		}
		//select the first function	  
		if(m_pcImmediateSetComboBox->GetCount() <= l_pcColListAttr->GetActiveColumn())
			m_pcImmediateSetComboBox->SetSelection(0);
		else
			m_pcImmediateSetComboBox->SetSelection(l_pcColListAttr->GetActiveColumn());
	}

	//load deterministic function set
	l_pcNodesList=(SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements());
	if(!l_pcNodesList->empty())
	{
		l_pcFstNode=l_pcNodesList->front();
		l_pcAttr=l_pcFstNode->GetAttribute(wxT("DelayList"));
		l_pcColListAttr=dynamic_cast<SP_DS_ColListAttribute *>(l_pcAttr);
		for (unsigned int i = 1; i < l_pcColListAttr->GetColCount(); i++)
		{
			wxString l_sSetName = l_pcColListAttr->GetColLabel(i);
			l_sSetName = l_sSetName.BeforeFirst(wxT(':'));
			m_pcDelaySetComboBox->Append(l_sSetName);
		}
		//select the first function	  
		if(m_pcDelaySetComboBox->GetCount() <= l_pcColListAttr->GetActiveColumn())
			m_pcDelaySetComboBox->SetSelection(0);
		else
			m_pcDelaySetComboBox->SetSelection(l_pcColListAttr->GetActiveColumn());
	}

	//load scheduled function set
	l_pcNodesList=(SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_SCHEDULED_TRANS)->GetElements());
	if(!l_pcNodesList->empty())
	{
		l_pcFstNode=l_pcNodesList->front();
		l_pcAttr=l_pcFstNode->GetAttribute(wxT("PeriodicList"));
		l_pcColListAttr=dynamic_cast<SP_DS_ColListAttribute *>(l_pcAttr);
		for (unsigned int i = 1; i < l_pcColListAttr->GetColCount(); i++)
		{
			wxString l_sSetName = l_pcColListAttr->GetColLabel(i);
			l_sSetName = l_sSetName.BeforeFirst(wxT(':'));
			m_pcScheduledSetComboBox->Append(l_sSetName);
		}
		//select the first function	  
		if(m_pcScheduledSetComboBox->GetCount() <= l_pcColListAttr->GetActiveColumn())
			m_pcScheduledSetComboBox->SetSelection(0);
		else
			m_pcScheduledSetComboBox->SetSelection(l_pcColListAttr->GetActiveColumn());
	}


}

void SP_DLG_ColHPNSimultionResults::DirectExportToCSV()
{
	if (!m_pcExport)	
		return;	

	const SP_Vector2DDouble& l_aanPLResults = m_pcMainSimulator->GetResultMatrix();
	const SP_Vector2DDouble& l_aanTRResults = m_pcMainSimulator->GetRateResultMatrix();	
	wxString l_sSpacer = GetSpacer(m_nExportSpacer);
	wxString l_sCurrentRow,l_sOutput;

	SP_DS_Attribute* l_pcAttribute=m_pcCurrentTablePlot->GetAttribute(wxT("Nodeclass"));
	CHECK_POINTER(l_pcAttribute,return);
	wxString l_sElementType=l_pcAttribute->GetValueString();

	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*> (m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo")));
	CHECK_POINTER(l_pcCurveInfoList,return);		
	
	for (unsigned long l_nRow = 0; l_nRow < m_pcMainSimulator->GetGeneratedResultPointsCount(); l_nRow++)
	{
		l_sOutput=wxT("");
		l_sOutput << wxString::Format(wxT("%g"), m_pcMainSimulator->GetOutputSampleSize() * l_nRow);
		l_sCurrentRow=wxT(""); 

		for(unsigned int l_n = 0; l_n < l_pcCurveInfoList->GetRowCount(); l_n++)
		{
			wxString l_sPos = l_pcCurveInfoList->GetCell(l_n,0);
			long l_nPos;
			if(!l_sPos.ToLong(&l_nPos))
				return;
			wxString l_sOutType = l_pcCurveInfoList->GetCell(l_n,1);
			wxString l_sCheckState = l_pcCurveInfoList->GetCell(l_n,2);	
			if(l_sCheckState != wxT("1") )
				continue;
			wxString l_sName=l_pcCurveInfoList->GetCell(l_n,6);

			double l_dResult = 0;
			if( l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Place")) ) //unfolded place
			{
				l_dResult = l_aanPLResults[l_nRow][l_nPos];				
			}
			else if( l_sOutType == wxT("Colored") && l_sElementType.IsSameAs(wxT("Place")) )//colored  place
			{
				l_dResult = m_aanColPlaceResults[l_nRow][l_nPos];	
			}
			else if( l_sOutType == wxT("Auxiliary variables") && l_sElementType.IsSameAs(wxT("Place")) ) //unfolded place
			{
				l_dResult = m_aanAuxPLVarsResults[l_nRow][l_nPos];	
			}


			else if( l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Transition")) ) //unfolded place
			{
				l_dResult = l_aanTRResults[l_nRow][l_nPos];			
			}
			
			else if( l_sOutType == wxT("Colored") && l_sElementType.IsSameAs(wxT("Transition")) ) //unfolded place
			{
				l_dResult = m_aanColTransResults[l_nRow][l_nPos];	
			}
			
			else if( l_sOutType == wxT("Auxiliary variables") && l_sElementType.IsSameAs(wxT("Transition")) ) //unfolded place
			{
				l_dResult = m_aanAuxTRVarsResults[l_nRow][l_nPos];	
			}
			else
			{
				continue;
			}

			//write 
			l_sCurrentRow<<l_sSpacer;
			l_sCurrentRow<<wxString::Format(wxT("%.16g"),l_dResult);
		}

		l_sOutput << l_sCurrentRow;
		(*m_pcExport) << l_sOutput << wxT("\n");
	}

}

void SP_DLG_ColHPNSimultionResults::SaveODE(wxCommandEvent& p_cEvent)
{
}


void SP_DLG_ColHPNSimultionResults::LoadPlaces()
{
	
	unsigned long l_nPlaceCount=m_pcUnfoldedNet->GetPlaceNames()->size();

	m_pcMainSimulator->SetPlaceCount(m_pcUnfoldedNet->GetPlaceNames()->size());
		  
	 m_pcMainSimulator->SetPlaceNames(*m_pcUnfoldedNet->GetPlaceNames());

	 // m_pcMainSimulator->SetInitialMarking(*m_pcUnfoldedNet->GetCurHybPNMarking());

	 SP_VectorBool l_abPlaceType;

	 l_abPlaceType.clear();

	 //continuous place count: workaround
	 m_nContinuousPlaceCount=m_pcUnfoldedNet->GetContinuousPlaceCount();

	 l_abPlaceType.assign(m_nContinuousPlaceCount,true);

	  if( m_sSimulatorType==wxT("Continuous"))
	   {
		  l_abPlaceType.resize(l_nPlaceCount,true);

		  m_nContinuousPlaceCount=l_nPlaceCount;
	   }
	   else
	   {
		  l_abPlaceType.resize(l_nPlaceCount,false);
	   }

	 
	  m_anCurrentMarking.clear();
	  m_anCurrentMarking = *m_pcUnfoldedNet->GetCurHybPNMarking();	  
      
     // int l_nMethodType=m_pcTimeSyncComboBox->GetSelection();
      
      if(m_sSimulatorType==wxT("Stochastic"))
      {
          SP_VectorLong l_anCurrentMarking;
         
          l_anCurrentMarking.clear();
         
          l_anCurrentMarking.assign(m_anCurrentMarking.size(),0);
          
          for(unsigned long l_nPlace=0; l_nPlace<m_anCurrentMarking.size();l_nPlace++)
          {
             l_anCurrentMarking[l_nPlace]=m_anCurrentMarking[l_nPlace];
          }
          
          m_pcMainSimulator->SetInitialMarking(l_anCurrentMarking);
      }
      else
      {
          //set initial marking
          m_pcMainSimulator->SetInitialMarking(m_anCurrentMarking);	

          (dynamic_cast<spsim::HybridSimulator*>(m_pcMainSimulator))->SetPlaceTypes(l_abPlaceType);
      }
       
}

void SP_DLG_ColHPNSimultionResults::LoadTransitions()
{
	
	       m_pcMainSimulator->ClearTransitions();

            m_pcMainSimulator->SetTransitionCount(m_pcUnfoldedNet->GetTransitionNames()->size());
            
            SP_VectorString* l_pcTransitionName		=	m_pcUnfoldedNet->GetTransitionNames();
            SP_VectorString* l_pcTransitionFunction	=	m_pcUnfoldedNet->GetCurRateFunction();
			SP_VectorString* l_pcTransitionType		=	m_pcUnfoldedNet->GetTransNodeType();

			spsim::TransitionType l_nTransitionType;

            for(unsigned long l_nTransPos=0;l_nTransPos<l_pcTransitionName->size();l_nTransPos++)
            {
				wxString l_sTransType = (*l_pcTransitionType)[l_nTransPos];

				if(l_sTransType == SP_DS_STOCHASTIC_TRANS)
				{
					l_nTransitionType =  m_sSimulatorType==wxT("Continuous")? spsim::TRANSITION_TYPE_CONTINUOUS: spsim::TRANSITION_TYPE_STOCHASTIC;
				}
				else if(l_sTransType == SP_DS_CONTINUOUS_TRANS)
				{
					l_nTransitionType = m_sSimulatorType==wxT("Stochastic")? spsim::TRANSITION_TYPE_STOCHASTIC: spsim::TRANSITION_TYPE_CONTINUOUS;
				}
				else if(l_sTransType == SP_DS_IMMEDIATE_TRANS)
				{
					l_nTransitionType = spsim::TRANSITION_TYPE_IMMEDIATE;
				}
				else if(l_sTransType == SP_DS_DETERMINISTIC_TRANS)
				{
					l_nTransitionType = spsim::TRANSITION_TYPE_DETERMINISTIC;
				}
				else if(l_sTransType == SP_DS_SCHEDULED_TRANS)
				{
					l_nTransitionType = spsim::TRANSITION_TYPE_SCHEDULED;
				}


            	m_pcMainSimulator->AddTransition(
            			                           (*l_pcTransitionName)[l_nTransPos],

            			                           (*l_pcTransitionFunction)[l_nTransPos],
            			                           
            			                            l_nTransitionType
            			                          );
            }
}


void SP_DLG_ColHPNSimultionResults::LoadConnections()
{
vector<SP_CPN_ContinuousArc>* l_pcPreConnection;
vector<SP_CPN_ContinuousArc>* l_pcPostConnection;
vector<SP_CPN_ContinuousArc>::const_iterator l_itConnection;

		//Map transition name to their positions
		m_pcMainSimulator->MapTransitionNames2Positions();

		m_pcMainSimulator->MapPlaceNames2Positions();

		//Load transitions' pre place
		l_pcPreConnection = m_pcUnfoldedNet->GetArcs_Place2Transition();
		for (l_itConnection = l_pcPreConnection->begin(); l_itConnection != l_pcPreConnection->end(); l_itConnection++)
		{
			double l_nNumericArcWeight = 0;

			wxString l_sArcWeight=l_itConnection->m_sMultiplicity;

			//l_itConnection->m_sMultiplicity.ToDouble(&l_nWeight);

			spsim::ConnectionType l_nArcType=GetConnectionType(l_itConnection->m_sArcClass);

			if (IsConstantArcWeight(l_sArcWeight, l_nNumericArcWeight))
			{
				m_pcMainSimulator->SetPreTransitionConnection(l_itConnection->m_nTranPos,l_itConnection->m_nPlaceID, l_nArcType, l_nNumericArcWeight);
			}
			else
			{
				m_pcMainSimulator->SetPreSelfModifyingWeights(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID,l_nArcType, l_sArcWeight);
			}

			SP_LOGMESSAGE(l_itConnection->m_sMultiplicity);

			//m_pcMainSimulator->SetPreTransitionConnection(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID, GetConnectionType(l_itConnection->m_sArcClass), l_nWeight);
		}

		//Load transitions' post place
		l_pcPostConnection = m_pcUnfoldedNet->GetArcs_Transition2Place();
		for (l_itConnection = l_pcPostConnection->begin(); l_itConnection != l_pcPostConnection->end(); l_itConnection++)
		{
			double l_nNumericArcWeight = 0;

			wxString l_sArcWeight=l_itConnection->m_sMultiplicity;

			//l_itConnection->m_sMultiplicity.ToDouble(&l_nWeight);

			if (IsConstantArcWeight(l_sArcWeight, l_nNumericArcWeight))
			{
				m_pcMainSimulator->SetPostTransitionConnection(l_itConnection->m_nTranPos,l_itConnection->m_nPlaceID,l_nNumericArcWeight);
			}
			else
			{
				m_pcMainSimulator->SetPostSelfModifyingWeights(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID,l_sArcWeight);
			}

			//m_pcMainSimulator->SetPostTransitionConnection(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID, l_nWeight);
		}
}

void SP_DLG_ColHPNSimultionResults::LoadParameters()
{
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass= m_pcGraph->GetNodeclass(SP_DS_PARAM);
	SP_ListNode::const_iterator l_itElem;
	SP_DS_ColListAttribute* l_pcColList;
	SP_VectorString l_asParameterNames;
	SP_VectorDouble l_anParameterValue;

	   l_anParameterValue.clear();
	   l_asParameterNames.clear();

	 //Go through all the transition nodes
	 for(l_itElem=l_pcNodeclass->GetElements()->begin();l_itElem!=l_pcNodeclass->GetElements()->end();l_itElem++)
	 {
		//Set the transition name
		l_asParameterNames.push_back(dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue());

		//Get the transition rate function
		 l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*l_itElem)->GetAttribute(wxT("ParameterList")) );
		 l_anParameterValue.push_back(l_pcColList->GetActiveCellValueDouble(1));
	 }

	m_pcMainSimulator->SetParameterNames(l_asParameterNames);
	m_pcMainSimulator->SetParameterValues(l_anParameterValue);
}

spsim::ConnectionType SP_DLG_ColHPNSimultionResults::GetConnectionType(const wxString& p_sConnectionType)
{
	if (p_sConnectionType == SP_DS_EDGE)
	{
		return spsim::STANDARD_ARC;
	}
	else if (p_sConnectionType == SP_DS_READ_EDGE)
	{
		return spsim::READ_ARC;
	}
	else if (p_sConnectionType == SP_DS_INHIBITOR_EDGE)
	{
		return spsim::INHIBITOR_ARC;
	}
	else if (p_sConnectionType == SP_DS_MODIFIER_EDGE)
	{
		return spsim::MODIFIER_ARC;
	}
	else if (p_sConnectionType == SP_DS_RESET_EDGE)
	{
		return spsim::RESET_ARC;
	}
	else if (p_sConnectionType == SP_DS_EQUAL_EDGE)
	{
		return spsim::EQUAL_ARC;
	}
	else
	{
		SP_LOGERROR( wxT("Unknown connection type,") + p_sConnectionType);
		return spsim::OTHER_CONNECTION;
	}
}

void SP_DLG_ColHPNSimultionResults::UpdateXAxisValues()
{
	      CHECK_POINTER(m_pcCurrentTablePlot,return);
	      
	      CalculateXAxisValues(m_pcCurrentTablePlot,m_anXAxisValues);     
}

void SP_DLG_ColHPNSimultionResults::CalculateXAxisValues(SP_DS_Metadata* p_pcView,SP_VectorDouble& p_anXAxisValues)
{
            CHECK_POINTER(m_pcMainSimulator,return);

	      wxString l_sCategory=GetViewAttributeValue(p_pcView,wxT("XAxisVariable"));
	      wxString l_sSubCategory=GetViewAttributeValue(p_pcView,wxT("XAxisVariableName"));

	      double l_nStartOutputPoint=m_pcMainSimulator->GetOutputStartPoint();


	      p_anXAxisValues.clear();
	      if(l_sCategory.IsSameAs(wxT("Time")))
	      {
	    	  double l_nTime=0;

	    	  for(unsigned long l_nPoint=0;l_nPoint<m_pcMainSimulator->GetGeneratedResultPointsCount();l_nPoint++)
	    	  {
	    		  p_anXAxisValues.push_back(l_nStartOutputPoint+l_nTime);

	    		  l_nTime+=m_pcMainSimulator->GetOutputSampleSize();
	    	  }

	      }
	      else if(l_sCategory.IsSameAs(wxT("Place-colored")) && !l_sSubCategory.IsEmpty())
	      {
	    	  unsigned long l_nPlacePosition=m_mPlaceName2Position[l_sSubCategory];

	    	  //number of rows in the colored matrix
	    	  unsigned long l_nRowCount=m_aanColPlaceResults.size();

	    	  for(unsigned long l_nPoint=0;l_nPoint<l_nRowCount;l_nPoint++)
	    	  {
	    		  double l_nValue=m_aanColPlaceResults[l_nPoint][l_nPlacePosition];

	    		  p_anXAxisValues.push_back(l_nValue);
	    	  }

	      }
	      else if(l_sCategory.IsSameAs(wxT("Transition-colored")) && !l_sSubCategory.IsEmpty())
	      {
	    	  unsigned long l_nTransitionPosition=m_mTransitionName2Position[l_sSubCategory];

	    	  //number of rows in the colored matrix
	    	  unsigned long l_nRowCount=m_aanColTransResults.size();

	    	  for(unsigned long l_nPoint=0;l_nPoint<l_nRowCount;l_nPoint++)
			  {
				  double l_nValue=m_aanColTransResults[l_nPoint][l_nTransitionPosition];

				  p_anXAxisValues.push_back(l_nValue);
			  }

	      }
	      else if(l_sCategory.IsSameAs(wxT("Place-uncolored")) && !l_sSubCategory.IsEmpty())
		  {
			  unsigned long l_nPlacePosition=m_mnPlaceArrayPos_FromUnfolding[l_sSubCategory];

			  //number of rows in the result matrix
			  unsigned long l_nRowCount=m_pcMainSimulator->GetResultMatrix().size();

			  for(unsigned long l_nPoint=0;l_nPoint<l_nRowCount;l_nPoint++)
			  {
				  double l_nValue=m_pcMainSimulator->GetResultMatrix()[l_nPoint][l_nPlacePosition];

				  p_anXAxisValues.push_back(l_nValue);
			  }

		  }
		  else if(l_sCategory.IsSameAs(wxT("Transition-uncolored")) && !l_sSubCategory.IsEmpty())
		  {
			  unsigned long l_nTransitionPosition=m_mnTransitionArrayPos[l_sSubCategory];

			  //number of rows in the result matrix
			  unsigned long l_nRowCount=m_pcMainSimulator->GetRateResultMatrix().size();

			  for(unsigned long l_nPoint=0;l_nPoint<l_nRowCount;l_nPoint++)
			  {
				  double l_nValue=m_pcMainSimulator->GetRateResultMatrix()[l_nPoint][l_nTransitionPosition];

				  p_anXAxisValues.push_back(l_nValue);
			  }

		  }
	      else
	      {
	    	  double l_nTime=0;

			  for(unsigned long l_nPoint=0;l_nPoint<m_pcMainSimulator->GetGeneratedResultPointsCount();l_nPoint++)
			  {
				  p_anXAxisValues.push_back(l_nStartOutputPoint+l_nTime);

				  l_nTime+=m_pcMainSimulator->GetOutputSampleSize();
			  }

	      }
}


void SP_DLG_ColHPNSimultionResults::InitializeEmptyView(SP_DS_Metadata* p_pcView)
{
		CHECK_POINTER(p_pcView,return);

		//get the current nodeclass type
		SP_DS_Attribute* l_pcAttribute=p_pcView->GetAttribute(wxT("Nodeclass"));

		CHECK_POINTER(l_pcAttribute,return);

		wxString l_sElementType=l_pcAttribute->GetValueString();

		SP_VectorString* l_pvCurrentInfo;

		SP_VectorString l_asColours;

		l_asColours.clear();

		if(l_sElementType.IsSameAs(wxT("Transition")))
		{
			l_pvCurrentInfo =& m_msColoredTransitionNames;

			//load continuous  place colour
			LoadNodeColours(SP_DS_CONTINUOUS_TRANS,l_asColours);

			//load continuous  place colour
			LoadNodeColours(SP_DS_DISCRETE_TRANS,l_asColours);
		}
		else
		{
			l_pvCurrentInfo =& m_msColoredPlaceNames;

			//load continuous  place colour
		    LoadNodeColours(SP_DS_CONTINUOUS_PLACE,l_asColours);

			//load continuous  place colour
			LoadNodeColours(SP_DS_DISCRETE_PLACE,l_asColours);
		}		

		l_pcAttribute=p_pcView->GetAttribute(wxT("CurveInfo"));
		CHECK_POINTER(l_pcAttribute, return);
	    SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttribute);

        //position of the element in the result matrix 
		for (unsigned int l_nPos = 0; l_nPos < l_pvCurrentInfo->size(); l_nPos++)
		{			
			wxString l_sName = (*l_pvCurrentInfo)[l_nPos];
			
			unsigned int l_nNewRow=l_pcCurveInfoList->AppendEmptyRow();

			l_pcCurveInfoList->SetCell(l_nNewRow,0,wxString::Format(wxT("%ld"),l_nPos));
			l_pcCurveInfoList->SetCell(l_nNewRow,1,wxT("Colored"));
			l_pcCurveInfoList->SetCell(l_nNewRow,2,wxT("0"));
			l_pcCurveInfoList->SetCell(l_nNewRow,3,GetColourString(l_nPos));

			if(l_nPos<l_asColours.size() && l_asColours[l_nPos]!=wxT(""))
			{
				l_pcCurveInfoList->SetCell(l_nNewRow,3,l_asColours[l_nPos]);
			}
			else
			{
				l_pcCurveInfoList->SetCell(l_nNewRow,3,GetColourString(l_nPos));
			}

			l_pcCurveInfoList->SetCell(l_nNewRow,4,wxT("-1"));
			l_pcCurveInfoList->SetCell(l_nNewRow,5,wxT("-1"));
			l_pcCurveInfoList->SetCell(l_nNewRow,6,l_sName);
			
		}
}

