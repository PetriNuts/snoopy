//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: alextov $
// $Version: 0.0 $
// $Date: 2005/06/03 $
// Short Description: dialog for unsigned integers
//////////////////////////////////////////////////////////////////////

#include "sp_gui/widgets/dialogs/SP_WDG_DialogUnsigned.h"

SP_WDG_DialogUnsigned::SP_WDG_DialogUnsigned(const wxString& p_sPage, unsigned p_nMin, unsigned p_nMax, unsigned int p_nOrdering)
  : SP_WDG_DialogBase(p_sPage, p_nOrdering)
{
  m_nMax = (p_nMax > INT_MAX) ? INT_MAX : p_nMax;
  m_nMin = (p_nMin > INT_MAX) ? INT_MAX : p_nMin;
}

SP_WDG_DialogUnsigned::~SP_WDG_DialogUnsigned()
{
}

SP_WDG_DialogBase*
SP_WDG_DialogUnsigned::Clone()
{
    return new SP_WDG_DialogUnsigned(GetName(), m_nMin, m_nMax, GetDialogOrdering());
}

bool
SP_WDG_DialogUnsigned::AddToDialog(const SP_ListAttribute* p_ptlAttributes,
                               SP_DLG_ShapeProperties* p_pcDlg,
                               bool p_bOverview)
{
    CHECK_POINTER(p_ptlAttributes, return FALSE);
    CHECK_BOOL(!p_ptlAttributes->empty(), return FALSE);
    CHECK_POINTER(p_pcDlg, return FALSE);

    m_pcSpinCtrl.clear();

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
			l_pcFlexGridSizer->Add(new wxStaticText(l_pcPage, -1, l_sName), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	        wxSpinCtrl* l_pcSpinCtrl = new wxSpinCtrl(l_pcPage);
	        m_pcSpinCtrl.push_back(l_pcSpinCtrl);

	        l_pcSpinCtrl->SetRange(m_nMin, m_nMax);
	        l_pcSpinCtrl->SetValue(l_pchValue);

			wxBoxSizer* l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
	        l_pcSizer->Add(l_pcSpinCtrl, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

			AddShowFlag(l_pcPage, l_pcSizer, l_pcAttr);
			l_pcFlexGridSizer->Add(l_pcSizer, 1, wxEXPAND);
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

    	wxString l_pchValue = (m_bMultiple ? wxT("-1") : l_pcAttr->GetValueString());

        l_pcSizer->Add(new wxStaticText(l_pcPage, -1, l_pcAttr->GetName()), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
        wxSpinCtrl* l_pcSpinCtrl = new wxSpinCtrl(l_pcPage);
        m_pcSpinCtrl.push_back(l_pcSpinCtrl);
        // -1 means unmodified
        if (m_bMultiple)
        {
        	l_pcSpinCtrl->SetRange(-1, m_nMax);
        }
        else
        {
        	l_pcSpinCtrl->SetRange(m_nMin, m_nMax);
        }

        l_pcSpinCtrl->SetValue(l_pchValue);

        l_pcSizer->Add(l_pcSpinCtrl, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        AddShowFlag(l_pcPage, l_pcSizer, l_pcAttr);
        l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);
    }

    return TRUE;
}

bool
SP_WDG_DialogUnsigned::OnDlgOk()
{
    CHECK_BOOL(!m_tlAttributes.empty(), return FALSE);
    CHECK_BOOL(!m_pcSpinCtrl.empty(), return FALSE);

    bool l_bReturn = TRUE;

    if(m_tlAttributes.size() == m_pcSpinCtrl.size())
    {
        SP_ListAttribute::const_iterator l_Iter;
        unsigned i = 0;
        for (l_Iter = m_tlAttributes.begin(); l_Iter != m_tlAttributes.end(); ++l_Iter, ++i)
        {
        	if (m_pcSpinCtrl[i]->GetValue() < (int) m_nMin)
			{
				m_pcSpinCtrl[i]->SetValue(m_nMin);
			}
        	wxString l_sValue = wxString::Format(wxT("%d"), m_pcSpinCtrl[i]->GetValue());

        	l_bReturn &= (*l_Iter)->SetValueString(l_sValue);
        }
    }
    else
    {
    	if (m_bMultiple)
    	{
    		if (m_pcSpinCtrl[0]->GetValue() < 0)
    		{ // -1 means unmodified
    			return SP_WDG_DialogBase::OnDlgOk();
    		}
    		else if (m_pcSpinCtrl[0]->GetValue() < (int) m_nMin)
    		{
    			m_pcSpinCtrl[0]->SetValue(m_nMin);
    		}
    	}

        wxString l_sValue = wxString::Format(wxT("%d"), m_pcSpinCtrl[0]->GetValue());


        SP_ListAttribute::const_iterator l_Iter;
        for (l_Iter = m_tlAttributes.begin(); l_Iter != m_tlAttributes.end(); ++l_Iter)
        {
            l_bReturn &= (*l_Iter)->SetValueString(l_sValue);
        }
    }

    m_pcSpinCtrl.clear();
    return l_bReturn && SP_WDG_DialogBase::OnDlgOk();
}

