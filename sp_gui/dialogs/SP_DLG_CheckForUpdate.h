//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2006/06/21 $
// Short Description: Check for Update Dialog
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_CheckForUpdate_H__
#define __SP_DLG_CheckForUpdate_H__

#include <wx/wx.h>

class SP_DLG_CheckForUpdate{

public:
    //! constructor
    SP_DLG_CheckForUpdate (wxWindow* p_pcParent);

    //! destructor
    virtual ~SP_DLG_CheckForUpdate ();

    bool CheckForUpdate(bool p_bAskUser = true);

private:
	wxString m_sUrl;
	bool m_bUpdateAllowed;
	bool m_bIs64bit;
	wxString m_sLastUpdateCheck;
	wxSizer* m_pcUpdateSizer;
	wxSizer* m_pcResultSizer;
	wxCheckBox *m_pcUpdateCheckBox;
	wxWindow* m_pcParent;
	wxDialog* m_pcDialog;

	void ShowResult(const wxString& p_sNewVersion, const wxString& p_sDateNewVersion, bool p_bAskUser);


};



#endif // __SP_DLG_CheckForUpdate_H__

