//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.40 $
// $Date: 2007/08/14 11:20:00 $
/* @change M.Herajy 5.6.2010
 * @Description
 * Defines an abstract class for simulation's result dialog. It includes minimum GUI elements
 * each simulation result dialog should inherited from this class
 */
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_SIMULATION__
#define __SP_DLG_SIMULATION__

#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/checklst.h>
#include <wx/treectrl.h>
#include <wx/bmpbuttn.h>
#include <wx/listctrl.h>
#include <wx/txtstrm.h>

#include <wx/wfstream.h>
#include <wx/file.h>

#include "sp_ds/SP_DS_Node.h"
#include "sp_core/SP_Map.h"
#include "sp_core/SP_Vector.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include "spsim/simulator.h"

#include "sp_ds/extensions/ResultViewer/SP_DS_ResultViewer.h"
#include "sp_gui/dialogs/SP_DLG_SelectXAxisVariable.h"
#include "sp_gui/dialogs/SP_DLG_BaseSimulation.h"
#include <wx/collpane.h>

typedef map<long, wxColour> SP_MapLong2Colour;

/*
 * Constants for different export types
 */
typedef enum
{
	SP_SIM_NO_EXPORT,
	SP_SIM_DIRECT_EXPORT,
	SP_SIM_DIRECT_SINGLE_EXPORT,
	SP_SIM_DIRECT_SINGLE_EXACT_EXPORT,
	SP_SIM_EXPORT_SNOOPY_FILE
} SP_ExportType;

class SP_DS_Simulation;
class SP_DS_ResultViewer;
class SP_DLG_ShowAllModelView;

class SP_DLG_Simulation: public SP_DLG_BaseSimulation
{
protected:
	// Current Graph
	SP_DS_Graph* m_pcGraph;
	long m_nRefreshRate;

	//New Simulator
	spsim::Simulator* m_pcMainSimulator;

	//Number of result points
	unsigned long m_nResultPointCount;
	bool m_bSimThreadCreated;

	vector<wxComboBox*> m_apcComboBoxes;
	vector<SP_DS_ColListAttribute*> m_apcColListAttr;

	SP_ExportType m_ExportType;

	//Export file
	wxString m_sExportFilename;
	wxFileOutputStream* m_pcExportFileOutputStream;
	wxBufferedOutputStream* m_pcExportBufferdOutputStream;
	wxTextOutputStream* m_pcExport;
	wxString m_sLastResultExport;
	int m_nExportSpacer;

	double m_nPlotYMax;

	SP_DS_Metadata* m_pcCurrentTablePlot;

	wxColour* m_apColours;

	SP_MapLong2Colour m_msPlacePos2Colour;

	wxGauge* m_pcSimulationProgressGauge;
	bool m_bCompressExact;
	wxString m_sNodeclass;
	bool m_bSumOfNodes;

	//ApFormulae used with SPN
	wxString m_sApFormulae;

	//a vector to store the x axis values
	SP_VectorDouble m_anXAxisValues;

	SP_MapString2VectorString m_pcXAxisChoices;

	//for control automatically view change through external window
	bool m_bCanChangeCurrentView;

	SP_SetString m_choices;

	void UpdateChoices();

	void StartSimulation();
	virtual void DoStartSimulation() = 0;

protected:
	wxStopWatch m_cSimulationStopWatch;
	//Export types Controls
	wxRadioButton* m_pcNoExportRadioButton;
	wxRadioButton* m_pcDirectExportRadioButton;
	wxRadioButton* m_pcDirectSingleExportRadioButton;
	wxRadioButton* m_pcDirectSingleExactExportRadioButton;

/*	//Input parameters for simulation
	wxTextCtrl* m_pcIntervalStartTextCtrl;
	wxTextCtrl* m_pcIntervalEndTextCtrl;
	wxTextCtrl* m_pcResultPointCountTextCtrl;*/

	//Simulation status controls
	wxStaticText* m_pcPlotStatusText;
	wxStaticText* m_pcSimulationStopWatch;

	wxCheckBox* m_pcImmediatelyOutputCheckBox;

//	wxBoxSizer* m_pcMainSizer;

	/*wxSizer* m_pcPropertySizer;
	wxSizer* m_pcSetsSizer;
	wxSizer* m_pcDirectExportSizer;
	wxSizer* m_pcSimulationButtonSizer;
	wxSizer* m_pcModelViewsSizer;*/

	bool m_nlayoutAdaptaion;


	wxComboBox* m_pcShowGraphViewBox;

	/*wxWindow *m_pcPropertyWindowSimulationButtonSizer;
	wxWindow *m_pcPropertyWindowPropertySizer;
	wxWindow *m_pcPropertyWindowDirectExportSizer;
	wxWindow *m_pcPropertyWindowSetsSizer;

	wxCollapsiblePane *m_pcCollpaneDirectExportSizer;
	wxCollapsiblePane *m_pcCollpanePropertySizer;
	wxCollapsiblePane *m_pcCollpaneSetsSizer;*/

	wxListBox *m_pcListboxShowAllGraphViewName;

	wxDialogLayoutAdapter *m_pLayoutAdapter;





	wxStaticBoxSizer* m_pcOutputSizer;

	wxStaticBox* m_pcPlaceChoiceSizerLabel;

	wxCheckListBox* m_pcPlaceChoiceCheckListBox;

	wxSizer* m_pcPlotControlSizer;
	wxSizer* m_pcTableControlSizer;


	wxButton* m_pcSaveTablePlotButton;

	wxButton* m_pcSimulationProperties;
	wxButton* m_pcStartButton;

	wxComboBox* m_pcViewBox;

	wxCheckBox* m_pcSelectClearAllChkBox;

	//A timer to update the simulation dialog periodically
	wxTimer* m_pcTimer;

	//a list of pointer to currently opened windows
	std::list<SP_DLG_ShowAllModelView*> m_pcExternalWindows;

public:
	wxArrayString m_ArrayUnPlaces, m_ArrayUnTranstions;
	wxArrayString m_ArrayColPlaces, m_ArrayColTranstions;
	wxArrayString m_ArrayAuxPlaces, m_ArrayAuxtranstions;


	//initialize views
	virtual void InitializeViews();

	/*
	 * This methods must be defined in the child class.
	 * Load and initialize the Dialog data.
	 * See SP_DLG_CPNSimulationResults for an example
	 */
	virtual void LoadData(bool p_bCreateNewTree = true);

	//TODO: implement it in your derived class
	virtual void UpdateViewer()=0;

	virtual void SetViewAttributeValue(SP_DS_Metadata* p_pcView, const wxString& p_sAttributeName, const wxString& p_sValue);

//update the x axis values base on the user selection
virtual void UpdateXAxisValues()=0;

protected:
	/*
	 * net information
	 * TODO: overload LoadNetInformation to fill in this information
	 */
	SP_VectorString m_asPlaceNames;
	SP_VectorString m_asTransitionNames;
	SP_MapString2Long m_mPlaceName2Position;
	SP_MapString2Long m_mTransitionName2Position;

	SP_MapLong2String m_mPlaceID2Name;
	SP_MapLong2String m_mTransID2Name;

    SP_MapString2Double m_msParameterName2Value;

	//simulation Matrices
protected:
	//for auxiliary variables
	SP_Vector2DDouble m_aanAuxPLVarsResults;
	SP_Vector2DDouble m_aanAuxTRVarsResults;

	//uncolored elements
	SP_Vector2DDouble m_anResultMatrix;

	//colored elements
	SP_Vector2DDouble m_aanColPlaceResults;
	SP_Vector2DDouble m_aanColTransResults;

	SP_VectorString m_vAuxPLVars;
	SP_VectorString m_vAuxTRVars;

	bool m_bComAuxVarSingleRun;
public:
	SP_VectorString* GetAuxPLVariables()
	{
		return &m_vAuxPLVars;
	}
	SP_VectorString* GetAuxTRVariables()
	{
		return &m_vAuxTRVars;
	}

	virtual bool ComputeAuxiliaryVars()
	{
		return true;
	}

	virtual void ComputeAverageAuxiliaryVars(long p_nSimulationRunNumber, long p_nSimulationRunCount)
	{
	}
	virtual void ComputeEachSimulationAuxiliaryVars()
	{
	}

	static wxString GetColourString(unsigned int p_nIndex);

	void SetComAuxVarSingleRun(bool p_bFlag)
	{
		m_bComAuxVarSingleRun = p_bFlag;
	}
	bool GetComAuxVarSingleRun()
	{
		return m_bComAuxVarSingleRun;
	}

	//remove an external window from the maintained external window list
	void RemoveExternalWindow(SP_DLG_ShowAllModelView* p_pcWindow)
	{
		m_pcExternalWindows.remove(p_pcWindow);
	}

	void ChangeViewTo(SP_DS_Metadata* p_pcView);

	virtual void LoadData(SP_DS_ResultViewer* p_pcViewer, SP_DS_Metadata* p_pcView);

	//load place colours
	void LoadNodeColours(const wxString& p_nNodeName, SP_VectorString& p_asColours);

protected:
DECLARE_EVENT_TABLE()

	virtual void LoadSets();
    virtual void LoadParameters();
	virtual bool InitializeSimulator()=0;

	virtual void OnColourClick(wxGridEvent& p_cEvent);

	virtual void OnLabelDClick(wxGridEvent& p_cEvent);

	//Must be defined in the child class
	virtual void OnStartAbortSimulation(wxCommandEvent& p_cEvent)=0;

	virtual void OnSimulationProperties(wxCommandEvent& p_cEvent);
	virtual void OnDirectExportProperties(wxCommandEvent& p_cEvent);

	wxColour GetNodeColour(SP_DS_Node* p_pcNode);

	//Zoom
	virtual void OnPlotZoomH_In(wxCommandEvent& p_cEvent);
	virtual void OnPlotZoomH_Out(wxCommandEvent& p_cEvent);
	virtual void OnPlotZoomV_In(wxCommandEvent& p_cEvent);
	virtual void OnPlotZoomV_Out(wxCommandEvent& p_cEvent);
	virtual void OnPlotZoomHV_In(wxCommandEvent& p_cEvent);
	virtual void OnPlotZoomHV_Out(wxCommandEvent& p_cEvent);
	virtual void OnPlotZoomCentral(wxCommandEvent& p_cEvent);
	virtual void OnPlotSaveArea(wxCommandEvent& p_cEvent);

	virtual void OnPrint(wxCommandEvent& p_cEvent);

	virtual void OnOpenAllViewsSeparately(wxCommandEvent& p_cEvent);

	virtual void OnExportToCSV();

	virtual void OnCollapsePropertySizer(wxCollapsiblePaneEvent& event);
	virtual void OnCollapseSetsSizer(wxCollapsiblePaneEvent& event);
	virtual void OnCollapseDirectExportSizer(wxCollapsiblePaneEvent& event);
	virtual void OnOpenSelectedGraphViews(wxCommandEvent& p_cEvent);
	virtual void OnAddingNewModalView(wxCommandEvent& p_cEvent);
	virtual void OnRemovingModalViews(wxCommandEvent& p_cEvent);
	//overwrite this function in child class to give periodic update
	virtual void OnTimer(wxTimerEvent& event)
	{
	}

	//on initialize dialog
	virtual void OnInitDialog(wxInitDialogEvent& event);

	virtual void OnModifyConstantSets(wxCommandEvent& p_cEvent);

protected:

	//Add the selected table row as a new marking to the net
	virtual void AddNewMarkingSet(const wxString& p_sMarkingSetName, const int& p_nRow);

	//Bitmap
	virtual wxBitmap GetEnlargeBitmap();
	virtual wxBitmap GetShrinkBitmap();
	virtual wxBitmap GetZoomInBitmap();
	virtual wxBitmap GetZoomOutBitmap();

	virtual wxString GetCurveName(long p_nCurveIndex);
	virtual wxString GetSpacer(int p_nSpacer);
	/*
	 * Minimal Dialog layout. Do not use it, if you want to design your dialog
	 * from scratch
	 */
	virtual void SetMinimalLayout();

	virtual SP_DS_ColListAttribute* GetOneMakingAttribute();
	virtual bool IsMarkingSetNameExist(const wxString& p_sName);

	//specify how we get a reference to the result matrices
	virtual void UpdateSimulationMatrix()=0;

	//load the net information first time the dialog is opened
	virtual void LoadNetInformation()=0;

	//check for existence of a view with a certain name
	virtual bool IsViewNameExist(const wxString& p_sViewName);

	//find a view with a certain name
	virtual SP_DS_Metadata* FindView(const wxString& p_sViewName);

	//Create a new view
	SP_DS_Metadata* CreateNewView(const wxString& p_sName, bool p_bCloneCurrent = false);

	//initialize empty view with data
	virtual void InitializeEmptyView(SP_DS_Metadata* p_pcView)=0;

	virtual wxString GetViewAttributeValue(SP_DS_Metadata* p_pcView, const wxString& p_sAttributeName);

	//Refresh currently opened window
	virtual void RefreshExternalWindows();

	//Refresh currnet external window
	virtual void RefreshCurrentExternalView(int p_nCurveIndex, wxString p_nColor, int p_nLineWidth, int p_nLineStyle);

	//open one window for a view
	virtual void OpenViewInSeparateWindow(SP_DS_Metadata* p_pcModelView);

	//add general GUI options to all simulator
	virtual void AddGuiOption2Simulator();

	virtual void ChangeCurrentView(SP_DS_Metadata* p_pcView);

	bool CreateViewerDataFromRegex(SP_DS_Metadata* p_pcView);

	//load view curves
	virtual bool LoadViewerData(SP_DS_ResultViewer* p_pcViewer, SP_DS_Metadata* p_pcView, wxArrayString& p_asPlaces);

	virtual int LoadSimulatorProperties();

	virtual void SaveSimulatorProperties();

	//get the current combo box selection
	virtual int GetCureentSelectedSimulator()=0;

	//create a simulator based on the child class type
	virtual spsim::Simulator* CreateSimulator(const int& p_nSimulatorType)=0;

	virtual void OnLoadData(wxCommandEvent& p_cEvent);

public:
	virtual void OnClearPlaceList(bool p_nCheck);
	virtual void OnItemCheckUncheck(unsigned int p_nListLocation, unsigned int p_nLocation, bool p_nCheck);
	virtual void OnEditXAxis(wxWindow *p_pcExternalWindowDialog);
	virtual void OnExportClicked(wxWindow *p_pcExternalWindowDialog, int p_nSelection);
protected:

DECLARE_CLASS( SP_DLG_Simulation )
public:
	SP_DLG_Simulation(SP_DS_Graph* p_pcGraph, wxWindow* p_pcParent, wxString p_sHelpText = wxT(""), wxString p_sTitle = wxT("Simulation controls"), long p_nStyle = wxDEFAULT_DIALOG_STYLE);

	virtual ~SP_DLG_Simulation();
	virtual wxButton* GetStartButton()
	{
		return m_pcStartButton;
	}
	virtual void SetSimulationProgressGauge(long p_nValue)
	{
		m_pcSimulationProgressGauge->SetValue(p_nValue);
		Update();
	}
	virtual void SetSimulationProgressGaugeRange(long p_nRangeValue)
	{
		m_pcSimulationProgressGauge->SetRange(p_nRangeValue);
	}

	virtual void SetDirectExportFilename(const wxString& p_sFilename)
	{
		m_sExportFilename = p_sFilename;
	}
	virtual wxString GetDirectExportFilename()
	{
		return m_sExportFilename;
	}

	virtual void SetDirectExportSpacer(long p_nSpacer)
	{
		m_nExportSpacer = p_nSpacer;
	}
	virtual long GetDirectExportSpacer()
	{
		return m_nExportSpacer;
	}

	virtual void OnDlgOk(wxCommandEvent& p_cEvent)
	{
	}
	virtual void OnDlgCancel(wxCommandEvent& p_cEvent);

	virtual void OnCloseWindow(wxCloseEvent& p_cEvent);
	
	virtual SP_ExportType GetExportType()
	{
		return m_ExportType;
	}

	//Should be overloaded if you need this functionality because it used by the simulator
	virtual void DirectExportToCSV()=0;
	virtual void DirectSingleExportToCSV(long p_nSimulatinRunNumber)
	{
	}
	virtual void DirectSingleExactExportToCSV(long p_nSimulatinRunNumber, double p_nTimepoint, const SP_VectorDouble& p_anMarking, const SP_VectorDouble& p_anHazardValues)
	{
	}

	static void SimulatorLogFunction(wxLogLevel p_nLogLevel, const wxString& p_sLogMsg);

	virtual void LoadResults();
	virtual void SetSimulationStopWatch(long p_nTime)
	{
		float se = (float)p_nTime / 1000;
		int secs = (int)(se);
		int hr = secs / 3600;
		int mi =  (secs % 3600) / 60;
		int secsi = secs - (hr * 3600 + mi * 60);
		se -= secs;
		se += (secsi);
		if (hr != 0) {
			m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%d h %d m %.3f s"), hr, mi, se));
		} else if (mi != 0) {
			m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%d m %.3f s"), mi, se));
		} else {
			m_pcSimulationStopWatch->SetLabel(wxString::Format(wxT("%.3f s"), se));
		}
	}

	virtual wxString GetApFormulae()
	{
		return m_sApFormulae;
	}

	virtual void OpenExportFile();
	virtual void CloseExportFile();

	//has to be implemented in each class
	virtual void CalculateXAxisValues(SP_DS_Metadata* p_pcView, SP_VectorDouble& p_anXAxisValues)=0;
};

#endif // __SP_DLG_Simulation__
