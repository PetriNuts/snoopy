//////////////////////////////////////////////////////////////////////
// $Author: sliese $
// $Version: 0.1 $
// $Date: 2009/03/22 $
// Short Description: export to smart
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTSMART__
#define __SP_EXPORTSMART__

#include "export/SP_ExportPT.h"
#include "export/SP_ExportConfiguration.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_ds/SP_DS_TreeNode.h"
#include "sp_core/TParser/SP_Parser.h"
#include "export/SP_CTL_SMART.h"
#include "export/SP_IL_SMART.h"

#include <wx/tokenzr.h>

struct smartplace
{
	wxString place_name;
	wxString place_marking;
	long id;
	wxString net;
};
struct smarttransition
{
	wxString transition_name;
	wxString firingrule;
};
struct smartarc
{
	wxString from_pt;
	wxString to_pt;
	wxString arc_cardinality;
};
struct smartinhibitor
{
	wxString from_p;
	wxString to_t;
	wxString arc_cardinality;
};
struct smartguard
{
	wxString transition;
	wxString guardinstruction;
};
struct smarthierarchy
{
	wxString net;
	wxString parentnet;
};




class SP_ExportSMART: public SP_ExportPT
{

 protected:

	wxArrayString DeleteDublicate(wxArrayString data);
	wxString checkname(wxString p_name);
	virtual bool DoWrite();
	bool g_bool_Check_names_OK;
	//SMART write function
	bool Preprocessing();
	bool WritePlaces();
	bool WriteTransitions();
	bool WritePartitions();
	bool WriteInit();
	bool WriteArcs();	
	bool WriteGuards();
	bool WriteInhibitor();
	bool WriteFiring();
	bool WritePre();
	bool WriteIn();
	bool WritePost();

	//net id check function
	bool LoadHierarchy_Net();
	wxString GetRootofHierarchy();
	wxArrayString GetChildofNet(wxString net,bool childofchilds);
	bool CheckNetnumberInGraph(SP_DS_Graph* graph);
	
	//CTL and IL
	void Transform_CTL_IL_toSMART();
	wxArrayString g_wxAS_CTL_IL_lines_inner;
	wxArrayString g_wxAS_CTL_IL_lines_outer;	
	
	//Function Transform rules
	bool Transform_TNumber_check(TreeNode * root);
	bool Transform_BiomassAction(TreeNode* root,SP_Transition* t);
	bool Transform_BioLevelInterpretation(TreeNode* root,SP_Transition* p_transition);
	bool Transform_Multiplicity(TreeNode* root);
	bool Transform_BinominalCoefficient(TreeNode* root);
	bool Transform_TIdentifier_Parameter(TreeNode * root);
	wxString Tokenlist_TIdentifier_Place(TreeNode* root);
	void Transform_Token_Place_tk(TreeNode* root);
	wxString Functiontransform(SP_Transition* p_transition);
	
	//Function definition of grammar and Token
	wxArrayString Get_Function_Tokendef();
	wxArrayString Get_Function_grammar();


	wxString g_wxS_nettype;
	long g_long_place_count;
	long g_long_transitions_count;
	long g_long_arcs_count;
	wxArrayString g_wxAS_place_list;
	//lists ,represent a smart lines
	std::list<smartplace*> m_l_SMPlaceList;
	std::list<smartplace*>::iterator m_it_SMPlaceIt;
	std::list<smarttransition*> m_l_SMTransitionList;
	std::list<smarttransition*>::iterator m_it_SMTransitionIt;
	std::list<smartarc*> m_l_SMArcList;
	std::list<smartarc*>::iterator m_it_SMArcIt;
	std::list<smartinhibitor*> m_l_SMInhibitorList;
	std::list<smartinhibitor*>::iterator m_it_SMInhibitorIt;
	std::list<smartguard*> m_l_SMGuardList;
	std::list<smartguard*>::iterator m_it_SMGuardIt;
	std::list<smarthierarchy*> m_l_SMHierarchy;
	std::list<smarthierarchy*>::iterator m_it_SMHierarchyIt;

	SP_ExportConfiguration* m_pc_EC;
	SP_Parser* m_pc_Parser;
  
 public:
	//this function is called by dialog SP_DLG_SMART
	bool AutoPartition();
	
	wxArrayString SplitwxString(wxString data,wxString separator);
	//sph parameter
	wxString Get_parameterhead_string();
	wxString Get_parameter_string();

	//CTL
	SP_CTL_SMART* CTL_SMART;
	//IL
	SP_IL_SMART* IL_SMART;
	

  SP_ExportSMART();
  virtual ~SP_ExportSMART();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
	
  inline virtual wxString GetName()
    { return wxT("Export to SMART");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net in SMART Format");}
  virtual wxString GetExtension()
    { return wxT("smart");}
  virtual wxString GetFileDescr()
    { return wxT("SMART Files");}

  public :

    void SetMarkingSet( int p_nSetNumber );
    void SetFunctionSet( int p_nSetNumber );
    void SetParameterSet( int p_nSetNumber );


};

#endif // __SP_EXPORTSMART__

