//////////////////////////////////////////////////////////////////////
// $Source: SP_GPR_Fonts.h
// $Author: MD $
// $Version: $
// $Revision: $
// $Date: 2005/09/19 $
// Short Description: contains global fonts preferences
//
// Notes: - there are currently 10 different font types allowed
//					otherwise the wxT("SP_ID_BUTTON_FONT") id's have to be adjusted
//					in <sp_defines.h> and the corresponding event table
// TODO:	- perhaps fonts should be stored in the net file
//////////////////////////////////////////////////////////////////////

#include "sp_core/SP_GPR_Fonts.h"

#include "sp_revision.h"
#include "sp_defines.h"
#include "snoopy.h"
#include "sp_core/SP_Core.h"
#include "sp_gr/base/SP_GR_BaseText.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/dialogs/SP_DLG_GlobalPreferences.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"

#include <wx/spinctrl.h>
#include <wx/fontdlg.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SP_GPR_Fonts::SP_GPR_Fonts()
{

	//register the different font categories that will be used
	m_lsFontTypeList.insert(wxString(wxT("Default")));
	m_lsFontTypeList.insert(wxString(wxT("Comments")));

}

SP_GPR_Fonts::~SP_GPR_Fonts()
{
}


//////////////////////////////////////////////////////////////////////
// Getter/Setter
//////////////////////////////////////////////////////////////////////
/****************
 * From the article "8 Definitive Web Font Stacks"
 * by Michael Tuck
 * http://www.sitepoint.com/article/eight-definitive-font-stacks
 * Copyright SitePoint 2008
 */

/* Times New Roman-based stack */
//font-family: Cambria, "Hoefler Text", Utopia, "Liberation Serif", "Nimbus Roman No9 L Regular", Times, "Times New Roman", serif;

/* Modern Georgia-based serif stack */
//font-family: Constantia, "Lucida Bright", Lucidabright, "Lucida Serif", Lucida, "DejaVu Serif", "Bitstream Vera Serif", "Liberation Serif", Georgia, serif;

/* Traditional Garamond-based serif stack */
//font-family: "Palatino Linotype", Palatino, Palladio, "URW Palladio L", "Book Antiqua", Baskerville, "Bookman Old Style", "Bitstream Charter", "Nimbus Roman No9 L", Garamond, "Apple Garamond", "ITC Garamond Narrow", "New Century Schoolbook", "Century Schoolbook", "Century Schoolbook L", Georgia, serif;

/* Helvetica/Arial-based sans serif stack */
//font-family: Frutiger, "Frutiger Linotype", Univers, Calibri, "Gill Sans", "Gill Sans MT", "Myriad Pro", Myriad, "DejaVu Sans Condensed", "Liberation Sans", "Nimbus Sans L", Tahoma, Geneva, "Helvetica Neue", Helvetica, Arial, sans serif;

/* Verdana-based sans serif stack */
//font-family: Corbel, "Lucida Grande", "Lucida Sans Unicode", "Lucida Sans", "DejaVu Sans", "Bitstream Vera Sans", "Liberation Sans", Verdana, "Verdana Ref", sans serif;

/* Trebuchet-based sans serif stack */
//font-family: "Segoe UI", Candara, "Bitstream Vera Sans", "DejaVu Sans", "Bitstream Vera Sans", "Trebuchet MS", Verdana, "Verdana Ref", sans serif;

/* Impact-based sans serif stack */
//font-family: Impact, Haettenschweiler, "Franklin Gothic Bold", Charcoal, "Helvetica Inserat", "Bitstream Vera Sans Bold", "Arial Black", sans serif;

/* Monospace stack */
//font-family: Consolas, "Andale Mono WT", "Andale Mono", "Lucida Console", "Lucida Sans Typewriter", "DejaVu Sans Mono", "Bitstream Vera Sans Mono", "Liberation Mono", "Nimbus Mono L", Monaco, "Courier New", Courier, monospace;
//TODO check whether future wxWidgets version use font family correctly
wxFont*
GetCorrectFont(const wxFont& p_Font)
{
	wxString l_aSerif[] = {wxT("Cambria"), wxT("Hoefler Text"), wxT("Utopia"), wxT("Liberation Serif"),
							wxT("Nimbus Roman No9 L Regular"), wxT("Times"), wxT("Times New Roman"), wxT("serif"),
							wxT("Constantia"), wxT("Lucida Bright"), wxT("Lucidabright"), wxT("Lucida Serif"),
							wxT("Lucida"), wxT("DejaVu Serif"), wxT("Bitstream Vera Serif"), wxT("Liberation Serif"), wxT("Georgia"),
							wxT("")};
	wxString l_aSansSerif[] = {wxT("Frutiger"), wxT("Frutiger Linotype"), wxT("Univers"), wxT("Calibri"), wxT("Gill Sans"), wxT("Gill Sans MT"),
								wxT("Myriad Pro"), wxT("Myriad"), wxT("DejaVu Sans Condensed"), wxT("Liberation Sans"), wxT("Nimbus Sans L"),
								wxT("Tahoma"), wxT("Geneva"), wxT("Helvetica Neue"), wxT("Helvetica"), wxT("Arial"), wxT("sans serif"),
								wxT("Corbel"), wxT("Lucida Grande"), wxT("Lucida Sans Unicode"), wxT("Lucida Sans"), wxT("DejaVu Sans"),
								wxT("Bitstream Vera Sans"), wxT("Liberation Sans"), wxT("Verdana"), wxT("Verdana Ref"), wxT("Segoe UI"),
								wxT("Candara"), wxT("Trebuchet MS"),
								wxT("")};
	wxString l_aMonospace[] = {wxT("Consolas"), wxT("Andale Mono WT"), wxT("Andale Mono"), wxT("Lucida Console"), wxT("Lucida Sans Typewriter"),
								wxT("DejaVu Sans Mono"), wxT("Bitstream Vera Sans Mono"), wxT("Liberation Mono"), wxT("Nimbus Mono L"),
								wxT("Monaco"), wxT("Courier New"), wxT("Courier"), wxT("monospace"),
								wxT("")};
	bool l_bFound = false;
	wxFontFamily l_eFamily = wxFONTFAMILY_DEFAULT;
	//check serif fonts
	wxString* l_itFontName = l_aSerif;
	while(!l_bFound && !l_itFontName->IsEmpty())
	{
		if((*l_itFontName) == p_Font.GetFaceName())
		{
			l_eFamily = wxFONTFAMILY_ROMAN;
			l_bFound = true;
			break;
		}
		++l_itFontName;
	}
	//check sans serif fonts
	l_itFontName = l_aSansSerif;
	while(!l_bFound && !l_itFontName->IsEmpty())
	{
		if((*l_itFontName) == p_Font.GetFaceName())
		{
			l_eFamily = wxFONTFAMILY_SWISS;
			l_bFound = true;
			break;
		}
		++l_itFontName;
	}
	//check monospace fonts
	l_itFontName = l_aMonospace;
	while(!l_bFound && !l_itFontName->IsEmpty())
	{
		if((*l_itFontName) == p_Font.GetFaceName())
		{
			l_eFamily = wxFONTFAMILY_MODERN;
			l_bFound = true;
			break;
		}
		++l_itFontName;
	}

	wxFont* l_pcFont = wxTheFontList->FindOrCreateFont(p_Font.GetPointSize(),
										l_eFamily,
										p_Font.GetStyle(),
										p_Font.GetWeight(),
										p_Font.GetUnderlined(),
										p_Font.GetFaceName());
	return l_pcFont;
}

void
SP_GPR_Fonts::SetFont(const wxString& p_sNetClass, const wxString& p_sFontType, wxFont* p_Font)
{
	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end())
	{
		wxString l_sKey = wxString::Format(wxT("%s|%s"), p_sNetClass.c_str(), p_sFontType.c_str());
		if (p_Font && p_Font->IsOk())
		{
			m_mFonts[l_sKey] = GetCorrectFont(*p_Font);
		}
		else
		{
			// if something went wrong, a default font is used if there isn't already one stored
			if (!m_mFonts[l_sKey] || !m_mFonts[l_sKey]->IsOk())
			{
				m_mFonts[l_sKey] = wxTheFontList->FindOrCreateFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, FALSE);
			}
		}
	}
}

wxFont*
SP_GPR_Fonts::GetNormalFont(const wxString& p_sNetClass, const wxString& p_sFontType)
{

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()
			&& m_lsFontTypeList.find(p_sFontType) != m_lsFontTypeList.end()) {

		wxString l_sKey = wxString::Format(wxT("%s|%s"), p_sNetClass.c_str(), p_sFontType.c_str());
		if (m_mFonts[l_sKey]->IsOk()) {
			return m_mFonts[l_sKey];
		}
	}
	wxFont* l_pcFont = wxTheFontList->FindOrCreateFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, FALSE);
	return l_pcFont;
}

wxFont*
SP_GPR_Fonts::GetSelectFont(const wxString& p_sNetClass, const wxString& p_sFontType) {
	wxFont* l_pcFont;
	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()
			&& m_lsFontTypeList.find(p_sFontType) != m_lsFontTypeList.end()) {

		wxString l_sKey = wxString::Format(wxT("%s|%s"), p_sNetClass.c_str(), p_sFontType.c_str());
		if (m_mFonts[l_sKey]->IsOk()) {
			l_pcFont = m_mFonts[l_sKey];
			wxFont* l_pcNewFont = wxTheFontList->FindOrCreateFont(
				l_pcFont->GetPointSize(),
				l_pcFont->GetFamily(),
				l_pcFont->GetStyle(),
				wxFONTWEIGHT_BOLD,
				l_pcFont->GetUnderlined(),
				l_pcFont->GetFaceName()
				);
			return  l_pcNewFont;
		}
	}
	l_pcFont = wxTheFontList->FindOrCreateFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, FALSE);
	return l_pcFont;
}

wxFont*
SP_GPR_Fonts::GetItalicFont(const wxString& p_sNetClass, const wxString& p_sFontType) {
	wxFont* l_pcFont;
	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()
			&& m_lsFontTypeList.find(p_sFontType) != m_lsFontTypeList.end()) {

		wxString l_sKey = wxString::Format(wxT("%s|%s"), p_sNetClass.c_str(), p_sFontType.c_str());
		if (m_mFonts[l_sKey]->IsOk()) {
			l_pcFont = m_mFonts[l_sKey];
			wxFont* l_pcNewFont = wxTheFontList->FindOrCreateFont(
				l_pcFont->GetPointSize(),
				l_pcFont->GetFamily(),
				wxFONTSTYLE_ITALIC,
				l_pcFont->GetWeight(),
				l_pcFont->GetUnderlined(),
				l_pcFont->GetFaceName()
				);
			return  l_pcNewFont;
		}
	}
	l_pcFont = wxTheFontList->FindOrCreateFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false);
	return l_pcFont;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool
SP_GPR_Fonts::AddToDialogPage(const wxString& p_sNetClass, SP_WDG_NotebookPage *p_pcPage)
{
	wxSizer* l_pcSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer *l_pcSectionSizer;
	wxBoxSizer *helpSizer;
	SP_SetString::iterator it;

	// you never know...
	m_mtcFaceName.clear();
	m_mtcStyle.clear();
	m_mtcSize.clear();

	//add some space at top
	l_pcSizer->Add(0, 20, 0);

	//the following three net classes will appear like this...
	//you can add your new net class to this list,
	//or you can create a new else-if-branch if you need a different appearance
	if (!m_lsFontTypeList.empty()
			&& (p_sNetClass.CmpNoCase(SP_DS_PN_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_EXTPN_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_CONTINUOUSPED_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_SPN_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_COLSPN_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_COLEPN_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_COLPN_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_COLCPN_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_COLHPN_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_MODULOPN_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_REACHABILITY_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_FAULTTREE_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_EXTFAULTTREE_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_MTBDD_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_TIMEPN_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_MUSICPN_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_HYBRIDPN_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_MTIDD_CLASS) == 0
					|| p_sNetClass.CmpNoCase(SP_DS_FREESTYLE_CLASS) == 0)
	) {
		int i = 0;
		for( it = m_lsFontTypeList.begin(); it != m_lsFontTypeList.end(); ++it, i++) {
			const wxString l_rString = *it;
			wxString l_sKey = wxString::Format(wxT("%s|%s"), p_sNetClass.c_str(), l_rString.c_str());
			l_pcSectionSizer = new wxStaticBoxSizer(new wxStaticBox(p_pcPage, -1, wxString::Format(wxT("%s"), l_rString.c_str())), wxVERTICAL);
			helpSizer = new wxBoxSizer(wxHORIZONTAL);
			helpSizer->Add(new wxStaticText(p_pcPage, -1, wxT("Font:")), 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
			wxFont* l_pcFont = m_mFonts[l_sKey];
			m_mtcFaceName[*it] = new wxTextCtrl(p_pcPage, -1, l_pcFont->GetFaceName(),
																		wxDefaultPosition, wxDefaultSize,	wxTE_READONLY);
			helpSizer->Add(m_mtcFaceName[*it], 1, wxEXPAND);
			helpSizer->Add(new wxButton(p_pcPage, SP_ID_BUTTON_FONT + i, wxT("Select Font")), 0, wxRIGHT | wxLEFT, 5);
			l_pcSectionSizer->Add(helpSizer, 0, wxTOP | wxBOTTOM, 5);

			helpSizer = new wxBoxSizer(wxHORIZONTAL);
			helpSizer->Add(new wxStaticText(p_pcPage, -1, wxT("Style:")), 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
			m_mtcStyle[*it] = new wxTextCtrl(p_pcPage, -1, DetermineStyle(l_pcFont),
																							wxDefaultPosition, wxDefaultSize,	wxTE_READONLY);
			helpSizer->Add(m_mtcStyle[*it]);
			helpSizer->Add(new wxStaticText(p_pcPage, -1, wxT("Size:")), 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
			m_mtcSize[*it] = new wxTextCtrl(p_pcPage, -1, wxString::Format(wxT("%d"), l_pcFont->GetPointSize()),
																						wxDefaultPosition, wxSize(30, -1),	wxTE_READONLY);
			helpSizer->Add(m_mtcSize[*it]);

		  l_pcSectionSizer->Add(helpSizer, 0, wxTOP | wxBOTTOM, 5);
		  l_pcSizer->Add(l_pcSectionSizer, 0, wxEXPAND | wxALL, 5);
		}
	}	else /*DEFAULT*/ {
		l_pcSizer->Add(new wxStaticText(p_pcPage, -1, wxT("No editable font preferences.")), 0,
									wxALL | wxALIGN_CENTER_VERTICAL, 5);
	}

	p_pcPage->AddControl(l_pcSizer,0,wxEXPAND);

	return true;
}

wxString
SP_GPR_Fonts::DetermineStyle(wxFont *p_Font) {
	if (p_Font->GetWeight()==wxFONTWEIGHT_BOLD && p_Font->GetStyle()==wxFONTSTYLE_ITALIC) {
		return wxT("Bold-Italic");
	} else if (p_Font->GetWeight()==wxFONTWEIGHT_BOLD) {
		return wxT("Bold");
	} else if (p_Font->GetStyle()==wxFONTSTYLE_ITALIC) {
		return wxT("Italic");
	} else {
		return wxT("Normal");
	}
}

bool
SP_GPR_Fonts::OnDialogOk(const wxString& p_sNetClass) {

	SP_MDI_Doc  *l_cDoc = SP_Core::Instance()->GetRootDocument();
	SP_DS_Graph *l_cGraph;
	SP_ListGraphic l_cGraphics, *l_cChildGraphics;
	SP_ListGraphic::iterator it, itc;
	wxString l_sClass;

	if (l_cDoc) {
		if (l_cDoc->GetNetnumber())
		{
			//getting nodes and edges of the current graph
			l_cGraph = l_cDoc->GetGraph();
			l_cGraph->GetGraphicsInNet(&l_cGraphics, l_cDoc->GetNetnumber(), SP_ELEMENT_NODE | SP_ELEMENT_EDGE | SP_ELEMENT_METADATA);
			for(it = l_cGraphics.begin(); it!=l_cGraphics.end(); ++it)
			{
				//getting attributes of nodes and edges

				// GetGraphicChildren can give a Null Pointer!!!
				// Snoopy crashes without this if
				// Maybe there is another bug, while  you get NULL here
				if ((*it)->GetGraphicChildren())
				{
					l_cChildGraphics = (*it)->GetGraphicChildren();
					for(itc = l_cChildGraphics->begin(); itc != l_cChildGraphics->end(); ++itc)
					{
						if(dynamic_cast<SP_DS_Node*>((*it)->GetParent()))
							l_sClass = dynamic_cast<SP_DS_Node*>((*it)->GetParent())->GetClassName();
						else
							l_sClass = wxT("");
						//update fonts of attributes and recalculate font dimensions
						if(SP_GR_BaseText* l_pcGR_Text = dynamic_cast<SP_GR_BaseText*>((*itc)->GetPrimitive()))
						{
							if (l_sClass.CmpNoCase(wxT("Comment")) == 0)
							{
								l_pcGR_Text->SetSelectFont(GetSelectFont(l_cGraph->GetNetclass()->GetName(), wxT("Comments")));
								l_pcGR_Text->SetNormalFont(GetNormalFont(l_cGraph->GetNetclass()->GetName(), wxT("Comments")));
							}
							else
							{
								l_pcGR_Text->SetSelectFont(GetSelectFont(l_cGraph->GetNetclass()->GetName(), wxT("Default")));
								l_pcGR_Text->SetNormalFont(GetNormalFont(l_cGraph->GetNetclass()->GetName(), wxT("Default")));
							}
							(*itc)->Update(true);
						}
					}
				}
			}
		}
	}



	return true;
}

void
SP_GPR_Fonts::ChooseFont(wxWindow* parent, const wxString& p_sNetClass, int p_nId) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		for(SP_SetString::iterator it = m_lsFontTypeList.begin(); it != m_lsFontTypeList.end(); ++it, p_nId--) {
			const wxString l_rString = *it;
			if (p_nId == 0) {
				wxString l_sKey = wxString::Format(wxT("%s|%s"), p_sNetClass.c_str(), l_rString.c_str());
				wxFont font = wxGetFontFromUser(parent, *m_mFonts[l_sKey]);
				SetFont(p_sNetClass, *it, &font);
				m_mtcFaceName[*it]->SetValue(m_mFonts[l_sKey]->GetFaceName());
				m_mtcStyle[*it]->SetValue(DetermineStyle(m_mFonts[l_sKey]));
				m_mtcSize[*it]->SetValue(wxString::Format(wxT("%d"), m_mFonts[l_sKey]->GetPointSize()));
				break;
			}
		}
	}
}

bool
SP_GPR_Fonts::UpdateFromConfig(wxConfig& p_config)
{
	wxString description;
	SP_SetString::iterator it, itNC;

	int l_nCount = SP_Core::Instance()->GetNetclassCount();
	for (int i = 0; i < l_nCount; i++)
	{
		m_lsNCNames.insert(SP_Core::Instance()->GetNetclassName(i));
	}

	for (itNC = m_lsNCNames.begin(); itNC != m_lsNCNames.end(); ++itNC)
	{
		const wxString l_rNCString = *itNC;
		for (it = m_lsFontTypeList.begin(); it != m_lsFontTypeList.end(); ++it)
		{
			const wxString l_rString = *it;
			if (p_config.Read(wxString::Format(wxT("%s//Font_%s"), l_rNCString.c_str(), l_rString.c_str()), &description))
			{
				wxFont font(description);
				SetFont(*itNC, *it, &font);
			}
			else
			{
				SetFont(*itNC, *it, NULL);
			}
		}
	}
	return true;
}

bool
SP_GPR_Fonts::WriteToConfig(wxConfig& p_config) {
	SP_SetString::iterator it, itNC;

	for (itNC = m_lsNCNames.begin(); itNC != m_lsNCNames.end(); ++itNC) {
		const wxString l_rNCString = *itNC;
		for(it = m_lsFontTypeList.begin(); it != m_lsFontTypeList.end(); ++it) {
			const wxString l_rString = *it;
			wxString l_sKey = wxString::Format(wxT("%s|%s"), l_rNCString.c_str(), l_rString.c_str());
			wxFont* l_pcFont = m_mFonts[l_sKey];
			if (l_pcFont->IsOk() )
			p_config.Write(wxString::Format(wxT("%s//Font_%s"), l_rNCString.c_str(), l_rString.c_str()),
				l_pcFont->GetNativeFontInfoDesc());
		}
	}
	return true;
}
