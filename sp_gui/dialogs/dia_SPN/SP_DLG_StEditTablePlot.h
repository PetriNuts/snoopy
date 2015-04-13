//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/28 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_STEDITTABLEPLOT__
#define __SP_DLG_STEDITTABLEPLOT__

#include <wx/wx.h>
#include <wx/grid.h>

typedef enum
{

  SP_ST_TABLE_PLOT_TYPE_NEW_TABLE,
  SP_ST_TABLE_PLOT_TYPE_NEW_PLOT,
  SP_ST_TABLE_PLOT_TYPE_EDIT_TABLE,
  SP_ST_TABLE_PLOT_TYPE_EDIT_PLOT,

} SP_StTablePlotEditType;


class SP_DLG_StEditTablePlot : public wxDialog
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

    wxCheckBox* m_pcSumOfNodesCheckBox;

	wxCheckBox* m_pcChooseAllCheckBox;

    bool m_bSumOfNodes;
    SP_DS_Metadata* m_pcEditMetadata;
    wxString m_sPlaceTransition;
    wxString m_sPlaceResults;

    wxStaticBox* m_pcPlaceChoiceStaticBox;
    wxCheckListBox* m_pcPlaceChoiceCheckListBox;

    SP_MapString2Int m_mnPlaceChoicePos;
    SP_MapInt2String m_mnPlaceChoiceId;

    wxString m_sPlaceType;

    void LoadData();
    bool SaveData();

    void LoadPlot( SP_DS_Metadata* p_pcMetadata );
    void LoadTable( SP_DS_Metadata* p_pcMetadata );

    SP_StTablePlotEditType m_eType;
    void EnableIntervalComponents();

    void OnChangedAdjustmentType( wxCommandEvent& p_cEvent );
    void OnChangedPlaceTransition( wxCommandEvent& p_cEvent );
    void OnChangedPlaceResults( wxCommandEvent& p_cEvent );
    void OnChangedSumOfNodes( wxCommandEvent& p_cEvent );
	void OnChooseAll( wxCommandEvent& p_cEvent );
	void OnClickItem( wxCommandEvent& p_cEvent );
  protected:

  public:

    SP_DLG_StEditTablePlot( SP_StTablePlotEditType p_eType,const wxString p_sPlaceType,
      SP_DS_Metadata* p_pcEditMetadata,
      wxWindow* p_pcParent,
      const wxString& p_sTitle = wxT("Edit table / plot"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );

  DECLARE_CLASS( SP_DLG_StEditTablePlot )
  DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_StEditTablePlot__

