/*
* SP_DLG_ColFSPNSimulationResults.h
* $Author: G. Assaf
* $Version: 0.0 $
* $Revision : 0.0 $
* $Date: 7.03.2020
* @Description:
* implementation file of the Simulation result dialog for Colored Fuzzy Stochastic PN
*
*/
#include "sp_gui/dialogs/dia_colFSPN/SP_DLG_ColFSPNSimulationResults.h"
#include <wx/dcps.h>
#include <wx/printdlg.h>
#include <wx/filename.h>
#include <wx/textfile.h>
#include <wx/regex.h>
#include "export/SP_EPSDC.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStSimulationResults.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StSimGillespie.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StSimFAU.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStMarkingOverview.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StDirectExportProperties.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StMarkingOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StParameterOverview.h"
#include "sp_gui/dialogs/SP_DLG_ColListOverview.h"
#include "sp_gui/dialogs/SP_DLG_SimulationProperties.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColPlacesSelection.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColTraceAnalyzer.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_SimulatorThreadStochastic.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_ConstantDefinition.h"//constants harmonizing by george
#include "spsim/spsim.h"
#include "sp_gui/dialogs/dia_FPN/SP_DLG_FpnConstantDefinition.h"

//by george for constants harmonizing
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"

#if !defined(__WXMSW__) && !defined(__WXPM__)
#include "sp_gui/widgets/sp_plot/plot_enl.xpm"
#include "sp_gui/widgets/sp_plot/plot_shr.xpm"
#include "sp_gui/widgets/sp_plot/plot_zin.xpm"
#include "sp_gui/widgets/sp_plot/plot_zot.xpm"
#endif

IMPLEMENT_CLASS(SP_DLG_ColFSPNSimulationResults, SP_DLG_FspnSimResult)

enum
{
	SP_ID_CHOICE_SIMULATOR_CHOICE = SP_ID_LAST_ID + 200,

	SP_ID_BUTTON_MODIFY_MARKING_SETS,
	SP_ID_BUTTON_MODIFY_FUNCTION_SETS,
	SP_ID_BUTTON_MODIFY_PARAMETER_SETS,

	SP_ID_BUTTON_MODIFY_DELAY_SETS, //By Liu on 6 Mar. 2009
	SP_ID_BUTTON_MODIFY_SCHEDULE_SETS, //By Liu on 6 Mar. 2009
	SP_ID_BUTTON_MODIFY_WEIGHT_SETS,

	SP_ID_BUTTON_SIMULATION_PROPERTIES,

	SP_ID_BUTTON_LOAD_ILFORMULAE,
	SP_ID_BUTTON_ENTER_ILFORMULAE,
	SP_ID_BUTTON_CHECK_ILFORMULAE,

	SP_ID_BUTTON_CHECK_ILFORMULAE_MENU,

	SP_ID_BUTTON_MODIFY_COL_CONSTANT_SETS,// by george

	SP_ID_BUTTON_CHANGE_COL_CONSTANT_SETS

};
BEGIN_EVENT_TABLE(SP_DLG_ColFSPNSimulationResults, SP_DLG_FspnSimResult)

EVT_BUTTON(SP_ID_BUTTON_MODIFY_MARKING_SETS, SP_DLG_ColFSPNSimulationResults::OnModifyMarkingSets)
EVT_BUTTON(SP_ID_BUTTON_MODIFY_FUNCTION_SETS, SP_DLG_ColFSPNSimulationResults::OnModifyFunctionSets)
EVT_BUTTON(SP_ID_BUTTON_MODIFY_PARAMETER_SETS, SP_DLG_ColFSPNSimulationResults::OnModifyParameterSets)
EVT_BUTTON(SP_ID_BUTTON_MODIFY_DELAY_SETS, SP_DLG_ColFSPNSimulationResults::OnModifyDelaySets) //By Liu on 6 Mar. 2009
EVT_BUTTON(SP_ID_BUTTON_MODIFY_SCHEDULE_SETS, SP_DLG_ColFSPNSimulationResults::OnModifyScheduleSets)//By Liu on 6 Mar. 2009
EVT_BUTTON(SP_ID_BUTTON_MODIFY_WEIGHT_SETS, SP_DLG_ColFSPNSimulationResults::OnModifyWeightSets)//By Liu on 6 Mar. 2009
EVT_BUTTON(SP_ID_BUTTON_SIMULATION_PROPERTIES, SP_DLG_ColFSPNSimulationResults::OnSimulationProperties)

EVT_CHOICE(SP_ID_CHOICE_SIMULATOR_CHOICE, SP_DLG_ColFSPNSimulationResults::OnSimulatorSelect)

EVT_MENU(SP_ID_BUTTON_LOAD_ILFORMULAE, SP_DLG_ColFSPNSimulationResults::OnLoadApFormulae)
EVT_MENU(SP_ID_BUTTON_ENTER_ILFORMULAE, SP_DLG_ColFSPNSimulationResults::OnEnterApFormulae)
EVT_MENU(SP_ID_BUTTON_CHECK_ILFORMULAE, SP_DLG_ColFSPNSimulationResults::CheckApFormulae)
EVT_BUTTON(SP_ID_BUTTON_CHECK_ILFORMULAE_MENU, SP_DLG_ColFSPNSimulationResults::OnCheckFormulaMenu)
EVT_SIMTHREAD(SP_SIMULATION_THREAD_EVENT, SP_DLG_ColFSPNSimulationResults::OnSimulatorThreadEvent)

EVT_BUTTON(SP_ID_BUTTON_MODIFY_COL_CONSTANT_SETS, SP_DLG_ColFSPNSimulationResults::OnModifyConstants)// by george
END_EVENT_TABLE()
 
SP_DLG_ColFSPNSimulationResults::SP_DLG_ColFSPNSimulationResults(SP_DS_Graph* p_pcGraph, SP_DS_ColPN_Unfolding* p_pcUnfoldedNet, wxWindow* p_pcParent, wxString p_sHelpText, const wxString& p_sTitle,
	long p_nStyle) :
	SP_DS_ColoredPNSimulation(p_pcUnfoldedNet), SP_DLG_FspnSimResult(p_pcGraph, p_pcParent, p_sHelpText, p_sTitle, p_nStyle)
{
	m_mGroup2Position.clear();
	m_pcXAxisChoices.clear();
	m_pcXAxisChoices[wxT("Place-colored")] = &m_msColoredPlaceNames;
	m_pcXAxisChoices[wxT("Transition-colored")] = &m_msColoredTransitionNames;
	m_pcXAxisChoices[wxT("Place-uncolored")] = &m_msPlaceNames;
	m_pcXAxisChoices[wxT("Transition-uncolored")] = &m_msTransitionNames;

	wxSizer* l_pcRowSizer = NULL;

	if (m_pcGraph->GetNodeclass(wxT("Place"))->GetElements()->size() > 0)
	{
	// rows of SetsSizer
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Marking set:")), 1, wxALL | wxEXPAND, 5);
	m_apcComboBoxes.push_back(new wxChoice(m_pcPropertyWindowSetsSizer, -1, wxDefaultPosition, wxSize(100, -1)));
	l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size() - 1], 1, wxALL, 5);
	l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_MARKING_SETS, wxT("Modify")), 0, wxALL, 5);
	m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);
	m_mGroup2Position[wxT("Marking")] = m_apcComboBoxes.size() - 1;
	}

	if (m_pcGraph->GetNodeclass(wxT("Transition"))->GetElements()->size() > 0)
		{
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Function set:")), 1, wxALL | wxEXPAND, 5);
	m_apcComboBoxes.push_back(new wxChoice(m_pcPropertyWindowSetsSizer, -1, wxDefaultPosition, wxSize(100, -1)));
	l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size() - 1], 1, wxALL, 5);
	l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_FUNCTION_SETS, wxT("Modify")), 0, wxALL, 5);
	m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);
	m_mGroup2Position[wxT("Rate")] = m_apcComboBoxes.size() - 1;
		}

	if (m_pcGraph->GetNodeclass(wxT("Immediate Transition"))->GetElements()->size() > 0)
		{
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Weight set:")), 1, wxALL | wxEXPAND, 5);
	m_apcComboBoxes.push_back(new wxChoice(m_pcPropertyWindowSetsSizer, -1, wxDefaultPosition, wxSize(100, -1)));
	l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size() - 1], 1, wxALL, 5);
	l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_WEIGHT_SETS, wxT("Modify")), 0, wxALL, 5);
	m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);
	m_mGroup2Position[wxT("Weight")] = m_apcComboBoxes.size() - 1;
		}

	if (m_pcGraph->GetNodeclass(wxT("Deterministic Transition"))->GetElements()->size() > 0)
		{
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Delay set:")), 1, wxALL | wxEXPAND, 5);
	m_apcComboBoxes.push_back(new wxChoice(m_pcPropertyWindowSetsSizer, -1, wxDefaultPosition, wxSize(100, -1)));
	l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size() - 1], 1, wxALL, 5);
	l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_DELAY_SETS, wxT("Modify")), 0, wxALL, 5);
	m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);
	m_mGroup2Position[wxT("Delay")] = m_apcComboBoxes.size() - 1;
		}

	if (m_pcGraph->GetNodeclass(wxT("Scheduled Transition"))->GetElements()->size() > 0)
		{
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Schedule set:")), 1, wxALL | wxEXPAND, 5);
	m_apcComboBoxes.push_back(new wxChoice(m_pcPropertyWindowSetsSizer, -1, wxDefaultPosition, wxSize(100, -1)));
	l_pcRowSizer->Add(m_apcComboBoxes[m_apcComboBoxes.size() - 1], 1, wxALL, 5);
	l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_SCHEDULE_SETS, wxT("Modify")), 0, wxALL, 5);
	m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);
	m_mGroup2Position[wxT("Scheduled")] = m_apcComboBoxes.size() - 1;
		}


	//by george for constants harmonizing 146-157
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
		m_mGroup2Position[l_sGroup] = m_apcComboBoxes.size() - 1;
	}

	m_iModifyCount = 0;
	SetSizerAndFit(m_pcMainSizer);
}


SP_DLG_ColFSPNSimulationResults::~SP_DLG_ColFSPNSimulationResults()
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


void SP_DLG_ColFSPNSimulationResults::AddNewMarkingSet(const wxString& p_sMarkingSetName, const int& p_nRow)
{
	//Get current row result
	SP_VectorDouble l_nCurrentTableRow = m_pcMainSimulator->GetResultMatrix()[p_nRow];

	SP_DS_ColListAttribute* l_pcColList;
	SP_ListNode::const_iterator l_itElem;

	//Continuous places
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);

	map<wxString, long> l_mColoredPlaceName2Pos;
	for (unsigned i = 0; i < m_msColoredPlaceNames.size(); i++)
	{
		l_mColoredPlaceName2Pos[m_msColoredPlaceNames[i]] = i;
	}

	if (l_pcNodeclass != NULL)
	{
		//Add the new making to each node
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			//Get the ColList attribute
			l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((*l_itElem)->GetAttribute(wxT("MarkingList")));

			/////////////////////////////////
			//get new marking
			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			long l_nPos = l_mColoredPlaceName2Pos[l_sPlaceName];
			map<wxString, vector<wxString> > l_mvNewMarking;
			for (unsigned int l_nCol = m_pnsPlaceCountById[l_nPos].m_nLow; l_nCol <= m_pnsPlaceCountById[l_nPos].m_nUp; l_nCol++)
			{
				wxString l_sColor = m_msColorsOfPlace[l_nCol];
				wxString l_sValue = dssd::aux::toString(l_nCurrentTableRow[l_nCol]);
				if (l_sValue != wxT("0"))
				{
					l_mvNewMarking[l_sValue].push_back(l_sColor);
				}
			}

			////Add new Column to the marking list
			unsigned int l_nNewColumn = l_pcColList->AppendEmptyColum();
			l_pcColList->SetColLabel(l_nNewColumn, p_sMarkingSetName + wxT(": Marking"));
			map<wxString, vector<wxString> >::iterator itMap;
			for (itMap = l_mvNewMarking.begin(); itMap != l_mvNewMarking.end(); itMap++)
			{
				unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
				for (unsigned l_nNewColumn = 0; l_nNewColumn < l_pcColList->GetColCount(); l_nNewColumn++)
				{
					if (l_nNewColumn == 0)
					{
						vector<wxString> l_vColors = itMap->second;
						wxString l_sColors = wxT("");
						for (unsigned i = 0; i < l_vColors.size(); i++)
						{
							l_sColors = l_sColors + wxT(",") + l_vColors[i];
						}
						l_sColors = l_sColors.AfterFirst(wxT(','));
						l_pcColList->SetCell(l_nNewRow, l_nNewColumn, l_sColors);
					}
					if (l_nNewColumn == l_pcColList->GetColCount() - 1)
					{
						l_pcColList->SetCell(l_nNewRow, l_nNewColumn, itMap->first);
					}
				}
			}
		}
	}

	if (!LoadUnfoldedPlaces())
	{
		SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
		return;
	}
}

bool SP_DLG_ColFSPNSimulationResults::IsMarkingSetNameExist(const wxString& p_sName)
{
	SP_DS_ColListAttribute* l_pcColList = GetOneMakingAttribute();

	CHECK_POINTER(l_pcColList, return true);

	for (unsigned int i = 0; i < l_pcColList->GetColCount(); i++)
	{
		if (l_pcColList->GetColLabel(i) == p_sName)
			return true;
	}

	return false;

}


void SP_DLG_ColFSPNSimulationResults::InitializeEmptyView(SP_DS_Metadata* p_pcView)
{
	CHECK_POINTER(p_pcView, return);

	//get the current nodeclass type
	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(wxT("Nodeclass"));

	CHECK_POINTER(l_pcAttribute, return);

	wxString l_sElementType = l_pcAttribute->GetValueString();

	SP_VectorStdString* l_pvCurrentInfo;

	SP_VectorString l_asColours;

	l_asColours.clear();

	if (l_sElementType.IsSameAs(wxT("Transition")))
	{
		l_pvCurrentInfo = &m_msColoredTransitionNames;

		//load element colours
		LoadNodeColours(SP_DS_DISCRETE_TRANS, l_asColours);
	}
	else
	{
		l_pvCurrentInfo = &m_msColoredPlaceNames;

		//load element colours
		LoadNodeColours(SP_DS_DISCRETE_PLACE, l_asColours);
	}

	l_pcAttribute = p_pcView->GetAttribute(wxT("CurveInfo"));
	CHECK_POINTER(l_pcAttribute, return);
	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttribute);

	//position of the element in the result matrix
	for (unsigned int l_nPos = 0; l_nPos < l_pvCurrentInfo->size(); l_nPos++)
	{
		wxString l_sName = (*l_pvCurrentInfo)[l_nPos];

		unsigned int l_nNewRow = l_pcCurveInfoList->AppendEmptyRow();

		l_pcCurveInfoList->SetCell(l_nNewRow, 0, wxString::Format(wxT("%d"), l_nPos));
		l_pcCurveInfoList->SetCell(l_nNewRow, 1, wxT("Colored"));
		l_pcCurveInfoList->SetCell(l_nNewRow, 2, wxT("0"));

		if (l_nPos < l_asColours.size() && l_asColours[l_nPos] != wxT(""))
		{
			l_pcCurveInfoList->SetCell(l_nNewRow, 3, l_asColours[l_nPos]);
		}
		else
		{
			l_pcCurveInfoList->SetCell(l_nNewRow, 3, GetColourString(l_nPos));
		}

		l_pcCurveInfoList->SetCell(l_nNewRow, 4, wxT("-1"));
		l_pcCurveInfoList->SetCell(l_nNewRow, 5, wxT("-1"));
		l_pcCurveInfoList->SetCell(l_nNewRow, 6, l_sName);

	}
}


void SP_DLG_ColFSPNSimulationResults::LoadTransitions()
{

	m_pcMainSimulator->ClearTransitions();
	size_t l_nCount =  m_pcUnfoldedNet->GetTransitionNames()->size();
	 
	m_pcMainSimulator->SetTransitionCount(l_nCount);

	SP_VectorStdString* l_pcTransitionName = m_pcUnfoldedNet->GetTransitionNames();
	SP_VectorString* l_pcTransitionFunction = m_pcUnfoldedNet->GetCurRateFunction();
	SP_VectorString* l_pcTransitionType = m_pcUnfoldedNet->GetTransNodeType();

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
	for (unsigned long l_nTransPos = 0; l_nTransPos < l_pcTransitionName->size(); l_nTransPos++)
	{

		m_pcMainSimulator->AddTransition((*l_pcTransitionName)[l_nTransPos],
			(*l_pcTransitionFunction)[l_nTransPos],
			GetTransitionType((*l_pcTransitionType)[l_nTransPos]));
	}
}


void SP_DLG_ColFSPNSimulationResults::LoadConnections()
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

		wxString l_sArcWeight = l_itConnection->m_sMultiplicity;

		//l_itConnection->m_sMultiplicity.ToDouble(&l_nWeight);

		spsim::ConnectionType l_nArcType = GetConnectionType(l_itConnection->m_sArcClass);

		if (IsConstantArcWeight(l_sArcWeight, l_nNumericArcWeight))
		{
			m_pcMainSimulator->SetPreTransitionConnection(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID, l_nArcType, l_nNumericArcWeight);
		}
		else
		{
			m_pcMainSimulator->SetPreSelfModifyingWeights(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID, l_nArcType, l_sArcWeight);
		}

		SP_LOGDEBUG(l_itConnection->m_sMultiplicity);
		SP_LOGDEBUG(l_sArcWeight);

		//m_pcMainSimulator->SetPreTransitionConnection(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID, GetConnectionType(l_itConnection->m_sArcClass), l_nWeight);
	}

	//Load transitions' post place
	l_pcPostConnection = m_pcUnfoldedNet->GetArcs_Transition2Place();
	for (l_itConnection = l_pcPostConnection->begin(); l_itConnection != l_pcPostConnection->end(); l_itConnection++)
	{
		double l_nNumericArcWeight = 0;

		wxString l_sArcWeight = l_itConnection->m_sMultiplicity;

		//l_itConnection->m_sMultiplicity.ToDouble(&l_nWeight);

		if (IsConstantArcWeight(l_sArcWeight, l_nNumericArcWeight))
		{
			m_pcMainSimulator->SetPostTransitionConnection(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID, l_nNumericArcWeight);
		}
		else
		{
			m_pcMainSimulator->SetPostSelfModifyingWeights(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID, l_sArcWeight);
		}

		//m_pcMainSimulator->SetPostTransitionConnection(l_itConnection->m_nTranPos, l_itConnection->m_nPlaceID, l_nWeight);
	}
}

void SP_DLG_ColFSPNSimulationResults::LoadPlaces()
{

	m_pcMainSimulator->SetPlaceCount(m_pcUnfoldedNet->GetPlaceNames()->size());

	m_pcMainSimulator->SetPlaceNames(*m_pcUnfoldedNet->GetPlaceNames());

	SP_VectorBool l_abIsFixed(m_pcUnfoldedNet->GetPlaceNames()->size(), false);
	unsigned long l_nPosition = 0;
	for (auto const& it : m_pcUnfoldedNet->GetUnfoldedDiscPlaces())
	{
		for (auto const& it2 : it.second)
		{
			l_abIsFixed[l_nPosition] = it2.second.m_bFixed;
			++l_nPosition;
		}
	}

	//set fixed flag
	(dynamic_cast<spsim::StochasticSimulator*>(m_pcMainSimulator))->SetFixedFlag(l_abIsFixed);

	(dynamic_cast<spsim::StochasticSimulator*>(m_pcMainSimulator))->SetInitialMarking(*m_pcUnfoldedNet->GetCurColStMarking());

}


void SP_DLG_ColFSPNSimulationResults::UpdateChoices()//by george for constant harmonizing
{
	m_choicesForColPNs.clear();
	//l_pcGraph->GetMetadataclass();
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
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


spsim::ConnectionType SP_DLG_ColFSPNSimulationResults::GetConnectionType(const wxString& p_sConnectionType)
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
		SP_LOGERROR(wxT("Unknown connection type,") + p_sConnectionType);
		return spsim::OTHER_CONNECTION;
	}
}


spsim::TransitionType SP_DLG_ColFSPNSimulationResults::GetTransitionType(const wxString& p_sTransitionType)
{
	if (p_sTransitionType == SP_DS_CONTINUOUS_TRANS)
	{
		return spsim::TRANSITION_TYPE_CONTINUOUS;
	}
	else
		if (p_sTransitionType == SP_DS_STOCHASTIC_TRANS)
		{
			return spsim::TRANSITION_TYPE_STOCHASTIC;
		}
		else
			if (p_sTransitionType == SP_DS_DETERMINISTIC_TRANS)
			{
				return spsim::TRANSITION_TYPE_DETERMINISTIC;
			}
			else
				if (p_sTransitionType == SP_DS_IMMEDIATE_TRANS)
				{
					return spsim::TRANSITION_TYPE_IMMEDIATE;
				}
				else
					if (p_sTransitionType == SP_DS_SCHEDULED_TRANS)
					{
						return spsim::TRANSITION_TYPE_SCHEDULED;
					}
					else
					{
						return spsim::TRANSITION_TYPE_UNKNOWN;
					}
}


bool SP_DLG_ColFSPNSimulationResults::LoadViewerData(SP_DS_ResultViewer* p_pcViewer, SP_DS_Metadata* p_pcView, wxArrayString& p_asPlaces)
{
	wxString l_sName;
	long l_nPos = 0;

	CHECK_POINTER(p_pcView, return false);

	//get the current nodeclass type
	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(wxT("Nodeclass"));
	CHECK_POINTER(l_pcAttribute, return false);
	wxString l_sElementType = l_pcAttribute->GetValueString();

	l_nPos = 0;

	CHECK_POINTER(p_pcViewer, return false);

	//remove old curves
	p_pcViewer->ClearCurves();

	//remove shown curves
	p_pcViewer->ClearShownCurves();

	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(p_pcView->GetAttribute(wxT("CurveInfo")));
	CHECK_POINTER(l_pcCurveInfoList, return false);

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

	for (unsigned int l_nRow = 0; l_nRow < l_pcCurveInfoList->GetRowCount(); l_nRow++)
	{
		wxString l_sPosition = l_pcCurveInfoList->GetCell(l_nRow, 0);
		unsigned long l_nPosition = 0;
		if (!l_sPosition.ToULong(&l_nPosition))
		{
			return false;
		}

		wxString l_sOutType = l_pcCurveInfoList->GetCell(l_nRow, 1);

		if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Place")) && l_nPosition < m_msPlaceNames.size()) //unfolded place
		{
			l_sName = m_msPlaceNames[l_nPosition];
		 
			wxString netClass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName();
			if (netClass.Contains(wxT("Fuzzy"))) {

				p_pcViewer->AddFuzzyCurves(l_sName, l_nPosition, &(*m_pcCompressedBand));


			}
			else {
				p_pcViewer->AddFuzzyCurves(l_sName, l_nPosition, &(*m_pcCompressedBand));
				//p_pcViewer->AddCurve(l_sName, l_nPosition, &m_anResultMatrix);
			}
		 
		}
		else
			if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Transition")) && l_nPosition < m_msTransitionNames.size()) //unfolded transition
			{
				l_sName = m_msTransitionNames[l_nPosition];

				p_pcViewer->AddCurve(l_sName, l_nPosition, &m_anResultMatrix);

				 
			}
			else
				if (l_sOutType == wxT("Colored") && l_sElementType.IsSameAs(wxT("Place")) && l_nPosition < m_msColoredPlaceNames.size()) //colored  place
				{
					l_sName = m_msColoredPlaceNames[l_nPosition];

					p_pcViewer->AddCurve(l_sName, l_nPosition, &m_aanColPlaceResults);
				}
				else
					if (l_sOutType == wxT("Colored") && l_sElementType.IsSameAs(wxT("Transition")) && l_nPosition < m_msColoredTransitionNames.size()) //colored transition
					{
						l_sName = m_msColoredTransitionNames[l_nPosition];

						p_pcViewer->AddCurve(l_sName, l_nPosition, &m_aanColTransResults);
					}
					else
						if (l_sOutType == wxT("Auxiliary variables") && l_sElementType.IsSameAs(wxT("Place")) && l_nPosition < m_vAuxPLVars.size()) //Auxiliary variables place
						{
							l_sName = m_vAuxPLVars[l_nPosition];

							p_pcViewer->AddCurve(l_sName, l_nPosition, &m_aanAuxPLVarsResults);
						}
						else
							if (l_sOutType == wxT("Auxiliary variables") && l_sElementType.IsSameAs(wxT("Transition")) && l_nPosition < m_vAuxTRVars.size()) //Auxiliary variables transition
							{
								l_sName = m_vAuxTRVars[l_nPosition];

								p_pcViewer->AddCurve(l_sName, l_nPosition, &m_aanAuxTRVarsResults);
							}
							else
							{
								SP_LOGERROR(wxT("Invalid node names, we stop loading the rest of the file"));

								//invalid row index, therefore we ignore the remaining rows
								l_pcCurveInfoList->RemoveRemainingRows(l_nRow);

								break;
							}

		wxString l_sOrignialName = l_pcCurveInfoList->GetCell(l_nRow, 6);

		if (l_sOrignialName != l_sName)
		{
			SP_LOGWARNING(wxT("Name for position (") + wxString::Format(wxT("%d"), l_nRow) + wxT(") is changed to ") + l_sName);
		}

		//update curve name
		l_pcCurveInfoList->SetCell(l_nRow, 6, l_sName);

		p_asPlaces.Add(l_sName);
	}
	return true;
}


void SP_DLG_ColFSPNSimulationResults::LoadSets()
{
	if (m_apcColListAttr.empty())
		{
			m_apcColListAttr.resize(m_apcComboBoxes.size());

			auto itFind = m_mGroup2Position.find(wxT("Marking"));

			SP_DS_Node* l_pcNode = NULL;
			if (m_pcGraph->GetNodeclass(wxT("Place"))->GetElements()->size() > 0 && itFind != m_mGroup2Position.end())
			{
				l_pcNode = m_pcGraph->GetNodeclass(wxT("Place"))->GetElements()->front();
			//	m_apcColListAttr[0] = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(SP_DS_CPN_MARKINGLIST));
				m_apcColListAttr[itFind->second] = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(SP_DS_CPN_MARKINGLIST));
			}

			itFind = m_mGroup2Position.find(wxT("Rate"));

			if (m_pcGraph->GetNodeclass(wxT("Transition"))->GetElements()->size() > 0 && itFind != m_mGroup2Position.end())
			{
				l_pcNode = m_pcGraph->GetNodeclass(wxT("Transition"))->GetElements()->front();
				//m_apcColListAttr[1] = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));
				m_apcColListAttr[itFind->second] = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));
			}

			if (m_pcGraph->GetNodeclass(wxT("Immediate Transition"))->GetElements()->size() > 0 && itFind != m_mGroup2Position.end())
			{
				l_pcNode = m_pcGraph->GetNodeclass(wxT("Immediate Transition"))->GetElements()->front();
				//m_apcColListAttr[2] = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));
				m_apcColListAttr[itFind->second] = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));
			}

			if (m_pcGraph->GetNodeclass(wxT("Deterministic Transition"))->GetElements()->size() > 0 && itFind != m_mGroup2Position.end())
			{
				l_pcNode = m_pcGraph->GetNodeclass(wxT("Deterministic Transition"))->GetElements()->front();
			//	m_apcColListAttr[3] = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("DelayList")));
				m_apcColListAttr[itFind->second] = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("DelayList")));
			}

			if (m_pcGraph->GetNodeclass(wxT("Scheduled Transition"))->GetElements()->size() > 0 && itFind != m_mGroup2Position.end())
			{
				l_pcNode = m_pcGraph->GetNodeclass(wxT("Scheduled Transition"))->GetElements()->front();
				//m_apcColListAttr[4] = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("PeriodicList")));
				m_apcColListAttr[itFind->second] = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("PeriodicList")));
			}
			/*********************************************/
			SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

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
					if (l_sChoice == l_sGroup )
					{
						itFind = m_mGroup2Position.find(l_sGroup);
						if (itFind != m_mGroup2Position.end())
						{
							m_apcColListAttr[itFind->second] = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));

							SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));
							m_mGroup2Selction[l_sGroup] = l_pcColList->GetActiveColumn();
							break;
						}
					}
				}
			}
			/*********************************************/
		}

		//assign the active sets for each group
		for (auto itMap = m_mGroup2Position.begin(); itMap != m_mGroup2Position.end(); ++itMap)
		{
			SP_DS_ColListAttribute* l_pcAttr = m_apcColListAttr[itMap->second];

			if (!l_pcAttr) continue;

			wxChoice* l_pcCombobox = m_apcComboBoxes[itMap->second];

			if (!l_pcCombobox) continue;

			l_pcCombobox->Clear();

			//add value sets of marking group
			if (itMap->first == wxT("Marking"))
			{
				for (unsigned int i = 0; i < l_pcAttr->GetColCount(); i++)
				{
					wxString l_sSetName = l_pcAttr->GetColLabel(i);
					if (l_sSetName.Contains(wxT(":")))//by george
					{
						l_sSetName = l_sSetName.BeforeFirst(wxT(':'));
						if (l_pcCombobox)
							l_pcCombobox->Append(l_sSetName);
					}
					i = i + 2;

				}
			}

			//add value sets of stoch rates
			else if (itMap->first == wxT("Rate") || itMap->first == wxT("Weight"))
			{
				for (unsigned int i = 1; i < l_pcAttr->GetColCount(); i++)
				{
					wxString l_sSetName = l_pcAttr->GetColLabel(i);
					l_sSetName = l_sSetName.BeforeFirst(wxT(':'));

					if (l_pcCombobox)
						l_pcCombobox->Append(l_sSetName);
				}
			}

			else if (itMap->first == wxT("Delay"))
			{
				for (unsigned i = 1; i < l_pcAttr->GetColCount(); i++)
				{
					wxString l_sSetName = l_pcAttr->GetColLabel(i);
					l_sSetName = l_sSetName.BeforeFirst(wxT(':'));

					if (l_pcCombobox)
						l_pcCombobox->Append(l_sSetName);

					i++;
					i++;
				}
			}

			else if (itMap->first == wxT("Schedule"))
			{
				for (unsigned int i = 0; i < l_pcAttr->GetRowCount(); i++)
				{
					wxString l_sSetName = l_pcAttr->GetCell(i, 0);

					if (l_pcCombobox)
						l_pcCombobox->Append(l_sSetName);
				}
				if (l_pcCombobox)
					l_pcCombobox->SetSelection(l_pcAttr->GetActiveList());
			}
			else
			{
				//user defined groups
				for (unsigned int i = 0; i < l_pcAttr->GetRowCount(); i++)
				{
					wxString l_sSetName = l_pcAttr->GetCell(i, 0);
					l_pcCombobox->Append(l_sSetName);
				}
				l_pcCombobox->SetSelection(l_pcAttr->GetActiveList());
			}

			//set the active sets
			if (l_pcCombobox->GetCount() <= l_pcAttr->GetActiveColumn())
			{
				l_pcCombobox->SetSelection(0);
				l_pcAttr->SetActiveColumn(0);
			}
			else
				l_pcCombobox->SetSelection(l_pcAttr->GetActiveColumn());
		}

}



void SP_DLG_ColFSPNSimulationResults::OnModifyConstants(wxCommandEvent& p_cEvent)
{
	SP_DLG_FpnConstantDefinition* l_pcConstantsDialog = new SP_DLG_FpnConstantDefinition(NULL);
	//SP_DLG_ConstantDefinition* l_pcConstantsDialog = new SP_DLG_ConstantDefinition(NULL);
	if (l_pcConstantsDialog->ShowModal() == wxID_OK)
	{
		UpdateChoices();
		if (m_choicesForColPNs.size() != m_nGroupCounts)
		{//special case, when a user add a new group during simulation
			m_iModifyCount = 0;
			m_apcColListAttr.clear();

			m_nGroupCounts = m_choicesForColPNs.size();//update groups size 
			LoadSets();
		}
		else {
			m_iModifyCount++;

			LoadSets();
		}

	}

}


void SP_DLG_ColFSPNSimulationResults::LoadSimulatorData()
{
	for (size_t i = 0; i < m_apcColListAttr.size(); i++)
	{
		if (m_apcColListAttr[i])
		{
			if (m_apcComboBoxes.size()>i)//
				m_apcColListAttr[i]->SetActiveList(m_apcComboBoxes[i]->GetSelection());
			else//
			{
				m_apcColListAttr[i]->SetActiveList(m_apcComboBoxes[i - m_nGroupCounts]->GetSelection());
			}
			if (i <= 4)
			{
				m_apcColListAttr[i]->SetActiveColumn(m_apcComboBoxes[i]->GetSelection());
			}
		}
	}

}

void SP_DLG_ColFSPNSimulationResults::OnModifyMarkingSets(wxCommandEvent& p_cEvent)
{

	SP_DLG_ColStMarkingOverview* l_pcDlg = new SP_DLG_ColStMarkingOverview(this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

		if (!LoadUnfoldedPlaces())
		{
			SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
			return;
		}

	}

}

void SP_DLG_ColFSPNSimulationResults::OnModifyFunctionSets(wxCommandEvent& p_cEvent)
{

	SP_DLG_ColStFunctionOverview* l_pcDlg = new SP_DLG_ColStFunctionOverview(wxT("Transition"), this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

		if (LoadUnfoldedPlaces())
		{
			if (LoadUnfoldingData())
			{
				return;
			}
		}
		SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
	}
}

void SP_DLG_ColFSPNSimulationResults::OnModifyDelaySets(wxCommandEvent& p_cEvent)
{

	SP_DLG_ColListOverview* l_pcDlg = new SP_DLG_ColListOverview(wxT("DelayList"), this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

		if (LoadUnfoldedPlaces())
		{
			if (LoadUnfoldingData())
			{
				return;
			}
		}
		SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
	}
}
void SP_DLG_ColFSPNSimulationResults::OnModifyScheduleSets(wxCommandEvent& p_cEvent)
{

	SP_DLG_ColListOverview* l_pcDlg = new SP_DLG_ColListOverview(wxT("PeriodicList"), this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

		if (LoadUnfoldedPlaces())
		{
			if (LoadUnfoldingData())
			{
				return;
			}
		}
		SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
	}
}

void SP_DLG_ColFSPNSimulationResults::OnModifyWeightSets(wxCommandEvent& p_cEvent)
{

	SP_DLG_ColStFunctionOverview* l_pcDlg = new SP_DLG_ColStFunctionOverview(wxT("Immediate Transition"), NULL);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

		if (LoadUnfoldedPlaces())
		{
			if (LoadUnfoldingData())
			{
				return;
			}
		}
		SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
	}
}
void SP_DLG_ColFSPNSimulationResults::OnModifyParameterSets(wxCommandEvent& p_cEvent)
{
	//SP_DLG_StParameterOverview* l_pcDlg = new SP_DLG_StParameterOverview(this);
	SP_DLG_ConstantDefinition * l_pcConstantDialog = new SP_DLG_ConstantDefinition(this);

	if (l_pcConstantDialog->ShowModal() == wxID_OK)
	{
		LoadSets();
	}


	//if (l_pcDlg->ShowModal() == wxID_OK)
	//	{
	//		LoadSets();
	//	}
}

void SP_DLG_ColFSPNSimulationResults::LoadResults()
{
	//Load colored or auxiliary variables
	LoadColAuxResults();

	UpdateViewer();

	RefreshExternalWindows();
}


void SP_DLG_ColFSPNSimulationResults::OnLoadApFormulae(wxCommandEvent& p_cEvent)
{
	wxString l_sFilename = wxFileSelector(wxT("Choose a file to open"));
	if (!l_sFilename.empty())
	{
		wxTextFile* l_pcFile = new wxTextFile(l_sFilename);
		if (l_pcFile->Open())
		{
			m_sApFormulae = l_pcFile->GetFirstLine();
			while (!l_pcFile->Eof())
			{
				m_sApFormulae += l_pcFile->GetNextLine();
			}
			if (!m_sApFormulae.IsEmpty())
			{
				SP_DS_ApParser l_ApParser(m_pcGraph);
				if (m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS)
				{
					l_ApParser.SetUnfoldedNet(m_pcUnfoldedNet);
				}
				l_ApParser.Initialise();
				l_ApParser.Parse(m_sApFormulae);
			}
			l_pcFile->Close();
		}
		wxDELETE(l_pcFile);
	}
}

void SP_DLG_ColFSPNSimulationResults::OnEnterApFormulae(wxCommandEvent& p_cEvent)
{
	m_sApFormulae = wxGetTextFromUser(wxT("Please enter at least one ap formula."), wxT("AP Formulae"), m_sApFormulae);
	if (!m_sApFormulae.IsEmpty())
	{
		SP_DS_ApParser l_ApParser(m_pcGraph);
		if (m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS)
		{
			l_ApParser.SetUnfoldedNet(m_pcUnfoldedNet);
		}
		l_ApParser.Initialise();
		l_ApParser.Parse(m_sApFormulae);
	}
}

void SP_DLG_ColFSPNSimulationResults::CheckApFormulae(wxCommandEvent& p_cEvent)
{
	//clear
	m_aApFormulaHappend.clear();
	m_aApFormulas.clear();
	m_aApResults.clear();
	m_sApFormulas.clear();

	//initialize it
	m_pcApParser = new SP_DS_ApParser(m_pcGraph);

	if (m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS)
	{
		m_pcApParser->SetUnfoldedNet(m_pcUnfoldedNet);
	}

	m_pcApParser->Initialise();

	wxString l_sApFormulae = m_sApFormulae;

	if (!l_sApFormulae.IsEmpty())
	{
		if (m_pcApParser->Parse(l_sApFormulae))
		{
			rfdag::vectorRFDAG l_Expressions = m_pcApParser->GetExpressions();
			for (size_t i = 0; i < l_Expressions.size(); ++i)
			{
				if (m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS)
				{
					m_aApFormulas.push_back(SP_DS_ApFormula(m_pcUnfoldedNet, m_pcGraph, l_Expressions[i]));
				}
				else
				{
					m_aApFormulas.push_back(SP_DS_ApFormula(m_pcGraph, l_Expressions[i]));
				}
				m_aApFormulaHappend.push_back(false);
				m_aApResults.push_back(0);
				m_sApFormulas.push_back(wxString(l_Expressions[i].toString().c_str(), wxConvUTF8));
			}
		}
	}

	m_aApFormulaHappend.assign(m_aApFormulas.size(), false);
	m_aApResults.assign(m_aApFormulas.size(), 0);

	//get the average value of multiple simulation runs
	const SP_Vector2DDouble& l_aanResults = m_pcMainSimulator->GetResultMatrix();
	if (l_aanResults.size() == 0)
		return;

	SP_MapLong2Double l_mnResult;
	for (unsigned long l_nRow = 0; l_nRow < m_nResultPointCount; l_nRow++)
	{
		SP_VectorDouble m_anCurrentMarking;
		for (unsigned long l_nCol = 0; l_nCol < m_pcMainSimulator->GetPlaceCount(); l_nCol++)
		{
			m_anCurrentMarking.push_back(l_aanResults[l_nRow][l_nCol]);
		}
		for (size_t i = 0; i < m_aApFormulas.size(); ++i)
		{
			if (!m_aApFormulaHappend[i])
			{
				double l_nResult = m_aApFormulas[i].PostEvaluate(&m_anCurrentMarking);
				if (l_nResult > 0)
				{
					m_aApFormulaHappend[i] = true;
					m_aApResults[i] += l_nResult;
				}
			}
		}
	}

	//write results
	for (size_t i = 0; i < m_aApFormulas.size(); ++i)
	{
		double l_nValue = m_aApResults[i];

		wxString l_sOutput = wxT("Result of formula ");
		l_sOutput << wxString(m_aApFormulas[i].GetExpressionTree().toString().c_str(), wxConvUTF8);
		SP_LOGMESSAGE(l_sOutput);

		l_sOutput = wxT("Prop: ");
		l_sOutput << l_nValue;
		SP_LOGMESSAGE(l_sOutput);
	}

}


void SP_DLG_ColFSPNSimulationResults::LoadColAuxResults()
{
	GetColPlaceResults(m_pcMainSimulator->GetResultMatrix(), m_aanColPlaceResults);
	GetColTransResults(m_pcMainSimulator->GetRateResultMatrix(), m_aanColTransResults);

	double l_nRunCount = (double)(m_pcWorkerThread->GetCurrentRunCount());

	//we need to account for the current run
	if (m_pcMainSimulator->IsSimulationRunning())
	{
		l_nRunCount += 1;
	}

	unsigned int l_nColCount = 0;
	l_nColCount = m_msColoredPlaceNames.size();

	if (m_pcWorkerThread->GetRunCount()>1)
	{
		//get the average values
		for (unsigned int l_nRow = 0; l_nRow<m_aanColPlaceResults.size(); l_nRow++)
			for (unsigned int l_nCol = 0; l_nCol<l_nColCount; l_nCol++)
			{
				m_aanColPlaceResults[l_nRow][l_nCol] /= l_nRunCount;
			}
	}

	l_nColCount = m_msColoredTransitionNames.size();
	if (m_pcWorkerThread->GetRunCount()>1)
	{
		//get the average values
		for (unsigned int l_nRow = 0; l_nRow<m_aanColTransResults.size(); l_nRow++)
			for (unsigned int l_nCol = 0; l_nCol<l_nColCount; l_nCol++)
			{
				m_aanColTransResults[l_nRow][l_nCol] /= l_nRunCount;
			}
	}

	if (!m_bComAuxVarSingleRun)
	{
		ComputeAuxiliaryVars();
	}
}

void SP_DLG_ColFSPNSimulationResults::UpdateXAxisValues()
{
	CHECK_POINTER(m_pcCurrentTablePlot, return);

	CalculateXAxisValues(m_pcCurrentTablePlot, m_anXAxisValues);
}


void SP_DLG_ColFSPNSimulationResults::CalculateXAxisValues(SP_DS_Metadata* p_pcView, SP_VectorDouble& p_anXAxisValues)
{
	CHECK_POINTER(m_pcMainSimulator, return);

	wxString l_sCategory = GetViewAttributeValue(p_pcView, wxT("XAxisVariable"));
	wxString l_sSubCategory = GetViewAttributeValue(p_pcView, wxT("XAxisVariableName"));

	double l_nStartOutputPoint = m_pcMainSimulator->GetOutputStartPoint();

	p_anXAxisValues.clear();
	if (l_sCategory.IsSameAs(wxT("Time")))
	{
		double l_nTime = 0;

		for (unsigned long l_nPoint = 0; l_nPoint <= m_pcMainSimulator->GetOutputPointsCount(); l_nPoint++)
		{
			p_anXAxisValues.push_back(l_nStartOutputPoint + l_nTime);

			l_nTime += m_pcMainSimulator->GetOutputSampleSize();
		}

	}
	else
		if (l_sCategory.IsSameAs(wxT("Place-colored")) && !l_sSubCategory.IsEmpty())
		{
			unsigned long l_nPlacePosition = m_mPlaceName2Position[l_sSubCategory];

			//number of rows in the colored matrix
			unsigned long l_nRowCount = m_aanColPlaceResults.size();

			for (unsigned long l_nPoint = 0; l_nPoint < l_nRowCount; l_nPoint++)
			{
				double l_nValue = m_aanColPlaceResults[l_nPoint][l_nPlacePosition];

				p_anXAxisValues.push_back(l_nValue);
			}

		}
		else
			if (l_sCategory.IsSameAs(wxT("Transition-colored")) && !l_sSubCategory.IsEmpty())
			{
				unsigned long l_nTransitionPosition = m_mTransitionName2Position[l_sSubCategory];

				//number of rows in the colored matrix
				unsigned long l_nRowCount = m_aanColTransResults.size();

				for (unsigned long l_nPoint = 0; l_nPoint < l_nRowCount; l_nPoint++)
				{
					double l_nValue = m_aanColTransResults[l_nPoint][l_nTransitionPosition];

					p_anXAxisValues.push_back(l_nValue);
				}

			}
			else
				if (l_sCategory.IsSameAs(wxT("Place-uncolored")) && !l_sSubCategory.IsEmpty())
				{
					unsigned long l_nPlacePosition = m_mnPlaceArrayPos_FromUnfolding[l_sSubCategory];

					//number of rows in the result matrix
					unsigned long l_nRowCount = m_pcMainSimulator->GetResultMatrix().size();

					for (unsigned long l_nPoint = 0; l_nPoint < l_nRowCount; l_nPoint++)
					{
						double l_nValue = m_pcMainSimulator->GetResultMatrix()[l_nPoint][l_nPlacePosition];
						;

						p_anXAxisValues.push_back(l_nValue);
					}

				}
				else
					if (l_sCategory.IsSameAs(wxT("Transition-uncolored")) && !l_sSubCategory.IsEmpty())
					{
						unsigned long l_nTransitionPosition = m_mnTransitionArrayPos[l_sSubCategory];

						//number of rows in the result matrix
						unsigned long l_nRowCount = m_pcMainSimulator->GetRateResultMatrix().size();

						for (unsigned long l_nPoint = 0; l_nPoint < l_nRowCount; l_nPoint++)
						{
							double l_nValue = m_pcMainSimulator->GetRateResultMatrix()[l_nPoint][l_nTransitionPosition];

							p_anXAxisValues.push_back(l_nValue);
						}

					}
					else
					{
						double l_nTime = 0;

						for (unsigned long l_nPoint = 0; l_nPoint <= m_pcMainSimulator->GetOutputPointsCount(); l_nPoint++)
						{
							p_anXAxisValues.push_back(l_nStartOutputPoint + l_nTime);

							l_nTime += m_pcMainSimulator->GetOutputSampleSize();
						}

					}
}

bool SP_DLG_ColFSPNSimulationResults::IsConstantArcWeight(const wxString& p_sWeight, double& p_nReturnValue)
{
	SP_MapString2Double::iterator l_itConstant;

	//explicit constant value
	if (p_sWeight.ToDouble(&p_nReturnValue))
	{
		return true; //constant
	}

	//check constant list
	/*if ((l_itConstant = m_msParameterName2Value.find(p_sWeight)) != m_msParameterName2Value.end())
	{
	p_nReturnValue = (*l_itConstant).second;

	return true; //constant
	}*/

	return false; //not a constant
}


void SP_DLG_ColFSPNSimulationResults::DirectSingleExportToCSV(long p_nSimulationRunNumber)
{
	const SP_Vector2DDouble& l_aanSingleResults = dynamic_cast<spsim::StochasticSimulator*>(m_pcMainSimulator)->GetSingleResultMatrix();
	const SP_Vector2DDouble& l_aanSingleTransitionFireCount = dynamic_cast<spsim::StochasticSimulator*>(m_pcMainSimulator)->GetSingleRateMatrix();

	//change here to compute auxiliary variables
	GetColPlaceResults(l_aanSingleResults, m_aanColPlaceResults);
	GetColTransResults(l_aanSingleTransitionFireCount, m_aanColTransResults);

	//m_bComAuxVarSingleRun = true;

	DirectSingleExactExportToCSV(p_nSimulationRunNumber, -1, l_aanSingleResults[0], l_aanSingleTransitionFireCount[0], 0);

	for (unsigned long i = 0; i < m_nResultPointCount; i++)
	{
		DirectSingleExactExportToCSV(p_nSimulationRunNumber, m_pcMainSimulator->GetOutputSampleSize() * i, l_aanSingleResults[i], l_aanSingleTransitionFireCount[i], i);
	}

}

void SP_DLG_ColFSPNSimulationResults::DirectSingleExactExportToCSV(long p_nSimulationRunNumber, double p_nTimepoint, const SP_VectorDouble& p_anMarking, const SP_VectorDouble& p_anHazardValues,
	long p_nCurrentTimePoint)
{
	if (!m_pcExport)
		return;

	wxString l_sValue;
	wxString l_sName;
	wxString l_sOutput = wxT("");
	wxString l_sSpacer = GetSpacer(m_nExportSpacer);

	if (p_nTimepoint < 0)
	{
		m_sLastResultExport.Clear();

		l_sOutput << wxT("--- run: ") << p_nSimulationRunNumber << wxT(" ---\n");
		*m_pcExport << l_sOutput;
	}
	else
	{
		SP_DS_Attribute* l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("Nodeclass"));
		CHECK_POINTER(l_pcAttribute, return);
		wxString l_sElementType = l_pcAttribute->GetValueString();

		SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo")));
		CHECK_POINTER(l_pcCurveInfoList, return);

		l_sOutput = wxT("");
		l_sOutput << p_nTimepoint;
		wxString l_sCurrentRow = wxT("");

		for (unsigned int l_n = 0; l_n < l_pcCurveInfoList->GetRowCount(); l_n++)
		{
			wxString l_sPos = l_pcCurveInfoList->GetCell(l_n, 0);
			long l_nPos;
			if (!l_sPos.ToLong(&l_nPos))
				return;
			wxString l_sOutType = l_pcCurveInfoList->GetCell(l_n, 1);
			wxString l_sCheckState = l_pcCurveInfoList->GetCell(l_n, 2);
			if (l_sCheckState != wxT("1"))
				continue;
			wxString l_sName = l_pcCurveInfoList->GetCell(l_n, 6);

			double l_dResult = 0;
			if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Place"))) //unfolded place
			{
				l_dResult = p_anMarking[l_nPos];
			}
			else
				if (l_sOutType == wxT("Colored") && l_sElementType.IsSameAs(wxT("Place"))) //colored  place
				{
					l_dResult = m_aanColPlaceResults[p_nCurrentTimePoint][l_nPos];
				}
				else
					if (l_sOutType == wxT("Auxiliary variables") && l_sElementType.IsSameAs(wxT("Place"))) //unfolded place
					{
						l_dResult = m_aanAuxPLVarsResults[p_nCurrentTimePoint][l_nPos];
					}
					else
						if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Transition"))) //unfolded place
						{
							l_dResult = p_anHazardValues[l_nPos];
						}
						else
							if (l_sOutType == wxT("Colored") && l_sElementType.IsSameAs(wxT("Transition"))) //colored  place
							{
								l_dResult = m_aanColTransResults[p_nCurrentTimePoint][l_nPos];
							}
							else
								if (l_sOutType == wxT("Auxiliary variables") && l_sElementType.IsSameAs(wxT("Transition"))) //unfolded place
								{
									l_dResult = m_aanAuxTRVarsResults[p_nCurrentTimePoint][l_nPos];
								}
								else
								{
									l_dResult = 0;
								}

			//write
			l_sCurrentRow << l_sSpacer;
			l_sCurrentRow << l_dResult;
		}

		if (!l_sCurrentRow.IsSameAs(m_sLastResultExport) || !m_bCompressExact)
		{
			l_sOutput << l_sCurrentRow;
			*m_pcExport << l_sOutput << wxT("\n");
		}
		m_sLastResultExport = l_sCurrentRow;
	}

}

bool SP_DLG_ColFSPNSimulationResults::ComputeAuxiliaryVars()
{
	m_aanAuxPLVarsResults.clear();
	m_vAuxPLVars.clear();
	SP_DS_ColTraceAnalyzer l_cTraceAnalyzer(m_pcUnfoldedNet, m_anResultMatrix, m_aanAuxPLVarsResults, m_vAuxPLVars);

	if (!l_cTraceAnalyzer.Compute())
		return false;

	return true;
}

//compute the auxiliary variables for each simulation run, and also compute the average values of all these simulation runs
void SP_DLG_ColFSPNSimulationResults::ComputeAverageAuxiliaryVars(long p_nSimulationRunNumber, long p_nSimulationRunCount)
{
	const SP_Vector2DDouble& l_aanSingleResults = dynamic_cast<spsim::StochasticSimulator*>(m_pcMainSimulator)->GetSingleResultMatrix();

	m_vAuxPLVars.clear();

	SP_Vector2DDouble l_aanSingleAuxPLVarsResults;

	SP_DS_ColTraceAnalyzer l_cTraceAnalyzer(m_pcUnfoldedNet, l_aanSingleResults, l_aanSingleAuxPLVarsResults, m_vAuxPLVars);
	if (!l_cTraceAnalyzer.Compute())
		return;

	long l_nRPCount = l_aanSingleResults.size();
	long l_nPlaceCount = m_vAuxPLVars.size();

	if (p_nSimulationRunNumber == 0)
	{
		m_aanTempAverageAuxPLVarsResults.clear(); //for the first time, clear it.
		m_aanTempAverageAuxPLVarsResults.assign(l_nRPCount, SP_VectorDouble(l_nPlaceCount, 0));
	}


	for (long l_nRow = 0; l_nRow < l_nRPCount; l_nRow++)
	{
		for (long l_nCol = 0; l_nCol < l_nPlaceCount; l_nCol++)
		{
			m_aanTempAverageAuxPLVarsResults[l_nRow][l_nCol] = m_aanTempAverageAuxPLVarsResults[l_nRow][l_nCol] + l_aanSingleAuxPLVarsResults[l_nRow][l_nCol];
		}
	}

	if (p_nSimulationRunNumber == p_nSimulationRunCount - 1)
	{
		m_aanAuxPLVarsResults.clear(); //for the first time, clear it.
		m_aanAuxPLVarsResults.assign(l_nRPCount, SP_VectorDouble(l_nPlaceCount, 0));

		for (long l_nRow = 0; l_nRow < l_nRPCount; l_nRow++)
		{
			for (long l_nCol = 0; l_nCol < l_nPlaceCount; l_nCol++)
			{
				m_aanAuxPLVarsResults[l_nRow][l_nCol] = m_aanTempAverageAuxPLVarsResults[l_nRow][l_nCol] / p_nSimulationRunCount;
			}
		}

		m_aanTempAverageAuxPLVarsResults.clear();
	}

}
//compute the auxiliary variables for each simulation run, the results will be used only by the export the result.
void SP_DLG_ColFSPNSimulationResults::ComputeEachSimulationAuxiliaryVars()
{
	const SP_Vector2DDouble& l_aanSingleResults = dynamic_cast<spsim::StochasticSimulator*>(m_pcMainSimulator)->GetSingleResultMatrix();
	//const SP_Vector2DDouble& l_aanSingleTransitionFireCount = m_pcSimulator->GetSingleResultFireCount();

	m_vAuxPLVars.clear();

	SP_Vector2DDouble l_aanSingleAuxPLVarsResults;

	SP_DS_ColTraceAnalyzer l_cTraceAnalyzer(m_pcUnfoldedNet, l_aanSingleResults, l_aanSingleAuxPLVarsResults, m_vAuxPLVars);
	if (!l_cTraceAnalyzer.Compute())
		return;

	long l_nRPCount = l_aanSingleResults.size();
	long l_nPlaceCount = m_vAuxPLVars.size();

	m_aanAuxPLVarsResults.clear();
	m_aanAuxPLVarsResults.assign(l_nRPCount, SP_VectorDouble(l_nPlaceCount, 0));

	for (long l_nRow = 0; l_nRow < l_nRPCount; l_nRow++)
	{
		for (long l_nCol = 0; l_nCol < l_nPlaceCount; l_nCol++)
		{
			m_aanAuxPLVarsResults[l_nRow][l_nCol] = l_aanSingleAuxPLVarsResults[l_nRow][l_nCol];
		}
	}

}
