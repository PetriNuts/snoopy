/*
 * SP_DLG_ViewerWindowSteering.cpp
 *
 *  Created on: Nov 27, 2016
 *      Author: Herajy
 */
#include "sp_gui/dialogs/SP_DLG_Simulation.h"
#include <sp_gui/dialogs/SP_DLG_ViewerWindowSteering.h>

IMPLEMENT_CLASS(SP_DLG_ViewerWindowSteering, wxFrame)

BEGIN_EVENT_TABLE( SP_DLG_ViewerWindowSteering, wxFrame )
END_EVENT_TABLE()

SP_DLG_ViewerWindowSteering::SP_DLG_ViewerWindowSteering(SP_DLG_Simulation* p_pcParentWnd,
		                                                 spsa::ModelView* p_pcModelView,
														 spsa::ResultMatrixInfo* p_pcResultMatrixInfo,
														 spsa::Vector2DDouble* p_an2DResultMatrix):
SP_DLG_ViewerWindow(p_pcParentWnd),
m_pcModelView(p_pcModelView),
m_pcResultMatrixInfo(p_pcResultMatrixInfo),
m_an2DResultMatrix(p_an2DResultMatrix)
{
	   if (m_pcModelView == NULL)
		  return;

		//use this to check for coloured simulation
		//m_bIsColouredSimulation = m_pcModelView->GetAttribute(wxT("OutputType")) != nullptr;

		wxString l_sViewName = m_pcModelView->GetName();

		SetTitle(l_sViewName);

		//wxString l_sViewerType=m_pcModelView-

		//TODO:
		CreateResultViewer(wxT("xyPlot"));

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

		Bind(wxEVT_ACTIVATE, &SP_DLG_ViewerWindowSteering::OnWindowActivate, this);
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

		//p_pcResultViewer->SetXAxisValues(&m_anXValues);

		//p_pcResultViewer->LoadViewFromSnoopyFormat(p_pcModelView);
		p_pcResultViewer->Update();

	//send the server a request to update the result matrix info.
	//m_pcGUIClient->UpdateResultMatrixInfo(m_pcResultMatrixInfo);

	//RefreshResultMatrix();

	//Update the viewer
	//UpdateView();

	//StartRefreshTimers();

	return true;
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



