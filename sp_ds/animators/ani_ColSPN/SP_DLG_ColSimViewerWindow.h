/*
* SP_DLG_ViewerWindow.h
* $Author: George Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 22.07.2023
* @Description
* Display all model views in one widow
*/ //======================================================================================

#ifndef SP_GUI_DIALOGS_SP_DLG_COL_SIM_VIEWERWINDOW_H_
#define SP_GUI_DIALOGS_SP_DLG_COL_SIM_VIEWERWINDOW_H_

#include "sp_core/SP_Vector.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_ResultViewer.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gui/dialogs/dia_SPN/SP_DLG_StDirectExportProperties.h"
#include <wx/wfstream.h>
#include <wx/file.h>


class SP_DS_Metadata;
class SP_DLG_Animation;

class SP_DLG_ColSimViewerWindow :public wxFrame, public SP_Error
{
private:
	DECLARE_EVENT_TABLE();
	DECLARE_CLASS(SP_DLG_ColSimViewerWindow)

	wxSizer* m_pcMainSizer;

	SP_DS_ResultViewer* m_pcResultViewer;

	SP_VectorDouble m_anXValues;

	SP_VectorDouble m_anYValues;//alpha levels membership funct
	std::map<wxString, long> m_mselectedPlace2Index;

								//viewer sizer
	wxSizer* m_pcViewerSizer;

	wxButton* m_pcRefreshButton;

	wxButton* m_pcConnectButton;

	//pointer to parent window
	SP_DLG_Animation* m_pcParentWnd;

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
	wxString m_sExportFilename;

	bool m_bIsShown;
	wxSizer *m_pcContentSizer;

	bool m_bIsColouredSimulation;
protected:
	wxSizer* m_pcScroll;
	wxScrollBar* m_pscrollBar;
	wxSlider *m_slider;
	wxStaticText* m_ptimeLabel;
	wxTextCtrl* m_ptimePointValue;

	wxArrayString m_arPlaces;

protected:
	//virtual void OnRefresh(wxCommandEvent& event) = 0;

	virtual void OnClose(wxCommandEvent& event);
	virtual void OnWindowClose(wxCloseEvent& event);
	//virtual void DoClose();
	//virtual void OnDisconnect(wxCommandEvent &event);

	///virtual void OnWindowActivate(wxActivateEvent& event) = 0;
	virtual void OnPlaceCheckUncheck(wxCommandEvent& p_cEvent) ;
	//virtual void OnItemDoubleClick(wxCommandEvent& p_cEvent) = 0;
	virtual void OnSelectClearAllItems(wxCommandEvent& p_cEvent) ;
	//virtual void OnEditViewerProperties(wxCommandEvent& p_cEvent) = 0;
	virtual void OnChangeResultViewer(wxCommandEvent& event);
	virtual void OnExportClick(wxCommandEvent& event)  ;
	void  OpenExportFile();

	//virtual void OnChangeXAxis(wxCommandEvent& event) = 0;
	//virtual void OnEditNodeList(wxCommandEvent& event) = 0;
	//virtual void OnShowHideNodeList(wxCommandEvent& event);
	//virtual void OnScrol(wxScrollEvent & event);//wxCommandEvent&  

	//virtual void OnTextEner(wxCommandEvent & event);

protected:
	//virtual void CreateResultViewer(const wxString& p_sViewerType);
public:
//	virtual void RefreshWindow(const bool& p_bUpdate = false) = 0;
	//virtual void RefreshCurrentWindow(int p_nCurveIndex, wxString p_nColor, int p_nLineWidth, int p_nLineStyle) = 0;
	//virtual void RemoveExternalWindowsPointer() = 0;
	//virtual wxScrollBar* GetScrollBar() { return m_pscrollBar; }

private:
	SP_Vector2DDouble m_vvResultMatrix;
	SP_MapString2Int m_mcreatedCurves;
	wxFileOutputStream* m_pcExportFileOutputStream;
	wxBufferedOutputStream* m_pcExportBufferdOutputStream;
	wxTextOutputStream* m_pcExport;
	int m_nExportSpacer;
	bool IsCurveCreated(wxString p_sName);
	void AddItems();
	wxString GetSpacer(int p_nSpacer);
public:

	SP_DLG_ColSimViewerWindow(SP_DLG_Animation* p_pcParentWnd, std::map<wxString, long> m_mselectedPlace2Index, SP_Vector2DDouble p_vvResultMatrix, SP_VectorDouble p_vCAxisData, wxString p_sExportName);
	virtual ~SP_DLG_ColSimViewerWindow();
};

#endif /* SP_GUI_DIALOGS_SP_DLG_COL_SIM_VIEWERWINDOW_H_ */
