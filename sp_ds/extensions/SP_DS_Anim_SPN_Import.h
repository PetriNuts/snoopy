/*
* SP_DS_Import.h
*
*  Created on: Oct 13th 2020
*      Author: George Assaf
*/

#ifndef SP_DS_SPN_IMPORT_H_
#define SP_DS_SPN_IMPORT_H_
#include <wx/wx.h>
#include <wx/statline.h>
#include<wx/filepicker.h>
#include<wx/textfile.h>

#include "sp_ds/extensions/ext_SPN/SP_DS_StAnimation.h"

class SP_DS_StAnimation;

class ImportSPN : public wxFrame
{
public: class SP_DS_StAnimation *m_pc_Animator; //To store the current animation window object

		wxPanel *m_pc_Panel;

		wxStaticText *m_pc_ChooseFile;
		wxFilePickerCtrl *m_pc_Browse;
		wxBoxSizer *m_pc_HBox1;

		wxStaticLine *m_pc_StLine1;

		wxButton *m_pc_OKBtn;
		wxButton *m_pc_CancelBtn;
		wxBoxSizer *m_pc_HBox2;

		wxBoxSizer *m_pc_VBox;

		ImportSPN(const wxString &, class SP_DS_StAnimation *);
		void OnExit(wxCommandEvent &);
		void OnOK(wxCommandEvent &);

		DECLARE_EVENT_TABLE()
};

#endif /* SP_DS_SPN_IMPORT_H_ */
