/*
 * SP_GUI_SelectEditingList.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 20.06.2011
 * Short Description:
 */
//=================================================

#include<algorithm>
#include "sp_ds/SP_DS_Graph.h"

#include "sp_gui/dialogs/dia_SteeringGUI/SP_GUI_SelectEditingList.h"

enum
{
	    SP_GUI_SELECTNODES_OPTIONBOX_NODETYPE,
		SP_GUI_SELECTNODES_BUTTON_MOVE_LEFT_TO_RIGHT,
		SP_GUI_SELECTNODES_BUTTON_MOVE_RIGHT_TO_LEFT,
		SP_GUI_SELECTNODES_BUTTON_MOVE_ALL_RIGHT_TO_LEFT,
		SP_GUI_SELECTNODES_BUTTON_MOVE_ALL_LEFT_TO_RIGHT,

		SP_GUI_SELECTNODES_LISTBOX_AVAILABLE_NODES,
		SP_GUI_SELECTNODES_LISTBOX_SELECYED_NODES
};

BEGIN_EVENT_TABLE(SP_GUI_SelectEditingList,wxDialog)

      EVT_BUTTON(SP_GUI_SELECTNODES_BUTTON_MOVE_LEFT_TO_RIGHT,SP_GUI_SelectEditingList::OnMoveLeftToRight)
      EVT_BUTTON(SP_GUI_SELECTNODES_BUTTON_MOVE_RIGHT_TO_LEFT,SP_GUI_SelectEditingList::OnMoveRightToLeft)

      EVT_BUTTON(SP_GUI_SELECTNODES_BUTTON_MOVE_ALL_LEFT_TO_RIGHT,SP_GUI_SelectEditingList::OnMoveAllLeftToRight)
      EVT_BUTTON(SP_GUI_SELECTNODES_BUTTON_MOVE_ALL_RIGHT_TO_LEFT,SP_GUI_SelectEditingList::OnMoveAllRightToLeft)

      EVT_BUTTON(wxID_OK,SP_GUI_SelectEditingList::OnOk)

      EVT_LISTBOX_DCLICK(SP_GUI_SELECTNODES_LISTBOX_AVAILABLE_NODES, SP_GUI_SelectEditingList::OnMoveLeftToRight)

      EVT_LISTBOX_DCLICK(SP_GUI_SELECTNODES_LISTBOX_SELECYED_NODES, SP_GUI_SelectEditingList::OnMoveRightToLeft)

END_EVENT_TABLE()

SP_GUI_SelectEditingList::SP_GUI_SelectEditingList(wxWindow* p_pcParent,const wxString& p_sTitle ,const SP_VectorString*
		                                          p_sListToselectFrom,const spsa::VectorULong* p_pcInitalSelection,
				                                 const SP_VectorDouble* p_pcCurrentValues)
:wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER),
m_sListToselectFrom(p_sListToselectFrom),
m_pcCurrentValues(p_pcCurrentValues)
{
		     wxSizer* l_pcMainSizer=new wxBoxSizer(wxVERTICAL);
		     wxSizer* l_pcSelectorSizer=new wxBoxSizer(wxHORIZONTAL);
		     wxSizer* l_pcButtonSizer=new wxBoxSizer(wxHORIZONTAL);

		     //add sub sizer
		     l_pcMainSizer->Add(l_pcSelectorSizer, 1, wxEXPAND | wxALL);
		     l_pcMainSizer->Add(l_pcButtonSizer, 0, wxEXPAND | wxALIGN_CENTER_VERTICAL);

		     //available nodes
	         wxSizer* l_pcRowSizer=new wxBoxSizer(wxVERTICAL);
	         m_pcAvailableNodes= new wxListBox(this,SP_GUI_SELECTNODES_LISTBOX_AVAILABLE_NODES,wxDefaultPosition,wxSize(-1,300),0,NULL,wxLB_MULTIPLE );
	         l_pcRowSizer->Add(m_pcAvailableNodes, 1, wxEXPAND | wxALL);
	         l_pcSelectorSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL);

	         //select node type
			 wxSizer* l_pcMiddleSizer=new wxBoxSizer(wxVERTICAL);
			 l_pcRowSizer=new wxBoxSizer(wxHORIZONTAL);
			 l_pcMiddleSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL);

	         //button sizer
	         l_pcRowSizer=new wxBoxSizer(wxVERTICAL);
	         l_pcRowSizer->Add(new wxButton(this,SP_GUI_SELECTNODES_BUTTON_MOVE_LEFT_TO_RIGHT,wxT(">")),0,wxEXPAND | wxALIGN_CENTER);
	         l_pcRowSizer->AddSpacer(8);
	         l_pcRowSizer->Add(new wxButton(this,SP_GUI_SELECTNODES_BUTTON_MOVE_RIGHT_TO_LEFT,wxT("<")),0,wxEXPAND | wxALIGN_CENTER);
	         l_pcRowSizer->AddSpacer(8);
	         l_pcRowSizer->Add(new wxButton(this,SP_GUI_SELECTNODES_BUTTON_MOVE_ALL_LEFT_TO_RIGHT,wxT(">>")),0,wxEXPAND | wxALIGN_CENTER);
	         l_pcRowSizer->AddSpacer(8);
	         l_pcRowSizer->Add(new wxButton(this,SP_GUI_SELECTNODES_BUTTON_MOVE_ALL_RIGHT_TO_LEFT,wxT("<<")),0,wxEXPAND | wxALIGN_CENTER);

	         l_pcMiddleSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL);
			 l_pcRowSizer=new wxBoxSizer(wxHORIZONTAL);
			 l_pcMiddleSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL);
			 l_pcSelectorSizer->Add(l_pcMiddleSizer, 0, wxEXPAND | wxALIGN_CENTER_VERTICAL);


	         //selected nodes
	         l_pcRowSizer=new wxBoxSizer(wxVERTICAL);
	          m_pcSelectedNodes= new wxListBox(this,SP_GUI_SELECTNODES_LISTBOX_SELECYED_NODES,wxDefaultPosition,wxSize(-1,300),0,NULL,wxLB_MULTIPLE );
		    l_pcRowSizer->Add(m_pcSelectedNodes, 1, wxEXPAND | wxALL);


			 l_pcSelectorSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL);


		     l_pcButtonSizer->Add(CreateSeparatedButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND | wxALIGN_CENTER);


			 //alignment the sizer
			 SetSizerAndFit(l_pcMainSizer);
			 Center();

			 LoadData(p_pcInitalSelection);

}

void SP_GUI_SelectEditingList::LoadData(const spsa::VectorULong* p_pcInitalSelection)
{
 wxArrayInt l_anSelections;

         m_mNodeName2Position.clear();

         l_anSelections.Clear();
        
          for(unsigned int l_nPos=0;l_nPos<m_sListToselectFrom->size();l_nPos++)
	      {
        	  m_pcAvailableNodes->Append((*m_sListToselectFrom)[l_nPos]);
	         
	         m_mNodeName2Position[(*m_sListToselectFrom)[l_nPos]]=l_nPos;
	      }

          for(unsigned int l_nPos=0;l_nPos<p_pcInitalSelection->size();l_nPos++)
          {
        	  int l_nValue=(*p_pcInitalSelection)[l_nPos];

        	  l_anSelections.Add(l_nValue);
          }
	      
          MoveElements(m_pcAvailableNodes,m_pcSelectedNodes,l_anSelections);

}

SP_GUI_SelectEditingList::~SP_GUI_SelectEditingList()
{

}
void SP_GUI_SelectEditingList::OnOk(wxCommandEvent& WXUNUSED(event))
{
	      m_anSelectedList.clear();

	      for(unsigned int l_nItem=0;l_nItem<m_pcSelectedNodes->GetCount();l_nItem++)
           {
                wxString l_sName=m_pcSelectedNodes->GetString(l_nItem);
        	      
        	    m_anSelectedList.push_back(m_mNodeName2Position[l_sName]);
           }

	      //end modal with ok button pressed
	      EndModal(wxID_OK);
}

void SP_GUI_SelectEditingList::MoveElements(wxListBox* p_pcSourceList,wxListBox* p_pcDestList, wxArrayInt& p_anSelections)
{
SP_VectorString l_asItemsToDelete;

          l_asItemsToDelete.resize(p_anSelections.Count());

	      for(unsigned long l_nElement=0;l_nElement<p_anSelections.Count();l_nElement++)
	      {
			    //get the item index
	    	    unsigned int l_nItemID=p_anSelections[l_nElement];

				wxString l_sItem=p_pcSourceList->GetString(l_nItemID);

	    		p_pcDestList->Append(l_sItem);

	    		l_asItemsToDelete[l_nElement]=l_sItem;
	      }

		  RemoveItemsFromList(p_pcSourceList,l_asItemsToDelete);
}

void SP_GUI_SelectEditingList::RemoveItemsFromList(wxListBox* p_pcList,const SP_VectorString& p_anSelections)
{
         //delete items
		 for(unsigned long l_nElement=0;l_nElement<p_anSelections.size();l_nElement++)
		 {
			 //find the item of the index name
			 int l_nIndexToDelete=p_pcList->FindString(p_anSelections[l_nElement],true);

			 //delete the item
			 if(l_nIndexToDelete!=wxNOT_FOUND )
				 p_pcList->Delete(l_nIndexToDelete);
		 }
}

void SP_GUI_SelectEditingList::MoveAllElement(wxListBox* p_pcSourceList,wxListBox* p_pcDestList)
{
	      for(unsigned long l_nElement=0;l_nElement<p_pcSourceList->GetCount();l_nElement++)
		  {
			  p_pcDestList->Append(p_pcSourceList->GetString(l_nElement));
		  }

	      p_pcSourceList->Clear();
}

void SP_GUI_SelectEditingList::OnMoveLeftToRight(wxCommandEvent& event)
{
 wxArrayInt l_anSelections;

          m_pcAvailableNodes->GetSelections(l_anSelections);

	      MoveElements(m_pcAvailableNodes,m_pcSelectedNodes,l_anSelections);
}

void SP_GUI_SelectEditingList::OnMoveRightToLeft(wxCommandEvent& event)
{
wxArrayInt l_anSelections;

           m_pcSelectedNodes->GetSelections(l_anSelections);

		  MoveElements(m_pcSelectedNodes,m_pcAvailableNodes,l_anSelections);
}

void SP_GUI_SelectEditingList::OnMoveAllLeftToRight(wxCommandEvent& event)
{
	      MoveAllElement(m_pcAvailableNodes,m_pcSelectedNodes);
}

void SP_GUI_SelectEditingList::OnMoveAllRightToLeft(wxCommandEvent& event)
{
	      MoveAllElement(m_pcSelectedNodes,m_pcAvailableNodes);
}

