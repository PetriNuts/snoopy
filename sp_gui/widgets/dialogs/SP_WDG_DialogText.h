//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_DIALOGTEXT_H__
#define __SP_WDG_DIALOGTEXT_H__

#include <wx/wx.h>

#include "sp_gui/widgets/dialogs/SP_WDG_DialogBase.h"

/**	\brief	The SP_WDG_DialogText: class

  Widget to be used for editing text attributes
*/
class SP_WDG_DialogText: public SP_WDG_DialogBase
{
private:
protected:
    std::vector<wxTextCtrl*> m_pcTextCtrl;
public:
    SP_WDG_DialogText(const wxString& p_sPage = wxT("Page 1"), unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max());
    virtual ~SP_WDG_DialogText();

    virtual SP_WDG_DialogBase* Clone();

    virtual bool AddToDialog(const SP_ListAttribute* p_ptlAttributes, 
        SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview);

    virtual bool OnDlgOk();

	wxTextCtrl* GetTextCtrl(){ return m_pcTextCtrl.front();}
};

#endif // __SP_WDG_DIALOGTEXT_H__
