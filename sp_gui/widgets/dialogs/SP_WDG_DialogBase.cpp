//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_gui/widgets/dialogs/SP_WDG_DialogBase.h"
#include "sp_gui/widgets/SP_WDG_NotebookPage.h"

BEGIN_EVENT_TABLE(SP_WDG_DialogBase, wxEvtHandler)
END_EVENT_TABLE()

SP_WDG_DialogBase::SP_WDG_DialogBase(const wxString& p_sPage, unsigned int p_nOrdering)
:SP_Name(p_sPage),
m_bMultiple(FALSE),
m_nDialogID(g_nDialogID+=10),
m_nDialogOrdering(p_nOrdering)
{
}

SP_WDG_DialogBase::~SP_WDG_DialogBase()
{
    CleanUpAttributes();
}

SP_WDG_DialogBase*
SP_WDG_DialogBase::Clone()
{
    return new SP_WDG_DialogBase(GetName(), GetDialogOrdering());
}

bool
SP_WDG_DialogBase::AddToDialog(const SP_ListAttribute* p_ptlAttributes,
                               SP_DLG_ShapeProperties* p_pcDlg,
                               bool p_bOverview)
{
    return TRUE;
}

bool
SP_WDG_DialogBase::AddShowFlag(SP_WDG_NotebookPage* p_pcPage, wxBoxSizer* p_pcSizer, SP_DS_Attribute* p_pcAttr)
{
    if (!p_pcPage || !p_pcSizer || !p_pcAttr || !p_pcAttr->GetGraphics())
        return FALSE;

    wxCheckBox* l_pcShow = new wxCheckBox(p_pcPage, -1, wxT("Show"));
    m_pcShow.push_back(l_pcShow);
    if (!p_pcAttr->GetGraphics()->empty())
        l_pcShow->SetValue(p_pcAttr->GetShow());
    else
        l_pcShow->Enable(FALSE);

    p_pcSizer->Add(l_pcShow, 0, wxALL , 5);

    return TRUE;
}

bool
SP_WDG_DialogBase::OnDlgOk()
{
    m_bMultiple = FALSE;
    bool l_bReturn = TRUE;

    if(m_tlAttributes.size() == m_pcShow.size())
    {
		SP_ListAttribute::const_iterator l_Iter;
		unsigned i = 0;
        for (l_Iter = m_tlAttributes.begin(); l_Iter != m_tlAttributes.end(); ++i, ++l_Iter)
		{
			l_bReturn &= (*l_Iter)->SetShow(m_pcShow[i]->GetValue());
			l_bReturn &= (*l_Iter)->Update(TRUE);
		}
    }
    else if (!m_pcShow.empty())
    {
		SP_ListAttribute::const_iterator l_Iter;
		for (l_Iter = m_tlAttributes.begin(); l_Iter != m_tlAttributes.end(); ++l_Iter)
		{
			l_bReturn &= (*l_Iter)->SetShow(m_pcShow.front()->GetValue());
			l_bReturn &= (*l_Iter)->Update(TRUE);
		}
    }
    m_pcShow.clear();
    return TRUE;
}

bool
SP_WDG_DialogBase::CleanUpAttributes()
{
    SP_ListAttribute::iterator l_Iter = m_tlAttributes.begin();
    while (l_Iter != m_tlAttributes.end())
    {
        SP_DS_Attribute* l_pcAttribute = (*l_Iter);
        if (l_pcAttribute->GetDelete())
        {
            m_tlAttributes.erase(l_Iter);
            wxDELETE(l_pcAttribute);
            l_Iter = m_tlAttributes.begin();
        }
        else
            l_Iter++;
    }

    return TRUE;
}
