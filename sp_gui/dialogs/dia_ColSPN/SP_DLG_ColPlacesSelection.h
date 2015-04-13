//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2011/08/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_ColPlacesSelection_H__
#define __SP_DLG_ColPlacesSelection_H__

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/listbox.h>
#include <wx/event.h>

#include "sp_gui/dialogs/dia_ColSPN/SP_DLG_PlacesSelection.h"
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"




class SP_DLG_ColPlacesSelection : public SP_DLG_PlacesSelection
{

private:

	wxString m_sNetClass;
	
	SP_DS_ColPN_Unfolding* m_pcUnfoldedNet;
	SP_DLG_Simulation* m_pcColSimulationResults;

	vector<wxString>* m_pvUnfoldedPlace;
	vector<wxString>* m_pvColoredPlace;
	vector<wxString>* m_pvUnfoldedTrans;
	vector<wxString>* m_pvColoredTrans;

	map<wxString,long> m_mColPlaceName2ID;
	map<wxString,long> m_mColTransName2ID;


  
public:
	
    SP_DLG_ColPlacesSelection(SP_DLG_Simulation* p_pcColSimulationResults, wxString p_sNodeType, SP_DS_Metadata* p_pcEditMetadata, wxWindow* p_pcParent,
    const wxString& p_sTitle = wxT("Edit"),
    long p_nStyle = wxDEFAULT_DIALOG_STYLE );

public:
	virtual void GetSelResults();

protected:
	
	virtual void LoadData();
	
	virtual void OnColPlaceSel( wxCommandEvent& p_cEvent );

	virtual void OnEditAuxVariable(wxCommandEvent& p_cEvent );

	virtual void OnChangedOutType(wxCommandEvent& p_cEvent);
	virtual void Initialize();
	
   virtual void AddtoCurLBInStrings(wxString p_sName);
	
	virtual void OnChangedPlaceTransition(wxCommandEvent& p_cEvent);

	DECLARE_CLASS( SP_DLG_ColPlacesSelection )
	DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_ColPlacesSelection_H__

