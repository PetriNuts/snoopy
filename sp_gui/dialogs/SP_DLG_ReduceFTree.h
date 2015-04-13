//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/07/05 $
// Short Description: dialog to show the reductions
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_REDUCEFTREE_H__
#define __SP_DLG_REDUCEFTREE_H__

#include <wx/wx.h>
#include "sp_core/SP_List.h"
#include "sp_core/base/SP_Error.h"
#include "sp_core/SP_Map.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeReductionFinder.h"

/**	\brief	Dialog to show all reductions 

*/

class SP_DLG_ReduceFTree: public wxDialog, public SP_Error
{
 private:
  DECLARE_EVENT_TABLE()   
  DECLARE_CLASS(SP_DLG_ReduceFTree)

  wxRadioBox* m_choice;
  wxSpinCtrl *m_idSpinCtrl;
  wxCheckBox* m_unneededGates;
  wxCheckBox* m_longChains;
  wxCheckBox* m_unneededNegs;
  wxCheckBox* m_apparentNegs;
  wxCheckBox* m_multipleInputs;
  wxCheckBox* m_possibleInputs;

  wxTextCtrl* m_pcDetailsCtrl;
  wxString m_msg;

  ReductionMap ft_map;

  wxString m_title;
  wxString m_titlePrefix;

  wxString m_nclass;

  SP_DS_Graph* m_graph;
  wxWindow *m_parent;
  SP_ListNode* m_nodeList;
  SP_ListNode::const_iterator m_nodeIt;
  SP_ListGraphic* m_grList;
  SP_ListGraphic::const_iterator m_grIt;

 protected:
 
  void OnDlgOk(wxCommandEvent& p_cEvent);
  void OnDlgCancel(wxCommandEvent& p_cEvent);
  
  // check which reduction is selected
  void OnCheckSelection(wxCommandEvent& p_event);

  //deselect all reductions
  void OnDeselectAll(wxCommandEvent& p_cEvent);

  //select all reductions
  void OnSelectAll(wxCommandEvent& p_cEvent);
  
  //details of the reduction types
  void OnDetailsLongChains(wxCommandEvent& p_cEvent);
  void OnDetailsUnneededGates(wxCommandEvent& p_cEvent);
  void OnDetailsNegations(wxCommandEvent& p_cEvent);
  void OnDetailsMultipleInputs(wxCommandEvent& p_cEvent);
  void OnDestailsMofNGates(wxCommandEvent& p_cEvent);
     

 public:
  SP_DLG_ReduceFTree(SP_DS_Graph *p_graph, wxWindow *p_parent,
		  const wxString& p_title = wxT("Reduction Rules"));
};

#endif // __SP_DLG_REDUCEFTREE_H__

