//////////////////////////////////////////////////////////////////////
// $Author: akurth $
// $Version: 0.1 $
// $Date: 2005/12/28 $
// Short Description: creates a fault tree on the basis of a logical expression 
////////////////////////////////////////////////////////////////////
#ifndef __SP_IMPORTFTREE_DNF_H__
#define __SP_IMPORTFTREE_DNF_H__

#include <wx/wx.h>
#include <wx/colordlg.h>
#include "sp_core/SP_List.h"
#include "sp_core/base/SP_Error.h"


#include "sp_core/FTree/SP_FTree_TermScanner.h"
#include "sp_core/FTree/SP_FTree_TermParser.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeCreater.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "import/SP_ImportRoutine.h"

class SP_DS_Graph;
class wxSpinCtrl;
class SP_DS_Node;
class SP_Graphic;

/**	\brief	Use to create a fault tree on the basis of a logical expression 

	The class imports a disjunctive normalform and tries to represents 
	the logical expression as a fault tree.

*/
class SP_ImportFTree_DNF: public SP_ImportRoutine,
	public SP_DS_FTreeCreater,
	//public wxDialog, 
	public SP_Error
{
 private:
	
	wxString m_fileName;
	SP_FTree_TermParser *m_pcParser;
	SP_FTree_TermScanner *m_pcScanner;
	
	/**
	\brief Creates fault tree
	To create a fault tree on the basis of the disjunctive normalform
	from the analysis tool SyRePa
	
	 \param		nodeList	elements of the logical expression
	 */
	void CreateGraph(SP_ListString* nodeList);

	/**
	\brief Connects a list of nodes with the same node
	
	 \param		sourceList	list of source-nodes
	 \param		target		target-node
	 */
	void drawEdges(SP_ListNode* sourceList, SP_DS_Node* target);

	/**
	\brief The test whether there is a basic event with the same name 
	
	 \param		basicName		name of the node
	 \returns	basicNode		dependent node
	 */
	SP_DS_Node* HasDuplicateNode(const wxString& basicName);
	
	SP_DS_Graph* m_pcGraph;

 protected:

	//list of elements from the disjunctive normalform
	SP_ListString* NodeList;
  
 
 public:
	SP_ImportFTree_DNF();
	virtual ~SP_ImportFTree_DNF();

	virtual bool ReadFile(const wxString& fileName,SP_ImportRoutine* p_sIR=NULL);
		
	inline virtual wxString GetName()
		{ return wxT("Import DNF");}
	inline virtual wxString GetDescr()
		{ return wxT("Import a DNF.");}
	virtual wxString GetExtension()
		{ return wxT("DNF file (*.dnf;*.fct)|*.dnf;*.fct");}
  
};

#endif // __SP_SP_IMPORTFTREE_DNF_H__
