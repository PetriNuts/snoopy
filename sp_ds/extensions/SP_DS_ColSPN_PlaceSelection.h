#ifndef SP_DLG_COLSIM_PLACE_SELECTION_H_
#define SP_DLG_COLSIM_PLACE_SELECTION_H_
#include <wx/wx.h>
#include <wx/listctrl.h>


class SP_DS_ColSPN_PlaceSelection : public wxDialog {
public:
	SP_DS_ColSPN_PlaceSelection(wxWindow *p_pcParent,const wxString& title, wxArrayString p_list);

	 
	void OnTextEnter(wxCommandEvent& event);
	void OnAppendOne(wxCommandEvent& event);
	void OnAppendAll(wxCommandEvent& event);
	void OnBackAll(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);
	void OnBackOne(wxCommandEvent& event);
public:
	std::vector<wxString> GetChosenVariables();
	

private:
	wxDECLARE_EVENT_TABLE();
	wxArrayString ar_PlaceList;
	wxArrayString ar_PlaceListOut;
	wxListCtrl* leftList;
	wxListCtrl* rightList;
	std::vector<wxString> m_vChosen;
};

#endif /* SP_DLG_COLSIM_PLACE_SELECTION_H_ */