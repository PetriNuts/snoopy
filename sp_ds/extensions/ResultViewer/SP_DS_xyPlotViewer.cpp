/*
 * SP_DS_xyPlotViewer.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 10.06.2011
 * Short Description:
 */ //=================================================
#include "sp_defines.h"
#include "sp_core/base/SP_Error.h"
#include "sp_core/SP_Vector.h"
#include <wx/xy/xyplot.h>
#include <wx/xy/xylinerenderer.h>
#include <wx/xy/xydynamicdataset.h>
#include <wx/aui/aui.h>
#include <wx/sizer.h>
#include "sp_ds/extensions/ResultViewer/SP_DS_PlotViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_xyPlotViewer.h"
#include "sp_ds/extensions/ResultViewer/AttributeTypes/SP_DS_ViewerAttributeCheckBox.h"
#include "sp_ds/extensions/ResultViewer/AttributeTypes/SP_DS_ViewerAttributeList.h"
#include "sp_ds/extensions/ResultViewer/AttributeTypes/SP_DS_ViewerAttributeText.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/SP_DS_Graph.h"

using namespace spsa;

SP_DS_xyPlotViewer::SP_DS_xyPlotViewer(wxWindow* p_pcParent, wxSizer* p_pcSizer) :
		SP_DS_PlotViewer(p_pcParent, p_pcSizer),
		m_nDefaultLineWidth(2), m_nDefaultLineStyle(wxSOLID),
		m_bShowSymbols(false), m_bShowLines(true)
{
	m_sViewerName = wxT("xy plot");

	AddViewerAttributeCategory(wxT("Curves"));

	//show lines
	SP_DS_ViewerAttributeCheckBox* l_pcAttributeCheckBox = new SP_DS_ViewerAttributeCheckBox(wxT("SHOW_LINES"), wxT("Curves"), wxT("Show lines"));
	l_pcAttributeCheckBox->SetValue(m_bShowLines);
	m_pcAttributeList->push_back(l_pcAttributeCheckBox);

	//show symbols
	l_pcAttributeCheckBox = new SP_DS_ViewerAttributeCheckBox(wxT("SHOW_SYMBOLS"), wxT("Curves"), wxT("Show symbols"));
	l_pcAttributeCheckBox->SetValue(m_bShowSymbols);
	m_pcAttributeList->push_back(l_pcAttributeCheckBox);

	//line width
	SP_DS_ViewerAttributeList* l_pcAttribute = new SP_DS_ViewerAttributeList(wxT("LINE_WIDTH"), wxT("Curves"), wxT("Line width"));

	for (int i = 1; i < 21; i++)
	{
		l_pcAttribute->AddToPossibleValues(wxString::Format(wxT("%d"), i));
	}

	l_pcAttribute->SetSelection(1);
	m_pcAttributeList->push_back(l_pcAttribute);

	//Line styles
	l_pcAttribute = new SP_DS_ViewerAttributeList(wxT("LINE_STYLE"), wxT("Curves"), wxT("Line style"));
	l_pcAttribute->AddToPossibleValues(wxT("Solid"));
	l_pcAttribute->AddToPossibleValues(wxT("Dot"));
	l_pcAttribute->AddToPossibleValues(wxT("Long Dash"));
	l_pcAttribute->AddToPossibleValues(wxT("Short Dash"));
	l_pcAttribute->AddToPossibleValues(wxT("Dot Dash"));
	l_pcAttribute->SetSelection(0);
	m_pcAttributeList->push_back(l_pcAttribute);
}

SP_DS_xyPlotViewer::~SP_DS_xyPlotViewer()
{

}

Chart* SP_DS_xyPlotViewer::CreateChart()
{
	std::list<unsigned int>::iterator l_itCurve;

	SP_VectorDouble::const_iterator l_itXValue;

	unsigned long l_nPos = 0;

	XYDynamicDataset* l_pcDataSet;

	CHECK_POINTER(m_pcXAxisValues, return NULL);

	l_pcDataSet = new XYDynamicDataset();

	for (l_itCurve = m_apcShownCurvesList.begin(); l_itCurve != m_apcShownCurvesList.end(); l_itCurve++)
	{
		XYDynamicSerie *l_pcSerie = new XYDynamicSerie();
		l_nPos = 0;

		for (l_itXValue = m_pcXAxisValues->begin(); l_itXValue != m_pcXAxisValues->end(); l_itXValue++, l_nPos++)
		{
			l_pcSerie->AddXY((*l_itXValue), m_aPlotCurves[(*l_itCurve)]->GetValue(l_nPos));
		}

		l_pcSerie->SetName(m_aPlotCurves[(*l_itCurve)]->GetName());

		// and add serie to it
		l_pcDataSet->AddSerie(l_pcSerie);

	}

	XYPlot *l_pcPlot = new XYPlot();

	XYLineRenderer* l_pcLineRenderer = new XYLineRenderer(m_bShowSymbols, m_bShowLines, m_nDefaultLineWidth, m_nDefaultLineStyle);

	//Add curve attributes
	int l_nSerie = 0;
	for (l_itCurve = m_apcShownCurvesList.begin(); l_itCurve != m_apcShownCurvesList.end(); l_itCurve++)
	{
		//get curve information
		wxColour l_nColor(m_aPlotCurves[(*l_itCurve)]->GetColor());

		int l_nLineWidth = m_aPlotCurves[(*l_itCurve)]->GetLineWidth();
		l_nLineWidth = l_nLineWidth < 0 ? m_nDefaultLineWidth : l_nLineWidth;

		int l_nLineStyle = m_aPlotCurves[(*l_itCurve)]->GetLineStyle();
		l_nLineStyle = l_nLineStyle < wxSOLID ? m_nDefaultLineStyle : l_nLineStyle;

		//create a pen
		wxPen l_pen(l_nColor, l_nLineWidth, l_nLineStyle);

		l_pcLineRenderer->SetSeriePen(l_nSerie, &l_pen);

		l_nSerie++;
	}

	// set line renderer to dataset
	l_pcDataSet->SetRenderer(l_pcLineRenderer);

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
		Legend* l_pcLegend = new Legend(m_nLegendVerticalPosition, m_nLegendHorizontalPosition);

		l_pcPlot->SetLegend(l_pcLegend);
	}

	//connect the event handler to this class
	//m_pcChartPanel->Connect(wxEVT_MOTION,wxMouseEventHandler(SP_DS_xyPlotViewer::OnMouseMotion));

	return new Chart(l_pcPlot, m_sTitle);
}

void SP_DS_xyPlotViewer::OnMouseMotion(wxMouseEvent& event)
{
	/*wxCoord l_XPosition, l_YPosition;

	 CHECK_POINTER(m_pcChartPanel,return);

	 event.GetPosition(&l_XPosition,&l_YPosition);

	 CHECK_POINTER(m_pcChart,return);

	 XYPlot* l_pcPlot;//=dynamic_cast<XYPlot*> (m_pcChart->GetPlot());

	 wxPaintDC dc(m_pcChartPanel);

	 wxRect rc;

	 m_pcChart->CalcPlotRect(dc,rc);

	 double l_nX,l_nY;

	 l_pcPlot->ToDataCoords(1,dc,rc,l_XPosition,l_YPosition,&l_nX,&l_nY);

	 wxString l_sInfo=SP_NUMBER_TO_STR(l_nX,wxT("%f"))+wxT(",")+SP_NUMBER_TO_STR(l_nY,wxT("%f"));

	 SP_LOG(wxLOG_Message,l_sInfo);*/
}

void SP_DS_xyPlotViewer::UpdateAttributes()
{
	int l_anLineStyle[] =
	{ wxSOLID, wxDOT, wxLONG_DASH, wxSHORT_DASH, wxDOT_DASH };

	SP_DS_PlotViewer::UpdateAttributes();

	//show lines
	if (GetAttributeByName(wxT("SHOW_LINES")) != NULL)
	{
		m_bShowLines = (dynamic_cast<SP_DS_ViewerAttributeCheckBox*>(GetAttributeByName(wxT("SHOW_LINES"))))->GetValue();
	}

	//show symbols
	if (GetAttributeByName(wxT("SHOW_SYMBOLS")) != NULL)
	{
		m_bShowSymbols = (dynamic_cast<SP_DS_ViewerAttributeCheckBox*>(GetAttributeByName(wxT("SHOW_SYMBOLS"))))->GetValue();
	}

	//Line width
	if (GetAttributeByName(wxT("LINE_WIDTH")) != NULL)
	{
		m_nDefaultLineWidth = (dynamic_cast<SP_DS_ViewerAttributeList*>(GetAttributeByName(wxT("LINE_WIDTH"))))->GetSelection() + 1;
	}

	//Line style
	if (GetAttributeByName(wxT("LINE_STYLE")) != NULL)
	{
		int l_nSelectedIndex = (dynamic_cast<SP_DS_ViewerAttributeList*>(GetAttributeByName(wxT("LINE_STYLE"))))->GetSelection();
		m_nDefaultLineStyle = l_anLineStyle[l_nSelectedIndex];
	}
}

void SP_DS_xyPlotViewer::LoadCurrentAttributeValues()
{
	SP_DS_PlotViewer::LoadCurrentAttributeValues();

	//show lines
	if (GetAttributeByName(wxT("SHOW_LINES")) != NULL)
	{
		(dynamic_cast<SP_DS_ViewerAttributeCheckBox*>(GetAttributeByName(wxT("SHOW_LINES"))))->SetValue(m_bShowLines);
	}

	//show symbols
	if (GetAttributeByName(wxT("SHOW_SYMBOLS")) != NULL)
	{
		(dynamic_cast<SP_DS_ViewerAttributeCheckBox*>(GetAttributeByName(wxT("SHOW_SYMBOLS"))))->SetValue(m_bShowSymbols);
	}

	//Line width
	if (GetAttributeByName(wxT("LINE_WIDTH")) != NULL)
	{
		(dynamic_cast<SP_DS_ViewerAttributeList*>(GetAttributeByName(wxT("LINE_WIDTH"))))->SetSelection(m_nDefaultLineWidth - 1);
	}

	//Line style
	if (GetAttributeByName(wxT("LINE_STYLE")) != NULL)
	{
		SP_MapLong2Long l_mLineStyle2Selection;

		l_mLineStyle2Selection[wxSOLID] = 0;
		l_mLineStyle2Selection[wxDOT] = 1;
		l_mLineStyle2Selection[wxLONG_DASH] = 2;
		l_mLineStyle2Selection[wxSHORT_DASH] = 3;
		l_mLineStyle2Selection[wxDOT_DASH] = 4;
		int l_nSel = l_mLineStyle2Selection[m_nDefaultLineStyle];
		(dynamic_cast<SP_DS_ViewerAttributeList*>(GetAttributeByName(wxT("LINE_STYLE"))))->SetSelection(l_nSel);
	}

}

void SP_DS_xyPlotViewer::SaveViewToServerFormat(spsa::ModelView* p_pcServerSideView)
{
	Attribute* l_pcAttribute = NULL;

	SP_DS_PlotViewer::SaveViewToServerFormat(p_pcServerSideView);

	//default line width
	l_pcAttribute = p_pcServerSideView->GetAttribute(wxT("DEFAULT_LINE_WIDTH"));
	CHECK_POINTER(l_pcAttribute, return);
	(dynamic_cast<AttributeInt*>(l_pcAttribute))->SetValue(m_nDefaultLineWidth);

	//default line style
	l_pcAttribute = p_pcServerSideView->GetAttribute(wxT("DEFAULT_LINE_STYLE"));
	CHECK_POINTER(l_pcAttribute, return);
	(dynamic_cast<AttributeInt*>(l_pcAttribute))->SetValue(m_nDefaultLineStyle);
}
void SP_DS_xyPlotViewer::LoadViewFromServerFormat(spsa::ModelView* p_pcServerSideView)
{
	Attribute* l_pcAttribute = NULL;

	SP_DS_PlotViewer::LoadViewFromServerFormat(p_pcServerSideView);

	//default line width
	l_pcAttribute = p_pcServerSideView->GetAttribute(wxT("DEFAULT_LINE_WIDTH"));
	CHECK_POINTER(l_pcAttribute, return);
	m_nDefaultLineWidth = (dynamic_cast<AttributeInt*>(l_pcAttribute))->GetValue();

	//default line style
	l_pcAttribute = p_pcServerSideView->GetAttribute(wxT("DEFAULT_LINE_STYLE"));
	CHECK_POINTER(l_pcAttribute, return);
	m_nDefaultLineStyle = (dynamic_cast<AttributeInt*>(l_pcAttribute))->GetValue();
}

void SP_DS_xyPlotViewer::SaveViewToSnoopyFormat(SP_DS_Metadata* p_pcView)
{
	SP_DS_PlotViewer::SaveViewToSnoopyFormat(p_pcView);

	//Show line
	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(wxT("ShowLines"));
	CHECK_POINTER(l_pcAttribute, return);
	(dynamic_cast<SP_DS_BoolAttribute*>(l_pcAttribute))->SetValue(m_bShowLines);

	//Show Symbols
	l_pcAttribute = p_pcView->GetAttribute(wxT("ShowSymbols"));
	CHECK_POINTER(l_pcAttribute, return);
	(dynamic_cast<SP_DS_BoolAttribute*>(l_pcAttribute))->SetValue(m_bShowSymbols);

	//line width
	l_pcAttribute = p_pcView->GetAttribute(wxT("DefaultLineWidth"));
	CHECK_POINTER(l_pcAttribute, return);
	(dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttribute))->SetValue(m_nDefaultLineWidth);

	//line style
	l_pcAttribute = p_pcView->GetAttribute(wxT("DefaultLineStyle"));
	CHECK_POINTER(l_pcAttribute, return);
	(dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttribute))->SetValue(m_nDefaultLineStyle);
}
void SP_DS_xyPlotViewer::LoadViewFromSnoopyFormat(SP_DS_Metadata* p_pcView)
{
	SP_DS_PlotViewer::LoadViewFromSnoopyFormat(p_pcView);

	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(wxT("ShowLines"));
	CHECK_POINTER(l_pcAttribute, return);
	m_bShowLines = (dynamic_cast<SP_DS_BoolAttribute*>(l_pcAttribute))->GetValue();

	l_pcAttribute = p_pcView->GetAttribute(wxT("ShowSymbols"));
	CHECK_POINTER(l_pcAttribute, return);
	m_bShowSymbols = (dynamic_cast<SP_DS_BoolAttribute*>(l_pcAttribute))->GetValue();

	//line width
	l_pcAttribute = p_pcView->GetAttribute(wxT("DefaultLineWidth"));
	CHECK_POINTER(l_pcAttribute, return)
	m_nDefaultLineWidth = (dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttribute))->GetValue();

	//line style
	l_pcAttribute = p_pcView->GetAttribute(wxT("DefaultLineStyle"));
	CHECK_POINTER(l_pcAttribute, return)
	m_nDefaultLineStyle = (dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttribute))->GetValue();
}
