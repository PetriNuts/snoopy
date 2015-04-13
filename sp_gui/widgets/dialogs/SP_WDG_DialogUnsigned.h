//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: alextov $
// $Version: 0.0 $
// $Date: 2005/06/03 $
// Short Description: dialog for unsigned integers
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_DIALOGUNSIGNED_H__
#define __SP_WDG_DIALOGUNSIGNED_H__

#include <wx/wx.h>
#include <wx/spinctrl.h>

#include "sp_gui/widgets/dialogs/SP_WDG_DialogBase.h"

/**	\brief	The SP_WDG_DialogUnsigned: class

  Widget to be used for editing unsigned int numeric attributes,
*/
class SP_WDG_DialogUnsigned: public SP_WDG_DialogBase
{
private:
    std::vector<wxSpinCtrl*> m_pcSpinCtrl;
protected:
    unsigned m_nMin, m_nMax;
public:
    SP_WDG_DialogUnsigned(const wxString& p_sPage = wxT("Page 1"),
			  unsigned p_nMin = 0, unsigned p_nMax = INT_MAX,
			  unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max());
    virtual ~SP_WDG_DialogUnsigned();

    virtual SP_WDG_DialogBase* Clone();

    virtual bool AddToDialog(const SP_ListAttribute* p_ptlAttributes,
        SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview);

    virtual bool OnDlgOk();
};

#endif // __SP_WDG_DIALOGUNSIGNED_H__
