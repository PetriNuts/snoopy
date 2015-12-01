//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////


#ifndef __SP_DLG_ColAuxiliaryVariable__
#define __SP_DLG_ColAuxiliaryVariable__

#include <wx/wx.h>
#include <wx/grid.h>

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
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Driver.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"

#include <wx/checkbox.h>

 const wxString m_PREDEFINEDFUNCTIONS[] =
 {
     _T("SUM")	 
 };

 enum SP_ColPN_NeighborDim
 {
	 NEIGHBOR1D,   //int Neighbor1D(x)
	 NEIGHBOR2D,   //int Neighbor2D(x,y)
	 NONEFUNCTION  
 };

class SP_DLG_ColAuxiliaryVariable : public wxDialog
{

  private:

    wxWindow* m_pcParent;
    wxBoxSizer* m_pcSizer;

   
	wxGrid* m_pcGrid;
	wxCheckBox *m_cbComSingleRun;
	SP_DS_Graph* m_pcGraph;

	bool m_bComSingleRun;

	//
	SP_CPN_Parse_Context_ptr m_pcParseContext;
	SP_CPN_Driver_ptr  m_pcDriver;	

	SP_CPN_ColorSetClass m_ColorSetClass;
	SP_CPN_ValueAssign m_ValueAssign;

	wxArrayString m_ArrayColoredPlaceNames;
	map<wxString,long> m_mColoredPlNames2Pos;

	SP_MapString2String m_msColPlName2ColSet;

	wxString m_sVarName;
	wxString m_sCurFunction;
	wxString m_sCurPredicate;
	SP_ColPN_NeighborDim m_PredType;

	wxString m_sSelColorSet;

	map<long, wxString> m_sSelUPlPos2Color;

	long m_n1Dimenstion;
	long m_n2Dimenstion;

	vector<double> m_vSingleTrace;	


  private:


	bool SaveData();	
	bool LoadData();

	bool Initialize();
	SP_ColPN_NeighborDim GetPredicateType(wxString p_sPredicate);
	bool ProcessSelPlaces(wxString p_sSelColPlaceName);
	bool ProcessPredicate();

	void OnAssistent(wxCommandEvent& p_cEvent);


  public:
	
    SP_DLG_ColAuxiliaryVariable( wxWindow* p_pcParent, const wxString& p_sTitle = wxT("Auxiliary variable definition"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
	void OnDlgApply( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent ); 

    void OnAdd( wxCommandEvent& p_cEvent ); 
    void OnDelete( wxCommandEvent& p_cEvent ); 


	void OnCellValueChanged(wxGridEvent& p_gEvent);
	void OnEditorShown( wxGridEvent& ev );
	void OnCheck( wxCommandEvent& p_cEvent );

	bool GetComSingleRunFlag(){ return m_bComSingleRun;}

	DECLARE_CLASS( SP_DLG_ColTraceAnalyzer )
	DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_ColAuxiliaryVariable__

