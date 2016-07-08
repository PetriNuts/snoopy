

#ifndef __SP_DS_ColPN_Coloring_H__
#define __SP_DS_ColPN_Coloring_H__

#include "snoopy.h"
#include "sp_defines.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Driver.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"

#include "sp_ds/extensions/Col_PN/Preprocessing/SP_ColPN_ProcessRateFunction.h"



#include "dssz/extern/alphanum.hpp"



#include <wx/ffile.h>


struct alphanum_comparerNc
{
	bool operator() (const wxString& left, const wxString& right) const;
};




struct SP_CPN_ColPlaceInfo
{
	wxString m_sColoredName;
	long     m_nID;
	wxString m_sRealName;
	vector<SP_VectorLong> m_anNetMarkings;
	wxString m_sType;
};

struct SP_CPN_ColCountInterval
{
	unsigned int m_nLow;
	unsigned int m_nUp;
};


class SP_DS_ColPN_Coloring
{
private:
	wxCriticalSection m_critsect;
protected:
	SP_CPN_ColorSetClass m_ColorSetClass;
	SP_DS_ColPN_Coloring* m_pcColored;
	map<wxString, wxString> m_mPrePlaceName2ColorSetName;  //record the preplace and its color set name for one transition
	map< wxString, wxString > m_mssColPlaceName2ColorSetforOrdering;

	vector<SP_DS_Edge*> m_pcpreEdgeP2TVector;
	//post Arcs
	vector<SP_DS_Edge*> m_pcpostEdgeVector;

	//places
	SP_MapString2Long m_mnColPlaceArrayPos;
	SP_MapString2String m_msPlaceNamesById;
	SP_VectorString m_msPlaceNames;
	SP_VectorString m_msColorSetOfPlace;
	SP_VectorString m_msColorSetOfstPlace;

	vector<wxString>  m_vsUncolPlaceNodeType;
	map<wxString, vector<wxString> > m_svColored2UnColoredPlaceNames;
	map<wxString, vector<wxString> > m_svColored2UnColoredstPlaceNames;

	



	SP_VectorString m_msColTransitionNames;

	SP_VectorString m_msColPreTransitionNames;
	SP_VectorString m_msColPostTransitionNames;

	SP_VectorString m_msColoredPlaceNames;
	SP_VectorString m_msColoredstPlaceNames;
	SP_VectorString m_msUnColoredPlacWColorSet;
	SP_VectorString m_msUnColoredstPlacWColorSet;
	SP_VectorString m_msMarking;
	SP_VectorString m_msstMarking;
	SP_VectorDouble m_mDMarking;
	SP_VectorString m_msColoredPrePlaceNames;
	SP_VectorString m_msColoredPostPlaceNames;
	wxString m_sNetClass;
	vector<SP_VectorLong> m_anNetMarkings;
	vector<double> m_adInitialNetMarking;
	vector<double> m_adHybInitialNetMarking;
	vector<long> m_anInitialNetMarking;
	vector<long> m_anstplaceMarking;
	vector<vector<double> > m_anColCPNNetMarkings; //for colored continuous PN
	SP_DS_Graph* m_pcGraph;
	SP_CPN_ValueAssign m_ValueAssign;

	//transition
	vector<wxString>  m_vsColTransNodeType;
	vector<SP_DS_Node*> m_vColTransitions;
	vector<SP_VectorString> m_anNetFunctions;
	SP_VectorString m_msColoredTransitionNames;
	SP_VectorString m_msColoredstTransitionNames;
	SP_VectorString m_msColoredTransGuard;
	SP_VectorString m_msColTransRatFun;
	SP_VectorString m_msColstTransRatFun;

	map<long, SP_CPN_ColCountInterval> m_nsPlaceCountById;
	map<long, SP_CPN_ColCountInterval> m_nsTransitionCountById;
	map<long, SP_CPN_ColCountInterval> m_nsstTransitionCountById;

	SP_VectorString m_msColTransitionFunctions;
	wxString m_sColoredTransName;
	map< long, SP_MapString2Long > m_mmPrePlaceArrayPosOfTransitionNode;
	vector<SP_DS_Edge*> m_pcEdgeVector;
	vector<SP_DS_Edge*> m_pcPreEdgesofplace;
	vector<SP_DS_Edge*> m_pcPostEdgesofplace;
	SP_VectorString m_pcEdgeExpression;
	SP_VectorString m_pcGuardsExpression;
	SP_VectorString m_pcReadEdgeExpression;
	SP_VectorString m_pcInhibitorEdgeExpression;
	SP_VectorString m_pcModifierEdgeExpression;
	SP_VectorString m_pcEdgeName;
	SP_VectorString m_pcReadEdgeName;
	SP_VectorString m_pcInhibitorEdgeName;
	SP_VectorString m_pcModifierEdgeName;
	wxString m_sArcsInformation;
	wxString m_sTranswithGuard;
	//SP_VectorString m_adMarkinguncolor;
	SP_VectorString m_vsUncoloredPlaces;
	SP_VectorString m_vsUncoloredstPlaces;
	SP_VectorString m_vsallPlaces;

	
	SP_DS_Node* m_pcTransNode;
	SP_DS_Node* m_pcPlaceNode;
	vector<SP_DS_Node*> m_vcolTransNode;
	vector<SP_DS_Node*> m_vcolstTransNode;
	vector<SP_DS_Node*> m_vcolplaceNode;
	vector<SP_DS_Node*> m_vcolstplaceNode;
	wxString m_sTransNodeClass;
	unsigned int m_nTransRateColumn;  //record the column number of the current transition

public:
	
	vector<SP_DS_Node*>* GetplaceNode() { return &m_vcolplaceNode; }
	SP_VectorString* GetColoredPlaceNames() { return &m_msColoredPlaceNames; }
	SP_VectorString* GetColoredstPlaceNames() { return &m_msColoredstPlaceNames; }
	SP_VectorString* Getmarking() { return &m_msMarking; }
	SP_VectorString* GetUncolorplaces() { return  &m_vsUncoloredPlaces; }
	SP_VectorString* Getallplaces() { return  &m_vsallPlaces;}
	SP_VectorDouble* GetmarkingDouble() { return &m_mDMarking; }
    SP_VectorLong* GetmarkingstPlaces() { return &m_anstplaceMarking; }
	SP_VectorString* GetmarkingString() { return &m_msMarking; }
	vector<SP_VectorLong> * GetNetMarkings() { return &m_anNetMarkings; }
	SP_MapString2Long* GetColPlaceArrayPos() { return &m_mnColPlaceArrayPos; }
	SP_MapString2String* GetPlaceNamesById() { return &m_msPlaceNamesById; }
	SP_VectorString* GetPlaceNames() { return &m_msUnColoredPlacWColorSet/*m_vsUncoloredPlaces*/; }
	SP_VectorString* GetColorSetOfPlace() { return &m_msColorSetOfPlace; }
	SP_VectorString* GetColorSetOfPlaceUpdate() { return &m_msUnColoredPlacWColorSet; }
	map<long, SP_CPN_ColCountInterval>* GetPlaceCountById() { return &m_nsPlaceCountById; }
	
	map<wxString, vector<wxString> >* GetsvColored2UnColoredPlaceNames() { return &m_svColored2UnColoredPlaceNames; }
public:
	wxString GetEdgeName(SP_DS_Edge* p_pcEdge);
	SP_VectorString* GetEdgeExpression() { return &m_pcEdgeExpression; }
	SP_VectorString* GetGuardExpression() { return &m_pcGuardsExpression; }
	
	
	vector<SP_DS_Edge*>* GetEdgeVector() { return &m_pcEdgeVector; }
	SP_VectorString* GetEdgeName() { return &m_pcEdgeName; }
	
	
	vector<SP_DS_Edge*>* GetArcs_place2Transition() { return &m_pcpreEdgeP2TVector; }
	SP_VectorString* GetReadEdgeExpression() { return &m_pcReadEdgeExpression; }
	long m_nLastUpdateTime;
	
	
	bool Coloring();
	void ReportColoringNet();
	
	wxString ArcsInformation();
	
	wxString* GetArcsInformation() { return &m_sArcsInformation; }
	wxString* GetTranswithArcs() { return &m_sTranswithGuard; }
	bool Start();
	
	unsigned int GetContinuousPlaceCount() { return m_anColCPNNetMarkings.size(); }
	
	wxString GuardeffectonPlaceName(SP_DS_Node* p_pcPlaceNode);
	wxString GuardeffectonprePlaceName(SP_DS_Node* p_pcPlaceNode);
	wxString GuardeffectonpostPlaceName(SP_DS_Node* p_pcPlaceNode);
	wxString GuardParsingprePlace(SP_DS_Node* p_pcTransNode, wxString& p_sArcVariable,
		                  long& p_nColorSetSize, vector<wxString>& p_ncolorSize, wxString& p_sColorSetName);
	wxString GuardParsingpostPlace(SP_DS_Node* p_pcTransNode, wxString& p_sArcVariable,
		long& p_nColorSetSize, vector<wxString>& p_ncolorSize, wxString& p_sColorSetName);
	
	wxString GetArcVariable(SP_DS_Edge* p_pcEdge);
	wxString evaluateGuardontheArc(SP_DS_Node* p_pcSourceNode, SP_DS_Edge* p_pcEdge, SP_DS_Node* p_pcTargetNode);

	wxString evaluateGuardonthepostArc(SP_DS_Node* p_pcSourceNode, SP_DS_Edge* p_pcEdge,
		                               SP_DS_Node* p_pcTargetNode, wxString& p_sArcWeight);
	wxString evaluateGuardonthepreArc(SP_DS_Node* p_pcSourceNode, SP_DS_Edge* p_pcEdge,
		                              SP_DS_Node* p_pcTargetNode, wxString& p_sArcWeight);
	
	wxString GetArcweightwithSize(SP_DS_Node* p_pcSourceNode, SP_DS_Edge* p_pcEdge, SP_DS_Node* p_pcTargetNode);
public:
	bool AssignTransInfo();
	SP_VectorString* GetColoredTransitionNames() { return &m_msColoredTransitionNames; }
	SP_VectorString* GetColoredstTransitionNames() { return &m_msColoredstTransitionNames; }
	SP_VectorString* GetColoredTranGuard() { return &m_msColoredTransGuard; }
	vector<SP_VectorString>* GetNetFunctions() { return &m_anNetFunctions; }
	
	vector<wxString>* GetTransNodeType() { return &m_vsColTransNodeType; }
	map< long, SP_MapString2Long >* GetPrePlace() { return &m_mmPrePlaceArrayPosOfTransitionNode; }
	map<long, SP_CPN_ColCountInterval>* GetTransitionCountById() { return &m_nsTransitionCountById; }
	SP_VectorString* GetColTransRatFun() { return &m_msColTransRatFun; }
	SP_VectorString* GetColstTransRatFun() { return &m_msColstTransRatFun; }
	SP_VectorString* GetPrecolTrans() { return &m_msColPreTransitionNames; }
	wxString ColTransitionsRatfn(SP_DS_Node* p_pcTransNode);
	wxString TranswithGuards();

	

protected:
	
	vector<vector<double> > * GetColCPNNetMarkings() { return &m_anColCPNNetMarkings; }
	SP_CPN_Driver_ptr  m_pDriver;
	bool AssignplacesInfo();
	bool AssignArcInfo();
	wxString FormatByComma(wxString p_sNumber);
	//void ReportColorednet();
	bool ClearColoredNet();
	bool LoadColoredNet();




	map<wxString, wxString> m_mBindVar2Val;
	vector<SP_CPN_Var_Color> m_vVar2ColorSet;
	vector<SP_CPN_Var_Color> m_vGuardVar2ColorSet;



public:
	
	wxString GetEdgeWeight(SP_DS_Edge* p_pcEdge);
	


protected:
	//colerd
	long int m_nColPlaceCount;
	long int m_nColTransitionCount;
	long int m_nOrdinaryArcNum;
	long int m_nReadArcNum;
	long int m_nInhibitorArcNum;
	long int m_nResetArcNum;
	long int m_nEqualArcNum;
	long int m_nModifierArcNum;

protected:
	
	bool Initialize();
	bool GetColTransitions();

public:
	SP_DS_ColPN_Coloring();
	~SP_DS_ColPN_Coloring();
};

#endif // __SP_DS_ColPN_Coloring_H__
