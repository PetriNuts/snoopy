/*
* SP_DLG_ColFCPNSimulationResults.cpp
* $Author: G. Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 3.03.2020
* Short Description:
* Implementation file for the coloured fuzzy Continuous result dialog
*/
//===============================================================================================
#include "sp_gui/dialogs/dia_colFCPN/SP_DLG_ColFCPNSimulationResults.h"
#include "sp_gui/dialogs/dia_FPN/SP_DLG_FpnConstantDefinition.h"
IMPLEMENT_CLASS(SP_DLG_ColFCPNSimulationResults, SP_DLG_FCPNSimulationResults)

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
	SP_ID_BUTTON_SAVE_ODE,
	SP_ID_BUTTON_MODIFY_COL_CONSTANT_SETS,// by george

	SP_ID_BUTTON_CHANGE_COL_CONSTANT_SETS
};
BEGIN_EVENT_TABLE(SP_DLG_ColFCPNSimulationResults, SP_DLG_FCPNSimulationResults)

EVT_BUTTON(SP_ID_BUTTON_MODIFY_MARKING_SETS, SP_DLG_ColFCPNSimulationResults::OnModifyMarkingSets)
EVT_BUTTON(SP_ID_BUTTON_MODIFY_FUNCTION_SETS, SP_DLG_ColFCPNSimulationResults::OnModifyFunctionSets)
////EVT_BUTTON(SP_ID_BUTTON_MODIFY_PARAMETER_SETS, SP_DLG_ColCPNSimulationResults::OnModifyParameterSets)

EVT_CHOICE(SP_ID_CHOICE_MARKING_SETS, SP_DLG_ColFCPNSimulationResults::OnMarkingSetChanged)
EVT_CHOICE(SP_ID_CHOICE_FUNCTION_SETS, SP_DLG_ColFCPNSimulationResults::OnFunctionSetChanged)
/////EVT_CHOICE(SP_ID_CHOICE_PARAMETER_SETS, SP_DLG_ColCPNSimulationResults::OnParameterSetChanged)

EVT_CHOICE(SP_ID_CHOICE_SOLVER, SP_DLG_ColFCPNSimulationResults::OnSolverChanged)
EVT_BUTTON(SP_ID_BUTTON_SIMULATION_PROPERTIES, SP_DLG_ColFCPNSimulationResults::OnSimulationProperties)
EVT_BUTTON(SP_ID_BUTTON_SAVE_ODE, SP_DLG_ColFCPNSimulationResults::SaveODE)
EVT_BUTTON(SP_ID_BUTTON_MODIFY_COL_CONSTANT_SETS, SP_DLG_ColFCPNSimulationResults::OnModifyConstants)// by george
END_EVENT_TABLE()



SP_DLG_ColFCPNSimulationResults::SP_DLG_ColFCPNSimulationResults(SP_DS_Graph* p_pcGraph, SP_DS_ColPN_Unfolding* p_pcUnfoldedNet, wxWindow* p_pcParent, wxString p_sHelpText, wxString p_sTitle,
	long p_nStyle) :
	SP_DS_ColoredPNSimulation(p_pcUnfoldedNet),
	SP_DLG_FCPNSimulationResults(p_pcGraph, p_pcParent)

{
	 
	m_pcXAxisChoices.clear();
	m_pcXAxisChoices[wxT("Place-colored")] = &m_msColoredPlaceNames;
	m_pcXAxisChoices[wxT("Transition-colored")] = &m_msColoredTransitionNames;
	m_pcXAxisChoices[wxT("Place-uncolored")] = &m_msPlaceNames;
	m_pcXAxisChoices[wxT("Transition-uncolored")] = &m_msTransitionNames;

	//Add special UI element for Continuous class
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
	SetSizerAndFit(m_pcMainSizer);

}

void SP_DLG_ColFCPNSimulationResults::LoadSets()
{
	m_pcMarkingSetComboBox->Clear();
	//m_pcParameterSetComboBox->Clear();
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

		for (unsigned int i = 0; i < l_pcColListAttr->GetColCount(); i++)
		{
			wxString l_sSetName = l_pcColListAttr->GetColLabel(i);
			if (l_sSetName.Contains(wxT(":")))//by george
			{
				l_sSetName = l_sSetName.BeforeFirst(wxT(':'));
				m_pcMarkingSetComboBox->Append(l_sSetName);
			}
			i = i + 2;

		}

		//Select the first marking
		if (m_pcMarkingSetComboBox->GetCount() <= l_pcColListAttr->GetActiveColumn())
			m_pcMarkingSetComboBox->SetSelection(0);
		else
			m_pcMarkingSetComboBox->SetSelection(l_pcColListAttr->GetActiveColumn());
	}
	/**
	//load parameter set
	l_pcNodesList = (SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_PARAM)->GetElements());
	if (!l_pcNodesList->empty())
	{
		l_pcFstNode = l_pcNodesList->front();
		l_pcAttr = l_pcFstNode->GetAttribute(wxT("ParameterList"));
		l_pcColListAttr = dynamic_cast<SP_DS_ColListAttribute *>(l_pcAttr);
		for (unsigned int i = 0; i < l_pcColListAttr->GetRowCount(); i++)
		{
			//m_pcParameterSetComboBox->Append(l_pcColListAttr->GetCell(i, 0));
		}
		//Select the first Parameter
		//m_pcParameterSetComboBox->SetSelection(l_pcColListAttr->GetActiveList());
	}
	*/

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
	LoadConstantsSetsForColPN();//by george
}


void SP_DLG_ColFCPNSimulationResults::LoadConstantsSetsForColPN()
{
	int ss = m_apcColListAttr.size();

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
			if (l_sChoice == l_sGroup && m_iModifyCount == 0)
			{
				m_apcColListAttr.push_back(dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList"))));
				break;
			}
		}
	}


	//int start = m_apcColListAttr.size() - ss;
	for (size_t j = ss; j < m_apcColListAttr.size(); j++)
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



void SP_DLG_ColFCPNSimulationResults::OnModifyMarkingSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_ColStMarkingOverview l_cDlg(this);
	if (l_cDlg.ShowModal() == wxID_OK)
	{
		LoadSets();

		//to be changed. partial unfolding
		if (!LoadUnfoldedPlaces())
		{
			SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
			return;
		}
	}
	m_bIsSimulatorInitialized = false;
}


void SP_DLG_ColFCPNSimulationResults::OnMarkingSetChanged(wxCommandEvent& p_cEvent)
{
	//Get the Places Nodes
	unsigned int l_nCurrentMarkingSet = m_pcMarkingSetComboBox->GetSelection();
	const SP_ListNode* l_pcPlaces = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements();

	//Set the current Active column	
	for (SP_ListNode::const_iterator l_itPlace = l_pcPlaces->begin(); l_itPlace != l_pcPlaces->end(); l_itPlace++)
	{
		(dynamic_cast<SP_DS_ColListAttribute*>((*l_itPlace)->GetAttribute(wxT("MarkingList"))))->SetActiveColumn(l_nCurrentMarkingSet);
	}

	//Todo: load only the current marking
	LoadPlaces();

}

bool SP_DLG_ColFCPNSimulationResults::LoadViewerData(SP_DS_ResultViewer* p_pcViewer, SP_DS_Metadata* p_pcView, wxArrayString& p_asPlaces)
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
				p_pcViewer->AddCurve(l_sName, l_nPosition, &m_anResultMatrix);
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


void SP_DLG_ColFCPNSimulationResults::SaveODE(wxCommandEvent& p_cEvent)
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

	if (!InitializeSimulator(0,0))
	{
		SP_MESSAGEBOX(wxT("Error: Can not Initialize Place ODE"));
		return;
	}

	wxString l_sResult = wxT("");

	for (unsigned i = 0; i < m_msPlaceNames.size(); i++)
	{
		wxString l_sName = m_msPlaceNames[i];
		l_sResult << wxT("d") << l_sName << wxT("/dt = ");

		l_sResult << (dynamic_cast<spsim::ContinuousSimulator*>(m_pcMainSimulator))->GetPlaceODEString(i) << wxT("\n\n");

	}

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

void SP_DLG_ColFCPNSimulationResults::LoadColAuxResults()
{
	GetColPlaceResults(m_pcMainSimulator->GetResultMatrix(), m_aanColPlaceResults);
	GetColTransResults(m_pcMainSimulator->GetRateResultMatrix(), m_aanColTransResults);

	if (!m_bComAuxVarSingleRun)
	{
		ComputeAuxiliaryVars();
	}
}

bool SP_DLG_ColFCPNSimulationResults::ComputeAuxiliaryVars()
{
	m_aanAuxPLVarsResults.clear();
	m_vAuxPLVars.clear();
	SP_DS_ColTraceAnalyzer l_cTraceAnalyzer(m_pcUnfoldedNet, m_pcMainSimulator->GetResultMatrix(), m_aanAuxPLVarsResults, m_vAuxPLVars);

	if (!l_cTraceAnalyzer.Compute())
		return false;

	return true;
}


void SP_DLG_ColFCPNSimulationResults::InitializeEmptyView(SP_DS_Metadata* p_pcView)
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


void SP_DLG_ColFCPNSimulationResults::UpdateChoices()//by george for constant harmonizing
{
	m_choicesForColPNs.clear();
	//l_pcGraph->GetMetadataclass();
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);// SP_DS_CONSTANTS_HARMONIZING
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

void SP_DLG_ColFCPNSimulationResults::LoadTransitions()
{
	m_pcMainSimulator->ClearTransitions();

	m_pcMainSimulator->SetTransitionCount(m_pcUnfoldedNet->GetTransitionNames()->size());

	SP_VectorStdString* l_pcTransitionName = m_pcUnfoldedNet->GetTransitionNames();
	SP_VectorString* l_pcTransitionFunction = m_pcUnfoldedNet->GetCurRateFunction();

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
		wxString l_sTName;
		wxString l_sTfun;
		l_sTName = (*l_pcTransitionName)[l_nTransPos];
		l_sTfun = (*l_pcTransitionFunction)[l_nTransPos];
		m_pcMainSimulator->AddTransition(l_sTName,

			l_sTfun,
		//m_pcMainSimulator->AddTransition((*l_pcTransitionName)[l_nTransPos],

		//	(*l_pcTransitionFunction)[l_nTransPos],

			spsim::TRANSITION_TYPE_CONTINUOUS);
	}
}

void SP_DLG_ColFCPNSimulationResults::LoadConnections()
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

		SP_LOGMESSAGE(l_itConnection->m_sMultiplicity);

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


void SP_DLG_ColFCPNSimulationResults::LoadPlaces()
{
	m_pcMainSimulator->SetPlaceCount(m_pcUnfoldedNet->GetPlaceNames()->size());

	m_pcMainSimulator->SetPlaceNames(*m_pcUnfoldedNet->GetPlaceNames());

	SP_VectorBool l_abIsFixed(m_pcUnfoldedNet->GetPlaceNames()->size(), false);
	unsigned long l_nPosition = 0;
	for (auto const& it : m_pcUnfoldedNet->GetUnfoldedContPlaces())
	{
		for (auto const& it2 : it.second)
		{
			l_abIsFixed[l_nPosition] = it2.second.m_bFixed;
			++l_nPosition;
		}
	}

	//set fixed flag
	(dynamic_cast<spsim::ContinuousSimulator*>(m_pcMainSimulator))->SetFixedFlag(l_abIsFixed);
	//std::vector<double>* l_vInitialMarking =*m_pcUnfoldedNet->GetCurCPNMarking();
	(dynamic_cast<spsim::ContinuousSimulator*>(m_pcMainSimulator))->SetInitialMarking(*m_pcUnfoldedNet->GetCurCPNMarking());
	 
}


spsim::ConnectionType SP_DLG_ColFCPNSimulationResults::GetConnectionType(const wxString& p_sConnectionType)
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

void SP_DLG_ColFCPNSimulationResults::UpdateXAxisValues()
{
	CHECK_POINTER(m_pcCurrentTablePlot, return);

	CalculateXAxisValues(m_pcCurrentTablePlot, m_anXAxisValues);
}


void SP_DLG_ColFCPNSimulationResults::CalculateXAxisValues(SP_DS_Metadata* p_pcView, SP_VectorDouble& p_anXAxisValues)
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
				if (l_sCategory.IsSameAs(wxT("Place-uncolored")) && !l_sSubCategory.IsEmpty())
				{
					unsigned long l_nPlacePosition = m_mnPlaceArrayPos_FromUnfolding[l_sSubCategory];

					//number of rows in the result matrix
					unsigned long l_nRowCount = m_pcMainSimulator->GetResultMatrix().size();

					for (unsigned long l_nPoint = 0; l_nPoint < l_nRowCount; l_nPoint++)
					{
						double l_nValue = m_pcMainSimulator->GetResultMatrix()[l_nPoint][l_nPlacePosition];

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

						for (unsigned long l_nPoint = 0; l_nPoint < m_pcMainSimulator->GetGeneratedResultPointsCount(); l_nPoint++)
						{
							p_anXAxisValues.push_back(l_nStartOutputPoint + l_nTime);

							l_nTime += m_pcMainSimulator->GetOutputSampleSize();
						}

					}
}


void SP_DLG_ColFCPNSimulationResults::AddNewMarkingSet(const wxString& p_sMarkingSetName, const int& p_nRow)
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

		if (!LoadUnfoldedPlaces())
		{
			SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
			return;
		}
	}
}

bool SP_DLG_ColFCPNSimulationResults::IsMarkingSetNameExist(const wxString& p_sName)
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


void SP_DLG_ColFCPNSimulationResults::DirectExportToCSV()
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
			if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Place"))) //unfolded place
			{
				l_dResult = l_aanPLResults[l_nRow][l_nPos];
			}
			else
				if (l_sOutType == wxT("Colored") && l_sElementType.IsSameAs(wxT("Place"))) //colored  place
				{
					l_dResult = m_aanColPlaceResults[l_nRow][l_nPos];
				}
				else
					if (l_sOutType == wxT("Auxiliary variables") && l_sElementType.IsSameAs(wxT("Place"))) //unfolded place
					{
						l_dResult = m_aanAuxPLVarsResults[l_nRow][l_nPos];
					}

					else
						if (l_sOutType == wxT("Unfolded") && l_sElementType.IsSameAs(wxT("Transition"))) //unfolded place
						{
							l_dResult = l_aanTRResults[l_nRow][l_nPos];
						}

						else
							if (l_sOutType == wxT("Colored") && l_sElementType.IsSameAs(wxT("Transition"))) //unfolded place
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


SP_DLG_ColFCPNSimulationResults::~SP_DLG_ColFCPNSimulationResults()
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

	wxDELETE(m_pcMarkingSetComboBox);
	wxDELETE(m_pcFunctionSetComboBox);
	//wxDELETE(m_pcParameterSetComboBox);
	//wxDELETE(m_pcSolver);
	//wxDELETE(m_pcSolverType);
	//wxDELETE(m_pcSimulationProperites);
}


void SP_DLG_ColFCPNSimulationResults::OnModifyConstants(wxCommandEvent& p_cEvent)
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


void SP_DLG_ColFCPNSimulationResults::OnModifyFunctionSets(wxCommandEvent& p_cEvent)
{
	SP_DLG_ColStFunctionOverview* l_pcDlg = new SP_DLG_ColStFunctionOverview(SP_DS_CONTINUOUS_TRANS, this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		LoadSets();

		if (LoadUnfoldedPlaces())
		{
			if (LoadUnfoldingData())
			{
				m_bIsSimulatorInitialized = false;
				return;
			}
		}
		SP_MESSAGEBOX(wxT("Unfolding error"), wxT("Unfolding checking"), wxOK | wxICON_ERROR);
	}
}

//
void SP_DLG_ColFCPNSimulationResults::OnFunctionSetChanged(wxCommandEvent& p_cEvent)
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
