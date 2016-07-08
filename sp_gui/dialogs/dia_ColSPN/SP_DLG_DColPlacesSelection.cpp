

#include <wx/valgen.h>
#include <wx/tipwin.h>

#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_DColPlacesSelection.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"

#include "sp_gui/dialogs/dia_ColSPN/TraceAnalyzer/SP_DLG_ColAuxiliaryVariable.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStSimulationResults.h"
#include "sp_gui/dialogs/dia_ColHPN/SP_DLG_ColHPNSimultionResults.h"
#include "sp_gui/dialogs/dia_ColCPN/SP_DLG_ColCPNDirSimulationResults.h"


IMPLEMENT_CLASS(SP_DLG_DColPlacesSelection, SP_DLG_DPlacesSelection)

enum
{
	SP_ID_BUTTON_LF2RT,
	SP_ID_OUTPUTTYPERADIOBOX

};

BEGIN_EVENT_TABLE(SP_DLG_DColPlacesSelection, SP_DLG_DPlacesSelection)

END_EVENT_TABLE()



SP_DLG_DColPlacesSelection::SP_DLG_DColPlacesSelection(SP_DLG_DirColorSimulation* p_pcColSimulationResults,
	SP_DS_Metadata* p_pcEditMetadata, wxWindow* p_pcParent, const wxString& p_sTitle, long p_nStyle)
	:SP_DLG_DPlacesSelection(p_pcEditMetadata, p_pcParent, p_sTitle, p_nStyle),
	m_pcColSimulationResults(p_pcColSimulationResults)
{

	m_pcEditMetadata = p_pcEditMetadata;

	Initialize();

	InitilizeFromMetaData();

	//initialize
	if (m_sNodeType == wxT("Place"))
	{
		for (unsigned i = 0; i < m_pcColoredNet->GetColoredPlaceNames()->size(); i++)
		{
			m_pcChooseColoredPlace->Append((*(m_pcColoredNet->GetColoredPlaceNames()))[i]);
		}
		m_pcChooseColoredPlace->SetSelection(0);
	}
	else
	{
		for (unsigned i = 0; i < m_pcColoredNet->GetColoredTransitionNames()->size(); i++)
		{
			m_pcChooseColoredPlace->Append((*(m_pcColoredNet->GetColoredTransitionNames()))[i]);
		}
		m_pcChooseColoredPlace->SetSelection(0);
	}

	LoadData();

	Layout();

}

void SP_DLG_DColPlacesSelection::Initialize()
{
	if (!m_pcColSimulationResults)
		return;
	/*SP_DS_Nodeclass* l_pcNodeclass;
	SP_ListNode::const_iterator l_itElem;
	long l_nPos = 0;*/
	m_sNetClass = m_pcGraph->GetNetclass()->GetName();

	vector<wxString>* l_vAuxVar;
	if (m_sNetClass == SP_DS_COLSPN_CLASS)
	{
		//m_pcColoredNet = dynamic_cast<SP_DLG_ColStSimulationResults*>(m_pcColSimulationResults)->GetUnfoldedNet();
		//l_vAuxVar = dynamic_cast<SP_DLG_ColStSimulationResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
	}
	else if (m_sNetClass == SP_DS_COLCPN_CLASS)
	{
		m_pcColoredNet = dynamic_cast<SP_DLG_ColCPNDirSimulationResults*>(m_pcColSimulationResults)->GetColoredNet();
		l_vAuxVar = dynamic_cast<SP_DLG_ColCPNDirSimulationResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
	}
	else if (m_sNetClass == SP_DS_COLHPN_CLASS)
	{
		
	/*	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Node* l_pcNode = *l_itElem;
			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			m_mPlaceID2Name[l_nPos] = l_sPlaceName;
			m_mPlaceName2ID[l_sPlaceName] = l_nPos;
			m_ArrayUnPlaces.Add(l_sPlaceName);
			l_nPos++;
		}*/
		
		//m_pcColoredNet = dynamic_cast<SP_DLG_ColHPNSimultionResults*>(m_pcColSimulationResults)->GetUnfoldedNet();
		//l_vAuxVar = dynamic_cast<SP_DLG_ColHPNSimultionResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
	}
	else
	{
		//NOT SUPPORT
		return;
	}

	for (unsigned int i = 0; i < l_vAuxVar->size(); i++)
	{
		wxString l_sPlaceName = (*l_vAuxVar)[i];
		m_ArrayPlaceAuxVar.Add(l_sPlaceName);
	}


	if (!m_pcColoredNet)
		return;

	m_pvUncoloredPlace = m_pcColoredNet->GetPlaceNames();
	m_pvColoredPlace = m_pcColoredNet->GetColoredPlaceNames();


	//m_pvUnfoldedTrans = m_pcColoredNet->GetTransitionNames();
	m_pvColoredTrans = m_pcColoredNet->GetColoredTransitionNames();

	m_ArrayUnPlaces.clear();
	m_ArrayColPlaces.clear();
	m_ArrayUnTranstions.clear();
	m_ArrayColTransitions.clear();

	m_mPlaceName2ID.clear();
	m_mTransName2ID.clear();
	m_mColPlaceName2ID.clear();
	m_mColTransName2ID.clear();

	for (unsigned int i = 0; i < m_pvUncoloredPlace->size(); i++)
	{
		wxString l_sPlaceName = (*m_pvUncoloredPlace)[i];
		m_ArrayUnPlaces.Add(l_sPlaceName);
		m_mPlaceName2ID[l_sPlaceName] = i;
	}

	for (unsigned int i = 0; i < m_pvColoredPlace->size(); i++)
	{
		wxString l_sPlaceName = (*m_pvColoredPlace)[i];
		m_ArrayColPlaces.Add(l_sPlaceName);
		m_mColPlaceName2ID[l_sPlaceName] = i;
	}


	/*for (unsigned int i = 0; i < m_pvUnfoldedTrans->size(); i++)
	{
		wxString l_sTransName = (*m_pvUnfoldedTrans)[i];
		m_ArrayUnTranstions.Add(l_sTransName);
		m_mTransName2ID[l_sTransName] = i;
	}*/

	for (unsigned int i = 0; i < m_pvColoredTrans->size(); i++)
	{
		wxString l_sTransName = (*m_pvColoredTrans)[i];
		m_ArrayColTransitions.Add(l_sTransName);
		m_mColTransName2ID[l_sTransName] = i;
	}
	m_msaCurLBInStrings[wxT("Place:Unfolded")] = m_ArrayUnPlaces;
	m_msaCurLBInStrings[wxT("Place:Colored")] = m_ArrayColPlaces;
	m_msaCurLBInStrings[wxT("Place:Auxiliary variables")] = m_ArrayPlaceAuxVar;
	m_msaCurLBInStrings[wxT("Transition:Unfolded")] = m_ArrayUnTranstions;
	m_msaCurLBInStrings[wxT("Transition:Colored")] = m_ArrayColTransitions;
	m_msaCurLBInStrings[wxT("Transition:Auxiliary variables")] = m_ArrayTransAuxVar;

}

void SP_DLG_DColPlacesSelection::LoadData()
{
	m_ArrayString.clear();
	m_ArrayString = m_msaCurLBInStrings[m_sNodeType + wxT(":") + m_sOutputType];

	m_pListBoxIn->Clear();
	if (!m_ArrayString.IsEmpty())
	{
		m_pListBoxIn->InsertItems(m_ArrayString, 0);
	}

	NodesThroughRegEx();
}




void SP_DLG_DColPlacesSelection::OnEditAuxVariable(wxCommandEvent& p_cEvent)
{
	SP_DLG_ColAuxiliaryVariable* l_pcDlg = new SP_DLG_ColAuxiliaryVariable(this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		m_bComSingleRun = l_pcDlg->GetComSingleRunFlag();
		m_pcColSimulationResults->SetComAuxVarSingleRun(m_bComSingleRun);

		m_ArrayPlaceAuxVar.clear();
		vector<wxString>* l_vAuxVar;

		if (m_sNetClass == SP_DS_COLSPN_CLASS)
		{
			dynamic_cast<SP_DLG_ColStSimulationResults*>(m_pcColSimulationResults)->ComputeAuxiliaryVars();
			l_vAuxVar = dynamic_cast<SP_DLG_ColStSimulationResults*>(m_pcColSimulationResults)->GetAuxPLVariables();

		}
		else if (m_sNetClass == SP_DS_COLCPN_CLASS)
		{
			dynamic_cast<SP_DLG_ColCPNDirSimulationResults*>(m_pcColSimulationResults)->ComputeAuxiliaryVars();
			l_vAuxVar = dynamic_cast<SP_DLG_ColCPNDirSimulationResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
		}
		else if (m_sNetClass == SP_DS_COLHPN_CLASS)
		{
			dynamic_cast<SP_DLG_ColHPNSimultionResults*>(m_pcColSimulationResults)->ComputeAuxiliaryVars();
			l_vAuxVar = dynamic_cast<SP_DLG_ColHPNSimultionResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
		}
		else
		{
			//NOT SUPPORT
			return;
		}

		for (unsigned int i = 0; i < l_vAuxVar->size(); i++)
		{
			wxString l_sPlaceName = (*l_vAuxVar)[i];
			m_ArrayPlaceAuxVar.Add(l_sPlaceName);
		}
	}
	l_pcDlg->Destroy();

	m_msaCurLBInStrings[wxT("Place:Auxiliary variables")] = m_ArrayPlaceAuxVar;

	LoadData();

}

void SP_DLG_DColPlacesSelection::GetSelResults()
{

	m_vmSelectedNodes.clear();

	wxArrayInt l_Selections;
	wxString l_SelectionItem;

	for (unsigned int Counter = 0; Counter <m_pListBoxOut->GetCount(); Counter++)
	{
		l_SelectionItem = m_pListBoxOut->GetString(Counter);

		SP_ColPN_Position_DOutputType l_Pos_Type;
		long l_nPos;
		wxString l_OutType;
		bool l_bNotFound = true;;

		if (m_sNodeType == wxT("Place"))
		{
			if (l_bNotFound)
			{
				for (unsigned int i = 0; i < m_ArrayPlaceAuxVar.size(); i++)
				{
					wxString l_sPlace = m_ArrayPlaceAuxVar[i];
					if (l_sPlace == l_SelectionItem)
					{
						l_bNotFound = false;
						l_nPos = (long)i;
						l_OutType = wxT("Auxiliary variables");
						break;
					}
				}
			}
			if (l_bNotFound)
			{
				for (unsigned int i = 0; i < m_pvColoredPlace->size(); i++)
				{
					wxString l_sPlace = (*m_pvColoredPlace)[i];
					if (l_sPlace == l_SelectionItem)
					{
						l_bNotFound = false;
						l_nPos = (long)i;
						l_OutType = wxT("Colored");
						break;
					}
				}
			}
			if (l_bNotFound)
			{
				for (unsigned int i = 0; i < m_pvUncoloredPlace->size(); i++)
				{
					wxString l_sPlace = (*m_pvUncoloredPlace)[i];
					if (l_sPlace == l_SelectionItem)
					{
						l_bNotFound = false;
						l_nPos = (long)i;
						l_OutType = wxT("Unfolded");
						break;
					}
				}
			}
		}

		if (m_sNodeType == wxT("Transition"))
		{
			if (l_bNotFound)
			{
				for (unsigned int i = 0; i < m_pvColoredTrans->size(); i++)
				{
					wxString l_sPlace = (*m_pvColoredTrans)[i];
					if (l_sPlace == l_SelectionItem)
					{
						l_bNotFound = false;
						l_nPos = (long)i;
						l_OutType = wxT("Colored");
						break;
					}
				}
			}
			/*if (l_bNotFound)
			{
				for (unsigned int i = 0; i < m_pvUnfoldedTrans->size(); i++)
				{
					wxString l_sPlace = (*m_pvUnfoldedTrans)[i];
					if (l_sPlace == l_SelectionItem)
					{
						l_bNotFound = false;
						l_nPos = (long)i;
						l_OutType = wxT("Unfolded");
						break;
					}
				}
			}*/
		}
		l_Pos_Type.m_nPosition = l_nPos;
		l_Pos_Type.m_OutputType = l_OutType;
		l_Pos_Type.m_sName = m_pListBoxOut->GetString(Counter);
		m_vmSelectedNodes.push_back(l_Pos_Type);
	}

}

void SP_DLG_DColPlacesSelection::OnChangedOutType(wxCommandEvent& p_cEvent)
{
	m_msaCurLBInStrings[m_sNodeType + wxT(":") + m_sOutputType] = m_pListBoxIn->GetStrings();

	if (m_pcOutputTypeRadioBox->GetSelection() == 0)
	{
		m_sOutputType = wxT("Unfolded");
	}
	else if (m_pcOutputTypeRadioBox->GetSelection() == 1)
	{
		m_sOutputType = wxT("Colored");
	}
	else
	{
		m_sOutputType = wxT("Auxiliary variables");
	}

	LoadData();

}





void SP_DLG_DColPlacesSelection::OnChangedPlaceTransition(wxCommandEvent& p_cEvent)
{
	m_msaCurLBInStrings[m_sNodeType + wxT(":") + m_sOutputType] = m_pListBoxIn->GetStrings();

	if (m_pcPlaceTransitionRadioBox->GetSelection() == 0)
	{
		m_sNodeType = wxT("Place");

		m_pcOutputTypeRadioBox->Show(2, true);

		m_msaCurLBInStrings[wxT("Place:Unfolded")] = m_ArrayUnPlaces;
		m_msaCurLBInStrings[wxT("Place:Colored")] = m_ArrayColPlaces;
		m_msaCurLBInStrings[wxT("Place:Auxiliary variables")] = m_ArrayPlaceAuxVar;

		m_pcChooseColoredPlace->Clear();
		m_pcChooseColoredPlace->Append(wxT(""));
		for (unsigned i = 0; i < m_pcColoredNet->GetColoredPlaceNames()->size(); i++)
		{
			m_pcChooseColoredPlace->Append((*(m_pcColoredNet->GetColoredPlaceNames()))[i]);
		}
		m_pcChooseColoredPlace->SetSelection(0);

	}
	else
	{
		m_sNodeType = wxT("Transition");

		m_pcOutputTypeRadioBox->Show(2, false);

		m_msaCurLBInStrings[wxT("Transition:Unfolded")] = m_ArrayUnTranstions;
		m_msaCurLBInStrings[wxT("Transition:Colored")] = m_ArrayColTransitions;
		m_msaCurLBInStrings[wxT("Transition:Auxiliary variables")] = m_ArrayTransAuxVar;

		m_pcChooseColoredPlace->Clear();
		m_pcChooseColoredPlace->Append(wxT(""));
		for (unsigned i = 0; i < m_pcColoredNet->GetColoredTransitionNames()->size(); i++)
		{
			m_pcChooseColoredPlace->Append((*(m_pcColoredNet->GetColoredTransitionNames()))[i]);
		}
		m_pcChooseColoredPlace->SetSelection(0);

	}

	m_pListBoxOut->Clear();

	m_pcPlaceChoiceStaticBox->SetLabel(wxT("The overall ") + m_sNodeType);
	m_pcFourthColumStaticBox->SetLabel(wxT("The selected ") + m_sNodeType);

	LoadData();

}

void SP_DLG_DColPlacesSelection::AddtoCurLBInStrings(wxString p_sName)
{
	wxString l_sOutputType;

	if (m_sNodeType == wxT("Place"))
	{
		if (m_mPlaceName2ID.find(p_sName) != m_mPlaceName2ID.end())
			l_sOutputType = wxT("Unfolded");
		else if (m_mColPlaceName2ID.find(p_sName) != m_mColPlaceName2ID.end())
			l_sOutputType = wxT("Colored");
		else
		{
			bool l_bNotFound = true;
			for (unsigned int i = 0; i < m_ArrayPlaceAuxVar.size(); i++)
			{
				wxString l_sPlace = m_ArrayPlaceAuxVar[i];
				if (p_sName == l_sPlace)
				{
					l_bNotFound = false;
					break;
				}
			}
			if (!l_bNotFound)
				l_sOutputType = wxT("Auxiliary variables");
		}
	}


	if (m_sNodeType == wxT("Transition"))
	{
		if (m_mTransName2ID.find(p_sName) != m_mTransName2ID.end())
			l_sOutputType = wxT("Unfolded");

		else if (m_mColTransName2ID.find(p_sName) != m_mColTransName2ID.end())
			l_sOutputType = wxT("Colored");
	}

	map<wxString, wxArrayString>::iterator itMap;

	itMap = m_msaCurLBInStrings.find(m_sNodeType + wxT(":") + l_sOutputType);

	if (itMap != m_msaCurLBInStrings.end())
		itMap->second.Add(p_sName);

}

void SP_DLG_DColPlacesSelection::OnColPlaceSel(wxCommandEvent& p_cEvent)
{
	wxString l_sSelColPlace = m_pcChooseColoredPlace->GetStringSelection();

	if (l_sSelColPlace == wxT(""))
		return;

	for (unsigned int Counter = 0; Counter <m_pListBoxIn->GetCount(); Counter++)
	{
		wxString l_SelectionItem = m_pListBoxIn->GetString(Counter);

		wxString l_sSubName = l_SelectionItem.Mid(0, l_sSelColPlace.size() + 1);
		if (l_sSubName == l_sSelColPlace + wxT("_"))
		{
			if (m_sNodeType == wxT("Place") && m_sOutputType == wxT("Unfolded"))
			{
			}
			else if (m_sNodeType == wxT("Transition") && m_sOutputType == wxT("Unfolded"))
			{
			}
			else
			{
				return;
			}

			map<wxString, wxArrayString>::iterator itMap;
			itMap = m_msaCurLBInStrings.find(m_sNodeType + wxT(":") + m_sOutputType);
			if (itMap != m_msaCurLBInStrings.end())
			{
				itMap->second.Add(l_SelectionItem);
			}
			m_pListBoxOut->Insert(l_SelectionItem, m_pListBoxOut->GetCount());
		}
		else
		{
			continue;
		}
	}

	LoadData();

}
