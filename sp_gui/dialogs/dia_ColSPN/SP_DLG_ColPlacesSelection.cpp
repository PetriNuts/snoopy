//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2011/08/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>
#include <wx/tipwin.h>

#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColPlacesSelection.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"

#include "sp_gui/dialogs/dia_ColSPN/TraceAnalyzer/SP_DLG_ColAuxiliaryVariable.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_ColStSimulationResults.h"
#include "sp_gui/dialogs/dia_ColHPN/SP_DLG_ColHPNSimultionResults.h"
#include "sp_gui/dialogs/dia_ColCPN/SP_DLG_ColCPNSimulationResults.h"


IMPLEMENT_CLASS( SP_DLG_ColPlacesSelection, SP_DLG_PlacesSelection )

enum
{
	SP_ID_BUTTON_LF2RT,
	SP_ID_OUTPUTTYPERADIOBOX
	
};

BEGIN_EVENT_TABLE( SP_DLG_ColPlacesSelection, SP_DLG_PlacesSelection )
	
END_EVENT_TABLE()



SP_DLG_ColPlacesSelection::SP_DLG_ColPlacesSelection(SP_DLG_Simulation* p_pcColSimulationResults,  wxString p_sNodeType, 
													 SP_DS_Metadata* p_pcEditMetadata, wxWindow* p_pcParent,const wxString& p_sTitle, long p_nStyle) 
		:SP_DLG_PlacesSelection(p_sNodeType,p_pcEditMetadata,p_pcParent,p_sTitle,p_nStyle),
		m_pcColSimulationResults(p_pcColSimulationResults)		
{

	m_pcEditMetadata = p_pcEditMetadata;

	Initialize();	

	InitilizeFromMetaData();

	//initialize
	if( m_sNodeType == wxT("Place") )
	{
		for(unsigned i = 0; i < m_pcUnfoldedNet->GetColoredPlaceNames()->size(); i++)
		{
			m_pcChooseColoredPlace->Append( (*(m_pcUnfoldedNet->GetColoredPlaceNames()))[i]);
		}
		m_pcChooseColoredPlace->SetSelection(0);
	}
	else
	{
		for(unsigned i = 0; i < m_pcUnfoldedNet->GetColoredTransitionNames()->size(); i++)
		{
			m_pcChooseColoredPlace->Append( (*(m_pcUnfoldedNet->GetColoredTransitionNames()))[i]);
		}
		m_pcChooseColoredPlace->SetSelection(0);
	}

	LoadData();

	Layout();	

}

void SP_DLG_ColPlacesSelection::Initialize()
{
	if(!m_pcColSimulationResults)
		return;

	m_sNetClass = m_pcGraph->GetNetclass()->GetName();	

	vector<wxString>* l_vAuxVar;
	if(m_sNetClass == SP_DS_COLSPN_CLASS )
	{
		m_pcUnfoldedNet = dynamic_cast<SP_DLG_ColStSimulationResults*>(m_pcColSimulationResults)->GetUnfoldedNet(); 
		l_vAuxVar =  dynamic_cast<SP_DLG_ColStSimulationResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
	}
	else if (m_sNetClass == SP_DS_COLCPN_CLASS )
	{
		m_pcUnfoldedNet = dynamic_cast<SP_DLG_ColCPNSimulationResults*>(m_pcColSimulationResults)->GetUnfoldedNet(); 
		l_vAuxVar =  dynamic_cast<SP_DLG_ColCPNSimulationResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
	}
	else if (m_sNetClass == SP_DS_COLHPN_CLASS )
	{
		m_pcUnfoldedNet = dynamic_cast<SP_DLG_ColHPNSimultionResults*>(m_pcColSimulationResults)->GetUnfoldedNet(); 
		l_vAuxVar =  dynamic_cast<SP_DLG_ColHPNSimultionResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
	}
	else
	{
		//NOT SUPPORT
		return;
	}

	for(unsigned int i= 0; i < l_vAuxVar->size(); i++)
	{
		wxString l_sPlaceName = (*l_vAuxVar)[i];
		m_ArrayPlaceAuxVar.Add(l_sPlaceName);
	}

	
	if(!m_pcUnfoldedNet)
		return;

	m_pvUnfoldedPlace = m_pcUnfoldedNet->GetPlaceNames();
	m_pvColoredPlace = m_pcUnfoldedNet->GetColoredPlaceNames();
	

	m_pvUnfoldedTrans = m_pcUnfoldedNet->GetTransitionNames();
	m_pvColoredTrans = m_pcUnfoldedNet->GetColoredTransitionNames();

	m_ArrayUnPlaces.clear();
	m_ArrayColPlaces.clear();
	m_ArrayUnTranstions.clear();
	m_ArrayColTransitions.clear();
	
	m_mPlaceName2ID.clear();
	m_mTransName2ID.clear();
	m_mColPlaceName2ID.clear();
	m_mColTransName2ID.clear();

	for(unsigned int i= 0; i < m_pvUnfoldedPlace->size(); i++)
	{
		wxString l_sPlaceName = (*m_pvUnfoldedPlace)[i];
		m_ArrayUnPlaces.Add(l_sPlaceName);
		m_mPlaceName2ID[l_sPlaceName] = i;
	}

	for(unsigned int i= 0; i < m_pvColoredPlace->size(); i++)
	{
		wxString l_sPlaceName = (*m_pvColoredPlace)[i];
		m_ArrayColPlaces.Add(l_sPlaceName);
		m_mColPlaceName2ID[l_sPlaceName] = i;
	}


	for(unsigned int i= 0; i < m_pvUnfoldedTrans->size(); i++)
	{
		wxString l_sTransName = (*m_pvUnfoldedTrans)[i];
		m_ArrayUnTranstions.Add(l_sTransName);
		m_mTransName2ID[l_sTransName] = i;
	}

	for(unsigned int i= 0; i < m_pvColoredTrans->size(); i++)
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

void SP_DLG_ColPlacesSelection::LoadData()
{	
	m_ArrayString.clear();
	m_ArrayString = m_msaCurLBInStrings[m_sNodeType+wxT(":")+m_sOutputType];

	m_pListBoxIn->Clear();
	if (!m_ArrayString.IsEmpty())
	{
	  m_pListBoxIn->InsertItems(m_ArrayString,0);
    }

}




void SP_DLG_ColPlacesSelection::OnEditAuxVariable(wxCommandEvent& p_cEvent )
{
	SP_DLG_ColAuxiliaryVariable* l_pcDlg = new SP_DLG_ColAuxiliaryVariable(this);

	if (l_pcDlg->ShowModal() == wxID_OK)
	{
		m_bComSingleRun = l_pcDlg->GetComSingleRunFlag();
		m_pcColSimulationResults->SetComAuxVarSingleRun(m_bComSingleRun);

		m_ArrayPlaceAuxVar.clear(); 
		vector<wxString>* l_vAuxVar;

		if(m_sNetClass == SP_DS_COLSPN_CLASS )
		{
			dynamic_cast<SP_DLG_ColStSimulationResults*>(m_pcColSimulationResults)->ComputeAuxiliaryVars();
			l_vAuxVar =  dynamic_cast<SP_DLG_ColStSimulationResults*>(m_pcColSimulationResults)->GetAuxPLVariables();			
			
		}
		else if (m_sNetClass == SP_DS_COLCPN_CLASS )
		{
			dynamic_cast<SP_DLG_ColCPNSimulationResults*>(m_pcColSimulationResults)->ComputeAuxiliaryVars(); 	
			l_vAuxVar =  dynamic_cast<SP_DLG_ColCPNSimulationResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
		}
		else if (m_sNetClass == SP_DS_COLHPN_CLASS )
		{
			dynamic_cast<SP_DLG_ColHPNSimultionResults*>(m_pcColSimulationResults)->ComputeAuxiliaryVars(); 
			l_vAuxVar =  dynamic_cast<SP_DLG_ColHPNSimultionResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
		}
		else
		{
			//NOT SUPPORT
			return;
		}

		for(unsigned int i= 0; i < l_vAuxVar->size(); i++)
		{
			wxString l_sPlaceName = (*l_vAuxVar)[i];
			m_ArrayPlaceAuxVar.Add(l_sPlaceName);
		}
	}
	l_pcDlg->Destroy();

	m_msaCurLBInStrings[wxT("Place:Auxiliary variables")] = m_ArrayPlaceAuxVar;

	LoadData();	
	
}

void SP_DLG_ColPlacesSelection::GetSelResults()
{	

	m_vmSelectedNodes.clear();

	wxArrayInt l_Selections;
	wxString l_SelectionItem;

	for(unsigned int Counter = 0; Counter <m_pListBoxOut->GetCount(); Counter++ )
	{
		l_SelectionItem = m_pListBoxOut->GetString(Counter);	
		
		SP_ColPN_Position_OutputType l_Pos_Type;
		long l_nPos;
		wxString l_OutType;
		bool l_bNotFound = true;;

		if( m_sNodeType == wxT("Place") )
		{
			if(l_bNotFound)
			{
				for( unsigned int i = 0; i < m_ArrayPlaceAuxVar.size(); i++ )
				{
					wxString l_sPlace = m_ArrayPlaceAuxVar[i];
					if(l_sPlace == l_SelectionItem)
					{
						l_bNotFound = false;
						l_nPos = (long)i;
						l_OutType = wxT("Auxiliary variables");
						break;
					}
				}
			}
			if(l_bNotFound)
			{
				for( unsigned int i = 0; i < m_pvColoredPlace->size(); i++ )
				{
					wxString l_sPlace = (*m_pvColoredPlace)[i];
					if(l_sPlace == l_SelectionItem)
					{
						l_bNotFound = false;
						l_nPos = (long)i;
						l_OutType = wxT("Colored");
						break;
					}
				}
			}
			if(l_bNotFound)
			{
				for( unsigned int i = 0; i < m_pvUnfoldedPlace->size(); i++ )
				{
					wxString l_sPlace = (*m_pvUnfoldedPlace)[i];
					if(l_sPlace == l_SelectionItem)
					{
						l_bNotFound = false;
						l_nPos = (long)i;
						l_OutType = wxT("Unfolded");
						break;
					}
				}
			}
		}

		if( m_sNodeType == wxT("Transition") )
		{
			if(l_bNotFound)
			{
				for( unsigned int i = 0; i < m_pvColoredTrans->size(); i++ )
				{
					wxString l_sPlace = (*m_pvColoredTrans)[i];
					if(l_sPlace == l_SelectionItem)
					{
						l_bNotFound = false;
						l_nPos = (long)i;
						l_OutType = wxT("Colored");
						break;
					}
				}
			}
			if(l_bNotFound)
			{
				for( unsigned int i = 0; i < m_pvUnfoldedTrans->size(); i++ )
				{
					wxString l_sPlace = (*m_pvUnfoldedTrans)[i];
					if(l_sPlace == l_SelectionItem)
					{
						l_bNotFound = false;
						l_nPos = (long)i;
						l_OutType = wxT("Unfolded");
						break;
					}
				}
			}
		}	
		l_Pos_Type.m_nPosition = l_nPos;
		l_Pos_Type.m_OutputType = l_OutType;
		l_Pos_Type.m_sName=m_pListBoxOut->GetString(Counter);
		m_vmSelectedNodes.push_back(l_Pos_Type);
	}				
	
}

void SP_DLG_ColPlacesSelection::OnChangedOutType(wxCommandEvent& p_cEvent)
{
	m_msaCurLBInStrings[m_sNodeType+wxT(":")+m_sOutputType] = m_pListBoxIn->GetStrings();

	if(m_pcOutputTypeRadioBox->GetSelection() == 0)
	{
		m_sOutputType = wxT("Unfolded");
	}
	else if(m_pcOutputTypeRadioBox->GetSelection() == 1)
	{
		m_sOutputType = wxT("Colored");
	}
	else
	{
		m_sOutputType = wxT("Auxiliary variables");
	}
	
	LoadData();

}





void SP_DLG_ColPlacesSelection::OnChangedPlaceTransition(wxCommandEvent& p_cEvent)
{
	m_msaCurLBInStrings[m_sNodeType+wxT(":")+m_sOutputType] = m_pListBoxIn->GetStrings();

	if(m_pcPlaceTransitionRadioBox->GetSelection() == 0 )
	{
		m_sNodeType = wxT("Place");

		m_pcOutputTypeRadioBox->Show(2,true);

		m_msaCurLBInStrings[wxT("Place:Unfolded")] = m_ArrayUnPlaces;
		m_msaCurLBInStrings[wxT("Place:Colored")] = m_ArrayColPlaces;
		m_msaCurLBInStrings[wxT("Place:Auxiliary variables")] = m_ArrayPlaceAuxVar;

		m_pcChooseColoredPlace->Clear();
		m_pcChooseColoredPlace->Append(wxT(""));
		for(unsigned i = 0; i < m_pcUnfoldedNet->GetColoredPlaceNames()->size(); i++)
		{
			m_pcChooseColoredPlace->Append( (*(m_pcUnfoldedNet->GetColoredPlaceNames()))[i]);
		}
		m_pcChooseColoredPlace->SetSelection(0);	
		
	}
	else
	{
		m_sNodeType = wxT("Transition");

		m_pcOutputTypeRadioBox->Show(2,false);	

		m_msaCurLBInStrings[wxT("Transition:Unfolded")] = m_ArrayUnTranstions;
		m_msaCurLBInStrings[wxT("Transition:Colored")] = m_ArrayColTransitions;
		m_msaCurLBInStrings[wxT("Transition:Auxiliary variables")] = m_ArrayTransAuxVar;

		m_pcChooseColoredPlace->Clear();
		m_pcChooseColoredPlace->Append(wxT(""));
		for(unsigned i = 0; i < m_pcUnfoldedNet->GetColoredTransitionNames()->size(); i++)
		{
			m_pcChooseColoredPlace->Append( (*(m_pcUnfoldedNet->GetColoredTransitionNames()))[i]);
		}
		m_pcChooseColoredPlace->SetSelection(0);
		
	}

	m_pListBoxOut->Clear();
	
	m_pcPlaceChoiceStaticBox->SetLabel(wxT("The overall ") + m_sNodeType);
	m_pcFourthColumStaticBox->SetLabel(wxT("The selected ") + m_sNodeType);
		
	LoadData();
}

void SP_DLG_ColPlacesSelection::AddtoCurLBInStrings(wxString p_sName)
{	
	wxString l_sOutputType;

	if(m_sNodeType == wxT("Place") )
	{
		if( m_mPlaceName2ID.find(p_sName) != m_mPlaceName2ID.end() )
			l_sOutputType = wxT("Unfolded");
		else if( m_mColPlaceName2ID.find(p_sName) != m_mColPlaceName2ID.end() )
			l_sOutputType = wxT("Colored");
		else
		{
			bool l_bNotFound = true;
			for( unsigned int i = 0; i < m_ArrayPlaceAuxVar.size(); i++ )
			{
				wxString l_sPlace = m_ArrayPlaceAuxVar[i];
				if(p_sName == l_sPlace)
				{
					l_bNotFound = false;				
					break;
				}
			}
			if(!l_bNotFound)
				l_sOutputType = wxT("Auxiliary variables");
		}
	}
	
	
	if(m_sNodeType == wxT("Transition"))
	{
		if( m_mTransName2ID.find(p_sName) != m_mTransName2ID.end() )
			l_sOutputType = wxT("Unfolded");

		else if( m_mColTransName2ID.find(p_sName) != m_mColTransName2ID.end() )
			l_sOutputType = wxT("Colored");
	}

	map<wxString, wxArrayString>::iterator itMap;
		
	itMap = m_msaCurLBInStrings.find( m_sNodeType+wxT(":")+l_sOutputType);

	if(itMap != m_msaCurLBInStrings.end())
		itMap->second.Add(p_sName);
	
}

void SP_DLG_ColPlacesSelection::OnColPlaceSel( wxCommandEvent& p_cEvent )
{
	wxString l_sSelColPlace = m_pcChooseColoredPlace->GetValue();

	if(l_sSelColPlace == wxT(""))
		return;

	for(unsigned int Counter = 0; Counter <m_pListBoxIn->GetCount(); Counter++ )
	{
		wxString l_SelectionItem = m_pListBoxIn->GetString(Counter);

		wxString l_sSubName = l_SelectionItem.Mid(0,l_sSelColPlace.size()+1);			
		if(l_sSubName == l_sSelColPlace+wxT("_") )
		{
			if( m_sNodeType == wxT("Place") && m_sOutputType == wxT("Unfolded") )
			{
			}
			else if( m_sNodeType == wxT("Transition") && m_sOutputType == wxT("Unfolded") )
			{
			}
			else
			{
				return;
			}

			map<wxString, wxArrayString>::iterator itMap;		
			itMap = m_msaCurLBInStrings.find( m_sNodeType+wxT(":")+m_sOutputType);
			if(itMap != m_msaCurLBInStrings.end())
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