//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_DIALOGNUMBER_H__
#define __SP_WDG_DIALOGNUMBER_H__

#include <wx/wx.h>
#include <wx/spinctrl.h>

#include "sp_gui/widgets/dialogs/SP_WDG_DialogBase.h"

/**	\brief	The SP_WDG_DialogNumber: class

  Widget to be used for editing numeric attributes,
  but not floating points
*/
class SP_WDG_DialogNumber: public SP_WDG_DialogBase
{
private:
    std::vector<wxSpinCtrl*> m_pcSpinCtrl;
protected:
    int m_nMin, m_nMax;
public:
    SP_WDG_DialogNumber(const wxString& p_sPage = wxT("Page 1"), int p_nMin = INT_MIN, int p_nMax = INT_MAX, unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max());
    virtual ~SP_WDG_DialogNumber();

    virtual SP_WDG_DialogBase* Clone();

    virtual bool AddToDialog(const SP_ListAttribute* p_ptlAttributes,
        SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview);

    virtual bool OnDlgOk();
};

#endif // __SP_WDG_DIALOGNUMBER_H__
