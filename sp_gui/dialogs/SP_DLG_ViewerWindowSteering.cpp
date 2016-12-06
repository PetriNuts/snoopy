/*
 * SP_DLG_ViewerWindowSteering.cpp
 *
 *  Created on: Nov 27, 2016
 *      Author: Herajy
 */
#include "sp_gui/dialogs/SP_DLG_Simulation.h"
#include <sp_gui/dialogs/SP_DLG_ViewerWindowSteering.h>
#include "sp_gui/dialogs/SP_DLG_SelectXAxisVariable.h"
#include "sp_gui/dialogs/SP_DLG_ResultViewerProperties.h"

#include "sp_gui/dialogs/dia_SteeringGUI/SP_DLG_SelectNodes.h"
#include "sp_gui/dialogs/dia_SteeringGUI/SP_DLG_ChangeCurveAttributes.h"




IMPLEMENT_CLASS(SP_DLG_ViewerWindowSteering, SP_DLG_ViewerWindow)

BEGIN_EVENT_TABLE( SP_DLG_ViewerWindowSteering, SP_DLG_ViewerWindow )
END_EVENT_TABLE()

SP_DLG_ViewerWindowSteering::SP_DLG_ViewerWindowSteering(SP_DLG_Simulation* p_pcParentWnd,
		                                                 spsa::ModelView* p_pcModelView,
														 spsa::ResultMatrixInfo* p_pcResultMatrixInfo,
														 spsa::Vector2DDouble* p_an2DResultMatrix,
														 SP_VectorDouble*  p_pcXAxisValues,
														 spsa::Model * p_pcCurrentModel):
SP_DLG_ViewerWindow(p_pcParentWnd),
m_pcModelView(p_pcModelView),
m_pcResultMatrixInfo(p_pcResultMatrixInfo),
m_an2DResultMatrix(p_an2DResultMatrix),
m_pcXAxisValues(p_pcXAxisValues),
m_pcCurrentModel(p_pcCurrentModel)

{
	   if (m_pcModelView == NULL)
		  return;

		//use this to check for coloured simulation
		//m_bIsColouredSimulation = m_pcModelView->GetAttribute(wxT("OutputType")) != nullptr;

		wxString l_sViewName = m_pcModelView->GetName();

		SetTitle(l_sViewName);

		spsa::Attribute* l_pcAttribute = m_pcModelView->GetAttribute(wxT("VIEWER_TYPE"));
		CHECK_POINTER(l_pcAttribute, return);
		unsigned int l_nViewerIndex = (dynamic_cast<spsa::AttributeUint*>(l_pcAttribute))->GetValue();
		wxString l_sViewerType=GetViewerType(l_nViewerIndex);

		CreateResultViewer(l_sViewerType);

		LoadView(m_pcResultViewer, m_pcModelView);

		//m_pcOutputViewerType->SetStringSelection(m_pcModelView->GetAttribute(wxT("ViewerType"))->GetValueString());

	    wxSize temp = m_pcMainSizer->GetSize();

	    long l_WindowWidth = 600;
		//temp1.ToLong(&l_WindowWidth);

		long l_WindowHeight = 0;
		//temp2.ToLong(&l_WindowHeight);
		if (l_WindowHeight <= 633)
		{
			l_WindowHeight += (633 - l_WindowHeight);
		}
		else
		{
			l_WindowHeight += 33;
		}
		temp.SetHeight(l_WindowHeight + 177);
		temp.SetWidth(l_WindowWidth + 284);

		this->SetSize(temp);

		wxPoint pos = p_pcParentWnd->GetPosition();
		wxSize size = p_pcParentWnd->GetSize();
		pos.x += size.GetX() + 10;
		SetPosition(pos);
}

SP_DLG_ViewerWindowSteering::~SP_DLG_ViewerWindowSteering()
{
}

bool SP_DLG_ViewerWindowSteering::LoadView(SP_DS_ResultViewer* p_pcResultViewer, spsa::ModelView* p_pcModelView)
{
	spsa::ModelView* l_pcCurrentView = p_pcModelView;
	spsa::Attribute* l_pcAttribute = NULL;

	spsa::AttributeVectorAttribute* l_pcCurveInfo;

	unsigned long l_nIndex = 0;
	int l_nLineWidth = -1;
	int l_nLineStyle = -1;
	wxString l_nLineColorName = wxT("");
	wxString l_sCurveName = wxT("");
	bool l_bShow = false;

	spsa::RM_ROW_TYPE l_nNodeType;

	//CHECK_POINTER(l_pcCurrentView, return);

	//StopRefreshTimers();

	l_pcAttribute = l_pcCurrentView->GetCurveInfo();

	//CHECK_POINTER(l_pcAttribute, return);

	//delete the current items if any
	m_pcPlaceChoiceCheckListBox->Clear();

	//clear old curves
	p_pcResultViewer->ClearCurves();

	//clear shown curve
	p_pcResultViewer->ClearShownCurves();

	l_pcCurveInfo = dynamic_cast<spsa::AttributeVectorAttribute*>(l_pcAttribute);

	unsigned int l_nCurveCount = 0;

	//get shown matrix type
	//l_pcAttribute = l_pcCurrentView->GetAttribute(wxT("VIEW_MATRIX_TYPE"));
	//CHECK_POINTER(l_pcAttribute, return);

	//m_nViewMatrixType = dynamic_cast<spsa::AttributeUint*>(l_pcAttribute)->GetValue();

	//load curves from the current view
	for (unsigned long l_nElement = 0; l_nElement < l_pcCurveInfo->size(); l_nElement++)
	{

		//CHECK_POINTER(l_pcCurveInfo->GetElement(l_nElement), return);

		spsa::AttributeStruct* l_pcAttribute = dynamic_cast<spsa::AttributeStruct*>(l_pcCurveInfo->GetElement(l_nElement));

		if (l_pcAttribute->GetField(wxT("INDEX")) != NULL)
		{
			l_nIndex = dynamic_cast<spsa::AttributeUlong*>(l_pcAttribute->GetField(wxT("INDEX")))->GetValue();
		}

		if (l_pcAttribute->GetField(wxT("CURVE_NAME")) != NULL)
		{
			l_sCurveName = l_pcAttribute->GetField(wxT("CURVE_NAME"))->GetValueString();
		}

		if (l_pcAttribute->GetField(wxT("LINE_WIDTH")) != NULL)
		{
			l_nLineWidth = dynamic_cast<spsa::AttributeInt*>(l_pcAttribute->GetField(wxT("LINE_WIDTH")))->GetValue();
		}

		if (l_pcAttribute->GetField(wxT("LINE_STYLE")) != NULL)
		{
			l_nLineStyle = dynamic_cast<spsa::AttributeInt*>(l_pcAttribute->GetField(wxT("LINE_STYLE")))->GetValue();
		}

		if (l_pcAttribute->GetField(wxT("CURVE_COLOR_NAME")) != NULL)
		{
			l_nLineColorName = l_pcAttribute->GetField(wxT("CURVE_COLOR_NAME"))->GetValueString();
		}

		if (l_pcAttribute->GetField(wxT("SHOW")) != NULL)
		{
			l_bShow = dynamic_cast<spsa::AttributeBool*>(l_pcAttribute->GetField(wxT("SHOW")))->GetValue();
		}

		if (l_pcAttribute->GetField(wxT("CURVE_TYPE")) != NULL)
		{
			l_nNodeType = (spsa::RM_ROW_TYPE) (dynamic_cast<spsa::AttributeUint*>(l_pcAttribute->GetField(wxT("CURVE_TYPE")))->GetValue());
		}

		m_pcPlaceChoiceCheckListBox->Append(l_sCurveName);

		m_pcPlaceChoiceCheckListBox->Check(l_nCurveCount, l_bShow);

		//fill in the result matrix info
		m_pcResultMatrixInfo->SetRowInfo(l_nElement, l_nIndex, l_nNodeType);

		//Add curve to the viewer for this item. Initially this curve will not be shown
		p_pcResultViewer->AddCurve(l_sCurveName, l_nElement, m_an2DResultMatrix);

		p_pcResultViewer->ShowCurve(l_nCurveCount, l_bShow);

		l_nCurveCount++;
	}

	//m_pcParentWnd->CalculateXAxisValues(p_pcModelView, m_anXValues);

	//p_pcResultViewer->SetXAxisValues(m_pcXAxisValues);

	p_pcResultViewer->Update();

	//send the server a request to update the result matrix info.
	//m_pcGUIClient->UpdateResultMatrixInfo(m_pcResultMatrixInfo);

	//RefreshResultMatrix();

	//Update the viewer
	//UpdateView();

	//StartRefreshTimers();

	return true;
}

void SP_DLG_ViewerWindowSteering::OnChangeXAxis(wxCommandEvent& WXUNUSED(event))
{
	/*SP_DLG_SelectXAxisVariable* l_pcSelectXAxis=new SP_DLG_SelectXAxisVariable(this,m_pcModelView->GetXAxisVariableType()
	 ,m_pcCurrentModel->GetPlaceNames(),
	 m_pcCurrentModel->GetTransitionNames(),
	 m_pcCurrentModel->GetParameterNames(),
	 m_pcModelView->GetXAxisVariableName());

	 if(l_pcSelectXAxis->ShowModal()==wxID_OK)
	 {
		 m_pcModelView->SetXAxisVariableType(l_pcSelectXAxis->GetSelectedVariableType());

		 m_pcModelView->SetXAxisVariableName(l_pcSelectXAxis->GetSelectedVariableName());

	  //RefreshResultMatrix();

	  //UpdateView();
	 }
	 l_pcSelectXAxis->Destroy();*/

}

void SP_DLG_ViewerWindowSteering::OnWindowActivate(wxActivateEvent& event)
{
	if (event.GetActive())
	{
		if (m_pcParentWnd != NULL)
		{
			//TODO:
			//m_pcParentWnd->ChangeViewTo(m_pcModelView);
		}
#ifndef __WXMSW__
		Raise();
	}
	else
	{
		Lower();
#endif
	}
	event.Skip();
}

void SP_DLG_ViewerWindowSteering::OnEditNodeList(wxCommandEvent& event)
{
	     SP_DLG_SelectNodes* l_pcSelectCurvsDlg;

		l_pcSelectCurvsDlg = new SP_DLG_SelectNodes(this, m_pcCurrentModel, m_pcModelView);

		if (l_pcSelectCurvsDlg->ShowModal() == wxID_OK)
		{
			LoadView(m_pcResultViewer,m_pcModelView);
		}

		l_pcSelectCurvsDlg->Destroy();

		//SetModified();
}

void SP_DLG_ViewerWindowSteering::OnItemDoubleClick(wxCommandEvent& event)
{

	if (event.GetSelection() < 0 && (unsigned int) event.GetSelection() >= m_pcPlaceChoiceCheckListBox->GetCount())
	{
		return;
	}

	CHECK_POINTER(m_pcResultViewer, return);

	int l_nCurveIndex = m_pcPlaceChoiceCheckListBox->GetSelection();

	SP_DLG_ChangeCurveAttributes l_dial(this, m_pcResultViewer->GetCurveColor(l_nCurveIndex),
			m_pcResultViewer->GetCurveLineWidth(l_nCurveIndex),
			m_pcResultViewer->GetCurveLineStyle(l_nCurveIndex));

	if (l_dial.ShowModal() == wxID_OK)
	{
		m_pcResultViewer->SetCurveLineWidth(l_nCurveIndex, l_dial.GetLineWidth());

		m_pcResultViewer->SetCurveLineStyle(l_nCurveIndex, l_dial.GetLineStyle());

		wxColour l_nColor = l_dial.GetLineColor();

		m_pcResultViewer->SetCurveColor(l_nCurveIndex, l_nColor.GetAsString(wxC2S_HTML_SYNTAX));

		//Update the current viewer
		//UpdateView();

		//we need to ask the user about saving the model
		//SetModified();
	}
}

void SP_DLG_ViewerWindowSteering::OnChangeResultViewer(wxCommandEvent& WXUNUSED(event))
{
	    //Change the current view viewer
		//SP_DS_Attribute* l_pcAttribute = m_pcModelView->GetAttribute(wxT("ViewerType"));
		//CHECK_POINTER(l_pcAttribute, return);
		if (m_pcOutputViewerType->GetStringSelection() == wxT("Tabular"))
		{
			m_pcOutputExportType->Delete(1);
			m_pcXAxis->Enable(false);
			m_pcOutputExportType->SetSelection(0);
			//l_pcAttribute->SetValueString(wxT("Tabular"));
		}
		else
		if (m_pcOutputViewerType->GetStringSelection() == wxT("xyPlot"))
		{
			m_pcXAxis->Enable(true);
			if (m_pcOutputExportType->GetCount() != 2)
			{
				m_pcOutputExportType->Append(wxT("Image Export"));
			}
			//l_pcAttribute->SetValueString(wxT("xyPlot"));
		}
		else
		if (m_pcOutputViewerType->GetStringSelection() == wxT("Histogram"))
		{
			if (m_pcOutputExportType->GetCount() != 2)
			{
				m_pcOutputExportType->Append(wxT("Image Export"));
			}
			m_pcXAxis->Enable(false);
			//l_pcAttribute->SetValueString(wxT("Histogram"));
		}

		wxString l_sViewerType=m_pcOutputViewerType->GetStringSelection();

		//delete old viewer
		m_pcResultViewer->Destroy();

		CreateResultViewer(l_sViewerType);

		LoadView(m_pcResultViewer, m_pcModelView);

		//m_pcParentWnd->LoadData(false);
		//RefreshWindow();
}

void SP_DLG_ViewerWindowSteering::OnEditViewerProperties(wxCommandEvent& p_cEvent)
{
	    SP_DLG_ResultViewerProperties* l_pcViewerProperties =
				new SP_DLG_ResultViewerProperties(m_pcResultViewer, this);

		if (l_pcViewerProperties->ShowModal() == wxID_OK)
		{
			//UpdateView();

			//we need to ask the user about saving the model
			//SetModified();
		}
		l_pcViewerProperties->Destroy();
}


wxString SP_DLG_ViewerWindowSteering::GetViewerType(const int& p_nViewerIndex)
{
      if(p_nViewerIndex==0)
      {
    	  return wxT("Tabular");
      }
      else if (p_nViewerIndex==1)
      {
    	  return wxT("xyPlot");
      }
      else if (p_nViewerIndex==2)
      {
    	  return wxT("Histogram");
      }
      else
      {
    	  return wxT("xyPlot");
      }
}







