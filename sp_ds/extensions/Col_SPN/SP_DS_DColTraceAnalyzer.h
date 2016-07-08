
#ifndef __SP_DS_DColTraceAnalyzer__
#define __SP_DS_DColTraceAnalyzer__

#include <wx/wx.h>

#include "sp_gui/widgets/SP_WDG_Notebook.h"
#include "sp_gui/widgets/SP_WDG_NotebookPage.h"

#include "sp_core/SP_Map.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_utilities.h"


#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"

#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_ColorProcessing.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Coloring.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Driver.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"

#include "sp_gui/dialogs/dia_ColSPN/TraceAnalyzer/SP_DLG_ColAuxiliaryVariable.h"




class SP_DS_DColTraceAnalyzer
{

private:

	SP_DS_Graph* m_pcGraph;

	//
	SP_CPN_Parse_Context_ptr m_pcParseContext;
	SP_CPN_Driver_ptr  m_pcDriver;

	SP_CPN_ColorSetClass* m_pcColorSetClass;
	SP_CPN_ValueAssign m_ValueAssign;

	SP_DS_ColPN_Coloring* m_pcColored;
	const SP_Vector2DDouble& m_aanResults;

	SP_VectorString* m_pvsColoredPlaceNames;

	map<wxString, long> m_mColoredPlNames2Pos;

	map<long, SP_CPN_CountInterval>* m_pnsPlaceCountById;
	SP_VectorString* m_pvsUnPlaceNames;

	SP_MapString2String* m_pmsColPlName2ColSet;
	SP_VectorString* m_pvsPlaceColor;


	wxString m_sVarName;
	wxString m_sCurFunction;
	wxString m_sCurPredicate;
	SP_ColPN_NeighborDim m_PredType;

	wxString m_sSelColorSet;

	map<long, wxString> m_sSelUPlPos2Color;

	vector< map<wxString, int> > m_vmCell2NBNum;
	map<wxString, vector< map<wxString, int> > > m_mExistingCell2NBNum;

	long m_n1Dimenstion;
	long m_n2Dimenstion;

	vector<double> m_vSingleTrace;
	SP_Vector2DDouble& m_aanUserResults;
	vector<wxString>&  m_vUserVariable;

	SP_Vector2DDouble m_aanOriginalUserResults;




private:
	bool GetColoredData();

	bool ProcessSelPlaces(wxString p_sSelColPlaceName);
	SP_ColPN_NeighborDim GetPredicateType(wxString p_sPredicate);

	//for the general case
	bool ComputeGeneral();
	bool GetSumSelUnPlaces(vector<long>& p_vSelPlPos);


	//for the special case Neighbor1D and Neighbor2D	
	bool ProcessPredicate();
	bool ComputeNeighborNumber();
	bool GetCellNeighbors(map<wxString, bool> &p_mCellExistToken);
	bool Match(wxString p_sCell, wxString p_sNBCell);
	bool SumNeighbors();

	void ReverseMatrix();




public:
	SP_DS_DColTraceAnalyzer(SP_DS_ColPN_Coloring* p_pcColored, const SP_Vector2DDouble& p_aanResults, SP_Vector2DDouble& p_aanUserResults, vector<wxString>&  p_vUserVariable);

	bool Compute();



};

#endif // __SP_DS_DColTraceAnalyzer__

