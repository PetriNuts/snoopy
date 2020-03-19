/***********************************/
/* $Source: $*/
/* $Author: George Assaf $  */
/* $Version: 0.0 $ */
/* $Revision: 1.0 $
/* $Date: 2018/10/16 11:20:00 $ */
/*SP_DS_UnUsedDeclaration.h */
/* Short Description:This class used to figure out unused declarations */
/***********************************/
#ifndef __SP_DS_UNUSED_DECLARATIONS_H__
#define __SP_DS_UNUSED_DECLARATIONS_H__

#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_Vector.h"
#include<boost/tokenizer.hpp>
#include<string>
using namespace std;
using namespace boost;

struct ConstantDependencyInfo {
	wxString m_sConstName;
	set<wxString> m_setConstants;
	set<wxString> m_setOppositeDepConstants;
	set<wxString> m_SetSimpleCS;
	set<wxString> m_SetCompoundCS;
	set<wxString> m_setFun;
	set<wxString> m_setForwardFun;//uncolored
	set<wxString> m_setVar;
	set<wxString> m_setObservers;
	bool m_bIsUsed;

};

struct FunctionDependencyInfo {
	wxString m_sFunName;
	set<wxString> m_setSimpleCs;
	set<wxString> m_setCompoundCs;
	set<wxString> m_setConst;
	set<wxString> m_setBackwordConst;//uncolored
	set<wxString> m_setVars;
	set<wxString> m_setObservers;
	set<wxString> m_setBackwordObservers;
	bool m_bIsUSed;

};
struct SimpleCsDependencyInfo {
	wxString m_sName;
	set<wxString> m_setCompCs;
	set<wxString> m_setFun;
	set<wxString> m_setVar;
	set<wxString> m_setConst;
	set<wxString> m_setIndirectCs;
	bool m_bIsUSed;
};
struct CompoundCsDependencyInfo {
	wxString m_sName;
	set<wxString> m_setSimpleCs;
	set<wxString> m_setIndirectCompCs;
	set<wxString> m_setFun;
	set<wxString> m_setVar;
	set<wxString> m_setConst;
	bool m_bIsUSed;
};
struct AliasCsDependencyInfo {
	wxString m_sName;
	set<wxString> m_setSimpleCs;
	set<wxString> m_setCompCs;
	set<wxString> m_setFun;
	set<wxString> m_setVar;
	set<wxString> m_setConst;
	bool m_bIsUSed;
};
struct SubSetCsDependencyInfo {
	wxString m_sName;
	wxString m_CompoundCS;
	set<wxString> m_setSimpleCs;
	set<wxString> m_setForwardVar;
	set<wxString> m_setFun;
	set<wxString> m_setVar;
	set<wxString> m_setConst;
	bool m_bIsUSed;

};
struct ObserverDependencyInfo {
	wxString m_observerName;
	set<wxString> m_setConstants;
	set<wxString> m_setFuns;
	set<wxString> m_setForwardFuns;
	bool         m_bIsUsed;
};

struct VariableDependencyInfo {
	wxString m_sName;
	set<wxString> m_CompoundCS;
	set<wxString> m_setSimpleCs;
	set<wxString> m_setFun;
	set<wxString> m_setConst;
	bool m_bIsUSed;

};
class SP_DS_UnUsedDeclaration {

private:
	/*returns map of defined Simple Cs 2 Constants (used to define the simple cs) */
	set<pair<wxString, wxString>> ObtainSimpleCsConstantsDependent();

	/*returns map of each defined function 2 used constants in the function */
	set<pair<wxString, set<wxString>>> ObtainFunctionsConstantsDependent();

	/*returns All constants used in the PN graph e.g, as marking, multiplicity ..etc*/
	set<wxString> ObtainUsedConstantsInGraph();

	/*returns map  <SimpleCS,ConstantDependent> which not used by the graph*/
	set<pair<wxString, wxString>> ObtainSimpleCS2ContantsDependentOutofGraph();

	/*returns All color sets used by the graph e.g, by used vars or by places*/
	set<wxString> ReadAllUsedCSByGraph();

	/*returns all defined funs with their CSs(simple +compound)*/
	map<wxString, set<wxString>> ReadFunctionsWithUsedCS();

	/*return map of sub-cs 2 colors*/
	map<wxString, set<wxString>> ObtainSubCs2ColorComponents();


	/*scan all color sets against the variables definetions, return set of used CSs as data type of variables */
	std::set<wxString> ReadUsedColorsetsByVariables()const;


	/*toknizer helper*/
	std::set<string>   ToknizeArcExp(wxString& exp) const;

	/*read constants used as marking in (coloured) Petri net classes*/
	std::set<wxString> ReadNetMarking();//const;//25.11

										/*read constants used as arc multiplicities in uncoloured petri net classes*/
	std::set<wxString> ReadNetMultiplicity(); //const;//25.11

											  /*return guards of a transition*/
	std::set<string>   ObtainGuardVar(SP_DS_Node* p_pcTransNode) const;

	/*returns the used guard by  a transition*/
	string             ObtainUsedGuard(SP_DS_Node* p_pcTransNode) const;

	/*returns the used functions by colored PN graph*/
	std::set<string>   ReadUsedColouredFunctions();//const;

												   /* Read Colour sets used by places of the net*/
	std::set<string>   ReadUsedColorsets()const;

	/*returns map of compound cs 2 its other CSs dependencies*/
	std::map<wxString, set<wxString>> ObtainCompoundCS2OtherCSDependency();

	/*return one level compound cs dependencies of compound cs*/
	map<wxString, set<wxString>> ObtainCompCs2SimpleOneLevel();


	/*read all compound CS with their simple cs components*/
	std::map<wxString, set<wxString>> ReadDefinedCompoundCS() const;

	/*return all defined simple cs*/
	set<wxString> ObtainSimpleCS();

	set<wxString> ObtainSimple2Compound(set<wxString> p_setSimple, map<wxString, set<wxString>>, set<wxString> p_setChildren, set<wxString> &p_setAllRes, wxString p_sProd);

	/*returns set of constants that its simple CSs depend on*/
	std::set<wxString> ObtainAllCompoundCSConstantDependent(set<wxString> p_setSimple);

	/*read used constants from body of defined functions for (coloured) Petri net classes*/
	std::set<wxString> ReadConstantsInFunctionDef();// used consts in functions defs

													/*returns map of fun 2 usedconstants to define the fun body*/
	std::map<wxString, set<wxString>> ObtainFun2ConstantsInDEf();//

																 /*read constants that used to define simple color sets*/
	std::set<wxString> ReadUsedConstantsinColorSetsDef() const;

	/*returns set of the constants used in observres def*/
	std::set<wxString> ReadUsedConstantsInObservers() const;

	/*return map of observer 2 used constants in each observer*/
	std::map<wxString, set< wxString>>   ObtainObservers2Constants();

	/*return map of observer 2 body defenition*/
	std::map<wxString, wxString>   ReadObserverBodies() const; //used observers

															   /*returns set of used funs by transitions*/
	std::set<wxString> ReadTransitionFunctions() const;//used functions

													   /*returns used constants(kinetic parametres)*/
	std::set<wxString> ReadTransitionParameters() const;

	/*returns used var in colPN*/
	std::set<string>   ReadColPNVariables();

	/*returns ARC EXpression */
	wxString ObtainArcExp(SP_DS_Node* p_pcPlaceNode, SP_DS_Edge* p_pcEdge, SP_DS_Node* p_pcTransNode) const;

	/*returns used CS by Functions arguments as data types*/
	std::set<wxString> ReadColorSetsFromFunctionsArguments(std::set<string> p_set) const;

	/*NOT USED*/
	std::set<wxString>  ReadColorSetsUsedFunctionsArguments();

	/*return set of Color sets used  by a given function*/
	std::set<wxString>  ReadColorSetsUsedByFunction(wxString p_sFunName);

	set<wxString> ObtainConstantsUsed2DefineCompoundCS(wxString p_sCompCS);

	/*returns (by reference wxString&)set forward dependency chain of constants  */
	void    FindForwardDepChain(wxString p_sConst, set<wxString> &p_setVals, map<wxString, wxString>&p_mname2val, wxString &p_sRes);

	/*returns (by reference wxString&)set forward dependency chain of a Compound cs  */
	void FindCompCsChain(wxString p_sCompcs, set<wxString> p_setSimple, map < wxString, set<wxString>> p_mComp2setSimple, wxString &p_sRes);

	bool ObtainCompBassics(wxString p_comp, set<wxString> p_setSimple, map < wxString, set<wxString>> p_mComp2setSimple, set<wxString> &p_setResSimple, set<wxString> &p_setCom);

	set<wxString> FindConstantDependChain(wxString p_scnst, set<wxString> &p_setChain);


	set<wxString> FindOppositeConstantDependChain(wxString p_scnst, set<wxString> &p_setChain);

	/*returns opposite dep. chain of a constant*/
	set<wxString> FindOppositeConstanChain(wxString p_scnst, set<wxString> &p_setChain);

	void   FindConChain(wxString p_sConst, set<wxString> &p_setVals, map<wxString, wxString>&p_mname2val, wxString &p_sRes);
	/*returns forward dependency chain of a simple cs*/
	void  FindSimpleCsChain(wxString p_sSimplecs, map<wxString, set<wxString>> p_mC2S, wxString &Res);

	/*returns alias 2 orginal*/
	pair<wxString, wxString> ObtainAlias2Orginal(wxString p_sAlias);

	/*returns set of all declarations used in all predicates in the colored net*/
	set<wxString> GetDeclarationsInPredicates();

	bool GetRateFunctionsOfTransitions(std::set<wxString>& p_setRateFun);

	bool TokenizeRateFunctions(std::set<wxString>& p_setRateFun, std::set<wxString>& p_setResultDeclarations);
public:
	/*constructor*/
	SP_DS_UnUsedDeclaration(SP_DS_Graph* p_pcGraph, SP_VectorString p_vUserDec);

	/*returns the type of passed variable*/
	wxString ObtainVarType(wxString p_sVarName);

	/*returns all defined vars*/
	set<wxString> ObtainAllDefinedVars();

	/*returns map of all var 2 type*/
	map<wxString, wxString> ObtainAllVar2Cs()const;

	/*return map of CS 2 Type(simple,compound,Alias)*/
	map<wxString, wxString> ObtainAllCs2Type();

	/*return set of all CS used by places*/
	std::set<wxString>   ReadUsedColorsetsbyPlaces()const;

	/*return map of compound 2 used other cs*/
	std::map<wxString, set<wxString>> ReadUsedCSByCompundCS();

	void SetUserDeclaration(SP_VectorString &p_vDec) { m_vUserDec = p_vDec; }

	SP_DS_UnUsedDeclaration() :m_pcGraph(NULL) {}

	/*returns set  of all defined Cs*/
	set<wxString> ObtainAllDefinedCS();

	/*returns set of all defined constants*/
	set<wxString> ObtainAllDefinedConstants();

	/*return all defined constants 2 values*/
	map<wxString, wxString> ObtainAllDefinedConstants2Valuses();


	/*return map of a specific declaration 2 its trimed form from declarations tree */
	std::map<wxString, wxString>    TrimUserDec(wxString& p_sDec);


	/*delete unused observer from uncolored net class*/
	void DeleteUnUsedObserver(wxString p_sObserverName);

	/*find unused observers in viewer list*/
	std::set<wxString>            FindUnusedObservers() const;

	/*returns map oserver name, used constants if exists*/
	map<wxString, set<wxString>> ObtainObserversConstantsDependent();

	/*find un-used constants for (coloured) Petri net classes in declaration tree*/
	std::set<wxString>   FindUnusedConstants();

	/*find unused functions in (coloured) Petri net classes*/
	std::set<wxString>   FindUnusedFunctions();


	/*find unused color sets in coloured Petri net Classes*/
	std::set<wxString>   FindUnusedColorSets(wxString p_sCSType);

	/*find unused variables in coloured Petri net Classes*/
	std::set<wxString>   FindUnusedVariables();

	/*delete unused constant from user defined declarations*/
	void DeleteUnusedConstant(wxString p_sName);

	/*delete unused function from declarations tree*/
	void DeleteUnusedFunction(wxString p_sName);

	/*delete unused color set from colored Petri net class*/
	void DeleteUnusedColorSet(wxString p_sCsName);

	/*delete unused variable from colored Petri net class*/
	void DeleteUnusedVariable(wxString p_sVarName);

	std::map<wxString, wxString> TrimAliasColorSets(wxString p_sDec);

	/*returns the dependencies of an unused constant*/
	ConstantDependencyInfo  FindConstantsDependencies(wxString p_sConstantName);

	/*returns the dependencies of an unused function*/
	FunctionDependencyInfo  FindFunctionDependencies(wxString p_sFunName);

	/*returns the dependencies of an unused variable*/
	VariableDependencyInfo  FindVariableDependencies(wxString p_sVarName);


	set<wxString>  ObtainSimpleCsConstantsDependent(wxString p_sConstantName);

	/*returns the dependencies of an unused Simple cs*/
	SimpleCsDependencyInfo FindSimpleCSDependencies(wxString p_sCSName);

	/*returns the dependencies of an unused compound cs*/
	CompoundCsDependencyInfo FindCompoundCSDependencies(wxString p_sCSName);

	CompoundCsDependencyInfo FindCompoundCSDependForSimpleCs(wxString p_sCSName);

	/*returns the dependencies of an unused sub cs*/
	SubSetCsDependencyInfo  FindSubCSDependency(wxString p_sCSName);

	/*returns the dependencies of an unused observer*/
	ObserverDependencyInfo  FindObserverDependency(wxString p_sObserverName);

	/*returns the dependency info of alias cs*/
	AliasCsDependencyInfo   FindAliasCSDependency(wxString p_sAliasName);


	/*returns all defined functions*/
	set<wxString> ReadAllDefinedFunctions();

protected:
	SP_DS_Graph* m_pcGraph;

	SP_VectorString m_vUserDec;



};

#endif // __SP_DS_UNUSED_DECLARATIONS_H__
