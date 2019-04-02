/*
 * SP_DLG_ShowAllModelView.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 09.08.2010
 * @Description
 * Display all model views in one widow
 */ //======================================================================================
#ifndef SP_DLG_SHOWALLMODELVIEW_H_
#define SP_DLG_SHOWALLMODELVIEW_H_

#include <sp_gui/dialogs/SP_DLG_ViewerWindow.h>


class SP_DLG_ShowAllModelView: public SP_DLG_ViewerWindow
{
private:

	DECLARE_EVENT_TABLE();
	DECLARE_CLASS(SP_DLG_ShowAllModelView)

	SP_DS_Metadata* m_pcModelView;

	SP_DS_Graph* m_pcGraph;

protected:

	virtual void OnRefresh(wxCommandEvent& event);

	virtual void OnWindowActivate(wxActivateEvent& event);

	virtual void OnPlaceCheckUncheck(wxCommandEvent& p_cEvent);
	virtual void OnItemDoubleClick(wxCommandEvent& p_cEvent);
	virtual void OnSelectClearAllItems(wxCommandEvent& p_cEvent);
	virtual void OnEditViewerProperties(wxCommandEvent& p_cEvent);
	virtual void OnChangeResultViewer(wxCommandEvent& event);
	virtual void OnExportClick(wxCommandEvent& event);
	virtual void OnChangeXAxis(wxCommandEvent& event);
	virtual void OnEditNodeList(wxCommandEvent& event);
	virtual void OnChangeThumbPage(wxCommandEvent& event);

protected:
	wxString GetViewAttributeValue(SP_DS_Metadata* p_pcView, const wxString& p_sAttributeName);

	bool LoadView(SP_DS_ResultViewer* p_pcResultViewer, SP_DS_Metadata* p_pcModelView);
	void LoadCurveSetting(SP_DS_ResultViewer* p_pcViewer, SP_DS_ColListAttribute* p_pcCurveInfoList, const unsigned int& p_nRow);

	//get the viewer type
	wxString GetViewerType();

public:

	SP_DLG_ShowAllModelView(SP_DLG_Simulation* p_pcWnd, SP_DS_Metadata* p_pcModelView, SP_DS_Graph* p_pcGraph);
	virtual ~SP_DLG_ShowAllModelView();

	virtual void RefreshWindow();
	virtual void RefreshCurrentWindow(int p_nCurveIndex, wxString p_nColor, int p_nLineWidth, int p_nLineStyle);
	virtual void RemoveExternalWindowsPointer();

	//return the associated model view
	SP_DS_Metadata* GetModelView(){ return m_pcModelView; }
};

#endif /* SP_DLG_SHOWALLMODELVIEW_H_ */
