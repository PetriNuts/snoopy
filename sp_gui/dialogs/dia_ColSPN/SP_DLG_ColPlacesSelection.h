//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2011/08/11 11:20:00 $
// $Modified by:GEorge Assaf$
// $Modification Date: 02.02.2021$
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

	vector<std::string>* m_pvUnfoldedPlace;
	vector<std::string>* m_pvColoredPlace;
	vector<std::string>* m_pvUnfoldedTrans;
	vector<std::string>* m_pvColoredTrans;


	//by george
	vector<wxString>* m_pvUnfoldedPlaceObserver;
	vector<wxString>* m_pvColoredPlaceObserver;
	vector<wxString>* m_pvUnfoldedTransObserver;
	vector<wxString>* m_pvColoredTransObserver;
	vector<wxString>* m_pvMixedObserver;//observers over anything


	//by george for obserevrs
	vector<wxString> m_bUnfoldedTransObservers;
	vector<wxString> m_bUnfoldedPlacesObservers;

	map<wxString,long> m_mColPlaceName2ID;
	map<wxString,long> m_mColTransName2ID;

	//by george 9.2.21
	map<wxString, long> m_mColPlaceObserverName2ID;
	map<wxString, long> m_mColTransObserverName2ID;

  
public:
	
    SP_DLG_ColPlacesSelection(SP_DLG_Simulation* p_pcColSimulationResults, SP_DS_Metadata* p_pcEditMetadata, wxWindow* p_pcParent,
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

