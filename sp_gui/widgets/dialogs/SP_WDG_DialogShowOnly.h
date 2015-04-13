//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_DIALOGSHOWONLY_H__
#define __SP_WDG_DIALOGSHOWONLY_H__

#include <wx/wx.h>

#include "sp_gui/widgets/dialogs/SP_WDG_DialogBase.h"

/**	\brief	The SP_WDG_DialogShowOnly: class

  Widget to be used for attributes that only be shown or hidden
*/
class SP_WDG_DialogShowOnly: public SP_WDG_DialogBase
{
private:
protected:
public:
    SP_WDG_DialogShowOnly(const wxString& p_sPage = wxT("Page 1"), unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max());
    virtual ~SP_WDG_DialogShowOnly();

    virtual SP_WDG_DialogBase* Clone();

    virtual bool AddToDialog(const SP_ListAttribute* p_ptlAttributes,
        SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview);
};

#endif // __SP_WDG_DIALOGTEXT_H__
