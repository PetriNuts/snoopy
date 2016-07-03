//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////

#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"

SP_WDG_DialogMultiline::SP_WDG_DialogMultiline(const wxString& p_sPage, unsigned int p_nOrdering)
:SP_WDG_DialogText(p_sPage, p_nOrdering)
{
}

SP_WDG_DialogMultiline::~SP_WDG_DialogMultiline()
{
}

SP_WDG_DialogBase*
SP_WDG_DialogMultiline::Clone()
{
    return new SP_WDG_DialogMultiline(GetName(), GetDialogOrdering());
}

bool
SP_WDG_DialogMultiline::AddToDialog(const SP_ListAttribute* p_ptlAttributes, 
                                    SP_DLG_ShapeProperties* p_pcDlg,
                                    bool p_bOverview)
{
    CHECK_POINTER(p_ptlAttributes, return FALSE);
    CHECK_BOOL(!p_ptlAttributes->empty(), return FALSE);
    CHECK_POINTER(p_pcDlg, return FALSE);

    m_pcTextCtrl.clear();

    // remember the list of attributes, we display
    m_tlAttributes = *p_ptlAttributes;
    m_bMultiple = (m_tlAttributes.size() > 1);

    if(p_bOverview)
    {
		wxString l_sPage = m_tlAttributes.front()->GetName() + wxT(":") + m_tlAttributes.front()->GetParent()->GetClassName();

		SP_WDG_NotebookPage* l_pcPage = p_pcDlg->AddPage(l_sPage , GetDialogOrdering());
		CHECK_POINTER(l_pcPage, return FALSE);

		wxFlexGridSizer* l_pcFlexGridSizer = new wxFlexGridSizer(2);
		l_pcFlexGridSizer->AddGrowableCol(1,1);

    	SP_ListAttribute::iterator it = m_tlAttributes.begin();
    	SP_ListAttribute::iterator end = m_tlAttributes.end();
    	for(; it != end; ++it)
    	{
			SP_DS_Attribute* l_pcAttr = *it;

			wxString l_pchValue = l_pcAttr->GetValueString();
			wxString l_sName = l_pcAttr->GetParentName();
			l_pcFlexGridSizer->Add(new wxStaticText(l_pcPage, -1, l_sName), 0, wxALL , 5);
			wxTextCtrl* l_pcTextCtrl = new wxTextCtrl(l_pcPage, -1, l_pchValue, wxDefaultPosition,
					wxSize(12,50),//wxDefaultSize,
					wxTE_MULTILINE);
			m_pcTextCtrl.push_back(l_pcTextCtrl);
			wxBoxSizer* l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
			l_pcSizer->Add(l_pcTextCtrl, 1, wxALL , 5);

			AddShowFlag(l_pcPage, l_pcSizer, l_pcAttr);
			l_pcFlexGridSizer->Add(l_pcSizer, 1, wxEXPAND | wxALL);
    	}
		l_pcPage->AddControl(l_pcFlexGridSizer, 0, wxEXPAND);
    }
    else
    {
        SP_DS_Attribute* l_pcAttr = m_tlAttributes.front();
        wxBoxSizer* l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
        wxString l_sPage = GetName() + wxT(":") + l_pcAttr->GetParent()->GetClassName();
        SP_WDG_NotebookPage* l_pcPage = p_pcDlg->AddPage(l_sPage, GetDialogOrdering());
        CHECK_POINTER(l_pcPage, return FALSE);

    	wxString l_pchValue = (m_bMultiple ? SP_WILDCARD : l_pcAttr->GetValueString());

        l_pcSizer->Add(new wxStaticText(l_pcPage, -1, l_pcAttr->GetName()), 1, wxALL , 5);
        AddShowFlag(l_pcPage, l_pcSizer, l_pcAttr);
        l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

        l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
        wxTextCtrl* l_pcTextCtrl = new wxTextCtrl(l_pcPage, -1, l_pchValue, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
		m_pcTextCtrl.push_back(l_pcTextCtrl);
        l_pcSizer->Add(l_pcTextCtrl, 1, wxEXPAND | wxALL, 5);

        l_pcPage->AddControl(l_pcSizer, 1, wxEXPAND);
    }

    return TRUE;
}
