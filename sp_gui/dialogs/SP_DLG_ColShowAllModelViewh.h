
#ifndef SP_DLG_ColSHOWALLMODELVIEW_H_
#define SP_DLG_ColSHOWALLMODELVIEW_H_

#include "sp_core/SP_Vector.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_ResultViewer.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gui/dialogs/SP_DLG_DirColorSimulation.h"


class SP_DS_Metadata;
class SP_DS_ResultViewer;
class SP_DLG_ColShowAllModelView : public wxFrame, public SP_Error
{
private:
	DECLARE_EVENT_TABLE();
	DECLARE_CLASS(SP_DLG_ColShowAllModelView)

	SP_DS_Metadata* m_pcModelView;

	SP_DS_ResultViewer* m_pcResultViewer;

	SP_DS_Graph* m_pcGraph;

	SP_VectorDouble m_anXValues;

	//viewer sizer
	wxSizer* m_pcViewerSizer;

	wxButton* m_pcRefreshButton;

	wxButton* m_pcConnectButton;

	//pointer to parent window
	SP_DLG_DirColorSimulation* m_pcParentWnd;

	

	bool m_bIsDisconnected;

	wxCheckListBox* m_pcPlaceChoiceCheckListBox;
	wxCheckBox* m_pcSelectClearAllChkBox;
	wxChoice *m_pcOutputViewerType;
	wxChoice *m_pcOutputExportType;
	wxSizer *m_pcRightSizer;
	wxButton *m_pcShowHideButton;
	wxButton *m_pcXAxis;
	wxButton *m_pcViewerTypeButton;
	wxButton *m_pcEditNodeListButton;

	bool m_bIsShown;
	wxSizer *m_pcContentSizer;

	bool m_bIsColouredSimulation;
protected:

	void OnRefresh(wxCommandEvent& event);

	void OnClose(wxCommandEvent& event);
	void OnWindowClose(wxCloseEvent& event);
	void DoClose();

	void OnWindowActivate(wxActivateEvent& event);

	void OnDisconnect(wxCommandEvent &event);
	void OnPlaceCheckUncheck(wxCommandEvent& p_cEvent);
	void OnItemDoubleClick(wxCommandEvent& p_cEvent);
	void OnSelectClearAllItems(wxCommandEvent& p_cEvent);
	void OnEditViewerProperties(wxCommandEvent& p_cEvent);
	void OnChangeResultViewer(wxCommandEvent& event);
	void OnExportClick(wxCommandEvent& event);
	void OnChangeXAxis(wxCommandEvent& event);
	void OnEditNodeList(wxCommandEvent& event);
	void OnShowHideNodeList(wxCommandEvent& event);

protected:
	void CreateResultViewer();

	wxString GetViewAttributeValue(SP_DS_Metadata* p_pcView, const wxString& p_sAttributeName);

	bool LoadView(SP_DS_ResultViewer* p_pcResultViewer, SP_DS_Metadata* p_pcModelView);
	
	void LoadCurveSetting(SP_DS_ResultViewer* p_pcViewer, SP_DS_ColListAttribute* p_pcCurveInfoList, const unsigned int& p_nRow);

public:

	SP_DLG_ColShowAllModelView(SP_DLG_DirColorSimulation* p_pcWnd, SP_DS_Metadata* p_pcModelView, SP_DS_Graph* p_pcGraph);

	
	
	virtual ~SP_DLG_ColShowAllModelView();

	void RefreshWindow();
	void RefreshCurrentWindow(int p_nCurveIndex, wxString p_nColor, int p_nLineWidth, int p_nLineStyle);
	void RemoveExternalWindowsPointer();

	//return the associated model view
	SP_DS_Metadata* GetModelView()
	{
		return m_pcModelView;
	}
};

#endif /* SP_DLG_SHOWALLMODELVIEW_H_ */
