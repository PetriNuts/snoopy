//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2010/1/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_ColStFunctionList_H__
#define __SP_WDG_ColStFunctionList_H__

#include <wx/wx.h>
#include <wx/grid.h>

#include "sp_gui/widgets/dialogs/SP_WDG_DialogBase.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

class SP_WDG_ColStFunctionList: public SP_WDG_DialogBase
{

  private:

    wxString m_sOldCellValue;

    DECLARE_EVENT_TABLE()

  protected:

    wxGrid* m_pcFunctionGrid;

    SP_DLG_ShapeProperties* m_pcDlg;

    bool LoadData();
    bool SaveData();

   
    void OnAssistent( wxCommandEvent& p_cEvent );

    void OnOverview( wxCommandEvent& p_cEvent );

    void ConnectEvents();
    void DisconnectEvents();


	//special 
	void OnAdd( wxCommandEvent& p_cEvent );
    void OnDelete( wxCommandEvent& p_cEvent ); 
	void OnCheck( wxCommandEvent& p_cEvent );
	void OnCheckPredicate( wxCommandEvent& p_cEvent );
	//

  public:

    SP_WDG_ColStFunctionList( const wxString& p_sPage = wxT("Page 1"), unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max() );
    virtual ~SP_WDG_ColStFunctionList();

    virtual SP_WDG_DialogBase* Clone();

    virtual bool AddToDialog( const SP_ListAttribute* p_ptlAttributes,
      SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview );

    virtual bool OnDlgOk();

    void OnGridCellValueChanged( wxGridEvent& ev );
    void OnGridCellSelected( wxGridEvent& ev );

};

#endif // __SP_WDG_ColStFunctionList_H__
