//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include <wx/panel.h>

#include "sp_defines.h"
#include "sp_gui/widgets/SP_WDG_Notebook.h"

IMPLEMENT_CLASS(SP_WDG_Notebook, wxNotebook)

BEGIN_EVENT_TABLE(SP_WDG_Notebook, wxNotebook)
    EVT_NOTEBOOK_PAGE_CHANGED(SP_WDG_NOTEBOOK_ID, SP_WDG_Notebook::OnPageChanged)
    EVT_NOTEBOOK_PAGE_CHANGING(SP_WDG_NOTEBOOK_ID, SP_WDG_Notebook::OnPageChanging)
END_EVENT_TABLE()

SP_WDG_Notebook::SP_WDG_Notebook(wxWindow* p_pcParent, 
        const wxString& p_sName,
        const wxPoint& p_cPos,
        const wxSize& p_cSize,
        long p_nStyle)
:wxNotebook(p_pcParent, SP_WDG_NOTEBOOK_ID, p_cPos, p_cSize, p_nStyle, p_sName)
{
}

SP_WDG_NotebookPage*
SP_WDG_Notebook::AddOrGetPage(const wxString& p_sName, unsigned int p_nOrdering)
{
    SP_WDG_NotebookPage* l_pcReturn = NULL;
    size_t l_nInsertPos = 0;

	wxString l_sName = p_sName.BeforeFirst(':');
	wxString l_sParent = p_sName.AfterFirst(':');

    for (size_t i = 0; i < GetPageCount() && l_pcReturn == NULL; i++)
    {
    	SP_WDG_NotebookPage* l_pcPage = dynamic_cast<SP_WDG_NotebookPage*>(GetPage(i));
        if (l_pcPage->GetOrdering() <= p_nOrdering)
        {
        	++l_nInsertPos;
        }
        if (GetPageText(i) == l_sName)
        {
        	l_pcReturn = l_pcPage;
        	if(p_nOrdering < l_pcPage->GetOrdering())
        	{
        		l_pcPage->SetOrdering(p_nOrdering);

        		if(l_nInsertPos < i)
        		{
        			RemovePage(i);
        			InsertPage(l_nInsertPos, l_pcPage, l_sName, false);
        		}

        	}
        }
    }

    if (!l_pcReturn)
    {
    	l_pcReturn = new SP_WDG_NotebookPage(this, p_nOrdering);
        
        InsertPage(l_nInsertPos, l_pcReturn, l_sName, false);
        SetSelection(0);
    }
    
    return l_pcReturn;
}

/* EVENTS */
void
SP_WDG_Notebook::OnPageChanged(wxNotebookEvent& p_cEvent)
{
    p_cEvent.Skip();
}

void
SP_WDG_Notebook::OnPageChanging(wxNotebookEvent& p_cEvent)
{
    p_cEvent.Skip();
}


