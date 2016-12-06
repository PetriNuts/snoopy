/*
 * SP_DLG_ViewerWindowSteering.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 27.11.2016
 * @Description
 * A viewer window for steering dialog
 */ //======================================================================================
#ifndef SP_GUI_DIALOGS_SP_DLG_VIEWERWINDOWSTEERING_H_
#define SP_GUI_DIALOGS_SP_DLG_VIEWERWINDOWSTEERING_H_

#include <sp_gui/dialogs/SP_DLG_ViewerWindow.h>
#include "spsa/spsa.h"

class SP_DLG_ViewerWindowSteering: public SP_DLG_ViewerWindow
{
private:
	    DECLARE_EVENT_TABLE();
		DECLARE_CLASS(SP_DLG_ViewerWindowSteering)

	    //viewer view
	    spsa::ModelView* m_pcModelView;

	    //curve info
	    spsa::ResultMatrixInfo* m_pcResultMatrixInfo;

	    //pointer to result matrix
	    spsa::Vector2DDouble* m_an2DResultMatrix;

	    //x axis values
	    SP_VectorDouble*  m_pcXAxisValues;

	    spsa::Model * m_pcCurrentModel;
protected:
	    virtual void OnRefresh(wxCommandEvent& event){}

		virtual void OnWindowActivate(wxActivateEvent& event);

		virtual void OnPlaceCheckUncheck(wxCommandEvent& p_cEvent){}
		virtual void OnItemDoubleClick(wxCommandEvent& p_cEvent);
		virtual void OnSelectClearAllItems(wxCommandEvent& p_cEvent){}
		virtual void OnEditViewerProperties(wxCommandEvent& p_cEvent);
		virtual void OnChangeResultViewer(wxCommandEvent& event);
		virtual void OnExportClick(wxCommandEvent& event){}
		virtual void OnChangeXAxis(wxCommandEvent& event);
		virtual void OnEditNodeList(wxCommandEvent& event);
protected:
		bool LoadView(SP_DS_ResultViewer* p_pcResultViewer, spsa::ModelView* p_pcModelView);

		wxString GetViewerType(const int& p_nViewerIndex);
public:
		virtual void RefreshWindow(){}
	    virtual void RefreshCurrentWindow(int p_nCurveIndex, wxString p_nColor, int p_nLineWidth, int p_nLineStyle){}
		virtual void RemoveExternalWindowsPointer(){}
public:
	  SP_DLG_ViewerWindowSteering(SP_DLG_Simulation* p_pcParentWnd,
			                      spsa::ModelView* p_pcModelView,
								  spsa::ResultMatrixInfo* p_pcResultMatrixInfo,
								  spsa::Vector2DDouble* p_an2DResultMatrix,
								  SP_VectorDouble*  p_pcXAxisValues,
								  spsa::Model * p_pcCurrentModel);
	  virtual ~SP_DLG_ViewerWindowSteering();

	  spsa::ModelView* GetModelView(){ return m_pcModelView; }
};

#endif /* SP_GUI_DIALOGS_SP_DLG_VIEWERWINDOWSTEERING_H_ */
