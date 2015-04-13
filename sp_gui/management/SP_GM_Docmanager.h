//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GM_DOCMANAGER_H__
#define __SP_GM_DOCMANAGER_H__

#include "sp_core/SP_List.h"

#include <wx/wx.h>
#include <wx/docview.h>
#include <wx/printdlg.h>

class SP_GM_DocTemplate;

class SP_GM_Docmanager: public wxDocManager
{
private:
    DECLARE_DYNAMIC_CLASS(SP_GM_Docmanager)
    DECLARE_EVENT_TABLE()

    // Global print data, to remember settings during the session
    wxPrintData* m_pcPrintData;
    // Global page setup data
    wxPageSetupDialogData* m_pcPageSetupData;

protected:
public:
    SP_GM_Docmanager(long p_nFlags = wxDOC_NEW, bool p_bInitialize = TRUE);
    virtual ~SP_GM_Docmanager();

    wxDocument* CreateDocument(const wxString& p_sPath, long p_nFlags);
    bool CloseDocuments(bool p_bForce = TRUE);
    bool Clear(bool force = TRUE);

    void OnFileSave(wxCommandEvent& p_cEvent);
	void OnSaveSign(wxCommandEvent& p_cEvent);
    void OnFileClose(wxCommandEvent& event);

    void OnPrint(wxCommandEvent& event);
    void OnPreview(wxCommandEvent& event);
    void OnPageSetup(wxCommandEvent& event);

    bool HasTemplate(const wxString& p_pchName);
    SP_GM_DocTemplate* GetTemplate(const wxString& p_pchName);

    void OnUpdateFileOpen(wxUpdateUIEvent& p_cEvent);
    void OnUpdateFileClose(wxUpdateUIEvent& p_cEvent);
    void OnUpdateFileRevert(wxUpdateUIEvent& p_cEvent);
    void OnUpdateFileNew(wxUpdateUIEvent& p_cEvent);
    void OnUpdateFileSave(wxUpdateUIEvent& p_cEvent);
    void OnUpdateFileSaveAs(wxUpdateUIEvent& p_cEvent);

	wxDocTemplate* SelectDocumentPath(wxDocTemplate **templates,
#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXMAC__)
                                                int noTemplates,
#else
                                                int WXUNUSED(noTemplates),
#endif
                                                wxString& path,
                                                long WXUNUSED(flags),
                                                bool WXUNUSED(save));


};

#endif // __SP_GM_DOCMANAGER_H__
