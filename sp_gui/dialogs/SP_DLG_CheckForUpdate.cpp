//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2008/06/21 $
// Short Description: Check for Update Dialog
//////////////////////////////////////////////////////////////////////
#include "sp_gui/dialogs/SP_DLG_CheckForUpdate.h"
#include <wx/config.h>
#include <wx/regex.h>
#include <wx/hyperlink.h>
#include <wx/url.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

#include "sp_revision.h"
#include "sp_build.h"
#include "sp_defines.h"

SP_DLG_CheckForUpdate::SP_DLG_CheckForUpdate (wxWindow *p_pcParent):
m_bUpdateAllowed(false),
m_bIs64bit(false),
m_sLastUpdateCheck(wxT("")),
m_pcUpdateSizer(NULL),
m_pcResultSizer(NULL),
m_pcUpdateCheckBox(NULL),
m_pcParent(p_pcParent)
{
#if defined(_LP64)
	m_bIs64bit = true;
#endif
	/**
	 * one has to update the checkupdate.txt file on the webserver each time a new beta/stable version is released.
	 */
	m_sUrl = wxT("http://www-dssz.informatik.tu-cottbus.de/checkupdate.txt");
	//read from config if update check is required
	wxConfig config(SP_APP_SHORT_NAME, SP_APP_VENDOR);
	config.Read(wxT("LastUpdateCheck"),&m_sLastUpdateCheck);
	if(!config.Read(wxT("UpdateAllowed"), &m_bUpdateAllowed))
	{
		m_bUpdateAllowed = true;
	}
	m_pcDialog = new wxDialog(m_pcParent, wxID_ANY, _("Check for Update"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP);
	m_pcDialog->Show(false);
}

SP_DLG_CheckForUpdate::~SP_DLG_CheckForUpdate ()
{
	if(m_pcUpdateCheckBox)
		m_bUpdateAllowed = m_pcUpdateCheckBox->GetValue();
	wxConfig config(SP_APP_SHORT_NAME, SP_APP_VENDOR);
	config.Write(wxT("LastUpdateCheck"),m_sLastUpdateCheck);
	config.Write(wxT("UpdateAllowed"), m_bUpdateAllowed);
	m_pcDialog->Destroy();

}

bool
SP_DLG_CheckForUpdate::CheckForUpdate(bool p_bAskUser)
{

	if(p_bAskUser)
	{
		m_pcUpdateSizer = new wxBoxSizer(wxVERTICAL);
		m_pcUpdateSizer->Add(new wxStaticText(m_pcDialog, wxID_ANY, wxT("Do you want to check for a new version?")),
							0,  wxALL,10);
		m_pcUpdateCheckBox = new wxCheckBox(m_pcDialog, wxID_ANY, wxT("Automatically check for update every week."));
		m_pcUpdateCheckBox->SetValue(m_bUpdateAllowed);
		m_pcUpdateSizer->Add(m_pcUpdateCheckBox, 0, wxALL, 10);
		m_pcUpdateSizer->Add(m_pcDialog->CreateButtonSizer(wxOK | wxCANCEL), 0,  wxALL, 10);
		m_pcUpdateSizer->Show(true);
		m_pcDialog->SetSizerAndFit(m_pcUpdateSizer);
		m_pcDialog->CentreOnParent();
		if(m_pcDialog->ShowModal() != wxID_OK)
		{
			return false;
		}
		if(m_pcUpdateCheckBox)
			m_bUpdateAllowed = m_pcUpdateCheckBox->GetValue();

	}else
	{
		wxDateTime l_cLastCheck;
		if(l_cLastCheck.ParseDate(m_sLastUpdateCheck))
		{
			wxTimeSpan l_cDiff = wxDateTime::Today() - l_cLastCheck;
			if(l_cDiff.GetWeeks() < 1 || !m_bUpdateAllowed)
				return false;
		}
	}

	m_sLastUpdateCheck = wxDateTime::Today().FormatISODate();
	wxURL l_cUrl(m_sUrl);

	if(l_cUrl.GetError() == wxURL_NOERR)
	{
		l_cUrl.GetProtocol().SetTimeout(5);
		wxInputStream* l_pcInputStream = l_cUrl.GetInputStream();
		if(!l_pcInputStream)
		{
			wxDELETE(l_pcInputStream);
			ShowResult(wxT("error"),wxT("error"), p_bAskUser);
			return false;
		}
		wxString l_sSnoopyWebpage(wxT(""));
		wxTextInputStream l_cTextInput( *l_pcInputStream );
		while(l_pcInputStream->CanRead())
		{
			l_sSnoopyWebpage.Append(l_cTextInput.GetChar());
		}
		wxDELETE(l_pcInputStream);

		wxString l_sOsVersion;
		wxOperatingSystemId l_nOsId = ::wxGetOsVersion();
		if (l_nOsId == wxOS_WINDOWS_NT)
		{
			l_sOsVersion = wxT("windows");
		}
		else if (l_nOsId == wxOS_UNIX_LINUX)
		{
			l_sOsVersion = wxT("linux");
			wxFFileInputStream l_cFile(wxT("/proc/version"));
			wxTextInputStream l_cInput(l_cFile);
			wxString l_sData;
			while(l_cFile.CanRead())
			{
				l_sData.Append(l_cInput.GetChar());
			}
			if(l_sData.Find(wxT("Ubuntu")) != wxNOT_FOUND)
				l_sOsVersion += wxT("-ubuntu");
			else if(l_sData.Find(wxT("Debian")) != wxNOT_FOUND)
				l_sOsVersion += wxT("-debian");
			else if(l_sData.Find(wxT("SUSE")) != wxNOT_FOUND)
				l_sOsVersion += wxT("-suse");
			else if(l_sData.Find(wxT("Red Hat")) != wxNOT_FOUND)
				l_sOsVersion += wxT("-fedora");
			if(m_bIs64bit)
				l_sOsVersion += wxT("64");
		}
		else if (l_nOsId == wxOS_MAC_OSX_DARWIN)
		{
			l_sOsVersion = wxT("macosx-intel");
		}
		else
		{
			l_sOsVersion = wxT("unknown");
		}
		wxDateTime l_cDateThis;
		l_cDateThis.ParseDate(SP_APP_BUILD);
		wxDateTime l_cDateNewVersion;

		//test for new stable version
		wxString l_sTestRegEx = wxT("snoopy-");
		l_sTestRegEx << wxT("stable-") << l_sOsVersion << wxT("-[0-9]{4}-[0-9]{2}-[0-9]{2}") << wxT("\\.(dmg|msi|tgz|rpm|deb)");
		SP_LOGDEBUG(l_sTestRegEx);
		wxRegEx l_cExp(l_sTestRegEx);
		if(l_cExp.Matches(l_sSnoopyWebpage))
		{
			wxString l_sTestResult = l_cExp.GetMatch(l_sSnoopyWebpage, 0);
			l_cExp.Compile(wxT("[0-9]{4}-[0-9]{2}-[0-9]{2}"));
			if(l_cExp.Matches(l_sTestResult))
			{
				l_cDateNewVersion.ParseDate(l_cExp.GetMatch(l_sTestResult,0));
				if(l_cDateNewVersion.IsLaterThan(l_cDateThis))
				{
					ShowResult(l_sTestResult, l_cDateNewVersion.FormatISODate(), p_bAskUser);
					return true;
				}
			}
		}
		//test for new beta version
		l_sTestRegEx.Replace(wxT("stable"), wxT("beta"));
		SP_LOGDEBUG(l_sTestRegEx);
		l_cExp.Compile(l_sTestRegEx);
		if(l_cExp.Matches(l_sSnoopyWebpage))
		{
			wxString l_sTestResult = l_cExp.GetMatch(l_sSnoopyWebpage, 0);
			l_cExp.Compile(wxT("[0-9]{4}-[0-9]{2}-[0-9]{2}"));
			if(l_cExp.Matches(l_sTestResult))
			{
				l_cDateNewVersion.ParseDate(l_cExp.GetMatch(l_sTestResult,0));
				if(l_cDateNewVersion.IsLaterThan(l_cDateThis))
				{
					ShowResult(l_sTestResult, l_cDateNewVersion.FormatISODate(), p_bAskUser);
				}
				else
				{
					ShowResult(wxEmptyString, wxEmptyString, p_bAskUser);
				}
				return true;
			}
		}
	}
	ShowResult(wxT("error"),wxT("error"), p_bAskUser);
	return false;
}

void
SP_DLG_CheckForUpdate::ShowResult(const wxString& p_sNewVersion, const wxString& p_sDateNewVersion, bool p_bAskUser)
{
	if(m_pcUpdateSizer)
	{
		m_pcUpdateSizer->Show(false);
	}
	m_pcResultSizer = new wxBoxSizer(wxVERTICAL);
	wxString l_sText = wxT("You already have the latest version.");
//	wxString l_sUrl = wxT("");
	if(!p_sNewVersion.IsEmpty())
	{
		if(p_sNewVersion.Contains(wxT("stable")))
		{
			l_sText = wxT("There is a new stable version available.");
//			l_sUrl = m_sUrl.BeforeLast(wxT('/')) + wxT('/') + p_sNewVersion;
		}
		else if(p_sNewVersion.Contains(wxT("beta")))
		{
			l_sText = wxT("There is a new beta version available.");
//			l_sUrl = m_sUrl.BeforeLast(wxT('/')) + wxT('/') + p_sNewVersion;
		}else if(p_sNewVersion.Contains(wxT("error")))
		{
			l_sText = wxT("Can't connect to server. Please try again later.");
		}
	}
	m_pcResultSizer->Add(new wxStaticText(m_pcDialog, wxID_ANY, l_sText),	0,  wxALL,10);
/*	if(!l_sUrl.IsEmpty())
	{
		m_pcResultSizer->Add(new wxStaticText(m_pcDialog, wxID_ANY, wxT("Click on the link to download the new version:")),	0,  wxALL,10);
		wxHyperlinkCtrl *l_pcWebsite = new wxHyperlinkCtrl(m_pcDialog, wxID_ANY, p_sNewVersion, l_sUrl);
		m_pcResultSizer->Add(l_pcWebsite, 0,  wxALL, 10);

	}
*/
	m_pcResultSizer->Add(new wxStaticText(m_pcDialog, wxID_ANY, wxT("Click on the link to open the")),	0,  wxALL,10);
	wxHyperlinkCtrl *l_pcWebsite = new wxHyperlinkCtrl(m_pcDialog, wxID_ANY, wxT("Snoopy webpage."), SP_APP_WEBPAGE);
	m_pcResultSizer->Add(l_pcWebsite, 0,  wxALL, 10);
	m_pcUpdateCheckBox = new wxCheckBox(m_pcDialog, wxID_ANY, wxT("Automatically check for update every week."));
	m_pcUpdateCheckBox->SetValue(m_bUpdateAllowed);
	m_pcResultSizer->Add(m_pcUpdateCheckBox, 0, wxALL, 10);
	m_pcResultSizer->Add(m_pcDialog->CreateButtonSizer(wxOK), 0,  wxALL, 10);
	m_pcResultSizer->Show(true);
	m_pcDialog->SetSizerAndFit(m_pcResultSizer);
	m_pcDialog->CentreOnParent();

	if(p_bAskUser || (!p_sNewVersion.IsEmpty() && !p_sNewVersion.Contains(wxT("error"))))
		m_pcDialog->ShowModal();
}

