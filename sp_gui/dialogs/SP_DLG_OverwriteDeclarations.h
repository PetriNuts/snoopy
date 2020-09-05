/*
*@author:George Assaf
*@Date:28.08.2020
*@Description:Overwrite redundant declarations when doing import CANDL/ANDL
*/

#ifndef __SP_DLG_OVERWRITEDEC__
#define __SP_DLG_OVERWRITEDEC__
#include <wx/wx.h>
#include "wx/wx.h"
#include <wx/event.h>
#include <map>
#include <set>
class SP_DLG_OverwriteDeclarations :public wxDialog
{
private:
	//sizers
	wxSizer* m_pcMainSizer;
	wxSizer* m_pcLeftSizer;
	wxSizer* m_pcMiddleSizer;
	wxSizer* m_pcRightSizer;
	wxSizer* m_pcBottomsizer;
	wxBoxSizer* m_pcSizer;
private:
	//list boxes
	wxListBox* m_pcLeftList;
	wxListBox* m_pcRightLsit;

private:
	//buttons
	wxButton* m_pcButton1;
	wxButton* m_pcButton2;
	wxButton* m_pcButtonAllFront;
	wxButton* m_pcButtonAllBack;

	void OnSingleFront(wxCommandEvent& p_event);
	void OnSingleBack(wxCommandEvent& p_event);
	void OnSingleFrontAll(wxCommandEvent& p_event);
	void OnSingleBackAll(wxCommandEvent& p_event);
private:
	std::map<wxString,std::set< wxString>> m_mType2Declaration;
	std::map<wxString, std::set< wxString>> m_mSelectedType2Declaration;
	void initialise();
	void StoreCurrentSelectedChoices();
public:
	std::map<wxString, std::set< wxString>> GetSelectedItems() { return m_mSelectedType2Declaration; }
public:
	SP_DLG_OverwriteDeclarations(wxWindow *p_pcParent,const wxString& p_sTitle,const std::map<wxString,std::set<wxString>>& p_mDecMap);

	DECLARE_EVENT_TABLE()
};

#endif __SP_DLG_OVERWRITEDEC__
