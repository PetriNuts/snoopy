//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_gui/windows/SP_GUI_DevbarContainer.h"

#include "sp_defines.h"

BEGIN_EVENT_TABLE(SP_GUI_DevbarContainer, wxPanel)
EVT_SIZE(SP_GUI_DevbarContainer::OnSize)
END_EVENT_TABLE()
SP_GUI_DevbarContainer::SP_GUI_DevbarContainer(SP_GUI_Mainframe* p_pcParent,
                                     int p_nId,
                                     wxPoint p_cPos,
                                     wxSize p_cSize)
:wxPanel(p_pcParent, p_nId, p_cPos, p_cSize, wxNO_BORDER | wxSW_3D | wxNO_FULL_REPAINT_ON_RESIZE ),
m_pcActTree(NULL)
{
    m_pcMainframe = p_pcParent;

    m_pcSizer = new wxBoxSizer(wxVERTICAL);
	m_pcSizer->SetMinSize(300,300);
    SetSizerAndFit(m_pcSizer);
}

bool
SP_GUI_DevbarContainer::AddTree(SP_WDG_Treectrl* p_pcVal)
{
    if (m_tsTrees.find(p_pcVal) == m_tsTrees.end())
    {
        m_tsTrees.insert(p_pcVal);
        m_pcSizer->Add(p_pcVal, 1, wxRIGHT | wxEXPAND, 2);
        m_pcSizer->Show(p_pcVal, FALSE);
    }

    ShowTree(p_pcVal);

    m_pcSizer->Layout();

    return TRUE;
}

bool
SP_GUI_DevbarContainer::RemoveTree(SP_WDG_Treectrl* p_pcVal)
{
    if (m_tsTrees.find(p_pcVal) != m_tsTrees.end())
    {
        m_tsTrees.erase(p_pcVal);
        m_pcSizer->Show(p_pcVal, FALSE);
        m_pcSizer->Detach(p_pcVal);
		m_pcSizer->Layout();
	if (m_pcActTree == p_pcVal)
	  m_pcActTree = NULL;
        return p_pcVal->Destroy();
    }
    return FALSE;
}

bool
SP_GUI_DevbarContainer::ShowTree(SP_WDG_Treectrl* p_pcVal)
{
    wxASSERT_MSG(m_tsTrees.find(p_pcVal) != m_tsTrees.end(), wxT("Tree not in Treeset!"));
    // already showing the right one, nothing to do
    if (m_pcActTree == p_pcVal)
        return TRUE;

    set<SP_WDG_Treectrl*>::iterator l_itTrees;
    for (l_itTrees = m_tsTrees.begin(); l_itTrees != m_tsTrees.end(); ++l_itTrees)
    {
        m_pcSizer->Show((*l_itTrees), FALSE);
    }
    m_pcSizer->Show(p_pcVal, TRUE);
    m_pcSizer->Layout();
    m_pcActTree = p_pcVal;

    return TRUE;
}

void
SP_GUI_DevbarContainer::OnChar(wxKeyEvent& p_cEvent)
{
    p_cEvent.Skip();
}

void
SP_GUI_DevbarContainer::OnSize(wxSizeEvent& p_cEvent)
{
#if wxABI_VERSION > 30000
	p_cEvent.Skip();
#else
    wxPanel::OnSize(p_cEvent);
#endif
    m_pcSizer->Layout();
    Layout();
}
