//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Main replacement
//////////////////////////////////////////////////////////////////////
#ifndef __SNOOPY_H__
#define __SNOOPY_H__

#include <wx/wx.h>
#include "sp_utilities.h"

#include "sp_core/SP_Map.h"

class SP_DS_Graph;
class SP_GUI_Mainframe;
class SP_GM_Docmanager;
class SP_ExportManager;
class SP_ImportManager;
class SP_IA_Manager;

class SP_GPR_Canvas;
class SP_GPR_Animation;
class SP_GPR_Elements;
class SP_GPR_Fonts;

class Snoopy: public wxApp
{
private:
    bool m_bMaximize;
    int  m_nXPos;
    int  m_nYPos;
    int  m_nWidth;
    int  m_nHeight;
	wxString localeSave;
	// used in CheckIntegrity to verify position/offset
	double DELTA = 500;
	// standard offset
	double STD_OFFSET = 20;
	map<wxString,SP_SetString> m_mClasses2Nodes;


protected:
    wxBrush* m_pcCanvasNormalBrush;
    wxBrush* m_pcCanvasEdgeBrush;
    wxBrush* m_pcCanvasLogicBrush;
    wxBrush* m_pcCanvasCoarseBrush;
    wxPen* m_pcCanvasNormalPen;
    wxPen* m_pcCanvasCoarsePen;

    SP_GUI_Mainframe* m_pcMainframe;
    SP_GM_Docmanager* m_pcDocmanager;

    SP_ExportManager* m_pcExportManager;
	SP_ImportManager* m_pcImportManager;

	SP_IA_Manager* m_pcIAManager;

	SP_GPR_Animation *m_pcAnimationPrefs;
	SP_GPR_Canvas *m_pcCanvasPrefs;
	SP_GPR_Elements *m_pcElementPrefs;
	SP_GPR_Fonts *m_pcFontPrefs;

public:
    Snoopy();
    ~Snoopy();

    SP_GUI_Mainframe* GetMainframe() { return m_pcMainframe; }
    SP_GM_Docmanager* GetDocmanager() { return m_pcDocmanager; }

    SP_ExportManager* GetExportManager() { return m_pcExportManager; }
	SP_IA_Manager *GetIAManager() { return m_pcIAManager; }

	SP_ImportManager* GetImportManager() { return m_pcImportManager; }

    // Operations
    bool OnInit();
    int OnExit();

	bool ProcessCommandLine(wxArrayString& p_aargs);//by george

    // Read/write configuration information
    bool ReadOptions();
    bool ReadHistory();
    bool WriteOptions();

    // accessors
    inline wxBrush* GetCanvasNormalBrush() const { return m_pcCanvasNormalBrush; }
    inline wxBrush* GetCanvasEdgeBrush() const { return m_pcCanvasEdgeBrush; }
    inline wxBrush* GetCanvasLogicBrush() const { return m_pcCanvasLogicBrush; }
    inline wxBrush* GetCanvasCoarseBrush() const { return m_pcCanvasCoarseBrush; }
    inline wxPen* GetCanvasNormalPen() const { return m_pcCanvasNormalPen; }
    inline wxPen* GetCanvasCoarsePen() const { return m_pcCanvasCoarsePen; }
	
	inline SP_GPR_Canvas *GetCanvasPrefs() { return m_pcCanvasPrefs; }
	inline SP_GPR_Animation *GetAnimationPrefs() { return m_pcAnimationPrefs; }
	inline SP_GPR_Elements *GetElementPrefs() { return m_pcElementPrefs; }
	inline SP_GPR_Fonts *GetFontPrefs() { return m_pcFontPrefs; }

    inline void SetMainPos(int p_nX, int p_nY) { m_nXPos = p_nX; m_nYPos = p_nY; }
    inline void SetMainDim(int p_nW, int p_nH) { m_nWidth = p_nW, m_nHeight = p_nH; }
    inline void SetMainMax(bool p_bVal) { m_bMaximize = p_bVal; }

	void SetCanvasLogicBrush(int style);

	double getDelta() const
	{
		return DELTA;
	}

	void setDelta(double delta)
	{
		DELTA = delta;
	}

	double getStdOffset() const
	{
		return STD_OFFSET;
	}

	void setStdOffset(double stdOffset)
	{
		STD_OFFSET = stdOffset;
	}

	void setNetClassesMap(map<wxString,SP_SetString> p_mClasses2Nodes)
	{
		m_mClasses2Nodes = p_mClasses2Nodes;
	}

	map<wxString,SP_SetString> getNetClassesMap()
	{
		return m_mClasses2Nodes;
	}

};

DECLARE_APP(Snoopy)

#endif // __SNOOPY_H__
