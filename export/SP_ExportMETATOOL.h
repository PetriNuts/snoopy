//////////////////////////////////////////////////////////////////////
// $Author: okrause $
// $Version: 0.1 $
// $Date: 2009/02/13 $
// Short Description: export of P/T nets to TINA
//////////////////////////////////////////////////////////////////////

#ifndef __SP_EXPORTMETATOOL__
#define __SP_EXPORTMETATOOL__


#include "export/SP_ExportPT.h"
#include "sp_gui/dialogs/SP_DLG_ExportProperties.h"
#include <wx/listctrl.h>
#include <wx/listbox.h>
#include <wx/wx.h>
#include <wx/event.h>

class SP_ExportMETATOOL: public SP_ExportPT
{
 private:
	wxRadioButton* m_pRadioButton1;
	wxRadioButton* m_pRadioButton2;
    SP_DLG_ExportProperties* m_pcDlg;
	bool m_RadioFocus;

	wxListBox * m_pListBoxIn;
	wxListBox * m_pListBoxOut1;
	wxListBox * m_pListBoxOut2;

	wxStaticText* m_pStaticTextIn;
	wxStaticText* m_pStaticTextOut1;
	wxStaticText* m_pStaticTextOut2;

	wxArrayString m_ListBoxOut1_Reaction_String;
	wxArrayString m_ListBoxIn_Reaction_String;
	wxArrayString m_ListBoxOut1_Place_String;
	wxArrayString m_ListBoxIn_Place_String;

	wxBoxSizer* m_pcMainSizer;
	wxFilePickerCtrl* m_pcFilePickerCtrl;
	wxFilePickerCtrl* m_pcFilePickerCtrlFromFile;


  bool DoWrite();

protected:
	/**
	\brief Writes the reversible enzymes to a metatool-input format to a file

	 */
  void WriteENZREV();

	/**
	\brief Writes the irreversible enzymes  to a metatool-input format to a file

	 */
  void WriteENZIRREV();

	/**
	\brief Writes the internal compounds to a metatool-input format to a file

	 */
  void WriteMETINT();
  void WriteMETEXT();

	/**
	\brief Writes the reactions to a metatool-input format to a file

	 */
  void WriteCAT();

	/**
	\brief Converts a petri-net to a metatool-input format to a file

	 */
 public:



  SP_ExportMETATOOL();
  virtual ~SP_ExportMETATOOL();
  virtual bool AcceptsDoc(SP_MDI_Doc*);

  inline virtual wxString GetName()
    { return wxT("Export to METATOOL");}
  inline virtual wxString GetDescr()
    { return wxT("Export Petri Net in METATOOL Input Format");}
  virtual wxString GetExtension()
    { return wxT("dat");}
  virtual wxString GetFileDescr()
    { return wxT("METATOOL Input Files");}

  virtual bool AddToDialog(SP_DLG_ExportProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc);
  virtual bool OnDialogOk();

     void LoadPlace( );
	 void LoadReaction( );
	 void Ontopleftright( wxCommandEvent& p_cEvent  );
     void Ontoprightleft(wxCommandEvent& p_cEvent  );
     void Onbottomleftright( wxCommandEvent& p_cEvent );
     void Onbottomrightleft( wxCommandEvent& p_cEvent );
	 void OnRadioPlace( wxCommandEvent& p_cEvent );
	 void OnRadioTransition( wxCommandEvent& p_cEvent );
	 void OnInSelection( wxCommandEvent& p_cEvent );
	 void OnOutSelection( wxCommandEvent& p_cEvent );
};

#endif // __SP_EXPORTMETATOOL__

