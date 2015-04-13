//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: crohr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2014/04/17 14:25:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_DialogList_H__
#define __SP_WDG_DialogList_H__

#include "sp_gui/widgets/dialogs/SP_WDG_DialogBase.h"
#include <wx/wx.h>

/**	\brief	The SP_WDG_DialogChoice: class

  Widget to be used for choosing one alternative
*/
class SP_WDG_DialogChoice: public SP_WDG_DialogBase
{
private:
    DECLARE_EVENT_TABLE()

protected:
    std::vector<wxChoice*> m_pcChoice;
    wxArrayString m_pcChoiceValues;

    SP_DLG_ShapeProperties* m_pcDlg;
    bool m_bFixedValues;

    void LoadData();

public:
    SP_WDG_DialogChoice(const wxString& p_sPage = wxT("Page 1"), unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max());
    SP_WDG_DialogChoice(const wxString& p_sPage, const wxArrayString& p_sValues, unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max());
    virtual ~SP_WDG_DialogChoice();

    virtual SP_WDG_DialogBase* Clone();

    virtual bool AddToDialog(const SP_ListAttribute* p_ptlAttributes,
        SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview);
    virtual bool OnDlgOk();
};

#endif // __SP_WDG_DialogList_H__
