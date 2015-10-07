/*
 * SP_DS_ViewerAttributeCheckBox.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 14.06.2011
 * Short Description:
 * *///=========================================================


#include "sp_ds/extensions/ResultViewer/AttributeTypes/SP_DS_ViewerAttributeCheckBox.h"

SP_DS_ViewerAttributeCheckBox::SP_DS_ViewerAttributeCheckBox(const wxString& p_sName,const wxString& p_sCategory,
                                                             const wxString& p_sDisplayName)
:SP_DS_ViewerAttribute(p_sName,p_sCategory,p_sDisplayName),
m_bValue(true)
{


}

SP_DS_ViewerAttributeCheckBox::~SP_DS_ViewerAttributeCheckBox()
{

}
void SP_DS_ViewerAttributeCheckBox::Create(wxWindow* p_pcParent,wxSizer* p_pcSizer)
{
    wxBoxSizer* l_pcRowSizer= new wxBoxSizer(wxHORIZONTAL);

    //create a check box control to hold the attribute value
	m_pcValueCheckBox = new wxCheckBox(p_pcParent, wxID_ANY , m_sDisplayName, wxDefaultPosition, wxDefaultSize, wxCHK_2STATE);
	l_pcRowSizer->Add(m_pcValueCheckBox,0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);

	m_pcValueCheckBox->SetValue(m_bValue);

	p_pcSizer->Add(l_pcRowSizer,0, wxALL, 5);
}

void SP_DS_ViewerAttributeCheckBox::Update()
{
	if(m_pcValueCheckBox!=NULL)
	{
		m_bValue = m_pcValueCheckBox->GetValue();
	}
}

