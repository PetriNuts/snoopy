//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_ColExtMarkingList_H__
#define __SP_WDG_ColExtMarkingList_H__

#include <wx/wx.h>
#include <wx/grid.h>

#include "sp_gui/widgets/dialogs/SP_WDG_DialogBase.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

class SP_WDG_ColExtMarkingList: public SP_WDG_DialogBase
{

  private:

    wxString m_sOldCellValue;

    DECLARE_EVENT_TABLE()

  protected:

    wxGrid* m_pcMarkingGrid;
	wxTextCtrl* m_pcColorSetTextCtrl;
	SP_DS_TextAttribute* m_pcNameAttibute;


    SP_DLG_ShapeProperties* m_pcDlg;

    bool LoadData();
    bool SaveData();

    void OnAdd( wxCommandEvent& p_cEvent );
    void OnImport( wxCommandEvent& p_cEvent );
    void OnDelete( wxCommandEvent& p_cEvent );
	void OnCheck( wxCommandEvent& p_cEvent );
	void OnAssistant(wxCommandEvent& p_cEvent);//by george

    void ConnectEvents();
    void DisconnectEvents();


  public:

    SP_WDG_ColExtMarkingList( const wxString& p_sPage = wxT("Page 1"), unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max() );
    virtual ~SP_WDG_ColExtMarkingList();

    virtual SP_WDG_DialogBase* Clone();

    virtual bool AddToDialog(const SP_ListAttribute* p_ptlAttributes,
      SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview);

    virtual bool OnDlgOk();

    void OnGridCellValueChanged( wxGridEvent& ev );
    void OnGridCellSelected( wxGridEvent& ev );
	void OnTextValueChanged( wxCommandEvent& ev );
	
	wxGrid* GetColorGrid();
	SP_DS_TextAttribute* GetNameAttibute();
	void UpdatePlaceColor();

};

#endif // __SP_WDG_ColExtMarkingList_H__
