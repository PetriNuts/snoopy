//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_ArcInscription_H__
#define __SP_WDG_ArcInscription_H__

#include <wx/wx.h>
#include <wx/grid.h>

#include "sp_gui/widgets/dialogs/SP_WDG_DialogBase.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

class SP_WDG_ArcInscription: public SP_WDG_DialogBase
{

  private:

    wxString m_sOldCellValue;

	SP_CPN_ColorSetClass m_cColorSetClass;

	wxString m_sNodeClass;

    DECLARE_EVENT_TABLE()

  protected:

    wxGrid* m_pcInscriptionGrid;


    SP_DLG_ShapeProperties* m_pcDlg;

    bool LoadData();
    bool SaveData();

    void OnEdit( wxCommandEvent& p_cEvent );
    void OnOverview( wxCommandEvent& p_cEvent );

    void ConnectEvents();
    void DisconnectEvents();

  public:

    SP_WDG_ArcInscription( const wxString& p_sPage = wxT("Page 1"), unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max() );
    virtual ~SP_WDG_ArcInscription();

    virtual SP_WDG_DialogBase* Clone();

    virtual bool AddToDialog(const SP_ListAttribute* p_ptlAttributes,
      SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview );

    virtual bool OnDlgOk();

    void OnGridCellValueChanged( wxGridEvent& p_gEvent );
    void OnGridCellSelected( wxGridEvent& ev );
	void OnAssistent( wxCommandEvent& p_cEvent );
	void OnCheck( wxCommandEvent& p_cEvent );
};

#endif // __SP_WDG_ArcInscription_H__
