//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gui/mdi/SP_MDI_CoarseView.h"
#include "sp_core/SP_Core.h"

#include "sp_ds/SP_DS_Netclass.h"

#include "sp_core/SP_Signer.h"

#include "snoopy.h"

IMPLEMENT_DYNAMIC_CLASS(SP_GM_Docmanager, wxDocManager)

BEGIN_EVENT_TABLE(SP_GM_Docmanager, wxDocManager)
EVT_MENU(wxID_SAVE, SP_GM_Docmanager::OnFileSave)
EVT_MENU(SP_MENU_ITEM_SAVE_SIGN, SP_GM_Docmanager::OnSaveSign)
EVT_MENU(wxID_CLOSE, SP_GM_Docmanager::OnFileClose)
EVT_MENU(wxID_PAGE_SETUP, SP_GM_Docmanager::OnPageSetup)
EVT_MENU(wxID_PRINT, SP_GM_Docmanager::OnPrint)
EVT_MENU(wxID_PREVIEW, SP_GM_Docmanager::OnPreview)
EVT_UPDATE_UI(wxID_OPEN, SP_GM_Docmanager::OnUpdateFileOpen)
EVT_UPDATE_UI(wxID_CLOSE, SP_GM_Docmanager::OnUpdateFileClose)
EVT_UPDATE_UI(wxID_CLOSE_ALL, SP_GM_Docmanager::OnUpdateFileClose)
EVT_UPDATE_UI(wxID_REVERT, SP_GM_Docmanager::OnUpdateFileRevert)
EVT_UPDATE_UI(wxID_NEW, SP_GM_Docmanager::OnUpdateFileNew)
EVT_UPDATE_UI(wxID_SAVE, SP_GM_Docmanager::OnUpdateFileSave)
EVT_UPDATE_UI(wxID_SAVEAS, SP_GM_Docmanager::OnUpdateFileSaveAs)
EVT_UPDATE_UI(SP_MENU_ITEM_SAVE_SIGN, SP_GM_Docmanager::OnUpdateFileSaveAs)
END_EVENT_TABLE()

SP_GM_Docmanager::SP_GM_Docmanager(long p_nFlags, bool p_bInitialize) :
	wxDocManager(p_nFlags, p_bInitialize)
{
	m_pcPrintData = new wxPrintData();
	m_pcPageSetupData = new wxPageSetupDialogData();
	(*m_pcPageSetupData) = *m_pcPrintData;
}

SP_GM_Docmanager::~SP_GM_Docmanager()
{
	wxDELETE(m_pcPrintData);
	wxDELETE(m_pcPageSetupData);
}

void SP_GM_Docmanager::OnFileClose(wxCommandEvent& p_cEvent)
{
	wxDocument *l_pcDoc = GetCurrentDocument();
	if (!l_pcDoc)
		return;
	if (l_pcDoc->IsKindOf(CLASSINFO(SP_MDI_CoarseDoc)))
	{
		SP_LOGDEBUG(wxT("COARSE_DOC..."));
		SP_MDI_CoarseView* l_pcView =
				dynamic_cast<SP_MDI_CoarseView*>(l_pcDoc->GetFirstView());
		if (l_pcView && l_pcView->GetFrame())
		{
			l_pcView->Close(FALSE);
		}
		return;
	}
	wxDocManager::OnFileClose(p_cEvent);
}

void SP_GM_Docmanager::OnPrint(wxCommandEvent& p_cEvent)
{
#if wxUSE_PRINTING_ARCHITECTURE
    wxView *view = GetCurrentView();
    if (!view)
        return;

    wxPrintout *printout = view->OnCreatePrintout();
    if (printout)
    {
    	wxPrintDialogData l_cPrintDialogData(*m_pcPrintData);
        wxPrinter printer(&l_cPrintDialogData);
        printer.Print(view->GetFrame(), printout, true);

        delete printout;
    }
#endif // wxUSE_PRINTING_ARCHITECTURE
}

void SP_GM_Docmanager::OnPreview(wxCommandEvent& p_cEvent)
{
#if wxUSE_PRINTING_ARCHITECTURE
    wxView *view = GetCurrentView();
    if (!view)
        return;

    wxPrintout *printout = view->OnCreatePrintout();
    if (printout)
    {
    	wxPrintDialogData l_cPrintDialogData(*m_pcPrintData);
        // Pass two printout objects: for preview, and possible printing.
        wxPrintPreviewBase *preview = new wxPrintPreview(printout, view->OnCreatePrintout(), &l_cPrintDialogData);
        if ( !preview->Ok() )
        {
            delete preview;
            wxMessageBox( _("Sorry, print preview needs a printer to be installed.") );
            return;
        }

        wxPreviewFrame *frame = new wxPreviewFrame(preview, (wxFrame *)wxTheApp->GetTopWindow(), _("Print Preview"),
                wxPoint(100, 100), wxSize(600, 650));
        frame->Centre(wxBOTH);
        frame->Initialize();
        frame->Show(true);
    }
#endif // wxUSE_PRINTING_ARCHITECTURE
}

void SP_GM_Docmanager::OnPageSetup(wxCommandEvent& p_cEvent)
{
#if wxUSE_PRINTING_ARCHITECTURE
	(*m_pcPageSetupData) = *m_pcPrintData;

    wxPageSetupDialog l_cPageSetupDialog(GetCurrentView()->GetFrame(), m_pcPageSetupData);
    l_cPageSetupDialog.ShowModal();

    (*m_pcPrintData) = l_cPageSetupDialog.GetPageSetupDialogData().GetPrintData();
    (*m_pcPageSetupData) = l_cPageSetupDialog.GetPageSetupDialogData();
#endif // wxUSE_PRINTING_ARCHITECTURE

}

wxDocument* SP_GM_Docmanager::CreateDocument(const wxString& p_sPath,
		long p_nFlags)
{
	//snoopy crashes in interaction mode when the same document is open twice
	//work around:
	//since opening a document twice doesn't seem to be important (for now), it's just prevented
	wxNode *l_pcNode = m_docs.GetFirst();
	wxDocument *l_pcDoc;

	while (l_pcNode)
	{
		l_pcDoc = (wxDocument*)l_pcNode->GetData();
		if (p_sPath.CmpNoCase(l_pcDoc->GetFilename()) == 0)
		{
			SP_MESSAGEBOX(wxT("File already opened."), wxT("Message"),
					wxOK | wxICON_EXCLAMATION);
			return 0;
		}
		l_pcNode = l_pcNode->GetNext();
	}
	//---------------------------------------------------------------------------

	if (SP_Core::Instance()->GetAnimMode())
	{
		int result = SP_MESSAGEBOX(
				wxT("Currently running animation will be terminated!?"), wxT("Warning"),
				wxOK | wxCANCEL | wxICON_EXCLAMATION);

		if (result == wxCANCEL)
		{
			return NULL;
		}
	}

	if(SP_Core::Instance()->GetSimulationMode())
	{
		SP_MESSAGEBOX(wxT("Currently running simulation!?"), wxT("Warning"), wxOK | wxICON_EXCLAMATION);
		return NULL;
	}

	l_pcDoc = wxDocManager::CreateDocument(p_sPath, p_nFlags);

	// p_nFlags == 0 means, we pass by from SP_MDI_Doc::OnOpenDocument
	// so, the document is already initialised
	if (l_pcDoc && (p_nFlags != 0))
	{
		SP_GM_DocTemplate
				* l_pcDocTemplate =
						dynamic_cast<SP_GM_DocTemplate*>(l_pcDoc->GetDocumentTemplate());

		if (l_pcDocTemplate)
		{
			SP_DS_Netclass* l_pcNS = l_pcDocTemplate->GetNetclass();
			SP_MDI_Doc* l_pcMyDoc = dynamic_cast<SP_MDI_Doc*>(l_pcDoc);
			l_pcMyDoc->InitGraph(l_pcNS);
		}
	}

	//  in wxWindows 2.6 document is not shown?
	if (l_pcDoc)
	{
		dynamic_cast<SP_MDI_Doc*>(l_pcDoc)->ShowAll();
	}

	return l_pcDoc;
}

bool SP_GM_Docmanager::CloseDocuments(bool p_bForce)
{
	wxNode *l_pcNode = m_docs.GetFirst();
	while (l_pcNode)
	{
		wxDocument *l_pcDoc = (wxDocument *)l_pcNode->GetData();

		if (!l_pcDoc->Close() && !p_bForce)
			return FALSE;

		// Implicitly deletes the document when the last
		// view is removed (deleted)
		l_pcDoc->DeleteAllViews();

		// Check document is deleted
		if (m_docs.Member(l_pcDoc))
			wxDELETE(l_pcDoc);

		// This assumes that documents are not connected in
		// any way, i.e. deleting one document does NOT
		// delete another.
		l_pcNode = m_docs.GetFirst();
	}
	return TRUE;
}

bool SP_GM_Docmanager::Clear(bool p_bForce)
{
	if (!CloseDocuments(p_bForce))
		return FALSE;

	wxNode *l_pcNode = m_templates.GetFirst();
	while (l_pcNode)
	{
		wxDocTemplate *l_pcTempl = (wxDocTemplate*) l_pcNode->GetData();
		wxNode* l_pcNext = l_pcNode->GetNext();
		wxDELETE(l_pcTempl);
		l_pcNode = l_pcNext;
	}
	return TRUE;
}

void SP_GM_Docmanager::OnFileSave(wxCommandEvent& p_cEvent)
{
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetCurrentDocument());
	if (!l_pcDoc)
		return;

	while (l_pcDoc->GetType() == SP_DOC_COARSE)
		l_pcDoc = l_pcDoc->GetParentDoc();

	// hack: but the chooseinvariant dialog cant get the wxid_save-event,
	// so we have to do reset from here
	l_pcDoc->ResetChooseInvariantColours();

	l_pcDoc->Save();
}

void SP_GM_Docmanager::OnSaveSign(wxCommandEvent& p_cEvent)
{

	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetCurrentDocument());
	if (!l_pcDoc)
		return;

	while (l_pcDoc->GetType() == SP_DOC_COARSE)
		l_pcDoc = l_pcDoc->GetParentDoc();

	if (l_pcDoc->SaveAs())
	{
		SP_Signer * sgn = NULL;
		try
		{
			sgn = new SP_Signer(l_pcDoc->GetFilename());
			sgn->updateSignatureFile();
			SP_MESSAGEBOX(wxT("The graph has been successfully signed.\n")
					wxT("The signature file has been written to the file's directory."), wxT("Message..."));
		}
		catch(const wxString& ex)
		{
			SP_MESSAGEBOX(ex, wxT("Error..."), wxOK | wxICON_EXCLAMATION);
		}
		wxDELETE(sgn);
	}
}

bool SP_GM_Docmanager::HasTemplate(const wxString& p_pchName)
{
	if (!p_pchName)
		return FALSE;

	//    SP_GM_DocTemplate* l_pcShow = NULL;
	for (size_t i = 0; i < m_templates.GetCount(); i++)
	{
		wxDocTemplate *l_pcTemp =
				dynamic_cast<wxDocTemplate*>(m_templates.Item(i)->GetData());

		if (l_pcTemp->GetDescription().CmpNoCase(p_pchName) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

SP_GM_DocTemplate* SP_GM_Docmanager::GetTemplate(const wxString& p_pchName)
{
	if (!p_pchName)
		return NULL;

	for (size_t i = 0; i < m_templates.GetCount(); i++)
	{
		SP_GM_DocTemplate *l_pcTemp =
				dynamic_cast<SP_GM_DocTemplate*>(m_templates.Item(i)->GetData());

		if (l_pcTemp->GetDescription().CmpNoCase(p_pchName) == 0)
		{
			return l_pcTemp;
		}
	}

	return NULL;
}

//R2: SelectDocumentPath overwrites this Function in wxDocTemplate
//so i copies this function, this reasoned in the strange naming conventions
//this function is for adding wxT("all known netclasses") in the Open Dialog
//this must be tested in Linux
//maybe there is a better solution for this issue, but for now it works

wxDocTemplate* SP_GM_Docmanager::SelectDocumentPath(
		wxDocTemplate **p_pcTemplates,
#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXMAC__)
		int p_iNoTemplates,
#else
		int WXUNUSED(p_iNoTemplates),
#endif
		wxString& p_strPath, long WXUNUSED(p_lFlags), bool WXUNUSED(p_bSave))
{
	// We can only have multiple filters in Windows and GTK
#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXMAC__)
	wxString l_strDescrBuf;

	wxString l_strAllNetclassExt= wxT("All known Netclasses (*.*) |");

	int i;
	for (i = 0; i < p_iNoTemplates; i++)
	{
		if (p_pcTemplates[i]->IsVisible())
		{
			// add a '|' to separate this filter from the previous one
			if ( !l_strDescrBuf.IsEmpty() )
				l_strDescrBuf << wxT('|');

			l_strAllNetclassExt << p_pcTemplates[i]->GetFileFilter() << wxT(";");
			l_strDescrBuf << p_pcTemplates[i]->GetDescription() << wxT(" (") << p_pcTemplates[i]->GetFileFilter() << wxT(") |") << p_pcTemplates[i]->GetFileFilter();
		}
	}

	l_strAllNetclassExt.RemoveLast();
	l_strAllNetclassExt.Append(wxT("|"));

	l_strDescrBuf.Prepend(l_strAllNetclassExt);

#else
	wxString l_strDescrBuf = wxT("*.*");
#endif

	int l_iFilterIndex = -1;

	wxWindow* l_pcParent= wxTheApp->GetTopWindow();

	wxWindow* l_pcFocusWindow = wxWindow::FindFocus();
	if (l_pcFocusWindow)
	{
		while (l_pcFocusWindow && !l_pcFocusWindow->IsKindOf(CLASSINFO(wxDialog)) && !l_pcFocusWindow->IsKindOf(CLASSINFO(wxFrame)))

			l_pcFocusWindow = l_pcFocusWindow->GetParent();

		if (l_pcFocusWindow)
			l_pcParent = l_pcFocusWindow;
	}

	wxString l_strPathTmp = wxFileSelectorEx(_("Select a file"),
												m_lastDirectory,
												wxT(""),
												&l_iFilterIndex,
												l_strDescrBuf,
												0,
												l_pcParent);

	wxDocTemplate *l_pcTheTemplate = (wxDocTemplate *)NULL;
	if (!l_strPathTmp.IsEmpty())
	{
		if (!wxFileExists(l_strPathTmp))
		{
			wxString msgTitle;
			if (!wxTheApp->GetAppName().IsEmpty())
				msgTitle = wxTheApp->GetAppName();
			else
				msgTitle = wxString(_("File error"));

			(void)SP_MESSAGEBOX(_("Sorry, could not open this file."), msgTitle, wxOK | wxICON_EXCLAMATION,
			l_pcParent);

			p_strPath = wxT("");
			return (wxDocTemplate *) NULL;
		}
		m_lastDirectory = wxPathOnly(l_strPathTmp);

		//by R2: set l_iFilterIndex to -1 due to manuell Filter changing
		l_iFilterIndex= -1;

		p_strPath = l_strPathTmp;
		l_pcTheTemplate = FindTemplateForPath(p_strPath);
	}
	else
	{
		p_strPath = wxT("");
	}

	return l_pcTheTemplate;

#if 0
	// In all other windowing systems, until we have more advanced
	// file selectors, we must select the document type (template) first, and
	// _then_ pop up the file selector.
	wxDocTemplate *temp = SelectDocumentType(p_pcTemplates, p_iNoTemplates);
	if (!temp)
	return (wxDocTemplate *) NULL;

	l_strPathTmp = wxFileSelector(_("Select a filewxT("), wxT(""), wxT("")),
			temp->GetDefaultExtension(),
			temp->GetFileFilter(),
			0, wxTheApp->GetTopWindow());

	if (l_strPathTmp)
	{
		p_strPath = l_strPathTmp;
		return temp;
	}
	else
	return (wxDocTemplate *) NULL;
#endif // 0
}

// Handlers for UI update commands

void SP_GM_Docmanager::OnUpdateFileOpen(wxUpdateUIEvent& p_cEvent)
{
	p_cEvent.Enable(!(SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode()));
}

void SP_GM_Docmanager::OnUpdateFileClose(wxUpdateUIEvent& p_cEvent)
{
	wxDocument *doc = GetCurrentDocument();
	p_cEvent.Enable( (doc != (wxDocument*) NULL) && !(SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode()));
}

void SP_GM_Docmanager::OnUpdateFileRevert(wxUpdateUIEvent& p_cEvent)
{
	wxDocument *doc = GetCurrentDocument();
	p_cEvent.Enable( (doc != (wxDocument*) NULL) && !(SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode()));
}

void SP_GM_Docmanager::OnUpdateFileNew(wxUpdateUIEvent& p_cEvent)
{
	p_cEvent.Enable( !(SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode()));
}

void SP_GM_Docmanager::OnUpdateFileSave(wxUpdateUIEvent& p_cEvent)
{
	wxDocument *doc = GetCurrentDocument();
	p_cEvent.Enable(doc && doc->IsModified() && !(SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode()));
}

void SP_GM_Docmanager::OnUpdateFileSaveAs(wxUpdateUIEvent& p_cEvent)
{
	wxDocument *doc = GetCurrentDocument();
	p_cEvent.Enable( (doc != (wxDocument*) NULL) && !(SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode()));
}

