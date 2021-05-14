//////////////////////////////////////////////////////////////////////
// $Author: by George Assaf. $
// $Version: 0.1 $
// $modification: adding selctive import with dependency checking
// $Date: 2011/12/14 $
// the basic class with basuic functionalities written by cr, but totally changed by george
////////////////////////////////////////////////////////////////////
#ifndef __SP_IMPORTCANDLH__
#define __SP_IMPORTCANDLH__

#include <wx/wx.h>
#include <wx/colordlg.h>
#include "sp_core/SP_List.h"
#include "sp_core/SP_Map.h"

#include "sp_core/base/SP_Error.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "import/SP_ImportRoutine.h"
#include <wx/ffile.h>
#include <wx/tokenzr.h>
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"

#include "dssd/misc/net.h"
#include "sp_gui/dialogs/SP_DLG_ImportProperties.h"//by george

class SP_DS_Graph;
class SP_DS_Edge;
class SP_DS_Node;
class SP_Graphic;

class SP_ImportCANDL: public SP_ImportRoutine,
	public SP_Error
{
protected:

	struct Weights
	{
		SP_DS_Node* place_;
		SP_DS_Node* trans_;
		wxString vpt_; // PT weight met
		wxString vtp_; // TP weight met
		wxString vi_; // for inhibitor arcs
		wxString vr_; // for read arcs
		wxString ve_; // for equal arcs
		wxString reset_; // for reset arcs
		wxString modifier_; // for modifier arcs
		Weights():
			place_(NULL),
			trans_(NULL),
			vpt_(wxEmptyString),
			vtp_(wxEmptyString),
			vi_(wxEmptyString),
			vr_(wxEmptyString),
			ve_(wxEmptyString),
			reset_(wxEmptyString),
			modifier_(wxEmptyString)
		{}
	};

	map<std::string, SP_DS_Node*> lookupPlaces;
	map<std::string, SP_DS_Node*> lookupParameters;
	map<std::string, SP_DS_Metadata*> lookupConstants;
	map<std::string, SP_DS_Metadata*> lookupColorsets;
	map<std::string, SP_DS_Metadata*> lookupVariables;
	map<std::string, SP_DS_Metadata*> lookupFunctions;
	map<std::string, SP_DS_Metadata*> lookupObservers;
	map<std::string, SP_DS_Node*> lookupTransitions;
	map<pair<std::string, std::string>, Weights > arcs;

	void ClearAll();

	//file to be read
	wxString m_fileName;
	wxString netName;

	////////////////////////Parsing//////////////////////
	wxFFile m_fFile;

	dssd::andl::NetType m_eNetType;
	SP_DS_Graph_Declarations m_CheckDec;
	std::map<wxString, std::set< wxString>> m_mType2Declaration;
	////////////////////////Graph Creation////////////////
	SP_MDI_View* m_pcView;
	SP_DS_Graph* m_pcGraph;
	SP_GUI_Canvas* m_pcCanvas;
	SP_MDI_Doc* m_pcMyDoc;

	double x;
	double y;



	//void CreateGraph();
	bool CreateGraph(const wxString& p_sFile, const dssd::andl::Net& p_Net);
	bool CreateEdge(SP_DS_Node* source, SP_DS_Node* target, const wxString& weight,const wxString& type);

	void ComputeInitMarking();//by george

	SP_DS_Graph* CreateDocument(const wxString& p_sFile, dssd::andl::NetType p_eType);
	bool CreatePlaces(const dssd::andl::Places& p_Places);
	bool CreateConst(const dssd::andl::Constants& p_Constants, const dssd::andl::Valuesets& p_Valuesets);
	bool CreateColorsets(const dssd::andl::Colorsets& p_Colorsets);
	bool CreateVariables(const dssd::andl::Variables& p_Variables);
	bool CreateFunctions(const dssd::andl::ColorFunctions& p_Functions);
	bool CreateObservers(const dssd::andl::Observers& p_Observers);
	bool CreateTransitions(const dssd::andl::Transitions& p_Transitions);
	bool CreateArcs();
	void doLayout();
	void AdaptColorExpression(wxString& p_ColorExpression);

	///////////////////  by george /////////////

	bool AddToGraph(const dssd::andl::Net& p_Net);
	bool AppendColorsets(const dssd::andl::Colorsets& p_Colorsets);
	bool AppendConstants(const dssd::andl::Constants& p_Constants, const dssd::andl::Valuesets& p_Valuesets);
	bool AppendVariables(const dssd::andl::Variables& p_Variables);
	bool AppendFunctions(const dssd::andl::ColorFunctions& p_Functions);
	bool AppendObservers(const dssd::andl::Observers& p_observers);
	bool Overwritedeclarations(const std::map<wxString, std::set<wxString>>& p_mType2Dec, const dssd::andl::Net& p_Net);
	bool OverwriteExistConstant(const wxString& p_sConstant, const dssd::andl::Net& p_Net);
	bool OverwriteExistColorSet(const wxString& p_sColorset, const dssd::andl::Net& p_Net);
	bool OverwriteExistFunction(const wxString& p_sFunction, const dssd::andl::Net& p_Net);
	bool OverwriteExistObserver(const wxString& p_sObserver, const dssd::andl::Net& p_Net);
	bool OverwriteExistVariable(const wxString& p_sObserver, const dssd::andl::Net& p_Net);
 private:
	 ///////////////////////////////////////////
	 vector<DependencyTree> m_vConstDependenciesVector;
	 vector<DependencyTree> m_vColFunDependenciesVector;
	 vector<DependencyTree> m_vObserversDependenciesVector;
	 vector<DependencyTree> m_vColorsetDependecncyTrees;
	 vector<DependencyTree> m_vVariablesDependecncyTrees;

	 ///////////////////
	 SP_DLG_ImportProperties* p_pcDlg;
	 dssd::andl::Net_ptr  p_pcDoc;
	 wxSizer* m_pcMainSizer_funs;
	 wxSizer* m_pcLeftSizer_funs;
	 wxSizer*	 m_pcRightSizer_funs;
	 SP_WDG_NotebookPage* m_pcNotebookPageFunctions;
	 wxRearrangeList* m_pcRearrangelist_function;
	// wxStaticText* m_pcTextFundefs;
	 wxTextCtrl * m_pcTextfunctions1;
	 wxArrayString m_Options_funs;
	 wxArrayInt m_Options_funs_order;
	 /////////////////////
	 SP_WDG_NotebookPage* m_pcNotebookPagecolorsets;
	 wxSizer* m_pcMainSizer_colorsets;
	 wxSizer* m_pcLeftSizer_colorsets;
	 wxSizer*	 m_pcRightSizer_colorsets;
	 wxRearrangeList* m_pcRearrangelist_colorsets;
	 wxTextCtrl * m_pcTextcolorsets1;
	 wxArrayString m_Options_cs;
	 wxArrayInt m_Options_cs_order;
	 /////////////////////////
	 SP_WDG_NotebookPage* m_pcNotebookPagevariables;
	 wxSizer* m_pcMainSizer_variables;
	 wxSizer* m_pcLeftSizer_variables;
	 wxSizer*	 m_pcRightSizer_variables;
	 wxRearrangeList* m_pcRearrangelist_variables;
	 wxTextCtrl * m_pcTextvariable1;
	 wxArrayString m_Options_vars;
	 wxArrayInt m_Options_vars_order;
	 ////////////////////////
	 SP_WDG_NotebookPage* m_pcNotebookPageConstants;
	 wxSizer* m_pcMainSizer_constants;
	 wxSizer* m_pcLeftSizer_constants;
	 wxSizer*	 m_pcRightSizer_constants;
	 wxRearrangeList* m_pcRearrangelist_constants;
	 wxTextCtrl * m_pcTextconstants1;
	 wxArrayString m_Options_constants;
	 wxArrayInt m_Options_constants_order;
	 std::map<wxString, vector<wxString>> m_mGroup2Const;
	 wxCheckBox* m_pcCheckSelectiveChoices;
	 wxCheckBox* m_pcCheckIsNewDoc;
	 bool m_bIsNewDoc;
	 wxCheckBox* m_pcCheckAll;
	 //////////////////////////
	 SP_WDG_NotebookPage* m_pcNotebookPageobservers;
	 wxSizer* m_pcMainSizer_observers;
	 wxSizer* m_pcLeftSizer_observers;
	 wxSizer*	 m_pcRightSizer_observers;
	 wxRearrangeList* m_pcRearrangelist_observers;
	 wxTextCtrl * m_pcTextobservers;
	 wxArrayString m_Options_observers;
	 wxArrayInt m_Options_observers_order;
	 //////////////////////
	 void AddConstants();
	 void AddColorSets();
	 void AddFunctions();
	 void AddVariables();
	 void AddObservers();
	 wxArrayString l_finalFunSel;
	 wxArrayString l_finalcolorsets;
	 wxArrayString l_finalvariables;
	 wxArrayString l_finalconstants;
	 wxArrayString l_finalObservers;

	// std::map<wxString, std::set<wxString>> m_mDeclaration2Overwritten;

	void LevelOrderTraversal(sp_node * root, std::map<NODE_TYPE, std::set<wxString>>& l_ResultKey);

 public:
	SP_ImportCANDL();
	virtual ~SP_ImportCANDL();

	virtual bool ReadFile(const wxString& fileName, SP_ImportRoutine* p_import=NULL);
	inline virtual SP_DS_Graph* ImportToDoc(const wxString& fileName);//by george
	inline virtual wxString GetName()
		{ return wxT("Import CANDL");}
	inline virtual wxString GetDescr()
		{ return wxT("Import a candl.");}
	virtual wxString GetExtension()
		{ return wxT("CANDL files (*.candl)|*.candl");}
	virtual bool AddToDialog(SP_DLG_ImportProperties* p_pcDlg, dssd::andl::Net_ptr);
	void OnSelChange_funs(wxCommandEvent& p_cEvent);
	void OnSelChange_Colorsets(wxCommandEvent& p_cEvent);
	void OnSelChange_Variables(wxCommandEvent& p_cEvent);
	void OnSelChange_Constants(wxCommandEvent& p_cEvent);
	void OnSelChange_observers(wxCommandEvent& p_cEvent);
	void OnSelectiveChoice(wxCommandEvent& p_cEvent);
	void OnIsNewDoc(wxCommandEvent& p_cEvent);
	void ONselectAll(wxCommandEvent& p_cEvent);
	void AddFunctions_Att();
	void AddColorSets_Att();
	void AddVariable_Att();
	void AddConstants_Att();
	void AddObservers_Att();

	void PrintChosenConstants();
	void PrintChosenColorSets();
	void PrintColorFunctions();
	void PrintVariables();
	void PrintObservers();
};

#endif // __SP_IMPORTCANDLH__
