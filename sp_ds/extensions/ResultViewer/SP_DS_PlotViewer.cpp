/*
 * SP_DS_PlotViewer.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 06.06.2011
 * Short Description:
 *///=================================================

#include "sp_defines.h"
#include "sp_core/base/SP_Error.h"
#include "sp_core/SP_Vector.h"
#include <wx/aui/aui.h>
#include <wx/sizer.h>
#include "sp_ds/SP_DS_Graph.h"
#include <wx/xy/xyplot.h>

#include "sp_ds/extensions/ResultViewer/SP_DS_PlotViewer.h"
#include "sp_ds/extensions/ResultViewer/AttributeTypes/SP_DS_ViewerAttributeCheckBox.h"
#include "sp_ds/extensions/ResultViewer/AttributeTypes/SP_DS_ViewerAttributeList.h"
#include "sp_ds/extensions/ResultViewer/AttributeTypes/SP_DS_ViewerAttributeText.h"
#include <wx/dcps.h>

#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include <algorithm>
#include <wx/event.h>

enum
{
	SP_ID_PLOT_WINDOW_ID=2000
};

SP_DS_PlotViewer::SP_DS_PlotViewer(wxWindow* p_pcParent,wxSizer* p_pcSizer)
:SP_DS_ResultViewer(p_pcParent,p_pcSizer),
 m_pcChartPanel(NULL),
m_bShowLegend(true),
m_nLegendHorizontalPosition(wxRIGHT),
m_nLegendVerticalPosition(wxCENTER),
m_sXAxisTitle(wxT("")),
m_sYAxisTitle(wxT("")),
m_bShowXAxis(true),
m_bShowYAxis(true),
m_nCurveOrder(0),
m_bFixedXAdjustment(false),
m_bFixedYAdjustment(false),
m_nMinXValue(0),
m_nMaxXValue(1),
m_nMinYValue(0),
m_nMaxYValue(1)
{
SP_DS_ViewerAttribute* l_pcAttribute;

             AddViewerAttributeCategory(wxT("Legend"));

             l_pcAttribute=new SP_DS_ViewerAttributeCheckBox(wxT("SHOW_LEGEND"),wxT("Legend"),wxT("Show Legend"));

			 m_pcAttributeList->push_back(l_pcAttribute);

			 //legend horizontal position
			 l_pcAttribute=new SP_DS_ViewerAttributeList(wxT("LEGEND_HORIZONTAL_POS"),wxT("Legend"),wxT("Horizontal position"));
			 (dynamic_cast<SP_DS_ViewerAttributeList*>(l_pcAttribute))->AddToPossibleValues(wxT("Left"));
			 (dynamic_cast<SP_DS_ViewerAttributeList*>(l_pcAttribute))->AddToPossibleValues(wxT("Center"));
			 (dynamic_cast<SP_DS_ViewerAttributeList*>(l_pcAttribute))->AddToPossibleValues(wxT("Right"));
			 (dynamic_cast<SP_DS_ViewerAttributeList*>(l_pcAttribute))->SetSelection(2);

			 m_pcAttributeList->push_back(l_pcAttribute);

			 //legend vertical position
			 l_pcAttribute=new SP_DS_ViewerAttributeList(wxT("LEGEND_VERTICAL_POS"),wxT("Legend"),wxT("Vertical position"));

			 (dynamic_cast<SP_DS_ViewerAttributeList*>(l_pcAttribute))->AddToPossibleValues(wxT("Top"));
			 (dynamic_cast<SP_DS_ViewerAttributeList*>(l_pcAttribute))->AddToPossibleValues(wxT("Center"));
			 (dynamic_cast<SP_DS_ViewerAttributeList*>(l_pcAttribute))->AddToPossibleValues(wxT("Bottom"));

			 (dynamic_cast<SP_DS_ViewerAttributeList*>(l_pcAttribute))->SetSelection(1);

			 m_pcAttributeList->push_back(l_pcAttribute);

			 //Curve Order
			 l_pcAttribute=new SP_DS_ViewerAttributeList(wxT("CURVE_SORT"),wxT("Legend"),wxT("Sort curves  "));
			 (dynamic_cast<SP_DS_ViewerAttributeList*>(l_pcAttribute))->AddToPossibleValues(wxT("ascending"));
			 (dynamic_cast<SP_DS_ViewerAttributeList*>(l_pcAttribute))->AddToPossibleValues(wxT("descending"));
			 (dynamic_cast<SP_DS_ViewerAttributeList*>(l_pcAttribute))->AddToPossibleValues(wxT("not sorted"));
			 (dynamic_cast<SP_DS_ViewerAttributeList*>(l_pcAttribute))->SetSelection(0);
			 m_pcAttributeList->push_back(l_pcAttribute);

			 //Axes
			 AddViewerAttributeCategory(wxT("Axes"));



			 SP_DS_ViewerAttributeCheckBox* l_pcAttributeCheckBox=new SP_DS_ViewerAttributeCheckBox(wxT("SHOW_X_AXIS"),wxT("Axes"),wxT("Show X Axis"));
			 m_pcAttributeList->push_back(l_pcAttributeCheckBox);

			 SP_DS_ViewerAttributeText* l_pcAttributeTextBox=new SP_DS_ViewerAttributeText(wxT("X_AXIS_LABEL"),wxT("Axes"),wxT("X Axis Title"));
			 m_pcAttributeList->push_back(l_pcAttributeTextBox);


			 l_pcAttributeCheckBox=new SP_DS_ViewerAttributeCheckBox(wxT("SHOW_Y_AXIS"),wxT("Axes"),wxT("Show Y Axis"));
			 m_pcAttributeList->push_back(l_pcAttributeCheckBox);

			 l_pcAttributeTextBox=new SP_DS_ViewerAttributeText(wxT("Y_AXIS_LABEL"),wxT("Axes"),wxT("Y Axis Title"));
			 m_pcAttributeList->push_back(l_pcAttributeTextBox);

			 AddViewerAttributeCategory(wxT("Fixed"));

		    //fixed X axes
		    l_pcAttributeCheckBox=new SP_DS_ViewerAttributeCheckBox(wxT("USE_FIXED_X_ADJUSTMENT"),wxT("Fixed"),wxT("Fixed X axis adjustment"));
		    l_pcAttributeCheckBox->SetValue(m_bFixedXAdjustment);
		    m_pcAttributeList->push_back(l_pcAttributeCheckBox);

		    //Min. X values
		    l_pcAttributeTextBox=new SP_DS_ViewerAttributeText(wxT("MIN_X_VALUE"),wxT("Fixed"),wxT("Min. X value"));
		    l_pcAttributeTextBox->SetValueString(wxString::Format(wxT("%.3f"),m_nMinXValue));
		    m_pcAttributeList->push_back(l_pcAttributeTextBox);

		   //Max. X values
		   l_pcAttributeTextBox=new SP_DS_ViewerAttributeText(wxT("MAX_X_VALUE"),wxT("Fixed"),wxT("Max. X value"));
		   l_pcAttributeTextBox->SetValueString(wxString::Format(wxT("%.3f"),m_nMaxXValue));
		   m_pcAttributeList->push_back(l_pcAttributeTextBox);

		   //fixed Y axes
		    l_pcAttributeCheckBox=new SP_DS_ViewerAttributeCheckBox(wxT("USE_FIXED_Y_ADJUSTMENT"),wxT("Fixed"),wxT("Fixed Y axis adjustment"));
		    l_pcAttributeCheckBox->SetValue(m_bFixedYAdjustment);
		    m_pcAttributeList->push_back(l_pcAttributeCheckBox);

		   //Min. Y values
		   l_pcAttributeTextBox=new SP_DS_ViewerAttributeText(wxT("MIN_Y_VALUE"),wxT("Fixed"),wxT("Min. Y value"));
		   l_pcAttributeTextBox->SetValueString(wxString::Format(wxT("%.3f"),m_nMinYValue));
		   m_pcAttributeList->push_back(l_pcAttributeTextBox);

		   //Max. Y values
		   l_pcAttributeTextBox=new SP_DS_ViewerAttributeText(wxT("MAX_Y_VALUE"),wxT("Fixed"),wxT("Max. Y value"));
		   l_pcAttributeTextBox->SetValueString(wxString::Format(wxT("%.3f"),m_nMaxYValue));
		   m_pcAttributeList->push_back(l_pcAttributeTextBox);

		   m_sSupportedSaveExtensions=wxT("Bitmap(*.bmp)|*.bmp | GIF (*.gif)|*.gif|PNG files (*.png)|*.png| jpg(*.jpg)|*.jpeg|Postscript(*.ps)|*.ps");
			 
}
SP_DS_PlotViewer::~SP_DS_PlotViewer()
{
	     Destroy();
}

void SP_DS_PlotViewer::Create()
{
	          //Create a new chart panel
		      m_pcChartPanel = new wxChartPanel(m_pcParent,SP_ID_PLOT_WINDOW_ID,NULL,wxDefaultPosition, wxSize( 600, 600 ));

		      m_pcSizer->Add(m_pcChartPanel, 1, wxEXPAND | wxALL, 5);

		      m_pcSizer->Layout();
}

void SP_DS_PlotViewer::Destroy()
{
	      SP_DS_ResultViewer::Destroy();

	      m_pcChartPanel=NULL;

}
void SP_DS_PlotViewer::Update()
{
Chart* l_pcChart;

          //check if we need to sort the curves
		switch(m_nCurveOrder)
		{
		   case 0:
		           m_apcShownCurvesList.sort();
				 break;
		   case 1:
		           m_apcShownCurvesList.sort(SortDescendingByPosition);
		           break;
		  default:
			      break;

		}

      l_pcChart=CreateChart();

	   CHECK_POINTER(l_pcChart,return);

	   m_pcChartPanel->SetChart(l_pcChart);
}
void SP_DS_PlotViewer::UpdateAttributes()
{
	      SP_DS_ResultViewer::UpdateAttributes();

		  //Show legend
		  if(GetAttributeByName(wxT("SHOW_LEGEND"))!=NULL)
		  {
			   m_bShowLegend=(dynamic_cast<SP_DS_ViewerAttributeCheckBox*>(GetAttributeByName(wxT("SHOW_LEGEND"))))->GetValue();
		  }

		  //Legend position(Horizontal)
		  if(GetAttributeByName(wxT("LEGEND_HORIZONTAL_POS"))!=NULL)
		  {
			  m_nLegendHorizontalPosition=GetPosition(
					                                   (dynamic_cast<SP_DS_ViewerAttributeList*>
			                                           (GetAttributeByName(wxT("LEGEND_HORIZONTAL_POS"))))
			                                            ->GetSelection()
			                                         );
		  }

		  //Legend position(Vertical)
		  if(GetAttributeByName(wxT("LEGEND_VERTICAL_POS"))!=NULL)
		  {
			  m_nLegendVerticalPosition=GetPosition(
													   (dynamic_cast<SP_DS_ViewerAttributeList*>
													   (GetAttributeByName(wxT("LEGEND_VERTICAL_POS"))))
														->GetSelection()+3
													 );
		  }

		  //Show X Axis
		  if(GetAttributeByName(wxT("SHOW_X_AXIS"))!=NULL)
		  {
			  m_bShowXAxis=(dynamic_cast<SP_DS_ViewerAttributeCheckBox*>(GetAttributeByName(wxT("SHOW_X_AXIS"))))->GetValue();
		  }

		  //Show Y Axis
		  if(GetAttributeByName(wxT("SHOW_Y_AXIS"))!=NULL)
		  {
			  m_bShowYAxis=(dynamic_cast<SP_DS_ViewerAttributeCheckBox*>(GetAttributeByName(wxT("SHOW_Y_AXIS"))))->GetValue();
		  }

		  //X Axis Title
		  if(GetAttributeByName(wxT("X_AXIS_LABEL"))!=NULL)
		  {
			  m_sXAxisTitle=(dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("X_AXIS_LABEL"))))->GetValueString();
		  }

		  //Y Axis Title
		  if(GetAttributeByName(wxT("Y_AXIS_LABEL"))!=NULL)
		  {
			  m_sYAxisTitle=(dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("Y_AXIS_LABEL"))))->GetValueString();
		  }

		  //Curve order
		  if(GetAttributeByName(wxT("CURVE_SORT"))!=NULL)
		  {
			  m_nCurveOrder=(dynamic_cast<SP_DS_ViewerAttributeList*>(GetAttributeByName(wxT("CURVE_SORT"))))
														              ->GetSelection();

		  }

		  //fixed adjustment
		  if(GetAttributeByName(wxT("USE_FIXED_X_ADJUSTMENT"))!=NULL)
		  {
			  m_bFixedXAdjustment=(dynamic_cast<SP_DS_ViewerAttributeCheckBox*>(GetAttributeByName(wxT("USE_FIXED_X_ADJUSTMENT"))))->GetValue();
		  }

		  //fixed adjustment
		  if(GetAttributeByName(wxT("USE_FIXED_Y_ADJUSTMENT"))!=NULL)
		  {
			  m_bFixedYAdjustment=(dynamic_cast<SP_DS_ViewerAttributeCheckBox*>(GetAttributeByName(wxT("USE_FIXED_Y_ADJUSTMENT"))))->GetValue();
		  }

		  //Min. X
		  if(GetAttributeByName(wxT("MIN_X_VALUE"))!=NULL)
		  {
			  wxString l_sMinX=(dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("MIN_X_VALUE"))))->GetValueString();

			  l_sMinX.ToDouble(&m_nMinXValue);
		  }

		  //Max. X
		  if(GetAttributeByName(wxT("MAX_X_VALUE"))!=NULL)
		  {
			  wxString l_sMaxX=(dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("MAX_X_VALUE"))))->GetValueString();

			  l_sMaxX.ToDouble(&m_nMaxXValue);
		  }

		  //Min. Y
		  if(GetAttributeByName(wxT("MIN_Y_VALUE"))!=NULL)
		  {
			  wxString l_sMinY=(dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("MIN_Y_VALUE"))))->GetValueString();

			  l_sMinY.ToDouble(&m_nMinYValue);
		  }

		  //Max. Y
		  if(GetAttributeByName(wxT("MAX_Y_VALUE"))!=NULL)
		  {
			  wxString l_sMaxY=(dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("MAX_Y_VALUE"))))->GetValueString();

			  l_sMaxY.ToDouble(&m_nMaxYValue);
		  }
}

void SP_DS_PlotViewer::LoadCurrentAttributeValues()
{
          SP_DS_ResultViewer::LoadCurrentAttributeValues();
          
          //Show legend
		if(GetAttributeByName(wxT("SHOW_LEGEND"))!=NULL)
		{
			(dynamic_cast<SP_DS_ViewerAttributeCheckBox*>(GetAttributeByName(wxT("SHOW_LEGEND"))))->SetValue(m_bShowLegend);
		}
		
		//Legend position(Horizontal)
		  if(GetAttributeByName(wxT("LEGEND_HORIZONTAL_POS"))!=NULL)
		  {
		       int l_nPosition=GetSelection(m_nLegendHorizontalPosition);
			  (dynamic_cast<SP_DS_ViewerAttributeList*>(GetAttributeByName(wxT("LEGEND_HORIZONTAL_POS"))))
			                                            ->SetSelection(l_nPosition);
		  }

		  //Legend position(Vertical)
		  if(GetAttributeByName(wxT("LEGEND_VERTICAL_POS"))!=NULL)
		  {
		     int l_nPosition=GetSelection(m_nLegendVerticalPosition);
		    (dynamic_cast<SP_DS_ViewerAttributeList*>(GetAttributeByName(wxT("LEGEND_VERTICAL_POS"))))
											->SetSelection(l_nPosition);
		  }

		
		//Show X Axis
		  if(GetAttributeByName(wxT("SHOW_X_AXIS"))!=NULL)
		  {
			  (dynamic_cast<SP_DS_ViewerAttributeCheckBox*>(GetAttributeByName(wxT("SHOW_X_AXIS"))))->SetValue(m_bShowXAxis);
		  }

		  //Show Y Axis
		  if(GetAttributeByName(wxT("SHOW_Y_AXIS"))!=NULL)
		  {
			  (dynamic_cast<SP_DS_ViewerAttributeCheckBox*>(GetAttributeByName(wxT("SHOW_Y_AXIS"))))->SetValue(m_bShowYAxis);
		  }

		  //X Axis Title
		  if(GetAttributeByName(wxT("X_AXIS_LABEL"))!=NULL)
		  {
			  (dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("X_AXIS_LABEL"))))->SetValueString(m_sXAxisTitle);
		  }

		  //Y Axis Title
		  if(GetAttributeByName(wxT("Y_AXIS_LABEL"))!=NULL)
		  {
			  (dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("Y_AXIS_LABEL"))))->SetValueString(m_sYAxisTitle);
		  }

		  //Curve order
		  if(GetAttributeByName(wxT("CURVE_SORT"))!=NULL)
		  {
			  (dynamic_cast<SP_DS_ViewerAttributeList*>(GetAttributeByName(wxT("CURVE_SORT"))))
																	  ->SetSelection(m_nCurveOrder);

		  }

		  //fixed symbols
		  if(GetAttributeByName(wxT("USE_FIXED_X_ADJUSTMENT"))!=NULL)
		  {
			  (dynamic_cast<SP_DS_ViewerAttributeCheckBox*>(GetAttributeByName(wxT("USE_FIXED_X_ADJUSTMENT"))))->SetValue(m_bFixedXAdjustment);
		  }

		  //fixed symbols
		  if(GetAttributeByName(wxT("USE_FIXED_Y_ADJUSTMENT"))!=NULL)
		  {
			  (dynamic_cast<SP_DS_ViewerAttributeCheckBox*>(GetAttributeByName(wxT("USE_FIXED_Y_ADJUSTMENT"))))->SetValue(m_bFixedYAdjustment);
		  }

		  //Min. X
		  if(GetAttributeByName(wxT("MIN_X_VALUE"))!=NULL)
		  {
			  wxString l_sValue=wxString::Format(wxT("%.3f"),m_nMinXValue);
			  (dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("MIN_X_VALUE"))))->SetValueString(l_sValue);
		  }

		  //Max. X
		  if(GetAttributeByName(wxT("MAX_X_VALUE"))!=NULL)
		  {
			  wxString l_sValue=wxString::Format(wxT("%.3f"),m_nMaxXValue);
			  (dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("MAX_X_VALUE"))))->SetValueString(l_sValue);
		  }

		  //Min. Y
		  if(GetAttributeByName(wxT("MIN_Y_VALUE"))!=NULL)
		  {
			  wxString l_sValue=wxString::Format(wxT("%.3f"),m_nMinYValue);
			  (dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("MIN_Y_VALUE"))))->SetValueString(l_sValue);
		  }

		  //Max. Y
		  if(GetAttributeByName(wxT("MAX_Y_VALUE"))!=NULL)
		  {
			  wxString l_sValue=wxString::Format(wxT("%.3f"),m_nMaxYValue);
			  (dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("MAX_Y_VALUE"))))->SetValueString(l_sValue);
		  }
}

int SP_DS_PlotViewer::GetPosition(const int& p_nIndex)
{
	     switch(p_nIndex)
	     {
	         case 0:
	        	   return wxLEFT;
	         case 1:
	        	   return wxCENTER;
	         case 2:
	        	   return wxRIGHT;
	         case 3:
	        	  return wxTOP;
	         case 4:
	        	  return wxCENTER;
	         case 5:
	        	  return wxBOTTOM;
	         default:
	        	  return wxCENTER;
	     }
}

int SP_DS_PlotViewer::GetSelection(const int& p_nValue)
{
          SP_MapLong2Long  l_mValues;
          
          l_mValues[wxLEFT]=0;
          l_mValues[wxCENTER]=1;
          l_mValues[wxRIGHT]=2;
          l_mValues[wxTOP]=0;
          l_mValues[wxBOTTOM]=2;

          return l_mValues[p_nValue];
}
void SP_DS_PlotViewer::SaveOutput(const wxString& p_sFileName,const int& p_sFileType)
{

}

void SP_DS_PlotViewer::SaveViewToServerFormat(spsa::ModelView* p_pcServerSideView)
{
	      SP_DS_ResultViewer::SaveViewToServerFormat(p_pcServerSideView);

		  //show legend
		  p_pcServerSideView->SetAttributeValue(wxT("SHOW_LEGEND"),m_bShowLegend);

		  //legend horizontal position
		  p_pcServerSideView->SetAttributeValue(wxT("LEGEND_HORIZONTAL_POSITION"),m_nLegendHorizontalPosition);

		  //legend horizontal position
		  p_pcServerSideView->SetAttributeValue(wxT("LEGEND_VERTICAl_POSITION"),m_nLegendVerticalPosition);

	      //x axis label
	      p_pcServerSideView->SetAttributeValue(wxT("X_AXIS_LABEL"),m_sXAxisTitle);
	   
		  //y axix label
		  p_pcServerSideView->SetAttributeValue(wxT("Y_AXIS_LABEL"),m_sYAxisTitle);

		  //fixed X
		  p_pcServerSideView->SetAttributeValue(wxT("FIXED_X_ADJUSTMENT"),m_bFixedXAdjustment);

		  //fixed Y
		  p_pcServerSideView->SetAttributeValue(wxT("FIXED_Y_ADJUSTMENT"),m_bFixedYAdjustment);

		  //Min X value
		  p_pcServerSideView->SetAttributeValue(wxT("MIN_X_VALUE"),m_nMinXValue);

		  //Max X value
		  p_pcServerSideView->SetAttributeValue(wxT("MAX_X_VALUE"),m_nMaxXValue);

		  //Min Y value
		  p_pcServerSideView->SetAttributeValue(wxT("MIN_Y_VALUE"),m_nMinYValue);

		  //Max Y value
		  p_pcServerSideView->SetAttributeValue(wxT("MAX_Y_VALUE"),m_nMaxYValue);

}
void SP_DS_PlotViewer::LoadViewFromServerFormat(spsa::ModelView* p_pcServerSideView)
{
	     SP_DS_ResultViewer::LoadViewFromServerFormat(p_pcServerSideView);

		 //show legend
		 m_bShowLegend=p_pcServerSideView->GetAttributeValueBool(wxT("SHOW_LEGEND"));

		 //legend horizontal position
		 m_nLegendHorizontalPosition=p_pcServerSideView->GetAttributeValueInt(wxT("LEGEND_HORIZONTAL_POSITION"));

		 //legend vertical position
		 m_nLegendVerticalPosition=p_pcServerSideView->GetAttributeValueInt(wxT("LEGEND_VERTICAl_POSITION"));

	     //x axis label
		 m_sXAxisTitle=p_pcServerSideView->GetAttributeValueString(wxT("X_AXIS_LABEL"));

		 //y axis label
		 m_sYAxisTitle=p_pcServerSideView->GetAttributeValueString(wxT("Y_AXIS_LABEL"));

		 //fixed x
		 m_bFixedXAdjustment=p_pcServerSideView->GetAttributeValueBool(wxT("FIXED_X_ADJUSTMENT"));

		 //fixed y
		 m_bFixedYAdjustment=p_pcServerSideView->GetAttributeValueBool(wxT("FIXED_Y_ADJUSTMENT"));

		 //Min X value
		 m_nMinXValue=p_pcServerSideView->GetAttributeValueDouble(wxT("MIN_X_VALUE"));

		 //Max X value
		 m_nMaxXValue=p_pcServerSideView->GetAttributeValueDouble(wxT("MAX_X_VALUE"));

		 //Min Y value
		 m_nMinYValue=p_pcServerSideView->GetAttributeValueDouble(wxT("MIN_Y_VALUE"));

		 //Max Y value
		 m_nMaxYValue=p_pcServerSideView->GetAttributeValueDouble(wxT("MAX_Y_VALUE"));
}

void SP_DS_PlotViewer::SaveViewToSnoopyFormat(SP_DS_Metadata* p_pcView)
{
	      SP_DS_ResultViewer::SaveViewToSnoopyFormat(p_pcView);
	     

	      //X Show Legend
		  SP_DS_Attribute* l_pcAttribute=p_pcView->GetAttribute(wxT("ShowLegend"));
		  CHECK_POINTER(l_pcAttribute,return);
		  (dynamic_cast<SP_DS_BoolAttribute*>(l_pcAttribute))->SetValue(m_bShowLegend);

		  l_pcAttribute=p_pcView->GetAttribute(wxT("LegendHorizontalPosition"));
		  CHECK_POINTER(l_pcAttribute,return);
		  (dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttribute))->SetValue(m_nLegendHorizontalPosition);

		  l_pcAttribute=p_pcView->GetAttribute(wxT("LegendVerticalPosition"));
		  CHECK_POINTER(l_pcAttribute,return);
		  (dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttribute))->SetValue(m_nLegendVerticalPosition);

	      //X axis title
		  l_pcAttribute=p_pcView->GetAttribute(wxT("XAxisTitle"));
		  CHECK_POINTER(l_pcAttribute,return)
		  l_pcAttribute->SetValueString(m_sXAxisTitle);

		  //y axis title
		  l_pcAttribute=p_pcView->GetAttribute(wxT("YAxisTitle"));
		  CHECK_POINTER(l_pcAttribute,return)
		  l_pcAttribute->SetValueString(m_sYAxisTitle);
		  
		  //sort curve by
		  l_pcAttribute=p_pcView->GetAttribute(wxT("SortCurvesBy"));
		  CHECK_POINTER(l_pcAttribute,return);
		  (dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttribute))->SetValue(m_nCurveOrder);

		 //fixed X
		 l_pcAttribute=p_pcView->GetAttribute(wxT("FixedXAdjustment"));
		 CHECK_POINTER(l_pcAttribute,return);
		 (dynamic_cast<SP_DS_BoolAttribute*>(l_pcAttribute))->SetValue(m_bFixedXAdjustment);

		 //fixed Y
		 l_pcAttribute=p_pcView->GetAttribute(wxT("FixedYAdjustment"));
		 CHECK_POINTER(l_pcAttribute,return);
		 (dynamic_cast<SP_DS_BoolAttribute*>(l_pcAttribute))->SetValue(m_bFixedYAdjustment);

		 //min x value
		 l_pcAttribute=p_pcView->GetAttribute(wxT("X_Axis_Min"));
		 CHECK_POINTER(l_pcAttribute,return);
		 (dynamic_cast<SP_DS_DoubleAttribute*>(l_pcAttribute))->SetValue(m_nMinXValue);

		 //Max x value
		 l_pcAttribute=p_pcView->GetAttribute(wxT("X_Axis_Max"));
		 CHECK_POINTER(l_pcAttribute,return);
		 (dynamic_cast<SP_DS_DoubleAttribute*>(l_pcAttribute))->SetValue(m_nMaxXValue);

		 //min y value
		 l_pcAttribute=p_pcView->GetAttribute(wxT("Y_Axis_Min"));
		 CHECK_POINTER(l_pcAttribute,return);
		 (dynamic_cast<SP_DS_DoubleAttribute*>(l_pcAttribute))->SetValue(m_nMinYValue);


		 //Max y value
		 l_pcAttribute=p_pcView->GetAttribute(wxT("Y_Axis_Max"));
		 CHECK_POINTER(l_pcAttribute,return);
		 (dynamic_cast<SP_DS_DoubleAttribute*>(l_pcAttribute))->SetValue(m_nMaxYValue);
}
void SP_DS_PlotViewer::LoadViewFromSnoopyFormat(SP_DS_Metadata* p_pcView)
{
	      SP_DS_ResultViewer::LoadViewFromSnoopyFormat(p_pcView);
	      
	      //X Show Legend
		  SP_DS_Attribute* l_pcAttribute=p_pcView->GetAttribute(wxT("ShowLegend"));
		  CHECK_POINTER(l_pcAttribute,return)
		  m_bShowLegend=(dynamic_cast<SP_DS_BoolAttribute*>(l_pcAttribute))->GetValue();

		  l_pcAttribute=p_pcView->GetAttribute(wxT("LegendHorizontalPosition"));
		  CHECK_POINTER(l_pcAttribute,return);
		  m_nLegendHorizontalPosition=(dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttribute))->GetValue();

		  l_pcAttribute=p_pcView->GetAttribute(wxT("LegendVerticalPosition"));
		  CHECK_POINTER(l_pcAttribute,return);
		  m_nLegendVerticalPosition=(dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttribute))->GetValue();


	      //X axis title
		  l_pcAttribute=p_pcView->GetAttribute(wxT("XAxisTitle"));
		  CHECK_POINTER(l_pcAttribute,return)
		  m_sXAxisTitle=l_pcAttribute->GetValueString();

		  //y axis title
		  l_pcAttribute=p_pcView->GetAttribute(wxT("YAxisTitle"));
		  CHECK_POINTER(l_pcAttribute,return)
		  m_sYAxisTitle=l_pcAttribute->GetValueString();
		  
		  //sort curve by
		  l_pcAttribute=p_pcView->GetAttribute(wxT("SortCurvesBy"));
		  CHECK_POINTER(l_pcAttribute,return);
		  m_nCurveOrder=(dynamic_cast<SP_DS_NumberAttribute*>(l_pcAttribute))->GetValue();

		 //fixed X
		 l_pcAttribute=p_pcView->GetAttribute(wxT("FixedXAdjustment"));
		 CHECK_POINTER(l_pcAttribute,return);
		 m_bFixedXAdjustment=(dynamic_cast<SP_DS_BoolAttribute*>(l_pcAttribute))->GetValue();

		 //fixed Y
		 l_pcAttribute=p_pcView->GetAttribute(wxT("FixedYAdjustment"));
		 CHECK_POINTER(l_pcAttribute,return);
		 m_bFixedYAdjustment=(dynamic_cast<SP_DS_BoolAttribute*>(l_pcAttribute))->GetValue();

		 //min. x value
		 l_pcAttribute=p_pcView->GetAttribute(wxT("X_Axis_Min"));
		 CHECK_POINTER(l_pcAttribute,return);
		 m_nMinXValue=(dynamic_cast<SP_DS_DoubleAttribute*>(l_pcAttribute))->GetValue();

		 //max. x value
		 l_pcAttribute=p_pcView->GetAttribute(wxT("X_Axis_Max"));
		 CHECK_POINTER(l_pcAttribute,return);
		 m_nMaxXValue=(dynamic_cast<SP_DS_DoubleAttribute*>(l_pcAttribute))->GetValue();

		 //min. x value
		 l_pcAttribute=p_pcView->GetAttribute(wxT("Y_Axis_Min"));
		 CHECK_POINTER(l_pcAttribute,return);
		 m_nMinYValue=(dynamic_cast<SP_DS_DoubleAttribute*>(l_pcAttribute))->GetValue();

		 //max. x value
		 l_pcAttribute=p_pcView->GetAttribute(wxT("Y_Axis_Max"));
		 CHECK_POINTER(l_pcAttribute,return);
		 m_nMaxYValue=(dynamic_cast<SP_DS_DoubleAttribute*>(l_pcAttribute))->GetValue();
}

void SP_DS_PlotViewer::ExportToImageFile(wxString& p_sFileName,const int& p_nFileType)
{
	wxBitmapType l_nExportType;

	CHECK_POINTER(m_pcChartPanel, return);

	if (p_sFileName.BeforeLast(wxT('.')) != wxT(""))
	{
		p_sFileName = p_sFileName.BeforeLast(wxT('.'));
	}

	wxBitmap bitmap = m_pcChartPanel->CopyBackbuffer();

	switch (p_nFileType)
	{
	case 0:
		l_nExportType = wxBITMAP_TYPE_BMP;
		p_sFileName = p_sFileName + wxT(".bmp");
		break;
	case 1:
		l_nExportType = wxBITMAP_TYPE_GIF;
		p_sFileName = p_sFileName + wxT(".gif");
		break;
	case 2:
		l_nExportType = wxBITMAP_TYPE_PNG;
		p_sFileName = p_sFileName + wxT(".png");
		break;
	case 3:
		l_nExportType = wxBITMAP_TYPE_JPEG;
		p_sFileName = p_sFileName + wxT(".jpg");
		break;
	case 4:
		SP_LOGERROR(wxT("This function has not been implemented"));
		return;
	default:
		l_nExportType = wxBITMAP_TYPE_BMP;
		p_sFileName = p_sFileName + wxT(".bmp");
		break;
	}

	bitmap.ConvertToImage().SaveFile(p_sFileName, l_nExportType);
}

