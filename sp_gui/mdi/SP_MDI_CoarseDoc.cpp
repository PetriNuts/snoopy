//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_gui/mdi/SP_MDI_CoarseView.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"

#include "sp_core/SP_Core.h"

#include "sp_defines.h"

#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
IMPLEMENT_DYNAMIC_CLASS(SP_MDI_CoarseDoc, SP_MDI_Doc)

SP_MDI_CoarseDoc::SP_MDI_CoarseDoc()
{
    m_eType = SP_DOC_COARSE;
    m_bClose = FALSE;
    m_pcParentDoc = SP_Core::Instance()->GetRootDocument();
    if (m_pcParentDoc)
    {
        m_pcParentDoc->AddChildDoc(this);
        UpdateTitle();
    }
    Modify(TRUE);
}

SP_MDI_CoarseDoc::~SP_MDI_CoarseDoc()
{
    // the d'tor of the base class is called right
    // after I leave my own here, so we avoid
    // deleting the m_pcGraph member just by
    // setting it to NULL, as we never had one.
    m_pcGraph = NULL;

    if (m_pcParentDoc)
        m_pcParentDoc->RemoveChildDoc(this);
}

SP_DS_Graph*
SP_MDI_CoarseDoc::SetGraph(SP_DS_Graph* p_pcVal)
{
    if (m_pcGraph)
        return m_pcGraph;

    m_pcGraph = p_pcVal;
    m_pcCoarseTreectrl = m_pcGraph->CreateCoarseTree();
    SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(GetFirstView());
    if (l_pcView && l_pcView->GetFrame())
    {
        l_pcView->GetFrame()->SetCoarseTreectrl(m_pcCoarseTreectrl);
        l_pcView->GetFrame()->AddGraphToMenu(m_pcGraph);
    }

    if (m_pcGraphTreectrl)
        m_pcGraphTreectrl->Initialise(m_pcGraph);

    return m_pcGraph;
}

void
SP_MDI_CoarseDoc::SetNameSuffix(wxString& p_sVal)
{
    m_sNameSuffix = p_sVal;

    UpdateTitle();
}

void
SP_MDI_CoarseDoc::UpdateTitle()
{
    if (!m_pcParentDoc)
        return;

    wxString l_sTitle;
#if wxABI_VERSION > 30000
    l_sTitle = m_pcParentDoc->GetUserReadableName();
#else
    m_pcParentDoc->GetPrintableName(l_sTitle);
#endif
    if (!m_sNameSuffix.IsEmpty())
        l_sTitle << wxT(": ") << m_sNameSuffix;

    SetTitle(l_sTitle);

    SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(GetFirstView());
    if (l_pcView && l_pcView->GetFrame())
    {
        l_pcView->GetFrame()->SetTitle(l_sTitle);
    }
}

void
SP_MDI_CoarseDoc::Modify(bool p_bVal)
{
    if (m_documentModified != p_bVal)
    {
        m_documentModified = p_bVal;
        m_pcParentDoc->DoSetModify(p_bVal, this);
    }
}

bool
SP_MDI_CoarseDoc::OnCloseDocument()
{
	if (m_bClose)
        return TRUE;

    SP_MDI_CoarseView* l_pcView = dynamic_cast<SP_MDI_CoarseView*>(GetFirstView());

    // reached from the wxT("close") entry in the file menu
    if (l_pcView)
    {
			return l_pcView->OnClose(FALSE);
    }

    return FALSE;
}

bool
SP_MDI_CoarseDoc::OnSaveDocument(const wxString& p_sFile)
{
    if (m_pcParentDoc)
        return m_pcParentDoc->OnSaveDocument(p_sFile);
    return FALSE;
}

bool
SP_MDI_CoarseDoc::SaveAs()
{
    if (m_pcParentDoc)
        return m_pcParentDoc->SaveAs();
    return FALSE;
}
