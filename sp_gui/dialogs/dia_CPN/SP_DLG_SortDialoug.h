/*
*@Author: George Assaf *
*@DATE: 29/04/2021*
*@REVESION:0.1*
*@DESCRIPTION: Sorting table of data based on multi columns*
*/

#ifndef SP_DLG_SORT_CONSTANTS_H_
#define SP_DLG_SORT_CONSTANTS_H_

#include <wx/wx.h>
#include "sp_utilities.h"

class SP_DLG_SortDialoug : public wxDialog
{
private:
	SP_VectorString m_vKeys;
	std::map<wxString, bool> m_mKey2SortType;
	wxWindow* m_pcParent;

	wxBoxSizer* m_pcSizer;
	vector<wxChoice*> m_pcKey;
	vector<wxRadioBox*> m_pcRadioGroups;
	int m_nMainSetIndex;
	SP_Vector2DString m_DataTable;
	std::map<wxString, int> m_mKey2Pos;


private:
	void OnDlgOk(wxCommandEvent& p_cEvent);
	void SortBasedOneColumn(int key,bool p_bIsAscending, SP_Vector2DString& p_vvTable);
	void SortBasedTwoColumns(int key1,int key2,bool p_bKey1SortType, bool p_bKey2SortType, SP_Vector2DString& p_vvTable);
	void MakeUniqueVector(std::vector<string> p_v1, std::vector<string>& p_vres);
	void SortBasedThreColumns(int key1, int key2, int key3, bool bsort1, bool bsort2, bool bsort3, SP_Vector2DString& p_vvTable);
public:
	SP_DLG_SortDialoug(wxWindow *p_pcParent, SP_VectorString p_vKeys, std::map<wxString, int> p_mk2p,SP_Vector2DString p_vvDataTable,int p_nMainSetIndex, const wxString& p_sTitle = wxT("Sorting Dialog"),
		long p_nStyle = wxDEFAULT_DIALOG_STYLE);
	inline SP_Vector2DString GetSortedTable() { return m_DataTable; }
	
	DECLARE_CLASS(SP_DLG_SortDialoug)
	DECLARE_EVENT_TABLE()
	 
	//SP_DLG_SortDialoug(wxWindow* p_pcParent,
	//	const wxString& p_sTitle = wxT("Sorting Dialog"),
	//	long p_nStyle = wxDEFAULT_DIALOG_STYLE);
};

#endif /* SP_DLG_SORT_CONSTANTS_H_*/