//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2013/11/11 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_TREEBOOK_H__
#define __SP_WDG_TREEBOOK_H__

#include <wx/treebook.h>
#include "sp_gui/widgets/SP_WDG_NotebookPage.h"

class SP_WDG_Treebook: public wxTreebook
{
private:
    DECLARE_CLASS(SP_WDG_Treebook)
    DECLARE_EVENT_TABLE()
protected:
public:
    SP_WDG_Treebook(wxWindow* p_pcParent,
        const wxString& p_sName = wxT("notebook"),
        const wxPoint& p_cPos = wxDefaultPosition,
        const wxSize& p_cSize = wxDefaultSize,
        long p_nStyle = 0);

    // create subpages by using the following pattern "child:parent"
    SP_WDG_NotebookPage* AddOrGetPage(const wxString& p_sName, unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max());

    /* EVENTS */
    void OnPageChanged(wxTreebookEvent& p_cEvent);
    void OnPageChanging(wxTreebookEvent& p_cEvent);
};

#endif // __SP_WDG_TREEBOOK_H__
