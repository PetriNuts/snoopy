/*
 * SP_GUI_ConnectToServer.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 08.03.2011
 * Short Description:
 ==============================================================================*/

#ifndef SP_GUI_CONNECTTOSERVER_H_
#define SP_GUI_CONNECTTOSERVER_H_


#include <wx/wx.h>
#include <wx/gauge.h>
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "spsa/spsa.h"
/*
 * try to connect to the server
 */

class SP_GUI_ConnectToServer :  public wxFrame, public SP_Error
{
private:
	   wxButton* m_pcConnectButtonCtrl;
	   wxButton* m_pcCloseButtonCtrl;
	   wxButton* m_pcPropertiesButtonCtrl;

	   wxGauge* m_pcIndicatorGaugeCtrl;

	   //Connection Details
	   wxStaticText* m_pcPortStaticTxtCtrl;
	   wxStaticText* m_pcIPStaticTxtCtrl;

	   wxStaticText* m_pcStatusTxtCtrl;

	   wxComboBox* m_pcPortComboCtrl;
	   wxComboBox* m_pcIPComboCtrl;

	   wxBoxSizer* m_pcPropertiesSize;

	   wxString m_sPort;
	   wxString m_sIP;

	   SP_DS_Graph* m_pcGraph;

	   spsa::SteeringClient* m_pcGUIClient;

	   SP_MapString2Double m_msParamName2Values;

protected:

		  void OnConnect(wxCommandEvent& event);
		  void OnConnectionProperties(wxCommandEvent& event);
		  void OnClose(wxCommandEvent& event);

		  bool ConnectToServer();

		  void DisableButtons();
		  void EnableButtons();

	      spsa::Model* CreateModel(const wxString& p_sModelName);

		  //create colored PN model
		  spsa::Model* CreateColoredModel(const wxString& p_sModelName);

		  SP_VectorString ToWxStringVector(SP_VectorStdString p_vsource);//by george for spsm

		  unsigned long LoadNodeNames(const wxString& p_sNodeName,SP_VectorString& p_asNames);

		  void loadInitialMarking(const wxString& p_sNodeName,SP_VectorDouble& p_anInitialMarking);

		  void LoadConnection(const wxString& p_sArcName,unsigned int& p_nIndex,spsa::Model* p_pcModel);

		  void LoadTransitionFunctions(const wxString& l_pcTransitionName,const unsigned int& p_nAttributeIndex,SP_VectorString& p_asFunctions);

		  void LoadParameters(SP_VectorString& p_asParameterNames,SP_VectorDouble& p_anParametervalues);

		  //for nets which use constants
		  void LoadConstants(SP_VectorString& p_asConstantNames,SP_VectorDouble& p_anConstantValues);

		  spsa::ConnectionType GetConnectionType(const wxString& p_sConnectionType);

		  double GetEdgeWeight(SP_DS_Edge* p_pcEdge);

		  //load all views
		  bool LoadModelViews(spsa::Model* p_pcModel);

		  //load one view
		  spsa::ModelView* LoadView(SP_DS_Metadata* p_pcMetadata);

		  wxString GetViewAttributeValue(SP_DS_Metadata* p_pcView, const wxString& p_sAttributeName);

		  void LoadCurveInfo(const unsigned int &p_nCurvePos,
						    spsa::ModelView* p_pcView,
							SP_DS_Metadata* p_pcMetadata,
						    SP_DS_ColListAttribute* p_pcCurveInfoList);

		  //load the view attributes
		  void LoadViewAttributes(spsa::ModelView* p_pcView,SP_DS_Metadata* p_pcMetadata);

		  //load simulation properties
		  void LoadSimulationProperties(spsa::Model* p_pcModel);

		  void UpdateConfig();

		  void ReadConfig();

		  //get net type given its extension
		  spsa::NET_TYPE GetNetType(const wxString& p_sNetExt);

public:
		SP_GUI_ConnectToServer(wxWindow* p_pcParent,const wxString& p_sTitle=wxT("Connect to the server"),long p_nStyle=wxDEFAULT_DIALOG_STYLE);
		virtual ~SP_GUI_ConnectToServer();

	    DECLARE_EVENT_TABLE()
};

#endif /* SP_GUI_CONNECTTOSERVER_H_ */
