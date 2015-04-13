//////////////////////////////////////////////////////////////////////
// $Author: okrause $
// $Version: 0.1 $
// $Date: 2009/02/13 $
// Short Description: export to metatool input format
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportContPed2METATOOL.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"

#include "snoopy.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_Core.h"

enum
{
	SP_ID_RADIOBUTTON_PLACE = SP_ID_LAST_ID + 1,
	SP_ID_RADIOBUTTON_TRANSITION,
	SP_ID_BUTTON_TOPLR,
	SP_ID_BUTTON_TOPRL,
	SP_ID_BUTTON_BOTTOMLR,
	SP_ID_BUTTON_BOTTOMRL,
	SP_ID_LISTBOX_IN,
	SP_ID_LISTBOX_OUT1,
	SP_ID_LISTBOX_OUT2
};


SP_ExportContPed2METATOOL::SP_ExportContPed2METATOOL()
{
}

SP_ExportContPed2METATOOL::~SP_ExportContPed2METATOOL()
{
}

bool SP_ExportContPed2METATOOL::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_CONTINUOUSPED_CLASS);
}

bool
SP_ExportContPed2METATOOL::DoWrite()
{

  m_file.Write(wxT("#\n"));
  m_file.Write(wxT("# Created by Snoopy 2\n"));
  m_file.Write(wxT("#\n"));
  m_file.Write(wxT("\n"));

  WriteENZREV();
  WriteENZIRREV();
  WriteMETINT();
  WriteMETEXT();
  WriteCAT();



  return !m_bError;
}


void
SP_ExportContPed2METATOOL::WriteENZREV()
{
  m_file.Write(wxT("\n"));
  m_file.Write(wxT("\n"));
  m_file.Write(wxT("-ENZREV\n"));

  for(unsigned i=0;i<m_ListBoxOut1_Reaction_String.size();i++)
  {
	  m_file.Write(m_ListBoxOut1_Reaction_String[i]);
	  m_file.Write(wxT(" "));
  }

}


void
SP_ExportContPed2METATOOL::WriteENZIRREV()
{
  m_file.Write(wxT("\n"));
  m_file.Write(wxT("\n"));
  m_file.Write(wxT("-ENZIRREV\n"));

  for(unsigned i=0;i<m_ListBoxIn_Reaction_String.size();i++)
  {
	  m_file.Write(m_ListBoxIn_Reaction_String[i]);
	  m_file.Write(wxT(" "));
  }
}

void SP_ExportContPed2METATOOL::WriteMETINT()
{
   m_file.Write(wxT("\n"));
  m_file.Write(wxT("\n"));
  m_file.Write(wxT("-METINT\n"));

  for(unsigned i=0;i<m_ListBoxOut1_Place_String.size();i++)
  {
	  m_file.Write(m_ListBoxOut1_Place_String[i]);
	  m_file.Write(wxT(" "));
  }

}
void SP_ExportContPed2METATOOL::WriteMETEXT()
{
  m_file.Write(wxT("\n"));
  m_file.Write(wxT("\n"));
  m_file.Write(wxT("-METEXT\n"));

  for(unsigned i=0;i<m_ListBoxIn_Place_String.size();i++)
  {
	  m_file.Write(m_ListBoxIn_Place_String[i]);
	  m_file.Write(wxT(" "));
  }
}


void SP_ExportContPed2METATOOL::WriteCAT()
{

  ContTransitionIdMap::iterator tIt;
  ContTransitionIdMap::iterator tempIt;
  SP_ContTransition* t;
  list<SP_ContArc*>* aList;
  list<SP_ContArc*>::iterator aIt;
  SP_ContArc* a;

  m_file.Write(wxT("\n"));
  m_file.Write(wxT("\n"));
  m_file.Write(wxT("-CAT\n"));

  for (tIt = m_contTrIdMap.begin(); tIt != m_contTrIdMap.end(); tIt++)
  {
	t = (*tIt).second;
	m_file.Write(t->m_name.c_str());
	m_file.Write(wxT(" : "));
	aList = m_preContTrMap[t->m_id];
	if (aList)
    {
	  aIt = aList->begin();
	  a = (*aIt);
	  m_file.Write((m_contPlaceIdMap[a->m_id])->m_name.c_str());
	  for (aIt++; aIt != aList->end(); aIt++)
      {
		a = (*aIt);
		m_file.Write(wxT(" + "));
		m_file.Write((m_contPlaceIdMap[a->m_id])->m_name.c_str());
      }
    }

    aList = m_postContTrMap[t->m_id];
    if (aList)
    {
	  m_file.Write(wxT(" = "));

	  aIt = aList->begin();
	  a = (*aIt);
	  m_file.Write((m_contPlaceIdMap[a->m_id])->m_name.c_str());
	  for (aIt++; aIt != aList->end(); aIt++)
      {
		a = (*aIt);
		m_file.Write(wxT(" + "));
		m_file.Write((m_contPlaceIdMap[a->m_id])->m_name.c_str());
      }
	  tempIt=tIt;
	  //if(++tempIt != m_contTrIdMap.end())m_file.Write(wxT(" .\n"));
	  m_file.Write(wxT(" .\n"));
    }
  }
}

bool
SP_ExportContPed2METATOOL::AddToDialog(SP_DLG_ExportProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc)
{

	
	m_pcDlg=p_pcDlg;
	SP_WDG_NotebookPage* l_pcNotebookPage = p_pcDlg->AddPage(wxT("MetaTool"));

	wxSizer* l_pcMainTablePropertiesSizer =	new wxStaticBoxSizer( new wxStaticBox( l_pcNotebookPage, -1,
											wxT("Please select the type of classification elements ") ), wxVERTICAL );
	wxSizer* l_pcRowSizer;
	
	l_pcRowSizer = new wxBoxSizer( wxVERTICAL );

	m_pRadioButton1=new wxRadioButton(l_pcNotebookPage, SP_ID_RADIOBUTTON_TRANSITION, wxT("&Enzyme"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_pRadioButton2=new wxRadioButton(l_pcNotebookPage, SP_ID_RADIOBUTTON_PLACE, wxT("&Metabolite"), wxDefaultPosition, wxDefaultSize);

	l_pcRowSizer->Add(m_pRadioButton1, 0, wxEXPAND | wxALL, 5);
	l_pcRowSizer->Add(m_pRadioButton2, 0, wxEXPAND | wxALL, 5);
	l_pcMainTablePropertiesSizer->Add(l_pcRowSizer, 0, wxEXPAND | wxALL, 5);
	l_pcNotebookPage->AddControl(l_pcMainTablePropertiesSizer, 0, wxEXPAND | wxALL, 5);

	l_pcMainTablePropertiesSizer =	new wxStaticBoxSizer( new wxStaticBox( l_pcNotebookPage, -1,
											wxT("Classification") ), wxVERTICAL );
	
	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );


	wxBoxSizer* l_pcRowSizerforEnzym = new wxBoxSizer( wxVERTICAL );
	m_pStaticTextIn=new wxStaticText(l_pcNotebookPage,-1,wxT("Irreversible Enzyme"),wxDefaultPosition,wxDefaultSize,wxALIGN_LEFT);
	m_pListBoxIn = new wxListBox(l_pcNotebookPage, SP_ID_LISTBOX_IN,wxDefaultPosition,wxDefaultSize,0,NULL,wxLB_EXTENDED);
	l_pcRowSizerforEnzym->Add(m_pStaticTextIn, 0, wxEXPAND | wxALL, 5);
	l_pcRowSizerforEnzym->Add(m_pListBoxIn, 1, wxEXPAND | wxALL, 5);


    wxBoxSizer* l_pcRowSizerleft = new wxBoxSizer( wxVERTICAL );
	wxStaticText* m_pStatic1=new wxStaticText(l_pcNotebookPage,-1,wxT(""),wxDefaultPosition,wxDefaultSize,wxALIGN_LEFT);
	wxButton* l_pButton1=new wxButton( l_pcNotebookPage, SP_ID_BUTTON_TOPLR, wxT( ">>" ) );
	wxButton* l_pButton2=new wxButton( l_pcNotebookPage, SP_ID_BUTTON_TOPRL, wxT( "<<" ) );
    l_pcRowSizerleft->Add( m_pStatic1, 0, wxALIGN_CENTER| wxALL, 10);
	l_pcRowSizerleft->Add(l_pButton1, 0,  wxALIGN_CENTER| wxALL, 5);
	l_pcRowSizerleft->Add(l_pButton2, 0, wxALIGN_CENTER| wxALL, 5);	


	wxBoxSizer* l_pcRowSizerforrever = new wxBoxSizer( wxVERTICAL );
	m_pStaticTextOut1=new wxStaticText(l_pcNotebookPage,-1,wxT("Reversible Enzyme"),wxDefaultPosition,wxDefaultSize,wxALIGN_LEFT);
	m_pListBoxOut1 = new wxListBox(l_pcNotebookPage,SP_ID_LISTBOX_OUT1,wxDefaultPosition,wxDefaultSize,0,NULL,wxLB_EXTENDED);
	l_pcRowSizerforrever->Add(m_pStaticTextOut1, 0, wxEXPAND | wxALL, 5);
	l_pcRowSizerforrever->Add(m_pListBoxOut1, 1, wxEXPAND | wxALL, 5);
	
	l_pcRowSizer->Add(l_pcRowSizerforEnzym, 1, wxEXPAND | wxALL, 5);
	l_pcRowSizer->Add(l_pcRowSizerleft, 1, wxEXPAND | wxALL, 5);
	l_pcRowSizer->Add(l_pcRowSizerforrever, 1, wxEXPAND | wxALL, 5);


	l_pcMainTablePropertiesSizer->Add(l_pcRowSizer, 0, wxEXPAND | wxALL, 5);

	l_pcNotebookPage->AddControl(l_pcMainTablePropertiesSizer, 0, wxEXPAND | wxALL, 5);	

	m_pRadioButton1->Bind(wxEVT_RADIOBUTTON, &SP_ExportContPed2METATOOL::OnRadioTransition, this, SP_ID_RADIOBUTTON_TRANSITION);
	m_pRadioButton2->Bind(wxEVT_RADIOBUTTON, &SP_ExportContPed2METATOOL::OnRadioPlace, this, SP_ID_RADIOBUTTON_PLACE);
	l_pButton1->Bind(wxEVT_BUTTON, &SP_ExportContPed2METATOOL::Ontopleftright, this, SP_ID_BUTTON_TOPLR);
	l_pButton2->Bind(wxEVT_BUTTON, &SP_ExportContPed2METATOOL::Ontoprightleft, this, SP_ID_BUTTON_TOPRL);
	m_pListBoxIn->Bind(wxEVT_LISTBOX_DCLICK, &SP_ExportContPed2METATOOL::OnInSelection, this, SP_ID_LISTBOX_IN);
	m_pListBoxOut1->Bind(wxEVT_LISTBOX_DCLICK, &SP_ExportContPed2METATOOL::OnOutSelection, this, SP_ID_LISTBOX_OUT1);

	m_doc = p_pcDoc;
	m_graph = m_doc->GetGraph();

	if(!InitNodes())
	   return false;	

	m_pListBoxIn->Clear();
	m_pListBoxOut1->Clear();	
	m_ListBoxOut1_Place_String.clear();
	m_ListBoxOut1_Reaction_String.clear();
	m_ListBoxIn_Place_String.clear();
	m_ListBoxIn_Reaction_String.clear();


	LoadReaction();
	m_RadioFocus=true;
	
	return true;
}

bool
SP_ExportContPed2METATOOL::OnDialogOk()
{

	if (m_pcDlg->Validate() && m_pcDlg->TransferDataFromWindow())	
	{
		if(m_pRadioButton1->GetValue())
		{
			m_ListBoxOut1_Reaction_String=m_pListBoxOut1->GetStrings();
			m_ListBoxIn_Reaction_String=m_pListBoxIn->GetStrings();
			
		}
		else
		{	
			m_ListBoxOut1_Place_String=m_pListBoxOut1->GetStrings();
			m_ListBoxIn_Place_String=m_pListBoxIn->GetStrings();
			
		}
	}	

	return true;
}

void
SP_ExportContPed2METATOOL :: Ontopleftright(wxCommandEvent& p_cEvent )
{

	wxArrayInt l_Selections;
	wxString l_SelectionItem;
	int l_number=m_pListBoxIn->GetSelections(l_Selections);
	if(l_number>0)
	{
		for(int Counter = 0; Counter <l_number; Counter++ )
		{
		    int l_nNum = l_Selections.Item(Counter);
			
			l_SelectionItem=m_pListBoxIn->GetString(l_nNum);
			m_pListBoxOut1->Insert(l_SelectionItem, m_pListBoxOut1->GetCount());
			
		}
		for(int Counter = l_number-1; Counter >=0; Counter-- )
		{
		    int l_nNum = l_Selections.Item(Counter);			
			m_pListBoxIn->Delete(l_nNum);
		}
	}

}
void
SP_ExportContPed2METATOOL :: Ontoprightleft(wxCommandEvent& p_cEvent )
{


	wxArrayInt l_Selections;
	wxString l_SelectionItem;
	int l_number=m_pListBoxOut1->GetSelections(l_Selections);
	if(l_number>0)
	{
		for(int Counter = 0; Counter <l_number; Counter++ )
		{
		    int l_nNum = l_Selections.Item(Counter);
			
			l_SelectionItem=m_pListBoxOut1->GetString(l_nNum);
			m_pListBoxIn->Insert(l_SelectionItem, m_pListBoxIn->GetCount());
			
		}
		for(int Counter = l_number-1; Counter >=0; Counter-- )
		{
		    int l_nNum = l_Selections.Item(Counter);		
			
			m_pListBoxOut1->Delete(l_nNum);

		}
	}

}


void SP_ExportContPed2METATOOL :: OnRadioPlace( wxCommandEvent& p_cEvent )
{
	 

			m_pStaticTextIn->SetLabel(wxT("Internal Metabolite"));
			m_pStaticTextOut1->SetLabel(wxT("External Metabolite"));
			
			m_ListBoxOut1_Reaction_String.clear();
			m_ListBoxIn_Reaction_String.clear();
			m_ListBoxOut1_Reaction_String=m_pListBoxOut1->GetStrings();	
			m_ListBoxIn_Reaction_String=m_pListBoxIn->GetStrings();
			m_pListBoxOut1->Clear();
			m_pListBoxIn->Clear();
			

			if ((!m_ListBoxOut1_Place_String.IsEmpty())||(!m_ListBoxIn_Place_String.IsEmpty())) 
			{
				
					m_pListBoxOut1->InsertItems(m_ListBoxOut1_Place_String,0);	
					m_pListBoxIn->InsertItems(m_ListBoxIn_Place_String,0);
		    }
			else
			{
				LoadPlace();	
			}		
		
}

void SP_ExportContPed2METATOOL :: OnRadioTransition( wxCommandEvent& p_cEvent )
{

			m_pStaticTextIn->SetLabel(wxT("Reversible Enzyme"));
			m_pStaticTextOut1->SetLabel(wxT("Irreversible Enzyme"));
	
			m_ListBoxOut1_Place_String.clear();
			m_ListBoxIn_Place_String.clear();
			m_ListBoxOut1_Place_String=m_pListBoxOut1->GetStrings();
			m_ListBoxIn_Place_String=m_pListBoxIn->GetStrings();
			m_pListBoxOut1->Clear();
			m_pListBoxIn->Clear();
				

			if ((!m_ListBoxOut1_Reaction_String.IsEmpty())||(!m_ListBoxIn_Reaction_String.IsEmpty())) 
			{
				
					m_pListBoxOut1->InsertItems(m_ListBoxOut1_Reaction_String,0);	
					m_pListBoxIn->InsertItems(m_ListBoxIn_Reaction_String,0);		
		    }
			else
			{
				LoadReaction();	
			}
		
}


 void SP_ExportContPed2METATOOL::LoadPlace( )
{	
	
	wxArrayString l_Place;
	ContPlaceIdMap::iterator pIt;
	SP_ContPlace* m_pSP_Place;

	for (pIt = m_contPlaceIdMap.begin(); pIt != m_contPlaceIdMap.end(); pIt++)
	{
		m_pSP_Place = (*pIt).second;
		l_Place.push_back(m_pSP_Place->m_name);		
	}
    m_pListBoxIn->Clear();
	if (!l_Place.IsEmpty()) 
	{
	  m_pListBoxIn->InsertItems(l_Place,0);
    }
	  
     
}

  void SP_ExportContPed2METATOOL :: LoadReaction( )
  {
	
	wxArrayString l_Transition;
	ContTransitionIdMap::iterator tIt;
	SP_ContTransition* l_pSP_Transition;
	
	for (tIt = m_contTrIdMap.begin(); tIt != m_contTrIdMap.end(); tIt++)
	{
		l_pSP_Transition = (*tIt).second;
        l_Transition.push_back(l_pSP_Transition->m_name);
	}
	
	m_pListBoxIn->Clear();
	if (!l_Transition.IsEmpty()) 
	{
		m_pListBoxIn->InsertItems(l_Transition,0);
	}	
		  

  }

  void
SP_ExportContPed2METATOOL ::OnInSelection( wxCommandEvent& p_cEvent )
 {

 }

   void
SP_ExportContPed2METATOOL ::OnOutSelection( wxCommandEvent& p_cEvent )
 {

 }
