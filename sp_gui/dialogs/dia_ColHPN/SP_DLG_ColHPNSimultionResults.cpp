/*
 * SP_DLG_ColHPNSimultionResults.h
 * $Author: F. Liu
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 6.12.2011
 * $Updated: by George Assaf: constants harmonizing and supporting observers$
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

//by george for constants harmonizing
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_ConstantDefinition.h"//constants harmonizing by george


IMPLEMENT_CLASS(SP_DLG_ColHPNSimultionResults, SP_DLG_HybridSimulationResults )

enum
{
   SP_ID_BUTTON_MODIFY_MARKING_SETS = SP_ID_LAST_ID + 200,
   SP_ID_BUTTON_MODIFY_FUNCTION_SETS,
   SP_ID_BUTTON_MODIFY_PARAMETER_SETS,
   SP_ID_BUTTON_MODIFY_STOCHASTIC_RATE_SETS,
   SP_ID_CHOICE_MARKING_SETS,
   SP_ID_CHOICE_FUNCTION_SETS,
   SP_ID_CHOICE_PARAMETER_SETS,
   SP_ID_CHOICE_TIME_SYNC,
   SP_ID_CHOICE_DELAY_SETS,
   SP_ID_CHOICE_IMMEDIATE_SETS,
   SP_ID_CHOICE_SCHEDULED_SETS,
   SP_ID_CHOICE_STOCHASTIC_RATE_SETS,
   SP_ID_CHOICE_SOLVER,
   SP_ID_BUTTON_SIMULATION_PROPERTIES,
   SP_ID_BUTTON_SAVE_ODE,
   SP_ID_CHOICE_STOCHASTIC_SIMULATOR,
   SP_ID_BUTTON_CONTINUOUS_SOLVER_PROPERTIES,
   SP_ID_BUTTON_MODIFY_DELAY_SETS,
   SP_ID_BUTTON_MODIFY_IMMDIATE_SETS,
   SP_ID_BUTTON_MODIFY_SCHEDULED_SETS,
   SP_ID_BUTTON_MODIFY_COL_CONSTANT_SETS,// by george

   SP_ID_BUTTON_CHANGE_COL_CONSTANT_SETS

};
BEGIN_EVENT_TABLE( SP_DLG_ColHPNSimultionResults, SP_DLG_HybridSimulationResults )

EVT_BUTTON( SP_ID_BUTTON_MODIFY_MARKING_SETS, SP_DLG_ColHPNSimultionResults :: OnModifyMarkingSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_FUNCTION_SETS, SP_DLG_ColHPNSimultionResults :: OnModifyFunctionSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_PARAMETER_SETS, SP_DLG_ColHPNSimultionResults :: OnModifyParameterSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_STOCHASTIC_RATE_SETS, SP_DLG_ColHPNSimultionResults :: OnModifyStochasicSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_DELAY_SETS, SP_DLG_ColHPNSimultionResults :: OnModifyDelaySets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_IMMDIATE_SETS, SP_DLG_ColHPNSimultionResults :: OnModifyImmediateSets )
EVT_BUTTON( SP_ID_BUTTON_MODIFY_SCHEDULED_SETS, SP_DLG_ColHPNSimultionResults :: OnModifyScheduledSets )

EVT_CHOICE( SP_ID_CHOICE_MARKING_SETS, SP_DLG_ColHPNSimultionResults::OnMarkingSetChanged )
EVT_CHOICE( SP_ID_CHOICE_FUNCTION_SETS, SP_DLG_ColHPNSimultionResults::OnFunctionSetChanged )
EVT_CHOICE( SP_ID_CHOICE_STOCHASTIC_RATE_SETS, SP_DLG_ColHPNSimultionResults::OnStochasticRateChanged )
EVT_CHOICE( SP_ID_CHOICE_IMMEDIATE_SETS, SP_DLG_ColHPNSimultionResults::OnImmediateSetChanged )
EVT_CHOICE( SP_ID_CHOICE_DELAY_SETS, SP_DLG_ColHPNSimultionResults::OnDelaySetChanged )
EVT_CHOICE( SP_ID_CHOICE_SCHEDULED_SETS, SP_DLG_ColHPNSimultionResults::OnScheduledSetChanged )

EVT_CHOICE( SP_ID_CHOICE_PARAMETER_SETS, SP_DLG_ColHPNSimultionResults::OnParameterSetChanged )
EVT_CHOICE( SP_ID_CHOICE_DELAY_SETS, SP_DLG_ColHPNSimultionResults::OnParameterSetChanged )
EVT_CHOICE( SP_ID_CHOICE_SOLVER, SP_DLG_ColHPNSimultionResults::OnSolverChanged )
EVT_CHOICE( SP_ID_CHOICE_TIME_SYNC, SP_DLG_ColHPNSimultionResults::OnTimeSyncTypeChanged )
EVT_BUTTON( SP_ID_BUTTON_SIMULATION_PROPERTIES, SP_DLG_ColHPNSimultionResults :: OnSimulationProperties )
EVT_BUTTON( SP_ID_BUTTON_CONTINUOUS_SOLVER_PROPERTIES, SP_DLG_ColHPNSimultionResults :: OnODESolverProperties )
EVT_BUTTON( SP_ID_BUTTON_SAVE_ODE, SP_DLG_ColHPNSimultionResults :: SaveODE )
EVT_CHOICE(SP_ID_BUTTON_CHANGE_COL_CONSTANT_SETS, SP_DLG_ColHPNSimultionResults::OnConstantsSetsChanged)
EVT_SIMTHREAD(SP_SIMULATION_THREAD_EVENT,SP_DLG_HybridSimulationResults::OnSimulatorThreadEvent)
EVT_BUTTON(SP_ID_BUTTON_MODIFY_COL_CONSTANT_SETS, SP_DLG_ColHPNSimultionResults::OnModifyConstants)// by george

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

	if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements()->size() > 0 ||
			SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_DISCRETE_PLACE)->GetElements()->size() > 0)
	{
	//Marking Set
	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add( new wxStaticText( m_pcPropertyWindowSetsSizer, -1, wxT("Marking set:") ), 1, wxALL | wxEXPAND, 5 );
	m_pcMarkingSetComboBox = new wxChoice( m_pcPropertyWindowSetsSizer, SP_ID_CHOICE_MARKING_SETS, wxDefaultPosition, wxSize(100,-1));
	l_pcRowSizer->Add( m_pcMarkingSetComboBox, 0, wxALL, 5 );
	l_pcRowSizer->Add( new wxButton( m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_MARKING_SETS, wxT("Modify") ), 0, wxALL, 5 );
	m_pcSetsSizer->Add( l_pcRowSizer, 1, wxEXPAND );
	}
	else
	{
		m_pcMarkingSetComboBox=NULL;

	}

	if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size() > 0)
	{
	//Function set
	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add( new wxStaticText( m_pcPropertyWindowSetsSizer, -1, wxT("Continuous rate:") ), 1, wxALL | wxEXPAND, 5 );
	m_pcFunctionSetComboBox = new wxChoice( m_pcPropertyWindowSetsSizer, SP_ID_CHOICE_FUNCTION_SETS, wxDefaultPosition, wxSize(100,-1));
	l_pcRowSizer->Add( m_pcFunctionSetComboBox, 1, wxALL, 5 );
	l_pcRowSizer->Add( new wxButton( m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_FUNCTION_SETS, wxT("Modify") ), 0, wxALL, 5 );
	m_pcSetsSizer->Add( l_pcRowSizer, 1, wxEXPAND );
	}
	else
	{
		m_pcFunctionSetComboBox = NULL;
	}

	if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements()->size() > 0)
	{
	//Rate set
	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add( new wxStaticText( m_pcPropertyWindowSetsSizer, -1, wxT("Stochastic  rate:") ), 1, wxALL | wxEXPAND, 5 );
	m_pcStochasticRateSetComboBox = new wxChoice( m_pcPropertyWindowSetsSizer, SP_ID_CHOICE_STOCHASTIC_RATE_SETS, wxDefaultPosition, wxSize(100,-1));
	l_pcRowSizer->Add( m_pcStochasticRateSetComboBox, 1, wxALL, 5 );
	l_pcRowSizer->Add( new wxButton( m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_STOCHASTIC_RATE_SETS, wxT("Modify") ), 0, wxALL, 5 );
	m_pcSetsSizer->Add( l_pcRowSizer, 1, wxEXPAND );
	}
	else
	{
		m_pcStochasticRateSetComboBox = NULL;
	}

	if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements()->size()>0)
	{//immediate Set
	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add( new wxStaticText( m_pcPropertyWindowSetsSizer, -1, wxT("Immediate set:") ), 1, wxALL | wxEXPAND, 5 );
	m_pcImmediateSetComboBox = new wxChoice( m_pcPropertyWindowSetsSizer, SP_ID_CHOICE_IMMEDIATE_SETS, wxDefaultPosition, wxSize(100,-1));
	l_pcRowSizer->Add( m_pcImmediateSetComboBox, 1, wxALL, 5 );
	l_pcRowSizer->Add( new wxButton( m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_IMMDIATE_SETS, wxT("Modify") ), 0, wxALL, 5 );
	m_pcSetsSizer->Add( l_pcRowSizer, 1, wxEXPAND );
	}
	else
	{
		m_pcImmediateSetComboBox=NULL;
	}

	if(SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements()->size()>0)
	{//delay Set
	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add( new wxStaticText( m_pcPropertyWindowSetsSizer, -1, wxT("Delay set:") ), 1, wxALL | wxEXPAND, 5 );
	m_pcDelaySetComboBox = new wxChoice( m_pcPropertyWindowSetsSizer, SP_ID_CHOICE_DELAY_SETS, wxDefaultPosition, wxSize(100,-1));
	l_pcRowSizer->Add( m_pcDelaySetComboBox, 1, wxALL, 5 );
	l_pcRowSizer->Add( new wxButton( m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_DELAY_SETS, wxT("Modify") ), 0, wxALL, 5 );
	m_pcSetsSizer->Add( l_pcRowSizer, 1, wxEXPAND );
	}
	else
	{
		m_pcDelaySetComboBox = NULL;
	}
	if (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_SCHEDULED_TRANS)->GetElements()->size() > 0)
	{//scheduled Set
	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add( new wxStaticText( m_pcPropertyWindowSetsSizer, -1, wxT("Scheduled set:") ), 1, wxALL | wxEXPAND, 5 );
	m_pcScheduledSetComboBox = new wxChoice( m_pcPropertyWindowSetsSizer, SP_ID_CHOICE_SCHEDULED_SETS, wxDefaultPosition, wxSize(100,-1));
	l_pcRowSizer->Add( m_pcScheduledSetComboBox, 1, wxALL, 5 );
	l_pcRowSizer->Add( new wxButton( m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_SCHEDULED_SETS, wxT("Modify") ), 0, wxALL, 5 );
	m_pcSetsSizer->Add( l_pcRowSizer, 1, wxEXPAND );
	}
	else
	{
		m_pcScheduledSetComboBox=NULL;
	}


	//by george for constants harmonizing
	UpdateChoices();
	m_nGroupCounts = 0;
	SP_SetString::iterator l_itChoice;
	for (l_itChoice = m_choicesForColPNs.begin(); l_itChoice != m_choicesForColPNs.end(); ++l_itChoice)
	{
		wxString l_sGroup = *l_itChoice;
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, l_sGroup + wxT(':')), wxSizerFlags(1).Expand().Border(wxALL, 2));
		m_apcComboBoxes.push_back(new wxChoice(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_CHANGE_COL_CONSTANT_SETS, wxDefaultPosition, wxSize(100, -1), 0, NULL, 0, wxDefaultValidator, l_sGroup));
		l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size() - 1], wxSizerFlags(1).Expand().Border(wxALL, 2));
		l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_COL_CONSTANT_SETS, wxT("Modify")), wxSizerFlags(0).Expand().Border(wxALL, 2));
		m_pcSetsSizer->Add(l_pcRowSizer, wxSizerFlags(0).Expand().Border(wxALL, 2));
		m_nGroupCounts++;
	}

	m_iModifyCount = 0;
	 //At the end call this function for alignment
	 SetSizerAndFit( m_pcMainSizer);

}


SP_DLG_ColHPNSimultionResults::~SP_DLG_ColHPNSimultionResults()
{
	//george, reset default groups
	for (size_t i = 0; i < m_apcColListAttr.size(); i++)
	{
		if (m_apcColListAttr[i])
		{
			if (i < m_apcComboBoxes.size())
			{
				m_apcComboBoxes[i]->SetSelection(0);
				m_apcColListAttr[i]->SetActiveList(m_apcComboBoxes[i]->GetSelection());
			}

		}
	}

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
			ObtainSimulationMatrix(false);//marking mat

			l_sName=m_msPlaceNames[l_nPosition];

			p_pcViewer->AddCurve(l_sName,l_nPosition,&m_anResultMatrix);
		}
		else if( l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Transition")) && l_nPosition<m_msTransitionNames.size()) //unfolded transition
		{
			ObtainSimulationMatrix(true);//rate mat

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
		if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Observer places")) && l_nPosition < m_vUnfoldedPlacesObservers.size())
		{

				l_sName = m_vUnfoldedPlacesObservers[l_nPosition];

				p_pcViewer->AddCurve(l_sName, l_nPosition, &m_anResultUnfoldedPlaceObserversMatrix);
		}
		else
		if (l_sOutType == wxT("Colored") && l_sElementType.IsSameAs(wxT("Observer places")) && l_nPosition < m_vColPlaceObservers.size())
		{
				l_sName = m_vColPlaceObservers[l_nPosition];

				p_pcViewer->AddCurve(l_sName, l_nPosition, &m_anResultColPlaceObserversMatrix);
		}
		else
		if (l_sOutType == wxT("Colored") && l_sElementType.IsSameAs(wxT("Observer transitions")) && l_nPosition < m_vColTransObservers.size())
		{
				l_sName = m_vColTransObservers[l_nPosition];

				p_pcViewer->AddCurve(l_sName, l_nPosition, &m_anResultColTransObserversMatrix);
		}
	    else
	    if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Observer transitions")) && l_nPosition < m_vUnfoldedTransitionsObservers.size())
		{
				l_sName = m_vUnfoldedTransitionsObservers[l_nPosition];

				p_pcViewer->AddCurve(l_sName, l_nPosition, &m_anResultUnfoldedTransitionObserversMatrix);
		}
		else
		if (l_sElementType.IsSameAs(wxT("Mixed observers")) && l_nPosition < m_vMixedObservers.size())
		{
			l_sName = m_vMixedObservers[l_nPosition];

			p_pcViewer->AddCurve(l_sName, l_nPosition, &m_anResultMixedObserversMatrix);

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


	//if(!m_bComAuxVarSingleRun)
	//{
	//	ComputeAuxiliaryVars();
	//}
/*
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
	*/
	if (m_pcCurrentTablePlot)//by george for computing the observers
		{
			SP_DS_Attribute*   l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("Nodeclass"));
			if (l_pcAttribute)
			{
				wxString l_sElementType = l_pcAttribute->GetValueString();
				UpdateObservers(wxT("Place"), m_pcMainSimulator->GetGeneratedResultPointsCount());
				UpdateObservers(wxT("Transition"), m_pcMainSimulator->GetGeneratedResultPointsCount());
				UpdateObservers(wxT("Mixed"), m_pcMainSimulator->GetGeneratedResultPointsCount());
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

		unsigned i = 0;
		for (auto it = m_mGroup2Selction.begin(); it != m_mGroup2Selction.end(); ++it)
		{
			m_apcComboBoxes[i]->SetSelection((it)->second);
			i++;
		}

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

		unsigned i = 0;
		for (auto it = m_mGroup2Selction.begin(); it != m_mGroup2Selction.end(); ++it)
		{
			m_apcComboBoxes[i]->SetSelection((it)->second);
			i++;
		}

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

		unsigned i = 0;
		for (auto it = m_mGroup2Selction.begin(); it != m_mGroup2Selction.end(); ++it)
		{
			m_apcComboBoxes[i]->SetSelection((it)->second);
			i++;
		}

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

		unsigned i = 0;
		for (auto it = m_mGroup2Selction.begin(); it != m_mGroup2Selction.end(); ++it)
		{
			m_apcComboBoxes[i]->SetSelection((it)->second);
			i++;
		}

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
		
		unsigned i = 0;
		for (auto it = m_mGroup2Selction.begin(); it != m_mGroup2Selction.end(); ++it)
		{
			m_apcComboBoxes[i]->SetSelection((it)->second);
			i++;
		}
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
		//recover selected sets of constants groups
		unsigned i = 0;
		for (auto it = m_mGroup2Selction.begin(); it != m_mGroup2Selction.end(); ++it)
		{
			m_apcComboBoxes[i]->SetSelection((it)->second);
			i++;
		}

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

		unsigned i = 0;
		for (auto it = m_mGroup2Selction.begin(); it != m_mGroup2Selction.end(); ++it)
		{
			m_apcComboBoxes[i]->SetSelection((it)->second);
			i++;
		}
	}

	m_bIsSimulatorInitialized = false;
}
//=============================================================================================
void SP_DLG_ColHPNSimultionResults::LoadSets()
{
	if(m_pcMarkingSetComboBox)
		m_pcMarkingSetComboBox->Clear();

	if(m_pcFunctionSetComboBox)
		m_pcFunctionSetComboBox->Clear();

	if(m_pcStochasticRateSetComboBox)
		m_pcStochasticRateSetComboBox->Clear();

	if(m_pcImmediateSetComboBox)
	  m_pcImmediateSetComboBox->Clear();

	if(m_pcDelaySetComboBox)
	  m_pcDelaySetComboBox->Clear();

	if(m_pcScheduledSetComboBox)
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

		for (unsigned int i = 0; i < l_pcColListAttr->GetColCount(); i++)
		{
			wxString l_sSetName = l_pcColListAttr->GetColLabel(i);

			if (l_sSetName == wxT("Color"))//old snoopy version has no : seprator in the color column
			{
					m_pcMarkingSetComboBox->Append(wxT("Main"));
					i = i + 2;
					continue;
			}

			if (l_sSetName.Contains(wxT(":")))//by george
			{
				l_sSetName = l_sSetName.BeforeFirst(wxT(':'));
				m_pcMarkingSetComboBox->Append(l_sSetName);
			}
			i = i + 2;
		}

		//Select the first marking
		if(m_pcMarkingSetComboBox->GetCount() <= l_pcColListAttr->GetActiveColumn())
			m_pcMarkingSetComboBox->SetSelection(0);
		else
			m_pcMarkingSetComboBox->SetSelection(l_pcColListAttr->GetActiveColumn());			
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

	LoadConstantsSetsForColPN();//by george
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

		double l_nTime=m_pcMainSimulator->GetOutputStartPoint()+m_pcMainSimulator->GetOutputSampleSize() * l_nRow;

		l_sOutput << dssd::aux::toString(l_nTime);
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

	wxFFile l_File;
		wxFileName fn = wxFileName(wxT("parsed-ode.txt"));
		wxString l_sOutName;
		wxString l_sSelName;
		wxString l_sSelDir;
		wxString l_sFileExt;
		wxString l_sFileExtFilter;
		bool l_bSelAgain;
		l_sSelName = fn.GetName() + wxT(".txt");
		l_sFileExt = wxT("txt");
		l_sFileExtFilter = wxT("Text file (*.txt)|*.txt");
		l_sSelDir = wxT("");

		if (m_pcMainSimulator->GetSimulatorClass() == wxT("Stochastic"))
		{
			SP_LOGERROR(wxT("ODEs cannot be saved for stochastic simulation"));

			return;
		}
		do
		{
			l_bSelAgain = false;
			l_sOutName = wxFileSelector(wxT("Simulation result file"), l_sSelDir, l_sSelName, l_sFileExt, l_sFileExtFilter, wxFD_SAVE);
			if (l_sOutName.empty())
			{
				break;
			}
			if (wxFileName::FileExists(l_sOutName))
			{
				int l_nAnswer = SP_MESSAGEBOX(wxT("File ") + l_sOutName + wxT(" exists.")
				wxT(" Overwrite?"), wxT("Overwrite?"), wxYES_NO | wxICON_QUESTION);
				if (l_nAnswer == wxNO)
				{
					l_bSelAgain = true;
					wxFileName l_tmpFN = wxFileName(l_sOutName);
					l_sSelDir = l_tmpFN.GetPath();
					l_sSelName = l_tmpFN.GetFullName();
				}
			}
		}
		while (l_bSelAgain);

		if (!InitializeSimulator())
		{
			SP_MESSAGEBOX(wxT("Error: Can not Initialize Place ODE"));
			return;
		}

		wxString l_sResult;

	    unsigned long l_nODECount=(dynamic_cast<spsim::HybridSimulator*>(m_pcMainSimulator))->
	    		                      GetCurrentMarking()->GetContinuousPlaceCount();;

		//Iterate for all the places
		for (unsigned long l_nPlacePos = 0; l_nPlacePos <l_nODECount; l_nPlacePos++)
		{
			wxString l_sName = m_msPlaceNames[l_nPlacePos];
			l_sResult << wxT("d")<<l_sName << wxT("/dt = ");
			l_sResult << (dynamic_cast<spsim::HybridSimulator*>(m_pcMainSimulator))->GetPlaceODEString(l_nPlacePos) << wxT("\n\n");
		}

		//Cumulative propensity
		l_sResult << wxT("\n\n=========== Total propensitiy ODE ==========\n\n");

		l_sResult << wxT("da0")<<wxT("/dt = ");
		l_sResult << (dynamic_cast<spsim::HybridSimulator*>(m_pcMainSimulator))->GetPlaceODEString(m_nContinuousPlaceCount) << wxT("\n\n");

		//after the simulator is initialized, it will automatically set running. Therefore we need to stop it here
		m_pcMainSimulator->AbortSimulation();

		if (l_sOutName.empty())
			return;
		if (!l_File.Open(l_sOutName.c_str(), wxT("wt")))
			return;
		l_File.Write(l_sResult);
		if (l_File.Close())
			return;
}


void SP_DLG_ColHPNSimultionResults::LoadPlaces()
{
	
	unsigned long l_nPlaceCount=m_pcUnfoldedNet->GetPlaceNames()->size();

	m_pcMainSimulator->SetPlaceCount(m_pcUnfoldedNet->GetPlaceNames()->size());
		  
	 m_pcMainSimulator->SetPlaceNames(*m_pcUnfoldedNet->GetPlaceNames());

	 // m_pcMainSimulator->SetInitialMarking(*m_pcUnfoldedNet->GetCurHybPNMarking());

	 //continuous place count: workaround
	 m_nContinuousPlaceCount=m_pcUnfoldedNet->GetContinuousPlaceCount();

	SP_VectorBool l_abPlaceType(m_nContinuousPlaceCount,true);

	if( m_sSimulatorType==wxT("Continuous"))
	{
		l_abPlaceType.resize(l_nPlaceCount,true);

	  	m_nContinuousPlaceCount=l_nPlaceCount;
	}
	else
	{
		l_abPlaceType.resize(l_nPlaceCount,false);
	}

	SP_VectorBool l_abIsFixed(l_nPlaceCount,false);
	unsigned long l_nPosition = 0;
	for(auto const& it : m_pcUnfoldedNet->GetUnfoldedContPlaces())
	{
		for(auto const& it2 : it.second)
		{
			l_abIsFixed[l_nPosition] = it2.second.m_bFixed;
			++l_nPosition;
		}
	}
	for(auto const& it : m_pcUnfoldedNet->GetUnfoldedDiscPlaces())
	{
		for(auto const& it2 : it.second)
		{
			l_abIsFixed[l_nPosition] = it2.second.m_bFixed;
			++l_nPosition;
		}
	}

	//set fixed flag 18.1.21 by george bugfix
	// (dynamic_cast<spsim::HybridSimulator*>(m_pcMainSimulator))->SetFixedFlag(l_abIsFixed);
	if (m_sSimulatorType == wxT("Stochastic"))
	{
			(dynamic_cast<spsim::StochasticSimulator*>(m_pcMainSimulator))->SetFixedFlag(l_abIsFixed);
	}
	else
	{
			(dynamic_cast<spsim::HybridSimulator*>(m_pcMainSimulator))->SetFixedFlag(l_abIsFixed);
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
            
            SP_VectorStdString* l_pcTransitionName		=	m_pcUnfoldedNet->GetTransitionNames();
            SP_VectorString* l_pcTransitionFunction	=	m_pcUnfoldedNet->GetCurRateFunction();
			SP_VectorString* l_pcTransitionType		=	m_pcUnfoldedNet->GetTransNodeType();

			if (l_pcTransitionFunction)//bugfix:by george
				{
					for (auto itVectorFun = l_pcTransitionFunction->begin(); itVectorFun != l_pcTransitionFunction->end(); ++itVectorFun)
					{
						if ((itVectorFun)->IsEmpty())
						{
							*itVectorFun = wxT("0");
						}
					}
				}


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

			//if (IsConstantArcWeight(l_sArcWeight, l_nNumericArcWeight))
			if (IsEvaluatedArcWeight(l_sArcWeight, l_nNumericArcWeight))
			{
				m_pcMainSimulator->SetPreTransitionConnection(l_itConnection->m_nTranPos,l_itConnection->m_nPlaceID, l_nArcType, l_nNumericArcWeight);
			}
			else
			{
				//SP_LOGMESSAGE(l_sArcWeight);

				m_pcMainSimulator->SetPreSelfModifyingWeights(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID,l_nArcType, l_sArcWeight);
			}



			//m_pcMainSimulator->SetPreTransitionConnection(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID, GetConnectionType(l_itConnection->m_sArcClass), l_nWeight);
		}

		//Load transitions' post place
		l_pcPostConnection = m_pcUnfoldedNet->GetArcs_Transition2Place();
		for (l_itConnection = l_pcPostConnection->begin(); l_itConnection != l_pcPostConnection->end(); l_itConnection++)
		{
			double l_nNumericArcWeight = 0;

			wxString l_sArcWeight=l_itConnection->m_sMultiplicity;

			 
			
			//if (IsConstantArcWeight(l_sArcWeight, l_nNumericArcWeight))
			if (IsEvaluatedArcWeight(l_sArcWeight, l_nNumericArcWeight))
			{
				m_pcMainSimulator->SetPostTransitionConnection(l_itConnection->m_nTranPos,l_itConnection->m_nPlaceID,l_nNumericArcWeight);
			}
			else
			{
				//SP_LOGMESSAGE(l_sArcWeight);

				m_pcMainSimulator->SetPostSelfModifyingWeights(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID,l_sArcWeight);
			}


			//m_pcMainSimulator->SetPostTransitionConnection(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID, l_nWeight);
		}
}

void SP_DLG_ColHPNSimultionResults::LoadParameters()
{
	SP_DS_Nodeclass* l_pcNodeclass;
	 
	SP_ListNode::const_iterator l_itElem;
	SP_DS_ColListAttribute* l_pcColList;
	SP_VectorStdString l_asParameterNames;
	SP_VectorDouble l_anParameterValue;


	//------- update user selection especially for constants // be george

	for (size_t i = 0; i < m_apcColListAttr.size(); i++)
	{
		if (m_apcColListAttr[i])
		{
			if (i<m_apcComboBoxes.size())
				m_apcColListAttr[i]->SetActiveList(m_apcComboBoxes[i]->GetSelection());
			else
			{
				m_apcColListAttr[i]->SetActiveList(m_apcComboBoxes[i - m_nGroupCounts]->GetSelection());
			}
		}
	}


	//-----------------------------------------
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
	SP_ListMetadata::const_iterator it;

	for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
	{
		SP_DS_Metadata* l_pcConstant = *it;
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		wxString l_sType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcConstant->GetAttribute(wxT("Type")))->GetValue();///Added by G.A

		SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();


		SP_DS_FunctionRegistryEntry l_FE = l_pcFR->lookUpFunction(l_sName);
		if (l_FE.IsOk())
		{
			SP_FunctionPtr l_Function = l_FE.getFunction();

			double l_nValue = 0.0;


			if (l_Function->isValue())
			{
				if (l_sType == wxT("int"))
				{
					l_nValue = (int)l_Function->getValue();
					l_asParameterNames.push_back(l_sName);
					l_anParameterValue.push_back(l_nValue);
				}
				else if (l_sType == wxT("double"))
				{
					l_nValue = l_Function->getValue();
					l_asParameterNames.push_back(l_sName);
					l_anParameterValue.push_back(l_nValue);
				}
				wxString l_sConstVal;
				l_sConstVal << l_nValue;

			}
			else
			{
				//evaluate string
				wxString l_sType = l_pcConstant->GetAttribute(wxT("Type"))->GetValueString();
				if (l_sType == wxT("int"))
				{
					l_nValue = SP_DS_FunctionEvaluatorLong{ l_pcFR, l_Function }();
					wxString l_sConstVal;
					l_sConstVal << l_nValue;


				}
				else if (l_sType == wxT("double"))
				{
					l_nValue = SP_DS_FunctionEvaluatorDouble{ l_pcFR, l_Function }();
				}
				l_pcFR->registerFunction(l_sName, to_string(l_nValue));
			}
		}
	}
	/*********************************/


	auto l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);
	auto l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i,0);
		wxString l_sType = l_pcColList->GetCell(i,1);
		wxString l_sConstantvalue = l_pcColList->GetCell(i,2);
		if(l_sType == wxT("int"))
		{
		 
			double l_nValue;
			 
		}
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

		SP_VectorStdString* l_pvCurrentInfo;

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


void SP_DLG_ColHPNSimultionResults::UpdateChoices()//by george for constant harmonizing
{
	m_choicesForColPNs.clear();
	//l_pcGraph->GetMetadataclass();
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
	if (mc)
	{
		SP_ListMetadata::const_iterator it;
		SP_DS_Metadata* l_pcMetadata = NULL;

		for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
		{
			l_pcMetadata = *it;
			SP_DS_Attribute* l_pcAttr = l_pcMetadata->GetAttribute(wxT("Group"));
			if (l_pcAttr)
			{
				wxString l_sGroup = l_pcAttr->GetValueString();
				m_choicesForColPNs.insert(l_sGroup);
			}
		}
	}
}

void SP_DLG_ColHPNSimultionResults::LoadConstantsSetsForColPN()
{
	int ss = m_apcColListAttr.size();

	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);

	SP_ListMetadata::const_iterator l_itElem;

	SP_SetString::iterator l_itChoice;
	for (l_itChoice = m_choicesForColPNs.begin(); l_itChoice != m_choicesForColPNs.end(); ++l_itChoice)
	{
		wxString l_sChoice = *l_itChoice;
		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin(); l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcConstant = *l_itElem;
			wxString l_sGroup = dynamic_cast<SP_DS_TextAttribute*>(l_pcConstant->GetAttribute(wxT("Group")))->GetValue();
			if (l_sChoice == l_sGroup && m_iModifyCount == 0 && m_mGroup2Selction.find(l_sGroup)== m_mGroup2Selction.end())
			{
				m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList"))));
				SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));
				m_mGroup2Selction[l_sGroup] = l_pcColList->GetActiveColumn();
				break;
			}
		}
	}


	//int start = m_apcColListAttr.size() - ss;
	for (size_t j =0; j < m_apcColListAttr.size(); j++)
	{
		SP_DS_ColListAttribute* l_pcAttr = m_apcColListAttr[j];
		//int l_Index = j - m_nGroupCounts;
		wxChoice* l_pcCombobox = m_apcComboBoxes[j];
		l_pcCombobox->Clear();
		if (l_pcAttr)
		{
			for (unsigned int i = 0; i < l_pcAttr->GetRowCount(); i++)
			{
				wxString l_sSetName = l_pcAttr->GetCell(i, 0);
				l_pcCombobox->Append(l_sSetName);
			}
			l_pcCombobox->SetSelection(l_pcAttr->GetActiveList());
		}
	}
}


void SP_DLG_ColHPNSimultionResults::OnConstantsSetsChanged(wxCommandEvent& p_cEvent)
{
	int l_nColoringGroup = 0;
		bool l_bColoringGroup = false;
		if (m_mGroup2Selction.find(wxT("coloring")) != m_mGroup2Selction.end())
		{
			l_nColoringGroup = m_mGroup2Selction.find(wxT("coloring"))->second;
		}


		unsigned i = 0;
		for (auto it = m_mGroup2Selction.begin(); it != m_mGroup2Selction.end(); ++it)
		{
			(it)->second = m_apcComboBoxes[i]->GetSelection();

			if (it->first == wxT("coloring") )
			{
				if (l_nColoringGroup != it->second)
				{
					l_bColoringGroup = true;
				}
			}

			i++;
		}

		if (l_bColoringGroup)
		{
			SP_SetString::iterator l_itChoice;
			int l_ncoloringGroupPosition = 0;

			for (l_itChoice = m_choicesForColPNs.begin(); l_itChoice != m_choicesForColPNs.end(); ++l_itChoice)
			{
				if (*l_itChoice == wxT("coloring"))
				{
					break;
				}
				++l_ncoloringGroupPosition;
			}

			m_apcColListAttr[l_ncoloringGroupPosition]->SetActiveList(m_apcComboBoxes[l_ncoloringGroupPosition]->GetSelection());

			if (!LoadUnfoldedPlaces())
			{
				SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
				return;
			}
			RefreshExternalWindows(true);
		}
}

void SP_DLG_ColHPNSimultionResults::OnModifyConstants(wxCommandEvent& p_cEvent)
{

	SP_DLG_ConstantDefinition* l_pcConstantsDialog = new SP_DLG_ConstantDefinition(NULL);
	if (l_pcConstantsDialog->ShowModal() == wxID_OK)
	{
		UpdateChoices();
		if (m_choicesForColPNs.size() != m_nGroupCounts)
		{//special case, when a user add a new group during simulation
			m_iModifyCount = 0;
			//m_apcColListAttr.clear();

			m_nGroupCounts = m_choicesForColPNs.size();//update groups size 
			LoadSets();
		}
		else {
			m_iModifyCount++;

			LoadSets();

			unsigned i = 0;
			for (auto it = m_mGroup2Selction.begin(); it != m_mGroup2Selction.end(); ++it)
			{
				m_apcComboBoxes[i]->SetSelection((it)->second);
				i++;
			}
		}

	}

}

void SP_DLG_ColHPNSimultionResults::LoadObservers()
{

	if (!m_pcGraph->GetNetclass()->GetDisplayName().Contains("Colored"))
	{
		return;
	}

	std::map<std::string, unsigned int> l_mPlaceToPosition;
	std::map<std::string, unsigned int> l_mTransitionToPosition;
	//14.12
	SP_VectorStdString* l_pvsColoredPlaceNames = m_pcUnfoldedNet->GetColoredPlaceNames();
	SP_VectorStdString* l_pvsColoredTransNames = m_pcUnfoldedNet->GetColoredTransitionNames();

	for (unsigned i = 0; i < l_pvsColoredPlaceNames->size(); i++)
	{
		wxString l_sPlName = (*l_pvsColoredPlaceNames)[i];
		m_mPlaceName2Position[l_sPlName] = i;
	}

	for (unsigned i = 0; i < l_pvsColoredTransNames->size(); i++)
	{
		wxString l_sTName = (*l_pvsColoredTransNames)[i];
		m_mTransitionName2Position[l_sTName] = i;
	}

	for (auto const& elem : m_mPlaceName2Position) {
		l_mPlaceToPosition[elem.first] = elem.second;
	}

	for (auto const& elem : m_mTransitionName2Position) {
		l_mTransitionToPosition[elem.first] = elem.second;
	}

	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_OBSERVER);
	if (l_pcMetadataclass)
	{
		SP_VectorString l_asParameterNames;

		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin(); l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcMetadata = *l_itElem;
			wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();
			wxString l_sType = l_pcMetadata->GetAttribute(wxT("Type"))->GetValueString();
			wxString l_sBody = l_pcMetadata->GetAttribute(wxT("Body"))->GetValueString();
			wxString l_sColType = l_pcMetadata->GetAttribute(wxT("ColPNType"))->GetValueString();
			unsigned long l_nPos = 0;


			if (l_sColType == wxT("Mixed"))
			{
				if (std::find(m_vMixedObservers.begin(), m_vMixedObservers.end(), l_sName) == m_vMixedObservers.end())
				{
					m_vMixedObservers.push_back(l_sName);
					continue;
				}
			}

			if (l_sColType == wxT("Place instance") && l_sType == wxT("Place"))
			{
				if (std::find(m_vUnfoldedPlacesObservers.begin(), m_vUnfoldedPlacesObservers.end(), l_sName) == m_vUnfoldedPlacesObservers.end())

				{
					m_vUnfoldedPlacesObservers.push_back(l_sName);

				}
			}

			if (l_sColType == wxT("Transition instance") && l_sType == wxT("Transition"))
			{
				if (std::find(m_vUnfoldedTransitionsObservers.begin(), m_vUnfoldedTransitionsObservers.end(), l_sName) == m_vUnfoldedTransitionsObservers.end())

				{
					m_vUnfoldedTransitionsObservers
						.push_back(l_sName);
				}
			}

			if (l_sColType == wxT("Col|Place") && l_sType == wxT("Place") && std::find(m_vColPlaceObservers.begin(), m_vColPlaceObservers.end(), l_sName) == m_vColPlaceObservers.end())

			{
				m_vColPlaceObservers.push_back(l_sName);
			}
			if (l_sColType == wxT("Col|Transition")  && l_sType == wxT("Transition") && std::find(m_vColTransObservers.begin(), m_vColTransObservers.end(), l_sName) == m_vColTransObservers.end())
			{
				m_vColTransObservers.push_back(l_sName);
			}
			// check the function
			SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();
			SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(l_sBody));
			if (!l_pcFunction)
			{
				SP_MESSAGEBOX(wxT("the body of observer ") + l_sName + wxT(" is not correct"), wxT("Check Observer"), wxOK | wxICON_ERROR);
				continue;
			}

			SP_FunctionPtr l_pcExpanded(l_pcFR->substituteFunctions(l_pcFunction));


			std::set<std::string> variables;
			l_pcExpanded->getVariables(variables);
			bool l_bIsValid = true;
			if (l_sType == wxT("Place"))
			{



				if (std::find(m_asPlaceNames.begin(), m_asPlaceNames.end(), l_sName) != m_asPlaceNames.end())
				{
					continue;
				}

				m_asPlaceNames.push_back(l_sName);
				m_mPlaceName2Position[l_sName] = l_nPos = m_asPlaceNames.size() - 1;
				m_mObserverPlaceFunctions[l_nPos] = l_pcExpanded;


				l_mPlaceToPosition[l_sName] = l_nPos;

				dssd::functions::convertOptions co;
				l_pcExpanded->setVariableIds(l_mPlaceToPosition, co);
			}
			else {
				if (l_sType != wxT("Place") &&/* l_sColType == wxT("Unfolded|Transition")*/l_sColType == wxT("Transition instance"))
				{
					continue;
					//m_msTransitionNames
					m_asTransitionNames.push_back(l_sName);
					m_mTransitionName2Position[l_sName] = l_nPos = m_asTransitionNames.size() - 1;
					m_mObserverTransitionFunctions[l_nPos] = l_pcExpanded;
					l_mTransitionToPosition[l_sName] = l_nPos;

					continue;
				}


				if (std::find(m_asTransitionNames.begin(), m_asTransitionNames.end(), l_sName) != m_asTransitionNames.end())
				{
					continue;
				}
				m_asTransitionNames.push_back(l_sName);
				m_mTransitionName2Position[l_sName] = l_nPos = m_asTransitionNames.size() - 1;
				m_mObserverTransitionFunctions[l_nPos] = l_pcExpanded;

				l_mTransitionToPosition[l_sName] = l_nPos;

				dssd::functions::convertOptions co;
				l_pcExpanded->setVariableIds(l_mTransitionToPosition, co);
			}
		}
	}
}

SP_FunctionPtr SP_DLG_ColHPNSimultionResults::RegesterFunctionVariables(const wxString& p_sType, const wxString& p_sObserverName, wxString p_sExpression)
{

	wxString	l_sBody = SubstituteConstants(p_sExpression);
	SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();
	SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(l_sBody));

	if (!l_pcFunction)
	{
		SP_MESSAGEBOX(wxT("the body of observer ") + p_sObserverName + wxT(" is not correct"), wxT("Check Observer"), wxOK | wxICON_ERROR);
		return NULL;
	}

	std::map<std::string, unsigned int> l_mVar2Pos1;
	std::map<std::string, unsigned int> l_mElementToPosition;

	if (p_sType == wxT("Place instance"))
	{
		for (unsigned int i = 0; i < m_msPlaceNames.size(); i++)
		{
			l_mVar2Pos1[m_msPlaceNames[i]] = i;
		}
	}
	else if (p_sType == wxT("Transition instance"))
	{
		for (unsigned int i = 0; i < m_msTransitionNames.size(); i++)
		{
			l_mVar2Pos1[m_msTransitionNames[i]] = i;
		}
	}
	else if (p_sType == wxT("Mixed"))
	{
		std::map<std::string, unsigned int> l_mapvar2Pos;
		ComputeCombinedMatrix(l_sBody, l_mapvar2Pos);
		l_mElementToPosition = l_mapvar2Pos;
	}

	SP_FunctionPtr l_pcExpanded(l_pcFR->substituteFunctions(l_pcFunction));
	if (p_sType != wxT("Mixed"))
	{
		std::set<std::string> variables;
		l_pcExpanded->getVariables(variables);

		for (auto strVar : variables)
		{
			std::map<std::string, unsigned int>::iterator l_it;
			l_it = l_mVar2Pos1.find(strVar);
			if (l_it != l_mVar2Pos1.end())
			{
				l_mElementToPosition[strVar] = l_it->second;
			}
			else
			{//something wrong
				return  NULL;
			}
		}
	}

	if (l_mElementToPosition.size() > 0)
	{
		dssd::functions::convertOptions co;
		l_pcExpanded->setVariableIds(l_mElementToPosition, co);
	}

	return l_pcExpanded;
}


void SP_DLG_ColHPNSimultionResults::UpdateUnfoldedObservers()
{

	if (m_vUnfoldedPlacesObservers.size() > 0)
	{
		SP_Vector2DDouble vec(m_pcMainSimulator->GetResultMatrix().size(), SP_VectorDouble(m_vUnfoldedPlacesObservers.size(), 0.0));
		m_anResultUnfoldedPlaceObserversMatrix = vec;
	}

	if (m_vUnfoldedTransitionsObservers.size() > 0)
	{
		SP_Vector2DDouble vec(m_pcMainSimulator->GetResultMatrix().size(), SP_VectorDouble(m_vUnfoldedTransitionsObservers.size(), 0.0));
		m_anResultUnfoldedTransitionObserversMatrix = vec;
	}
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_OBSERVER);
	if (l_pcMetadataclass)
	{
		SP_VectorString l_asParameterNames;

		SP_ListMetadata::const_iterator l_itElem;
		int l_nObserverPlacesIndex = 0;
		int l_nObserverTranIndex = 0;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin(); l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcMetadata = *l_itElem;
			wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();
			wxString l_sType = l_pcMetadata->GetAttribute(wxT("Type"))->GetValueString();
			wxString l_sBody = l_pcMetadata->GetAttribute(wxT("Body"))->GetValueString();

			wxString l_sColType = l_pcMetadata->GetAttribute(wxT("ColPNType"))->GetValueString();
			unsigned long l_nPos = 0;
			bool l_bIsValid = true;


			if (l_sType == wxT("Place") && l_sColType == wxT("Place instance"))
			{
				SP_FunctionPtr funcPtrEvaluator = RegesterFunctionVariables(wxT("Place instance"), l_sName, l_sBody);

				if (funcPtrEvaluator == NULL) continue;

				ObtainSimulationMatrix(false);//switch shared result mat to the marking result

				SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();

				if (m_anResultMatrix.size() > 0)
				{
					for (unsigned int l_nRow = 0; l_nRow < m_anResultMatrix.size(); l_nRow++)
					{

						double l_Val = SP_DS_FunctionEvaluatorDouble{ l_pcFR, funcPtrEvaluator, std::numeric_limits<double>::min() }(
							m_anResultMatrix[l_nRow]);
						m_anResultUnfoldedPlaceObserversMatrix[l_nRow][l_nObserverPlacesIndex] = l_Val;
					}
				}
				l_nObserverPlacesIndex++;
			}
			else if (l_sType == wxT("Transition") && l_sColType == wxT("Transition instance")/*l_sColType == wxT("Unfolded|Transition")*/)
			{

				SP_FunctionPtr funcPtrEvaluator = RegesterFunctionVariables(wxT("Transition instance"), l_sName, l_sBody);

				if (funcPtrEvaluator == NULL) continue;


				ObtainSimulationMatrix(true);//switch shared result mat to the rate matrix

				SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();

				if (m_anResultMatrix.size() > 0)
				{
					for (unsigned int l_nRow = 0; l_nRow < m_anResultMatrix.size(); l_nRow++)
					{

						double l_dVal = SP_DS_FunctionEvaluatorDouble{ l_pcFR, funcPtrEvaluator, std::numeric_limits<double>::min() }(
							m_anResultMatrix[l_nRow]);
						m_anResultUnfoldedTransitionObserversMatrix[l_nRow][l_nObserverTranIndex] = l_dVal;
					}
					l_nObserverTranIndex++;
				}
			}
		}

	}

}

void SP_DLG_ColHPNSimultionResults::UpdateObservers(const wxString& p_sObsersverType, unsigned int p_nRowCount)
{

	if (m_vUnfoldedTransitionsObservers.size() > 0 || m_vUnfoldedPlacesObservers.size() > 0)
	{
		UpdateUnfoldedObservers();
	}
	SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();
	SP_FunctionPtr l_pcFunction;

	if (p_sObsersverType.IsSameAs(wxT("Place"))) {// observer over coloured place
		int l_nObserverPlacesIndex = 0;
		if (m_vColPlaceObservers.size() > 0)
		{
			SP_Vector2DDouble vec(m_pcMainSimulator->GetResultMatrix().size(), SP_VectorDouble(m_vColPlaceObservers.size(), 0.0));
			m_anResultColPlaceObserversMatrix = vec;
		}
		if (m_aanColPlaceResults.size()>0)
			for (auto it : m_mObserverPlaceFunctions) {
				l_pcFunction = it.second;

				for (unsigned int l_nRow = 0; l_nRow < m_aanColPlaceResults.size(); l_nRow++) {

					double l_Val = SP_DS_FunctionEvaluatorDouble{ l_pcFR, l_pcFunction, std::numeric_limits<double>::min() }(
						m_aanColPlaceResults[l_nRow]);
					m_anResultColPlaceObserversMatrix[l_nRow][l_nObserverPlacesIndex] = l_Val;
				}
				l_nObserverPlacesIndex++;
			}
	}
	else if (p_sObsersverType.IsSameAs(wxT("Transition"))) {// observer over coloured transition

		int l_nObserverTransIndex = 0;//observers counter

									  //init result matrix
		if (m_vColTransObservers.size() > 0)
		{
			SP_Vector2DDouble vec(m_pcMainSimulator->GetResultMatrix().size(), SP_VectorDouble(m_vColTransObservers.size(), 0.0));
			m_anResultColTransObserversMatrix = vec;
		}

		for (auto it : m_mObserverTransitionFunctions) {
			l_pcFunction = it.second;

			for (unsigned int l_nRow = 0; l_nRow < m_aanColTransResults.size(); l_nRow++) {
				double l_Val = SP_DS_FunctionEvaluatorDouble{ l_pcFR, l_pcFunction, std::numeric_limits<double>::min() }(
					m_aanColTransResults[l_nRow]);
				m_anResultColTransObserversMatrix[l_nRow][l_nObserverTransIndex] = l_Val;

			}
			l_nObserverTransIndex++;
		}
	}
	else
	{//mmixed observer

		if (m_vMixedObservers.size() > 0)
		{
			SP_Vector2DDouble vec(m_pcMainSimulator->GetResultMatrix().size(), SP_VectorDouble(m_vMixedObservers.size(), 0.0));
			m_anResultMixedObserversMatrix = vec;
		}

		SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_OBSERVER);
		if (l_pcMetadataclass)
		{
			SP_VectorString l_asParameterNames;

			SP_ListMetadata::const_iterator l_itElem;
			int l_nObserverPlacesIndex = 0;
			for (l_itElem = l_pcMetadataclass->GetElements()->begin(); l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
			{
				SP_DS_Metadata* l_pcMetadata = *l_itElem;
				wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();
				wxString l_sType = l_pcMetadata->GetAttribute(wxT("Type"))->GetValueString();
				wxString l_sBody = l_pcMetadata->GetAttribute(wxT("Body"))->GetValueString();
				wxString l_sColType = l_pcMetadata->GetAttribute(wxT("ColPNType"))->GetValueString();

				if (l_sColType == _T("Mixed"))
				{
					SP_FunctionPtr funcPtrEvaluator = RegesterFunctionVariables(wxT("Mixed"), l_sName, l_sBody);

					if (funcPtrEvaluator == NULL) continue;

					SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();

					for (unsigned int l_nRow = 0; l_nRow < m_anCombinedResultMatrix.size(); l_nRow++)
					{

						double l_dVal = SP_DS_FunctionEvaluatorDouble{ l_pcFR, funcPtrEvaluator, std::numeric_limits<double>::min() }(
							m_anCombinedResultMatrix[l_nRow]);
						m_anResultMixedObserversMatrix[l_nRow][l_nObserverPlacesIndex] = l_dVal;
					}
					l_nObserverPlacesIndex++;
				}
			}
		}
	}
}

wxString SP_DLG_ColHPNSimultionResults::SubstituteConstants(const wxString& p_sExpression)

{
	wxString l_sResultingExp;

	if (m_msParameterName2Value.size()>0)
	{
		wxStringTokenizer l_StringTok(p_sExpression, wxT("()+*/^=<>!%&|,- "), wxTOKEN_RET_EMPTY_ALL);
		while (l_StringTok.HasMoreTokens())
		{
			wxString l_sToken = l_StringTok.GetNextToken();
			l_sToken.Replace(_T(" "), _T(""));
			if (!l_sToken.IsEmpty())
			{
				auto it = m_msParameterName2Value.find(l_sToken);


				if (it != m_msParameterName2Value.end())
				{
					double l_DataType = it->second;
					int l_nMarking = it->second;
					l_sResultingExp << l_nMarking;

				}
				else
				{
					l_sResultingExp << l_sToken;
				}
			}
			wxChar l_chDelim = l_StringTok.GetLastDelimiter();
			if (l_chDelim != '\0')
			{
				l_sResultingExp << l_chDelim;
			}
		}
	}

	return l_sResultingExp;
}


void SP_DLG_ColHPNSimultionResults::ComputeCombinedMatrix(const wxString& p_sObserverBody, std::map<std::string, unsigned int>& p_mapvar2Pos)
{
	SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();
	SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(p_sObserverBody));
	if (!l_pcFunction)
	{
		SP_MESSAGEBOX(wxT("the body of observer ") + p_sObserverBody + wxT(" is not correct"), wxT("Check Observer"), wxOK | wxICON_ERROR);
		return;
	}

	SP_FunctionPtr l_pcExpanded(l_pcFR->substituteFunctions(l_pcFunction));
	std::set<std::string> variables;
	l_pcExpanded->getVariables(variables);

	if (variables.size() > 0)
	{
		SP_Vector2DDouble vec(m_pcMainSimulator->GetResultMatrix().size(), SP_VectorDouble(0, 0.0));
		m_anCombinedResultMatrix = vec;
	}
	for (auto v : variables)
	{
		wxString l_sVarName = v;
		bool l_bIsFound = false;
		for (int i = 0; i < m_msColoredPlaceNames.size(); i++)
		{//col places

			if (m_msColoredPlaceNames[i] == l_sVarName)
			{
				l_bIsFound = true;
				int l_npos = 0;
				for (int j = 0; j < m_anCombinedResultMatrix.size(); j++)
				{
					m_anCombinedResultMatrix[j].push_back(m_aanColPlaceResults[j][i]);
				}
				l_npos = m_anCombinedResultMatrix[0].size() - 1;
				p_mapvar2Pos[v] = l_npos;
				break;
			}
		}

		if (l_bIsFound) continue;

		for (int i = 0; i < m_msColoredTransitionNames.size(); i++)
		{//col transitions

			if (m_msColoredTransitionNames[i] == l_sVarName)
			{
				l_bIsFound = true;
				int l_npos = 0;
				for (int j = 0; j < m_anCombinedResultMatrix.size(); j++)
				{
					m_anCombinedResultMatrix[j].push_back(m_aanColTransResults[j][i]);
				}
				l_npos = m_anCombinedResultMatrix[0].size() - 1;
				p_mapvar2Pos[v] = l_npos;
				break;
			}
		}

		if (l_bIsFound) continue;

		for (int i = 0; i < m_msPlaceNames.size(); i++)
		{//unfolded places

			if (m_msPlaceNames[i] == l_sVarName)
			{
				int l_npos = 0;
				l_bIsFound = true;
				ObtainSimulationMatrix(false);//switch to unfolded places matrix
				for (int j = 0; j < m_anCombinedResultMatrix.size(); j++)
				{
					m_anCombinedResultMatrix[j].push_back(m_anResultMatrix[j][i]);
				}
				l_npos = m_anCombinedResultMatrix[0].size() - 1;
				p_mapvar2Pos[v] = l_npos;
				break;
			}
		}


		if (l_bIsFound) continue;

		for (int i = 0; i < m_msTransitionNames.size(); i++)
		{//unfolded transitions

			if (m_msTransitionNames[i] == l_sVarName)
			{
				int l_npos = 0;
				l_bIsFound = true;
				ObtainSimulationMatrix(true);//switch to unfolded trans matrix
				for (int j = 0; j < m_anCombinedResultMatrix.size(); j++)
				{
					m_anCombinedResultMatrix[j].push_back(m_anResultMatrix[j][i]);
				}
				l_npos = m_anCombinedResultMatrix[0].size() - 1;
				p_mapvar2Pos[v] = l_npos;
				break;
			}
		}

		if (l_bIsFound) continue;

		for (int i = 0; i < m_vUnfoldedPlacesObservers.size(); i++)
		{//unfolded place observers, observer depends on a pre-defined place inst. obs

			if (m_vUnfoldedPlacesObservers[i] == l_sVarName)
			{
				int l_npos = 0;
				l_bIsFound = true;
				//	ObtainSimulationMatrix(true);//switch to unfolded trans matrix
				for (int j = 0; j < m_anCombinedResultMatrix.size(); j++)
				{
					m_anCombinedResultMatrix[j].push_back(m_anResultUnfoldedPlaceObserversMatrix[j][i]);
				}
				l_npos = m_anCombinedResultMatrix[0].size() - 1;
				p_mapvar2Pos[v] = l_npos;
				break;
			}
		}

		if (l_bIsFound) continue;

		for (int i = 0; i < m_vUnfoldedTransitionsObservers.size(); i++)
		{//unfolded trans observers, observer depends on a pre-defined trans inst. obs

			if (m_vUnfoldedTransitionsObservers[i] == l_sVarName)
			{
				int l_npos = 0;
				l_bIsFound = true;
				//	ObtainSimulationMatrix(true);//switch to unfolded trans matrix
				for (int j = 0; j < m_anCombinedResultMatrix.size(); j++)
				{
					m_anCombinedResultMatrix[j].push_back(m_anResultUnfoldedTransitionObserversMatrix[j][i]);
				}
				l_npos = m_anCombinedResultMatrix[0].size() - 1;
				p_mapvar2Pos[v] = l_npos;
				break;
			}
		}

		if (l_bIsFound) continue;

		for (int i = 0; i < m_vColPlaceObservers.size(); i++)
		{//colored place observers, observer depends on a pre-defined col place obs

			if (m_vColPlaceObservers[i] == l_sVarName)
			{
				int l_npos = 0;
				l_bIsFound = true;

				for (int j = 0; j < m_anCombinedResultMatrix.size(); j++)
				{
					m_anCombinedResultMatrix[j].push_back(m_anResultColPlaceObserversMatrix[j][i]);
				}
				l_npos = m_anCombinedResultMatrix[0].size() - 1;
				p_mapvar2Pos[v] = l_npos;
				break;
			}
		}

		if (l_bIsFound) continue;

		for (int i = 0; i < m_vColTransObservers.size(); i++)
		{//colored trans observers, observer depends on a pre-defined col trans obs

			if (m_vColTransObservers[i] == l_sVarName)
			{
				int l_npos = 0;
				l_bIsFound = true;

				for (int j = 0; j < m_anCombinedResultMatrix.size(); j++)
				{
					m_anCombinedResultMatrix[j].push_back(m_anResultColTransObserversMatrix[j][i]);
				}
				l_npos = m_anCombinedResultMatrix[0].size() - 1;
				p_mapvar2Pos[v] = l_npos;
				break;
			}
		}

	}


}
