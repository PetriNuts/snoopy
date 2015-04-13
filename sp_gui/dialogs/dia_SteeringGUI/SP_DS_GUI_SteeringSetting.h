/*
 * SP_DS_GUI_SteeringSetting.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 25.06.2011
 * Short Description:
 *///=================================================

#ifndef SP_DS_GUI_STEERINGSETTING_H_
#define SP_DS_GUI_STEERINGSETTING_H_

#include<wx/wx.h>
#include<wx/string.h>

class SP_DS_GUI_SteeringSetting: public wxDialog
{
	      bool m_bRefreshResult;
	      bool m_bRefreshRuntime;

	      double m_nRefreshResultDuration;
	      double m_nRefreshRuntimeDuration;

	      int m_nTimeout;

	      bool m_bEnableRefreshCalculationTime;

	      double m_nSteeringConstant;

	      //checkboxes
	      wxCheckBox* m_pcRefreshResultChkBox;
	      wxCheckBox* m_pcRefreshRuntimeChkBox;

	      wxCheckBox* m_pcEnableRefreshCalculationTimeChkBox;

	      //sliderbars
	      wxSlider* m_pcRefreshResultSlider;
	      wxSlider* m_pcRefreshRuntimeSlider;

	      wxSlider* m_pcTimeoutSlider;

	      wxTextCtrl* m_pcSteeringConstant;
protected:
	      void OnOK(wxCommandEvent& WXUNUSED(event));
public:
		  SP_DS_GUI_SteeringSetting(wxWindow* p_pcParent);
		  virtual ~SP_DS_GUI_SteeringSetting();

		  //bool states
		  void SetRefreshResult(const bool& p_bRefresh){m_bRefreshResult=p_bRefresh;}
		  bool GetRefreshResult(){return m_bRefreshResult;}

		  void SetRefreshRuntime(const bool& p_bRefresh){m_bRefreshRuntime=p_bRefresh;}
		  bool GetRefreshRuntime(){return m_bRefreshRuntime;}

		  //durations
		  void SetResultRefreshDuration(const double& p_nValue){m_nRefreshResultDuration=p_nValue;}
		  double GetResultRefreshDuration(){return m_nRefreshResultDuration;}

		  void SetRuntimeRefreshDuration(const double& p_nValue){m_nRefreshRuntimeDuration=p_nValue;}
		  double GetRuntimeRefreshDuration(){return m_nRefreshRuntimeDuration;}


		  int GetTimeout(){return m_nTimeout;}
		  void SetTimeOut(const int& p_nTimeout){m_nTimeout=p_nTimeout;}

		  //Enable Refresh Calculation Time
		  bool GetEnableRefreshCalculationTime(){return m_bEnableRefreshCalculationTime;}
		  void SetEnableRefreshCalculationTime(const bool& p_bState){m_bEnableRefreshCalculationTime=p_bState;}

		  //steering constant
		  double GetSteeringConstant(){return m_nSteeringConstant;}
		  void SetSteeringConstant(const double& p_nValue){m_nSteeringConstant=p_nValue;}

		  void Update();

		  DECLARE_EVENT_TABLE()
};

#endif /* SP_DS_GUI_STEERINGSETTING_H_ */
