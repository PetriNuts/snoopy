#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/valgen.h>
#include <wx/gbsizer.h>
#include <math.h>
#include <wx/ffile.h>
#include <wx/textfile.h>

#include "sp_core/base/SP_Error.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/dialogs/dia_ColCPN/SP_DLG_ColCPNDirSimulationResults.h"

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StMarkingOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StFunctionOverview.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StParameterOverview.h"
#include "sp_gui/dialogs/SP_DLG_SimulationProperties.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_core/SP_Core.h"

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"

#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStMarkingOverview.h"
#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStFunctionOverview.h"

#include "spsim/spsim.h"


#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColPlacesSelection.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_DColTraceAnalyzer.h"
#include <wx/regex.h>

IMPLEMENT_CLASS(SP_DLG_ColCPNDirSimulationResults, SP_DLG_CPNColDirSimResults)

enum
{
	SP_ID_BUTTON_MODIFY_MARKING_SETS = SP_ID_LAST_ID + 200,
	SP_ID_BUTTON_MODIFY_FUNCTION_SETS,
	SP_ID_BUTTON_MODIFY_PARAMETER_SETS,
	SP_ID_CHOICE_MARKING_SETS,
	SP_ID_CHOICE_FUNCTION_SETS,
	SP_ID_CHOICE_PARAMETER_SETS,
	SP_ID_CHOICE_SOLVER,
	SP_ID_RADIOBOX_SOLVER_TYPE,
	SP_ID_BUTTON_SIMULATION_PROPERTIES,
	SP_ID_BUTTON_SAVE_ODE
};
BEGIN_EVENT_TABLE(SP_DLG_ColCPNDirSimulationResults, SP_DLG_CPNColDirSimResults)

EVT_BUTTON(SP_ID_BUTTON_MODIFY_MARKING_SETS, SP_DLG_ColCPNDirSimulationResults::OnModifyMarkingSets)
EVT_BUTTON(SP_ID_BUTTON_MODIFY_FUNCTION_SETS, SP_DLG_ColCPNDirSimulationResults::OnModifyFunctionSets)
EVT_BUTTON(SP_ID_BUTTON_MODIFY_PARAMETER_SETS, SP_DLG_ColCPNDirSimulationResults::OnModifyParameterSets)

EVT_CHOICE(SP_ID_CHOICE_MARKING_SETS, SP_DLG_ColCPNDirSimulationResults::OnMarkingSetChanged)
EVT_CHOICE(SP_ID_CHOICE_FUNCTION_SETS, SP_DLG_ColCPNDirSimulationResults::OnFunctionSetChanged)
EVT_CHOICE(SP_ID_CHOICE_PARAMETER_SETS, SP_DLG_ColCPNDirSimulationResults::OnParameterSetChanged)

EVT_CHOICE(SP_ID_CHOICE_SOLVER, SP_DLG_ColCPNDirSimulationResults::OnSolverChanged)
EVT_BUTTON(SP_ID_BUTTON_SIMULATION_PROPERTIES, SP_DLG_ColCPNDirSimulationResults::OnSimulationProperties)
EVT_BUTTON(SP_ID_BUTTON_SAVE_ODE, SP_DLG_ColCPNDirSimulationResults::SaveODE)
END_EVENT_TABLE()


SP_DLG_ColCPNDirSimulationResults::SP_DLG_ColCPNDirSimulationResults(SP_DS_Graph* p_pcGraph, SP_DS_ColPN_Coloring* p_pcColordedNet,  wxWindow* p_pcParent, wxString p_sHelpText, wxString p_sTitle,
	long p_nStyle) :
	SP_DS_ColoredPNDirSimulation(p_pcColordedNet),
	SP_DLG_CPNColDirSimResults(p_pcGraph, p_pcParent)

{
	m_pcXAxisChoices.clear();
	m_pcXAxisChoices[wxT("Place-colored")] = &m_msColoredPlaceNames;
	m_pcXAxisChoices[wxT("Transition-colored")] = &m_msColoredTransitionNames;
//	m_pcXAxisChoices[wxT("Place-uncolored")] = &m_msUncoloredPlaceNames;

	
	wxSizer* l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Marking set:")), 1, wxALL | wxEXPAND, 5);

	m_pcMarkingSetComboBox = new wxChoice(m_pcPropertyWindowSetsSizer, SP_ID_CHOICE_MARKING_SETS, wxDefaultPosition, wxSize(100, -1));

	l_pcRowSizer->Add(m_pcMarkingSetComboBox, 1, wxALL, 5);

	l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_MARKING_SETS, wxT("Modify")), 0, wxALL, 5);

	m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Function set:")), 1, wxALL | wxEXPAND, 5);

	m_pcFunctionSetComboBox = new wxChoice(m_pcPropertyWindowSetsSizer, SP_ID_CHOICE_FUNCTION_SETS, wxDefaultPosition, wxSize(100, -1));

	l_pcRowSizer->Add(m_pcFunctionSetComboBox, 1, wxALL, 5);

	l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_FUNCTION_SETS, wxT("Modify")), 0, wxALL, 5);

	m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

	l_pcRowSizer->Add(new wxStaticText(m_pcPropertyWindowSetsSizer, -1, wxT("Parameter set:")), 1, wxALL | wxEXPAND, 5);

	m_pcParameterSetComboBox = new wxChoice(m_pcPropertyWindowSetsSizer, SP_ID_CHOICE_PARAMETER_SETS, wxDefaultPosition, wxSize(100, -1));

	l_pcRowSizer->Add(m_pcParameterSetComboBox, 1, wxALL, 5);

	l_pcRowSizer->Add(new wxButton(m_pcPropertyWindowSetsSizer, SP_ID_BUTTON_MODIFY_PARAMETER_SETS, wxT("Modify")), 0, wxALL, 5);

	m_pcSetsSizer->Add(l_pcRowSizer, 1, wxEXPAND);

	//At the end call this function for alignment
	SetSizerAndFit(m_pcMainSizer);

}

SP_DLG_ColCPNDirSimulationResults::~SP_DLG_ColCPNDirSimulationResults()
{
	wxDELETE(m_pcMarkingSetComboBox);
	wxDELETE(m_pcFunctionSetComboBox);
	wxDELETE(m_pcParameterSetComboBox);
	
}

bool SP_DLG_ColCPNDirSimulationResults::LoadViewerData(SP_DS_ResultViewer* p_pcViewer, SP_DS_Metadata* p_pcView, wxArrayString& p_asPlaces)
{
	wxString l_sName;
	long l_nPos = 0;
	//SP_LOGMESSAGE(wxT("colorcontinous load viewer data"));
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

	m_ArrayColPlaces.Clear();
	m_ArrayColTranstions.Clear();
	m_ArrayUncolPlaces.Clear();
	m_ArrayAuxPlaces.Clear();
	m_ArrayAuxtranstions.Clear();

	
	for (unsigned int i = 0; i < m_msColoredPlaceNames.size(); i++) {
		m_ArrayColPlaces.Add(m_msColoredPlaceNames[i]);
	}
	for (unsigned int i = 0; i < m_msColoredTransitionNames.size(); i++) {
		m_ArrayColTranstions.Add(m_msColoredTransitionNames[i]);
	}
	for (unsigned int i = 0; i < m_msUncoloredPlaceNames.size(); i++) {
		m_ArrayUncolPlaces.Add(m_msUncoloredPlaceNames[i]);
	}
	for (unsigned int i = 0; i < m_vAuxPLVars.size(); i++) {
		m_ArrayAuxPlaces.Add(m_vAuxPLVars[i]);
	}
	for (unsigned int i = 0; i < m_vAuxTRVars.size(); i++) {
		m_ArrayAuxtranstions.Add(m_vAuxTRVars[i]);
	}


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
		 
		         if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Place")) && l_nPosition < m_msUncoloredPlaceNames.size()) //unfolded place
		        {
			        l_sName = m_msUncoloredPlaceNames[l_nPosition];

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

void SP_DLG_ColCPNDirSimulationResults::OnMarkingSetChanged(wxCommandEvent& p_cEvent)
{
	//Get the Places Nodes
	unsigned int l_nCurrentMarkingSet = m_pcMarkingSetComboBox->GetSelection();
	const SP_ListNode* l_pcPlaces = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements();

	//Set the current Active column	
	for (SP_ListNode::const_iterator l_itPlace = l_pcPlaces->begin(); l_itPlace != l_pcPlaces->end(); l_itPlace++)
	{
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itPlace)->GetAttribute(wxT("MarkingList"))))->SetActiveColumn(l_nCurrentMarkingSet);
	}


	LoadPlaces();

}

void SP_DLG_ColCPNDirSimulationResults::OnParameterSetChanged(wxCommandEvent& p_cEvent)
{
	unsigned int l_nCurrentParameterSet = m_pcParameterSetComboBox->GetSelection();
	const SP_ListNode* l_pcParams = m_pcGraph->GetNodeclass(SP_DS_PARAM)->GetElements();

	//Set the current Active List
	for (SP_ListNode::const_iterator l_itParam = l_pcParams->begin(); l_itParam != l_pcParams->end(); l_itParam++)
	{
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itParam)->GetAttribute(wxT("ParameterList"))))->SetActiveList(l_nCurrentParameterSet);
	}

	//Reload the parameters
	LoadParameters();
}

void SP_DLG_ColCPNDirSimulationResults::OnFunctionSetChanged(wxCommandEvent& p_cEvent)
{
	unsigned int l_nCurrentFunctionSet = m_pcFunctionSetComboBox->GetSelection();
	const SP_ListNode* l_pcTransitions = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements();

	//Set the current Active List
	for (SP_ListNode::const_iterator l_itTrans = l_pcTransitions->begin(); l_itTrans != l_pcTransitions->end(); l_itTrans++)
	{
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itTrans)->GetAttribute(wxT("FunctionList"))))->SetActiveColumn(l_nCurrentFunctionSet);
	}

	//TODO: load only rate functions
	m_bIsSimulatorInitialized = false;
}

void SP_DLG_ColCPNDirSimulationResults::OnModifyMarkingSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_ColStMarkingOverview l_cDlg(this);
	if (l_cDlg.ShowModal() == wxID_OK)
	{
		LoadSets();

		
	}
	m_bIsSimulatorInitialized = false;
}
void SP_DLG_ColCPNDirSimulationResults::OnModifyFunctionSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_ColStFunctionOverview* l_pcDlg = new SP_DLG_ColStFunctionOverview(SP_DS_CONTINUOUS_TRANS, this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();


	}
}

void SP_DLG_ColCPNDirSimulationResults::OnModifyParameterSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_StParameterOverview* l_pcDlg = new SP_DLG_StParameterOverview(this);
	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();
	}
	m_bIsSimulatorInitialized = false;
}

void SP_DLG_ColCPNDirSimulationResults::LoadSets()
{
	m_pcMarkingSetComboBox->Clear();
	m_pcParameterSetComboBox->Clear();
	m_pcFunctionSetComboBox->Clear();

	const SP_ListNode *l_pcNodesList;
	SP_DS_Node *l_pcFstNode;
	SP_DS_Attribute *l_pcAttr;
	SP_DS_ColListAttribute* l_pcColListAttr;

	//load Marking set
	l_pcNodesList = (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements());
	if (!l_pcNodesList->empty())
	{
		l_pcFstNode = l_pcNodesList->front();
		l_pcAttr = l_pcFstNode->GetAttribute(wxT("MarkingList"));
		l_pcColListAttr = dynamic_cast<SP_DS_ColListAttribute *>(l_pcAttr);

		for (unsigned int i = 1; i < l_pcColListAttr->GetColCount(); i++)
		{
			wxString l_sSetName = l_pcColListAttr->GetColLabel(i);
			l_sSetName = l_sSetName.BeforeFirst(wxT(':'));
			m_pcMarkingSetComboBox->Append(l_sSetName);
			i++;
		}

		//Select the first marking
		if (m_pcMarkingSetComboBox->GetCount() <= l_pcColListAttr->GetActiveColumn())
			m_pcMarkingSetComboBox->SetSelection(0);
		else
			m_pcMarkingSetComboBox->SetSelection(l_pcColListAttr->GetActiveColumn());
	}

	//load parameter set
	l_pcNodesList = (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_PARAM)->GetElements());
	if (!l_pcNodesList->empty())
	{
		l_pcFstNode = l_pcNodesList->front();
		l_pcAttr = l_pcFstNode->GetAttribute(wxT("ParameterList"));
		l_pcColListAttr = dynamic_cast<SP_DS_ColListAttribute *>(l_pcAttr);
		for (unsigned int i = 0; i < l_pcColListAttr->GetRowCount(); i++)
		{
			m_pcParameterSetComboBox->Append(l_pcColListAttr->GetCell(i, 0));
		}
		//Select the first Parameter
		m_pcParameterSetComboBox->SetSelection(l_pcColListAttr->GetActiveList());
	}

	//load function set
	l_pcNodesList = (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements());
	if (!l_pcNodesList->empty())
	{
		l_pcFstNode = l_pcNodesList->front();
		l_pcAttr = l_pcFstNode->GetAttribute(wxT("FunctionList"));
		l_pcColListAttr = dynamic_cast<SP_DS_ColListAttribute *>(l_pcAttr);
		for (unsigned int i = 1; i < l_pcColListAttr->GetColCount(); i++)
		{
			wxString l_sSetName = l_pcColListAttr->GetColLabel(i);
			l_sSetName = l_sSetName.BeforeFirst(wxT(':'));
			m_pcFunctionSetComboBox->Append(l_sSetName);
		}
		//select the first function	  
		if (m_pcFunctionSetComboBox->GetCount() <= l_pcColListAttr->GetActiveColumn())
			m_pcFunctionSetComboBox->SetSelection(0);
		else
			m_pcFunctionSetComboBox->SetSelection(l_pcColListAttr->GetActiveColumn());
	}
}

void SP_DLG_ColCPNDirSimulationResults::DirectExportToCSV()
{
	if (!m_pcExport)
		return;

	const SP_Vector2DDouble& l_aanPLResults = m_pcMainSimulator->GetResultMatrix();
	const SP_Vector2DDouble& l_aanTRResults = m_pcMainSimulator->GetRateResultMatrix();
	wxString l_sSpacer = GetSpacer(m_nExportSpacer);
	wxString l_sCurrentRow, l_sOutput;

	SP_DS_Attribute* l_pcAttribute = m_pcCurrentTablePlot->GetAttribute(wxT("Nodeclass"));
	CHECK_POINTER(l_pcAttribute, return);
	wxString l_sElementType = l_pcAttribute->GetValueString();

	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo")));
	CHECK_POINTER(l_pcCurveInfoList, return);

	for (unsigned long l_nRow = 0; l_nRow < m_pcMainSimulator->GetGeneratedResultPointsCount(); l_nRow++)
	{
		l_sOutput = wxT("");
		l_sOutput << dssd::aux::toString(m_pcMainSimulator->GetOutputSampleSize() * l_nRow);
		l_sCurrentRow = wxT("");

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
			
				if (l_sOutType == wxT("Colored") && l_sElementType.IsSameAs(wxT("Place"))) //colored  place
				{
					l_dResult = m_aanColPlaceResults[l_nRow][l_nPos];
				}
				else
					if (l_sOutType == wxT("Auxiliary variables") && l_sElementType.IsSameAs(wxT("Place"))) //uncolored place
					{
						l_dResult = m_aanAuxPLVarsResults[l_nRow][l_nPos];
					}

					

						else
							if (l_sOutType == wxT("Colored") && l_sElementType.IsSameAs(wxT("Transition"))) //colored transitions
							{
								l_dResult = m_aanColTransResults[l_nRow][l_nPos];
							}

							else
								if (l_sOutType == wxT("Auxiliary variables") && l_sElementType.IsSameAs(wxT("Transition"))) //unfolded place
								{
									l_dResult = m_aanAuxTRVarsResults[l_nRow][l_nPos];
								}
								else
								{
									continue;
								}

			//write 
			l_sCurrentRow << l_sSpacer;
			l_sCurrentRow << wxString::Format(wxT("%.16g"), l_dResult);
		}

		l_sOutput << l_sCurrentRow;
		(*m_pcExport) << l_sOutput << wxT("\n");
	}
	
}

void SP_DLG_ColCPNDirSimulationResults::SaveODE(wxCommandEvent& p_cEvent)
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
	} while (l_bSelAgain);

	if (!InitializeSimulator())
	{
		SP_MESSAGEBOX(wxT("Error: Can not Initialize Place ODE"));
		return;
	}

	wxString l_sResult = wxT("");
	
	for (unsigned i = 0; i < m_msColoredPlaceNames.size(); i++)
	{
		
		wxString l_sUncoloredplaces = m_msUncoloredPlaceNames[i];
		
		l_sResult << wxT("d") << l_sUncoloredplaces << wxT("/dt = ");
		l_sResult << (dynamic_cast<spsim::ColContinuousSimulator*>(m_pcMainSimulator))->GetColPlaceODEString(i) << wxT("\n\n");
	}
	
	m_pcMainSimulator->AbortSimulation();

	if (l_sOutName.empty())
		return;
	if (!l_File.Open(l_sOutName.c_str(), wxT("wt")))
		return;
	l_File.Write(l_sResult);
	if (l_File.Close())
		return;
}


// this function not used 
wxString SP_DLG_ColCPNDirSimulationResults::GetODEString(SP_DS_Node* p_pcPlaceNode)
{

	wxString l_sfinalEq = wxT("");
	wxString l_sSecondEq;
	wxString l_sColPlaceName = dynamic_cast<SP_DS_NameAttribute*>(p_pcPlaceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
	SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(p_pcPlaceNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
	
	wxString l_sColorSetName = l_pcNameAttibute->GetValue();
	vector<wxString> l_ColorVector;
	SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSetName);
	
	l_ColorVector = l_pcColorSet->GetStringValue();
	wxString l_snumberofcol = wxString::Format(wxT("%ld"), l_ColorVector.size());
	if (l_snumberofcol == wxT("1"))
	{
		l_sSecondEq = wxT("d(") + l_sColPlaceName + wxT("_") + l_sColorSetName + wxT(")/dt") + wxT("=") +
			FillInPreConnection(p_pcPlaceNode) + FillInPostConnection(p_pcPlaceNode) + wxT(" \n \n");
	}
	else
	{
		l_sSecondEq = wxT("d(") + l_sColPlaceName + wxT("_") + l_sColorSetName + wxT(")i/dt") + wxT("=") +
			FillInPreConnection(p_pcPlaceNode) + FillInPostConnection(p_pcPlaceNode) +
			wxT("    i from 1 to ") + l_snumberofcol + wxT(" \n \n");
	}
	l_sfinalEq += l_sSecondEq;

	return l_sfinalEq;
}
//================================================================
wxString SP_DLG_ColCPNDirSimulationResults::FillInPreConnection(SP_DS_Node* p_pcPlaceNode)
{
	wxString l_sfirstEq = wxT("");
	CHECK_POINTER(p_pcPlaceNode, return l_sfirstEq);

	
	wxString l_sRest = wxT("");
	wxString l_sColorSetName = wxT("");
	wxString l_sColPlaceName = wxT("");

	for (auto l_itEdge = p_pcPlaceNode->GetTargetEdges()->begin(); l_itEdge != p_pcPlaceNode->GetTargetEdges()->end(); ++l_itEdge)
	{

		m_pcpreEdgeVector.push_back(*l_itEdge);

		SP_DS_Node* l_pcPlacesNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetTarget());
		l_sColPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlacesNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		
		SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(p_pcPlaceNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
		if (!l_pcNameAttibute)
			return l_sfirstEq;
		l_sColorSetName = l_pcNameAttibute->GetValue();

		SP_DS_Node* l_pcTransNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource());

		wxString l_sColTranName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		SP_DS_ColListAttribute* l_pcColList;
		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((l_pcTransNode)->GetAttribute(wxT("FunctionList")));
		CHECK_POINTER(l_pcColList, return l_sfirstEq);
		wxString l_TransRatFunction = l_pcColList->GetActiveCellValue(1);
		SP_ListEdge::const_iterator l_itEdge1;
		wxString l_sColprePlac = wxT("");
		for (l_itEdge1 = l_pcTransNode->GetTargetEdges()->begin(); l_itEdge1 != l_pcTransNode->GetTargetEdges()->end(); ++l_itEdge1)
		{
			
			SP_DS_Node* l_pcTransNode1 = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetTarget());
			wxString l_sColTransName1 = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlacesNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();


			SP_DS_Node* l_pcPlacesNode1 = dynamic_cast<SP_DS_Node*>((*l_itEdge1)->GetSource());
			wxString l_sColPlaceName1 = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlacesNode1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			
			SP_DS_TextAttribute* l_pcNameAttibute1 = dynamic_cast< SP_DS_TextAttribute* >(l_pcPlacesNode1->GetAttribute(SP_DS_CPN_COLORSETNAME));
			if (!l_pcNameAttibute1)
				return l_sfirstEq;
			wxString l_sColorSetName1 = l_pcNameAttibute1->GetValue();
			vector<wxString> l_ColorVector1;
			SP_CPN_ColorSet* l_pcColorSet1 = m_ColorSetClass.LookupColorSet(l_sColorSetName1);
			
			l_ColorVector1 = l_pcColorSet1->GetStringValue();
			wxString l_snumberofcol1 = wxString::Format(wxT("%ld"), l_ColorVector1.size());


			SP_DS_Edge* p_pcEdge1 = *l_itEdge1;
			wxString l_sArcmult1 = GetEdgeWeight(p_pcEdge1);
				
			

			std::size_t foundrat = l_TransRatFunction.find("MassAction");
			if (foundrat != std::string::npos)
			{
				vector<wxString> l_vsUncoloredPlaces;
				wxString l_sUncolPrePlaces = wxT("");
				for (unsigned int l_nPos = 0; l_nPos < l_ColorVector1.size(); l_nPos++)
				{
					wxString l_sColor = l_ColorVector1[l_nPos];
					//ModifyName(l_sColor);
					wxString l_sUnColoredPlaceName = wxT("(")+l_sColPlaceName1 + wxT("_") +
						                            l_sColor + wxT("^") + l_sArcmult1+ wxT(")");
					l_vsUncoloredPlaces.push_back(l_sUnColoredPlaceName);
					l_sUncolPrePlaces += wxT("+") + l_sUnColoredPlaceName;
					if (l_sUncolPrePlaces[0] == wxT('+'))
					{
						l_sUncolPrePlaces = l_sUncolPrePlaces.AfterFirst(wxT('+'));
					}
				}
				l_sColprePlac += wxT("*[") + l_sUncolPrePlaces + wxT("]");
			}
			else
				l_sColprePlac += wxT("*") + l_snumberofcol1;

		}

		wxString l_sTransRat = GetTransRate(l_TransRatFunction);
		

		SP_DS_Edge* p_pcEdge = *l_itEdge;
		wxString l_sArcmult= GetEdgeWeight(p_pcEdge);
		
		l_sRest += wxT("+") + l_sArcmult + wxT("*") + l_sTransRat + l_sColprePlac;

		if (l_sRest[0] == wxT('+'))
		{
			l_sRest = l_sRest.AfterFirst(wxT('+'));
		}
		
		m_msColPreTransNames.push_back(l_sColTranName);


		CHECK_POINTER(l_pcTransNode, return l_sfirstEq);

		wxString l_sTransType = l_pcTransNode->GetClassName();

		//we record only connections between places and continuous or stochastic transitions.
		if (!(l_sTransType == SP_DS_CONTINUOUS_TRANS || l_sTransType == SP_DS_STOCHASTIC_TRANS || l_sTransType == SP_DS_CONTINUOUS_PLACE))
			continue; //jumping to the end of loop 

		
	}
	l_sfirstEq += l_sRest;

	

	return l_sfirstEq;
}
//==============================================================================
wxString SP_DLG_ColCPNDirSimulationResults::FillInPostConnection(SP_DS_Node* p_pcPlaceNode)
{
	wxString l_ssecondEq = wxT("");
	CHECK_POINTER(p_pcPlaceNode, return l_ssecondEq);

	
	SP_ListEdge::const_iterator l_itEdge;
	wxString l_sRest = wxT("");
	for (l_itEdge = p_pcPlaceNode->GetSourceEdges()->begin(); l_itEdge != p_pcPlaceNode->GetSourceEdges()->end(); ++l_itEdge)
	{
		m_pcpostEdgeVector.push_back(*l_itEdge);

		SP_DS_Node* l_pcPlacesNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource());
		wxString l_sColPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlacesNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		
		SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast< SP_DS_TextAttribute* >(p_pcPlaceNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
		if (!l_pcNameAttibute)
			return l_sRest;
		wxString l_sColorSetName = l_pcNameAttibute->GetValue();
		vector<wxString> l_ColorVector;
		SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSetName);
		
		l_ColorVector = l_pcColorSet->GetStringValue();
		wxString l_snumberofcol = wxString::Format(wxT("%ld"), l_ColorVector.size());


		SP_DS_Node* l_pcTransNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetTarget());
		wxString l_sColTranName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		SP_DS_ColListAttribute* l_pcColList;
		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((l_pcTransNode)->GetAttribute(wxT("FunctionList")));
		CHECK_POINTER(l_pcColList, return l_sRest);
		wxString l_TransRatFunction = l_pcColList->GetActiveCellValue(1);
		
		
		SP_DS_Edge* p_pcEdge = *l_itEdge;
		wxString l_sArcmultplicity = GetEdgeWeight(p_pcEdge);
			


		SP_ListEdge::const_iterator l_itEdge2;
		wxString l_Spostplaces = wxT("");
		for (l_itEdge2 = l_pcTransNode->GetSourceEdges()->begin(); l_itEdge2 != l_pcTransNode->GetSourceEdges()->end(); ++l_itEdge2)
		{
			
			SP_DS_Node* l_pcTransNode1 = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource());
			wxString l_sColTransName1 = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlacesNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();


			SP_DS_Node* l_pcPlacesNode2 = dynamic_cast<SP_DS_Node*>((*l_itEdge2)->GetTarget());
			wxString l_sColPlaceName2 = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlacesNode2->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			SP_DS_TextAttribute* l_pcNameAttibute2 = dynamic_cast<SP_DS_TextAttribute*>(l_pcPlacesNode2->GetAttribute(SP_DS_CPN_COLORSETNAME));
			if (!l_pcNameAttibute2)
				return l_sRest;
			wxString l_sColorSetName2 = l_pcNameAttibute2->GetValue();
			vector<wxString> l_ColorVector2;
			SP_CPN_ColorSet* l_pcColorSet2 = m_ColorSetClass.LookupColorSet(l_sColorSetName2);
			
			l_ColorVector2 = l_pcColorSet2->GetStringValue();
			wxString l_snumberofcol2 = wxString::Format(wxT("%ld"), l_ColorVector2.size());


			l_Spostplaces += wxT("*") + l_snumberofcol2;
			
		}
		wxString l_sTransType = l_pcTransNode->GetClassName();
		stringstream testTrans(l_TransRatFunction);
		
		wxString l_sTransRat = GetTransRate(l_TransRatFunction);
		std::size_t foundrat = l_TransRatFunction.find("MassAction");
		
		
		if (l_ColorVector.size() == 1 && foundrat != std::string::npos)
		{
			l_sRest += wxT("-") + l_sArcmultplicity + wxT("*") + l_sTransRat + l_Spostplaces + wxT("*(") +
				l_sColPlaceName + wxT("^") + l_sArcmultplicity + wxT(")");
		}
		else if (l_ColorVector.size() > 1 && foundrat != std::string::npos)
		{
			l_sRest += wxT("-") + l_sArcmultplicity + wxT("*") + l_sTransRat + l_Spostplaces + wxT("*(") +
				l_sColPlaceName + wxT("_i^") + l_sArcmultplicity + wxT(")");
		}
		else
			l_sRest += wxT("-") + l_sArcmultplicity + wxT("*") + l_sTransRat + l_Spostplaces;


		//we record only connections between places and continuous or stochastic transitions.
		if (!(l_sTransType == SP_DS_CONTINUOUS_TRANS || l_sTransType == SP_DS_STOCHASTIC_TRANS || l_sTransType == SP_DS_CONTINUOUS_PLACE))
			continue;

		
	}
	l_ssecondEq += l_sRest;
	

	long l_npostEdgeNumber = m_pcpostEdgeVector.size();
	wxString l_spostTranNumber = wxString::Format(wxT("%ld"), l_npostEdgeNumber);
	
	m_pcpostEdgeVector.clear();
	return l_ssecondEq;
}
//=================================================================
//this function not used
wxString SP_DLG_ColCPNDirSimulationResults::GetODETranswGuard(SP_DS_Node* p_pcPlaceNode)
{
	wxString l_stotalChange = wxT("");
	wxString l_sinflow = wxT("");
	wxString l_soutflow = wxT("");

	wxString l_sColPlaceName = dynamic_cast<SP_DS_NameAttribute*>(p_pcPlaceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

	SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast<SP_DS_TextAttribute*>(p_pcPlaceNode->GetAttribute(SP_DS_CPN_COLORSETNAME));

	wxString l_sColorSetName = l_pcNameAttibute->GetValue();

	vector<wxString> l_ColorVector;

	SP_CPN_ColorSet* l_pcColorSet = m_ColorSetClass.LookupColorSet(l_sColorSetName);
	l_ColorVector = l_pcColorSet->GetStringValue();
	wxString l_sColor;

	for (unsigned int l_nPos = 0; l_nPos < l_ColorVector.size(); l_nPos++)
	{
		l_sColor = l_ColorVector[l_nPos];
		wxString l_sfirschange = wxT("d") + l_sColPlaceName + wxT("_") + l_sColor + wxT("/dt = ");
		
		l_sinflow=FillInPreConnguard(p_pcPlaceNode, l_sColor);
		l_soutflow=FillInPostConnguard(p_pcPlaceNode, l_sColor);
		l_stotalChange += l_sfirschange + l_sinflow + l_soutflow + wxT("\n \n");
		
	}
	
	return l_stotalChange;
	
}
//================================================================================
wxString SP_DLG_ColCPNDirSimulationResults::FillInPreConnguard(SP_DS_Node* p_pcPlaceNode, wxString p_sVariablval)
{
	
		wxString l_sinflow = wxT("");
		for (auto l_itEdge = p_pcPlaceNode->GetTargetEdges()->begin(); l_itEdge != p_pcPlaceNode->GetTargetEdges()->end(); ++l_itEdge)
		{

			m_pcpreEdgeVector.push_back(*l_itEdge);
			SP_DS_Node* l_pcPlacesNode1 = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetTarget());

			SP_DS_Node* l_pcTransNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource());

			wxString l_sColTranName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			SP_DS_ColListAttribute* l_pcColList;
			l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((l_pcTransNode)->GetAttribute(wxT("FunctionList")));
			SP_DS_ColListAttribute* l_pcColList11 = dynamic_cast<SP_DS_ColListAttribute*>(l_pcTransNode->GetAttribute(SP_DS_CPN_GUARDLIST));
			wxString l_TransRatFunction = l_pcColList->GetActiveCellValue(1);

			wxString l_sTranGuard = wxString(l_pcColList11->GetCell(0, 1).c_str());
			wxString l_sTransRat = GetTransRate(l_TransRatFunction);

			SP_DS_Edge* p_pcEdge1 = *l_itEdge;
			wxString l_sArcmult1 = GetEdgeWeight(p_pcEdge1);
			SP_DS_ColListAttribute* l_pcColList1 = dynamic_cast< SP_DS_ColListAttribute* >(p_pcEdge1->GetAttribute(SP_DS_CPN_INSCRIPTION));
			wxString l_sArcExpression = l_pcColList1->GetCell(0, 1);
			unsigned found = l_sArcExpression.find_last_of("`");
			wxString l_sArcVariable = l_sArcExpression.substr(found + 1);

			SP_ListEdge::const_iterator l_itEdge1;
			wxString l_sColprePlac = wxT("");
			

			std::size_t foundg = l_sTranGuard.find(l_sArcVariable + wxT('=') + p_sVariablval);
			std::size_t ArcVariable = l_sTranGuard.find(l_sArcVariable);

//*********************************************************
			//if (foundg != std::string::npos )
			//{

				for (l_itEdge1 = l_pcTransNode->GetTargetEdges()->begin(); l_itEdge1 != l_pcTransNode->GetTargetEdges()->end(); ++l_itEdge1)
				{

					SP_DS_Node* l_pcTransNode1 = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetTarget());
					wxString l_sColTransName1 = dynamic_cast<SP_DS_NameAttribute*>(l_pcTransNode1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();


					SP_DS_Node* l_pcPlacesNode1 = dynamic_cast<SP_DS_Node*>((*l_itEdge1)->GetSource());
					wxString l_sColPlaceName1 = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlacesNode1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

					SP_DS_TextAttribute* l_pcNameAttibute1 = dynamic_cast<SP_DS_TextAttribute*>(l_pcPlacesNode1->GetAttribute(SP_DS_CPN_COLORSETNAME));
					
					wxString l_sColorSetName1 = l_pcNameAttibute1->GetValue();
					vector<wxString> l_ColorVector1;
					SP_CPN_ColorSet* l_pcColorSet1 = m_ColorSetClass.LookupColorSet(l_sColorSetName1);

					l_ColorVector1 = l_pcColorSet1->GetStringValue();
					wxString l_snumberofcol1 = wxString::Format(wxT("%ld"), l_ColorVector1.size());


					SP_DS_Edge* p_pcEdge11 = *l_itEdge1;
					SP_DS_ColListAttribute* l_pcColList11 = dynamic_cast<SP_DS_ColListAttribute*>(p_pcEdge11->GetAttribute(SP_DS_CPN_INSCRIPTION));
					wxString l_sArcExpression1 = l_pcColList11->GetCell(0, 1);
					unsigned found1 = l_sArcExpression1.find_last_of("`");
					wxString l_sArcVariable1 = l_sArcExpression1.substr(found + 1);

					wxString l_sArcmult11 = GetEdgeWeight(p_pcEdge11);

					std::size_t foundrat = l_TransRatFunction.find("MassAction");
					if (foundrat != std::string::npos)
					{
						vector<wxString> l_vsUncoloredPlaces;
						wxString l_sUncolPrePlaces = wxT("");
						for (unsigned int l_nPos = 0; l_nPos < l_ColorVector1.size(); l_nPos++)
						{
							wxString l_sColor = l_ColorVector1[l_nPos];
							//ModifyName(l_sColor);

							std::size_t foundg = l_sTranGuard.find(l_sArcVariable1 + wxT('=') + l_sColor);

							wxString l_sUnColoredPlaceName;
							if (foundg != std::string::npos)
							{
							l_sUnColoredPlaceName = wxT("(") + l_sColPlaceName1 + wxT("_") +
								l_sColor + wxT("^") + l_sArcmult11 + wxT(")");
							    l_sUncolPrePlaces += wxT("+") + l_sUnColoredPlaceName;
							if (l_sUncolPrePlaces[0] == wxT('+'))
							{
								l_sUncolPrePlaces = l_sUncolPrePlaces.AfterFirst(wxT('+'));
							}
							}
							


						}
						l_sColprePlac += wxT("*[") + l_sUncolPrePlaces + wxT("]");
					}
					else
						l_sColprePlac += wxT("*") + l_snumberofcol1;

				}

				

				l_sinflow += wxT("+") + l_sArcmult1 + wxT("*") + l_sTransRat + l_sColprePlac;
				if (l_sinflow[0] == wxT('+'))
				{
					l_sinflow = l_sinflow.AfterFirst(wxT('+'));
				}

	
		
			
		}
		if (l_sinflow == wxT(""))
		{
		
			l_sinflow = wxT("0");
	    }

	return l_sinflow;
}

//==========================================================================

wxString SP_DLG_ColCPNDirSimulationResults::FillInPostConnguard(SP_DS_Node* p_pcPlaceNode, wxString p_sVariablval)
{


	wxString l_soutflow = wxT("");
	for (auto l_itEdge = p_pcPlaceNode->GetSourceEdges()->begin(); l_itEdge != p_pcPlaceNode->GetSourceEdges()->end(); ++l_itEdge)
	{

		m_pcpreEdgeVector.push_back(*l_itEdge);
		SP_DS_Node* l_pcPlacesNode1 = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource ());
		wxString l_sColplaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlacesNode1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		
		
		
		SP_DS_Node* l_pcTransNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetTarget());
        wxString l_sColTranName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		SP_DS_ColListAttribute* l_pcColList;
		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((l_pcTransNode)->GetAttribute(wxT("FunctionList")));
		SP_DS_ColListAttribute* l_pcColList11 = dynamic_cast<SP_DS_ColListAttribute*>(l_pcTransNode->GetAttribute(SP_DS_CPN_GUARDLIST));

		SP_DS_Edge* p_pcEdge1 = *l_itEdge;
		
		
		
		SP_DS_ColListAttribute* l_pcColList1 = dynamic_cast< SP_DS_ColListAttribute* >(p_pcEdge1->GetAttribute(SP_DS_CPN_INSCRIPTION));
		wxString l_sArcExpression = l_pcColList1->GetCell(0, 1);
		unsigned found = l_sArcExpression.find_last_of("`");
		wxString l_sArcVariable = l_sArcExpression.substr(found + 1);
		//SP_LOGERROR(l_sArcVariable);
		wxString l_sArcmult1 = GetEdgeWeight(p_pcEdge1);

		wxString l_TransRatFunction = l_pcColList->GetActiveCellValue(1);
		wxString l_sTranGuard = wxString(l_pcColList11->GetCell(0, 1).c_str());
		wxString l_sTransRat = GetTransRate(l_TransRatFunction);




		SP_ListEdge::const_iterator l_itEdge2;
		wxString l_Spostplaces = wxT("");

		for (l_itEdge2 = l_pcTransNode->GetSourceEdges()->begin(); l_itEdge2 != l_pcTransNode->GetSourceEdges()->end(); ++l_itEdge2)
		{

			SP_DS_Node* l_pcTransNode1 = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource());
			wxString l_sColTransName1 = dynamic_cast<SP_DS_NameAttribute*>(l_pcTransNode1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();


			SP_DS_Node* l_pcPlacesNode2 = dynamic_cast<SP_DS_Node*>((*l_itEdge2)->GetTarget());
			wxString l_sColPlaceName2 = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlacesNode2->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			SP_DS_TextAttribute* l_pcNameAttibute2 = dynamic_cast<SP_DS_TextAttribute*>(l_pcPlacesNode2->GetAttribute(SP_DS_CPN_COLORSETNAME));
			
			wxString l_sColorSetName2 = l_pcNameAttibute2->GetValue();
			vector<wxString> l_ColorVector2;
			SP_CPN_ColorSet* l_pcColorSet2 = m_ColorSetClass.LookupColorSet(l_sColorSetName2);

			l_ColorVector2 = l_pcColorSet2->GetStringValue();
			wxString l_snumberofcol2 = wxString::Format(wxT("%ld"), l_ColorVector2.size());


			l_Spostplaces += wxT("*") + l_snumberofcol2;



		}


		wxString l_splacecolor;
		std::size_t foundrat = l_TransRatFunction.find("MassAction");


		if (foundrat != std::string::npos)
		{
			l_splacecolor += wxT("(")+ l_sColplaceName + wxT("_") + p_sVariablval + wxT("^") 
				+ l_sArcmult1 + wxT(")") + l_Spostplaces;

		}
		else
			l_splacecolor =  l_Spostplaces;





		if (l_sTranGuard != wxT(""))
		{
			std::size_t foundg = l_sTranGuard.find(l_sArcVariable + wxT('=') + p_sVariablval);

			if (foundg != std::string::npos/* && foundrat != std::string::npos*/)
			{

				l_soutflow += wxT("-") + l_sArcmult1+wxT("*") + l_sTransRat + l_splacecolor;
				
			}
			else
			{

				l_soutflow += wxT("")/* + l_sArcmult1 + wxT("*") + l_sTransRat + l_splacecolor*/;

			}
		}

	}
	
return l_soutflow;
}

//=======================================================



//not used
wxString SP_DLG_ColCPNDirSimulationResults::GetTransRate(wxString l_spTransrate)
{
	wxString l_sTransRate;
	/*if (l_spTransrate.Find(wxT("(")))
		l_sTransRate = l_spTransrate.SubString('('+1, ')'-'('-1);
	else
		l_sTransRate = l_spTransrate;*/

	stringstream testTrans(l_spTransrate);

	size_t s = l_spTransrate.find("(");
	size_t e = l_spTransrate.find(")", s);
	string sub = l_spTransrate.substr(s + 1, e - s - 1);

	std::size_t foundrat = l_spTransrate.find("MassAction");
	if (foundrat != std::string::npos)
	{

		l_sTransRate = sub;
		
	}
	else
		l_sTransRate = l_spTransrate;

	return l_sTransRate;
}
//====================================================================
void SP_DLG_ColCPNDirSimulationResults::AddNewMarkingSet(const wxString& p_sMarkingSetName, const int& p_nRow)
{

	//Get current row result
	SP_VectorDouble l_nCurrentTableRow = m_pcMainSimulator->GetResultMatrix()[p_nRow];

	SP_DS_ColListAttribute* l_pcColList;
	SP_ListNode::const_iterator l_itElem;

	//Continuous places
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);

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
}

bool SP_DLG_ColCPNDirSimulationResults::IsMarkingSetNameExist(const wxString& p_sName)
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

void SP_DLG_ColCPNDirSimulationResults::LoadPlaces()
{

	
	*m_pcColoredNet->GetColoredPlaceNames();

	m_pcMainSimulator->SetPlaceCount(m_pcColoredNet->GetColoredPlaceNames()->size());

	m_pcMainSimulator->SetPlaceNames(*m_pcColoredNet->GetColoredPlaceNames());
	
	//m_pcMainSimulator->SetPlaceColorSet(*m_pcColoredNet->GetColorSetOfPlace());
	
	(dynamic_cast<spsim::ColContinuousSimulator*>(m_pcMainSimulator))->SetInitialMarking(*m_pcColoredNet->GetmarkingDouble());
	
}

void SP_DLG_ColCPNDirSimulationResults::LoadTransitions()
{
	
	m_pcMainSimulator->ClearTransitions();


	m_pcMainSimulator->SetTransitionCount(m_pcColoredNet->GetColoredTransitionNames()->size());

	SP_VectorString* l_pcTransitionName = m_pcColoredNet->GetColoredTransitionNames();
	SP_VectorString* l_pcTransitionFunction = m_pcColoredNet->GetColTransRatFun();
	//SP_VectorString* l_pcTransitionGuard = m_pcColoredNet->GetColoredTranGuard();

	for (unsigned long l_nTransPos = 0; l_nTransPos < l_pcTransitionName->size(); l_nTransPos++)
	{
		m_pcMainSimulator->AddTransition((*l_pcTransitionName)[l_nTransPos],

			(*l_pcTransitionFunction)[l_nTransPos],/* (*l_pcTransitionGuard)[l_nTransPos],******/

			spsim::TRANSITION_TYPE_CONTINUOUS);
	}
}

void SP_DLG_ColCPNDirSimulationResults::LoadConnections()
	{
		//Map transition name to their positions
		m_pcMainSimulator->MapTransitionNames2Positions();

		m_pcMainSimulator->MapPlaceNames2Positions();

		//Edge
		LoadConnectionOfType(SP_DS_EDGE, spsim::STANDARD_ARC);

		//Read Arc
		LoadConnectionOfType(SP_DS_READ_EDGE, spsim::READ_ARC);

		//Inhibitor edge
		LoadConnectionOfType(SP_DS_INHIBITOR_EDGE, spsim::INHIBITOR_ARC);

		//Modifier
		LoadConnectionOfType(SP_DS_MODIFIER_EDGE, spsim::MODIFIER_ARC);
}
	
void SP_DLG_ColCPNDirSimulationResults::LoadConnectionOfType(const wxString& p_sArcType, const spsim::ConnectionType& p_ArcType)
{
		const SP_ListEdge* l_pcEdgeList;
		SP_ListEdge::const_iterator l_itEdge;

		wxString l_sSourceNodeName, l_sDestNodeName;

		SP_DS_Node* l_pcSourceNode, *l_pcTargetNode;

		m_ValueAssign.InitializeColorset(m_ColorSetClass);

		SP_DS_NameAttribute* l_pcNameAttribute;

		CHECK_POINTER(m_pcGraph->GetEdgeclass(p_sArcType), return);

		l_pcEdgeList = m_pcGraph->GetEdgeclass(p_sArcType)->GetElements();

		for (l_itEdge = l_pcEdgeList->begin(); l_itEdge != l_pcEdgeList->end(); l_itEdge++)
		{
			//Get source Node Name
			l_pcSourceNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource());

			CHECK_POINTER(l_pcSourceNode, return);

			l_pcNameAttribute = dynamic_cast<SP_DS_NameAttribute*>(l_pcSourceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));

			l_sSourceNodeName = l_pcNameAttribute->GetValue();

			//Get target Node Name
			l_pcTargetNode = dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetTarget());

			CHECK_POINTER(l_pcTargetNode, return);

			l_pcNameAttribute = dynamic_cast<SP_DS_NameAttribute*>(l_pcTargetNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));

			l_sDestNodeName = l_pcNameAttribute->GetValue();

			wxString l_sArcWeight = wxT("");
		
			
			l_sArcWeight = m_pcColoredNet->GetArcweightwithSize( l_pcSourceNode, *l_itEdge, l_pcTargetNode);
			
		
			// to convert the arc weight to number
			double l_nNumericArcWeight = 0;

			
			if (l_pcSourceNode->GetClassName().Contains(wxT("Transition")) == true)
			{


				if (IsConstantArcWeight(l_sArcWeight, l_nNumericArcWeight) == true)
				{
					m_pcMainSimulator->SetPostTransitionConnection(l_sSourceNodeName, l_sDestNodeName, l_nNumericArcWeight);
					//m_pcMainSimulator->SetVarArcSize(l_nlArcVariablSize);
				}
				else
				{
					

					m_pcMainSimulator->SetPostSelfModifyingWeights(l_sSourceNodeName, l_sDestNodeName, l_sArcWeight);
				}

			}
			else
			{

				if (IsConstantArcWeight(l_sArcWeight, l_nNumericArcWeight) == true)
				{
					m_pcMainSimulator->SetPreTransitionConnection(l_sDestNodeName, l_sSourceNodeName, p_ArcType, l_nNumericArcWeight);
				}
				else
				{
					m_pcMainSimulator->SetPreSelfModifyingWeights(l_sDestNodeName, l_sSourceNodeName, p_ArcType, l_sArcWeight);
				}

			}
		}
	}

bool SP_DLG_ColCPNDirSimulationResults::Testcolorsetofallplaces()
{
	SP_DS_Nodeclass* l_pcNodeclass;

	SP_ListNode::const_iterator l_itElem;
	std::list<SP_DS_Nodeclass*> m_pcContPlaceNodeClass;

	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
	if (l_pcNodeclass)
	{
		m_pcContPlaceNodeClass.push_back(l_pcNodeclass);
		for(auto it_node: m_pcContPlaceNodeClass)
		for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;
			wxString l_sColPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(SP_DS_CPN_MARKINGLIST));


			SP_DS_TextAttribute* l_pcNameAttibute = dynamic_cast<SP_DS_TextAttribute*>(l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME));

			wxString l_sColorSetName = l_pcNameAttibute->GetValue();
			m_msColorSetOfPlacetest.push_back(l_sColorSetName);

		}

	}
	for (long i = 0; i < m_msColorSetOfPlacetest.size(); i++)
	{
		if (m_msColorSetOfPlacetest.size() == 1)
			return true;
		else
			if (m_msColorSetOfPlacetest[i] == m_msColorSetOfPlacetest[i + 1])
				return true;
			else
			{
				SP_LOGERROR(wxT("Sorry,all places not have the same color set"));

				return false;
			}
	}
	return true;
}
//========================================================================
spsim::ConnectionType SP_DLG_ColCPNDirSimulationResults::GetConnectionType(const wxString& p_sConnectionType)
{
	if (p_sConnectionType == SP_DS_EDGE)
	{
		return spsim::STANDARD_ARC;
	}
	else
		if (p_sConnectionType == SP_DS_READ_EDGE)
		{
			return spsim::READ_ARC;
		}
		else
			if (p_sConnectionType == SP_DS_INHIBITOR_EDGE)
			{
				return spsim::INHIBITOR_ARC;
			}
			else
				if (p_sConnectionType == SP_DS_MODIFIER_EDGE)
				{
					return spsim::MODIFIER_ARC;
				}
				else
					if (p_sConnectionType == SP_DS_RESET_EDGE)
					{
						return spsim::RESET_ARC;
					}
					else
						if (p_sConnectionType == SP_DS_EQUAL_EDGE)
						{
							return spsim::EQUAL_ARC;
						}
						else
						{
							SP_LOGERROR(wxT("Unknown connection type,") + p_sConnectionType);
							return spsim::OTHER_CONNECTION;
						}
}

bool SP_DLG_ColCPNDirSimulationResults::ComputeAuxiliaryVars()
{
	m_aanAuxPLVarsResults.clear();
	m_vAuxPLVars.clear();
	SP_DS_DColTraceAnalyzer l_cTraceAnalyzer(m_pcColoredNet, m_pcMainSimulator->GetResultMatrix(), m_aanAuxPLVarsResults, m_vAuxPLVars);

	if (!l_cTraceAnalyzer.Compute())
		return false;

	return true;
}

void SP_DLG_ColCPNDirSimulationResults::LoadColAuxResults()
{
	GetColPlaceResults(m_pcMainSimulator->GetResultMatrix(), m_aanColPlaceResults);
	GetColTransResults(m_pcMainSimulator->GetRateResultMatrix(), m_aanColTransResults);

	if (!m_bComAuxVarSingleRun)
	{
		ComputeAuxiliaryVars();
	}
}

void SP_DLG_ColCPNDirSimulationResults::InitializeEmptyView(SP_DS_Metadata* p_pcView)
{
	CHECK_POINTER(p_pcView, return);

	//get the current nodeclass type
	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(wxT("Nodeclass"));

	CHECK_POINTER(l_pcAttribute, return);

	wxString l_sElementType = l_pcAttribute->GetValueString();

	SP_VectorString* l_pvCurrentInfo;

	SP_VectorString l_asColours;

	l_asColours.clear();

	if (l_sElementType.IsSameAs(wxT("Transition")))
	{
		l_pvCurrentInfo = &m_msColoredTransitionNames;

		//load element colours
		LoadNodeColours(SP_DS_CONTINUOUS_TRANS, l_asColours);
	}
	else
	{
		l_pvCurrentInfo = &m_msColoredPlaceNames;

		//load element colours
		LoadNodeColours(SP_DS_CONTINUOUS_PLACE, l_asColours);
	}

	l_pcAttribute = p_pcView->GetAttribute(wxT("CurveInfo"));
	CHECK_POINTER(l_pcAttribute, return);
	SP_DS_ColListAttribute* l_pcCurveInfoList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttribute);

	//position of the element in the result matrix
	for (unsigned int l_nPos = 0; l_nPos < l_pvCurrentInfo->size(); l_nPos++)
	{
		wxString l_sName = (*l_pvCurrentInfo)[l_nPos];

		unsigned int l_nNewRow = l_pcCurveInfoList->AppendEmptyRow();

		l_pcCurveInfoList->SetCell(l_nNewRow, 0, wxString::Format(wxT("%ld"), l_nPos));
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
void SP_DLG_ColCPNDirSimulationResults::UpdateXAxisValues()
{
	CHECK_POINTER(m_pcCurrentTablePlot, return);

	CalculateXAxisValues(m_pcCurrentTablePlot, m_anXAxisValues);
}

void SP_DLG_ColCPNDirSimulationResults::CalculateXAxisValues(SP_DS_Metadata* p_pcView, SP_VectorDouble& p_anXAxisValues)
{
	CHECK_POINTER(m_pcMainSimulator, return);

	wxString l_sCategory = GetViewAttributeValue(p_pcView, wxT("XAxisVariable"));
	wxString l_sSubCategory = GetViewAttributeValue(p_pcView, wxT("XAxisVariableName"));

	double l_nStartOutputPoint = m_pcMainSimulator->GetOutputStartPoint();

	p_anXAxisValues.clear();
	if (l_sCategory.IsSameAs(wxT("Time")))
	{
		double l_nTime = 0;

		for (unsigned long l_nPoint = 0; l_nPoint < m_pcMainSimulator->GetGeneratedResultPointsCount(); l_nPoint++)
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
					{
						double l_nTime = 0;

						for (unsigned long l_nPoint = 0; l_nPoint < m_pcMainSimulator->GetGeneratedResultPointsCount(); l_nPoint++)
						{
							p_anXAxisValues.push_back(l_nStartOutputPoint + l_nTime);

							l_nTime += m_pcMainSimulator->GetOutputSampleSize();
						}

					}
}

void SP_DLG_ColCPNDirSimulationResults::LoadParameters()
{
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_PARAM);
	SP_ListNode::const_iterator l_itElem;
	SP_DS_ColListAttribute* l_pcColList;
	SP_VectorString l_asParameterNames;
	SP_VectorDouble l_anParameterValue;

	auto l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);
	auto l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0);
		wxString l_sType = l_pcColList->GetCell(i, 1);
		wxString l_sConstantvalue = l_pcColList->GetCell(i, 2);
		if (l_sType == wxT("int"))
		{
			l_asParameterNames.push_back(l_sName);
			double l_nValue;
			l_sConstantvalue.ToDouble(&l_nValue);
			l_anParameterValue.push_back(l_nValue);
		}
	}

	//Go through all the transition nodes
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); l_itElem++)
	{
		//Set the transition name
		l_asParameterNames.push_back(dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue());

		//Get the transition rate function
		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>((*l_itElem)->GetAttribute(wxT("ParameterList")));
		l_anParameterValue.push_back(l_pcColList->GetActiveCellValueDouble(1));
	}

	m_pcMainSimulator->SetParameterNames(l_asParameterNames);
	m_pcMainSimulator->SetParameterValues(l_anParameterValue);
}

