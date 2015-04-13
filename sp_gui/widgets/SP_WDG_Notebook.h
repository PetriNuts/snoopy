//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_NOTEBOOK_H__
#define __SP_WDG_NOTEBOOK_H__

#include <wx/notebook.h>
#include "sp_gui/widgets/SP_WDG_NotebookPage.h"

class SP_WDG_Notebook: public wxNotebook
{
private:
    DECLARE_CLASS(SP_WDG_Notebook)
    DECLARE_EVENT_TABLE()
protected:
public:
    SP_WDG_Notebook(wxWindow* p_pcParent, 
        const wxString& p_sName = wxT("notebook"),
        const wxPoint& p_cPos = wxDefaultPosition,
        const wxSize& p_cSize = wxDefaultSize,
        long p_nStyle = 0);

    SP_WDG_NotebookPage* AddOrGetPage(const wxString& p_sName, unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max());

    /* EVENTS */
    void OnPageChanged(wxNotebookEvent& p_cEvent);
    void OnPageChanging(wxNotebookEvent& p_cEvent);
};

#endif // __SP_WDG_NOTEBOOK_H__
