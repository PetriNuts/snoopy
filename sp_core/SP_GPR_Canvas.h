//////////////////////////////////////////////////////////////////////
// $Source: SP_GPR_Canvas.h
// $Author: MD $
// $Version: $
// $Revision: $
// $Date: 2005/09/19 $
// Short Description: contains global canvas preferences
//////////////////////////////////////////////////////////////////////

#if !defined(__SP_GPR_CANVAS_H__)
#define __SP_GPR_CANVAS_H__

const int SP_DEFAULT_GRID_SPACING = 20;
const int SP_GRID_MIN = 5;
const int SP_GRID_MAX = 100;
const bool SP_DEFAULT_GRID_VISIBLE = false;
const bool SP_DEFAULT_GRID_SNAPPING = true;
const int SP_DEFAULT_GRID_SIZE = 2000;
const int SP_GRID_SIZE_MIN = 1000;
const int SP_GRID_SIZE_MAX = 10000;
const int SP_WINDOW_SIZE_X = 600;
const int SP_WINDOW_SIZE_Y = 400;
const bool SP_DEFAULT_LOG_VERBOSE = false;
const bool SP_DEFAULT_COMPRESS_FILE = true;

#include <wx/tokenzr.h>

#include "sp_defines.h"
#include "sp_core/SP_GPR_Base.h"

struct SP_WindowOptions
{
	int PosX,PosY,SizeX,SizeY;
	bool isMaximized;

	SP_WindowOptions()
	:PosX(0), PosY(0), SizeX(600), SizeY(400), isMaximized(false)
	{}
};

wxString wxToString(const SP_WindowOptions& p_WindowOptions);

bool wxFromString(const wxString& p_String, SP_WindowOptions* p_WindowOptions);

typedef std::map<wxString, SP_WindowOptions> SP_MapString2WindowOptions;

class SP_DLG_GlobalPreferences;
class wxSpinCtrl;

class SP_GPR_Canvas: public SP_GPR_Base {

private:

protected:
	//member variables
	SP_MapString2Int m_mnGridSpacing;
	SP_MapString2Bool m_mbGridVisible;
	SP_MapString2Bool m_mbGridSnappingEnabled;
	SP_MapString2Int m_mnGridSize;
	SP_MapString2Int m_mnWindowSizeX;
	SP_MapString2Int m_mnWindowSizeY;

	bool m_bLogVerbose;
	bool m_bCompressFile;

	SP_MapString2WindowOptions m_mWindowOptions;

	//dialog controls
	wxSpinCtrl *m_scGridSpacing;
	wxCheckBox *m_cbGridVisible;
	wxCheckBox *m_cbGridSnappingEnabled;
	wxSpinCtrl *m_scGridSize;
	wxSpinCtrl *m_scWindowSizeX;
	wxSpinCtrl *m_scWindowSizeY;
	wxCheckBox *m_cbLogVerbose;
	wxCheckBox *m_cbCompressFile;

public:
	SP_GPR_Canvas();
	virtual ~SP_GPR_Canvas();

	//setter
	void SetGridSpacing(const wxString& p_sNetClass, int p_nVal);
	void SetGridVisible(const wxString& p_sNetClass, bool p_bVal);
	void SetGridSnapping(const wxString& p_sNetClass, bool p_bVal);
	void SetGridSize(const wxString& p_sNetClass, int p_bVal);
	void SetWindowSizeX(const wxString& p_sNetClass, int p_bVal);
	void SetWindowSizeY(const wxString& p_sNetClass, int p_bVal);
	void SetWindowOptions(const wxString& p_sKey, const SP_WindowOptions& p_WindowOptions);
	void SetLogVerbose(bool p_bVal);
	void SetCompressFile(bool p_bVal);
	//getter
	inline int GetGridSpacing(const wxString& p_sNetClass) { return m_mnGridSpacing[p_sNetClass]; }
	inline bool GridVisible(const wxString& p_sNetClass) { return m_mbGridVisible[p_sNetClass]; }
	inline bool GridSnappingEnabled(const wxString& p_sNetClass) { return m_mbGridSnappingEnabled[p_sNetClass]; }
	inline int GetGridSize(const wxString& p_sNetClass) { return m_mnGridSize[p_sNetClass]; }
	int GetWindowSizeX(const wxString& p_sNetClass) { return m_mnWindowSizeX[p_sNetClass]; }
	int GetWindowSizeY(const wxString& p_sNetClass) { return m_mnWindowSizeY[p_sNetClass]; }
	const SP_WindowOptions& GetWindowOptions(const wxString& p_sKey);
	inline bool GetLogVerbose() { return m_bLogVerbose; }
	inline bool GetCompressFile() { return m_bCompressFile; }

	void LoadWindowOptions(wxFrame* window);
	void SaveWindowOptions(wxFrame* window);

	bool AddToDialogPage(const wxString& p_sNetClass, SP_WDG_NotebookPage *p_pcPage);
	bool OnDialogOk(const wxString& p_sNetClass);

	bool UpdateFromConfig(wxConfig& p_config);
	bool WriteToConfig(wxConfig& p_config);

};

#endif // !defined(__SP_GPR_CANVAS_H__)
