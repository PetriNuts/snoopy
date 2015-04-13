//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2010/1/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_ColStEditTablePlot__
#define __SP_DLG_ColStEditTablePlot__

#include <wx/wx.h>
#include <wx/grid.h>

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StEditTablePlot.h"


class SP_DLG_ColStEditTablePlot : public wxDialog
{

  private:

    wxBoxSizer* m_pcMainSizer1;

    wxTextCtrl* m_pcNameTextCtrl;

    wxTextCtrl* m_pcXAxisMinTextCtrl;
    wxTextCtrl* m_pcXAxisMaxTextCtrl;

    wxTextCtrl* m_pcYAxisMinTextCtrl;
    wxTextCtrl* m_pcYAxisMaxTextCtrl;

    wxRadioBox* m_pcFixedPlotAdjustment;
    wxRadioBox* m_pcPlaceTransitionRadioBox;
	wxRadioBox* m_pcPlaceResultsRadioBox;

	 wxRadioBox* m_pcOutputTypeRadioBox;

    wxCheckBox* m_pcSumOfNodesCheckBox;

	wxCheckBox* m_pcChooseAllCheckBox;
	wxComboBox* m_pcChooseColoredPlace;

    bool m_bSumOfNodes;
    SP_DS_Metadata* m_pcEditMetadata;
	wxString m_sOutputType;
    wxString m_sPlaceTransition;
	wxString m_sPlaceResults;

    wxStaticBox* m_pcPlaceChoiceStaticBox;
    wxCheckListBox* m_pcPlaceChoiceCheckListBox;

    SP_MapString2Int m_mnPlaceChoicePos;
    SP_MapInt2String m_mnPlaceChoiceId;

	wxString m_sPlaceType;

	SP_MapString2String* m_pmsPlaceNamesById;
    SP_MapString2Long* m_pmnPlaceArrayPos;

	SP_MapString2Long* m_pmnTransitionArrayPos;
    SP_MapString2String* m_pmsTransitionNamesById;

	SP_VectorString* m_pmsPlaceNames;
	SP_VectorString* m_psTransitionNames;

	vector<wxString>* m_pMixexPlaceNameArray;
	vector<wxString>* m_pMixexTransitionNameArray;

private:
    void LoadData();
    bool SaveData();

    void LoadPlot( SP_DS_Metadata* p_pcMetadata );
    void LoadTable( SP_DS_Metadata* p_pcMetadata );

    SP_StTablePlotEditType m_eType;
    void EnableIntervalComponents();

    void OnChangedAdjustmentType( wxCommandEvent& p_cEvent );
    void OnChangedPlaceTransition( wxCommandEvent& p_cEvent );
    void OnChangedSumOfNodes( wxCommandEvent& p_cEvent );
	void OnChangedOutType( wxCommandEvent& p_cEvent );
	void OnChangedPlaceResults( wxCommandEvent& p_cEvent );

	void OnChooseAll( wxCommandEvent& p_cEvent );
	void OnClickItem( wxCommandEvent& p_cEvent );
	void OnComBoxSel( wxCommandEvent& p_cEvent );

  protected:

  public:

    SP_DLG_ColStEditTablePlot( SP_StTablePlotEditType p_eType,

		const wxString p_sPlaceType,
		SP_MapString2String* p_pmsPlaceNamesById,
		SP_MapString2String* p_pmsTransitionNamesById,
		SP_VectorString* p_psColoredPlaceNames,
		SP_VectorString* p_psColoredTransitionNames,
		vector<wxString>* p_pMixexPlaceNameArray,
		vector<wxString>* p_pMixexTransitionNameArray,

      SP_DS_Metadata* p_pcEditMetadata,
      wxWindow* p_pcParent,
      const wxString& p_sTitle = wxT("Edit table / plot"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );
   



  DECLARE_CLASS( SP_DLG_ColStEditTablePlot )
  DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_ColStEditTablePlot__

