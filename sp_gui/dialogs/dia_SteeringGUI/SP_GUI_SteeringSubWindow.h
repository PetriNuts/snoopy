/*
 * SP_GUI_SteeringSubWindow.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 06.01.2012
 * Short Description:
 */
//=================================================

#ifndef SP_GUI_STEERINGSUBWINDOW_H_
#define SP_GUI_STEERINGSUBWINDOW_H_

#include "wx/wx.h"
#include "spsa/spsa.h"

#include"sp_gui/widgets/dialogs/SP_WDG_SpinCtrlDouble.h"

class SP_GUI_SteeringSubWindow: public wxScrolledWindow
{
private:
	       unsigned long m_nSlideCount;

	       //editing place or parameters
	       wxString m_sSubWindowType;

	       double m_nSpinIncrement;
protected:
	       wxSizer* l_pcMainSizer;

	      //steering client to communicate with the server
	      spsa::SteeringClient* m_pcGUIClient;

	      //list of steering variables
	      std::vector<SP_WDG_SpinCtrlDouble*> m_apcSteeringVariables;

		  spsa::VectorULong m_anSelectedItems;
protected:
	      
	      void OnSpinValueChanged(wxSpinEvent & event);

	      //command event handler
	      void OnEnterPressed(wxCommandEvent& event);

	      //show steered items on the window
	      void ShowSteeredItems(const SP_VectorString& p_asNames,
	    		                const SP_VectorDouble&  p_anCurrentValues);

public:
	      SP_GUI_SteeringSubWindow(wxWindow* p_pcParent,spsa::SteeringClient* p_pcGUIClient, const wxString& p_sWindowType);
	      virtual ~SP_GUI_SteeringSubWindow();

	      void AddVariable(const wxString& p_sName,const double& p_nCurrentValue,
	    		          const double& p_nMinValue,const double& p_nMaxValue);

	      void OnSelectItemToEdit();
	      
	      void ChangeValue(const long& p_nPosition,const double& p_nValue);
	      
	      void RefreshValues();

	      void SaveSteeredPositions();

	      void LoadSteeredPositions();

	      void SetSpinIncrement(const double& p_nIncrement){m_nSpinIncrement=p_nIncrement;}
	      double GetSpinIncrement(){return m_nSpinIncrement;}

	      DECLARE_EVENT_TABLE();
};

#endif /* SP_GUI_STEERINGSUBWINDOW_H_ */
