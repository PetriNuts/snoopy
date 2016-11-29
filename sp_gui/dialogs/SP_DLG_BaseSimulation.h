#include <wx/collpane.h>

#ifndef __SP_DLG_BASESIMULATION__
#define __SP_DLG_BASESIMULATION__

#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/checklst.h>
#include <wx/treectrl.h>
#include <wx/bmpbuttn.h>
#include <wx/listctrl.h>
#include <wx/txtstrm.h>

#include <wx/wfstream.h>
#include <wx/file.h>
#include <wx/collpane.h>

#include "sp_core/SP_Vector.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

class SP_DS_Metadata;
class SP_DLG_BaseSimulation: public wxDialog, public SP_Error
{
private:

	DECLARE_EVENT_TABLE();
	DECLARE_CLASS( SP_DLG_BaseSimulation )

protected:

	wxScrolledWindow* m_pcScrolledWindow;
	wxBoxSizer* m_pcMainSizer;
	wxSizer* m_pcSimulationControlSizer;
	wxSizer* m_pcContentSizer;
	wxSizer* m_pcLeftSizer;

	//divide the sizers into smaller parts
	wxSizer* m_pcModelConfigSizer;
	wxSizer* m_pcModelSimulationSizer;
	wxSizer* m_pcExportSizer;
	wxSizer* m_pcViewerSizer;


	wxSizer* m_pcPropertySizer;
	wxSizer* m_pcSetsSizer;
	wxSizer* m_pcDirectExportSizer;
	wxSizer* m_pcSimulationButtonSizer;
	wxSizer* m_pcModelViewsSizer;

	//Input parameters for simulation
	wxTextCtrl* m_pcIntervalStartTextCtrl;
	wxTextCtrl* m_pcIntervalEndTextCtrl;
	wxTextCtrl* m_pcResultPointCountTextCtrl;


	wxWindow *m_pcPropertyWindowSimulationButtonSizer;
	wxWindow *m_pcPropertyWindowPropertySizer;
	wxWindow *m_pcPropertyWindowDirectExportSizer;
	wxWindow *m_pcPropertyWindowSetsSizer;

	wxCollapsiblePane *m_pcCollpaneDirectExportSizer;
	wxCollapsiblePane *m_pcCollpanePropertySizer;
	wxCollapsiblePane *m_pcCollpaneSetsSizer;

protected:

	//on initialize dialog
	virtual void OnInitDialog(wxInitDialogEvent& event) =0;

public:
	SP_DLG_BaseSimulation(wxWindow* p_pcParent, wxString p_sHelpText = wxT(""), wxString p_sTitle = wxT("Simulation controls"), long p_nStyle = wxDEFAULT_DIALOG_STYLE);
	virtual ~SP_DLG_BaseSimulation();
};
#endif
