/*
 * SP_DLG_SelectXAxisVariable.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 24.07.2011
 * Short Description:
 */
//=======================================================
#ifndef SP_GUI_SELECTZAXISVARIABLE_H_
#define SP_GUI_SELECTZAXISVARIABLE_H_

#include <wx/wx.h>
#include <map>
#include "sp_core/SP_Vector.h"

typedef std::map<wxString, SP_VectorStdString*> SP_MapString2VectorString;

class SP_DLG_SelectXAxisVariable: public wxDialog
{
private:
	wxChoice* m_pcVariableTypeBox;

	wxChoice* m_pcVariableNameBox;

	wxString m_sSelectedCategory;
	wxString m_sSelectedSubCategory;

	SP_MapString2VectorString m_mChoices;

protected:
	void AddItemNames();

	void OnVariableTypeChanged(wxCommandEvent& WXUNUSED(event));
public:
	SP_DLG_SelectXAxisVariable(wxWindow* p_pcParent, const SP_MapString2VectorString& p_mChoices, const wxString& p_sCurrentSelectedCategory, const wxString& p_sCurrentSelectedSubCategory);
	~SP_DLG_SelectXAxisVariable();

	wxString GetSelectedCategory()
	{
		return m_sSelectedCategory;
	}
	wxString GetSelectedSubCategory()
	{
		return m_sSelectedSubCategory;
	}

	void OnVariableNameChanged(wxCommandEvent& WXUNUSED(event));
};
#endif /* SP_GUI_STEERINGDIALOG_H_ */
