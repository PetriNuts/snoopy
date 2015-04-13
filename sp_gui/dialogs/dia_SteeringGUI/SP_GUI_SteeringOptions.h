/*
 * SP_GUI_SteeringOptions.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 22.03.2011
 * Short Description:
 ==============================================================================*/

#ifndef SP_GUI_STEERINGOPTIONS_H_
#define SP_GUI_STEERINGOPTIONS_H_

#include <wx/wx.h>


class SP_GUI_SteeringOptions: public wxDialog
{
private:
	      wxRadioBox * m_pcModelOptions;
	      wxComboBox* m_pcExistingModelBox;

	      wxTextCtrl* m_pcModelNameTextCtrl;

	      spsa::SteeringClient* m_pcGUIClient;

	      bool m_bShowTwoOptions;

protected:
	      void OnModelOptionChanged(wxCommandEvent& event);

	      void OnOkButtonClicked(wxCommandEvent& event);

	      void OnExistingModelBoxDropDown(wxCommandEvent& event);

	      void UpdateModelnames();

	      bool IsModelNameExist(const wxString& p_sModelName);
public:
		  SP_GUI_SteeringOptions(wxWindow* p_pcParent,spsa::SteeringClient* p_pcGUIClient, const bool& p_bShowTwoOptions,const wxString& p_sTitle);

		  virtual ~SP_GUI_SteeringOptions();

		  int GetUserSelection(){return m_bShowTwoOptions==true?m_pcModelOptions->GetSelection():1;}

		  int GetSelectedModel(){return m_pcExistingModelBox->GetSelection();}

		  wxString GetModelName(){return m_pcModelNameTextCtrl->GetValue();}

		  DECLARE_EVENT_TABLE()
};

#endif /* SP_GUI_STEERINGOPTIONS_H_ */
