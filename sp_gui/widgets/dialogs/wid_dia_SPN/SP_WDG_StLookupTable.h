//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/21 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_StLookupTable_H__
#define __SP_WDG_StLookupTable_H__

#include <wx/wx.h>
#include <wx/grid.h>

#include "sp_gui/widgets/dialogs/SP_WDG_DialogBase.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"


class SP_WDG_StLookupTable: public SP_WDG_DialogBase
{

  private:

    wxString m_sOldCellValue;

    DECLARE_EVENT_TABLE()

  protected:

    SP_DS_ColListAttribute* m_pcTempLookupTableList;

    wxGrid* m_pcLookupTableGrid;
    wxTextCtrl* m_pcDefaultValueTextCtrl;

    SP_DLG_ShapeProperties* m_pcDlg;

    bool LoadData();
    bool SaveData();

    void OnEditRow( wxCommandEvent& p_cEvent );
    void OnNewRow( wxCommandEvent& p_cEvent );
    void OnDeleteRow( wxCommandEvent& p_cEvent );

    void ConnectEvents();
    void DisconnectEvents();

    bool CheckFunctionProperty( int p_nRowNumber, double p_nToCheck );
    double GetGreatestFunctionArgument();

  public:

    SP_WDG_StLookupTable( const wxString& p_sPage = wxT("Page 1"), unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max() );
    virtual ~SP_WDG_StLookupTable();

    virtual SP_WDG_DialogBase* Clone();

    virtual bool AddToDialog( const SP_ListAttribute* p_ptlAttributes,
      SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview );

    virtual bool OnDlgOk();

    void OnGridCellValueChanged( wxGridEvent& ev );
    void OnGridCellSelected( wxGridEvent& ev );

};

#endif // __SP_WDG_StLookupTable_H__
