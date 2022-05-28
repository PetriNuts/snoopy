//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: crohr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2014/02/17 14:25:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_gui/widgets/dialogs/SP_WDG_DialogChoice.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include <wx/choice.h>//
BEGIN_EVENT_TABLE(SP_WDG_DialogChoice, SP_WDG_DialogBase)
// intentionally left blank
END_EVENT_TABLE()

SP_WDG_DialogChoice::SP_WDG_DialogChoice(const wxString& p_sPage, unsigned int p_nOrdering) :
	SP_WDG_DialogBase(p_sPage, p_nOrdering), m_bFixedValues(false)
{
}

SP_WDG_DialogChoice::SP_WDG_DialogChoice(const wxString& p_sPage, const wxArrayString& p_sValues, unsigned int p_nOrdering):
	SP_WDG_DialogBase(p_sPage, p_nOrdering), m_pcChoiceValues(p_sValues), m_bFixedValues(true)
{
}

SP_WDG_DialogChoice::~SP_WDG_DialogChoice()
{
}

SP_WDG_DialogBase*
SP_WDG_DialogChoice::Clone()
{
	if(m_bFixedValues)
	{
		return new SP_WDG_DialogChoice(GetName(), m_pcChoiceValues, GetDialogOrdering());
	}
	else
	{
		return new SP_WDG_DialogChoice(GetName(), GetDialogOrdering());
	}
}

bool SP_WDG_DialogChoice::AddToDialog(const SP_ListAttribute* p_ptlAttributes, SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview)
{
	CHECK_POINTER(p_ptlAttributes, return FALSE);
	CHECK_BOOL(!p_ptlAttributes->empty(), return FALSE);
	CHECK_POINTER(p_pcDlg, return FALSE);
	m_pcDlg = p_pcDlg;

	m_pcChoice.clear();

    // remember the list of attributes, we display
    m_tlAttributes = *p_ptlAttributes;
    m_bMultiple = (m_tlAttributes.size() > 1);

    LoadData();

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

			wxString l_pchValue;
			l_pchValue = l_pcAttr->GetValueString();
			wxString l_sName = l_pcAttr->GetParentName();
			l_pcFlexGridSizer->Add(new wxStaticText(l_pcPage, -1, l_sName), 0, wxALL , 5);
			
			///////////////////////
			if (l_pcAttr->GetName() == _T("Node Type"))
			{//by george for node type attribute
				l_pcAttr->SetValueString(_T(""));
				l_pchValue = l_pcAttr->GetValueString();
				LoadDataForNodeTypeAttribute(l_pcAttr);
			}
			//////////////////////
			wxChoice* l_pcChoice = new wxChoice(l_pcPage, -1, wxDefaultPosition, wxDefaultSize, m_pcChoiceValues);
			
		
			
			int l_nSelected = l_pcChoice->FindString(l_pchValue);
			if (l_nSelected != wxNOT_FOUND)
			{
				l_pcChoice->SetSelection(l_nSelected);
			}
			m_pcChoice.push_back(l_pcChoice);
			wxBoxSizer* l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
			l_pcSizer->Add(l_pcChoice, 1, wxALL , 5);

			AddShowFlag(l_pcPage, l_pcSizer, l_pcAttr);
			l_pcFlexGridSizer->Add(l_pcSizer, 1, wxEXPAND);
    	}
		l_pcPage->AddControl(l_pcFlexGridSizer, 0, wxEXPAND);
		m_pcPairPage2Choices.first = l_sPage;
		m_pcPairPage2Choices.second = m_pcChoice;
    }
    else
    {
        SP_DS_Attribute* l_pcAttr = m_tlAttributes.front();
        wxBoxSizer* l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
        wxString l_sPage = GetName() + wxT(":") + l_pcAttr->GetParent()->GetClassName();
        SP_WDG_NotebookPage* l_pcPage = p_pcDlg->AddPage(l_sPage, GetDialogOrdering());
        CHECK_POINTER(l_pcPage, return FALSE);

		if (l_pcAttr->GetName() == _T("Node Type"))
		{
			l_pcAttr->SetValueString(_T(""));
			LoadDataForNodeTypeAttribute(p_ptlAttributes);
		}

    	l_pcSizer->Add(new wxStaticText(l_pcPage, -1, l_pcAttr->GetName()), 0, wxALL , 5);
		wxChoice* l_pcChoice = new wxChoice(l_pcPage, -1, wxDefaultPosition, wxDefaultSize, m_pcChoiceValues);
		if(m_bMultiple)
		{
			l_pcChoice->Insert(SP_WILDCARD, 0);
			l_pcChoice->SetSelection(0);
		}
		else
		{
	    	wxString l_pchValue = l_pcAttr->GetValueString();

			int l_nSelected = l_pcChoice->FindString(l_pchValue);
			if (l_nSelected != wxNOT_FOUND)
			{
				l_pcChoice->SetSelection(l_nSelected);
			}
		}
		m_pcChoice.push_back(l_pcChoice);
        l_pcSizer->Add(l_pcChoice, 1, wxALL , 5);

        AddShowFlag(l_pcPage, l_pcSizer, l_pcAttr);
        l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);
	 
    }

	return TRUE;
}
void SP_WDG_DialogChoice::LoadDataForNodeTypeAttribute(  SP_DS_Attribute* p_ptlAttributes)
{
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	wxString l_sNetClass = l_pcGraph->GetNetclass()->GetName();
	if (!l_sNetClass.Contains(wxT("Hybrid")))
	{//one choice node type
		//SP_ListAttribute::iterator it = m_tlAttributes.begin();
		//SP_ListAttribute::iterator end = m_tlAttributes.end();
		 
			SP_DS_Attribute* l_pcAttr = p_ptlAttributes;

			wxString l_pchValue = l_pcAttr->GetValueString();
			wxString l_sName = l_pcAttr->GetParentName();
			unsigned int l_nArraysize = 1;
			m_pcChoiceValues.Clear();
			m_pcChoiceValues.Alloc(l_nArraysize);

			m_pcChoiceValues.Add(l_pchValue);

	 
	}
	else
	{//multi choices node type
		SP_ListAttribute::iterator it = m_tlAttributes.begin();
		SP_ListAttribute::iterator end = m_tlAttributes.end();
		 
			SP_DS_Attribute* l_pcAttr = p_ptlAttributes;

			wxString l_pchValue = l_pcAttr->GetValueString();
			wxString l_sName = l_pcAttr->GetParentName();
			unsigned int l_nArraysize;
			if (l_pchValue.Contains(wxT("Place")))
			{
				l_nArraysize = 2;
				m_pcChoiceValues.Clear();
				m_pcChoiceValues.Alloc(l_nArraysize);
				if (l_pchValue == _T("Discrete Place"))
				{
					m_pcChoiceValues.Add(l_pchValue);
					m_pcChoiceValues.Add(_T("Continuous Place"));
				}
				else if (l_pchValue == _T("Continuous Place"))
				{
					m_pcChoiceValues.Add(l_pchValue);
					m_pcChoiceValues.Add(_T("Discrete Place"));
				}

			}
			else if (l_pchValue.Contains(wxT("Transition")))
			{
				l_nArraysize = 5;
				m_pcChoiceValues.Clear();
				m_pcChoiceValues.Alloc(l_nArraysize);
				if (l_pchValue == _T("Continuous Transition"))
				{
					m_pcChoiceValues.Add(l_pchValue);
					m_pcChoiceValues.Add(_T("Stochastic Transition"));
					m_pcChoiceValues.Add(_T("Deterministic Transition"));
					m_pcChoiceValues.Add(_T("Immediate Transition"));
					m_pcChoiceValues.Add(_T("Scheduled Transition"));
				}
				else if (l_pchValue == _T("Stochastic Transition"))
				{
					m_pcChoiceValues.Add(l_pchValue);
					m_pcChoiceValues.Add(_T("Continuous Transition"));
					m_pcChoiceValues.Add(_T("Deterministic Transition"));
					m_pcChoiceValues.Add(_T("Immediate Transition"));
					m_pcChoiceValues.Add(_T("Scheduled Transition"));
				}
				else if (l_pchValue == _T("Deterministic Transition"))
				{
					m_pcChoiceValues.Add(l_pchValue);
					m_pcChoiceValues.Add(_T("Continuous Transition"));
					m_pcChoiceValues.Add(_T("Stochastic Transition"));
					m_pcChoiceValues.Add(_T("Immediate Transition"));
					m_pcChoiceValues.Add(_T("Scheduled Transition"));
				}
				else if (l_pchValue == _T("Immediate Transition"))
				{
					m_pcChoiceValues.Add(l_pchValue);
					m_pcChoiceValues.Add(_T("Continuous Transition"));
					m_pcChoiceValues.Add(_T("Stochastic Transition"));
					m_pcChoiceValues.Add(_T("Deterministic Transition"));
					m_pcChoiceValues.Add(_T("Scheduled Transition"));
				}
				else if (l_pchValue == _T("Scheduled Transition"))
				{
					m_pcChoiceValues.Add(l_pchValue);
					m_pcChoiceValues.Add(_T("Continuous Transition"));
					m_pcChoiceValues.Add(_T("Stochastic Transition"));
					m_pcChoiceValues.Add(_T("Deterministic Transition"));
					m_pcChoiceValues.Add(_T("Immediate Transition"));
				}

			}
			else
			{

			}

	}
}

void SP_WDG_DialogChoice::LoadDataForNodeTypeAttribute(const SP_ListAttribute* p_ptlAttributes)
{
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	wxString l_sNetClass = l_pcGraph->GetNetclass()->GetName();

	if (!l_sNetClass.Contains(wxT("Hybrid")) && !l_sNetClass.Contains(wxT("Stochastic")))
	{//one choice node type
		SP_ListAttribute::iterator it = m_tlAttributes.begin();
		SP_ListAttribute::iterator end = m_tlAttributes.end();
		for (; it != end; ++it)
		{
			SP_DS_Attribute* l_pcAttr = *it;
			if(l_pcAttr)
			{
			wxString l_pchValue = l_pcAttr->GetValueString();
			unsigned int l_nArraysize = 1;
			m_pcChoiceValues.Clear();
			m_pcChoiceValues.Alloc(l_nArraysize);

			m_pcChoiceValues.Add(l_pchValue);
			}

		}
	}
	else
	{//multi choices node type
		SP_ListAttribute::iterator it = m_tlAttributes.begin();
		SP_ListAttribute::iterator end = m_tlAttributes.end();
		for (; it != end; ++it)
		{
			SP_DS_Attribute* l_pcAttr = *it;

			if(l_pcAttr)
			{
			m_pcChoiceValues.Clear();
			wxString l_pchValue = l_pcAttr->GetValueString();
			unsigned int l_nArraysize;
			if (l_pchValue.Contains(wxT("Place")) && l_sNetClass.Contains(wxT("Hybrid")))
			{
				l_nArraysize = 2;
				m_pcChoiceValues.Clear();
				m_pcChoiceValues.Alloc(l_nArraysize);
				if (l_pchValue == _T("Discrete Place"))
				{
					m_pcChoiceValues.Add(l_pchValue);
					m_pcChoiceValues.Add(_T("Continuous Place"));
				}
				else if (l_pchValue == _T("Continuous Place"))
				{
					m_pcChoiceValues.Add(l_pchValue);
					m_pcChoiceValues.Add(_T("Discrete Place"));
				}
				 
			}
			else if (l_pchValue.Contains(wxT("Transition")) &&  l_sNetClass.Contains(wxT("Hybrid")))
			{
				l_nArraysize = 5;
				m_pcChoiceValues.Clear();
				m_pcChoiceValues.Alloc(l_nArraysize);
				if (l_pchValue == _T("Continuous Transition"))
				{
					m_pcChoiceValues.Add(l_pchValue);
					m_pcChoiceValues.Add(_T("Stochastic Transition"));
					m_pcChoiceValues.Add(_T("Deterministic Transition"));
					m_pcChoiceValues.Add(_T("Immediate Transition"));
					m_pcChoiceValues.Add(_T("Scheduled Transition"));
				}
				else if (l_pchValue == _T("Stochastic Transition"))
				{
					m_pcChoiceValues.Add(l_pchValue);
					m_pcChoiceValues.Add(_T("Continuous Transition"));
					m_pcChoiceValues.Add(_T("Deterministic Transition"));
					m_pcChoiceValues.Add(_T("Immediate Transition"));
					m_pcChoiceValues.Add(_T("Scheduled Transition"));
				}
				else if (l_pchValue == _T("Deterministic Transition"))
				{
					m_pcChoiceValues.Add(l_pchValue);
					m_pcChoiceValues.Add(_T("Continuous Transition"));
					m_pcChoiceValues.Add(_T("Stochastic Transition"));
					m_pcChoiceValues.Add(_T("Immediate Transition"));
					m_pcChoiceValues.Add(_T("Scheduled Transition"));
				}
				else if (l_pchValue == _T("Immediate Transition"))
				{
					m_pcChoiceValues.Add(l_pchValue);
					m_pcChoiceValues.Add(_T("Continuous Transition"));
					m_pcChoiceValues.Add(_T("Stochastic Transition"));
					m_pcChoiceValues.Add(_T("Deterministic Transition"));
					m_pcChoiceValues.Add(_T("Scheduled Transition"));
				}
				else if (l_pchValue == _T("Scheduled Transition"))
				{
					m_pcChoiceValues.Add(l_pchValue);
					m_pcChoiceValues.Add(_T("Continuous Transition"));
					m_pcChoiceValues.Add(_T("Stochastic Transition"));
					m_pcChoiceValues.Add(_T("Deterministic Transition"));
					m_pcChoiceValues.Add(_T("Immediate Transition"));
				}
				
			}
			else if(l_pchValue.Contains(wxT("Stochastic Transition")) && l_sNetClass.Contains(wxT("Stochastic")))
			{//conversion between special transitions for stoch nets
				m_pcChoiceValues.Add(l_pchValue);
				m_pcChoiceValues.Add(_T("Deterministic Transition"));
				m_pcChoiceValues.Add(_T("Immediate Transition"));
				m_pcChoiceValues.Add(_T("Scheduled Transition"));
			}
			else if (l_pchValue.Contains(wxT("Immediate Transition")) && l_sNetClass.Contains(wxT("Stochastic")))
			{
				m_pcChoiceValues.Add(l_pchValue);
				m_pcChoiceValues.Add(_T("Stochastic Transition"));
				m_pcChoiceValues.Add(_T("Deterministic Transition"));
				m_pcChoiceValues.Add(_T("Scheduled Transition"));
			}
			else if (l_pchValue.Contains(wxT("Deterministic Transition")) && l_sNetClass.Contains(wxT("Stochastic")))
			{
				m_pcChoiceValues.Add(l_pchValue);
				m_pcChoiceValues.Add(_T("Stochastic Transition"));
				m_pcChoiceValues.Add(_T("Immediate Transition"));
				m_pcChoiceValues.Add(_T("Scheduled Transition"));
			}
			else if (l_pchValue.Contains(wxT("Scheduled Transition")) && l_sNetClass.Contains(wxT("Stochastic")))
			{
				m_pcChoiceValues.Add(l_pchValue);
				m_pcChoiceValues.Add(_T("Stochastic Transition"));
				m_pcChoiceValues.Add(_T("Immediate Transition"));
				m_pcChoiceValues.Add(_T("Deterministic Transition"));
			}
			else if (l_pchValue.Contains(wxT("Place")) && l_sNetClass.Contains(wxT("Stochastic")))
			{
				m_pcChoiceValues.Add(wxT("Discrete Place"));
			}
			
			}
		}

	}
}
void SP_WDG_DialogChoice::LoadData()
{
	if(m_bFixedValues)
	{
		return;
	}
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Metadataclass* l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	if(!l_pcMetadataclass)
	{
		return;
	}
	unsigned l_nArraysize = 0;

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute *l_pcBasicColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
	l_nArraysize += l_pcBasicColList->GetRowCount();

	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	if(!l_pcMetadataclass)//by george
	{
		return;
	}
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute *l_pcStructColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));
	l_nArraysize += l_pcStructColList->GetRowCount();


	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_ALIASCOLORSETCLASS);
	if(!l_pcMetadataclass)//by george
	{
			return;
	}
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute *l_pcAliasColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AliasColorsetList")));
	l_nArraysize += l_pcAliasColList->GetRowCount();

	m_pcChoiceValues.Clear();
	m_pcChoiceValues.Alloc(l_nArraysize );

	for (unsigned int i = 0; i < l_pcBasicColList->GetRowCount(); i++)
	{
		m_pcChoiceValues.Add(l_pcBasicColList->GetCell(i,0));
	}
	for (unsigned int i = 0; i < l_pcStructColList->GetRowCount(); i++)
	{
		m_pcChoiceValues.Add(l_pcStructColList->GetCell(i,0));
	}
	for (unsigned int i = 0; i < l_pcAliasColList->GetRowCount(); i++)
	{
		m_pcChoiceValues.Add(l_pcAliasColList->GetCell(i,0));
	}
}

bool SP_WDG_DialogChoice::OnDlgOk()
{
    CHECK_BOOL(!m_tlAttributes.empty(), return FALSE);
    CHECK_BOOL(!m_pcChoice.empty(), return FALSE);

    bool l_bReturn = TRUE;
    if(m_tlAttributes.size() == m_pcChoice.size())
    {
        SP_ListAttribute::const_iterator l_Iter;
        unsigned i = 0;
        for (l_Iter = m_tlAttributes.begin(); l_Iter != m_tlAttributes.end(); ++l_Iter, ++i)
        {
        	int l_nSelected = m_pcChoice[i]->GetSelection();
            l_bReturn &= (*l_Iter)->SetValueString(m_pcChoice[i]->GetString(l_nSelected));
        }
    }
    else
    {
    	int l_nSelected = m_pcChoice.front()->GetSelection();
    	wxString l_sValue = m_pcChoice.front()->GetString(l_nSelected);
        if (m_bMultiple && (l_sValue.Cmp(SP_WILDCARD) == 0))
            return SP_WDG_DialogBase::OnDlgOk();

        SP_ListAttribute::const_iterator l_Iter;
        for (l_Iter = m_tlAttributes.begin(); l_Iter != m_tlAttributes.end(); ++l_Iter)
        {
            l_bReturn &= (*l_Iter)->SetValueString(l_sValue);
        }
    }



    m_pcChoice.clear();
    return l_bReturn && SP_WDG_DialogBase::OnDlgOk();

}
wxString SP_WDG_DialogChoice::GetCurentSelection(bool p_bIsOverviewShow)//by george
{
	if (p_bIsOverviewShow)
	{
		int l_nSelected = m_pcChoice.front()->GetSelection();
		if (l_nSelected > 0) --l_nSelected;
		if (l_nSelected == m_pcChoiceValues.size()) l_nSelected = l_nSelected - 1;
		if (m_pcChoiceValues.size() >= 0 && l_nSelected >= 0)
			return m_pcChoiceValues[l_nSelected];
		else
			return wxT("");
	}
	int l_nSelected = m_pcChoice.front()->GetSelection();
	if (l_nSelected == m_pcChoiceValues.size()) l_nSelected = l_nSelected - 1;
	if (m_pcChoiceValues.size() >= 0 && l_nSelected >= 0)
		return m_pcChoiceValues[l_nSelected];
	else
		return wxT("");
}
 
