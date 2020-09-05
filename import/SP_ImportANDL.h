//////////////////////////////////////////////////////////////////////
// $Author: george assaf $
// $Version: 0.1 $
// $Date: 2020/10/27 $
// the basic class written by cr, but totally changed by george
////////////////////////////////////////////////////////////////////
#ifndef __SP_IMPORTANDLH__
#define __SP_IMPORTANDLH__

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


class SP_ImportANDL: public SP_ImportRoutine,
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
		bool reset_; // for reset arcs
		bool modifier_; // for modifier arcs
		Weights():
			place_(NULL),
			trans_(NULL),
			vpt_(wxEmptyString),
			vtp_(wxEmptyString),
			vi_(wxEmptyString),
			vr_(wxEmptyString),
			ve_(wxEmptyString),
			reset_(false), modifier_(false)
		{}
	};

	map<std::string, SP_DS_Node*> lookupPlaces;
	map<std::string, SP_DS_Node*> lookupParameters;
	map<std::string, SP_DS_Node*> lookupTransitions;
	map<pair<std::string, std::string>, Weights > arcs;

	void ClearAll();

	//file to be read
	wxString m_fileName;
	wxString netName;

	dssd::andl::NetType m_eNetType;
	SP_DS_Graph_Declarations m_CheckDec;
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

	//SP_DS_Graph* CreateDocument();
	SP_DS_Graph* CreateDocument(const wxString& p_sFile, dssd::andl::NetType p_eType);
	bool CreateFunc(const dssd::andl::Functions& p_Functions);
	bool CreatePlaces(const dssd::andl::Places& p_Places);
	bool CreateConst(const dssd::andl::Constants& p_Constants, const dssd::andl::Valuesets& p_Valuesets);
	bool CreateObservers(const dssd::andl::Observers& p_Observers);
	bool CreateTransitions(const dssd::andl::Transitions& p_Transitions);
	bool CreateArcs();
	void doLayout();
private:
	//check boxes
	wxCheckBox* m_pcCheckSelectiveChoices;
	wxCheckBox* m_pcCheckIsNewDoc;
	bool m_bIsNewDoc;
	wxCheckBox* m_pcCheckAll;


private:
	void AddConstants();
	void AddFunctions();
	void AddObservers();

	wxArrayString l_finalconstants;
	wxArrayString l_finalFunSel;
	wxArrayString l_finalobservers;
	std::map<wxString, std::set<wxString>> m_mType2Declartions;
	void OnSelectiveChoice(wxCommandEvent& p_cEvent);
	void OnIsNewDoc(wxCommandEvent& p_cEvent);
	void OnSelChange_Constants(wxCommandEvent& p_cEvent);
	void ONselectAll(wxCommandEvent& p_cEvent);
	void OnSelChange_funs(wxCommandEvent& p_cEvent);
	void OnSelChange_Observers(wxCommandEvent& p_cEvent);

	void PrintChosenConstants();
	void PrintChosenFunctions();
	void PrintChosenObservers();

	//Append declarations to graph
	bool  AddToGraph(const dssd::andl::Net& p_Net);
	bool AppendConstants(const dssd::andl::Constants& p_Constants, const dssd::andl::Valuesets& p_Valuesets);
	bool AppendFunctions(const dssd::andl::Functions& p_Functions);
	bool AppendObservers(const dssd::andl::Observers& p_Functions);
	bool Overwritedeclarations(const std::map<wxString, std::set<wxString>>& p_mType2Dec, const dssd::andl::Net& p_Net);
	bool OverwriteExistConstant(const wxString& p_sConstant, const dssd::andl::Net& p_Net);
	bool OverwriteExistFunction(const wxString& p_sFunction, const dssd::andl::Net& p_Net);
	bool OverwriteExistObserver(const wxString& p_sObserver, const dssd::andl::Net& p_Net);
	//traverse declaration dependency tree
	void LevelOrderTraversal(sp_node * root, std::map<NODE_TYPE, std::set<wxString>>& l_ResultKey);
	/*************/
	vector<DependencyTree> m_vConstDependenciesVector;
	vector<DependencyTree> m_vFunDependenciesVector;
	vector<DependencyTree> m_vObserversDependenciesVector;
	/********constants****/

	SP_DLG_ImportProperties* p_pcDlg;//by george
	dssd::andl::Net_ptr  p_pcDoc;//by george
	wxSizer* m_pcMainSizer_constants;
	wxSizer* m_pcLeftSizer_constants;
	wxSizer*	 m_pcRightSizer_constants;
	SP_WDG_NotebookPage* m_pcNotebookPageConstants;
	wxRearrangeList* m_pcRearrangelist_constants;
	wxTextCtrl * m_pcTextconstants;
	wxArrayString m_Options_constants;
	wxArrayInt m_Options_constants_order;
	std::map<wxString, vector<wxString>> m_mGroup2Const;
	std::map<wxString, std::set<wxString>> m_mMap2ConstDep;
	/******functions***/

	wxSizer* m_pcMainSizer_funs;
	wxSizer* m_pcLeftSizer_funs;
	wxSizer*	 m_pcRightSizer_funs;
	SP_WDG_NotebookPage* m_pcNotebookPageFunctions;
	wxRearrangeList* m_pcRearrangelist_function;
	wxTextCtrl * m_pcTextfunctions;
	wxArrayString m_Options_funs;
	wxArrayInt m_Options_funs_order;
	/*********Observers page******/
	wxSizer* m_pcMainSizer_observers;
	wxSizer* m_pcLeftSizer_observers;
	wxSizer*	 m_pcRightSizer_observers;
	SP_WDG_NotebookPage* m_pcNotebookPageObservers;
	wxRearrangeList* m_pcRearrangelist_observers;
	wxTextCtrl * m_pcTextobservers;
	wxArrayString m_Options_observers;
	wxArrayInt m_Options_observers_order;
	std::map<wxString, map<wxString,wxString>> m_mType2Ob;
	private:
		void AddConstants_Att();
		void AddFunctions_Att();
		void AddObservers_Att();
 public:
	SP_ImportANDL();
	virtual ~SP_ImportANDL();

	virtual bool ReadFile(const wxString& fileName, SP_ImportRoutine* p_import = NULL);
	virtual bool AddToDialog(SP_DLG_ImportProperties* p_pcDlg, dssd::andl::Net_ptr);
	inline virtual wxString GetName()
		{ return wxT("Import ANDL");}
	inline virtual wxString GetDescr()
		{ return wxT("Import a andl.");}
	virtual wxString GetExtension()
		{ return wxT("ANDL files (*.andl)|*.andl");}

};

#endif // __SP_IMPORTANDLH__
