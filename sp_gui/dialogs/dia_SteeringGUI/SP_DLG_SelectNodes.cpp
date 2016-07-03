/*
 * SP_DLG_SelectNodes.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 10.10.2011
 * Short Description:
 */
//=================================================
#include "sp_gui/dialogs/dia_SteeringGUI/SP_DLG_SelectNodes.h"
#include "spsa/utilities.h"

enum
{
	SP_GUI_SELECTNODES_OPTIONBOX_NODETYPE,
	SP_GUI_SELECTNODES_BUTTON_MOVE_LEFT_TO_RIGHT,
	SP_GUI_SELECTNODES_BUTTON_MOVE_RIGHT_TO_LEFT,
	SP_GUI_SELECTNODES_BUTTON_MOVE_ALL_RIGHT_TO_LEFT,
	SP_GUI_SELECTNODES_BUTTON_MOVE_ALL_LEFT_TO_RIGHT,

	SP_GUI_SELECTNODES_LISTBOX_AVAILABLE_NODES,
	SP_GUI_SELECTNODES_LISTBOX_SELECYED_NODES,

	SP_GUI_STEERINGNODES_COLORUNCOLOR_CHECK,

	SP_GUI_STEERINGNODES_CURRENT_COLOR_NAME
};
BEGIN_EVENT_TABLE(SP_DLG_SelectNodes,wxDialog)

EVT_RADIOBOX(SP_GUI_SELECTNODES_OPTIONBOX_NODETYPE, SP_DLG_SelectNodes::OnNodeTypeChanged)

EVT_BUTTON(SP_GUI_SELECTNODES_BUTTON_MOVE_LEFT_TO_RIGHT,SP_DLG_SelectNodes::OnMoveLeftToRight)
EVT_BUTTON(SP_GUI_SELECTNODES_BUTTON_MOVE_RIGHT_TO_LEFT,SP_DLG_SelectNodes::OnMoveRightToLeft)

EVT_BUTTON(SP_GUI_SELECTNODES_BUTTON_MOVE_ALL_LEFT_TO_RIGHT,SP_DLG_SelectNodes::OnMoveAllLeftToRight)
EVT_BUTTON(SP_GUI_SELECTNODES_BUTTON_MOVE_ALL_RIGHT_TO_LEFT,SP_DLG_SelectNodes::OnMoveAllRightToLeft)

EVT_BUTTON(wxID_OK,SP_DLG_SelectNodes::OnOk)

EVT_LISTBOX_DCLICK(SP_GUI_SELECTNODES_LISTBOX_AVAILABLE_NODES, SP_DLG_SelectNodes::OnMoveLeftToRight)

EVT_LISTBOX_DCLICK(SP_GUI_SELECTNODES_LISTBOX_SELECYED_NODES, SP_DLG_SelectNodes::OnMoveRightToLeft)

EVT_CHECKBOX(SP_GUI_STEERINGNODES_COLORUNCOLOR_CHECK, SP_DLG_SelectNodes::OnSelectColorNodesCheckUncheck)

EVT_CHOICE(SP_GUI_STEERINGNODES_CURRENT_COLOR_NAME, SP_DLG_SelectNodes::OnColorNameChanged)

END_EVENT_TABLE()

SP_DLG_SelectNodes::SP_DLG_SelectNodes(wxWindow* p_pcParentWnd, spsa::Model * p_pcCurrentModel, const unsigned int& p_nCurrentView) :
		wxDialog(p_pcParentWnd, -1, wxT("Select curves"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER),
		m_bIsColoredModel(false),
		m_nCurrentView(p_nCurrentView),
		m_pcCurrentModel(p_pcCurrentModel)
{
	CHECK_POINTER(m_pcCurrentModel, return);

	//get the current model type
	if (m_pcCurrentModel->GetModelType() == spsa::COLORED_MODEL)
	{
		m_bIsColoredModel = true;
	}

	wxSizer* l_pcMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer* l_pcSelectorSizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* l_pcColorUncolorSizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* l_pcNodeTypeSizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* l_pcButtonSizer = new wxBoxSizer(wxHORIZONTAL);

	//add sub sizer
	l_pcMainSizer->Add(l_pcSelectorSizer, 1, wxEXPAND | wxALL);
	l_pcMainSizer->Add(l_pcColorUncolorSizer, 0, wxEXPAND | wxALL);
	l_pcMainSizer->Add(l_pcNodeTypeSizer, 0, wxEXPAND | wxALL);
	l_pcMainSizer->Add(l_pcButtonSizer, 0, wxEXPAND | wxALIGN_CENTER);

	//available nodes
	wxSizer* l_pcRowSizer = new wxBoxSizer(wxVERTICAL);
	m_pcAvailableNodes = new wxListBox(this, SP_GUI_SELECTNODES_LISTBOX_AVAILABLE_NODES, wxDefaultPosition, wxSize(-1, 300), 0, NULL, wxLB_EXTENDED);
	l_pcRowSizer->Add(m_pcAvailableNodes, 1, wxEXPAND | wxALL);
	l_pcSelectorSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL);

	//select node type
	wxSizer* l_pcMiddleSizer = new wxBoxSizer(wxVERTICAL);
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcMiddleSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL);

	//button sizer
	l_pcRowSizer = new wxBoxSizer(wxVERTICAL);
	l_pcRowSizer->Add(new wxButton(this, SP_GUI_SELECTNODES_BUTTON_MOVE_LEFT_TO_RIGHT, wxT(">")), 0, wxEXPAND | wxALIGN_CENTER);
	l_pcRowSizer->AddSpacer(8);
	l_pcRowSizer->Add(new wxButton(this, SP_GUI_SELECTNODES_BUTTON_MOVE_RIGHT_TO_LEFT, wxT("<")), 0, wxEXPAND | wxALIGN_CENTER);
	l_pcRowSizer->AddSpacer(8);
	l_pcRowSizer->Add(new wxButton(this, SP_GUI_SELECTNODES_BUTTON_MOVE_ALL_LEFT_TO_RIGHT, wxT(">>")), 0, wxEXPAND | wxALIGN_CENTER);
	l_pcRowSizer->AddSpacer(8);
	l_pcRowSizer->Add(new wxButton(this, SP_GUI_SELECTNODES_BUTTON_MOVE_ALL_RIGHT_TO_LEFT, wxT("<<")), 0, wxEXPAND | wxALIGN_CENTER);

	l_pcMiddleSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL);
	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcMiddleSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL);
	l_pcSelectorSizer->Add(l_pcMiddleSizer, 0, wxEXPAND | wxALIGN_CENTER);

	if (m_bIsColoredModel)
	{
		//coloured/ uncoloured nodes
		l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
		//check box
		m_pcSelectColorNodes = new wxCheckBox(this, SP_GUI_STEERINGNODES_COLORUNCOLOR_CHECK, wxT("Uncolored Nodes"));
		m_pcSelectColorNodes->SetValue(false);
		l_pcRowSizer->Add(m_pcSelectColorNodes, 1, wxEXPAND | wxALIGN_CENTER);

		m_pcColorName = new wxChoice(this, SP_GUI_STEERINGNODES_CURRENT_COLOR_NAME, wxDefaultPosition, wxSize(150, -1));
		//list box
		l_pcRowSizer->Add(m_pcColorName, 1, wxEXPAND | wxALIGN_CENTER);
		l_pcColorUncolorSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALIGN_CENTER);
	}

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	wxString l_sNodes[] =
	{ wxT("Place"), wxT("Transitions") };
	m_pcNodeTypes = new wxRadioBox(this, SP_GUI_SELECTNODES_OPTIONBOX_NODETYPE, wxT(""), wxDefaultPosition, wxDefaultSize, 2, l_sNodes);
	l_pcRowSizer->Add(m_pcNodeTypes, 0, wxEXPAND | wxALIGN_CENTER);
	l_pcNodeTypeSizer->Add(l_pcRowSizer, 0, wxEXPAND | wxALIGN_CENTER);

	//selected nodes
	l_pcRowSizer = new wxBoxSizer(wxVERTICAL);
	m_pcSelectedNodes = new wxListBox(this, SP_GUI_SELECTNODES_LISTBOX_SELECYED_NODES, wxDefaultPosition, wxSize(-1, 300), 0, NULL, wxLB_EXTENDED);
	l_pcRowSizer->Add(m_pcSelectedNodes, 1, wxEXPAND | wxALL);

	l_pcSelectorSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL);

	l_pcButtonSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALIGN_CENTER);

	//alignment the sizer
	SetSizerAndFit(l_pcMainSizer);
	Center();

	LoadData();

	//read old user selection
	ReadOldUserSelection();

}

SP_DLG_SelectNodes::~SP_DLG_SelectNodes()
{

}

void SP_DLG_SelectNodes::LoadData()
{
	spsa::Attribute* l_pcAttribute = NULL;

	spsa::ModelView* l_pcCurrentView = (*m_pcCurrentModel->GetViews())[m_nCurrentView];

	unsigned int l_nNodeType;
	SP_VectorString l_asNodeNames;

	spsa::RM_ROW_TYPE l_nCurveType;

	l_pcAttribute = l_pcCurrentView->GetAttribute(wxT("VIEW_MATRIX_TYPE"));
	CHECK_POINTER(l_pcAttribute, return);

	//get view node type
	l_nNodeType = dynamic_cast<spsa::AttributeUint*>(l_pcAttribute)->GetValue();

	m_pcNodeTypes->SetSelection(l_nNodeType);

	//load the colored node names, if the model is a colored one
	if (l_nNodeType == 1)
	{
		if (m_bIsColoredModel)
		{
			l_asNodeNames = dynamic_cast<spsa::ColoredModel*>(m_pcCurrentModel)->GetColoredTransitionNames();

			l_nCurveType = spsa::RM_ROW_TYPE_COLORED_TRANSITION;
		}
		else
		{
			l_asNodeNames = m_pcCurrentModel->GetTransitionNames();

			l_nCurveType = spsa::RM_ROW_TYPE_UNCOLORED_TRANSITION;
		}
	}
	else
	{
		if (m_bIsColoredModel)
		{
			l_asNodeNames = dynamic_cast<spsa::ColoredModel*>(m_pcCurrentModel)->GetColoredPlaceNames();

			l_nCurveType = spsa::RM_ROW_TYPE_COLORED_PLACE;
		}
		else
		{
			l_asNodeNames = m_pcCurrentModel->GetPlaceNames();

			l_nCurveType = spsa::RM_ROW_TYPE_UNCOLORED_PLACE;
		}

	}

	//add this node to the available node list
	AddNodesToAvailable(l_asNodeNames, l_nCurveType);
}

void SP_DLG_SelectNodes::AddNodesToAvailable(const SP_VectorString& p_anNames, const spsa::RM_ROW_TYPE& p_nNodeType)
{
	//add node names to the available nodes
	SP_VectorString::const_iterator l_itNodeName;

	//clear old nodes
	m_pcAvailableNodes->Clear();

	unsigned long l_nPosition = 0;
	for (l_itNodeName = p_anNames.begin(); l_itNodeName != p_anNames.end(); l_itNodeName++)
	{
		m_pcAvailableNodes->Append((*l_itNodeName), new SP_NodeClientData(l_nPosition, p_nNodeType));

		l_nPosition++;

	}
}

void SP_DLG_SelectNodes::OnNodeTypeChanged(wxCommandEvent& event)
{
	spsa::Attribute* l_pcAttribute = NULL;

	spsa::ModelView* l_pcCurrentView = (*m_pcCurrentModel->GetViews())[m_nCurrentView];

	SP_VectorString l_asNodeNames;

	l_pcAttribute = l_pcCurrentView->GetAttribute(wxT("VIEW_MATRIX_TYPE"));
	CHECK_POINTER(l_pcAttribute, return);

	//get view node type
	dynamic_cast<spsa::AttributeUint*>(l_pcAttribute)->SetValue(event.GetSelection());

	m_pcAvailableNodes->Clear();

	m_pcSelectedNodes->Clear();

	//change the color names
	ChangeColorName();

	//reload the data
	LoadData();
}

void SP_DLG_SelectNodes::MoveElements(wxListBox* p_pcSourceList, wxListBox* p_pcDestList, wxArrayInt& p_anSelections)
{
	SP_VectorString l_asItemsToDelete;

	l_asItemsToDelete.resize(p_anSelections.Count());

	for (unsigned long l_nElement = 0; l_nElement < p_anSelections.Count(); l_nElement++)
	{
		//get the item index
		unsigned int l_nItemID = p_anSelections[l_nElement];

		CopyListItem(p_pcSourceList, p_pcDestList, l_nItemID);

		l_asItemsToDelete[l_nElement] = p_pcSourceList->GetString(l_nItemID);
	}

	RemoveItemsFromList(p_pcSourceList, l_asItemsToDelete);
}

void SP_DLG_SelectNodes::RemoveItemsFromList(wxListBox* p_pcList, const SP_VectorString& p_anSelections)
{

	//delete items
	for (unsigned long l_nElement = 0; l_nElement < p_anSelections.size(); l_nElement++)
	{
		//find the item of the index name
		int l_nIndexToDelete = p_pcList->FindString(p_anSelections[l_nElement], true);

		//delete the item
		if (l_nIndexToDelete != wxNOT_FOUND)
			p_pcList->Delete(l_nIndexToDelete);
	}
}

void SP_DLG_SelectNodes::CopyListItem(wxListBox* p_pcSourceList, wxListBox* p_pcDestList, const unsigned int& p_nItemID)
{
	SP_NodeClientData* l_pcClientData = dynamic_cast<SP_NodeClientData*>(p_pcSourceList->GetClientObject(p_nItemID));

	CHECK_POINTER(l_pcClientData, return);

	//get the item name
	wxString l_sItemName = p_pcSourceList->GetString(p_nItemID);

	l_pcClientData = l_pcClientData->Clone();

	//add the new item
	p_pcDestList->Append(l_sItemName, l_pcClientData);

	p_pcSourceList->SetSelection(p_nItemID, false);
}
void SP_DLG_SelectNodes::MoveAllElement(wxListBox* p_pcSourceList, wxListBox* p_pcDestList)
{
	for (unsigned long l_nElement = 0; l_nElement < p_pcSourceList->GetCount(); l_nElement++)
	{
		CopyListItem(p_pcSourceList, p_pcDestList, l_nElement);
	}

	p_pcSourceList->Clear();
}

void SP_DLG_SelectNodes::OnMoveLeftToRight(wxCommandEvent& event)
{
	wxArrayInt l_anSelections;

	m_pcAvailableNodes->GetSelections(l_anSelections);

	MoveElements(m_pcAvailableNodes, m_pcSelectedNodes, l_anSelections);
}

void SP_DLG_SelectNodes::OnMoveRightToLeft(wxCommandEvent& event)
{
	wxArrayInt l_anSelections;

	m_pcSelectedNodes->GetSelections(l_anSelections);

	MoveElements(m_pcSelectedNodes, m_pcAvailableNodes, l_anSelections);
}

void SP_DLG_SelectNodes::OnMoveAllLeftToRight(wxCommandEvent& event)
{
	MoveAllElement(m_pcAvailableNodes, m_pcSelectedNodes);
}

void SP_DLG_SelectNodes::OnMoveAllRightToLeft(wxCommandEvent& event)
{
	MoveAllElement(m_pcSelectedNodes, m_pcAvailableNodes);
}

void SP_DLG_SelectNodes::OnOk(wxCommandEvent& event)
{
	spsa::ModelView* l_pcCurrentView = (*m_pcCurrentModel->GetViews())[m_nCurrentView];

	CHECK_POINTER(l_pcCurrentView, return);

	l_pcCurrentView->RemoveAllCurves();

	for (unsigned long l_nElement = 0; l_nElement < m_pcSelectedNodes->GetCount(); l_nElement++)
	{
		wxString l_sCurveName = m_pcSelectedNodes->GetString(l_nElement);

		SP_NodeClientData* l_pcClientData = dynamic_cast<SP_NodeClientData*>(m_pcSelectedNodes->GetClientObject(l_nElement));

		CHECK_POINTER(l_pcClientData, return);

		unsigned long l_nPosition = l_pcClientData->GetMatrixPosition();

		spsa::RM_ROW_TYPE l_nNodeType = l_pcClientData->GetNodeType();

		//add new curve to the view
		l_pcCurrentView->AddNewCurve(l_nPosition, l_sCurveName, -1, -1, l_nNodeType);
	}

	EndModal(wxID_OK);
}
void SP_DLG_SelectNodes::ReadOldUserSelection()
{
	spsa::ModelView* l_pcCurrentView = (*m_pcCurrentModel->GetViews())[m_nCurrentView];

	spsa::AttributeVectorAttribute* l_pcCurveInfo;

	wxString l_sItemName;

	spsa::RM_ROW_TYPE l_nType;

	CHECK_POINTER(l_pcCurrentView, return);

	CHECK_POINTER(l_pcCurrentView->GetCurveInfo(), return);

	l_pcCurveInfo = dynamic_cast<spsa::AttributeVectorAttribute*>(l_pcCurrentView->GetCurveInfo());

	SP_VectorString l_asItemsToDelete;

	l_asItemsToDelete.resize(l_pcCurveInfo->size());

	//load curves from the current view
	for (unsigned long l_nElement = 0; l_nElement < l_pcCurveInfo->size(); l_nElement++)
	{
		int l_nIndex = 0;

		CHECK_POINTER(l_pcCurveInfo->GetElement(l_nElement), return);

		spsa::AttributeStruct* l_pcAttribute = dynamic_cast<spsa::AttributeStruct*>(l_pcCurveInfo->GetElement(l_nElement));

		//position
		if (l_pcAttribute->GetField(wxT("INDEX")) != NULL)
		{
			l_nIndex = dynamic_cast<spsa::AttributeUlong*>(l_pcAttribute->GetField(wxT("INDEX")))->GetValue();
		}

		//name
		if (l_pcAttribute->GetField(wxT("CURVE_NAME")) != NULL)
		{
			l_sItemName = (l_pcAttribute->GetField(wxT("CURVE_NAME")))->GetValueString();
		}

		//curve type (colored/uncolored)
		if (l_pcAttribute->GetField(wxT("CURVE_TYPE")) != NULL)
		{
			l_nType = (spsa::RM_ROW_TYPE) (dynamic_cast<spsa::AttributeUint*>(l_pcAttribute->GetField(wxT("CURVE_TYPE")))->GetValue());
		}

		l_asItemsToDelete[l_nElement] = l_sItemName;

		//add the item to the selected list
		m_pcSelectedNodes->Append(l_sItemName, new SP_NodeClientData(l_nIndex, l_nType));
	}

	//move these element from available to selected list
	RemoveItemsFromList(m_pcAvailableNodes, l_asItemsToDelete);
}

void SP_DLG_SelectNodes::OnSelectColorNodesCheckUncheck(wxCommandEvent& event)
{
	//change color names
	ChangeColorName();
}

void SP_DLG_SelectNodes::ChangeColorName()
{
	SP_VectorString l_asNodeNames;

	SP_VectorString::const_iterator l_itString;

	if (!m_bIsColoredModel)
	{
		return;
	}

	CHECK_POINTER(m_pcColorName, return);
	CHECK_POINTER(m_pcSelectColorNodes, return);

	m_pcColorName->Clear();

	//check select color state
	if (m_pcSelectColorNodes->GetValue() == false)
	{
		return;
	}

	if (m_pcNodeTypes->GetSelection() == 1)
	{
		l_asNodeNames = dynamic_cast<spsa::ColoredModel*>(m_pcCurrentModel)->GetColoredTransitionNames();
	}
	else
	{
		l_asNodeNames = dynamic_cast<spsa::ColoredModel*>(m_pcCurrentModel)->GetColoredPlaceNames();
	}

	//node color names
	for (l_itString = l_asNodeNames.begin(); l_itString != l_asNodeNames.end(); l_itString++)
	{
		m_pcColorName->Append((*l_itString));
	}

	//set the selection to the first element
	m_pcColorName->SetSelection(0);

	ChangeUncoloredNodes(0);
}

void SP_DLG_SelectNodes::OnColorNameChanged(wxCommandEvent& event)
{
	unsigned long l_nCurrentNodePosition = event.GetSelection();

	ChangeUncoloredNodes(l_nCurrentNodePosition);
}

void SP_DLG_SelectNodes::ChangeUncoloredNodes(const unsigned int& p_NodeID)
{
	spsa::Ulong l_nStartNodePosition;
	spsa::Ulong l_nEndNodePosition;

	SP_VectorString l_asNodeNames;

	spsa::RM_ROW_TYPE l_nNodeType;

	if (m_pcNodeTypes->GetSelection() == 1)
	{
		dynamic_cast<spsa::ColoredModel*>(m_pcCurrentModel)->GetColorTransitionRange(p_NodeID, l_nStartNodePosition,

		l_nEndNodePosition);
		//get transition names
		l_asNodeNames = m_pcCurrentModel->GetTransitionNames();

		l_nNodeType = spsa::RM_ROW_TYPE_UNCOLORED_TRANSITION;
	}
	else
	{
		dynamic_cast<spsa::ColoredModel*>(m_pcCurrentModel)->GetColorPlaceRange(p_NodeID, l_nStartNodePosition, l_nEndNodePosition);

		//get transition name
		l_asNodeNames = m_pcCurrentModel->GetPlaceNames();

		l_nNodeType = spsa::RM_ROW_TYPE_UNCOLORED_PLACE;
	}

	//remove the current available node names and add the new ones
	m_pcAvailableNodes->Clear();

	for (unsigned l_nNodePos = l_nStartNodePosition; l_nNodePos <= l_nEndNodePosition; l_nNodePos++)
	{
		m_pcAvailableNodes->Append(l_asNodeNames[l_nNodePos], new SP_NodeClientData(l_nNodePos, l_nNodeType));
	}
}

