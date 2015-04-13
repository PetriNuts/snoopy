//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_TOOLBAR_H__
#define __SP_WDG_TOOLBAR_H__

#include "sp_gui/windows/SP_GUI_Mainframe.h"



class SP_WDG_Toolbar: public wxToolBar
{
private:
	SP_GUI_Mainframe* m_pcMainframe;
protected:
public:
    SP_WDG_Toolbar(
        SP_GUI_Mainframe* p_pcParent,
        const wxWindowID p_nId,
        const wxPoint& p_cPos = wxDefaultPosition,
        const wxSize& p_cSize = wxDefaultSize,
        const long p_nStyle = wxNO_BORDER,
        const wxString& p_sName = wxToolBarNameStr);
};

#endif // __SP_WDG_TOOLBAR_H__


