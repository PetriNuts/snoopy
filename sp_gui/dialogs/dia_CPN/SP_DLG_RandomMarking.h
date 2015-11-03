//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description: randomly assign initial markings
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_RandomMarking__
#define __SP_DLG_RandomMarking__

#include <wx/wx.h>
#include <wx/grid.h>

#include "sp_core/SP_Map.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_utilities.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"

class SP_DLG_RandomMarking : public wxDialog
{
private:
	wxChoice* m_pcCSComboBox;
	wxGrid* m_pcColorGrid;
	wxGrid* m_pcPlaceGrid;

	SP_CPN_ColorSetClass m_cColorSetClass;
	wxString m_sPlaceNodeClass;

	map<wxString,int > m_mColorToMarkingMap;

	map<wxString, map<wxString,int> > m_mPlace2Color2Num;

	int m_nSelColumn;
	
	SP_DS_Graph* m_pcGraph;

private:
	bool LoadData();
	bool Initialize();

	bool GetInitialMarking();
	bool RandomAssign();
	void AddMultiSet(wxString p_sPlaceName, wxString p_Color, int p_Appearance);
	
	void OnComBoxChange(wxCommandEvent& p_cEvent);
	void onNumberGridClick(wxGridEvent& p_cEvent);

	bool Process();


  private:


    wxString m_sOldCellValue;

    wxWindow* m_pcParent;

	vector<wxString> m_ComboBoxValue;

	wxArrayString  m_choices;


  private:


	bool SaveData();
	
	void InitializeChoice();
  
  public:
	
    SP_DLG_RandomMarking( int p_nSelColumn, wxWindow* p_pcParent,
      const wxString& p_sTitle = wxT("Create a random marking"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );    
    void OnAddColorSet( wxCommandEvent& p_cEvent ); 
    void OnDeleteColorSet( wxCommandEvent& p_cEvent ); 
    void OnEditColorSet( wxCommandEvent& p_cEvent ); 
	void OnGridCellValueChanged(wxGridEvent& p_gEvent);

	void OnCheckFunction( wxCommandEvent& p_cEvent );
	
	

   DECLARE_CLASS( SP_DLG_RandomMarking )
   DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_RandomMarking__

