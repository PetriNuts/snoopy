//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_gui/widgets/dialogs/wid_dia_SPN/SP_WDG_StLookupTableName.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/SP_DS_Node.h"
#include "sp_core/SP_Core.h"

#include <wx/event.h>


BEGIN_EVENT_TABLE( SP_WDG_StLookupTableName, SP_WDG_DialogBase )

END_EVENT_TABLE()


SP_WDG_StLookupTableName::SP_WDG_StLookupTableName(const wxString& p_sPage, unsigned int p_nOrdering)
:SP_WDG_DialogBase(p_sPage, p_nOrdering)
{
}

SP_WDG_StLookupTableName::~SP_WDG_StLookupTableName()
{
}

SP_WDG_DialogBase*
SP_WDG_StLookupTableName::Clone()
{

  return new SP_WDG_StLookupTableName(GetName(), GetDialogOrdering());

}

bool
SP_WDG_StLookupTableName::AddToDialog( const SP_ListAttribute* p_ptlAttributes,
  SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview )
{

  CHECK_POINTER( p_ptlAttributes, return FALSE );
  CHECK_BOOL( ! p_ptlAttributes->empty(), return FALSE );
  CHECK_POINTER( p_pcDlg, return FALSE );

  m_pcDlg = p_pcDlg;

  // remember the list of attributes, we display
  m_tlAttributes = *p_ptlAttributes;
  SP_DS_Attribute* l_pcAttr = (*m_tlAttributes.begin());

  wxString l_sPage = GetName() + wxT(":") + l_pcAttr->GetParent()->GetClassName();
  SP_WDG_NotebookPage* l_pcPage = p_pcDlg->AddPage(l_sPage, GetDialogOrdering());
  CHECK_POINTER( l_pcPage, return FALSE );

  wxBoxSizer* l_pcSizer = new wxBoxSizer( wxHORIZONTAL );
  m_bMultiple = ( p_ptlAttributes->size() > 1 );

  l_pcSizer->Add( new wxStaticText( l_pcPage, -1, wxT("") ), 1,
    wxALL | wxALIGN_CENTER_VERTICAL, 5 );

  l_pcPage->AddControl( l_pcSizer, 0, wxEXPAND );


  wxBoxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );

  l_pcRowSizer->Add( new wxStaticText( l_pcPage, -1, wxT("Name:") ), 1, wxLEFT | wxRIGHT | wxTOP, 5 );

  m_pcNameTextCtrl = new wxTextCtrl( l_pcPage, -1, wxT("") );

  l_pcRowSizer->Add( m_pcNameTextCtrl, 1, wxLEFT | wxRIGHT | wxTOP, 5 );

  LoadData();

  l_pcPage->AddControl( l_pcRowSizer, 0, wxEXPAND );

  p_pcDlg->PushEventHandler( this );

  return TRUE;

}

bool
SP_WDG_StLookupTableName::OnDlgOk()
{

  if( ! IsNameValid( m_pcNameTextCtrl->GetValue() ) ) {

    SP_MESSAGEBOX( wxT("The inputed name is already existing."), wxT("Invalid name"),
      wxOK | wxICON_ERROR );
    return false;


  }

  SaveData();
  return SP_WDG_DialogBase::OnDlgOk();

}


bool
SP_WDG_StLookupTableName::LoadData()
{

  SP_DS_Attribute* l_pcAttr = ( *m_tlAttributes.begin() );

  m_pcNameTextCtrl->SetValue( l_pcAttr->GetValueString() );

  return true;

}


bool
SP_WDG_StLookupTableName::SaveData()
{

  CHECK_BOOL( ! m_tlAttributes.empty(), return FALSE );

  SP_DS_Attribute* l_pcAttr = *m_tlAttributes.begin();

  l_pcAttr->SetValueString( m_pcNameTextCtrl->GetValue() );


  return true;

}


bool
SP_WDG_StLookupTableName :: IsNameValid( const wxString& p_sInputedName )
{

	if (p_sInputedName == wxT(""))
	{
		return true;
	}

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();

	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass = l_pcGraph->GetNodeclass(wxT("LookupTable"));

	SP_ListNode::const_iterator l_itElem;
	wxString l_sParameterName;

	for(l_itElem = l_pcNodeclass->GetElements()->begin();
		l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		l_sParameterName = (*l_itElem)->GetAttribute(wxT("Name"))->GetValueString();

		if (l_sParameterName == p_sInputedName)
		{
			return false;
		}
	}

  return true;

}
