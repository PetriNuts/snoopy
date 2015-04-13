/*
 * SP_DLG_ChangeCurveAttributes.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 23.03.2011
 * Short Description:
 */
//=================================================/
#include "sp_gui/dialogs/dia_SteeringGUI/SP_DLG_ChangeCurveAttributes.h"

BEGIN_EVENT_TABLE(SP_DLG_ChangeCurveAttributes,wxDialog)
EVT_BUTTON(wxID_OK,SP_DLG_ChangeCurveAttributes::OnOk)
EVT_COMBOBOX(wxID_ANY, SP_DLG_ChangeCurveAttributes::OnComboBoxChange)
EVT_COLOURPICKER_CHANGED(wxID_ANY,SP_DLG_ChangeCurveAttributes::OnCurveColorChanged)
END_EVENT_TABLE()

SP_DLG_ChangeCurveAttributes::SP_DLG_ChangeCurveAttributes(wxWindow* p_pcParent,const wxString& p_sColor,const int& p_nLineWidth,const int p_nLineStyle)
:wxDialog(p_pcParent, -1, wxT("Edit curve properties"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE),
m_nLineWidth(p_nLineWidth),
m_nLineStyle(p_nLineStyle)
{
wxSizer* l_pcMainSizer=new wxBoxSizer(wxVERTICAL);

		  wxSizer* l_pcTopSizer=new wxBoxSizer(wxVERTICAL);
		  wxSizer* l_pcBottomSizer=new wxBoxSizer(wxVERTICAL);
		  l_pcMainSizer->Add(l_pcTopSizer, 1, wxEXPAND);
		  l_pcMainSizer->Add(l_pcBottomSizer, 0, wxEXPAND);
		  

		  wxBoxSizer* l_pcRowSizer= new wxStaticBoxSizer( new wxStaticBox( this,wxID_ANY,wxT("") ), wxHORIZONTAL);
		  l_pcRowSizer->Add(new wxStaticText(this,wxID_ANY,wxT("Curve color")),0, wxALL, 5);
		  wxColour l_Colour(p_sColor);
		  m_pcColourPickerCtrl = new wxColourPickerCtrl(this,  wxID_ANY, l_Colour,wxDefaultPosition, wxDefaultSize,wxCLRP_DEFAULT_STYLE);
		  l_pcRowSizer->Add(m_pcColourPickerCtrl,0, wxALL, 5);
		  l_pcTopSizer->Add(l_pcRowSizer, 1, wxEXPAND);
		  m_nOldLineColor=m_pcColourPickerCtrl->GetColour();

		  l_pcRowSizer= new wxStaticBoxSizer( new wxStaticBox( this,wxID_ANY,wxT("") ), wxHORIZONTAL);
		  //Curve width
		  m_pcLineWidthBox=new wxComboBox( this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(100,-1), 0, NULL, wxCB_READONLY );

		  l_pcRowSizer->Add(new wxStaticText(this,wxID_ANY,wxT("Curve width")),1, wxALL, 5);
		  l_pcRowSizer->Add(m_pcLineWidthBox,1, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);

		  for(int i=1; i<=20;i++)
		  {
			  m_pcLineWidthBox->Append(wxString::Format(wxT("%d"),i));
		  }

		  m_nLineWidth=m_nLineWidth<=0?2:m_nLineWidth;
		  m_pcLineWidthBox->Select(m_nLineWidth-1);

		  l_pcTopSizer->Add(l_pcRowSizer, 1, wxEXPAND);

		  //Curve style
		  l_pcRowSizer= new wxStaticBoxSizer( new wxStaticBox( this,wxID_ANY,wxT("") ), wxHORIZONTAL);

		  m_pcLineStyleBox=new wxComboBox( this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(100,-1), 0, NULL, wxCB_READONLY );

		  l_pcRowSizer->Add(new wxStaticText(this,wxID_ANY,wxT("Line style")),1, wxALL, 5);
		  l_pcRowSizer->Add(m_pcLineStyleBox,1, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
		  l_pcTopSizer->Add(l_pcRowSizer, 1, wxEXPAND);

		  m_pcLineStyleBox->Append(wxT("Sold"));
		  m_pcLineStyleBox->Append(wxT("Dot"));
		  m_pcLineStyleBox->Append(wxT("Long Dash"));
		  m_pcLineStyleBox->Append(wxT("Short Dash"));
		  m_pcLineStyleBox->Append(wxT("Dot Dash"));

		  switch(m_nLineStyle)
		  {
		     case wxSOLID:
		    	          m_pcLineStyleBox->Select(0);
		    	          break;
		     case wxDOT:
						  m_pcLineStyleBox->Select(1);
						  break;
		     case wxLONG_DASH:
						  m_pcLineStyleBox->Select(2);
						  break;
		     case wxSHORT_DASH:
						  m_pcLineStyleBox->Select(3);
						  break;
		     case wxDOT_DASH:
						  m_pcLineStyleBox->Select(5);
						  break;
		     default:
		    	         m_pcLineStyleBox->Select(0);
		    	         break;

		  }

		  //use default setting
		  m_pcUseDefaultSetting=new wxCheckBox(this, wxID_ANY,wxT("use default setting"), wxDefaultPosition, wxDefaultSize, wxCHK_3STATE);
		  m_pcUseDefaultSetting->SetValue(true);
		  l_pcRowSizer= new wxBoxSizer( wxHORIZONTAL);
		  l_pcRowSizer->Add(m_pcUseDefaultSetting,1, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
		  l_pcTopSizer->Add(l_pcRowSizer, 1);

		  l_pcBottomSizer->Add(CreateSeparatedButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND | wxALIGN_CENTER);

		  SetSizerAndFit(l_pcMainSizer);
		  Center();
}

SP_DLG_ChangeCurveAttributes::~SP_DLG_ChangeCurveAttributes()
{

}
void SP_DLG_ChangeCurveAttributes::OnOk(wxCommandEvent& event)
{
int l_anLineStyle[]={wxSOLID,wxDOT,wxLONG_DASH,wxSHORT_DASH,wxDOT_DASH};

        if(m_pcUseDefaultSetting->GetValue()==false)
        {
        	  m_nLineWidth=m_pcLineWidthBox->GetSelection()+1;

        	  m_nLineStyle=l_anLineStyle[m_pcLineStyleBox->GetSelection()];

        	  m_nLineColor=m_pcColourPickerCtrl->GetColour();
        }
        else
        {
        	  m_nLineWidth=-1;

        	  m_nLineStyle=-1;

        	  m_nLineColor=m_nOldLineColor;
        }
	    
	    EndModal(wxID_OK);
}

void SP_DLG_ChangeCurveAttributes::OnComboBoxChange(wxCommandEvent& event)
{
	      m_pcUseDefaultSetting->SetValue(false);
}

void SP_DLG_ChangeCurveAttributes::OnCurveColorChanged(wxColourPickerEvent& event)
{
	     m_pcUseDefaultSetting->SetValue(false);
}
