//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/12/03 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_SIMULATIONPROPERTIES__
#define __SP_DLG_SIMULATIONPROPERTIES__

#include <wx/wx.h>
class SP_DS_Simulation;
#include"spsim/simulator.h"
class SP_DLG_SimulationProperties: public wxDialog
{
private:
    DECLARE_EVENT_TABLE()
    //! the top level sizer
    wxBoxSizer* m_pcSizer;

    spsim::Simulator* m_pcMainSimulator;

    std::vector<wxControl*> m_apcPropertiesCtrl;

	unsigned m_nSimAlgo;//for stoch sim properties bug when changing the sim algo, by george
    DECLARE_CLASS(SP_DLG_SimulationProperties)
protected:

     virtual void SetProperties();
     virtual void GetProperties();

public:
      SP_DLG_SimulationProperties(
    	spsim::Simulator* p_pcMainSimulator,wxWindow* p_pcParent,const int& p_nSimAlgo=0,
        const wxString& p_sTitle = wxT("Simulation Properties"),
        long p_nStyle = wxDEFAULT_DIALOG_STYLE);

    /* EVENTS */
    /**	\brief	The OnDlgOk function

	    \param	p_cEvent	the event object as generated by wx'

	    This method is called, if the user clicks the 'OK' button
    */
    void OnDlgOk(wxCommandEvent& p_cEvent);
    /**	\brief	The OnDlgCancel function

	    \param	p_cEvent	the event object as generated by wx'

	    This method is called, if the user clicks the 'Cancel' button, pushes ESC
        or closes the dialog with the window-close methods of the ui-manager.
    */
    void OnDlgCancel(wxCommandEvent& p_cEvent);
};

#endif // __SP_DLG_SIMULATIONPROPERTIES__

