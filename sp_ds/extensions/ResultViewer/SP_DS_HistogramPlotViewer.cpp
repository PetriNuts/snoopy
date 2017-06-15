/*
 * SP_DS_HistogramPlotViewer.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 14.06.2011
 * Short Description:
 */ //=================================================

#include "sp_utilities.h"

#include "sp_core/base/SP_Error.h"
#include "sp_core/SP_Vector.h"
#include <wx/xy/xyplot.h>
#include <wx/xy/xylinerenderer.h>
#include <wx/xy/xydynamicdataset.h>
#include <wx/xy/xyhistorenderer.h>
#include <wx/aui/aui.h>
#include <wx/sizer.h>
#include "sp_ds/extensions/ResultViewer/SP_DS_PlotViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_HistogramPlotViewer.h"
#include "sp_ds/extensions/ResultViewer/AttributeTypes/SP_DS_ViewerAttributeText.h"
#include "sp_ds/extensions/ResultViewer/AttributeTypes/SP_DS_ViewerAttributeList.h"
#include "sp_utilities.h"
#include <algorithm>
#include <dssd/auxi/auxi.h>

#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"

SP_DS_HistogramPlotViewer::SP_DS_HistogramPlotViewer(wxWindow* p_pcParent, wxSizer* p_pcSizer) :
		SP_DS_PlotViewer(p_pcParent, p_pcSizer), m_nBarWidth(20), m_nIntervalWidth(0), m_nFrequcyType(0)
{
	m_sViewerName = wxT("Histogram");

	//add bars
	AddViewerAttributeCategory(wxT("Bars"));

	//Add individual attributes
	//Bar width
	SP_DS_ViewerAttributeText* l_pcAttribute = new SP_DS_ViewerAttributeText(wxT("BAR_WIDTH"), wxT("Bars"), wxT("Bar width"));
	l_pcAttribute->SetValueString(wxString::Format(wxT("%ld"), m_nBarWidth));
	m_pcAttributeList->push_back(l_pcAttribute);

	//interval width
	l_pcAttribute = new SP_DS_ViewerAttributeText(wxT("INTERVAL_WIDTH"), wxT("Bars"), wxT("Interval width"));
	l_pcAttribute->SetValueString(dssd::aux::toString(m_nIntervalWidth));
	m_pcAttributeList->push_back(l_pcAttribute);

	SP_DS_ViewerAttributeList* l_pcAttribute1 = new SP_DS_ViewerAttributeList(wxT("FREQUENCY"), wxT("Bars"), wxT("Frequency"));
	l_pcAttribute1->AddToPossibleValues(wxT("Count"));
	l_pcAttribute1->AddToPossibleValues(wxT("Percentage"));
	l_pcAttribute1->SetSelection(0);
	m_pcAttributeList->push_back(l_pcAttribute1);

}

SP_DS_HistogramPlotViewer::~SP_DS_HistogramPlotViewer()
{

}

Chart* SP_DS_HistogramPlotViewer::CreateChart()
{
	std::list<unsigned int>::iterator l_itCurve;

	SP_VectorDouble::const_iterator l_itXValue;

	unsigned long l_nPos = 0;

	SP_VectorDouble l_anColumnValues;
	XYDynamicDataset* l_pcDataSet;

	CHECK_POINTER(m_pcXAxisValues, return NULL);

	XYPlot *l_pcPlot = new XYPlot();

	// create histogram renderer with bar width = 10 and vertical bars
	XYHistoRenderer* l_pcHistoRenderer = new XYHistoRenderer(m_nBarWidth, true);

	// create dataset and add serie to it
	l_pcDataSet = new XYDynamicDataset();

	for (l_itCurve = m_apcShownCurvesList.begin(); l_itCurve != m_apcShownCurvesList.end(); l_itCurve++)
	{
		l_anColumnValues.clear();

		l_nPos = 0;
		for (l_itXValue = m_pcXAxisValues->begin(); l_itXValue != m_pcXAxisValues->end(); l_itXValue++, l_nPos++)
		{

			l_anColumnValues.push_back(m_aPlotCurves[(*l_itCurve)]->GetValue(l_nPos));
		}

		//create a serie
		XYDynamicSerie* l_pcSerie = PreProcessData(l_anColumnValues);

		if (l_pcSerie != NULL)
		{
			l_pcDataSet->AddSerie(l_pcSerie);

			//set serie name
			l_pcSerie->SetName(m_aPlotCurves[(*l_itCurve)]->GetName());
		}
	}

	// set bar areas to renderer
	// in this case, we set green bar with black outline for serie 0
	l_pcHistoRenderer->SetBarArea(0, new FillAreaDraw(*wxBLACK_PEN, *wxRED_BRUSH));
	l_pcHistoRenderer->SetBarArea(1, new FillAreaDraw(*wxBLACK_PEN, *wxGREEN_BRUSH));
	l_pcHistoRenderer->SetBarArea(2, new FillAreaDraw(*wxBLACK_PEN, *wxBLUE_BRUSH));
	l_pcHistoRenderer->SetBarArea(3, new FillAreaDraw(*wxBLACK_PEN, *wxGREY_BRUSH));
	l_pcHistoRenderer->SetBarArea(4, new FillAreaDraw(*wxBLACK_PEN, *wxTRANSPARENT_BRUSH));
	l_pcHistoRenderer->SetBarArea(5, new FillAreaDraw(*wxBLACK_PEN, *wxWHITE_BRUSH));
	l_pcHistoRenderer->SetBarArea(6, new FillAreaDraw(*wxBLACK_PEN, *wxGREEN_BRUSH));
	l_pcHistoRenderer->SetBarArea(7, new FillAreaDraw(*wxBLACK_PEN, *wxGREY_BRUSH));

	l_pcDataSet->SetRenderer(l_pcHistoRenderer);

	//create x and y axes
	NumberAxis *l_pcXAxis = NULL;
	NumberAxis *l_pcYAxis = NULL;

	l_pcXAxis = new NumberAxis(AXIS_BOTTOM);
	l_pcXAxis->SetTitle(m_sXAxisTitle);
	l_pcXAxis->SetLabelCount(11);

	l_pcYAxis = new NumberAxis(AXIS_LEFT);
	l_pcYAxis->SetTitle(m_sYAxisTitle);
	l_pcYAxis->SetLabelCount(11);

	if (m_bFixedXAdjustment == true)
	{
		l_pcXAxis->SetFixedBounds(m_nMinXValue, m_nMaxXValue);
	}
	if (m_bFixedYAdjustment == true)
	{
		l_pcYAxis->SetFixedBounds(m_nMinYValue, m_nMaxYValue);
	}

	// add axes and dataset to plot
	l_pcPlot->AddObjects(l_pcDataSet, l_pcYAxis, l_pcXAxis);

	// set legend to plot
	if (m_bShowLegend == true)
	{
		l_pcPlot->SetLegend(new Legend(m_nLegendVerticalPosition, m_nLegendHorizontalPosition));
	}

	return new Chart(l_pcPlot, m_sTitle);
}

XYDynamicSerie* SP_DS_HistogramPlotViewer::PreProcessData(SP_VectorDouble& p_anColumnData)
{
	unsigned int l_nNumberCount = std::sqrt((double) p_anColumnData.size());
	XYDynamicSerie* l_pcSerie;

	if (l_nNumberCount == 0)
	{
		return NULL;
	}

	//sort the list of numbers
	std::sort(p_anColumnData.begin(), p_anColumnData.end());

	//compute the interval
	if (m_nIntervalWidth <= 0)
	{
		double l_nMaxValue = p_anColumnData[p_anColumnData.size() - 1];
		double l_nMinValue = p_anColumnData[0];

		m_nIntervalWidth = (l_nMaxValue - l_nMinValue) / l_nNumberCount;
	}

	if (m_nIntervalWidth <= 0)
	{
		return NULL;
	}

	l_pcSerie = new XYDynamicSerie();

	CHECK_POINTER(l_pcSerie, return NULL);

	//compute the number of elements in each interval
	SP_VectorDouble::iterator l_itElement;
	double l_nCurrentInterval = p_anColumnData[0] + m_nIntervalWidth;
	double l_nIntervalNumberCount = 0;
	for (l_itElement = p_anColumnData.begin(); l_itElement != p_anColumnData.end(); l_itElement++)
	{
		if (l_nCurrentInterval < (*l_itElement))
		{
			//in case of percentage frequency
			if (m_nFrequcyType == 1)
			{
				l_nIntervalNumberCount = (l_nIntervalNumberCount / p_anColumnData.size()) * 100;
			}
			//Add this point to the data serie
			l_pcSerie->AddXY(l_nCurrentInterval, l_nIntervalNumberCount);

			//go to the next interval
			while (l_nCurrentInterval < (*l_itElement))
			{
				l_nCurrentInterval += m_nIntervalWidth;
			}

			//reset interval count
			l_nIntervalNumberCount = 0;
		}

		l_nIntervalNumberCount++;
	}

	//in case of percentage frequency
	if (m_nFrequcyType == 1)
	{
		l_nIntervalNumberCount = (l_nIntervalNumberCount / p_anColumnData.size()) * 100;
	}
	//Add the last point
	l_pcSerie->AddXY(l_nCurrentInterval, l_nIntervalNumberCount);

	return l_pcSerie;
}
void SP_DS_HistogramPlotViewer::UpdateAttributes()
{
	SP_DS_PlotViewer::UpdateAttributes();

	//bar width
	if (GetAttributeByName(wxT("BAR_WIDTH")) != NULL)
	{
		wxString l_sBarWidth = (dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("BAR_WIDTH"))))->GetValueString();

		//get long representation
		l_sBarWidth.ToLong(&m_nBarWidth);
	}

	if (m_nBarWidth < 1 || m_nBarWidth > 200)
	{
		m_nBarWidth = 20;

		SP_MESSAGEBOX(wxT("Invalid bar width. default value will be used"));
	}

	//interval width
	if (GetAttributeByName(wxT("INTERVAL_WIDTH")) != NULL)
	{
		wxString l_sIntervalWidth = (dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("INTERVAL_WIDTH"))))->GetValueString();

		//get long representation
		l_sIntervalWidth.ToDouble(&m_nIntervalWidth);
	}

	if (m_nIntervalWidth > 20000)
	{
		m_nIntervalWidth = 0;

		SP_MESSAGEBOX(wxT("Invalid interval width. default value will be used"));
	}

	//Frequency type
	if (GetAttributeByName(wxT("FREQUENCY")) != NULL)
	{
		m_nFrequcyType = (dynamic_cast<SP_DS_ViewerAttributeList*>(GetAttributeByName(wxT("FREQUENCY"))))->GetSelection();
	}

}

void SP_DS_HistogramPlotViewer::LoadCurrentAttributeValues()
{
	SP_DS_PlotViewer::LoadCurrentAttributeValues();

	//Show legend
	if (GetAttributeByName(wxT("BAR_WIDTH")) != NULL)
	{
		(dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("BAR_WIDTH"))))->SetValueString(wxString::Format(wxT("%d"), m_nBarWidth));
	}

	//interval width
	if (GetAttributeByName(wxT("INTERVAL_WIDTH")) != NULL)
	{
		(dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("INTERVAL_WIDTH"))))->SetValueString(wxString::Format(wxT("%5.2g"), m_nIntervalWidth));
	}

	//Frequency type
	if (GetAttributeByName(wxT("FREQUENCY")) != NULL)
	{
		(dynamic_cast<SP_DS_ViewerAttributeList*>(GetAttributeByName(wxT("FREQUENCY"))))->SetSelection(m_nFrequcyType);
	}
}
void SP_DS_HistogramPlotViewer::SaveViewToSnoopyFormat(SP_DS_Metadata* p_pcView)
{
	SP_DS_PlotViewer::SaveViewToSnoopyFormat(p_pcView);

	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(wxT("BarWidth"));
	CHECK_POINTER(l_pcAttribute, return);
	(dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttribute))->SetValue(m_nBarWidth);

	l_pcAttribute = p_pcView->GetAttribute(wxT("IntervalWidth"));
	CHECK_POINTER(l_pcAttribute, return);
	(dynamic_cast<SP_DS_DoubleAttribute*>(l_pcAttribute))->SetValue(m_nIntervalWidth);

	//histogram frequency type
	l_pcAttribute = p_pcView->GetAttribute(wxT("HistogramFrequencyType"));
	CHECK_POINTER(l_pcAttribute, return);
	(dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttribute))->SetValue(m_nFrequcyType);

}
void SP_DS_HistogramPlotViewer::LoadViewFromSnoopyFormat(SP_DS_Metadata* p_pcView)
{
	SP_DS_PlotViewer::LoadViewFromSnoopyFormat(p_pcView);

	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(wxT("BarWidth"));
	CHECK_POINTER(l_pcAttribute, return);
	m_nBarWidth = (dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttribute))->GetValue();

	l_pcAttribute = p_pcView->GetAttribute(wxT("IntervalWidth"));
	CHECK_POINTER(l_pcAttribute, return);
	m_nIntervalWidth = (dynamic_cast<SP_DS_DoubleAttribute*>(l_pcAttribute))->GetValue();

	//histogram frequency type
	l_pcAttribute = p_pcView->GetAttribute(wxT("HistogramFrequencyType"));
	CHECK_POINTER(l_pcAttribute, return);
	m_nFrequcyType = (dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttribute))->GetValue();
}

