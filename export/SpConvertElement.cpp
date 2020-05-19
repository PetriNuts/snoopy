#include "SpConvertElement.h"
#include "sp_core/base/SP_Error.h"
#include "sp_defines.h"
#include "sp_utilities.h"

#include "sp_core/SP_Core.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"

SpConvertElement::SpConvertElement(double p_dMVal)
{
	m_dSourceMarking = p_dMVal;
}
SpConvertElement::~SpConvertElement()
 {}

 SP_DS_Node* SpConvertElement::ConvertNode(SP_DS_Node* p_pcOldNode, SP_DS_Nodeclass* p_pcNewNodeClass)
 {
	 SP_ListGraphic::const_iterator l_itOldGraphic;
	 SP_ListGraphic::const_iterator l_itNewGraphic;

	// CHECK_POINTER(p_pcOldNode, return NULL);

	 SP_DS_Node* l_pcNewNode = p_pcNewNodeClass->NewElement(p_pcOldNode->GetNetnumber());

	 //copy  values of attributes with same names
	 const SP_ListAttribute* l_pcOldAttributeList = p_pcOldNode->GetAttributes();//
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

	 //CHECK_POINTER(l_pcOldGraphicList, return NULL);
	/// CHECK_POINTER(l_pcNewGraphicList, return NULL);

	 //add graphic to the new node as in the old one. We already have one
	 for (l_itOldGraphic = l_pcOldGraphicList->begin();
		 l_itOldGraphic != l_pcOldGraphicList->end();
		 l_itOldGraphic++)
	 {
		 SP_Graphic* l_pcOldGraphic = (*l_itOldGraphic);

		 SP_Graphic* l_pcNewGraphic;
		 if (l_itOldGraphic == l_pcOldGraphicList->begin())
		 {
			 l_pcNewGraphic = l_pcNewGraphicList->front();
			 l_pcNewGraphic->SetNetnumber(l_pcOldGraphic->GetNetnumber());
		 }
		 else
		 {
			 l_pcNewGraphic = l_pcNewNode->NewGraphicInSubnet(l_pcOldGraphic->GetNetnumber());
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

	 }

	 //copy source and targets edges
	 const SP_ListEdge* l_pcEdgeList = p_pcOldNode->GetSourceEdges();
	 SP_ListEdge::const_iterator l_itEdge;

	 for (l_itEdge = l_pcEdgeList->begin(); l_itEdge != l_pcEdgeList->end(); l_itEdge++)
	 {
		 l_pcNewNode->AddSourceEdge((*l_itEdge));
		 
		// UpdateSourceEdgeGraphics((*l_itEdge), l_pcOldGraphicList, l_pcNewGraphicList);//
	 }

	 //change edge list
	 l_pcEdgeList = p_pcOldNode->GetTargetEdges();

	 for (l_itEdge = l_pcEdgeList->begin(); l_itEdge != l_pcEdgeList->end(); l_itEdge++)
	 {
		 l_pcNewNode->AddTargetEdge((*l_itEdge));

		// UpdateTargetEdgeGraphics((*l_itEdge), l_pcOldGraphicList, l_pcNewGraphicList);//
		 //SP_Graphic* l_pcNodeGraphics;//

		// CHECK_POINTER((*l_itEdge), return false);

		// const SP_ListGraphic* l_pcGraphicList = (*l_itEdge)->GetGraphics();//

		/// CHECK_POINTER(l_pcGraphicList, return false);

		// for (SP_Graphic* l_pcGraphic : *l_pcGraphicList)//
		// {//
		//	 if (l_pcGraphic->GetGraphicType() == SP_GRAPHIC_EDGE)//
		//	 {//
		//		 SP_GR_Edge* l_pcGrEdge = static_cast<SP_GR_Edge*>(l_pcGraphic);//

			//	 l_pcNodeGraphics = FindGraphic(l_pcOldGraphicList, l_pcNewGraphicList, l_pcGrEdge->GetTarget());//

			//	 CHECK_POINTER(l_pcNodeGraphics, return false);

			//	 l_pcGrEdge->SetTarget(l_pcNodeGraphics);//
		//	 }//
		 //}//




		 /////////////////
	 }

	 SP_DS_Attribute* nameAttr = l_pcNewNode->GetAttribute(wxT("Name"));
	 wxString valString = nameAttr->GetValueString();
	 m_lConvertedElements.push_back(valString);
	 return l_pcNewNode;
 }

 

 bool SpConvertElement::UpdateSourceEdgeGraphics(SP_DS_Edge& p_edge, const SP_ListGraphic& p_oldGraphicList, const SP_ListGraphic& p_newGraphicList)  const
 {
	 SP_Graphic* l_pcNodeGraphics;

	// CHECK_POINTER(p_pcEdge, return false);

	 const SP_ListGraphic* l_pcGraphicList = p_edge.GetGraphics();

	// CHECK_POINTER(l_pcGraphicList, return false);

	 for (SP_Graphic* l_pcGraphic : *l_pcGraphicList)
	 {
		 if (l_pcGraphic->GetGraphicType() == SP_GRAPHIC_EDGE)
		 {
			 SP_GR_Edge* l_pcGrEdge = static_cast<SP_GR_Edge*>(l_pcGraphic);

			 l_pcNodeGraphics = FindGraphic(p_oldGraphicList, p_newGraphicList, *(l_pcGrEdge->GetSource()));

		//	 CHECK_POINTER(l_pcNodeGraphics, return false);

			 l_pcGrEdge->SetSource(l_pcNodeGraphics);
		 }
	 }

	 return true;
 }
 SP_Graphic* SpConvertElement::FindGraphic(const SP_ListGraphic& p_oldGraphicList, const SP_ListGraphic& p_newGraphicList, const SP_Graphic& p_oldGraphic) const
 {
	 SP_ListGraphic::const_iterator l_itNewGraphic = p_newGraphicList.begin();

	 for (SP_Graphic* l_pcOldGr : p_oldGraphicList)
	 {
		 if (l_pcOldGr == &p_oldGraphic)
		 {
			 return (*l_itNewGraphic);
		 }
		 ++l_itNewGraphic;
	 }

	 return NULL;
 }

 bool SpConvertElement::UpdateTargetEdgeGraphics(SP_DS_Edge& p_edge, const SP_ListGraphic& p_oldGraphicList, const SP_ListGraphic& p_newGraphicList) const
 {
	 SP_Graphic* l_pcNodeGraphics;

	// CHECK_POINTER(p_pcEdge, return false);

	 const SP_ListGraphic* l_pcGraphicList = p_edge.GetGraphics();

	// CHECK_POINTER(l_pcGraphicList, return false);

	 for (SP_Graphic* l_pcGraphic : *l_pcGraphicList)
	 {
		 if (l_pcGraphic->GetGraphicType() == SP_GRAPHIC_EDGE)
		 {
			 SP_GR_Edge* l_pcGrEdge = static_cast<SP_GR_Edge*>(l_pcGraphic);

			 l_pcNodeGraphics = FindGraphic(p_oldGraphicList, p_newGraphicList, *l_pcGrEdge->GetTarget());

	//		 CHECK_POINTER(l_pcNodeGraphics, return false);

			 l_pcGrEdge->SetTarget(l_pcNodeGraphics);
		 }
	 }

	 return true;
 }
 bool  SpConvertElement::IsConverted(wxString& elementName)
 {
	 bool result = false;
	 for (vector<wxString>::const_iterator i = m_lConvertedElements.begin(); i != m_lConvertedElements.end(); ++i) 
		 { // process i
			 if (*i == elementName)
			 {
				 result = true;
				 break;

			 }
		 }

	 return result;

 }
 
 SP_DS_Node* SpConvertElement::Clone(SP_DS_Node& p_pcOldNode, SP_DS_Nodeclass& p_pcNewNodeClass) const
 {
	 SP_ListGraphic::const_iterator l_itOldGraphic;
	 SP_ListGraphic::const_iterator l_itNewGraphic;

	 // CHECK_POINTER(p_pcOldNode, return NULL);

	 SP_DS_Node* l_pcNewNode = p_pcNewNodeClass.NewElement(p_pcOldNode.GetNetnumber());

	 //copy  values of attributes with same names
	 const SP_ListAttribute* l_pcOldAttributeList = p_pcOldNode.GetAttributes();
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

	 const SP_ListGraphic* l_pcOldGraphicList = p_pcOldNode.GetGraphics();
	 const SP_ListGraphic* l_pcNewGraphicList = l_pcNewNode->GetGraphics();

	 //add graphic to the new node as in the old one. We already have one
	 for (l_itOldGraphic = l_pcOldGraphicList->begin();
		 l_itOldGraphic != l_pcOldGraphicList->end();
		 l_itOldGraphic++)
	 {
		 SP_Graphic* l_pcOldGraphic = (*l_itOldGraphic);

		 SP_Graphic* l_pcNewGraphic;
		 if (l_itOldGraphic == l_pcOldGraphicList->begin())
		 {
			 l_pcNewGraphic = l_pcNewGraphicList->front();
			 l_pcNewGraphic->SetNetnumber(l_pcOldGraphic->GetNetnumber());
		 }
		 else
		 {
			 l_pcNewGraphic = l_pcNewNode->NewGraphicInSubnet(l_pcOldGraphic->GetNetnumber());
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

	 }

	 //copy source and targets edges
	 const SP_ListEdge* l_pcEdgeList = p_pcOldNode.GetSourceEdges();
	 SP_ListEdge::const_iterator l_itEdge;
	 

	 for (l_itEdge = l_pcEdgeList->begin(); l_itEdge != l_pcEdgeList->end(); l_itEdge++)
	 {
		
		 l_pcNewNode->AddSourceEdge((*l_itEdge));

		  UpdateSourceEdgeGraphics((**l_itEdge),* l_pcOldGraphicList, *l_pcNewGraphicList);//


	 }

	 //change edge list
	 l_pcEdgeList = p_pcOldNode.GetTargetEdges();

	 for (l_itEdge = l_pcEdgeList->begin(); l_itEdge != l_pcEdgeList->end(); l_itEdge++)
	 {


		 l_pcNewNode->AddTargetEdge(*l_itEdge);

		 UpdateTargetEdgeGraphics(**l_itEdge, *l_pcOldGraphicList, *l_pcNewGraphicList);
		 
		 
	 }
		 
	
	 return l_pcNewNode;
 }
 
 void SpConvertElement::ChangeRepresentation(SP_DS_Node* p_pcOldNode, const bool& p_bChangeMarking)
 {
	 SP_ListGraphic::const_iterator l_itOldGraphic;
	 //	CHECK_POINTER(p_pcOldNode,return );

	 const SP_ListGraphic* l_pcOldGraphicList = p_pcOldNode->GetGraphics();
	 wxString l_sOldNodeClass = p_pcOldNode->GetClassName();
	 for (l_itOldGraphic = l_pcOldGraphicList->begin();
		 l_itOldGraphic != l_pcOldGraphicList->end();
		 l_itOldGraphic++)
	 {
		 if (l_sOldNodeClass == wxT("Place") || l_sOldNodeClass == wxT("Transition"))//Discrete place
		 {//convert to continuous representaion

			 SP_Graphic* l_pcOldGraphic = (*l_itOldGraphic);
			 wxColour l_pcGray(128, 128, 128);
			 l_pcOldGraphic->SetPen(new wxPen(l_pcGray, 1));
			 wxColour l_pcWhite(255, 255, 255);
			 wxBrush* l_pcBrush = wxTheBrushList->FindOrCreateBrush(*wxWHITE, l_pcOldGraphic->GetBrush()->GetStyle());
			 l_pcOldGraphic->SetBrush(l_pcBrush);
			 l_pcOldGraphic->SetThickness(3);
			 
		 }
		 else if ((l_sOldNodeClass.Contains(wxT("Place")) && l_sOldNodeClass.Contains(wxT("Continuous"))) || l_sOldNodeClass.Contains(wxT("Transition, Continuous")))
		 {
			 //convert to discrete representaion
			 SP_Graphic* l_pcOldGraphic = (*l_itOldGraphic);
			 l_pcOldGraphic->SetPen(new wxPen(*wxBLACK, 1));
			 wxColour l_pcWhite(255, 255, 255);
			 wxBrush* l_pcBrush = wxTheBrushList->FindOrCreateBrush(*wxWHITE, l_pcOldGraphic->GetBrush()->GetStyle());
			 l_pcOldGraphic->SetBrush(l_pcBrush);
			 l_pcOldGraphic->SetThickness(1);
			 if (p_bChangeMarking &&l_sOldNodeClass.Contains(wxT("Place")))
			 {
				 SP_DS_Attribute* l_pcOldAttribute = (p_pcOldNode)->GetAttribute(wxT("Marking"));
				 wxString valString = l_pcOldAttribute->GetValueString();

				// m_sSourceMarking = valString.ToStdString();
				 double l_dmarking;
				 bool l_bIsNumeric=valString.ToDouble(&l_dmarking);
				 m_dSourceMarking = l_dmarking;
				 m_sMarking = valString;

				 if (l_bIsNumeric)
				 {
					 if (floor(l_dmarking) == ceil(l_dmarking))
					 {
						 //do nothing
					 }
					 else
					 {
						 int roundedVal = round(l_dmarking);
						 wxString l_sRoundedMarking;// = wxString::Format(wxT("%i"), (int)l_dmarking);
						 l_sRoundedMarking << roundedVal;
						 (p_pcOldNode)->GetAttribute(wxT("Marking"))->SetValueString(l_sRoundedMarking);

					 }

				 }
			 }

		 }



	 }

 }


 void SpConvertElement::ResetNodeRepresentation(SP_DS_Node* p_pcOldNode, const bool& p_bChangeMarking)
 {
	 SP_ListGraphic::const_iterator l_itOldGraphic;
	 //	CHECK_POINTER(p_pcOldNode,return );

	 const SP_ListGraphic* l_pcOldGraphicList = p_pcOldNode->GetGraphics();
	 wxString l_sOldNodeClass = p_pcOldNode->GetClassName();
	 for (l_itOldGraphic = l_pcOldGraphicList->begin();
		 l_itOldGraphic != l_pcOldGraphicList->end();
		 l_itOldGraphic++)
	 {
		 if (l_sOldNodeClass == wxT("Place") || l_sOldNodeClass == wxT("Transition"))//Discrete place or Stochastic transition
		 {
			 SP_Graphic* l_pcOldGraphic = (*l_itOldGraphic);
			 l_pcOldGraphic->SetPen(new wxPen(*wxBLACK, 1));
			 wxColour l_pcWhite(255, 255, 255);
			 wxBrush* l_pcBrush = wxTheBrushList->FindOrCreateBrush(*wxWHITE, l_pcOldGraphic->GetBrush()->GetStyle());
			 l_pcOldGraphic->SetBrush(l_pcBrush);
			 l_pcOldGraphic->SetThickness(1);
		 }
		 else if (l_sOldNodeClass == wxT("Place, Continuous") || l_sOldNodeClass == wxT("Transition, Continuous"))
		 {
			 SP_Graphic* l_pcOldGraphic = (*l_itOldGraphic);
			 wxColour l_pcGray(128, 128, 128);
			 l_pcOldGraphic->SetPen(new wxPen(l_pcGray, 1));
			 wxColour l_pcWhite(255, 255, 255);
			 wxBrush* l_pcBrush = wxTheBrushList->FindOrCreateBrush(*wxWHITE, l_pcOldGraphic->GetBrush()->GetStyle());
			 l_pcOldGraphic->SetBrush(l_pcBrush);
			 l_pcOldGraphic->SetThickness(3);

			 //Check Reset flag 
			 if(l_sOldNodeClass == wxT("Place, Continuous")&&p_bChangeMarking)
			 {
				 wxString l_sMarkingVal;
				 l_sMarkingVal << m_dSourceMarking;
				 (p_pcOldNode)->GetAttribute(wxT("Marking"))->SetValueString(m_sMarking);
			 }
		 }
	 }


	 p_pcOldNode->Update();
 }