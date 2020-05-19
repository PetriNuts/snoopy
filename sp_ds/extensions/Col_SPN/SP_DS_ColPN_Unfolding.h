//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/12/28 10:20:00 $
// Short Description: colored transition animator class
//////////////////////////////////////////////////////////////////////

//#define _CRTDBG_MAP_ALLOC

#ifndef __SP_DS_ColPN_Unfolding_H__
#define __SP_DS_ColPN_Unfolding_H__

#include "snoopy.h"
#include "sp_utilities.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_Binding.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Driver.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"

#include "sp_ds/extensions/Col_PN/Preprocessing/SP_ColPN_ProcessRateFunction.h"

#include "sp_ds/extensions/Col_PN/Preprocessing/SP_ColPN_ProcessGuard.h"

#include "sp_gui/dialogs/dia_ColSPN//SP_DLG_ColStUnfolding.h"

#include <wx/ffile.h>


struct SP_CPN_PlaceInfo
{
	wxString m_sColoredName;
	long     m_nID;
	wxString m_sRealName;
	vector<SP_VectorLong> m_anNetMarkings;
	wxString m_sType;
};

struct SP_CPN_ArcInfo
{
	wxString m_sPlaceName;
	//wxString m_sTransName;	
	int      m_nMultiplicity;	
	wxString m_sType;
};

struct SP_CPN_TransInfo
{
	wxString m_sColoredName;
	long     m_nID;
	wxString m_sRealName;
	vector<SP_VectorString> m_anNetFunctions;
	wxString m_sType;

	vector<SP_CPN_ArcInfo> m_vPreArcs;
	vector<SP_CPN_ArcInfo> m_vPostArcs;
};


struct SP_CPN_Expression
{
	SP_CPN_ExprType					m_eExprType;
	wxString						m_sExpression;
	wxString						m_sColorSetName;
	SP_DS_Node*						m_pcPlaceNode;	
	SP_DS_Node*						m_pcTransNode;	
	SP_DS_Edge*						m_pcEdge;
	//list<SP_CPN_ParsedColors>		m_ParsedColorsList;
	vector<wxString>				m_RateFunctionVector;
	vector<map<wxString, SP_ColPN_RateFunction> > m_RateFunctionTransformation;  //for rate functions	
	vector<SP_CPN_Var_Color>        m_vVariable2Color;

	SP_CPN_Parse_Context_ptr			m_pParseContext;
	SP_CPN_Driver_ptr			m_pDriver;

	vector<SP_CPN_EvaluatedSingleValue>			m_ParsedInfo;

	wxString						m_sPredicate; //only for rate functions
	unsigned int                    m_nColumn;    //only for rate functions

	wxString						m_sErrorPosition;
	wxString						m_sColPlaceName;
	wxString						m_sPlaceNodeClass;
	wxString						m_sEdgeClass;
};

//Store arc information during unfolding a continuous PN
struct SP_CPN_ContinuousArc
{
	long m_nTranPos; //In the case of output to APNN, it is used to store TransID
	wxString m_sArcClass;
	wxString m_sMultiplicity;
	long m_nPlaceID; // only for the output to APNN

};

//Store the Positions of unfolded nodes for a colored node
struct SP_CPN_CountInterval
{
	unsigned int m_nLow;
	unsigned int m_nUp;
};


struct SP_CPN_UnfoldedPlaceInfo
{
	bool			m_bIsolated;
	bool 			m_bFixed;
	SP_VectorLong	m_anNetMarkings;
	SP_VectorDouble m_adNetMarkings;
	wxString		m_sNodeType; 
	wxString		m_sColorSet;
};


struct SP_CPN_UnfoldedArcInfo
{
	wxString m_sColPlaceName;
	wxString m_sColor;
	long     m_nMultiplicity;
	double   m_dMultiplicity;
	wxString m_sMultiplicity;
	wxString m_sArcType;
	wxString m_sDiscContType;
};

struct SP_CPN_UnfoldedTransInfo
{
	bool 							m_bReversible;
	wxString						m_sAnimTransInstName;
	SP_VectorString					m_anNetFunctions;
	wxString						m_sType;	
	vector<SP_CPN_UnfoldedArcInfo>	m_vInputArcs;
	vector<SP_CPN_UnfoldedArcInfo>	m_vOutputArcs;
};

// alphanumeric comparision of strings
struct alphanum_comparer
{
	bool operator() (const wxString& left, const wxString& right) const;
};

typedef std::map<wxString,SP_CPN_UnfoldedPlaceInfo,alphanum_comparer> SP_CPN_UnfoldedPlace;
typedef std::map<wxString,SP_CPN_UnfoldedPlace > SP_CPN_UnfoldedPlaces;
typedef std::map<wxString,SP_CPN_UnfoldedTransInfo,alphanum_comparer> SP_CPN_UnfoldedTransition;
typedef std::map<wxString,SP_CPN_UnfoldedTransition > SP_CPN_UnfoldedTransitions;

class SP_DS_ColPN_Unfolding: public SP_Error
{

public:

	SP_DS_ColPN_Unfolding();
	virtual ~SP_DS_ColPN_Unfolding();


//configuration setting for unfolding
protected: 
	wxString m_sLoadFilename;
	wxString m_sSaveFilename;
	wxString m_sUnfoldSource;	//wxT("Unfold") or wxT("Load")
	bool     m_bWriteFlag;		// if write the unfolding to a file
	bool     m_bIddDsszUtilSolve;
	bool     m_bGecodeDsszUtilSolve;
	bool     m_bCSPSolve;
	wxString m_sCSPSearch;		//wxT("DSF") and wxT("BDB")
	unsigned long  m_nThreadCount;
	bool     m_bRemIsoPlaces;
	bool     m_bRemIsoTranss;

public:
	void SetLoadFilename(wxString p_sFileName) { m_sLoadFilename = p_sFileName; }
	void SetSaveFilename(wxString p_sFileName) { m_sSaveFilename = p_sFileName; }
	void SetUnfoldSource(wxString p_sSourceFlag) { m_sUnfoldSource = p_sSourceFlag; }
	void SetWriteFlag(bool p_bWriteFlag) { m_bWriteFlag = p_bWriteFlag; }
	void SetIddDsszUtilSolveFlag(bool p_bVal) {m_bIddDsszUtilSolve = p_bVal;}
	void SetGecodeDsszUtilSolveFlag(bool p_bVal) {m_bGecodeDsszUtilSolve = p_bVal;}
	void SetCSPSolveFlag(bool p_bVal) {m_bCSPSolve = p_bVal;}
	void SetCSPSearch(wxString p_sCSPSearch) { m_sCSPSearch = p_sCSPSearch; }
	void SetThreadCount(long p_nThreadCount) { m_nThreadCount = p_nThreadCount;}
	void SetRemIsoPlFlag(bool p_sRemIsoPlFlag) {m_bRemIsoPlaces = p_sRemIsoPlFlag;}
	void SetRemIsoTrFlag(bool p_bRemIsoTranss) {m_bRemIsoTranss = p_bRemIsoTranss;}



//variables used during unfolding 
protected:
	SP_CPN_ColorSetClass m_ColorSetClass;
	SP_CPN_ValueAssign m_ValueAssign;
	SP_DS_Graph* m_pcGraph;
	wxString m_sNetClass;
	vector<SP_DS_Node*> m_vTransitions;	
	long int m_nTranNumPerThread;  //the number of transitions that is for each thread, for progress indication
	long int m_nTranNumInFirstThread;	
	map<wxString, wxString> m_mPrePlaceName2ColorSetName;  //record the preplace and its color set name for one transition
	map< wxString,wxString > m_mssColPlaceName2ColorSetforOrdering;

protected:
	SP_CPN_UnfoldedPlaces m_mmUnfoldedDiscPlaces;    //colored name, color, other info
	SP_CPN_UnfoldedPlaces m_mmUnfoldedContPlaces;

	SP_CPN_UnfoldedTransitions m_mmUnfoldedStochTransions; //colored name, color, other info
	SP_CPN_UnfoldedTransitions m_mmUnfoldedContTransions; //colored name, color, other info
	SP_CPN_UnfoldedTransitions m_mmUnfoldedImmTransions; //colored name, color, other info
	SP_CPN_UnfoldedTransitions m_mmUnfoldedDetTransions; //colored name, color, other info
	SP_CPN_UnfoldedTransitions m_mmUnfoldedSchedTransions; //colored name, color, other info

public:
	SP_CPN_UnfoldedPlaces& GetUnfoldedDiscPlaces(){ return m_mmUnfoldedDiscPlaces; }
	SP_CPN_UnfoldedPlaces& GetUnfoldedContPlaces(){ return m_mmUnfoldedContPlaces; }

	SP_CPN_UnfoldedTransitions& GetUnfoldedStochTransions(){return m_mmUnfoldedStochTransions;}
	SP_CPN_UnfoldedTransitions& GetUnfoldedContTransions(){return m_mmUnfoldedContTransions;}
	SP_CPN_UnfoldedTransitions& GetUnfoldedImmTransions(){return m_mmUnfoldedImmTransions;}
	SP_CPN_UnfoldedTransitions& GetUnfoldedDetTransions(){return m_mmUnfoldedDetTransions;}
	SP_CPN_UnfoldedTransitions& GetUnfoldedSchedTransions(){return m_mmUnfoldedSchedTransions;}



//for configeration dialogue
protected:
	SP_DLG_ColStUnfolding* m_pcUnfoldDlg;
	wxStopWatch m_cUnfoldingStopWatch;
	long m_nLastUpdateTime;
	unsigned long m_nCurrentIterationNumber;
	bool m_bFlagUnfoldingRunning;
	bool m_bThreadSuccess;
	bool m_bFlagPause;

protected:
	//statistics
	long int m_nPlaceCountStat;
	long int m_nTransitionCountStat;
	long int m_nOrdinaryArcNum;
	long int m_nReadArcNum;
	long int m_nInhibitorArcNum;
	long int m_nResetArcNum;
	long int m_nEqualArcNum;
	long int m_nModifierArcNum;

public:
	bool IsUnfoldingRunning() { return m_bFlagUnfoldingRunning; }
	void AbortUnfolding() { m_bFlagUnfoldingRunning = false; }
	void UpdateUnfoldDlg(unsigned long p_nFinishedNumber);

	bool IsUnfoldingPausing() { return m_bFlagPause; }
	void SetResumePause(bool p_bFlag) { m_bFlagPause = p_bFlag; }



public:	
	bool Start();	   //called by configuration dialogue
	bool Unfolding();  //the entrance to unfolding

	void ModifyName(wxString &p_sPlaceName);

protected:
	//places
	bool Initialize();
	bool UnfoldPlaceNodeClass(wxString p_sPlaceNodeClass);
	bool UnfoldPlaces();		
	bool GetTransitions();
	virtual bool ProcessResult();
	wxString FormatByComma(wxString p_sNumber);
	void ReportUnfoldingStatistics();

	

protected:
	void FindIsolatedPlaces(SP_CPN_UnfoldedTransitions & p_mmUnfoldedTransions);
	void AssignPlacePositions(SP_CPN_UnfoldedPlaces& p_mmUnfoldedPlaces );
	void AssignPlacePositionsNew(SP_CPN_UnfoldedPlaces& p_mmUnfoldedPlaces );	
	void AssignTransPositions(SP_CPN_UnfoldedTransitions & p_mmUnfoldedTransions);
	void ClearPlaces();
	void ClearTranss();

	//void SetStochInputArcDeltaVector(long p_nTransArrayPos, vector<SP_CPN_UnfoldedArcInfo>& m_vArcs);
	//void SetStochOutputArcDeltaVector(long p_nTransArrayPos, vector<SP_CPN_UnfoldedArcInfo>& m_vArcs);
	void SetContInputArcDeltaVector(long p_nTransArrayPos, vector<SP_CPN_UnfoldedArcInfo>& m_vArcs);
	void SetContOutputArcDeltaVector(long p_nTransArrayPos, vector<SP_CPN_UnfoldedArcInfo>& m_vArcs);

public:
	bool UnfoldPlacesForMarkingDependent(); //for marking-dependent arcs, added by Fei, 09.2015

protected:
	//places
	SP_MapString2Long m_mnPlaceArrayPos;
    SP_VectorString m_msPlaceNames;	
	SP_VectorString m_msColorsOfPlace; 
	SP_VectorString m_msColoredPlaceNames;
	map<wxString, vector<wxString> > m_svColored2UnColoredPlaceNames; //for marking-dependent arcs, added by Fei, 09.2015
    SP_MapString2String m_msPlaceNamesById;
	SP_MapString2String m_msColPlName2ColSet;
	map<long, SP_CPN_CountInterval> m_nsPlaceCountById;
	vector<wxString>  m_vsPlaceNodeType;
	vector<SP_VectorLong> m_anNetMarkings;  //for colored qualitative/stochastic PN	
	vector<vector<double> > m_anColCPNNetMarkings; //for colored continuous PN
	vector<double> m_adInitialNetMarking;
	vector<long> m_anInitialNetMarking;
	vector<double> m_adHybInitialNetMarking;
	long m_nPlaceCount;

	//transitions
	SP_MapString2Long m_mnTransitionArrayPos;
    SP_VectorString m_msTransitionNames;
	SP_VectorString m_msColoredTransitionNames;
    SP_MapString2String m_msTransitionNamesById;
	vector<wxString>  m_vsTransNodeType;
	vector<SP_VectorString> m_anNetFunctions;
	SP_VectorString m_msTransitionFunctions;
	map<long, SP_CPN_CountInterval> m_nsTransitionCountById;
	map< long, SP_MapString2Long > m_mmPrePlaceArrayPosOfTransitionNode;  //trans id, preplace name, preplace id
	SP_MapString2String m_msAnimationTransitionNamesById;   //only for animation
	

	//ARCS
	vector<SP_CPN_ContinuousArc> m_vArcs_Place2Transition;
	vector<SP_CPN_ContinuousArc> m_vArcs_Transition2Place;

	//arcs
	//for colored stochastic PN
	//SP_MatrixDouble* m_aanDeltaIncMatrix;
   // SP_MatrixBool* m_aanResetIncMatrix;
    //SP_MatrixPairDouble* m_aanPreIncMatrix;
	//SP_VectorBool m_abNonConsumingMode;

	//for APNN
	//vector<SP_CPN_ContinuousArc> m_vPreTransitionArc;
	//vector<SP_CPN_ContinuousArc> m_vPostTransitionArc;

	//for colored continuous PN
	//map< long, SP_MapString2String > m_mPrePlaceToArcsOfTransitionNode;
	//map< long, SP_MapString2String > m_mPostPlace2ArcsOfTransitionNode; 
	//map<long, vector<SP_CPN_ContinuousArc> > m_mvPlaceID_PreTransitionID;
	//map<long, vector<SP_CPN_ContinuousArc> > m_mvPlaceID_PostTransitionID;

	

	//Get MARKINGS AND RATE functions
public:
	vector<double>*		GetCurCPNMarking();
	vector<double>*		GetCurHybPNMarking();
	vector<long>*		GetCurColStMarking();
	SP_VectorString*	GetCurRateFunction();

public:
	//places
	SP_MapString2Long* GetPlaceArrayPos() { return &m_mnPlaceArrayPos; }
    SP_VectorString* GetPlaceNames() { return &m_msPlaceNames; }
	SP_VectorString* GetColoredPlaceNames() { return &m_msColoredPlaceNames; }
    SP_MapString2String* GetPlaceNamesById() { return &m_msPlaceNamesById; }
	SP_VectorString* GetColorsOfPlace(){ return &m_msColorsOfPlace;}
	SP_MapString2String* GetColPlName2ColSet() { return &m_msColPlName2ColSet; }
	map<long, SP_CPN_CountInterval>* GetPlaceCountById(){ return &m_nsPlaceCountById ;}

	map<wxString, vector<wxString> >* GetsvColored2UnColoredPlaceNames(){ return &m_svColored2UnColoredPlaceNames; }

	//workaround to get the number of continuous places
    unsigned int GetContinuousPlaceCount(){return m_anColCPNNetMarkings.size();}

	SP_CPN_CountInterval GetPlaceCountById(const unsigned long& p_nID){ return m_nsPlaceCountById[p_nID];}

	vector<vector<double> > * GetColCPNNetMarkings() { return &m_anColCPNNetMarkings; }
	vector<SP_VectorLong> * GetNetMarkings() { return &m_anNetMarkings; }
	map<long, SP_CPN_CountInterval>* GetTransitionCountById(){ return &m_nsTransitionCountById ;}

	SP_CPN_CountInterval GetTransitionCountById(const unsigned long& p_nID){ return m_nsTransitionCountById[p_nID];}

	//transitions
    SP_MapString2Long* GetTransitionArrayPos() { return &m_mnTransitionArrayPos; }
    SP_VectorString* GetTransitionNames() { return &m_msTransitionNames; }
	SP_VectorString* GetColoredTransitionNames() { return &m_msColoredTransitionNames; }
    SP_MapString2String* GetTransitionNamesById() { return &m_msTransitionNamesById; }
	vector<SP_VectorString>* GetNetFunctions() { return &m_anNetFunctions; }
	SP_MapString2String* GetAnimationTransitionNamesById() { return &m_msAnimationTransitionNamesById; }
	map< long, SP_MapString2Long >* GetPrePlace() { return &m_mmPrePlaceArrayPosOfTransitionNode; }
	vector<wxString>* GetTransNodeType() { return &m_vsTransNodeType; }

	//ARCS
	vector<SP_CPN_ContinuousArc>* GetArcs_Place2Transition(){ return &m_vArcs_Place2Transition;}
	vector<SP_CPN_ContinuousArc>* GetArcs_Transition2Place(){ return &m_vArcs_Transition2Place;}

	SP_CPN_ColorSetClass* GetColorSetClass() { return &m_ColorSetClass;}

	//SP_VectorBool* GetNonConsumingMode() { return &m_abNonConsumingMode;}
	//arcs -- colored stochastic PN
	//SP_MatrixDouble** GetDeltaIncMatrix(){return &m_aanDeltaIncMatrix; }
	//SP_MatrixBool** GetResetIncMatrix(){return &m_aanResetIncMatrix ;}
	//SP_MatrixPairDouble** GetPreIncMatrix(){return &m_aanPreIncMatrix; }

	
	//arcs -- colored continuous PN
	//map< long, SP_MapString2String >* GetPlaceToArcsOfTransitionNode(){return &m_mPrePlaceToArcsOfTransitionNode; }
	//map< long, SP_MapString2String >* GetPostPlace2ArcsOfTransitionNode(){return &m_mPostPlace2ArcsOfTransitionNode; }
	//map<long, vector<SP_CPN_ContinuousArc> >* GetPlaceID_PreTransitionID(){return &m_mvPlaceID_PreTransitionID; }
	//map<long, vector<SP_CPN_ContinuousArc> >* GetPlaceID_PostTransitionID(){return &m_mvPlaceID_PostTransitionID; }

	

	//arcs -- for APNN
	//vector<SP_CPN_ContinuousArc>* GetPreTransitionArc(){ return &m_vPreTransitionArc; }
	//vector<SP_CPN_ContinuousArc>* GetPostTransitionArc() { return &m_vPostTransitionArc; }

	



	//for export to a file
private:
	wxFFile m_file;
protected:
	bool WriteToFile();

	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//for configurating multi-threads
public:
	bool StartThreadUnfolding();	

private:
	// crit section
	wxCriticalSection m_critsect;
	//unsigned long m_nThreadCount;
	unsigned long m_nThreadFinished;
	

public:
	//thread functions
	void ThreadFinished();
	void GetTransInstances(SP_CPN_UnfoldedTransition & p_mUnfoldedOneTransInfo, SP_CPN_UnfoldedTransition& p_msstUnfoldedTransInfo);
	void ThreadAddStochResult( SP_CPN_UnfoldedTransitions& p_mmUnfoldedStochTransions);
	void ThreadAddContResult( SP_CPN_UnfoldedTransitions& p_mmUnfoldedContTransions);
	void ThreadAddImmResult( SP_CPN_UnfoldedTransitions& p_mmUnfoldedImmTransions);
	void ThreadAddDetResult( SP_CPN_UnfoldedTransitions& p_mmUnfoldedDetTransions);
	void ThreadAddSchedResult( SP_CPN_UnfoldedTransitions& p_mmUnfoldedSchedTransions);
	void SetThreadSuccess(bool p_bThreadSuccess);

public:
	void ComputeBindings(SP_DS_Node* p_pcTransNode);
	bool SumPlaceInstances(map<wxString, wxString> *p_pmPlaceToColorSet, wxString &p_sRateFunExpr, SP_CPN_ColorSetClass* p_pcColorSetClass, bool p_bCheck=false);




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#if 1
	class UnfoldThread : public wxThread
	{
	
public:
		UnfoldThread(	unsigned long p_nThreadId, 
						SP_DS_ColPN_Unfolding* p_pcColPNUnfolding, 
						vector<SP_DS_Node*>    p_vTransitions,							
						SP_DS_Graph* p_pcGraph,						
						bool p_bCSPSolve
					);

		~UnfoldThread();

		UnfoldThread(){}
		
		// thread execution starts here
		virtual void *Entry();

		unsigned long GetCurrentFinishedNumber(){return m_nCurrentFinished;}

protected:
		unsigned long m_nThreadId;
		SP_DS_ColPN_Unfolding* m_pcColPNUnfolding;
		vector<SP_DS_Node*> m_vTransitions;
		//vector<SP_CPN_ThreadInfo> m_vpcTransExprs;
		vector<SP_CPN_Expression>* m_pArcsExpressionVector;		
		wxString m_sNetClass;	
		wxString m_sTransNodeClass;
		SP_CPN_ColorSetClass m_ColorSetClass;
		SP_CPN_ValueAssign m_ValueAssign;
		map<wxString, vector<SP_CPN_Collist_Declarations> > m_svDeclarations;

		SP_DS_Node* m_pcTransNode;

		SP_DS_Graph* m_pcGraph;

		unsigned long m_nCurrentFinished;
		bool     m_bCSPSolve;

protected:
	SP_CPN_Parse_Context_ptr m_pParseContext;
	SP_CPN_Driver_ptr  m_pDriver;
	vector<SP_CPN_Expression> m_ExpressionVector;		
	
	map<wxString, wxString> m_mPrePlaceName2ColorSetName;  //record the preplace and its color set name for one transition
	unsigned int m_nTransRateColumn;  //record the column number of the current transition
	map<wxString, wxString> m_mPlaceName2ColorSetName;

protected: //for one transition
	map<wxString, wxString> m_mBindVar2Val;
	vector<SP_CPN_Var_Color> m_vVar2ColorSet;

	vector<SP_CPN_Var_Color> m_vGuardVar2ColorSet;

	wxString m_sAnimationTransitionInstanceName;
	wxString m_sOneBinding;
	wxString m_sColoredTransName;
	wxString m_sTransType;

	SP_CPN_UnfoldedTransInfo m_scOneTransInfo;
	SP_CPN_UnfoldedTransition m_mTransInstances;

	//CSP
	vector< vector<unsigned int> > m_vvCSPAssigns;
	//for reuse
	map<wxString, vector< vector<unsigned int> > > m_vvExisBindMatrix;
	//CSP flag
	bool m_bCSPFlag;

	//by george
	std::map<wxString, wxString> m_mVar2Val; // tempory remember the varlist binding for evaluate vars in the rate function
protected:
	SP_CPN_UnfoldedTransitions m_mmUnfoldedStochTransions; //colored name, color, other info
	SP_CPN_UnfoldedTransitions m_mmUnfoldedContTransions; //colored name, color, other info
	SP_CPN_UnfoldedTransitions m_mmUnfoldedImmTransions; //colored name, color, other info
	SP_CPN_UnfoldedTransitions m_mmUnfoldedDetTransions; //colored name, color, other info
	SP_CPN_UnfoldedTransitions m_mmUnfoldedSchedTransions; //colored name, color, other info

protected:
	//transitions
	bool Unfold();
	bool UnfoldTransNodeClass(wxString p_sTransNodeClass);
	bool UnfoldTransitons();
	bool UnfoldOneTransition();

	//expressions
	bool InitializeParsedExprs();
	bool CollectPreArcExprs();
	bool CollectPostArcExprs();
	bool CollectGuard();

	//rate functions
	bool CollectRateFunction();
	bool CollectStochContRateFunction();
	bool CollectDetRateFunction();
	bool CollectImmRateFunction();
	bool CollectScheduleRateFunction();

	bool BuildParserTree();
	bool CheckVariableIsLegal();
	void CollectVariables();

	//2016.10.31 by Fei
	//implement the unique function
	bool m_bTransitionUnique;
	void ProcessUnique();

	
	

	bool ProcessConstantTransition();
	bool ProcessVariableTransition();
	bool Parsing();
	bool EvaluateGuard();
	bool DynamicBinding(int p_nVarPos);
	bool DynamicParse();
	bool ProcessTransition();

	
	wxString GetEdgeName(SP_DS_Edge* p_pcEdge);

	bool ProcessRateFunction(SP_CPN_Expression* p_eExpression, SP_VectorString& p_sNetFunctions, bool p_bExport = false);
	void CollectPlace2ColorMapforATransition( map<wxString, vector<wxString> > &p_mPlace2ColorsMap );
	void GenerateUnfoledRateFunction(map<wxString, vector<wxString> > p_mPlaceToColors, vector<wxString> p_sColoredRateFunctionVector, vector<wxString> &p_sUnfoldedRateFunctionVector);

	void ProcessArcs();
	void ModifyPlaceName(wxString &p_sPlaceName);

	bool IntCSSuccessiveTest();
	bool CSPSolve();	
	bool Binding(vector<SP_CPN_Var_Color>& p_vNonCSPVars, vector<vector<wxString> > & p_vvNonCSPBindingRes);
	bool ProcessTransitionByCSP();
	bool ProcessOneBinding(map<wxString,wxString>& p_mBindVar2Val);

	bool InitializeDeclaration();

	//to show all bindings for one transition
	bool Binding(SP_MapString2UInt &p_IndexMap, vector<vector<wxString> > & p_AllBindingVector); 

public:
	bool ComputeBindings(SP_DS_Node* p_pcTransNode, bool& p_bCSPSucceed, vector<wxString>& p_vsBindings);
	bool SumPlaceInstances(map<wxString, wxString> *p_pmPlaceToColorSet, wxString &p_sRateFunExpr, SP_CPN_ColorSetClass* p_pcColorSetClass, bool p_bCheck=false);


	};
#endif


};

#endif // __SP_DS_ColPN_Unfolding_H__


