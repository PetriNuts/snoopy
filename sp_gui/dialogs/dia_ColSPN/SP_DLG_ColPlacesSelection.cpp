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
//by george
#include "sp_gui/dialogs/dia_colFCPN/SP_DLG_ColFCPNSimulationResults.h"
#include "sp_gui/dialogs/dia_colFSPN/SP_DLG_ColFSPNSimulationResults.h"
#include "sp_gui/dialogs/dia_colFHPN/SP_DLG_ColFHPNSimulationResults.h"
IMPLEMENT_CLASS( SP_DLG_ColPlacesSelection, SP_DLG_PlacesSelection )

enum
{
	SP_ID_BUTTON_LF2RT,
	SP_ID_OUTPUTTYPERADIOBOX
	
};

BEGIN_EVENT_TABLE( SP_DLG_ColPlacesSelection, SP_DLG_PlacesSelection )
	
END_EVENT_TABLE()



SP_DLG_ColPlacesSelection::SP_DLG_ColPlacesSelection(SP_DLG_Simulation* p_pcColSimulationResults,
													 SP_DS_Metadata* p_pcEditMetadata, wxWindow* p_pcParent,const wxString& p_sTitle, long p_nStyle) 
		:SP_DLG_PlacesSelection(p_pcEditMetadata,p_pcParent,p_sTitle,p_nStyle),
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
	if(m_sNetClass == SP_DS_COLSPN_CLASS)
	{
		m_pcUnfoldedNet = dynamic_cast<SP_DLG_ColStSimulationResults*>(m_pcColSimulationResults)->GetUnfoldedNet(); 
		l_vAuxVar =  dynamic_cast<SP_DLG_ColStSimulationResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
	}
	else if (m_sNetClass == SP_DS_FUZZY_ColSPN_CLASS/*by george*/)
	{
		m_pcUnfoldedNet = dynamic_cast<SP_DLG_ColFSPNSimulationResults*>(m_pcColSimulationResults)->GetUnfoldedNet();
		l_vAuxVar = dynamic_cast<SP_DLG_ColFSPNSimulationResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
	}
	else if (m_sNetClass == SP_DS_COLCPN_CLASS)
	{
		m_pcUnfoldedNet = dynamic_cast<SP_DLG_ColCPNSimulationResults*>(m_pcColSimulationResults)->GetUnfoldedNet(); 
		l_vAuxVar =  dynamic_cast<SP_DLG_ColCPNSimulationResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
	}
	else if ( m_sNetClass == SP_DS_FUZZY_ColCPN_CLASS/*by george*/)
	{
		m_pcUnfoldedNet = dynamic_cast<SP_DLG_ColFCPNSimulationResults*>(m_pcColSimulationResults)->GetUnfoldedNet();
		l_vAuxVar = dynamic_cast<SP_DLG_ColFCPNSimulationResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
	}
	else if (m_sNetClass == SP_DS_COLHPN_CLASS)
	{
		m_pcUnfoldedNet = dynamic_cast<SP_DLG_ColHPNSimultionResults*>(m_pcColSimulationResults)->GetUnfoldedNet(); 
		l_vAuxVar =  dynamic_cast<SP_DLG_ColHPNSimultionResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
	}
	else if (  m_sNetClass == SP_DS_FUZZY_ColHPN_CLASS/*by george*/)
	{
		m_pcUnfoldedNet = dynamic_cast<SP_DLG_ColFHPNSimulationResults*>(m_pcColSimulationResults)->GetUnfoldedNet();
		l_vAuxVar = dynamic_cast<SP_DLG_ColFHPNSimulationResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
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

	//inistantiate the vectors
	m_pvUnfoldedPlaceObserver = new vector<wxString>;
	m_pvColoredPlaceObserver = new vector<wxString>;
	m_pvUnfoldedTransObserver = new vector<wxString>;
	m_pvColoredTransObserver = new vector<wxString>;
	m_pvMixedObserver = new vector<wxString>;

	m_ArrayUnPlaces.clear();
	m_ArrayColPlaces.clear();
	m_ArrayUnTranstions.clear();
	m_ArrayColTransitions.clear();
	//by george
	m_ArrayUnPlacesObserver.clear();
	m_ArrayColPlacesObserver.clear();
	m_ArrayUnTransObserver.clear();
	m_ArrayColTransObserver.clear();
	m_ArrayMixedObservers.clear();
	
	m_mPlaceName2ID.clear();
	m_mTransName2ID.clear();
	m_mColPlaceName2ID.clear();
	m_mColTransName2ID.clear();
	//by george
	m_mColPlaceObserverName2ID.clear();
	m_mColTransObserverName2ID.clear();

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
				wxString l_sColpnType = l_pcMetadata->GetAttribute(wxT("ColPNType"))->GetValueString();
				if (l_sColpnType.IsSameAs(wxT("Place instance")) && l_sType.IsSameAs(wxT("Place")) && std::find(m_pvUnfoldedPlace->begin(), m_pvUnfoldedPlace->end(), l_sName) == m_pvUnfoldedPlace->end())
				{//add observers over unfolded places
					m_pvUnfoldedPlaceObserver->push_back(l_sName);
				}

				if (l_sColpnType.IsSameAs(wxT("Col|Place")) && l_sType.IsSameAs(wxT("Place")) && std::find(m_pvColoredPlace->begin(), m_pvColoredPlace->end(), l_sName) == m_pvColoredPlace->end())
				{//add observers over coloured places
					m_pvColoredPlaceObserver->push_back(l_sName);
				}
				else if (l_sColpnType.IsSameAs(wxT("Col|Transition"))   && l_sType.IsSameAs(wxT("Transition")) && std::find(m_pvColoredTransObserver->begin(), m_pvColoredTransObserver->end(), l_sName) == m_pvColoredTransObserver->end())
				{//observers over coloured transition
					m_pvColoredTransObserver->push_back(l_sName);
				}
				else if (l_sColpnType.IsSameAs(wxT("Transition instance")) /*l_sColpnType.IsSameAs(wxT("Unfolded|Transition")) */&& l_sType.IsSameAs(wxT("Transition")) && std::find(m_pvUnfoldedTransObserver->begin(), m_pvUnfoldedTransObserver->end(), l_sName) == m_pvUnfoldedTransObserver->end())
				{
					m_pvUnfoldedTransObserver->push_back(l_sName);
				}
				else if (l_sColpnType==wxT("Mixed"))
				{
					m_pvMixedObserver->push_back(l_sName);
				}
			}
		}


			for (unsigned int i = 0; i < m_pvUnfoldedPlaceObserver->size(); i++)
			{
				wxString l_sObsPlaceName = (*m_pvUnfoldedPlaceObserver)[i];
				m_ArrayUnPlacesObserver.Add(l_sObsPlaceName);
				m_mPlaceObserverName2ID[l_sObsPlaceName] = i;

			}

			for (unsigned int i = 0; i < m_pvUnfoldedTransObserver->size(); i++)
			{
				wxString l_sObsTransName = (*m_pvUnfoldedTransObserver)[i];
				m_ArrayUnTransObserver.Add(l_sObsTransName);
				m_mTransObserverName2ID[l_sObsTransName] = i;
			}

			for (unsigned int i = 0; i <   m_pvMixedObserver->size(); i++)
			{
				wxString l_sObsName = (*m_pvMixedObserver)[i];
				m_ArrayMixedObservers.Add(l_sObsName);
				m_mMixedObserverName2ID[l_sObsName] = i;
			}

		for(unsigned int i= 0; i < m_pvColoredPlaceObserver->size(); i++)
		{
			wxString l_sObsPlaceName = (*m_pvColoredPlaceObserver)[i];
			m_ArrayColPlacesObserver.Add(l_sObsPlaceName);
			m_mColPlaceObserverName2ID[l_sObsPlaceName] = i;
		}

		for (unsigned int i = 0; i < m_pvColoredTransObserver->size(); i++)
		{
			wxString l_sTransName = (*m_pvColoredTransObserver)[i];
			m_ArrayColTransObserver.Add(l_sTransName);
			m_mColTransObserverName2ID[l_sTransName] = i;
		}



	m_msaCurLBInStrings[wxT("Place:Unfolded")] = m_ArrayUnPlaces;
	m_msaCurLBInStrings[wxT("Place:Colored")] = m_ArrayColPlaces;
	m_msaCurLBInStrings[wxT("Observer places:Colored")] = m_ArrayColPlacesObserver;
	//m_msaCurLBInStrings[wxT("Place:Auxiliary variables")] = m_ArrayPlaceAuxVar;
	m_msaCurLBInStrings[wxT("Transition:Unfolded")] = m_ArrayUnTranstions;
	m_msaCurLBInStrings[wxT("Transition:Colored")] = m_ArrayColTransitions;
	m_msaCurLBInStrings[wxT("Observer transitions:Colored")] = m_ArrayColTransObserver;//
	//m_msaCurLBInStrings[wxT("Transition:Auxiliary variables")] = m_ArrayTransAuxVar;
	m_msaCurLBInStrings[wxT("Observer places:Unfolded")] = m_ArrayUnPlacesObserver;
	m_msaCurLBInStrings[wxT("Observer transitions:Unfolded")] = m_ArrayUnTransObserver;
	m_msaCurLBInStrings[wxT("Mixed observers")] = m_ArrayMixedObservers;//

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

	NodesThroughRegEx();
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
		else if (m_sNetClass == SP_DS_FUZZY_ColSPN_CLASS/*by george*/)
		{
			dynamic_cast<SP_DLG_ColFSPNSimulationResults*>(m_pcColSimulationResults)->ComputeAuxiliaryVars();
			l_vAuxVar = dynamic_cast<SP_DLG_ColFSPNSimulationResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
		}
		else if (m_sNetClass == SP_DS_COLCPN_CLASS )
		{
			dynamic_cast<SP_DLG_ColCPNSimulationResults*>(m_pcColSimulationResults)->ComputeAuxiliaryVars(); 	
			l_vAuxVar =  dynamic_cast<SP_DLG_ColCPNSimulationResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
		}
		else if ( m_sNetClass == SP_DS_FUZZY_ColCPN_CLASS/*by george*/)
		{
			dynamic_cast<SP_DLG_ColFCPNSimulationResults*>(m_pcColSimulationResults)->ComputeAuxiliaryVars();
			l_vAuxVar = dynamic_cast<SP_DLG_ColFCPNSimulationResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
		}
		else if (m_sNetClass == SP_DS_COLHPN_CLASS)
		{
			dynamic_cast<SP_DLG_ColHPNSimultionResults*>(m_pcColSimulationResults)->ComputeAuxiliaryVars(); 
			l_vAuxVar =  dynamic_cast<SP_DLG_ColHPNSimultionResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
		}
		else if (m_sNetClass == SP_DS_FUZZY_ColHPN_CLASS/*by george*/)
		{
			dynamic_cast<SP_DLG_ColFHPNSimulationResults*>(m_pcColSimulationResults)->ComputeAuxiliaryVars();
			l_vAuxVar = dynamic_cast<SP_DLG_ColFHPNSimulationResults*>(m_pcColSimulationResults)->GetAuxPLVariables();
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

		if (m_sNodeType == wxT("Observer places"))
				{//by george

					if (l_bNotFound)
					{
						for (unsigned int i = 0; i < m_ArrayColPlacesObserver.size(); i++)
						{
							wxString l_sPlace = m_ArrayColPlacesObserver[i];
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
						for (unsigned int i = 0; i < m_ArrayUnPlacesObserver.size(); i++)
						{
							wxString l_sPlace = m_ArrayUnPlacesObserver[i];
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

				if (m_sNodeType == wxT("Observer transitions"))
				{

					if (l_bNotFound)
					{
						for (unsigned int i = 0; i < m_ArrayColTransObserver.size(); i++)
						{
							wxString l_sPlace = m_ArrayColTransObserver[i];
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
						for (unsigned int i = 0; i < m_ArrayUnTransObserver.size(); i++)
						{
							wxString l_sPlace = m_ArrayUnTransObserver[i];
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

				if (m_sNodeType == wxT("Mixed observers"))
				{

					if (l_bNotFound)
					{
						for (unsigned int i = 0; i < m_ArrayMixedObservers.size(); i++)
						{
							wxString l_sPlace = m_ArrayMixedObservers[i];
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
						for (unsigned int i = 0; i < m_ArrayMixedObservers.size(); i++)
						{
							wxString l_sPlace = m_ArrayMixedObservers[i];
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
	
	if (m_sNodeType == wxT("Observer places") || m_sNodeType == wxT("Observer transitions")|| m_sNodeType == wxT("Mixed observers"))
	{
			m_msaCurLBInStrings[wxT("Observer places:Unfolded")] = m_ArrayUnPlacesObserver;
			m_msaCurLBInStrings[wxT("Observer places:Colored")] = m_ArrayColPlacesObserver;
			m_msaCurLBInStrings[wxT("Observer transitions:Unfolded")] = m_ArrayUnTransObserver;
			m_msaCurLBInStrings[wxT("Observer transitions:Colored")] = m_ArrayColTransObserver;
			m_msaCurLBInStrings[wxT("Mixed observers:Colored")] = m_ArrayMixedObservers;
			m_pListBoxOut->Clear();
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
		//m_msaCurLBInStrings[wxT("Place:Auxiliary variables")] = m_ArrayPlaceAuxVar;

		m_pcChooseColoredPlace->Clear();
		m_pcChooseColoredPlace->Append(wxT(""));
		for(unsigned i = 0; i < m_pcUnfoldedNet->GetColoredPlaceNames()->size(); i++)
		{
			m_pcChooseColoredPlace->Append( (*(m_pcUnfoldedNet->GetColoredPlaceNames()))[i]);
		}
		m_pcChooseColoredPlace->SetSelection(0);	
		
	}

	else if (m_pcPlaceTransitionRadioBox->GetSelection() == 2)//observer place
		{
			m_sNodeType = wxT("Observer places");

			m_pcOutputTypeRadioBox->Show(2, true);


			m_msaCurLBInStrings[wxT("Observer places:Unfolded")] = m_ArrayUnPlacesObserver;
			m_msaCurLBInStrings[wxT("Observer places:Colored")] = m_ArrayColPlacesObserver;


			m_pcChooseColoredPlace->Clear();
			m_pcChooseColoredPlace->Append(wxT(""));

			m_pcChooseColoredPlace->SetSelection(0);
		}
		else if (m_pcPlaceTransitionRadioBox->GetSelection() == 3)//observer transition
		{
			m_sNodeType = wxT("Observer transitions");

			m_pcOutputTypeRadioBox->Show(2, true);


			m_msaCurLBInStrings[wxT("Observer transitions:Unfolded")] = m_ArrayUnTransObserver;
			m_msaCurLBInStrings[wxT("Observer transitions:Colored")] = m_ArrayColTransObserver;


			m_pcChooseColoredPlace->Clear();
			m_pcChooseColoredPlace->Append(wxT(""));

			m_pcChooseColoredPlace->SetSelection(0);
		}
		else if (m_pcPlaceTransitionRadioBox->GetSelection() == 4)
		{
			m_sNodeType = wxT("Mixed observers");

			m_pcOutputTypeRadioBox->Show(2, true);


			m_msaCurLBInStrings[wxT("Mixed observers:Unfolded")] = m_ArrayMixedObservers;
			m_msaCurLBInStrings[wxT("Mixed observers:Colored")] = m_ArrayMixedObservers;


			m_pcChooseColoredPlace->Clear();
			m_pcChooseColoredPlace->Append(wxT(""));

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
	wxString l_sSelColPlace = m_pcChooseColoredPlace->GetStringSelection();

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
