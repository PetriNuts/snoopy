
/*
* SP_DS_CPN_AnimExport.h
*
*  Created on: Oct 8, 2020
*   Author: George Assaf
*/

#ifndef SP_DS_EXPORT_STANIM_CPN_H_
#define SP_DS_EXPORT_STANIM_CPN_H_ 
#include <wx/wx.h>
#include <wx/statline.h>
#include<wx/filepicker.h>
#include<wx/textfile.h>
//#include "SP_DS_PedAnimation.h"
#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStAnimation.h"
#include "sp_ds/extensions/SP_DS_CPN_AnimExport.h"//by george
 
class SP_DS_ColStAnimation;
class ExportStochCPN : public wxFrame
{
public:
	SP_DS_ColStAnimation *m_pc_Animator; //To store the current animation window object
										 
	wxPanel *m_pc_Panel;

	wxStaticText *m_pc_Filename;
	wxFilePickerCtrl *m_pc_Browse;
	wxBoxSizer *m_pc_HBox1;

	wxStaticLine *m_pc_StLine1;
	wxStaticText *m_pc_StepDetails;
	wxStaticLine *m_pc_StLine2;

	wxStaticText *m_pc_Start;
	wxTextCtrl *m_pc_StartText;
	wxBoxSizer *m_pc_HBox2;

	wxStaticText *m_pc_Every;
	wxTextCtrl *m_pc_EveryText;
	wxBoxSizer *m_pc_HBox3;

	wxStaticText *m_pc_Stop;
	wxTextCtrl *m_pc_StopText;
	wxBoxSizer *m_pc_HBox4;

	wxRadioBox *m_pc_RadioChoice;

	wxButton *m_pc_OKBtn;
	wxButton *m_pc_CancelBtn;
	wxBoxSizer *m_pc_HBox5;

	wxBoxSizer *m_pc_VBox;
	wxString m_choices[2];

	wxDialog *m_pc_Overwrite;

	ExportStochCPN(const wxString &, class SP_DS_ColStAnimation *, bool p_bIsColSimMode=false);
	void OnRadioChoice(wxCommandEvent &);
	void OnOK(wxCommandEvent &);
	void OnExit(wxCommandEvent &);


private:
	bool m_bIsColSimMode;

	DECLARE_EVENT_TABLE()
};

#endif /* SP_DS_EXPORT_STANIM_CPN_H_ */
