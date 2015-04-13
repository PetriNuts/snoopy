/*
 * SP_GUI_SelectEditingList.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 20.06.2011
 * Short Description:
 */
//=================================================


#ifndef SP_GUI_SELECTEDITINGLIST_H_
#define SP_GUI_SELECTEDITINGLIST_H_

#include <wx/wx.h>
#include "wx/grid.h"
#include "spsa/spsa.h"

/*
 * selects a set of items to edit during the simulation. It is used both for selecting
 * places and parameter.
 */
class SP_GUI_SelectEditingList:public wxDialog
{
private:

	    spsa::VectorULong m_anSelectedList;
	    
	    const SP_VectorString*  m_sListToselectFrom;
	    
	    const SP_VectorDouble* m_pcCurrentValues;
	    
	    SP_MapString2Long m_mNodeName2Position;
private:
	     //list of available nodes
		  wxListBox* m_pcAvailableNodes;

		  //list of selected nodes
		  wxListBox* m_pcSelectedNodes;
	    
protected:
	     void OnOk(wxCommandEvent& WXUNUSED(event));

		  void OnMoveLeftToRight(wxCommandEvent& event);

		  void OnMoveRightToLeft(wxCommandEvent& event);

		  void OnMoveAllLeftToRight(wxCommandEvent& event);

		  void OnMoveAllRightToLeft(wxCommandEvent& event);

		  void MoveElements(wxListBox* p_pcSourceList,wxListBox* p_pcDestList, wxArrayInt& p_anSelections);

		  void MoveAllElement(wxListBox* p_pcSourceList,wxListBox* p_pcDestList);

		  void RemoveItemsFromList(wxListBox* p_pcList,const SP_VectorString& p_anSelections);
protected:
          void InitWithReportItems();

				   
	    void  LoadData(const spsa::VectorULong* p_pcInitalSelection);
public:
		SP_GUI_SelectEditingList(wxWindow* p_pcParent,const wxString& p_sTitle ,const SP_VectorString*
				                p_sListToselectFrom,const spsa::VectorULong* p_pcInitalSelection,
				                const SP_VectorDouble* p_pcCurrentValues);
		virtual ~SP_GUI_SelectEditingList();

		spsa::VectorULong& GetSelectedList(){return m_anSelectedList;}

		DECLARE_EVENT_TABLE();

};

#endif /* SP_GUI_SELECTEDITINGLIST_H_ */
