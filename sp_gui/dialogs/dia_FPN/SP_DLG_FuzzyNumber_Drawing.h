// @Author: George Asssaf
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 02.03.2020
// Short Description:Fuzzy coloured Cont. Petri Net Class
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FUZZY_Number_Drawing__
#define __SP_DS_FUZZY_Number_Drawing__
#include <wx/aui/aui.h>
#include <wx/xy/xyplot.h>

#include "wx/wxfreechartdefs.h"
#include "wx/chartpanel.h"
#include <wx/xy/xyplot.h>
#include <wx/areadraw.h>
//#include "sp_ds/extensions/ResultViewer/SP_DS_ResultViewer.h"
#include "wx/wx.h"
#include <wx/event.h>
#include <wx/xy/xylinerenderer.h>
#include <wx/xy/xyarearenderer.h>
#include <wx/xy/xydynamicdataset.h>
#include <wx/xy/xyplot.h>
#include <wx/spinctrl.h>
class SP_DLG_FuzzyNumber_Drawing : public wxFrame/*, public wxEvtHandler */{
protected:
	wxChartPanel*      m_pcDrawingPanel;
	wxStaticBoxSizer*	m_pcScroll;
	wxScrollBar* scrollBar;
	wxSizer*     m_pcSizer;
	wxMenuBar *m_pMenuBar;
	wxMenu *m_pFileMenu;
private:
	XYDynamicDataset*  m_pcDataSet;
	XYDynamicSerie     *m_pcSerie;
	Chart              * m_pcChart;
 
	wxSpinCtrlDouble* m_pctxtCtrlA;

 
	wxSpinCtrlDouble* m_pctxtCtrlB;

 
	wxSpinCtrlDouble* m_pctxtCtrlC;

	double m_dA;
	double m_dB;
	double m_dC;
public:
	double GetAVal() { return m_dA; }
	double GetBVal() { return m_dB; }
	double GetCVal() { return m_dC; }

	void SetAVal(wxString& l_sTexTA);
	void SetBVal(wxString& l_sTexTB);
	void SetCVal(wxString& l_sTexTC);
	void OnCloseDrsawingFrame(wxCloseEvent& ev);
private:
 
	void  OnSpinChanged(wxSpinDoubleEvent& event);
	void  OnSavePlot(wxCommandEvent& event);
	void  ExportToImageFile(wxString &p_sFileName, const int &p_nFileType);
	void init();
	Chart* CreateFuzzzyNumChart();

public:
	
	SP_DLG_FuzzyNumber_Drawing(wxWindow *p_pcParent, wxString& p_sTitle, int xpos, int ypos, int width, int height);

	
	DECLARE_EVENT_TABLE()
};
#endif // __SP_DS_FUZZY_Number_Drawing__