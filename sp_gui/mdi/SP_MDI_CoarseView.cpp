//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_gui/mdi/SP_MDI_CoarseView.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_core/SP_Core.h"

#include "sp_defines.h"

#include "snoopy.h"

IMPLEMENT_DYNAMIC_CLASS(SP_MDI_CoarseView, SP_MDI_View)

BEGIN_EVENT_TABLE(SP_MDI_CoarseView, SP_MDI_View)
END_EVENT_TABLE()


SP_MDI_CoarseView::SP_MDI_CoarseView()
{
    m_eType = SP_VIEW_COARSE;
    // we don't really close ourself nor our document
    m_bClose = FALSE;
}

SP_MDI_CoarseView::~SP_MDI_CoarseView()
{
}

bool
SP_MDI_CoarseView::OnClose(bool p_bDeleteWindow)
{
	if (SP_Core::Instance()->GetAnimRunning())
        return FALSE;

    if (m_bClose)
        return SP_MDI_View::OnClose(p_bDeleteWindow);

    SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());

    if (!l_pcDoc || !m_pcFrame)
        return FALSE;

    SelectAll(FALSE);
    m_bFrameWasMaximized = FALSE;

    if (m_pcFrame->IsMaximized())
    {
        m_bFrameWasMaximized = TRUE;
        m_pcFrame->Restore();
    }

    m_pcFrame->Hide();
    m_pcFrame->Show(FALSE);

    wxGetApp().GetMainframe()->ActivateNextFrame(m_bFrameWasMaximized);

    return m_bClose;
}


void
SP_MDI_CoarseView::OnActivateView(bool activate, wxView *activeView,
				wxView *deactiveView)
{
	if(activate && this == activeView && this != deactiveView)
	{
		SP_MDI_CoarseDoc* l_pcDoc = dynamic_cast<SP_MDI_CoarseDoc*>(GetDocument());
		if (l_pcDoc && l_pcDoc->GetGraph())
		{
		// calling with show=FALSE, document is already shown
		l_pcDoc->GetGraph()->CreateCoarseTree()->SetCurrentDoc(l_pcDoc, FALSE);
		}
	}
}
