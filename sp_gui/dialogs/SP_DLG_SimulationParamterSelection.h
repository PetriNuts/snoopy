//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/08/29 11:20:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_SIMULATIONPARAMETERSELECTION__
#define __SP_DLG_SIMULATIONPARAMETERSELECTION__

#include <wx/wx.h>
class SP_DS_ContinuousPedAnimation;
class SP_DS_Animation;

class SP_DLG_SimulationParamterSelection: public wxDialog
{
private:
  DECLARE_EVENT_TABLE()
  //! the top level sizer
  wxBoxSizer* m_pcSizer;
  wxStaticText* m_pcError;
  SP_DS_ContinuousPedAnimation* m_pcAnimation;

  DECLARE_CLASS(SP_DLG_SimulationParamterSelection)
protected:
public:
  SP_DLG_SimulationParamterSelection(SP_DS_Animation* p_pcAnim,
                                     wxWindow* p_pcParent,
                                     const wxString& p_sTitle = wxT("Simulation Parameter Selection"),
                                     long p_nStyle = wxDEFAULT_DIALOG_STYLE);

  /* EVENTS */
  /** \brief	The OnDlgOk function
      \param	p_cEvent	the event object as generated by wx'
      This method is called, if the user clicks the 'OK' button
   */
  void OnDlgOk(wxCommandEvent& p_cEvent);
  /** \brief	The OnDlgCancel function
      \param	p_cEvent	the event object as generated by wx'
      This method is called, if the user clicks the 'Cancel' button, pushes ESC
      or closes the dialog with the window-close methods of the ui-manager.
   */
  void OnDlgCancel(wxCommandEvent& p_cEvent);

  void OnDlgAddCol(wxCommandEvent& p_cEvent);

};

#endif // __SP_DLG_SIMULATIONPARAMETERSELECTION__

