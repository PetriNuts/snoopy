//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/06/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_StDummyMarking_H__
#define __SP_WDG_StDummyMarking_H__


#include <wx/wx.h>
#include <wx/grid.h>
#include "sp_gui/widgets/dialogs/SP_WDG_DialogBase.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

class SP_WDG_StDummyMarking : public SP_WDG_DialogBase
{

  private:

    DECLARE_EVENT_TABLE()

  protected:


    SP_DLG_ShapeProperties* m_pcDlg;

  public:

    SP_WDG_StDummyMarking( const wxString& p_sPage = wxT("Page 1"), unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max() );
    virtual ~SP_WDG_StDummyMarking();

    virtual SP_WDG_DialogBase* Clone();

    virtual bool AddToDialog( const SP_ListAttribute* p_ptlAttributes,
      SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview );

    virtual bool OnDlgOk();

};

#endif // __SP_WDG_StDummyMarking_H__
