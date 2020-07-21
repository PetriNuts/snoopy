//////////////////////////////////////////////////////////////////////
// $Author: george assaf $
// $Version: 0.1 $
// $Date: 2020/06/14 $
////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_DEPENDENCIY__
#define __SP_DS_DEPENDENCIY__


#include <wx/wx.h>
#include <wx/colordlg.h>
//#include "sp_core/SP_List.h"
//#include "sp_core/SP_Map.h"
#include "sp_ds/SP_DS_Graph.h"
//#include "sp_core/base/SP_Error.h"
//#include "sp_gui/mdi/SP_MDI_Doc.h"
 
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include <wx/tokenzr.h>
#include "dssd/misc/net.h"
//#include "sp_core/base/SP_Graphic.h"
//#include "sp_core/SP_Core.h"
//#include "sp_gui/mdi/SP_MDI_Doc.h"
//#include "sp_gui/management/SP_GM_Docmanager.h"
//#include "sp_gui/management/SP_GM_DocTemplate.h"
#include <iostream> 
#include <queue> 



enum NODE_TYPE {
	COLOR_SET,
	CONSTANT,
	VAR,
	FUN_COLOOR,
	OBSERVER
};
struct sp_node
{
	wxString key;
	bool     isUsed;
	NODE_TYPE type;
	vector<sp_node *>child;
};


struct FuncStruct {
	std::set<wxString> paramTypes;
	wxString body;
};

typedef sp_node* DependencyTree;
typedef std::map<wxString, FuncStruct> SP_FUN_2_NAME_MAP;
typedef std::map<wxString, std::vector<wxString>> SP_CONST_2_VALUES_MAP;

//class SP_DS_Graph;

class SP_DS_Graph_Declarations
{

protected:
	SP_DS_Graph* m_pcGraph1;
	dssd::andl::Net_ptr m_pcANDLFile;
	bool         m_bIsANDL;

private:
	SP_CONST_2_VALUES_MAP  m_mConstant2Values;
	SP_MapString2String  m_mColorSet2Definition;
	SP_MapString2String  m_mVar2ColorSet;
	SP_FUN_2_NAME_MAP     m_Function2Definition;
	SP_MapString2String   m_mObserver2Body;

private:
	//dependencies stuff
	vector<DependencyTree> m_vConstantsDepTrees;
	vector<DependencyTree> m_vVarsDepTrees;
	vector<DependencyTree> m_vFunctionsDepTrees;
	vector<DependencyTree> m_vColorsetDepTrees;
	vector<DependencyTree> m_vObserverDepTrees;;

	void BuildConstantDependencyTree(const wxString& p_sConstant, sp_node* tree);
	void BuildConstantsDependencyTrees();
	void BuildColorSetDependencyTree(const wxString& p_sCS, sp_node* tree);
	void BuildColorSetDependencyTrees();
	void BuildVariableDependencyTree(const wxString& p_Var, sp_node* tree);
	void BuildVariableDependencies();
	void BuildFunctionDependencyTree(const wxString& p_sFunName, sp_node* tree);
	void BuildFunctionsDependencyTrees();
	void BuildObserverssDependencyTrees();
	//void BuildObserverDependencyTree(const wxString& p_sObservName, sp_node* tree);
	void BuildDeclarationDependencies();

	void LevelOrderTraversal(sp_node * root, std::map<NODE_TYPE, std::set<wxString>>& l_ResultKey);
	void ClearAll();
	bool  IsToken(const wxString& p_sExp, const wxString& p_sToken);

	sp_node *newNode(const wxString& key, NODE_TYPE type)
	{
		sp_node *temp = new sp_node;
		temp->type = type;
		temp->key = key;
		temp->isUsed = false;
		return temp;
	}

private:
	//the stuff of looking for a declaration in the net 
	bool  IsDeclarationExistInNetMarking(const wxString& p_pcDec);//for colored/uncolored pns
	bool  IsExistonArcMultiplicity(const wxString& p_sDec);//check declaration if it is on arc multiplicities(uncolored PNs only )
	bool  CheckDeclarationInColouredPredicateofTrans(const wxString& p_sDec);
	bool  CheckDeclarationInTransitionFunction(const wxString& p_sDec);//check in function of uncolored pns
	bool  CheckDeclarationOverArcsAndGuards(const wxString& p_sDec);//for colored pn checking arc inscriptions
	bool  IsDeclarationExistInRateFunctions(const wxString& p_sDec);//rate functions of col/uncolred pns
	bool  IsColorsetUsedByColoredPlace(const wxString& p_SColorSet);//colored PNS

private:
	//dependency checking stuff
	/*
	dependency checking of each declaration is done as follows:
	1.check if the declaration itself is in use within the net
	2.if not, the visit all childeren and check if one of them is in use, then mark the parent as in use
	*/
	void  ComputeUsedConstants();
	void  ComputeUsedColorSets();
	void  ComputeUsedFunctions();
	void  ComputeUsedVariables();
	void  ComputeUsedObservers();
	

	bool CheckConstantInNet(const wxString& p_sConstant);
	bool CheckColorSetInNet(const wxString& p_sColorSet);
	bool CheckFunctionInNet(const wxString& p_sColorSet);
	bool CheckVariableInNet(const wxString& p_sVar);
	bool CheckObserverInViewe(const wxString& p_sObserver);

public:
	SP_DS_Graph_Declarations():m_pcGraph1(NULL),m_bIsANDL(true)
	{
		m_mColorSet2Definition = {};
		m_mVar2ColorSet = {};
		m_Function2Definition = {};
		m_mObserver2Body = {};
	}
	SP_DS_Graph_Declarations(SP_DS_Graph* graph,bool p_bIsANDL = false);
	 
	SP_DS_Graph_Declarations(dssd::andl::Net_ptr p_pcNet, bool p_bIsANDL = true);
	~SP_DS_Graph_Declarations() 
	{
		ClearAll();
	}
	void operator()();
	void  ComputeUsedDeclarations();
	//deletion stuff
	bool DeleteConstant(const wxString& p_sConstName);
	bool DeleteVariable(const wxString& p_sVarName);
	bool DeleteColorSet(const wxString& p_sCsName);
	bool DeleteFunction(const wxString& p_sfun);
	bool DeleteObserver(const wxString& p_sobserver);

	vector<DependencyTree> GetConstantsDependencyVector() { return m_vConstantsDepTrees; }
	vector<DependencyTree> GetVariableDependencyVector() { return m_vVarsDepTrees; }
	vector<DependencyTree> GetFunctionDependencyVector() { return m_vFunctionsDepTrees; }
	vector<DependencyTree> GetColorSetDependenncyVector() { return m_vColorsetDepTrees; }
	vector<DependencyTree> GetObserverDependencyVector() { return m_vObserverDepTrees; }

	void ComputeBackwardDependency( wxString& p_sdec, NODE_TYPE p_T, set<wxString>& p_sSet);
	
};

#endif // __SP_DS_DEPENDENCIY__