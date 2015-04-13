//////////////////////////////////////////////////////////////////////
// $Source: SP_GPR_Fonts.h
// $Author: MD $
// $Version: $
// $Revision: $
// $Date: 2005/09/19 $
// Short Description: contains global fonts preferences
//////////////////////////////////////////////////////////////////////

#if !defined(__SP_GPR_FONTS_H__)
#define __SP_GPR_FONTS_H__

#include "sp_core/SP_GPR_Base.h"

class SP_DLG_GlobalPreferences;


class SP_GPR_Fonts: public SP_GPR_Base {

private:

	//the list of font categories, e.g. "comments", "default", ...
	SP_SetString m_lsFontTypeList;
	//returns style (bold, italic, bold-italic, normal) for given font
	wxString DetermineStyle(wxFont *p_Font);

protected:

	//the mapping key is a combination of net class and the font type
	//such as: "<net class>|<font type>"
	map<wxString, wxFont*> m_mFonts;

	//dialog controls
	//the mapping key is the font type
	map<wxString, wxTextCtrl*> m_mtcFaceName, m_mtcStyle, m_mtcSize;

public:
	SP_GPR_Fonts();
	virtual ~SP_GPR_Fonts();

	//setter and getter
	void SetFont(const wxString& p_sNetClass, const wxString& p_sFontType, wxFont *p_Font);
	wxFont* GetNormalFont(const wxString& p_sNetClass, const wxString& p_sFontType);
	wxFont* GetSelectFont(const wxString& p_sNetClass, const wxString& p_sFontType);
	wxFont* GetItalicFont(const wxString& p_sNetClass, const wxString& p_sFontType);

	bool AddToDialogPage(const wxString& p_sNetClass, SP_WDG_NotebookPage *p_pcPage);
	//calls the font selection dialog
	//and updates dialog controls that show selected font attributes
	void ChooseFont(wxWindow* parent, const wxString& p_sNetClass, int p_nFontTypeID);
	bool OnDialogOk(const wxString& p_sNetClass);

	bool UpdateFromConfig(wxConfig& p_config);
	bool WriteToConfig(wxConfig& p_config);

};

#endif // !defined(__SP_GPR_FONTS_H__)
