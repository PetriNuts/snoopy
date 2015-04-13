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

#include "sp_core/SP_Vector.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_ResultViewer.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

class SP_DS_Metadata;
class SP_DS_ResultViewer;
class SP_DLG_ShowAllModelView: public wxDialog, public SP_Error
{
private:
	DECLARE_EVENT_TABLE();
	DECLARE_CLASS(SP_DLG_ShowAllModelView)

	SP_DS_Metadata* m_pcModelView;

	SP_DS_ResultViewer* m_pcResultViewer;

	SP_VectorDouble m_anXValues;

	//viewer sizer
	wxSizer* m_pcViewerSizer;

	wxButton* m_pcRefreshButton;

	wxButton* m_pcConnectButton;

	//pointer to parent window
	wxWindow* m_pcParentWnd;

	bool m_bIsDisconnected;
protected:

	void OnRefresh(wxCommandEvent& event);

	void OnClose(wxCommandEvent& event);

	void OnWindowActivate(wxActivateEvent& event);

	void OnImageExport(wxCommandEvent &event);

	void OnDisconnect(wxCommandEvent &event);
protected:
	void CreateResultViewer();

	wxString GetViewAttributeValue(SP_DS_Metadata* p_pcView, const wxString& p_sAttributeName);

	bool LoadView(SP_DS_ResultViewer* p_pcResultViewer, SP_DS_Metadata* p_pcModelView);
	void LoadCurveSetting(SP_DS_ResultViewer* p_pcViewer, SP_DS_ColListAttribute* p_pcCurveInfoList, const unsigned int& p_nRow);

public:
	SP_DLG_ShowAllModelView(wxWindow* p_pcWnd, SP_DS_Metadata* p_pcModelView);
	virtual ~SP_DLG_ShowAllModelView();

	void RefreshWindow();

	void RemoveExternalWindowsPointer();

	//return the associated model view
	SP_DS_Metadata* GetModelView()
	{
		return m_pcModelView;
	}
};

#endif /* SP_DLG_SHOWALLMODELVIEW_H_ */
