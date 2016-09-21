#include "sp_utilities.h"

#include "sp_gui/widgets/SP_WDG_NotebookPage.h"
#include "sp_gui/widgets/SP_WDG_Notebook.h"

IMPLEMENT_CLASS(SP_WDG_NotebookPage, wxScrolledWindow)

BEGIN_EVENT_TABLE(SP_WDG_NotebookPage, wxScrolledWindow)
END_EVENT_TABLE()

SP_WDG_NotebookPage::SP_WDG_NotebookPage(wxWindow* p_pcParent, unsigned int p_nOrdering)
:wxScrolledWindow(p_pcParent), m_nOrdering(p_nOrdering)
{
    // all controls are added from top to bottom
    m_pcTopSizer = new wxBoxSizer(wxVERTICAL);
    SetScrollRate(10,10);
    SetAutoLayout(true);
    SetSizer(m_pcTopSizer);

#if wxABI_VERSION < 30000
    m_pcTopSizer->SetVirtualSizeHints(this);
#endif
}

bool
SP_WDG_NotebookPage::AddControl(wxSizer* p_pcSizer,
                                int p_nOpt,
                                int p_nFlag,
                                int p_nBorder)
{
    if (!p_pcSizer || !m_pcTopSizer)
        return false;

    m_pcTopSizer->Add(p_pcSizer, p_nOpt, p_nFlag, p_nBorder);
    SetSizer(m_pcTopSizer);

#if wxABI_VERSION < 30000
    m_pcTopSizer->SetVirtualSizeHints(this);
#endif

    m_pcTopSizer->Layout();
    return true;
}

bool
SP_WDG_NotebookPage::SetOrdering(unsigned int p_nOrdering)
{
	m_nOrdering = p_nOrdering;
	return true;
}

