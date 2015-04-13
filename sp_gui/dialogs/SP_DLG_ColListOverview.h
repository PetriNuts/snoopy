//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: crohr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2008/06/28 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_COLLISTOVERVIEW_H__
#define __SP_DLG_COLLISTOVERVIEW_H__

#include <wx/wx.h>
#include <wx/grid.h>
#include "sp_core/SP_Vector.h"

#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

typedef vector<SP_DS_ColListAttribute*> SP_VectorColListAttribute;
typedef vector<SP_DS_Node*> SP_VectorNode;

class SP_DLG_ColListOverview : public wxDialog
{

private:
	SP_COLLIST_TYPE m_eColListType;
	wxString m_sAttrName;
	wxString m_sOldCellValue;
	wxWindow* m_pcParent;
	SP_VectorColListAttribute m_vAttributes;
	SP_VectorNode m_vNodes;
	wxBoxSizer* m_pcSizer;

	wxGrid* m_pcValueGrid;

	const wxChar m_chSeperator;

	void LoadData();
	void SaveData();
	bool ExistName(const wxString&);
	void RefreshFrame();

protected:

public:

	SP_DLG_ColListOverview(const wxString& p_sAttrName, wxWindow* p_pcParent,
			const wxString& p_sTitle = wxT("Lists overview"),
			long p_nStyle = wxDEFAULT_DIALOG_STYLE);

	void OnDlgOk(wxCommandEvent& p_cEvent);
	void OnDlgCancel(wxCommandEvent& p_cEvent);

	void OnAddSet(wxCommandEvent& p_cEvent);
	void OnDeleteSet(wxCommandEvent& p_cEvent);
	void OnRenameSet(wxCommandEvent& p_cEvent);

	void OnCellValueChanged(wxGridEvent& ev);
	void OnGridCellSelected(wxGridEvent& ev);

DECLARE_CLASS( SP_DLG_ColListOverview )
DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_COLLISTOVERVIEW_H__
