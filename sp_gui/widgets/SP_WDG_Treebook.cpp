//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2013/11/11 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include <wx/panel.h>

#include "sp_utilities.h"

#include "sp_gui/widgets/SP_WDG_Treebook.h"

IMPLEMENT_CLASS(SP_WDG_Treebook, wxTreebook)

BEGIN_EVENT_TABLE(SP_WDG_Treebook, wxTreebook)
    EVT_TREEBOOK_PAGE_CHANGED(SP_WDG_TREEBOOK_ID, SP_WDG_Treebook::OnPageChanged)
    EVT_TREEBOOK_PAGE_CHANGING(SP_WDG_TREEBOOK_ID, SP_WDG_Treebook::OnPageChanging)
END_EVENT_TABLE()

SP_WDG_Treebook::SP_WDG_Treebook(wxWindow* p_pcParent,
        const wxString& p_sName,
        const wxPoint& p_cPos,
        const wxSize& p_cSize,
        long p_nStyle)
:wxTreebook(p_pcParent, SP_WDG_TREEBOOK_ID, p_cPos, p_cSize,
		p_nStyle | wxTR_HIDE_ROOT | wxTR_NO_LINES | wxTR_FULL_ROW_HIGHLIGHT,
		p_sName)
{
}

// create subpages by using the following pattern "child:parent"
SP_WDG_NotebookPage*
SP_WDG_Treebook::AddOrGetPage(const wxString& p_sName, unsigned int p_nOrdering)
{
    SP_WDG_NotebookPage* l_pcReturn = NULL;
    SP_WDG_NotebookPage* l_pcParent = NULL;
    size_t l_nParentPos = wxNOT_FOUND;

	wxString l_sName = p_sName.BeforeFirst(':');
	wxString l_sParent = p_sName.AfterFirst(':');

	if(!l_sParent.IsEmpty())
    {
		for (size_t i = 0; i < GetPageCount() && l_pcParent == NULL; i++)
		{
			if(GetPageText(i) == l_sParent && GetPageParent(i) == wxNOT_FOUND)
			{
				l_nParentPos = i;
				l_pcParent = dynamic_cast<SP_WDG_NotebookPage*>(GetPage(i));
			}
		}
		if(l_nParentPos == wxNOT_FOUND)
		{
			l_nParentPos = GetPageCount();
			l_pcParent = new SP_WDG_NotebookPage(this, p_nOrdering);
			AddPage(l_pcParent, l_sParent);
		}

    }

	for (size_t i = 0; i < GetPageCount() && l_pcReturn == NULL; i++)
	{
		SP_WDG_NotebookPage* l_pcPage = dynamic_cast<SP_WDG_NotebookPage*>(GetPage(i));
		if (GetPageText(i) == l_sName && GetPageParent(i) == l_nParentPos)
		{
			l_pcReturn = l_pcPage;
		}
	}

	if (!l_pcReturn)
	{
		l_pcReturn = new SP_WDG_NotebookPage(this, p_nOrdering);

		if(l_nParentPos == wxNOT_FOUND)
		{
			AddPage(l_pcReturn, l_sName, false);
		}
		else
		{
			InsertSubPage(l_nParentPos, l_pcReturn, l_sName, false);
			ExpandNode(l_nParentPos, true);
		}
		SetSelection(1);
	}

    return l_pcReturn;
}

/* EVENTS */
void
SP_WDG_Treebook::OnPageChanged(wxTreebookEvent& p_cEvent)
{
    p_cEvent.Skip();
}

void
SP_WDG_Treebook::OnPageChanging(wxTreebookEvent& p_cEvent)
{
    p_cEvent.Skip();
}


