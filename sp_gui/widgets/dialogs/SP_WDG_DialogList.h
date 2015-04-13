//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/08/11 14:25:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_DialogList_H__
#define __SP_WDG_DialogList_H__

#include "sp_gui/widgets/dialogs/SP_WDG_DialogBase.h"
#include "sp_ds/attributes/SP_DS_ListAttribute.h"
#include <wx/wx.h>

/**	\brief	The SP_WDG_DialogList: class

  Widget to be used for choosing one alternative
*/
class SP_WDG_DialogList: public SP_WDG_DialogBase
{
private:
    DECLARE_EVENT_TABLE()

protected:
    wxListBox* m_pcListBox;
    wxArrayString m_pcListBoxValues;
    wxArrayString FillValues(SP_DS_ListAttribute* p_pcListAttr = NULL);

    SP_DLG_ShapeProperties* m_pcDlg;

    void OnAdd(wxCommandEvent& p_cEvent);
    void OnRemove(wxCommandEvent& p_cEvent);
    void OnEdit(wxCommandEvent& p_cEvent);
    void ConnectEvents();
    void DisconnectEvents();

public:
    SP_WDG_DialogList(const wxString& p_sPage = wxT("Page 1"), unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max());
    virtual ~SP_WDG_DialogList();

    virtual SP_WDG_DialogBase* Clone();

    virtual bool AddToDialog(const SP_ListAttribute* p_ptlAttributes,
        SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview);
    virtual bool OnDlgOk();
};

#endif // __SP_WDG_DialogList_H__
