/*
 * SP_DS_ViewerAttributeText.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 14.06.2011
 * Short Description:
 * *///=========================================================

#include "sp_ds/extensions/ResultViewer/AttributeTypes/SP_DS_ViewerAttributeText.h"

SP_DS_ViewerAttributeText::SP_DS_ViewerAttributeText(const wxString& p_sName,const wxString& p_sCategory,
                                                     const wxString& p_sDisplayName)
:SP_DS_ViewerAttribute(p_sName,p_sCategory,p_sDisplayName)
{

}

SP_DS_ViewerAttributeText::~SP_DS_ViewerAttributeText()
{

}

void SP_DS_ViewerAttributeText::Create(wxWindow* p_pcParent,wxSizer* p_pcSizer)
{
	wxBoxSizer* l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

	//create a text control to hold the attribute value
	m_pcValueTxtCtrl=new wxTextCtrl(p_pcParent,wxID_ANY,m_sValue);

	l_pcRowSizer->Add(new wxStaticText(p_pcParent,wxID_ANY,m_sDisplayName),0, wxALL|wxALIGN_CENTER, 5);
	l_pcRowSizer->Add(m_pcValueTxtCtrl,0, wxALL|wxALIGN_CENTER, 5);

	p_pcSizer->Add(l_pcRowSizer,0, wxALL, 5);
}

void SP_DS_ViewerAttributeText::Update()
{
	if(m_pcValueTxtCtrl!=NULL)
	{
		m_sValue=m_pcValueTxtCtrl->GetValue();
	}
}
