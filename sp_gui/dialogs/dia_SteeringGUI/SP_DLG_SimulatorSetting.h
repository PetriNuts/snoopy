/*
 * SP_DLG_SimulatorSetting.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 19.10.2011
 * Short Description:
 *///==============================================================================/

#ifndef SP_DLG_SIMULATORSETTING_H_
#define SP_DLG_SIMULATORSETTING_H_

#include <wx/wx.h>
#include "spsa/spsa.h"

class SP_DLG_SimulatorSetting: public wxDialog
{
private:
	     std::vector<wxControl*> m_apcPropertiesCtrl;

	     //client
	     spsa::SteeringClient* m_pcGUIClient;

	     spsa::SimulatorAttribute* m_pcSimulatorAttribute;

	     wxSizer* m_pcSizer;

	     bool m_bInitialised;

	     wxString m_sSimulatorName;
protected:

	      bool CreateCtrls();

	      void OnOK(wxCommandEvent& WXUNUSED(event));

	      void DoOK();

	      DECLARE_EVENT_TABLE();
public:


	      SP_DLG_SimulatorSetting(wxWindow* p_pcParentWnd,spsa::SteeringClient* p_pcGUIClient,const wxString& p_sSimName);
	      virtual ~SP_DLG_SimulatorSetting();

	      bool IsInitialised(){return m_bInitialised;}
};

#endif /* SP_DLG_SIMULATORSETTING_H_ */
