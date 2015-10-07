//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_gui/widgets/dialogs/SP_WDG_DialogBool.h"

SP_WDG_DialogBool::SP_WDG_DialogBool(const wxString& p_sPage, unsigned int p_nOrdering)
:SP_WDG_DialogBase(p_sPage, p_nOrdering)
{
}

SP_WDG_DialogBool::~SP_WDG_DialogBool()
{
}

SP_WDG_DialogBase*
SP_WDG_DialogBool::Clone()
{
    return new SP_WDG_DialogBool(GetName(), GetDialogOrdering());
}

bool
SP_WDG_DialogBool::AddToDialog(const SP_ListAttribute* p_ptlAttributes,
                               SP_DLG_ShapeProperties* p_pcDlg,
                               bool p_bOverview)
{
    CHECK_POINTER(p_ptlAttributes, return FALSE);
    CHECK_POINTER(p_pcDlg, return FALSE);

    // no attributes, nothing to show
    CHECK_BOOL(!p_ptlAttributes->empty(), return FALSE);
    m_pcBoolCtrl.clear();

    // remember the list of attributes, we display
    m_tlAttributes = *p_ptlAttributes;
	m_bMultiple = (p_ptlAttributes->size() > 1);

	// set the value
	if (p_bOverview)
	{
		wxString l_sPage = m_tlAttributes.front()->GetName() + wxT(":") + m_tlAttributes.front()->GetParent()->GetClassName();

		SP_WDG_NotebookPage* l_pcPage = p_pcDlg->AddPage(l_sPage , GetDialogOrdering());
		CHECK_POINTER(l_pcPage, return FALSE);

		wxFlexGridSizer* l_pcFlexGridSizer = new wxFlexGridSizer(2);
		//l_pcFlexGridSizer->AddGrowableCol(1,1);

    	SP_ListAttribute::iterator it = m_tlAttributes.begin();
    	SP_ListAttribute::iterator end = m_tlAttributes.end();
    	for(; it != end; ++it)
    	{
			SP_DS_Attribute* l_pcAttr = *it;

			wxString l_pchValue = l_pcAttr->GetValueString();
			wxString l_sName = l_pcAttr->GetParentName();
			l_pcFlexGridSizer->Add(new wxStaticText(l_pcPage, -1, l_sName), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	        wxCheckBox* l_pcBoolCtrl = new wxCheckBox(l_pcPage
										  ,-1
										  ,wxT("")
										  ,wxDefaultPosition
										  ,wxDefaultSize
										  ,wxCHK_2STATE | wxBU_NOTEXT
										  );
	        m_pcBoolCtrl.push_back(l_pcBoolCtrl);
			wxBoxSizer* l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
			l_pcBoolCtrl->SetValue(l_pchValue.Cmp(wxT("1")) == 0);

			l_pcSizer->Add(l_pcBoolCtrl, 0, wxALL, 5);

			l_pcFlexGridSizer->Add(l_pcSizer, 1, wxEXPAND | wxALL | wxALIGN_CENTER_VERTICAL);
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

        l_pcSizer->Add(new wxStaticText(l_pcPage, -1, l_pcAttr->GetName()), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
        wxCheckBox* l_pcBoolCtrl = NULL;
    	// set the value
    	if (m_bMultiple)
    	{
    		l_pcBoolCtrl = new wxCheckBox(l_pcPage
										  ,-1
										  ,wxT("")
										  ,wxDefaultPosition
										  ,wxDefaultSize
										  ,wxCHK_3STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER
										  );
    		l_pcBoolCtrl->Set3StateValue(wxCHK_UNDETERMINED);
    	}
    	else
    	{
    		l_pcBoolCtrl = new wxCheckBox(l_pcPage
										  ,-1
										  ,wxT("")
										  ,wxDefaultPosition
										  ,wxDefaultSize
										  ,wxCHK_2STATE | wxBU_NOTEXT
										  );
    		wxString l_pchValue = l_pcAttr->GetValueString();
    		l_pcBoolCtrl->SetValue(l_pchValue.Cmp(wxT("1")) == 0);
        }
        m_pcBoolCtrl.push_back(l_pcBoolCtrl);

		l_pcSizer->Add(l_pcBoolCtrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	    l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);
    }

    return TRUE;
}

bool
SP_WDG_DialogBool::OnDlgOk()
{
    CHECK_BOOL(!m_tlAttributes.empty(), return FALSE);
    CHECK_BOOL(!m_pcBoolCtrl.empty(), return FALSE);

    bool l_bReturn = TRUE;
    if(m_tlAttributes.size() == m_pcBoolCtrl.size())
    {
        SP_ListAttribute::const_iterator l_Iter;
        unsigned i = 0;
        for (l_Iter = m_tlAttributes.begin(); l_Iter != m_tlAttributes.end(); ++l_Iter, ++i)
        {
        	if (wxCHK_UNDETERMINED != m_pcBoolCtrl[i]->Get3StateValue() )
        	{
                wxString l_sBool = wxT("0");
                if(m_pcBoolCtrl[i]->GetValue())
                {
                	l_sBool = wxT("1");
                }
            	l_bReturn &= (*l_Iter)->SetValueString(l_sBool);
        	}
        }
    }
    else
    {
    	if (wxCHK_UNDETERMINED == m_pcBoolCtrl.front()->Get3StateValue() )
    	{
    		return TRUE;
    	}

        bool l_bReturn = TRUE;
        SP_ListAttribute::const_iterator l_Iter;
        for (l_Iter = m_tlAttributes.begin(); l_Iter != m_tlAttributes.end(); ++l_Iter)
        {
            wxString l_sBool = wxT("0");
            if(m_pcBoolCtrl.front()->GetValue())
            {
            	l_sBool = wxT("1");
            }
        	l_bReturn &= (*l_Iter)->SetValueString(l_sBool);
        }
    }

    m_pcBoolCtrl.clear();

    return l_bReturn && SP_WDG_DialogBase::OnDlgOk();
}

int
SP_WDG_DialogBool::GetEvalState()
{
    if (m_pcBoolCtrl.front())
    {
		return 1;
    }
    return 0;
}
