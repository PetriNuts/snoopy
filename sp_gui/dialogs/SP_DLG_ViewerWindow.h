/*
 * SP_DLG_ViewerWindow.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 22.011.2016
 * @Description
 * Display all model views in one widow
 */ //======================================================================================

#ifndef SP_GUI_DIALOGS_SP_DLG_VIEWERWINDOW_H_
#define SP_GUI_DIALOGS_SP_DLG_VIEWERWINDOW_H_

#include "sp_core/SP_Vector.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_ResultViewer.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

class SP_DS_Metadata;
class SP_DLG_Simulation;

class SP_DLG_ViewerWindow:public wxFrame, public SP_Error
{
private:
	    DECLARE_EVENT_TABLE();
		DECLARE_CLASS(SP_DLG_ViewerWindow)

	    wxSizer* m_pcMainSizer;

	    SP_DS_ResultViewer* m_pcResultViewer;

		SP_VectorDouble m_anXValues;

		//viewer sizer
		wxSizer* m_pcViewerSizer;

		wxButton* m_pcRefreshButton;

		wxButton* m_pcConnectButton;

		//pointer to parent window
		SP_DLG_Simulation* m_pcParentWnd;

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
		virtual void OnRefresh(wxCommandEvent& event)=0;

		virtual void OnClose(wxCommandEvent& event);
		virtual void OnWindowClose(wxCloseEvent& event);
		virtual void DoClose();
		virtual void OnDisconnect(wxCommandEvent &event);

		virtual void OnWindowActivate(wxActivateEvent& event)=0;
		virtual void OnPlaceCheckUncheck(wxCommandEvent& p_cEvent)=0;
		virtual void OnItemDoubleClick(wxCommandEvent& p_cEvent)=0;
		virtual void OnSelectClearAllItems(wxCommandEvent& p_cEvent)=0;
		virtual void OnEditViewerProperties(wxCommandEvent& p_cEvent)=0;
		virtual void OnChangeResultViewer(wxCommandEvent& event)=0;
		virtual void OnExportClick(wxCommandEvent& event)=0;
		virtual void OnChangeXAxis(wxCommandEvent& event)=0;
		virtual void OnEditNodeList(wxCommandEvent& event)=0;
		virtual void OnShowHideNodeList(wxCommandEvent& event);

protected:
		virtual void CreateResultViewer(const wxString& p_sViewerType);
public:
		virtual void RefreshWindow()=0;
		virtual void RefreshCurrentWindow(int p_nCurveIndex, wxString p_nColor, int p_nLineWidth, int p_nLineStyle)=0;
		virtual void RemoveExternalWindowsPointer()=0;
public:

	  SP_DLG_ViewerWindow(SP_DLG_Simulation* p_pcParentWnd);
	  virtual ~SP_DLG_ViewerWindow();
};

#endif /* SP_GUI_DIALOGS_SP_DLG_VIEWERWINDOW_H_ */
