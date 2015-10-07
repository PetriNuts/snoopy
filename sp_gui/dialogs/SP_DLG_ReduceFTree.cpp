//////////////////////////////////////////////////////////////////////
// $Author: akurth $
// $Version: 0.1 $
// $Date: 2006/07/11 $
// Short Description: dialog to reduce Fault Tree
//////////////////////////////////////////////////////////////////////

#include "sp_gui/dialogs/SP_DLG_ReduceFTree.h"
#include "sp_gui/dialogs/SP_DLG_ChooseFTreeReduction.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeReductionFinder.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_core/base/SP_Graphic.h"
#include "sp_core/SP_Core.h"
//#include "bitmaps/ftree/duplicateAND_png.h"


#include <wx/spinctrl.h>

enum
{
    SP_RESET_CHOISES,
	SP_ALL_CHOISES,
	SP_CHECK_SELECTION,
	SP_DETAILS_IDENTICAL_GATES,
	SP_DETAILS_UNNEEDED_GATES,
	SP_DETAILS_MULTIPLE_INPUTS,
	SP_DETAILS_M_OF_N_GATE,
	SP_DETAILS_DUPLICATE_NEG
};

IMPLEMENT_CLASS(SP_DLG_ReduceFTree, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_ReduceFTree, wxDialog)
EVT_BUTTON(wxID_OK, SP_DLG_ReduceFTree::OnDlgOk)
EVT_BUTTON(wxID_CANCEL, SP_DLG_ReduceFTree::OnDlgCancel)
EVT_BUTTON(SP_CHECK_SELECTION, SP_DLG_ReduceFTree::OnCheckSelection)
EVT_BUTTON(SP_RESET_CHOISES, SP_DLG_ReduceFTree::OnDeselectAll)
EVT_BUTTON(SP_ALL_CHOISES, SP_DLG_ReduceFTree::OnSelectAll)

EVT_BUTTON(SP_DETAILS_IDENTICAL_GATES, SP_DLG_ReduceFTree::OnDetailsLongChains)

EVT_BUTTON(	SP_DETAILS_DUPLICATE_NEG, SP_DLG_ReduceFTree::OnDetailsNegations)
EVT_BUTTON(	SP_DETAILS_UNNEEDED_GATES, SP_DLG_ReduceFTree::OnDetailsUnneededGates)
EVT_BUTTON(	SP_DETAILS_MULTIPLE_INPUTS, SP_DLG_ReduceFTree::OnDetailsMultipleInputs)
EVT_BUTTON(	SP_DETAILS_M_OF_N_GATE, SP_DLG_ReduceFTree::OnDestailsMofNGates)

END_EVENT_TABLE()

SP_DLG_ReduceFTree::SP_DLG_ReduceFTree( SP_DS_Graph *p_graph,
				 wxWindow *p_parent,
				 const wxString& p_title)
  : wxDialog(p_parent, -1, p_title, wxDefaultPosition,
	     wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER),
 	m_msg(wxT(" ")),
    m_title(p_title),
    m_graph(p_graph),
	m_parent(p_parent),
    m_nodeList(NULL),
    m_grList(NULL)
{

	wxStaticBox* itemStaticBoxSizer3Static = new wxStaticBox(this, wxID_ANY, _("Select some options"));
	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* elemSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* levelSizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBoxSizer* itemStaticBoxSizer3 = new wxStaticBoxSizer(itemStaticBoxSizer3Static, wxVERTICAL);
	topSizer->Add(itemStaticBoxSizer3, 1, wxGROW|wxALL, 5);

 	//checkbox for all reduction rules
	m_unneededGates = new wxCheckBox( this,-1, wxT("Eliminate unneeded gates."),
	wxDefaultPosition, wxDefaultSize);
	m_unneededGates->SetValue(TRUE);

	elemSizer->Add(m_unneededGates, 17, wxEXPAND | wxALL, 5);

	m_longChains = new wxCheckBox( this,-1, wxT("Combine chains of identical gates to one gate. "),
	wxDefaultPosition, wxDefaultSize);
	m_longChains->SetValue(TRUE);

	elemSizer->Add(m_longChains, 0, wxEXPAND | wxALL, 5);

	m_unneededNegs = new wxCheckBox( this,-1, wxT("Eliminate duplicate negations."),
	wxDefaultPosition, wxDefaultSize);
	m_unneededNegs->SetValue(TRUE);

	elemSizer->Add(m_unneededNegs, 17, wxEXPAND | wxALL, 5);

	m_possibleInputs = new wxCheckBox( this,-1, wxT("Combine possible part of the tree by M-OF-N-Gates."),
	wxDefaultPosition, wxDefaultSize);
	m_possibleInputs->SetValue(TRUE);

	elemSizer->Add(m_possibleInputs, 17, wxEXPAND | wxALL, 5);

	m_multipleInputs = new wxCheckBox( this,-1, wxT("Relocate of multiple inputs."),
	wxDefaultPosition, wxDefaultSize);
	m_multipleInputs->SetValue(TRUE);

	elemSizer->Add(m_multipleInputs, 0, wxEXPAND | wxALL, 5);

	levelSizer->Add(elemSizer, 0, wxALIGN_LEFT);

	wxBoxSizer* detailsSizer = new wxBoxSizer(wxVERTICAL);

	//all Buttons for the details of each reduction rule
	detailsSizer-> Add(new wxButton(this, SP_DETAILS_UNNEEDED_GATES, wxT("&Details")),
		     0, wxLEFT | wxRIGHT | wxTOP, 5);

	detailsSizer-> Add(new wxButton(this, SP_DETAILS_IDENTICAL_GATES, wxT("&Details")),
		     0, wxLEFT | wxRIGHT | wxTOP, 5);

	detailsSizer-> Add(new wxButton(this, SP_DETAILS_DUPLICATE_NEG, wxT("&Details")),
		     0, wxLEFT | wxRIGHT | wxTOP, 5);

	detailsSizer-> Add(new wxButton(this, SP_DETAILS_MULTIPLE_INPUTS, wxT("&Details")),
		     0, wxLEFT | wxRIGHT | wxTOP, 5);

	detailsSizer-> Add(new wxButton(this, SP_DETAILS_M_OF_N_GATE, wxT("&Details")),
		     0, wxLEFT | wxRIGHT | wxTOP, 5);

	levelSizer->Add(detailsSizer, 0, wxALIGN_RIGHT);


	/* Buttons in the lower right hand corner */
	wxBoxSizer *buttonSizer = new wxBoxSizer( wxHORIZONTAL );


	buttonSizer->Add(new wxButton(this, SP_CHECK_SELECTION, wxT("&Check")),
		     0, wxLEFT | wxRIGHT | wxTOP, 5);

	buttonSizer->Add(new wxButton(this, SP_RESET_CHOISES, wxT("&Deselect All")),
		     0, wxLEFT | wxRIGHT | wxTOP, 5);

	buttonSizer->Add(new wxButton(this, SP_ALL_CHOISES, wxT("&Select All")),
		     0, wxLEFT | wxRIGHT | wxTOP, 5);

	buttonSizer->Add(new wxButton(this, wxID_CANCEL, wxT("&Cancel")),
		     0, wxLEFT | wxRIGHT | wxTOP, 5);

	itemStaticBoxSizer3->Add(levelSizer, 20, wxALIGN_CENTER);
	itemStaticBoxSizer3->Add(buttonSizer, 5, wxALIGN_CENTER);

	topSizer->Add(new wxStaticText(this, -1, wxT("   Details:")), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);

	m_pcDetailsCtrl = new wxTextCtrl(this, -1, m_msg, wxDefaultPosition, wxSize(510,100),
        wxTE_MULTILINE | wxTE_READONLY);


	topSizer->Add(m_pcDetailsCtrl,0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	SetSizerAndFit(topSizer);

	Centre(wxBOTH);
}

void
SP_DLG_ReduceFTree::OnDlgOk(wxCommandEvent& p_cEvent)
{
	this->Show(false);
}

void
SP_DLG_ReduceFTree::OnDlgCancel(wxCommandEvent& p_cEvent)
{
    p_cEvent.Skip();
}


void
SP_DLG_ReduceFTree::OnCheckSelection(wxCommandEvent& p_event)
{
	//On the basis of the choice of the reduction rules, the methods are called.

	SP_DS_FTreeReductionFinder* fReduc = new SP_DS_FTreeReductionFinder(m_graph);


	//The order of the application of the reduction rules (priority of the reduction
	//rules) are fixed.
	if (m_unneededGates->GetValue())
			fReduc->FindUnneededGates();

	if (m_longChains->GetValue())
		fReduc->FindChainsOfIdenticalGates();

	if (m_unneededNegs->GetValue())
		fReduc->FindUnneededNegations();

	if (m_possibleInputs->GetValue())
			fReduc->FindPossibleMOFNInputs();

	if (m_multipleInputs->GetValue())
			fReduc->FindMultipleInputs();


	ft_map = fReduc->GetMap();
	//If places were found in the fault tree which can be simplified, the results are presented
	//in a separate window.
	if (!ft_map.empty()) {

		SP_DLG_ChooseFTreeReduction* dlg = new SP_DLG_ChooseFTreeReduction(m_graph, m_parent,ft_map);
		if (dlg) {
			if (dlg->IsShown())
				dlg->Raise();
			else
				dlg->Show();
		}
	}else{
		SP_MESSAGEBOX(wxT("No possible reductions!"), wxT("Notification"), wxOK | wxICON_INFORMATION );
	}
}

void
SP_DLG_ReduceFTree::OnDeselectAll(wxCommandEvent& p_cEvent)
{
	m_longChains->SetValue(FALSE);
	m_unneededGates->SetValue(FALSE);
	m_unneededNegs->SetValue(FALSE);
	m_multipleInputs->SetValue(FALSE);
	m_possibleInputs->SetValue(FALSE);

}

void
SP_DLG_ReduceFTree::OnSelectAll(wxCommandEvent& p_cEvent)
{
	m_longChains->SetValue(TRUE);
	m_unneededGates->SetValue(TRUE);
	m_unneededNegs->SetValue(TRUE);
	m_multipleInputs->SetValue(TRUE);
	m_possibleInputs->SetValue(TRUE);

}

void
SP_DLG_ReduceFTree::OnDetailsLongChains(wxCommandEvent& p_cEvent)
{

	m_msg = wxT("Long chains of identical gates in the form of \n\n")
			wxT("	(X * Y) * Z = (X * Y * Z) and\n")
		    wxT("	(X + Y) + Z = (X + Y + Z) \n\n")
		    wxT("will be combine to one gate.");
		  ;
	if (m_pcDetailsCtrl)
        m_pcDetailsCtrl->SetValue(m_msg);
}


void
SP_DLG_ReduceFTree::OnDetailsUnneededGates(wxCommandEvent& p_cEvent)
{
	m_msg = wxT("Gates will be delete if: \n")
	wxT("	1. They have only one input and output edge \n")
	wxT("	2. They are combined with another gate. ");
	if (m_pcDetailsCtrl)
        m_pcDetailsCtrl->SetValue(m_msg);
}

void
SP_DLG_ReduceFTree::OnDetailsNegations(wxCommandEvent& p_cEvent)
{
	m_msg = wxT("Duplicate negations in the form of - (- X) will be\n")
	  wxT("reduce by X.		\n");

	if (m_pcDetailsCtrl)
        m_pcDetailsCtrl->SetValue(m_msg);
}

void
SP_DLG_ReduceFTree::OnDetailsMultipleInputs(wxCommandEvent& p_cEvent)
{
	m_msg = wxT("Multiple inputs will be relocate on the basis of the\n")
	      wxT("distributive law: \n\n")
	  wxT("    (X + Y) * (X + Z) = X + (Y * Z)		\n")
	  wxT("    (X * Y) + (X * Z) = X * (Y + Z)		");

	if (m_pcDetailsCtrl)
        m_pcDetailsCtrl->SetValue(m_msg);
}

void
SP_DLG_ReduceFTree::OnDestailsMofNGates(wxCommandEvent& p_cEvent)
{
	m_msg = wxT("The reduction rule search for combinations of gates and events\n")
	      wxT("which can be replaced by a M-OF-N gate.\n\n")
		  wxT("For example a fault tree with the logical expression \n\n")
		  wxT("	(X * Y) + (X * Z) + (Y * Z)  \n\n")
		  wxT("can be replaced by a 2-OF-3 gate.");

	if (m_pcDetailsCtrl)
        m_pcDetailsCtrl->SetValue(m_msg);
}






