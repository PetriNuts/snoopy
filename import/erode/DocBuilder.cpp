/*
 * DocBuilder.cpp
 *
 *  Created on: 5 Jan 2018
 *      Author: JCH
 */
#include "sp_core/SP_Core.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"
#include "sp_gui/dialogs/SP_DLG_LayoutProperties.h"

#include "snoopy.h"

#include "import/erode/ErodeNet.h"

#include "import/erode/DocBuilder.h"

namespace sp {
namespace import {
namespace erode {
/**
 *
 */
//DocBuilder::DocBuilder() {
//	// TODO Auto-generated constructor stub
//
//}
DocBuilder::DocBuilder() = default;
/**
 *
 */
//DocBuilder::~DocBuilder() {
//	// TODO Auto-generated destructor stub
//}
DocBuilder::~DocBuilder() = default;
/**
 *
 */
DocBuilder& DocBuilder::appendConstants(const std::vector<Constant>& vConstants) {
	// Get doc
	std::string strNumType = "int";
	SP_MDI_Doc& mdiDoc = getDoc();
	SP_DS_Graph* pDsGraph = mdiDoc.GetGraph();
	// For registering constants
	SP_DS_FunctionRegistry* pFunctionRegistry = NULL;
	//
	SP_DS_Metadataclass* pMetaDataClass = pDsGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	if(!pMetaDataClass) {
		//TODO throw
		return *this;
	}
	SP_DS_Metadata* pMetadataConstant = NULL;
	for (auto constant : vConstants) {
		switch (constant.m_value.m_type) {
			case num::INT:
				strNumType = "int";
				break;
			case num::DOUBLE:
				strNumType = "double";
				break;
			default:
				strNumType = "double";
		}
		pMetadataConstant = pMetaDataClass->NewElement(1);
		pMetadataConstant->GetAttribute("Name")->SetValueString(constant.m_strName);
		pMetadataConstant->GetAttribute("Group")->SetValueString("all");
		pMetadataConstant->GetAttribute("Type")->SetValueString(strNumType);

		SP_DS_ColListAttribute* pColListAttr = dynamic_cast<SP_DS_ColListAttribute*>(pMetadataConstant->GetAttribute("ValueList"));
		/*
		 * TODO
		 * check type of the expression if num is NaN
		 * for now default is = double
		 */
		pColListAttr->SetCell(0, 1,
				constant.m_value.m_type == num::NaN ? constant.m_strValue : constant.m_value.m_strValue);
//		if(p_Valuesets.size() <= 1)
//		{
//			if(!constant->values_.empty())
//				pColListAttr->SetCell(0, 1, constant->values_[0]);
//		}
//		else
//		{
//			pColListAttr->Clear();
//			for(size_t i = 0; i < p_Valuesets.size(); ++i)
//			{
//				pColListAttr->AppendEmptyRow();
//				wxString vset = p_Valuesets[i];
//				pColListAttr->SetCell(i, 0, vset);
//				if(i < constant->values_.size())
//				{
//					wxString value = constant->values_[i];
//					pColListAttr->SetCell(i, 1, value);
//				}
//			}
//		}
		pColListAttr->UpdateActiveListColumnPtr();
	}
	/**
	 * Registering constants
	 * if this will not call than it's necessary to
	 * manually apply in constant tree (by clciking on OK btn)
	 */
	pFunctionRegistry = pDsGraph->GetFunctionRegistry();
	if(pFunctionRegistry) {
		pFunctionRegistry->LoadFromNet(pDsGraph);
	}

	return *this;
}
/**
 *
 */
DocBuilder& DocBuilder::appendFunctions(const std::vector<Function>& vFunctions) {
//	SP_DS_Metadataclass* pMetaDataClass = m_pcGraph->GetMetadataclass(SP_DS_META_FUNCTION);
//	if(!pMetaDataClass) {
//		//TODO throw
//		return *this;
//	}
//	SP_DS_Metadata* pMetadataFunction = NULL;
//	for(auto& function : functions) {
//			if(!function) continue;
//
//			wxString name = function->name_;
//			wxString param = function->param_;
//			wxString body = function->body_;
//			pMetadataFunction = pMetaDataClass->NewElement(1);
//			pMetadataFunction->GetAttribute("Name")->SetValueString(name);
//			pMetadataFunction->GetAttribute("Parameter")->SetValueString(param);
//			pMetadataFunction->GetAttribute("Body")->SetValueString(body);
//		}
//	}

	return *this;
}
/**
 *
 */
DocBuilder& DocBuilder::appendPlaces(const std::vector<Place>& vPlaces) {
	// Get doc
	SP_MDI_Doc& mdiDoc = getDoc();
	SP_DS_Graph* pDsGraph = mdiDoc.GetGraph();
	SP_MDI_View* pMdiView = dynamic_cast<SP_MDI_View*>(mdiDoc.GetFirstView());
	// Create places
	SP_DS_Nodeclass *pNodeClass = NULL;
	SP_DS_Node *pNode = NULL;
	//TODO Copy ErodeParsert & etc and start creating net
	/*
	 * For SP_GM_DocTemplate = SP_DS_HYBRIDPN_CLASS
	 * SP_DS_CONTINUOUS_PLACE & SP_DS_DISCRETE_PLACE
	 */
	pNodeClass = pDsGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
	if(!pNodeClass) {
		//TODO throw
		return *this;
	}
	//TODO catch NULL pointer exception
//	pNodeClass = pDsGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
	for(auto place : vPlaces) {
		pNode = pNodeClass->NewElement(pMdiView->GetCanvas()->GetNetnumber());
		if(!pNode) {
			//TODO throw
			return *this;
		}
		pNode->GetAttribute("Name")->SetValueString(place.m_strName);
		pNode->GetAttribute("Name")->SetShow(TRUE);
		pNode->GetAttribute("Marking")->SetValueString(place.m_strValue);
		pNode->ShowOnCanvas(pMdiView->GetCanvas(), FALSE, 10, 10, 0);
	
		// For edge Place <-> Transition
		m_mapPlaceNode[place.m_strName] = pNode;
	}
	return *this;
}
/**
 *
 */
DocBuilder& DocBuilder::appendTransitions(const std::vector<Transition>& vTransitions) {
	// Get doc
	SP_MDI_Doc& mdiDoc = getDoc();
	SP_DS_Graph* pDsGraph = mdiDoc.GetGraph();
	SP_MDI_View* pMdiView = dynamic_cast<SP_MDI_View*>(mdiDoc.GetFirstView());
	// Create transitions
	SP_DS_Nodeclass *pNodeClass = NULL;
	SP_DS_Node *pNode = NULL;
	std::string strName = "";
	//TODO SP_DS_STOCHASTIC_TRANS
	pNodeClass = pDsGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);

	for(auto transition : vTransitions) {
		pNode = pNodeClass->NewElement(pMdiView->GetCanvas()->GetNetnumber());
		if(!pNode) {
			//TODO throw
			return *this;
		}
		strName = transition.name();
		pNode->GetAttribute("Name")->SetValueString(strName);
		pNode->GetAttribute("Name")->SetShow(TRUE);
		// Set function
		SP_DS_ColListAttribute* pColListAttr = dynamic_cast<SP_DS_ColListAttribute*>(pNode->GetAttribute("FunctionList"));
		pColListAttr->SetCell(0, 1, transition.m_function.m_strName
									+ "("
									+ transition.m_function.m_strParam
									+ ")");
		// Show on
		pNode->ShowOnCanvas(pMdiView->GetCanvas(), FALSE, 20, 20, 0);
		// For edge Place <-> Transition
		m_mapTransitionNode[strName] = pNode;
		m_vTransition.push_back(transition);
	}

	return *this;
}
/**
 *
 */
void DocBuilder::connect() {
	// Get doc
	SP_MDI_Doc& mdiDoc = getDoc();
	SP_DS_Graph* pDsGraph = mdiDoc.GetGraph();
	SP_MDI_View* pMdiView = dynamic_cast<SP_MDI_View*>(mdiDoc.GetFirstView());
	//
	SP_DS_Node* pSourceNode = NULL;
	SP_DS_Node* pDestinationNode = NULL;
	SP_DS_Edgeclass* pEdgeClass = NULL;
	SP_DS_Edge *pEdge = NULL;
	//
	std::string strMultiplicity;
	int iMultiplicity = 0;
	//
	pEdgeClass = pDsGraph->GetEdgeclass(SP_DS_EDGE);
	for(auto transition : m_vTransition) {
		//
		std::map<std::string, SP_DS_Edge*> mapPrePlaceEdge;
		std::map<std::string, SP_DS_Edge*> mapPostPlaceEdge;
		//
		pDestinationNode = m_mapTransitionNode[transition.name()];
		for(auto strPrePlace : transition.m_setPrePlaces.m_vSet) {
			pSourceNode = m_mapPlaceNode[strPrePlace];

			try {
				/*
				 * If a place occur multiple times then change
				 * weight of an edge accordingly, else
				 * create new edge (see catch scope)
				 */
				pEdge = mapPrePlaceEdge.at(strPrePlace);
				strMultiplicity = pEdge->GetAttribute("Multiplicity")->GetValueString();
				try {
					iMultiplicity = std::stoi(strMultiplicity);
					iMultiplicity += 1;
					strMultiplicity = std::to_string(iMultiplicity);
				} catch(const std::invalid_argument& iaException) {
					strMultiplicity += "+1";
				}
				pEdge->GetAttribute("Multiplicity")->SetValueString(strMultiplicity);


			} catch(const std::out_of_range& oorExcpeption) {
				pEdge = pEdgeClass->GetPrototype()->Clone();
				pEdge->SetNetnumber(pMdiView->GetCanvas()->GetNetnumber());

				pEdge->SetNodes(pSourceNode, pDestinationNode);
				pEdge->SetNodes(*(pSourceNode->GetGraphics()->begin()), *(pDestinationNode->GetGraphics()->begin()));

				pEdge->GetAttribute("Multiplicity")->SetValueString("1");
				pEdgeClass->AddElement(pEdge);
				mapPrePlaceEdge[strPrePlace] = pEdge;
				// Show on
				pEdge->ShowOnCanvas(pMdiView->GetCanvas(), FALSE);
			}
		}
		pSourceNode = m_mapTransitionNode[transition.name()];
		for(auto strPostPlace : transition.m_setPostPlaces.m_vSet) {
			pDestinationNode = m_mapPlaceNode[strPostPlace];

			try {
				pEdge = mapPostPlaceEdge.at(strPostPlace);
				strMultiplicity = pEdge->GetAttribute("Multiplicity")->GetValueString();
				try {
					iMultiplicity = std::stoi(strMultiplicity);
					iMultiplicity += 1;
					strMultiplicity = std::to_string(iMultiplicity);
				} catch(const std::invalid_argument& iaException) {
					strMultiplicity += "+1";
				}
				pEdge->GetAttribute("Multiplicity")->SetValueString(strMultiplicity);


			} catch(const std::out_of_range& oorExcpeption) {
				pEdge = pEdgeClass->GetPrototype()->Clone();
				pEdge->SetNetnumber(pMdiView->GetCanvas()->GetNetnumber());

				pEdge->SetNodes(pSourceNode, pDestinationNode);
				pEdge->SetNodes(*(pSourceNode->GetGraphics()->begin()), *(pDestinationNode->GetGraphics()->begin()));

				pEdge->GetAttribute("Multiplicity")->SetValueString("1");
				pEdgeClass->AddElement(pEdge);
				mapPostPlaceEdge[strPostPlace] = pEdge;
				// Show on
				pEdge->ShowOnCanvas(pMdiView->GetCanvas(), FALSE);
			}
		}

	}
}
/**
 *
 */
SP_MDI_Doc& DocBuilder::build() {
	connect();
	SP_MDI_Doc& mdiDoc = getDoc();
	SP_DS_Graph* pDsGraph = mdiDoc.GetGraph();
	// Update tree
	pDsGraph->CreateDeclarationTree()->UpdateOtherTree();

	return mdiDoc;
}
/**
 *
 */
SP_MDI_Doc& DocBuilder::getDoc() {
	if(m_pMdiDoc) {
		return *m_pMdiDoc;
	}

	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
//	std::string netName = SP_DS_PN_CLASS;
//	std::string strNetName = SP_DS_CONTINUOUSPED_CLASS;
	std::string strNetName = SP_DS_HYBRIDPN_CLASS;

	SP_GM_DocTemplate* l_pcTemplate = l_pcDM->GetTemplate(strNetName);
	SP_DS_Netclass* newClass = l_pcTemplate->GetNetclass();

	m_pMdiDoc = dynamic_cast<SP_MDI_Doc*>(((wxDocTemplate*)l_pcTemplate)->CreateDocument("", 1));
	if (m_pMdiDoc) {
		m_pMdiDoc->OnNewDocument();
		m_pMdiDoc->InitGraph(newClass);
//		m_pDsGraph = pMdiDoc->GetGraph();
//		m_pMdiView = dynamic_cast<SP_MDI_View*>(pMdiDoc->GetFirstView());
//		m_pcCanvas = m_pMdiView->GetCanvas();
		m_pMdiDoc->DoSetModify(true, m_pMdiDoc);
	} else {
		//TODO throw
	}

	return *m_pMdiDoc;


}

} /* namespace erode */
} /* namespace import */
} /* namespace sp */
