/*
 * SP_GUI_SteeringDialog.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 23.03.2011
 * Short Description:
 */
//=================================================

#ifndef SP_GUI_STEERINGDIALOG_H_
#define SP_GUI_STEERINGDIALOG_H_

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/timer.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/file.h>

#include "spsa/helper/resultMatrixInfo.h"
#include "sp_gui/dialogs/SP_DLG_Simulation.h"


#define MAX_VIEWER_TYPES  3

#define MAX_RESULT_MATRIX_TYPES  3


class SP_DS_ResultViewer;
class SP_GUI_SteeringSubWindow;

class SP_GUI_SteeringDialog: public SP_DLG_Simulation
{
private:
	spsa::SteeringClient* m_pcGUIClient;

	spsa::Model*  m_pcCurrentModel;

	wxString m_sCurrentWorkingModelName;

	spsa::Vector2DDouble m_an2DResultMatrix;

	std::vector<SP_DS_ResultViewer*> m_apcResultViewers;

	SP_VectorDouble  m_anXAxisValues;

	unsigned int m_nCurrentViewerIndex;

	double m_nOutputStartPoint;

	double m_nOutputEndPoint;

	double m_nOutputSamplingSize;

	int m_nViewMatrixType;

	//Setting
	bool m_bRefreshResult;
	bool m_bRefreshRuntime;

	spsa::VectorString m_asEditedPlaceNames;

	//how often do refresh;
	double m_nRefreshResultDuration;

	//refresh runtime
	double m_nRefreshRuntimeDuration;

	//A timer for periodic refresh of the client data
	wxTimer* m_pcResultRefershTimer;

	//for displaying statistics and other runtimes
	wxTimer* m_pcRuntimeRefershTimer;

	spsa::VectorSimulators m_msSimulatorByCategory;

	int m_nOldSimulationState;

	spsa::VectorModelView* m_pcModelViews;

	unsigned int m_nCurrentView;

	//csv export separator
	int m_nExportSpacer;

	//csv export file name
	 wxString m_sExportFilename;

	wxFileOutputStream* m_pcExportFileOutputStream;
	wxBufferedOutputStream* m_pcExportBufferdOutputStream;
	wxTextOutputStream* m_pcExport;

	spsa::ResultMatrixInfo* m_pcResultMatrixInfo;

	//a flag to check if the dialog is modified
	bool m_bIsModified;

	//enable estimation of the refresh time
	bool m_bEnableRefreshCalculationTime;
private:
	//GUI elements
	wxStaticText* m_pcSimulatorState;
	wxStaticText* m_pcSimulationRunTime;
	wxStaticText* m_pcCurrentSimulatorTextBox;
	wxStaticText* m_pcSimulationProgressPercentage;
	wxGauge* m_pcSimulationProgressBar;
	//wxChoice* m_pcModelViewBox;

	wxChoice* m_pcSimulatorsAlgorithmBox;

	wxChoice* m_pcSimulatorsCategoryBox;

	SP_GUI_SteeringSubWindow* m_pcPlaceEditingWindow;
	SP_GUI_SteeringSubWindow* m_pcParameterEditingWindow;

	wxChoice* m_pcModelsBox;


	wxSizer*  m_pcOutputSizer;

	//Choice box
	wxCheckListBox* m_pcItemsChoiceCheckListBox;

	wxChoice* m_pcOutputViewerType;

	//select all items check box
	wxCheckBox* m_pcSelectClearAllChkBox;

	//simulator speed
	wxSlider* m_pcSimulatorSpeedSlider;

	wxCollapsiblePane *m_pcCollPanalSteeringParamSizer;
	wxCollapsiblePane *m_pcCollPanalSteeringPlaceSizer;

protected:

	//==========begin new layout
	virtual void DoStartSimulation() {}

	virtual void UpdateViewer(SP_DS_Metadata* p_pcView = nullptr){}
	virtual void UpdateXAxisValues(){}

	virtual bool InitializeSimulator(){return false;}

	virtual void OnStartAbortSimulation(wxCommandEvent& p_cEvent){}

	virtual void UpdateSimulationMatrix(SP_DS_Metadata* p_pcView = nullptr){}

	virtual void LoadNetInformation(){}

	virtual void InitializeEmptyView(SP_DS_Metadata* p_pcView){}

	virtual int GetCureentSelectedSimulator(){return 0;}

	virtual spsim::Simulator* CreateSimulator(const int& p_nSimulatorType){
		return NULL;
	}

	virtual void SetMinimalLayout();

	virtual void DirectExportToCSV(){}

	//==========end new layout

protected:

	//viewer window handler
	virtual void OnOpenSelectedGraphViews(wxCommandEvent& p_cEvent);

protected:

	//assistant view functions
	virtual void OpenViewInSeparateWindow(spsa::ModelView* p_pcModelView);

	virtual spsa::ModelView* FindSteeringView(const wxString& p_sViewName);

protected:

	virtual void CalculateXAxisValues(SP_DS_Metadata* p_pcView, SP_VectorDouble& p_anXAxisValues){}

	//events handlers
	void OnPauseButton(wxCommandEvent& WXUNUSED(event));

	void OnStartButton(wxCommandEvent& WXUNUSED(event));

	void OnStopButton(wxCommandEvent& WXUNUSED(event));

	void OnResumeButton(wxCommandEvent& WXUNUSED(event));

	void OnRestartButton(wxCommandEvent& WXUNUSED(event));

	void OnRefreshButton(wxCommandEvent& WXUNUSED(event));

	void OnChangeViewCombobox(wxCommandEvent& WXUNUSED(event));

	void OnItemStateChanged(wxCommandEvent& event);

	void OnModelNameChanged(wxCommandEvent& WXUNUSED(event));

	//void OnViewerPropertiesButton(wxCommandEvent& event);

	void OnResultRefreshTimer(wxTimerEvent& WXUNUSED(event));

	void OnRuntimeRefreshTimer(wxTimerEvent& WXUNUSED(event));

	void OnSelectPlacesToEdit(wxCommandEvent& WXUNUSED(event));

	void OnSelectParametersToEdit(wxCommandEvent& WXUNUSED(event));

	void OnSettingButton(wxCommandEvent& WXUNUSED(event));

	void OnSimulatorCategoryChanged(wxCommandEvent& event);

	void OnAddNewView(wxCommandEvent& WXUNUSED(event));

	void OnModelViewChanged(wxCommandEvent& event);

	void OnOverWriteModelSetting(wxCommandEvent& WXUNUSED(event));

	void OnCloseDlg(wxCommandEvent& WXUNUSED(event));

	void OnRefershModelNames(wxCommandEvent& WXUNUSED(event));

	void OnRemoveCurrentView(wxCommandEvent& WXUNUSED(event));

	void OnRefreshModelViews(wxCommandEvent& WXUNUSED(event));

	void OnMoreOptionButton(wxCommandEvent& WXUNUSED(event));

	void OnExport2CSV(wxCommandEvent& WXUNUSED(event));

	void OnExport2CSVSelected(wxCommandEvent& WXUNUSED(event));

	void OnExport2CSVAll(wxCommandEvent& WXUNUSED(event));

	void OnExport2CSVAllColoured(wxCommandEvent& WXUNUSED(event));

	void OnImageExport(wxCommandEvent &event);

	//void OnDoEditViewerProperties(wxCommandEvent& WXUNUSED(event));

	void OnResetParameterValues(wxCommandEvent& WXUNUSED(event));

	void OnResetMarkingValues(wxCommandEvent& WXUNUSED(event));

	void OnSimulatorSetting(wxCommandEvent& WXUNUSED(event));

	void OnSelectClearAllItems(wxCommandEvent& p_cEvent);

	void OnSimulationSpeedChanged(wxScrollEvent& p_cEvent);
protected:
	//GUI bars
	void CreateControlButtonBar(wxSizer* p_pcParentSizer);
	//void CreatModelViewsBar(wxSizer* p_pcParentSizer);
	void CreatStatisticBar(wxSizer* p_pcParentSizer);

	void CreateEditValuesBar(wxSizer* p_pcParentSizer);

	void CreateSimulatorBar(wxSizer* p_pcParentSizer);

protected:

	void UpdateModelNames();

	const SP_VectorString& GetCheckListItemNames();

	/*
	 * load the result in a view
	 */
	void UpdateView();

	/*
	 * Return output result count
	 */
	unsigned long GetOutputResultCount();

	/*
	 * Start output point
	 */
	double GetStartOutputPoint();
	/*
	 * End output point
	 */
	double GetEndOutputPoint();
	/*
	 * Sampling step
	 */
	double GetSamplingStepSize();


	void InitializeResultViewers();

	bool UpdateSimulationIntervals();
	void ShowSimulationIntervals();

	/*
	 * Get the result matrix from the server
	 */
	void RefreshResultMatrix();

	void InitializeEditedList();

	/*
	 * update simulation statistics
	 */
	void UpdateStatistics();

	bool Initialize();

	void ChangeSimulatorCategory(const unsigned int& p_nCategory);

	//write steering dialog setting to the registery
	void WriteSettingToRegistery();

	//read steering dialog from registery
	void ReadSettingFromRegistery();

	void UpdateModelViews();

	void ChangeCurrentViewerType();

	spsa::ModelView* CreateNewView(const wxString& p_sModelViewName);

	void SaveCurrentViewSetting();

	void OverWriteModelViews();

	void ChangeViewTo(const unsigned int& l_ViewIndex);


	void SetupSimulators();

	void SelectCurrentSimulator();

	//check if the view with this name exist
	bool IsViewNameExist(const wxString p_sViewName);

	void LoadCurrentView();

	void SaveItemCheckState(const unsigned int& p_nPosition,const bool& b_bState);

	void ClearSimulatorStatistics();

	//open export file
	void OpenExportFile(SP_VectorLong& p_anSelectedCurves, SP_VectorString& p_asCurveType);

	//close export file
	void CloseExportFile();

	//do the actual export
	void DirectExportToCSV(const SP_VectorLong& p_anExportedCurves, const SP_VectorString& p_asCurveType);

	//get the string representation of the spacer
	wxString GetSpacer(int p_nSpacer);

	//get the dialog state
	bool GetIsModified(){return m_bIsModified;}

	//set the dialog modification state
	void SetModified(const bool& p_bStat=true){m_bIsModified=p_bStat;}

	//start and stop timers
	void StartRefreshTimers();
	void StopRefreshTimers();

	//load steered variables
	void LoadSteeredVariables();

	//check the selected simulator with the net type
	bool CheckSelectedSimulator();

	//format the given time in term of the string repr.
	wxString FormatRuntime(const double& p_nTime);

	virtual void OnInitDialog(wxInitDialogEvent& event){}
public:
	SP_GUI_SteeringDialog(wxWindow* p_pcParent,spsa::SteeringClient* p_pcGUIClient,const wxString& p_sTitle);
	virtual ~SP_GUI_SteeringDialog();

	DECLARE_EVENT_TABLE();
};

#endif /* SP_GUI_STEERINGDIALOG_H_ */
