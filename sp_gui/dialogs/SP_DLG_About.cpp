//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2006/11/21 $
// Short Description: About Dialog
//////////////////////////////////////////////////////////////////////
#include "sp_gui/dialogs/SP_DLG_About.h"
#include "sp_revision.h"
#include "sp_build.h"
#include "sp_defines.h"

// under Windows the icons are in the .rc file
// else, we have to include them manually
#ifndef __WXMSW__
    #include "bitmaps/snoopy.xpm"
#endif

SP_DLG_About::SP_DLG_About (wxWindow *p_pcParent, int p_nMilliseconds)
: wxDialog (p_pcParent, wxID_ANY, _("About..."), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP)
{

	SetIcon(wxICON(snoopy));

    // about info
    wxFlexGridSizer *l_pcAboutInfo = new wxFlexGridSizer (2, 0, 2);
    l_pcAboutInfo->Add (new wxStaticText(this, wxID_ANY, _("Vendor: ")),0, wxALIGN_LEFT);
    l_pcAboutInfo->Add (new wxStaticText(this, wxID_ANY, SP_APP_VENDOR),0, wxALIGN_LEFT);
    l_pcAboutInfo->Add (new wxStaticText(this, wxID_ANY, _("Version: ")),0, wxALIGN_LEFT);
    l_pcAboutInfo->Add (new wxStaticText(this, wxID_ANY, SP_APP_REVISION),0, wxALIGN_LEFT);
    l_pcAboutInfo->Add (new wxStaticText(this, wxID_ANY, _("Release: ")),0, wxALIGN_LEFT);
    l_pcAboutInfo->Add (new wxStaticText(this, wxID_ANY, SP_APP_RELEASE),0, wxALIGN_LEFT);
    l_pcAboutInfo->Add (new wxStaticText(this, wxID_ANY, _("Build: ")),0, wxALIGN_LEFT);
    l_pcAboutInfo->Add (new wxStaticText(this, wxID_ANY, SP_APP_BUILD),0, wxALIGN_LEFT);
    l_pcAboutInfo->Add (new wxStaticText(this, wxID_ANY, _("Contributors: \n(in alphabetic order)  ")),0, wxALIGN_LEFT);
    l_pcAboutInfo->Add (new wxStaticText(this, wxID_ANY,
	wxT("Denny Bayer, Matthias Dube, Markus Fieber, Monika Heiner, \n")
	wxT("Mostafa Herajy, Erik Jongsma, Christian Krueger, Anja Kurth, \n")
	wxT("Steffen Laarz, Sebastian Lehrack, Fei Liu, Thomas Meier, \n")
	wxT("Ronny Richter, Christian Rohr, Daniel Scheibler, Martin Schwarick, \n")
	wxT("Alexey Tovchigrechko, Katja Winder")
    )
	,0, wxALIGN_LEFT);

    // about icontitle//info
    wxBoxSizer *l_pcAboutpane = new wxBoxSizer (wxHORIZONTAL);
    wxBitmap bitmap = wxBitmap(wxICON (snoopy));
    l_pcAboutpane->Add (new wxStaticBitmap (this, wxID_ANY, bitmap),
                    1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 20);
    l_pcAboutpane->Add (l_pcAboutInfo, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    l_pcAboutpane->Add (20, 0);

    // about complete
    wxBoxSizer *l_pcTotalpane = new wxBoxSizer (wxVERTICAL);
    l_pcTotalpane->Add (0, 10);
    wxStaticText *l_pcAppname = new wxStaticText(this, wxID_ANY, SP_APP_LONG_NAME);
    l_pcAppname->SetFont (wxFont (16, wxDEFAULT, wxNORMAL, wxBOLD));
    l_pcTotalpane->Add (l_pcAppname, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 40);
    l_pcTotalpane->Add (0, 10);
	l_pcTotalpane->Add (new wxStaticText(this, wxID_ANY, SP_APP_DESCRIPTION),
                    0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 10);
    l_pcTotalpane->Add (0, 15);
	l_pcTotalpane->Add (l_pcAboutpane, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    wxHyperlinkCtrl *l_pcWebsite = new wxHyperlinkCtrl(this, wxID_ANY, SP_APP_WEBPAGE, SP_APP_WEBPAGE);

    l_pcTotalpane->Add (l_pcWebsite, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxBOTTOM, 10);
	l_pcTotalpane->Add (0, 6);
    l_pcTotalpane->Add (new wxStaticText(this, wxID_ANY, wxT("Any comments to:")),
                    0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 10);
	l_pcTotalpane->Add (0, 6);
	wxString l_sEmailadress = wxString(SP_APP_EMAIL).Mid(7);
	wxHyperlinkCtrl *l_pcEmail = new wxHyperlinkCtrl (this, wxID_ANY, l_sEmailadress, SP_APP_EMAIL);
	l_pcTotalpane->Add (l_pcEmail, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxBOTTOM, 10);

	l_pcTotalpane->Add (this->CreateButtonSizer(wxOK), 0, wxALIGN_CENTER | wxALL, 10);

    SetSizerAndFit (l_pcTotalpane);
    CentreOnParent();
    ShowModal();
}

SP_DLG_About::~SP_DLG_About ()
{
}
