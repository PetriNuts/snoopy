/*
 * SP_DS_HistogramPlotViewer.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 14.06.2011
 * Short Description:
 *///=================================================

#ifndef SP_DS_HISTOGRAMPLOTVIEWER_H_
#define SP_DS_HISTOGRAMPLOTVIEWER_H_

#include <wx/xy/xydynamicdataset.h>

class SP_DS_HistogramPlotViewer:public SP_DS_PlotViewer
{
protected:
	     long m_nBarWidth;

		 double m_nIntervalWidth;

		 long m_nFrequcyType;
protected:
	     /*
		 * preprocess dats
		 */
	     XYDynamicSerie* PreProcessData(SP_VectorDouble& p_anColumnData);
public:
		SP_DS_HistogramPlotViewer(wxWindow* p_pcParent,wxSizer* p_pcSizer);
		virtual ~SP_DS_HistogramPlotViewer();

		//update attributes
		virtual void UpdateAttributes();

		//set the attribute values to the current one
		virtual void LoadCurrentAttributeValues();

		//Create a plot
		virtual Chart* CreateChart();
		virtual Chart* CreateFuzzyPlot();// Added by G.A
	    virtual Chart* CreateMembershipFunction(double timepoint);

		virtual void SaveViewToSnoopyFormat(SP_DS_Metadata* p_pcView);
		virtual void LoadViewFromSnoopyFormat(SP_DS_Metadata* p_pcView);
};

#endif /* SP_DS_HISTOGRAMPLOTVIEWER_H_ */
