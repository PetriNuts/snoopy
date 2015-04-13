//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GUI_DEVBARCONTAINER_H__
#define __SP_GUI_DEVBARCONTAINER_H__

#include <wx/wx.h>

#include "sp_core/SP_Set.h"

#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/widgets/SP_WDG_Treectrl.h"

class SP_GUI_DevbarContainer: public wxPanel
{
private:
    DECLARE_EVENT_TABLE()

    SP_GUI_Mainframe* m_pcMainframe;
    set<SP_WDG_Treectrl*> m_tsTrees;
    SP_WDG_Treectrl* m_pcActTree;

    wxBoxSizer* m_pcSizer;
public:
    SP_GUI_DevbarContainer(
        SP_GUI_Mainframe* parent, 
        int pId = -1, 
        wxPoint pPosition = wxDefaultPosition, 
        wxSize pSize = wxDefaultSize);

    bool AddTree(SP_WDG_Treectrl* p_pcVal);
    bool ShowTree(SP_WDG_Treectrl* p_pcVal);
    bool RemoveTree(SP_WDG_Treectrl* p_pcVal);

    /* EVENTS */
    void OnChar(wxKeyEvent& p_cEvent);
    void OnSize(wxSizeEvent& p_cEvent);
};

#endif // __SP_GUI_DEVBARCONTAINER_H__


