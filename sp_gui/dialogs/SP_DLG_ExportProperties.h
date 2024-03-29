//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/03/30 11:55:00 $
// $Modified: by George Assaf$
// $Modification date: 2020/12/11$
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_EXPORTPROPERTIES__
#define __SP_DLG_EXPORTPROPERTIES__

#include <wx/wx.h>
#include <wx/filepicker.h>

#include "sp_gui/widgets/SP_WDG_Notebook.h"
#include "sp_gui/widgets/SP_WDG_NotebookPage.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

//bysl
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"


class SP_ExportRoutine;
class SP_MDI_Doc;

class SP_DLG_ExportProperties: public wxDialog
{
private:
    DECLARE_EVENT_TABLE()
    //! the top level sizer
    wxBoxSizer* m_pcSizer;
    //! the notebook to have multiple pages, if needed
    SP_WDG_Notebook* m_pcNotebook;
    SP_ExportRoutine* m_pcExport;
    wxFilePickerCtrl* m_pcFilePickerCtrl;
    map<SP_DS_ColListAttribute*, wxChoice*> m_mColListComboBoxes;
    SP_MDI_Doc* m_pcDoc;

    DECLARE_CLASS(SP_DLG_ExportProperties)

protected:
	void SaveData();

public:
    SP_DLG_ExportProperties(SP_ExportRoutine* p_pcAnim,
        wxWindow* p_pcParent,
        SP_MDI_Doc* p_pcDoc,
        const wxString& p_sFilename,
        const wxString& p_sTitle = wxT("Export Properties"),
        long p_nStyle = wxDEFAULT_DIALOG_STYLE);

    SP_WDG_NotebookPage* AddPage(const wxString& p_sLabel = wxT(""));
    /* EVENTS */
    /**	\brief	The OnDlgOk function

	    \param	p_cEvent	the event object as generated by wx'

	    This method is called, if the user clicks the 'OK' button
    */
    void OnDlgOk(wxCommandEvent& p_cEvent);
    /**	\brief	The OnDlgCancel function

	    \param p_cEvent	the event object as generated by wx'

	    This method is called, if the user clicks the 'Cancel' button, pushes ESC
        or closes the dialog with the window-close methods of the ui-manager.
    */
    void OnDlgCancel(wxCommandEvent& p_cEvent);

    void SetNotebookSize(int width, int height); //added by anjali (for export to latex)

    /*const wxString& m_sFilename; */
    wxString GetCurrentFileName() { return  m_pcFilePickerCtrl->GetPath(); }

    std::map<wxString, std::vector<wxString>>  GetGroup2ValueSets();


};

#endif // __SP_DLG_EXPORTPROPERTIES__

