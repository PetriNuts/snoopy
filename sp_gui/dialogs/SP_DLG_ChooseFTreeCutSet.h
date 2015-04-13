//////////////////////////////////////////////////////////////////////
// $Author: akurth $
// $Version: 0.1 $
// $Date: 2006/08/02 $
// Short Description: dialog to check cut sets of FaultTree 
////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_CHOOSEFTREECUTSET_H__
#define __SP_DLG_CHOOSEFTREECUTSET_H__

#include <wx/wx.h>
#include <wx/clrpicker.h>
#include "sp_core/SP_List.h"
#include "sp_core/base/SP_Error.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/tools/SP_FTreeNodeColourizer.h"

/**	\brief	Dialog to choose and highlight a cut set 

    Used to display all cut sets of the fault tree.
*/


class SP_DLG_ChooseFTreeCutSet: 
	public wxDialog, 
	public SP_Error
{
 private:
 
  DECLARE_EVENT_TABLE()   
  DECLARE_CLASS(SP_DLG_ChooseFTreeCutSet)
  
  //list of nodes and their markings
  map<SP_DS_Node*, long> *m_pcMapListNum2MarkValue;
  
  //list of nodes and their stored colours
  map<long, SP_ListNode*> *m_pcMapListNum2CutSet;

  //list of cut sets
  list<SP_ListNode*>*  m_cutSets;
  
  SP_FTreeNodeColourizer *m_pcNodeColourizer;
  wxListBox* m_pcListBox;
  wxArrayString m_pcListBoxValues;
  
  wxColourPickerCtrl* m_pcButtonColour;

  SP_DS_Graph* m_pcGraph;

  // Array to handle multiple selections
  wxArrayInt m_aVectorNumbers;

 protected:
 
	/**
	\brief Reset marking of nodes
	Reset the marking of all nodes which belongs to a cut set.
	*/
	bool ResetMarking();

	void InitGUI();
	
	/**
	\brief Insert new entry and update listbox
	*/
	void UpdateList();
  
	/**
	\brief Choose Colour 
	*/
	void OnColour(wxColourPickerEvent& p_cEvent);

	/**
	\brief Refresh Colour in dialog
	*/
	void RefreshColour();

	/**
	\brief Find selecting cut set
	Find the cut set, which should be displayed and
	colouring the nodes of the selected cut set.
	*/
	void OnChoosingCutSet();
  
	/**
	\brief Set markings of the nodes
	Set markings of the nodes of a cut set which is selected.

	\param	cutSet	selected cut set
	*/
	bool SetFailureStatus(SP_ListNode* cutSet);

	/**
	\brief Update the frame
	
	\param	l_doc		document
	*/
	void RefreshFrame();
 
 public:
  
	 SP_DLG_ChooseFTreeCutSet(SP_DS_Graph *p_pcGraph, wxWindow *p_parent, list<SP_ListNode*>* cutSets,
		  const wxString& p_title = wxT("Select Cut Set"));
 
	//Close dialog with choosen entry
	void OnDlgOk(wxCommandEvent& p_cEvent);
	//close dialog
	void OnDlgCancel(wxCommandEvent& p_cEvent);
	//choose entry
	void OnKlick(wxCommandEvent& p_cEvent);

	/**
	\brief Update listbox
	Reading and writing cut sets in the listbox
	*/
	bool ReadCutSets();

};

#endif // __SP_DLG_CHOOSEFTREECUTSETS_H__
