//////////////////////////////////////////////////////////////////////
// $Author: akurth $
// $Version: 0.1 $
// $Date: 2006/08/02 $
// Short Description: dialog to check reduction of FaultTree 
////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_CHOOSEFTREEREDUCTION_H__
#define __SP_DLG_CHOOSEFTREEREDUCTION_H__

#include <wx/wx.h>
#include <wx/clrpicker.h>
#include "sp_core/SP_List.h"
#include "sp_core/tools/SP_FTreeNodeColourizer.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeReductionTransformer.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeReductionFinder.h"

/**	\brief	Dialog to choose and highlight a reduction 

    Used to display all possible reductions of the fault tree.
*/


class SP_DLG_ChooseFTreeReduction: 
	public wxDialog, 
	public SP_Error
{
 private:
 
  DECLARE_EVENT_TABLE()   
  DECLARE_CLASS(SP_DLG_ChooseFTreeReduction)
 
  SP_FTreeNodeColourizer *m_pcNodeColourizer;
  bool m_colorize;
  bool m_bChanged;
 
  wxListBox* m_pcListBox;
  wxArrayString m_pcListBoxValues;

  wxColourPickerCtrl* m_pcButtonColour;
  
  //to copy fault tree
  SP_MDI_Doc* l_pcDoc;
  
  // Array to handle multiple selections
  wxArrayInt m_aVectorNumbers;

  //  wxRadioBox* m_pcChoice;
  wxSpinCtrl *m_idSpinCtrl;
  wxString m_nclass;

  SP_DS_Graph* m_pcGraph;
  SP_ListNode* m_nodeList;
  SP_ListNode::const_iterator m_nodeIt;
  SP_DS_FTreeReductionTransformer* doReduction;

  ReductionMap m_reduceList;

 protected:

  
  void InitGUI();

  /**
	\brief Read list of selected reductions
  */
  void ReadReductionList();

  //update of listbox
  void UpdateList();

  void OnUpdateUI(wxUpdateUIEvent& p_event);
  void OnDlgCancel(wxCommandEvent& p_cEvent);
  // choose first entry
  void OnFirst(wxCommandEvent& p_cEvent);
  // choose next entry
  void OnNext(wxCommandEvent& p_cEvent);
  // choose  previous entry
  void OnPrev(wxCommandEvent& p_cEvent);

  /**
	\brief Start to transform the graph
  */
  void OnStart(wxCommandEvent& p_cEvent);

  /**
	\brief Delete list entry
	can be deleted before transforming the graph.
  */
  void OnDelete(wxCommandEvent& p_cEvent);
  //colour selected entry(s)
  void OnKlick(wxCommandEvent& p_cEvent);

  /**
	\brief new list entry / colouring nodes of the selected reduction

  \param	nlist	list of nodes of the reduction type
  \param	count	id of the reduction list
  */
  bool Visit(NodeList* nlist, long count);

  /**
	\brief Find list of nodes of the selected reduction type
  */
  void OnChoosingReduction();
  
  /**
	\brief Search list entry
	Search list of nodes of the selected reduction type

	\param	l_nSel	number of list entry
  */
  long SearchID(int l_nSel);

  /**
	\brief Check if the graph has to be exported to extended fault tree
	Graphs from type "Fault Tree" have to be proved, because some possible
	reductions require elements which are only member of "Extended
	Fault Tree".

	\returns TRUE, FALSE
	*/
  bool ExportToExtendedFTreeIsNeeded();

  /**
	\brief Update list of reduction results with listbox
	*/
  void UpdateReductionList();

  /**
	\brief Start of reductions
	*/
  void DoReductions();

  /**
	\brief Check of existence of nodes
	Before doing next reduction, we have to check, if some nodes still be deleted by another reduction.

  \param	l_pclAllNodes	list of nodes of the next reduction
  \returns	TRUE, FALSE
	*/
  bool ExistenceOfNodes(SP_ListNode* l_pclAllNodes);
  
  /**
	\brief Choose Colour 
	*/
  void OnColour(wxColourPickerEvent& p_cEvent);
  
  /**
	\brief Refresh Colour in dialog
	*/
  void RefreshColour();

  /**
	\brief Update the frame
	
	\param	l_doc		document
	*/
  void RefreshFrame(SP_MDI_Doc* l_doc);

 
 public:
  SP_DLG_ChooseFTreeReduction(SP_DS_Graph *m_pcGraph, wxWindow *p_parent, ReductionMap redNodes,
		  const wxString& p_title = wxT("Possible Reductions"));
};

#endif // __SP_DLG_CHOOSEFTREEREDUCTION_H__
