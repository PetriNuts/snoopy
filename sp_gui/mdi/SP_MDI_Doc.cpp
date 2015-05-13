//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/dialogs/SP_DLG_SearchId.h"
#include "sp_gui/dialogs/SP_DLG_ReduceFTree.h"
#include "sp_gui/dialogs/SP_DLG_ChooseInvariant.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_core/SP_Core.h"
#include "sp_core/SP_GPR_Canvas.h"
#include "sp_defines.h"
#include "sp_core/SP_Signer.h"
#include <wx/file.h>
#include <wx/regex.h>
#include <wx/busyinfo.h>

#include "snoopy.h"
#include "sp_gui/interaction/SP_IA_Manager.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_gui/windows/SP_GUI_DevbarContainer.h"

#include "sp_ds/extensions/SP_DS_Transformer.h"

namespace {
// panel with custom controls for file dialog
class SP_CompressPanel : public wxPanel
{
public:
	SP_CompressPanel(wxWindow *parent);
    bool GetCompress() const
    {
        return m_cb->GetValue();
    }

private:
    wxCheckBox *m_cb;
};

SP_CompressPanel::SP_CompressPanel(wxWindow *parent)
            : wxPanel(parent)
{
    m_cb = new wxCheckBox(this, -1, wxT("Compress File"));
    if(wxGetApp().GetCanvasPrefs()->GetCompressFile())
    {
        m_cb->SetValue(SP_Core::Instance()->GetRootDocument()->GetCompress());
    }
    else
    {
        m_cb->SetValue(false);
        m_cb->Disable();
    }
    wxBoxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);
    sizerTop->Add(m_cb, wxSizerFlags().Centre().Border());

    SetSizerAndFit(sizerTop);
}

// a static method can be used instead of a function with most of compilers
static wxWindow* createCompressPanel(wxWindow *parent)
{
    return new SP_CompressPanel(parent);
}

}

IMPLEMENT_DYNAMIC_CLASS(SP_MDI_Doc, wxDocument)

SP_MDI_Doc::SP_MDI_Doc() :
	m_pcGraph(NULL), m_pcDiagram(this), m_pcGraphTreectrl(NULL),
			m_pcCoarseTreectrl(NULL),m_pcColorSetTreectrl(NULL), m_SearchIdDialog(NULL),
			m_ReduceFTDialog(NULL), m_ChooseInvariantDialog(NULL),
			m_eType(SP_DOC_SIMPLE), m_bClose(TRUE), m_pcParentDoc(NULL),
			m_sNetclassName(wxT("")), m_bCompress(wxGetApp().GetCanvasPrefs()->GetCompressFile())
{
}

SP_MDI_Doc::~SP_MDI_Doc()
{
	wxDELETE(m_pcGraph);
}

SP_DS_Graph* SP_MDI_Doc::InitGraph(SP_DS_Netclass* p_pcNetclass)
{
	SP_LOGDEBUG(wxT("InitGraph"));

	if (!p_pcNetclass)
		return NULL;

	m_pcParentDoc = this;
	m_sNetclassName = p_pcNetclass->GetName();

	SP_Core::Instance()->SetDocumentReactivatingFlag( true);
	SP_Core::Instance()->ManageDocChange(this);

	if (m_pcGraph)
		return m_pcGraph;

	m_pcGraph = new SP_DS_Graph(p_pcNetclass);
	// new graph means new coarse tree
	m_pcCoarseTreectrl = m_pcGraph->CreateCoarseTree();
	
	//if(m_pcGraph->GetNetclass()->GetName()==SP_DS_COLEPN_CLASS
	//	||m_pcGraph->GetNetclass()->GetName()==SP_DS_COLSPN_CLASS)
	{
		wxGetApp().GetMainframe()->GetDeclarationContainer()->Show(true);	
		m_pcColorSetTreectrl = m_pcGraph->CreateDeclarationTree();//liu
	}	

	SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(GetFirstView());
	if (l_pcView && l_pcView->GetFrame())
	{
		l_pcView->GetFrame()->SetCoarseTreectrl(m_pcCoarseTreectrl);

		//if(m_pcGraph->GetNetclass()->GetName()==SP_DS_COLEPN_CLASS
		//   ||m_pcGraph->GetNetclass()->GetName()==SP_DS_COLSPN_CLASS)
		{
			l_pcView->GetFrame()->SetColorSetTreectrl(m_pcColorSetTreectrl); 
		}

		l_pcView->GetFrame()->AddGraphToMenu(m_pcGraph);	
		
	}

	if (m_pcGraphTreectrl)
		m_pcGraphTreectrl->Initialise(m_pcGraph);

	if (m_pcColorSetTreectrl)
		m_pcColorSetTreectrl->Initialise(m_pcGraph);

	SetNetnumber(m_pcGraph->GetNewNetnumber());
	DrawAllElements();
	return m_pcGraph;
}

unsigned int SP_MDI_Doc::GetNetnumber()
{
	SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(GetFirstView());

	wxASSERT(l_pcView);

	if (l_pcView)
		return l_pcView->GetNetnumber();

	return 0;
}

void SP_MDI_Doc::SetNetnumber(unsigned int p_nVal)
{
	wxList& l_lViews = GetViews();
	wxNode* l_pcNode = l_lViews.GetFirst();
	while (l_pcNode)
	{
		SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(l_pcNode->GetData());
		l_pcView->SetNetnumber(p_nVal);
		l_pcNode = l_pcNode->GetNext();
	}
}

bool SP_MDI_Doc::DrawAllElements(bool p_bLocalOnly, bool p_bIncludeChildDocs)
{
	if (!m_pcGraph)
		return FALSE;

	bool l_bReturn= TRUE;
	wxList& l_lViews = GetViews();
	wxNode* l_pcNode = l_lViews.GetFirst();
	while (l_pcNode)
	{
		SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(l_pcNode->GetData());

		if (l_pcView->GetFrame() && (!p_bLocalOnly || l_pcView->GetFrame()->IsShown()))
			l_bReturn &= l_pcView->DrawAllElements(m_pcGraph, TRUE);

		l_pcNode = l_pcNode->GetNext();
	}

	if (!p_bIncludeChildDocs)
		return l_bReturn;

	SP_SetCoarseDoc::iterator l_Iter;
	for (l_Iter = m_tsChildDocs.begin(); l_Iter != m_tsChildDocs.end(); ++l_Iter)
		l_bReturn &= (*l_Iter)->DrawAllElements(false, true);
	//----------------------------------------------------------------------------------

	return l_bReturn;
}

void SP_MDI_Doc::Refresh()
{
	if (m_pcParentDoc)
		if (m_pcParentDoc != this)
		{
			m_pcParentDoc->Refresh();
			return;
		}

	SP_SetCoarseDoc::iterator l_Iter;
	for (l_Iter = m_tsChildDocs.begin(); l_Iter != m_tsChildDocs.end(); ++l_Iter)
		(*l_Iter)->RefreshAllViews();

	RefreshAllViews();
}

void SP_MDI_Doc::RefreshAllViews()
{
	wxList& l_lViews = GetViews();
	wxNode* l_pcNode = l_lViews.GetFirst();
	while (l_pcNode)
	{
		SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(l_pcNode->GetData());

		if (l_pcView)
			l_pcView->Refresh();

		l_pcNode = l_pcNode->GetNext();
	}
}

bool SP_MDI_Doc::ShowAll()
{
	bool l_bReturn= TRUE;
	wxList& l_lViews = GetViews();
	wxNode* l_pcNode = l_lViews.GetFirst();
	while (l_pcNode)
	{
		SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(l_pcNode->GetData());
		l_bReturn &= l_pcView->ShowFrame();
		l_pcNode = l_pcNode->GetNext();
	}
	return l_bReturn;
}

bool SP_MDI_Doc::SetEditElement(SP_Type* p_pcElement)
{
	bool l_bReturn= TRUE;
	wxList& l_lViews = GetViews();
	wxNode* l_pcNode = l_lViews.GetFirst();
	while (l_pcNode)
	{
		SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(l_pcNode->GetData());

		l_bReturn &= l_pcView->SetEditElement(p_pcElement);

		l_pcNode = l_pcNode->GetNext();
	}
	return l_bReturn;
}

bool SP_MDI_Doc::SetClose(bool p_bVal)
{
	bool l_bReturn= TRUE;

	m_bClose = p_bVal;

	wxNode* l_pcNode = GetViews().GetFirst();
	while (l_pcNode)
	{
		SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(l_pcNode->GetData());
		if(l_pcView)
			l_bReturn &= l_pcView->SetClose(m_bClose);
		l_pcNode = l_pcNode->GetNext();
	}

	return l_bReturn;
}

void SP_MDI_Doc::Modify(bool p_bVal)
{
	SP_SetCoarseDoc::iterator l_Iter;
	for (l_Iter = m_tsChildDocs.begin(); l_Iter != m_tsChildDocs.end(); ++l_Iter)
		(*l_Iter)->Modify(p_bVal);

	m_documentModified = p_bVal;
}

void SP_MDI_Doc::DoSetModify(bool p_bVal, SP_MDI_Doc* p_pcSender)
{
	SP_SetCoarseDoc::iterator l_Iter;
	for (l_Iter = m_tsChildDocs.begin(); l_Iter != m_tsChildDocs.end(); ++l_Iter)
		if ((*l_Iter) != p_pcSender)
			(*l_Iter)->Modify(p_bVal);

	wxDocument::Modify(p_bVal);
}

void SP_MDI_Doc::SetFilename(const wxString& p_sName, bool p_bNotify)
{
	SP_SetCoarseDoc::iterator l_Iter;
	for (l_Iter = m_tsChildDocs.begin(); l_Iter != m_tsChildDocs.end(); ++l_Iter)
	{
		(*l_Iter)->SetFilename(p_sName, p_bNotify);
		(*l_Iter)->UpdateTitle();
	}

	wxDocument::SetFilename(p_sName, p_bNotify);

	SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(GetFirstView());
	if (l_pcView && l_pcView->GetFrame())
	{
		l_pcView->GetFrame()->SetTitle(GetUserReadableName());
	}

	if (m_SearchIdDialog)
	{
		m_SearchIdDialog->SetTitlePrefix(GetTitle() + wxT(": "));
	}
}

bool SP_MDI_Doc::OnCloseDocument()
{
	if (wxGetApp().GetIAManager())
		wxGetApp().GetIAManager()->DisconnectNet(GetFilename(), true);

	SP_Core::Instance()->RemoveFromDocChange(this);

	return wxDocument::OnCloseDocument();
}

bool SP_MDI_Doc::SaveAs()
{
#if 1
    wxDocTemplate *docTemplate = GetDocumentTemplate();
    if (!docTemplate)
        return false;

    wxString filter = docTemplate->GetFileFilter() ;

    wxString defaultDir = docTemplate->GetDirectory();
    if ( defaultDir.empty() )
    {
        defaultDir = wxPathOnly(GetFilename());
        if ( defaultDir.empty() )
            defaultDir = GetDocumentManager()->GetLastDirectory();
    }

/*    wxString fileName = wxFileSelector(_("Save As"),
            defaultDir,
            wxFileNameFromPath(GetFilename()),
            docTemplate->GetDefaultExtension(),
            filter,
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
            GetDocumentWindow());
*/
    wxFileDialog
            saveFileDialog(GetDocumentWindow(), _("Save As"),
            		defaultDir, wxFileNameFromPath(GetFilename()),
            		filter, wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    saveFileDialog.SetExtraControlCreator(&createCompressPanel);

    if (saveFileDialog.ShowModal() == wxID_CANCEL)
    	return false;     // the user changed idea...

    wxString fileName = saveFileDialog.GetPath();
    wxWindow* extra = saveFileDialog.GetExtraControl();
    if(extra)
    	SetCompress(static_cast<SP_CompressPanel*>(extra)->GetCompress());

    if (fileName.empty())
        return false; // cancelled by user

    // Files that were not saved correctly are not added to the FileHistory.
    if (!OnSaveDocument(fileName))
        return false;

    SetTitle(wxFileNameFromPath(fileName));
    SetFilename(fileName, true);    // will call OnChangeFileName automatically

    // A file that doesn't use the default extension of its document template
    // cannot be opened via the FileHistory, so we do not add it.
    if (docTemplate->FileMatchesTemplate(fileName))
    {
        GetDocumentManager()->AddFileToHistory(fileName);
    }
    else //: the user will probably not be able to open the file again, so we
    	//      could warn about the wrong file-extension here
    {
        SP_MESSAGEBOX( wxT("The extension name should not be changed while saving as. Please choose export if you want to change from one net class to another."),
        					wxT("Error while saving"), wxOK | wxICON_EXCLAMATION);

    }
    return true;

#else
	wxString l_sOldFileFilter = this->GetDocumentTemplate()->GetFileFilter();
	bool l_bReturn = wxDocument::SaveAs();
	if(l_bReturn)
	{
		SetFilename(GetFilename());

		wxFileName l_cFilename(GetFilename());
		wxString l_sExt = l_cFilename.GetExt();

		wxRegEx l_cRegEx(wxT("\\.") + l_sExt + wxT("\\W|$"), wxRE_ADVANCED);

		if( !l_cRegEx.Matches(l_sOldFileFilter) )
		{
			SP_MESSAGEBOX( wxT("The extension name should not be changed while saving as. Please choose export if you want to change from one net class to another."),
					wxT("Error while saving"), wxOK | wxICON_EXCLAMATION);
		}
	}
	return l_bReturn;
#endif
}

bool SP_MDI_Doc::OnSaveDocument(const wxString& p_sFile)
{
	if (m_pcGraph)
	{
        wxWindowDisabler disableAll;
        wxBusyInfo wait("Please wait, saving...");

		SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(GetFirstView());
		l_pcView->SelectAll(true);
		wxCommandEvent l_cEvent;
		l_pcView->OnUnHide(l_cEvent);
		m_pcGraph->SqueezeIdAttributes();
		if (!m_pcGraph->OnSaveDocument(p_sFile))
		{
			SP_MESSAGEBOX(m_pcGraph->GetLastError(), wxT("Error while saving"),
					wxOK | wxICON_EXCLAMATION);
			return FALSE;
		}
		Modify(FALSE);
		m_savedYet = TRUE;
		SetFilename(p_sFile);
	}

	return TRUE;
}

bool SP_MDI_Doc::OnOpenDocument(const wxString& p_sFile)
{

	// we are now right inside SP_GM_Docmanager::CreateDocument but
	// before the initialising of the graph member, so we have to do it ourself
	SP_GM_DocTemplate* l_pcDocTemplate =
			dynamic_cast<SP_GM_DocTemplate*>(GetDocumentTemplate());

	if (l_pcDocTemplate)
	{
		SP_DS_Netclass* l_pcNS = l_pcDocTemplate->GetNetclass();
		InitGraph(l_pcNS);
	}
	// don't know, whether it should fail, but check it anyway
	if (!m_pcGraph)
		return FALSE;

	// we will show the log window if needed
	SP_GUI_LogWindow* logWindow = wxGetApp().GetMainframe()->GetExportLogWindow();
	if (logWindow)
		logWindow->ResetNewMessages();

	if (!m_pcGraph->OnOpenDocument(p_sFile))
	{
		SP_LOGERROR( m_pcGraph->GetLastError());
		if (logWindow && logWindow->NewMessagesAdded())
			logWindow->Show();
		return FALSE;
	}
	else
	{
		if(!m_pcGraph->GetLastError().IsEmpty())
			wxGetApp().GetMainframe()->SetStatusText(m_pcGraph->GetLastError());
	}

	Modify(FALSE);
	m_savedYet = TRUE;

    //Add codes that deal with the transformation
	{
		SP_DS_Transformer l_cTransformer;
		l_cTransformer.Check(m_pcGraph);
	}
//	else
    if(m_pcGraph->GetNetclass()->GetName()==SP_DS_COLEPN_CLASS
    	|| m_pcGraph->GetNetclass()->GetName()==SP_DS_COLSPN_CLASS
		|| m_pcGraph->GetNetclass()->GetName()==SP_DS_COLPN_CLASS
		|| m_pcGraph->GetNetclass()->GetName()==SP_DS_COLHPN_CLASS
    	|| m_pcGraph->GetNetclass()->GetName()==SP_DS_COLCPN_CLASS)
	{
		m_pcGraph->CreateDeclarationTree()->UpdateColorSetTree();//liu
	}
    //by sl
    if(m_pcGraph->GetNetclass()->GetName()==SP_DS_PN_CLASS
    		|| m_pcGraph->GetNetclass()->GetName()==SP_DS_EXTPN_CLASS
    		|| m_pcGraph->GetNetclass()->GetName()==SP_DS_SPN_CLASS
    		|| m_pcGraph->GetNetclass()->GetName()==SP_DS_HYBRIDPN_CLASS
    		|| m_pcGraph->GetNetclass()->GetName()==SP_DS_CONTINUOUSPED_CLASS)
    	m_pcGraph->CreateDeclarationTree()->UpdateOtherTree();

	SP_MDI_View* l_pcView = dynamic_cast<SP_MDI_View*>(GetFirstView());
	m_pcGraph->ShowOnCanvas(l_pcView->GetCanvas());

	if (wxFile::Exists(p_sFile+wxT(".sgn")))
	{
		SP_Signer* sgn = NULL;
		try
		{
			sgn = new SP_Signer(p_sFile);
			if (sgn->verifyFile())
			{
				SP_MESSAGEBOX(wxString::Format(wxT("The graph has not been changed.\n\n")
								wxT("Date of signature: %s"),
								sgn->getTimeStampInSgnFile().c_str()),
						wxT("Verification result..."),
						wxOK | wxICON_INFORMATION);
			}
			else if (sgn->verifyStructure())
			{
				SP_MESSAGEBOX(wxString::Format(wxT("The graph's layout has been changed,\n")
								wxT("but the structure is still unchanged.\n\n")
								wxT("Date of signature: %s"),
								sgn->getTimeStampInSgnFile().c_str()),
						wxT("Verification result..."),
						wxOK | wxICON_EXCLAMATION);
			}
			else
			{
				SP_MESSAGEBOX(wxString::Format(wxT("The graph has been changed.\n\n")
								wxT("Date of signature: %s"),
								sgn->getTimeStampInSgnFile().c_str()),
						wxT("Verification result..."),
						wxOK | wxICON_ERROR);
			}
		}
		catch(const wxString& ex)
		{
			SP_MESSAGEBOX(ex, wxT("Error..."), wxOK | wxICON_EXCLAMATION);
		}
		wxDELETE(sgn);
	}
	if (logWindow && logWindow->NewMessagesAdded())
		logWindow->Show();
	return TRUE;
}

SP_DLG_SearchId* SP_MDI_Doc::GetSearchIdDialog()
{
	if (!m_pcParentDoc)
		return NULL;
	if (m_pcParentDoc != this)
	{
		return m_pcParentDoc->GetSearchIdDialog();
	}

	if (!m_SearchIdDialog)
	{
		SP_MDI_View* view = dynamic_cast<SP_MDI_View*>(GetFirstView());
		if (!view || !view->GetFrame())
			return NULL;
		m_SearchIdDialog = new SP_DLG_SearchId (m_pcGraph, view->GetFrame());
		m_SearchIdDialog->SetTitlePrefix(view->GetFrame()->GetTitle() + wxT(": "));
		SP_LOGDEBUG(wxT("new search dialog created"));
	}
	return m_SearchIdDialog;
}

SP_DLG_ChooseInvariant* SP_MDI_Doc::GetChooseInvariantDialog()
{
	if (!m_pcParentDoc)
		return NULL;
	if (m_pcParentDoc != this)
	{
		return m_pcParentDoc->GetChooseInvariantDialog();
	}

	if (!m_ChooseInvariantDialog)
	{
		SP_MDI_View* view = dynamic_cast<SP_MDI_View*>(GetFirstView());
		if (!view || !view->GetFrame())
			return NULL;

		m_ChooseInvariantDialog = new SP_DLG_ChooseInvariant(m_pcGraph, view->GetFrame());
		SP_LOGDEBUG(wxT("new Invariant Choosing Dialog created"));
	}
	if (!m_ChooseInvariantDialog->LoadInvariantFile())
	{
		return NULL;
	}

	return m_ChooseInvariantDialog;
}

void SP_MDI_Doc::ResetChooseInvariantColours()
{
	if (m_ChooseInvariantDialog)
	{
		m_ChooseInvariantDialog->DiscardColours();
	}
}

SP_DLG_ReduceFTree* SP_MDI_Doc::GetReduceFTDialog()
{
	if (!m_pcParentDoc)
		return NULL;
	if (m_pcParentDoc != this)
	{
		return m_pcParentDoc->GetReduceFTDialog();
	}

	if (!m_ReduceFTDialog)
	{
		SP_MDI_View* view = dynamic_cast<SP_MDI_View*>(GetFirstView());
		if (!view || !view->GetFrame())
			return NULL;
		m_ReduceFTDialog = new SP_DLG_ReduceFTree(m_pcGraph, view->GetFrame());
		SP_LOGDEBUG(wxT("new search dialog created"));
	}

	return m_ReduceFTDialog;
}

