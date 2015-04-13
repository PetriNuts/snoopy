//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2010/08/01 10:20:00 $
// Short Description: generate twin nets class
//////////////////////////////////////////////////////////////////////

#ifndef __SP_CPN_GenMasterNet__
#define __SP_CPN_GenMasterNet__

#include <wx/wx.h>
#include "sp_core/SP_Vector.h"
#include "sp_core/SP_Map.h"

#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"



class SP_CPN_GenMasterNet
{

private:
	wxString m_sFilePath;
	SP_DS_Graph* m_pcGraph;
	wxString m_sColorMax;
	wxString m_sColorSetName;
	wxString m_sVariableName;

	map< wxString, vector<wxString> > m_mvTransitionToColors;
	map< wxString, map<wxString, long> > m_mmColor2PlaceName2Token;

	wxString m_sTransNodeClass;
	wxString m_sPlaceNodeClass;
	wxString m_sStandardEdgeClass;
	wxString m_sNetClass;

private:
	void GenerateInitialMarking(SP_DS_Node* p_pcNode, vector<wxString>* p_pvColors);

	void ShowArcExpression(SP_DS_ColListAttribute * p_pcColList);
	bool GenerateDeclaration();
	bool ProcessTransition();
	bool ProcessPlace();
	bool ProcessArc(wxString p_sEdgeClassName);

	bool Generate();


public:
	SP_CPN_GenMasterNet(SP_DS_Graph* p_pcGraph);
	~SP_CPN_GenMasterNet();

	bool GenerateMasterNet();	
	


//for testing
private:
	vector<vector<wxString> > CompMSum( vector<vector<wxString> > A, vector<vector<wxString> > B, wxString c);
	vector<vector<wxString> > CompMMlt( vector<vector<wxString> > A, vector<vector<wxString> > B);
	vector<vector<wxString> > CompKMlt( vector<vector<wxString> > A, vector<vector<wxString> > B);
	void compute();
	


};

#endif // __SP_CPN_GenMasterNet__

