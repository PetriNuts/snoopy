//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_ExpressionAssistent__
#define __SP_DLG_ExpressionAssistent__

#include <wx/wx.h>
#include <wx/textctrl.h>

#include <wx/listctrl.h>
#include <wx/checkbox.h>

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"


enum SP_CPN_ExprAssistType
{
	EXPRESSION_ARCGUARD,
	EXPRESSION_TRACEANALYSIS,
	EXPRESSION_COLORSET_DEFINITION,//2020,by george
	EXPRESSION_COLORMARKING//2021,by george
};

class SP_DLG_ExpressionAssistent;

class SP_DLG_ExpressionAssistent_ListCtrl : public wxListCtrl
{

  public:


    SP_DLG_ExpressionAssistent_ListCtrl( SP_DLG_ExpressionAssistent* p_pcDlg,
      wxWindow *parent, const wxWindowID id,
      const wxPoint& pos, const wxSize& size, long style )
    : wxListCtrl( parent, id, pos, size, style )
    {

      m_pcParentDlg = p_pcDlg;

    };

    void OnDblClick( wxListEvent& event );

  private :

    SP_DLG_ExpressionAssistent* m_pcParentDlg;

    DECLARE_EVENT_TABLE()

};

class SP_DLG_ExpressionAssistent : public wxDialog
{

  protected :

    DECLARE_EVENT_TABLE()
   
    wxBoxSizer* m_pcSizer;

    wxString m_sReturnText;
	wxString m_sColorsetName;
	SP_CPN_ColorSetClass* m_pcColorSetClass;  

	wxString m_sPlaceType;

    wxTextCtrl* m_pcFormulaTextCtrl;

	SP_CPN_ExprAssistType m_ExprAssistType;

    SP_DLG_ExpressionAssistent_ListCtrl* m_pcVariableListCtrl;
    SP_DLG_ExpressionAssistent_ListCtrl* m_pcConstantListCtrl;
    SP_DLG_ExpressionAssistent_ListCtrl* m_pcFunctionListCtrl;
    SP_DLG_ExpressionAssistent_ListCtrl* m_pcSimpleCSListCtrl;//by george
	SP_DLG_ExpressionAssistent_ListCtrl* m_pcColorFunctions;//by george

    virtual void LoadData();
    virtual void LoadConstant();
    virtual void LoadVariable();
    virtual void LoadFunctions();
    virtual void LoadSimpleColorSets();//by george
    virtual void LoadColorFuns();//george

    DECLARE_CLASS( SP_DLG_ExpressionAssistent )

  protected :

  public:


    SP_DLG_ExpressionAssistent(SP_CPN_ExprAssistType p_ExprAssistType, wxString p_psColorsetName, wxString p_sPlaceType, SP_CPN_ColorSetClass& p_pcColorSetClass,
      wxString p_psReturnText, wxWindow* p_pcParent, wxString p_sHelpText = wxT(""),
      const wxString& p_sTitle = wxT("Assistent"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );

    void OnDlgCancel( wxCommandEvent& p_cEvent );

    virtual void OnCheckFormula( wxCommandEvent& p_cEvent );

    virtual void InsertIntoFormulaText( const wxString& p_sInsertText );

    virtual const wxString GetReturnText(){ return m_sReturnText; }

};

#endif // __SP_DLG_ExpressionAssistent__

