/*
* SP_DS_Import.h
*
*  Created on: 10.10.2020
*      Author: George Assaf
*/

#ifndef SP_DDLG_COLANIM_IMPORT_H_
#define SP_DDLG_COLANIM_IMPORT_H_
#include <wx/wx.h>
#include <wx/statline.h>
#include<wx/filepicker.h>
#include<wx/textfile.h>

#include "SP_DS_PedAnimation.h"
#include "sp_ds/animators/ani_ColSPN/SP_DS_ColStAnimation.h"
#include "sp_ds/animators/ani_CPN/SP_DS_CPN_Animation.h"
//class SP_DS_PedAnimation;
class SP_DS_CPN_Animation;
class SP_DS_ColStAnimation;

class ImportColAnim : public wxFrame
{
public: class SP_DS_CPN_Animation *m_pc_Animator; //To store the current animation window object
		class SP_DS_ColStAnimation *m_pcColStAnimator;
		wxPanel *m_pc_Panel;

		wxStaticText *m_pc_ChooseFile;
		wxFilePickerCtrl *m_pc_Browse;
		wxBoxSizer *m_pc_HBox1;

		wxStaticLine *m_pc_StLine1;

		wxButton *m_pc_OKBtn;
		wxButton *m_pc_CancelBtn;
		wxBoxSizer *m_pc_HBox2;

		wxBoxSizer *m_pc_VBox;

		ImportColAnim(const wxString &, class SP_DS_CPN_Animation *);
		ImportColAnim(const wxString &, class SP_DS_ColStAnimation *);
		void OnExit(wxCommandEvent &);
		void OnOK(wxCommandEvent &);

		DECLARE_EVENT_TABLE()
};

#endif /* SP_DDLG_COLANIM_IMPORT_H_ */
