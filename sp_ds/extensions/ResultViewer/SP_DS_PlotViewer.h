/*
 * SP_DS_PlotViewer.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 06.06.2011
 * Short Description:
 *///=================================================
#ifndef SP_DS_PLOTVIEWER_H_
#define SP_DS_PLOTVIEWER_H_


#include "wx/wxfreechartdefs.h"
#include "wx/chartpanel.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_ResultViewer.h"

#include <wx/event.h>

/*
 * Base class for all plot viewer.
 */
class SP_DS_PlotViewer: public SP_DS_ResultViewer, public wxEvtHandler
{
protected :

	      //Chart panel where the chart will be created

	  wxChartPanel *m_pcChartPanel;

	      //plot legend state
	      bool m_bShowLegend;

	      int m_nLegendHorizontalPosition;
	      int m_nLegendVerticalPosition;

	      wxString m_sXAxisTitle;
		  wxString m_sYAxisTitle;

		  wxString m_ChartWidth;
		  wxString m_ChartHeight;

		  bool m_bShowXAxis;
		  bool m_bShowYAxis;

		  int m_nCurveOrder;

		  bool m_bFixedXAdjustment;
		  bool m_bFixedYAdjustment;

		  double m_nMinXValue;
		  double m_nMaxXValue;

		  double m_nMinYValue;
		  double m_nMaxYValue;
protected:
            static bool SortDescendingByPosition(unsigned int p_nElement1,unsigned int p_nElement2){return p_nElement1>p_nElement2;}

public:
		  SP_DS_PlotViewer(wxWindow* p_pcParent,wxSizer* p_pcSizer);
		  virtual ~SP_DS_PlotViewer();
		  //Update a viewer
		  virtual void Update();

		  //Create a viewer window
		  virtual void Create();

		  //destroy a viewer window
		  virtual void Destroy();

		  //Create a chart
		  virtual Chart* CreateChart()=0;

		  virtual void UpdateAttributes();
		  
		  //set the attribute values to the current one
		  virtual void LoadCurrentAttributeValues();

		  virtual int GetPosition(const int& p_nIndex);
		  virtual int GetSelection(const int& p_nValue);

		  virtual void SaveOutput(const wxString& p_sFileName,const int& p_sFileType);

		  virtual void SaveViewToServerFormat(spsa::ModelView* p_pcServerSideView);
		  virtual void LoadViewFromServerFormat(spsa::ModelView* p_pcServerSideView);

		  //Load and save from Snoopy formate
		  virtual void SaveViewToSnoopyFormat(SP_DS_Metadata* p_pcView);
		  virtual void LoadViewFromSnoopyFormat(SP_DS_Metadata* p_pcView);
		  
		  //export the plot to an image file
		  virtual void ExportToImageFile(wxString& p_sFileName,const int& p_nFileType);
};

#endif /* SP_DS_PLOTVIEWER_H_ */
