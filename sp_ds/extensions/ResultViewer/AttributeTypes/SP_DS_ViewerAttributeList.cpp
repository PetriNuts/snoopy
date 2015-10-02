/*
 * SP_DS_ViewerAttributeList.cpp
 * SP_DS_ViewerAttributeText.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 15.06.2011
 * Short Description:
 *///==============================================================================
#include "sp_ds/extensions/ResultViewer/AttributeTypes/SP_DS_ViewerAttributeList.h"

SP_DS_ViewerAttributeList::SP_DS_ViewerAttributeList(const wxString& p_sName,const wxString& p_sCategory,
                                                     const wxString& p_sDisplayName)
:SP_DS_ViewerAttribute(p_sName,p_sCategory,p_sDisplayName),
 m_nSelection(0)
{
	      m_asPossibleValues.clear();
}

SP_DS_ViewerAttributeList::~SP_DS_ViewerAttributeList()
{

}

void SP_DS_ViewerAttributeList::Create(wxWindow* p_pcParent,wxSizer* p_pcSizer)
{
	SP_VectorString::const_iterator l_itValue; ;

	wxBoxSizer* l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

	//create a text control to hold the attribute value
	m_pcPossibleValuesBox=new wxComboBox( p_pcParent, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(150,-1), 0, NULL, wxCB_READONLY );

	l_pcRowSizer->Add(new wxStaticText(p_pcParent,wxID_ANY,m_sDisplayName),0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
	l_pcRowSizer->Add(m_pcPossibleValuesBox,0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);

	for(l_itValue=m_asPossibleValues.begin();l_itValue!=m_asPossibleValues.end();l_itValue++)
	{
		m_pcPossibleValuesBox->Append((*l_itValue));
	}

	m_pcPossibleValuesBox->SetSelection(m_nSelection);

	p_pcSizer->Add(l_pcRowSizer,0, wxALL, 5);
}

void SP_DS_ViewerAttributeList::Update()
{
            if(m_pcPossibleValuesBox!=NULL)
            {
            	m_sValue=m_pcPossibleValuesBox->GetValue();

            	m_nSelection=m_pcPossibleValuesBox->GetSelection();
            }
}

