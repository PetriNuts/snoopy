//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#include "sp_gui/dialogs/SP_DLG_GraphProperties.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

IMPLEMENT_CLASS(SP_DLG_GraphProperties, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_GraphProperties, wxDialog)
EVT_BUTTON(wxID_OK, SP_DLG_GraphProperties::OnDlgOk)
EVT_BUTTON(wxID_CANCEL, SP_DLG_GraphProperties::OnDlgCancel)
END_EVENT_TABLE()

SP_DLG_GraphProperties::SP_DLG_GraphProperties(SP_GUI_Mainframe* p_pcParent,
                                               SP_DS_Graph* p_pcGraph,
                                               const wxString& p_sTitle,
                                               long p_nStyle)
:wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxDefaultSize, p_nStyle | wxSTAY_ON_TOP),
m_pcGraph(p_pcGraph),
m_nCountAttr(0)
{
 	/* top level sizer */
	wxSizer* l_pcBoxSizer = new wxBoxSizer(wxVERTICAL);
    m_pcSizer = new wxGridBagSizer();
	l_pcBoxSizer->Add(m_pcSizer, 1, wxALL | wxEXPAND, 5);
	
    ShowGraphAttributes();

	//ShowDotExpression();
    
    /* Buttons in the lower right hand corner */
    l_pcBoxSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxALL | wxEXPAND | wxALIGN_RIGHT, 5);
 
	SetSizerAndFit(l_pcBoxSizer);

	Layout();
    Centre(wxBOTH);
}

bool
SP_DLG_GraphProperties::ShowGraphAttributes()
{
    if (!m_pcGraph)
        return FALSE;

    return m_pcGraph->ShowGlobalOptions(this);
}

wxSizer*
SP_DLG_GraphProperties::AddSection(const wxString& p_sName)
{
    if (!m_pcSizer)
        return NULL;
    
    wxStaticBox* l_pcBox = new wxStaticBox(this, -1, p_sName);
    wxStaticBoxSizer* l_pcSizer = new wxStaticBoxSizer(l_pcBox, wxVERTICAL);
	l_pcSizer->SetMinSize(l_pcBox->GetEffectiveMinSize());
    m_pcSizer->Add(l_pcSizer, wxGBPosition(m_nCountAttr/5,m_nCountAttr%5), wxGBSpan(), wxEXPAND | wxALL, 5);

    m_nCountAttr++;
    return l_pcSizer;
}

wxCheckBox*
SP_DLG_GraphProperties::AddFlagLine(wxSizer* p_pcSection, const wxString& p_sName, bool p_bState)
{
    if (!p_pcSection)
        return NULL;

    wxCheckBox* l_pcBox = new wxCheckBox(this, -1, p_sName);
    l_pcBox->SetValue(p_bState);

    p_pcSection->Add(l_pcBox, 0, wxEXPAND | wxTOP | wxBOTTOM, 2);
	
    return l_pcBox;
}

bool
SP_DLG_GraphProperties::AddFlagLine(wxSizer* p_pcSection, SP_DS_Attribute* p_pcAttr)
{
    if (!p_pcSection || !p_pcAttr)
        return FALSE;

    m_mAttributes[AddFlagLine(p_pcSection, p_pcAttr->GetName(), p_pcAttr->GetShow())] = p_pcAttr;

    return TRUE;
}

//------------------------------------------------------------------------
void
SP_DLG_GraphProperties::OnDlgOk(wxCommandEvent& p_cEvent)
{
    map<wxCheckBox*, SP_DS_Attribute*>::iterator l_Iter;
    for (l_Iter = m_mAttributes.begin(); l_Iter != m_mAttributes.end(); ++l_Iter)
    {
        if ((*l_Iter).first && (*l_Iter).second)
            (*l_Iter).second->SetShow((*l_Iter).first->GetValue());
    }

	//SwitchDot();

    p_cEvent.Skip();
}

void
SP_DLG_GraphProperties::OnDlgCancel(wxCommandEvent& p_cEvent)
{
    p_cEvent.Skip();
}


void SP_DLG_GraphProperties::ShowDotExpression()
{
	 if (!m_pcSizer)
        return;

	 if (!m_pcGraph)
        return;

	  if( !(m_pcGraph->GetNetclass()->GetName() == SP_DS_COLCPN_CLASS || 
		  m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS   ||
		  m_pcGraph->GetNetclass()->GetName() == SP_DS_COLEPN_CLASS   ||
		   m_pcGraph->GetNetclass()->GetName() == SP_DS_COLHPN_CLASS   ||
		  m_pcGraph->GetNetclass()->GetName() == SP_DS_COLPN_CLASS))
	  {
		  return;
	  }
    
    wxStaticBox* l_pcBox = new wxStaticBox(this, -1, wxT("ColorPN-Specific"));
    wxStaticBoxSizer* l_pcSizer = new wxStaticBoxSizer(l_pcBox, wxVERTICAL);
	m_pcDotShowBox = new wxCheckBox(this, -1, wxT("Show Dot Expressions") );
	//l_pcSizer->SetMinSize(l_pcBox->GetEffectiveMinSize());
	l_pcSizer->Add(m_pcDotShowBox, 0, wxEXPAND | wxTOP | wxBOTTOM, 2);
    m_pcSizer->Add(l_pcSizer, wxGBPosition(m_nCountAttr/5,m_nCountAttr%5), wxGBSpan(), wxEXPAND | wxALL, 5);	

}


void SP_DLG_GraphProperties::SwitchDot()
{	
	 if (!m_pcGraph)
        return;

	  if( !(m_pcGraph->GetNetclass()->GetName() == SP_DS_COLCPN_CLASS || 
		  m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS   ||
		  m_pcGraph->GetNetclass()->GetName() == SP_DS_COLHPN_CLASS   ||
		  m_pcGraph->GetNetclass()->GetName() == SP_DS_COLEPN_CLASS   ||
		  m_pcGraph->GetNetclass()->GetName() == SP_DS_COLPN_CLASS))
	  {
		  return;
	  }

	 bool l_bShowDot;
	 if( m_pcDotShowBox )
	     l_bShowDot= m_pcDotShowBox->GetValue();
	 else
		 return;	

	 if(m_pcGraph->GetNetclass()->GetName() != SP_DS_COLCPN_CLASS)
	 {
		 SetEdgeShow( wxT("Edge"),l_bShowDot );
		 SetEdgeShow( wxT("Inhibitor Edge"),l_bShowDot );
		 SetEdgeShow( wxT("Equal Edge"),l_bShowDot );
		 SetEdgeShow( wxT("Reset Edge"),l_bShowDot );
		 SetEdgeShow( wxT("Read Edge"),l_bShowDot );
		 SetEdgeShow( wxT("Modifier Edge"),l_bShowDot );		 
	 }

	  if(m_pcGraph->GetNetclass()->GetName() == SP_DS_COLCPN_CLASS)
	 {
		 SetEdgeShow( SP_DS_EDGE,l_bShowDot );
		 SetEdgeShow( SP_DS_INHIBITOR_EDGE,l_bShowDot );
		 SetEdgeShow( SP_DS_READ_EDGE,l_bShowDot );
		 SetEdgeShow( SP_DS_MODIFIER_EDGE,l_bShowDot );
	 }

	  SP_Core::Instance()->GetRootDocument()->Modify(true);

}


void SP_DLG_GraphProperties::SetEdgeShow( wxString p_sEdgeClass, bool p_bShowDot )
{
	 SP_DS_ColListAttribute* l_pcColList;
	 SP_DS_Edgeclass* l_pcEdgeclass;
	 SP_ListEdge::const_iterator l_itElem;

	 l_pcEdgeclass= m_pcGraph->GetEdgeclass( p_sEdgeClass );
	if( l_pcEdgeclass )
	{	

		for (l_itElem = l_pcEdgeclass->GetElements()->begin(); l_itElem != l_pcEdgeclass->GetElements()->end(); ++l_itElem)
		{
			(*l_itElem)->Update(true);

			l_pcColList	= dynamic_cast< SP_DS_ColListAttribute* >(( *l_itElem )->GetAttribute(SP_DS_CPN_INSCRIPTION));
			wxString l_sExpression = l_pcColList->GetCell(0,1);
			if( l_sExpression.Find(wxT("dot")) != wxNOT_FOUND )
			{
				l_pcColList->SetShow(p_bShowDot);
			}
			
			SP_ListGraphic::const_iterator l_itGr; 
			for( l_itGr = l_pcColList->GetGraphics()->begin(); l_itGr != l_pcColList->GetGraphics()->end(); l_itGr++)
			{
				(*l_itGr)->SetShow( p_bShowDot );
				(*l_itGr)->Update(true);
			}

			
			(*l_itElem)->Update(true);
			
		}

	}

}

