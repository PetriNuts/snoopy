/*
 * SP_DLG_ConvertElements.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 01.11.2010
 * Short Description:
 ==============================================================================*/

#include "sp_core/SP_Core.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/dialogs/SP_DLG_ConvertElements.h"
#include "snoopy.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gr/edges/SP_GR_ExtendedEdge.h"
#include "sp_gr/edges/SP_GR_ArrowEdge.h"
#include "sp_gr/edges/SP_GR_ExtendedEdge.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"
#include "sp_gr/attributes/SP_GR_MultiplicityAttribute.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogUnsigned.h"
#include "sp_gui/dialogs/dia_CPN/SP_WDG_ArcInscription.h"
#include "sp_gr/attributes/SP_GR_ColListAttribute.h" 

enum
{
	SP_ID_CHOICE_CONVERSION_TYPE
};

SP_DLG_ConvertElements::SP_DLG_ConvertElements(SP_MDI_View* p_pcView, wxWindow *p_parent, const wxString& p_title) :
	wxDialog(p_parent, -1, p_title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER), m_nCopyBrush(false), m_nCanConvert(true)
{
	SP_VectorString l_vsPossibleConversions;

	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	CHECK_POINTER(m_pcGraph, return);

	m_sNetClassName = m_pcGraph->GetNetclass()->GetName();

	m_pcView = p_pcView;
	//layout
	wxSizer* l_pcSizer = new wxBoxSizer(wxVERTICAL);

	wxSizer* l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxStaticText(this, -1, wxT("Convert to:")), 1, wxALL | wxEXPAND, 5);

	m_pcPossibleSelectionsComboBox = new wxChoice(this, SP_ID_CHOICE_CONVERSION_TYPE, wxDefaultPosition, wxSize(100, -1));
	l_pcRowSizer->Add(m_pcPossibleSelectionsComboBox, 0, wxALL, 5);

	l_pcSizer->Add(l_pcRowSizer);

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pcCopyBrushBox = new wxCheckBox(this, wxID_ANY, wxT("Copy graphics"));
	m_pcCopyBrushBox->SetValue(false);
	l_pcRowSizer->Add(m_pcCopyBrushBox, 1, wxALL | wxEXPAND, 5);

	l_pcSizer->Add(l_pcRowSizer);

	l_pcRowSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcRowSizer->Add(new wxButton(this, wxID_OK, wxT("Convert")), 1, wxALL, 5);
	l_pcRowSizer->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel")), 1, wxALL, 5);
	l_pcSizer->Add(l_pcRowSizer);

	SetSizerAndFit(l_pcSizer);

	//Get the selected element by the user
	p_pcView->FindSelectedElements(m_lSelectedElements);

	//Fill in the combo box with suitable elements
	m_pcPossibleSelectionsComboBox->Clear();
	//if the user select some thing
	if (!m_lSelectedElements.empty())
	{
		//Get the the class name of the selected element
		wxString l_sSelectedElementType = m_lSelectedElements.front()->GetClassName();

		if (dynamic_cast<SP_DS_Edge*>((m_lSelectedElements.front()))
			&& (dynamic_cast<SP_DS_Node*>(dynamic_cast<SP_DS_Edge*>((m_lSelectedElements.front()))->GetSource()))->GetNodeclass()->GetName().Find(wxT("Place")) == wxNOT_FOUND)
		{
			SP_MESSAGEBOX(wxT("Output edge is not allowed to convert to other types."), wxT("Notification"));

			m_nCanConvert = false;

			//close the dialog
			return;
		}

		if (dynamic_cast<SP_DS_Edge*>((m_lSelectedElements.front())) && (dynamic_cast<SP_DS_Edge*>((m_lSelectedElements.front())))->GetEdgeclass()->GetName() == SP_DS_MODIFIER_EDGE)
		{
			SP_MESSAGEBOX(wxT("Not support the conversion of modifier arcs yet."), wxT("Notification"));

			m_nCanConvert = false;

			return;
		}

		l_vsPossibleConversions = GetPossibleConversionType(l_sSelectedElementType, GetSelectedType(l_sSelectedElementType));

		//fill in combo box with the possible conversions type
		SP_VectorString::const_iterator itType = l_vsPossibleConversions.begin();
		for (; itType != l_vsPossibleConversions.end(); itType++)
		{
			m_pcPossibleSelectionsComboBox->Append((*itType));
		}

		m_pcPossibleSelectionsComboBox->SetSelection(0);

		//Set the selected element type
		m_sOriginalElementType = l_sSelectedElementType;
	}

	if (m_lSelectedElements.empty() || l_vsPossibleConversions.empty())
	{
		m_nCanConvert = false;
	}

	Centre(wxBOTH);

}
wxString SP_DLG_ConvertElements::GetSelectedType(const wxString& p_sSelectedElementType)
{
	if (p_sSelectedElementType.Find(wxT("Place")) != wxNOT_FOUND)
	{
		return wxT("Place");
	}
	else if (p_sSelectedElementType.Find(wxT("Transition")) != wxNOT_FOUND)
	{
		return wxT("Transition");
	}
	else if (p_sSelectedElementType.Find(wxT("Edge")) != wxNOT_FOUND)
	{
		return wxT("Edge");
	}
	else
	{
		return wxT("None");
	}
}
SP_VectorString SP_DLG_ConvertElements::GetPossibleConversionType(const wxString& p_sSelectedName, const wxString& p_sSelectedType)
{
	const SP_ListNodeclass* l_pcNodeClasses;
	SP_ListNodeclass::const_iterator itNodeClass;

	const SP_ListEdgeclass* l_pcEdgeClasses;
	SP_ListEdgeclass::const_iterator itEdgeClass;

	SP_VectorString vsTypes;

	//Get Nodeclasses
	l_pcNodeClasses = m_pcGraph->GetNodeclasses();
	CHECK_POINTER(l_pcNodeClasses, return vsTypes);

	//Get Edge classes
	l_pcEdgeClasses = m_pcGraph->GetEdgeclasses();
	CHECK_POINTER(l_pcEdgeClasses, return vsTypes);

	vsTypes.clear();
	// fill in the conversion table
	for (itNodeClass = l_pcNodeClasses->begin(); itNodeClass != l_pcNodeClasses->end(); itNodeClass++)
	{
		wxString l_sNCName = (*itNodeClass)->GetDisplayName();

		if (l_sNCName.Find(p_sSelectedType) != wxNOT_FOUND && p_sSelectedName != (*itNodeClass)->GetName())
		{
			//Add this name to the list
			if (!(l_sNCName == SP_DS_COARSE_PLACE || l_sNCName == SP_DS_COARSE_TRANSITION))
				vsTypes.push_back(l_sNCName);
		}
	}

	//if not found so far, Search edge class
	if (vsTypes.size() == 0)
		for (itEdgeClass = l_pcEdgeClasses->begin(); itEdgeClass != l_pcEdgeClasses->end(); itEdgeClass++)
		{
			wxString l_sNCName = (*itEdgeClass)->GetDisplayName();

			if (l_sNCName.Find(p_sSelectedType) != wxNOT_FOUND && p_sSelectedName != (*itEdgeClass)->GetName())
			{
				//Add this name to the list
				if (l_sNCName != SP_DS_MODIFIER_EDGE)
					vsTypes.push_back(l_sNCName);
			}
		}

	return vsTypes;
}
SP_DLG_ConvertElements::~SP_DLG_ConvertElements()
{

}

void SP_DLG_ConvertElements::DoConversion()
{
	m_sConvertToType = m_pcPossibleSelectionsComboBox->GetStringSelection();

	m_nCopyBrush = m_pcCopyBrushBox->GetValue();

	m_pcView->SelectAll(false);

	//dealing with edges
	if (m_sOriginalElementType.Find(wxT("Edge")) != wxNOT_FOUND)
	{
		ConvertEdges();
	}
	//Dealing with nodes
	else
	{
		ConvertNodes();
	}

	//this file needs to be saved
	SP_Core::Instance()->GetRootDocument()->Refresh();
	SP_Core::Instance()->GetRootDocument()->Modify(true);
}

SP_Graphic* SP_DLG_ConvertElements::FindGraphic(const SP_ListGraphic* p_pcOldGraphicList, const SP_ListGraphic* p_pcNewGraphicList, SP_Graphic* p_pcOldGraphic)
{
	SP_ListGraphic::const_iterator l_itNewGraphic = p_pcNewGraphicList->begin();

	for (SP_Graphic* l_pcOldGr : *p_pcOldGraphicList)
	{
		if (l_pcOldGr == p_pcOldGraphic)
		{
			return (*l_itNewGraphic);
		}
		++l_itNewGraphic;
	}

	return NULL;
}

bool SP_DLG_ConvertElements::UpdateSourceEdgeGraphics(SP_DS_Edge* p_pcEdge, const SP_ListGraphic* p_pcOldGraphicList, const SP_ListGraphic* p_pcNewGraphicList)
{
	SP_Graphic* l_pcNodeGraphics;

	CHECK_POINTER(p_pcEdge, return false);

	const SP_ListGraphic* l_pcGraphicList = p_pcEdge->GetGraphics();

	CHECK_POINTER(l_pcGraphicList, return false);

	for (SP_Graphic* l_pcGraphic : *l_pcGraphicList)
	{
		if (l_pcGraphic->GetGraphicType() == SP_GRAPHIC_EDGE)
		{
			SP_GR_Edge* l_pcGrEdge = static_cast<SP_GR_Edge*>(l_pcGraphic);

			l_pcNodeGraphics = FindGraphic(p_pcOldGraphicList, p_pcNewGraphicList, l_pcGrEdge->GetSource());

			CHECK_POINTER(l_pcNodeGraphics, return false);

			l_pcGrEdge->SetSource(l_pcNodeGraphics);
		}
	}

	return true;
}

bool SP_DLG_ConvertElements::UpdateTargetEdgeGraphics(SP_DS_Edge* p_pcEdge, const SP_ListGraphic* p_pcOldGraphicList, const SP_ListGraphic* p_pcNewGraphicList)
{
	SP_Graphic* l_pcNodeGraphics;

	CHECK_POINTER(p_pcEdge, return false);

	const SP_ListGraphic* l_pcGraphicList = p_pcEdge->GetGraphics();

	CHECK_POINTER(l_pcGraphicList, return false);

	for (SP_Graphic* l_pcGraphic : *l_pcGraphicList)
	{
		if (l_pcGraphic->GetGraphicType() == SP_GRAPHIC_EDGE)
		{
			SP_GR_Edge* l_pcGrEdge = static_cast<SP_GR_Edge*>(l_pcGraphic);

			l_pcNodeGraphics = FindGraphic(p_pcOldGraphicList, p_pcNewGraphicList, l_pcGrEdge->GetTarget());

			CHECK_POINTER(l_pcNodeGraphics, return false);

			l_pcGrEdge->SetTarget(l_pcNodeGraphics);
		}
	}

	return true;
}

bool SP_DLG_ConvertElements::ConvertNodes()
{
	SP_ListData::const_iterator itElement;
	SP_VectorString l_vsConvertedElements;

	l_vsConvertedElements.clear();
	SP_DS_Nodeclass* l_pcConvertToNodeClass = m_pcGraph->GetNodeclassByDisplayedName(m_sConvertToType);
	SP_DS_Nodeclass* l_pcOldNetClass = m_pcGraph->GetNodeclass(m_sOriginalElementType);

	for (itElement = m_lSelectedElements.begin(); itElement != m_lSelectedElements.end(); itElement++)
	{
		SP_DS_Node* l_pcOldNode = dynamic_cast<SP_DS_Node*>(*itElement);
		
		CHECK_POINTER(l_pcOldNode, return NULL);
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcOldNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		wxString l_sOldNodeClass = l_pcOldNode->GetClassName();
		wxString l_sToNodeClass = l_pcConvertToNodeClass->GetName();

		if (l_sOldNodeClass == l_sToNodeClass &&l_sToNodeClass.Contains("Place"))
		{
		   //a case which should not be happen, conversion between two similiar node classes
			SP_DS_Nodeclass* l_pcConvertToNodeClassTemp;
			if(m_sConvertToType=="Continuous Place")
			{
				l_pcConvertToNodeClassTemp = m_pcGraph->GetNodeclassByDisplayedName("Discrete Place");
			}
			else
			{
				l_pcConvertToNodeClassTemp = m_pcGraph->GetNodeclassByDisplayedName(m_sConvertToType);
			}
			
			//make the conversion
			SP_DS_Node* l_pcNewNode1 = ConvertNode(l_pcOldNode, l_pcConvertToNodeClassTemp);

			//delete old Node
			l_pcOldNetClass->RemoveElement(l_pcOldNode);
 
			l_pcNewNode1->Update();
			l_pcOldNode = l_pcNewNode1;
		}

		if(l_sOldNodeClass.Contains(wxT("Transition")) && l_sToNodeClass.Contains(wxT("Place")))
		{
			wxString l_sMsg = wxT("Can not convert this node (");
			l_sMsg << l_sName;
			l_sMsg << wxT("). Conversion Transition <-> Place is not allowed \n");

			SP_LOGERROR(l_sMsg);
			SP_MESSAGEBOX(l_sMsg, wxT("Error"));
			break;
		}
		if (l_sOldNodeClass.Contains(wxT("Place")) && l_sToNodeClass.Contains(wxT("Transition")))
		{
			wxString l_sMsg = wxT("Can not convert this node (");
			l_sMsg << l_sName;
			l_sMsg << wxT("). This type of conversion is not allowed\n");

			SP_LOGERROR(l_sMsg);
			SP_MESSAGEBOX(l_sMsg, wxT("Error"));
			break;

		}
		//check edge requirements here
		if (CheckEdgeRequirementofNode(l_pcOldNode, l_pcConvertToNodeClass->GetPrototype()) == false)
		{
			wxString l_sMsg = wxT("Can not convert this node (");
			l_sMsg << l_sName;
			l_sMsg << wxT("). This type of conversion is not allowed\n");

			SP_LOGERROR( l_sMsg);
			SP_MESSAGEBOX(l_sMsg, wxT("Error"));
			continue;
		}

		l_vsConvertedElements.push_back(l_sName);

		//make the conversion
		SP_DS_Node* l_pcNewNode = ConvertNode(l_pcOldNode, l_pcConvertToNodeClass);

		//delete old Node
		l_pcOldNetClass->RemoveElement(l_pcOldNode);

		wxDELETE(l_pcOldNode);

		//l_pcNewNode->SynchronizeAttributes();

		//refersh the new node
		l_pcNewNode->Update();

	}

	//tell the user about the conversions
	ShowConversionMessage(l_vsConvertedElements, m_pcGraph->GetNodeclass(m_sOriginalElementType)->GetDisplayName(), l_pcConvertToNodeClass->GetDisplayName());

	return true;
}

 
SP_DS_Node* SP_DLG_ConvertElements::ConvertNode(SP_DS_Node* p_pcOldNode, SP_DS_Nodeclass* p_pcNewNodeClass)
{
	SP_ListGraphic::const_iterator l_itOldGraphic;
	SP_ListGraphic::const_iterator l_itNewGraphic;

	CHECK_POINTER(p_pcOldNode, return NULL);

	SP_DS_Node* l_pcNewNode = p_pcNewNodeClass->NewElement(p_pcOldNode->GetNetnumber());

	//copy  values of attributes with same names
	const SP_ListAttribute* l_pcOldAttributeList = p_pcOldNode->GetAttributes();
	SP_ListAttribute::const_iterator l_itAttribute;

	for (l_itAttribute = l_pcOldAttributeList->begin();
			l_itAttribute != l_pcOldAttributeList->end();
			l_itAttribute++)
	{
		wxString l_sAttributeName = (*l_itAttribute)->GetName();

		SP_DS_Attribute* l_pcNewAttribute = l_pcNewNode->GetAttribute(l_sAttributeName);

		if (l_pcNewAttribute != NULL)
		{
			l_pcNewAttribute->CopyValueFrom((*l_itAttribute));
			l_pcNewAttribute->SetShow((*l_itAttribute)->GetShow());
		}
	}

	const SP_ListGraphic* l_pcOldGraphicList = p_pcOldNode->GetGraphics();
	const SP_ListGraphic* l_pcNewGraphicList = l_pcNewNode->GetGraphics();

	CHECK_POINTER(l_pcOldGraphicList, return NULL);
	CHECK_POINTER(l_pcNewGraphicList, return NULL);

	//add graphic to the new node as in the old one. We already have one
	for (l_itOldGraphic = l_pcOldGraphicList->begin();
			l_itOldGraphic != l_pcOldGraphicList->end();
			l_itOldGraphic++)
	{
		SP_Graphic* l_pcOldGraphic = (*l_itOldGraphic);

		SP_Graphic* l_pcNewGraphic;
		if(l_itOldGraphic == l_pcOldGraphicList->begin())
		{
			l_pcNewGraphic = l_pcNewGraphicList->front();
			l_pcNewGraphic->SetNetnumber(l_pcOldGraphic->GetNetnumber());
		}
		else
		{
			l_pcNewGraphic = l_pcNewNode->NewGraphicInSubnet(l_pcOldGraphic->GetNetnumber());
		}

		//copy graphics
		if (m_nCopyBrush)
		{
			l_pcNewGraphic->SetBrush(l_pcOldGraphic->GetBrush());

			l_pcNewGraphic->SetPen(l_pcOldGraphic->GetPen());
		}

		l_pcNewGraphic->SetHeight(l_pcOldGraphic->GetHeight());
		l_pcNewGraphic->SetWidth(l_pcOldGraphic->GetWidth());

		l_pcNewGraphic->SetPosX(l_pcOldGraphic->GetPosX());
		l_pcNewGraphic->SetPosY(l_pcOldGraphic->GetPosY());
		l_pcNewGraphic->SetOffsetX(l_pcOldGraphic->GetOffsetX());
		l_pcNewGraphic->SetOffsetY(l_pcOldGraphic->GetOffsetY());

		SP_ListGraphic::const_iterator l_itOldGraphicChild = l_pcOldGraphic->GetGraphicChildren()->begin();
		SP_ListGraphic::const_iterator l_itNewGraphicChild = l_pcNewGraphic->GetGraphicChildren()->begin();

		for (; l_itNewGraphicChild != l_pcNewGraphic->GetGraphicChildren()->end();
				++l_itNewGraphicChild, ++l_itOldGraphicChild)
		{
			SP_Graphic* l_pcOldGraphicChild = (*l_itOldGraphicChild);
			SP_Graphic* l_pcNewGraphicChild = (*l_itNewGraphicChild);
			l_pcNewGraphicChild->SetPosX(l_pcOldGraphicChild->GetPosX());
			l_pcNewGraphicChild->SetPosY(l_pcOldGraphicChild->GetPosY());
			l_pcNewGraphicChild->SetOffsetX(l_pcOldGraphicChild->GetOffsetX());
			l_pcNewGraphicChild->SetOffsetY(l_pcOldGraphicChild->GetOffsetY());
		}
		l_pcNewGraphic->AddToCanvas(m_pcView->GetCanvas(), l_pcOldGraphic->GetPrimitive()->GetX(), l_pcOldGraphic->GetPrimitive()->GetY(), false);
		 
	}

	//copy source and targets edges
	const SP_ListEdge* l_pcEdgeList = p_pcOldNode->GetSourceEdges();
	SP_ListEdge::const_iterator l_itEdge;

	for (l_itEdge = l_pcEdgeList->begin(); l_itEdge != l_pcEdgeList->end(); l_itEdge++)
	{
		l_pcNewNode->AddSourceEdge((*l_itEdge));

		UpdateSourceEdgeGraphics((*l_itEdge), l_pcOldGraphicList, l_pcNewGraphicList);
	}

	//change edge list
	l_pcEdgeList = p_pcOldNode->GetTargetEdges();

	for (l_itEdge = l_pcEdgeList->begin(); l_itEdge != l_pcEdgeList->end(); l_itEdge++)
	{
		l_pcNewNode->AddTargetEdge((*l_itEdge));

		UpdateTargetEdgeGraphics((*l_itEdge), l_pcOldGraphicList, l_pcNewGraphicList);
	}

	//remove the node graphics from the canvas
	SP_Graphic* l_pcOldGraphic = (*l_pcOldGraphicList->begin());
	RemoveGraphicFromCanvas(l_pcOldGraphic);

	return l_pcNewNode;
}

void SP_DLG_ConvertElements::RemoveGraphicFromCanvas(SP_Graphic *p_pcGraphic)
{
	//SP_LOGDEBUG(wxT("entered RemoveGraphicFromCanvas(graphic)..."));

	p_pcGraphic->RemoveFromCanvas();
	//also removing graphic children (e.g. token graphics, labels)
	SP_ListGraphic *gr = p_pcGraphic->GetGraphicChildren();
	if (!gr)
		return;
	SP_ListGraphic::iterator itg;
	for (itg = gr->begin(); itg != gr->end(); ++itg)
	{
		RemoveGraphicFromCanvas(*itg);
	}
}

bool SP_DLG_ConvertElements::ConvertEdges()
{
	SP_ListData::const_iterator itElement;
	SP_VectorString l_vsConvertedElements;

	SP_DS_Edgeclass* l_pcConvertToNodeClass = m_pcGraph->GetEdgeclassByDisplayedName(m_sConvertToType);
	for (itElement = m_lSelectedElements.begin(); itElement != m_lSelectedElements.end(); itElement++)
	{
		SP_DS_Edge* l_pcNewNode = dynamic_cast<SP_DS_Edge*>(*itElement);
		if(l_pcNewNode==NULL) 
		{
			continue;
		}
		//check edge requirements here
		if (EdgeRequirement(l_pcConvertToNodeClass, l_pcNewNode->GetSource(), l_pcNewNode->GetTarget()) == false)
		{
			SP_LOGERROR( wxT("Can not convert this edge. This type of conversion is not allowed\n"));
			continue;
		}

		l_pcConvertToNodeClass->AddElement(l_pcNewNode);

		wxString l_sSourceName = dynamic_cast<SP_DS_Node*>(l_pcNewNode->GetSource())->GetAttribute(wxT("Name"))->GetValueString();
		wxString l_sTargetName = dynamic_cast<SP_DS_Node*>(l_pcNewNode->GetTarget())->GetAttribute(wxT("Name"))->GetValueString();
		l_vsConvertedElements.push_back(l_sSourceName + wxT("->") + l_sTargetName);

		wxString l_sNewEdgeClassType = l_pcConvertToNodeClass->GetName();
		long l_nDesignType;

		if (l_sNewEdgeClassType == SP_DS_EDGE)
		{
			l_nDesignType = dynamic_cast<SP_GR_Edge*>(*(l_pcConvertToNodeClass->GetPrototype()->GetGraphics()->begin()))->GetDesignType();

			SP_ListGraphic::const_iterator l_itGr;
			for (l_itGr = l_pcNewNode->GetGraphics()->begin(); l_itGr != l_pcNewNode->GetGraphics()->end(); l_itGr++)
			{
				SP_Graphic* l_pcNewGr = *l_itGr;
				SP_GR_Edge* l_pcExtendedEdge = dynamic_cast<SP_GR_Edge*>(l_pcNewGr);

				l_pcExtendedEdge->SetDesignType(l_nDesignType);
				dynamic_cast<SP_GR_BaseEdge*>(l_pcExtendedEdge->GetPrimitive())->ClearArrowsAtPosition(ARROW_POSITION_END);
				dynamic_cast<SP_GR_BaseEdge*>(l_pcExtendedEdge->GetPrimitive())->AddArrow(l_nDesignType, ARROW_POSITION_END, 7);
			}

		}
		if (l_sNewEdgeClassType == SP_DS_INHIBITOR_EDGE || l_sNewEdgeClassType == SP_DS_READ_EDGE || l_sNewEdgeClassType == SP_DS_EQUAL_EDGE || l_sNewEdgeClassType == SP_DS_RESET_EDGE)
		{
			l_nDesignType = dynamic_cast<SP_GR_ArrowEdge*>(*(l_pcConvertToNodeClass->GetPrototype()->GetGraphics()->begin()))->GetDesignType();

			SP_ListGraphic::const_iterator l_itGr;
			for (l_itGr = l_pcNewNode->GetGraphics()->begin(); l_itGr != l_pcNewNode->GetGraphics()->end(); l_itGr++)
			{
				SP_Graphic* l_pcNewGr = *l_itGr;
				SP_GR_ArrowEdge* l_pcExtendedEdge = dynamic_cast<SP_GR_ArrowEdge*>(l_pcNewGr);

				l_pcExtendedEdge->SetDesignType(l_nDesignType);
				dynamic_cast<SP_GR_BaseEdge*>(l_pcExtendedEdge->GetPrimitive())->ClearArrowsAtPosition(ARROW_POSITION_END);
				dynamic_cast<SP_GR_BaseEdge*>(l_pcExtendedEdge->GetPrimitive())->AddArrow(l_nDesignType, ARROW_POSITION_END, 7);

				if (l_sNewEdgeClassType == SP_DS_RESET_EDGE)
				{
					l_pcExtendedEdge->AddAnotherArrow(l_nDesignType, ARROW_POSITION_END, 7);
				}

				//find
				SP_DS_Attribute* l_pcMultAttr = NULL;
				const SP_ListAttribute* l_pcAttList = l_pcNewNode->GetAttributes();
				wxString l_sRateColListOld;
				for (SP_DS_Attribute* l_pcAttr : *l_pcAttList)
				{
					wxString l_sAttrName = l_pcAttr->GetName();
					if (l_sAttrName == wxT("Multiplicity") || l_sAttrName == SP_DS_CPN_INSCRIPTION)
					{
						l_pcMultAttr = l_pcAttr;
						break;
					}
				}

				if (l_sNewEdgeClassType == SP_DS_RESET_EDGE)
				{
					if (l_pcMultAttr)
					{
						l_pcMultAttr->RemoveAllGraphics();
						l_pcNewNode->RemoveAttribute(l_pcMultAttr);
					}
				}
				else
				{
					if (!l_pcMultAttr)
					{
						//add new attribute
						if (m_sNetClassName.Find(wxT("Col")) == wxNOT_FOUND)
						{
							l_pcMultAttr = new SP_DS_MultiplicityAttribute(wxT("Multiplicity"));
							l_pcMultAttr->RegisterDialogWidget(new SP_WDG_DialogUnsigned(wxT("General"), 0));
							l_pcMultAttr->SetGlobalShow();
							SP_Graphic* gr;
							gr = l_pcMultAttr->AddGraphic(new SP_GR_MultiplicityAttribute(l_pcMultAttr));
							gr->SetOffsetX(20);
							gr->SetShow(TRUE);
							l_pcNewNode->AddAttribute(l_pcMultAttr);
						}
						else
						{
							l_pcMultAttr = new SP_DS_ColListAttribute(SP_DS_CPN_INSCRIPTION, SP_COLLIST_STRING, 2);
							l_pcMultAttr->RegisterDialogWidget(new SP_WDG_ArcInscription(wxT("Expression")));

							SP_DS_ColListAttribute* l_pcColList;
							l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcMultAttr);
							l_pcColList->SetColLabel(0, wxT("Inscription set"));
							l_pcColList->SetColLabel(1, wxT("Inscription"));
							int l_nNewRow = l_pcColList->AppendEmptyRow();
							l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
							l_pcColList->SetCell(l_nNewRow, 1, wxT("dot "));
							l_pcMultAttr->SetGlobalShow();

							SP_Graphic* l_pcGrAttr;
							l_pcGrAttr = l_pcMultAttr->AddGraphic(new SP_GR_ColListAttribute(l_pcMultAttr, wxT("%")));
							l_pcGrAttr->SetTextColor(wxGetApp().GetElementPrefs()->GetExpressionColor(m_sNetClassName));
							l_pcGrAttr->SetOffsetX(25);
							l_pcGrAttr->SetOffsetY(0);
							l_pcGrAttr->SetShow(TRUE);

						}
					}
				}
			}

		}
		if (l_sNewEdgeClassType == SP_DS_MODIFIER_EDGE)
		{
			/*
			 long l_nDesignArrowTypeLeft;
			 long l_nDesignArrowTypeRight;
			 l_nDesignType = dynamic_cast<SP_GR_ExtendedEdge*>(*(l_pcConvertToNodeClass->GetPrototype()->GetGraphics()->begin()))->GetDesignType();
			 l_nDesignArrowTypeLeft = dynamic_cast<SP_GR_ExtendedEdge*>(*(l_pcConvertToNodeClass->GetPrototype()->GetGraphics()->begin()))->GetDesignArrowTypeLeft();
			 l_nDesignArrowTypeRight = dynamic_cast<SP_GR_ExtendedEdge*>(*(l_pcConvertToNodeClass->GetPrototype()->GetGraphics()->begin()))->GetDesignArrowTypeRight();

			 SP_ListGraphic::const_iterator l_itGr;
			 for( l_itGr = l_pcNewNode->GetGraphics()->begin(); l_itGr != l_pcNewNode->GetGraphics()->end(); l_itGr++)
			 {
			 SP_Graphic* l_pcNewGr = *l_itGr;
			 SP_GR_ExtendedEdge* l_pcExtendedEdge = dynamic_cast<SP_GR_ExtendedEdge*>(l_pcNewGr);
			 l_pcExtendedEdge->SetDesignType( l_nDesignType );
			 l_pcExtendedEdge->SetDesignArrowTypeLeft( l_nDesignArrowTypeLeft );
			 l_pcExtendedEdge->SetDesignArrowTypeRight( l_nDesignArrowTypeRight );
			 }
			 */

		}
	}
	if (l_pcConvertToNodeClass==NULL) return true;
	//tell the user about the conversions
	ShowConversionMessage(l_vsConvertedElements, m_sOriginalElementType, l_pcConvertToNodeClass->GetDisplayName());

	return true;
}

void SP_DLG_ConvertElements::ShowConversionMessage(const SP_VectorString& p_asConvertedNames, const wxString& p_sOriginalType, const wxString& p_sConvertedType)
{
	SP_VectorString::const_iterator l_itName;

	if (p_asConvertedNames.empty())
	{
		return;
	}

	wxString l_sss = wxT("The following elements have been converted from ") + p_sOriginalType + wxT(" to ") + p_sConvertedType;

	for (l_itName = p_asConvertedNames.begin(); l_itName != p_asConvertedNames.end(); l_itName++)
	{
		l_sss << wxT("\n") + (*l_itName);
	}

	SP_LOGMESSAGE( l_sss + wxT("\n"));
	SP_MESSAGEBOX(l_sss, wxT("Notice"));
}

bool SP_DLG_ConvertElements::EdgeRequirement(SP_DS_Edgeclass* p_pcEdgeclass, SP_Data* p_pcSrc, SP_Data* p_pcTrg)
{
	SP_DS_Netclass* l_pcNetClass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass();

	CHECK_POINTER(l_pcNetClass, return false);

	return l_pcNetClass->EdgeRequirement(p_pcEdgeclass, p_pcSrc, p_pcTrg);
}

bool SP_DLG_ConvertElements::CheckEdgeRequirementofNode(SP_DS_Node* p_pcNodeToCheck, SP_DS_Node* p_pcToConvertType)
{
	SP_ListEdge::const_iterator l_itEdge;

	const SP_ListEdge* l_pcEdgeList;

	//check source edges
	l_pcEdgeList = p_pcNodeToCheck->GetSourceEdges();

	for (l_itEdge = l_pcEdgeList->begin(); l_itEdge != l_pcEdgeList->end(); l_itEdge++)
	{
		if (EdgeRequirement((*l_itEdge)->GetEdgeclass(), p_pcToConvertType, (*l_itEdge)->GetTarget()) == false)
		{
			return false;
		}
	}

	//check target edges
	l_pcEdgeList = p_pcNodeToCheck->GetTargetEdges();

	for (l_itEdge = l_pcEdgeList->begin(); l_itEdge != l_pcEdgeList->end(); l_itEdge++)
	{
		if (EdgeRequirement((*l_itEdge)->GetEdgeclass(), (*l_itEdge)->GetSource(), p_pcToConvertType) == false)
		{
			return false;
		}
	}

	return true;

}

