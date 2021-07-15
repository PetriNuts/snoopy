//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/22 11:20:00 $
// $Modified: George $
// $Modification Date:2.2021:update the assistant to be suitable for observers, color marking and color sets def.$
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_STFUNCTIONASSISTENT__
#define __SP_DLG_STFUNCTIONASSISTENT__

#include <wx/wx.h>
#include <wx/textctrl.h>

#include <wx/listctrl.h>
#include <wx/checkbox.h>

#include "sp_ds/SP_DS_Attribute.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StParser.h"

class SP_DLG_StFunctionAssistent;

class SP_DLG_StFunctionAssistent_ListCtrl : public wxListCtrl
{

  public:


    SP_DLG_StFunctionAssistent_ListCtrl( SP_DLG_StFunctionAssistent* p_pcDlg,
      wxWindow *parent, const wxWindowID id,
      const wxPoint& pos, const wxSize& size, long style )
    : wxListCtrl( parent, id, pos, size, style )
    {

      m_pcParentDlg = p_pcDlg;

    };

    void OnDblClick( wxListEvent& event );

  private :

    SP_DLG_StFunctionAssistent* m_pcParentDlg;

    DECLARE_EVENT_TABLE()

};

class SP_DLG_StFunctionAssistent : public wxDialog
{

  private :

    DECLARE_EVENT_TABLE()

    SP_DS_Graph* m_pcGraph;
    SP_DS_Node* m_pcParentNode;
    wxBoxSizer* m_pcSizer;

    wxString m_sReturnText;

    SP_VectorString* m_vUnfoldedPlaces;
    SP_VectorString* m_vUnfoldedTrans;

    SP_StParserType m_nAssistentType;

    wxTextCtrl* m_pcFormulaTextCtrl;
    wxTextCtrl* m_pcRegExTextCtrl;//for regExp

    SP_DLG_StFunctionAssistent_ListCtrl* m_pcPlaceListCtrl;
    SP_DLG_StFunctionAssistent_ListCtrl* m_pcParameterListCtrl;
    SP_DLG_StFunctionAssistent_ListCtrl* m_pcFunctionListCtrl;

    SP_DLG_StFunctionAssistent_ListCtrl* m_pcTransListCtrl;//by george for observers
    SP_DLG_StFunctionAssistent_ListCtrl* m_pcUnfoldedTransListCtrl;//by george for observers
    SP_DLG_StFunctionAssistent_ListCtrl* m_pcUnfoldedPlacesListCtrl;//by george for observers

    void LoadData();

    void LoadPlaces();
    void LoadPrePlaces();

    void LoadParameter();
    void LoadLookupTables();

    void LoadFunctions();


    void LoadTransitions();//by george for observers

    void LoadUnfoldedElements();//by george for observers

    void OnRegexSelection(wxCommandEvent& p_cEvent);//by george for observers

    DECLARE_CLASS( SP_DLG_StFunctionAssistent )

  protected :

  public:


    SP_DLG_StFunctionAssistent( SP_StParserType p_nAssistentType, SP_DS_Node* m_pcParentNode,
      wxString p_psReturnText, wxWindow* p_pcParent, wxString p_sHelpText = wxT(""),
      const wxString& p_sTitle = wxT("Assistent"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE, SP_VectorString* p_vUnfoldedTrans=NULL, SP_VectorString* p_vUnfoldedPlaces = NULL);

    void OnDlgOk( wxCommandEvent& p_cEvent );

    void OnDlgCancel( wxCommandEvent& p_cEvent );

    void OnCheckFormula( wxCommandEvent& p_cEvent );

    void InsertIntoFormulaText( const wxString& p_sInsertText );

    const wxString GetReturnText(){ return m_sReturnText; }

};

#endif // __SP_DLG_STFUNCTIONASSISTENT__

