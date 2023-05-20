//////////////////////////////////////////////////////////////////////
// $Author: rr $
// $Version: 0.1 $
// $Date: 2008/01/07 $
// Short Description:Modulo Nets
//////////////////////////////////////////////////////////////////////

#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"

#include "sp_ds/netclasses/SP_DS_ModuloNets.h"


#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"

#include "sp_gr/edges/SP_GR_ArrowEdge.h"
#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gr/attributes/SP_GR_MarkNumberAttribute.h"
#include "sp_gr/attributes/SP_GR_MultiplicityAttribute.h"
#include "sp_gr/shapes/SP_GR_ExtendedEllipse.h"




#include "sp_gui/widgets/dialogs/SP_WDG_DialogUnsigned.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"

#include "sp_ds/animators/SP_DS_ModNetPlaceAnimator.h"

SP_DS_ModuloNets::SP_DS_ModuloNets()
:SP_DS_SimplePed(SP_DS_MODULOPN_CLASS)
{
	m_sExtension = wxT("mpn");
	m_sFilter = wxT("*.mpn;*.spmnet");
}

SP_DS_ModuloNets::SP_DS_ModuloNets(const wxString& p_pchName)
:SP_DS_SimplePed(p_pchName)
{
	m_sExtension = wxT("mpn");
	m_sFilter = wxT("*.mpn;*.spmnet");
}

SP_DS_Graph*
SP_DS_ModuloNets::CreateGraph(SP_DS_Graph* p_graph, SP_MapString2Int p_mgraphicsPosMap)
{
	if (!SP_DS_SimplePed::CreateGraph(p_graph))
	return NULL;

	SP_DS_Nodeclass* l_pcNodeClass;
	SP_DS_Edgeclass* l_pcEdgeClass;
	SP_DS_Attribute* l_pcAttr;
	SP_Graphic* l_pcGrAttr;
	SP_GR_Node* l_pcGrNode;
	SP_GR_ArrowEdge* l_pcGrEdge;
	//////////////////////////////////////////////////////////////////////////////
	l_pcNodeClass = p_graph->GetNodeclass(wxT("Place"));
	l_pcNodeClass->AddAnimator(new SP_DS_ModNetPlaceAnimator(wxT("Marking")));
	//////////////////////////////////////////////////////////////////////////////
	l_pcEdgeClass = p_graph->AddEdgeclass(new SP_DS_Edgeclass(p_graph, wxT("Undirected Edge")));

	l_pcGrEdge = new SP_GR_ArrowEdge(l_pcEdgeClass->GetPrototype(),0,0,0);

	l_pcGrEdge->AddAnotherArrow(0, 0, 0);
	l_pcEdgeClass->SetGraphic(l_pcGrEdge);

	l_pcEdgeClass->SetShortcut(wxT("U"));
	l_pcEdgeClass->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	l_pcAttr = l_pcEdgeClass->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"),wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(40);

	l_pcAttr = l_pcEdgeClass->AddAttribute(new SP_DS_MultiplicityAttribute(wxT("Multiplicity")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogUnsigned(wxT("General"), 1));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_MultiplicityAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(20);
	l_pcGrAttr->SetShow(TRUE);
	//////////////////////////////////////////////////////////////////////////////
	l_pcNodeClass = p_graph->AddNodeclass(new SP_DS_Nodeclass(p_graph, wxT("Modulo")));
	l_pcNodeClass->SetShortcut(wxT("m"));
	l_pcAttr = l_pcNodeClass->AddAttribute(new SP_DS_NumberAttribute(wxT("Modulo"), 2));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogUnsigned(wxT("General")));

	l_pcAttr->AddGraphic(new SP_GR_MarkNumberAttribute(l_pcAttr));
	l_pcAttr->SetGlobalShow();

/* change ckruege2 19.07.09 */
    l_pcGrNode = new SP_GR_ExtendedEllipse(l_pcNodeClass->GetPrototype(), 40,20,TRUE,SP_EXTENDED_TYPE_DEFAULT,1,wxColour(128, 128, 128));
/* change end */ 
    l_pcGrNode->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(128, 128, 128), 1 ));
	l_pcGrNode->SetFixedSize(FALSE);
	l_pcNodeClass->SetGraphic(l_pcGrNode);

	l_pcNodeClass->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
	///////////////////////////////////////////////////////////////////////////////

  return p_graph;
}// end CreateGraph


bool
SP_DS_ModuloNets::NodeRequirement(SP_DS_Node* p_pcNode)
{
	wxString l_sNodeClass = p_pcNode->GetClassName();
	if (l_sNodeClass.Cmp(wxT("Modulo")) == 0){
		SP_DS_Nodeclass* PClass = p_pcNode->GetNodeclass();
		const SP_ListNode* listElements = PClass->GetElements();
		SP_ListNode::const_iterator l_Iter;

		for (l_Iter = listElements->begin(); l_Iter != listElements->end(); ++l_Iter)
		{
				if(!(*l_Iter)->GetDelete())
				{
					SP_MESSAGEBOX(wxT("There is only one Modulo-Node!"),
								wxT("Error"), wxOK | wxICON_ERROR);
					return FALSE;
				}
		} //end for
	} //end if

	return SP_DS_SimplePed::NodeRequirement(p_pcNode);
}//end NodeRequirement

bool
SP_DS_ModuloNets::EdgeRequirement(SP_DS_Edgeclass* p_pcClass,
								  SP_Data* p_pcSrc,
								  SP_Data* p_pcTrg)
{
  	if (!SP_DS_SimplePed::EdgeRequirement(p_pcClass, p_pcSrc, p_pcTrg))
        return false;

	wxString l_sSrcNodeClass = p_pcSrc->GetClassName();
	wxString l_sTrgNodeClass = p_pcTrg->GetClassName();
	wxString l_sEdgeClass =  p_pcClass->GetName();

	if (l_sSrcNodeClass.Cmp(wxT("Modulo"))== 0)
        return FALSE;
	if (l_sTrgNodeClass.Cmp(wxT("Modulo"))== 0)
        return FALSE;


	if (l_sSrcNodeClass.Cmp(wxT("Place")) == 0)
	{

		SP_ListEdge::const_iterator eIt;
		const SP_ListEdge* l_pcEdges;

		l_pcEdges = (dynamic_cast<SP_DS_Node*>(p_pcSrc))->GetSourceEdges();

		for (eIt = l_pcEdges->begin(); eIt != l_pcEdges->end(); ++eIt)
		{
			if(!(*eIt)->GetDelete())
			{
				wxString l_sExistingEdgeClass=(*eIt)->GetClassName();
				if (l_sExistingEdgeClass.Cmp(l_sEdgeClass) != 0) {

					SP_MESSAGEBOX(wxT("Only one edge type at place allowed"),
							  wxT("Error"), wxOK | wxICON_ERROR);
					return false;
				} //end if
			} // end if
		} // end for

		l_pcEdges = (dynamic_cast<SP_DS_Node*>(p_pcSrc))->GetTargetEdges();
		for (eIt = l_pcEdges->begin(); eIt != l_pcEdges->end(); ++eIt)
		{
			if(!(*eIt)->GetDelete())
			{
				wxString l_sExistingEdgeClass=(*eIt)->GetClassName();
				if (l_sExistingEdgeClass.Cmp(l_sEdgeClass) != 0) {
					SP_MESSAGEBOX(wxT("Only one edge type at place allowed"),
						  wxT("Error"), wxOK | wxICON_ERROR);
					return false;
				} //end if
			} // end if
		} // end for

	} //end if

	if (l_sTrgNodeClass.Cmp(wxT("Place")) == 0)
	{

		SP_ListEdge::const_iterator eIt;
		const SP_ListEdge* l_pcEdges;

		l_pcEdges = (dynamic_cast<SP_DS_Node*>(p_pcTrg))->GetSourceEdges();

		for (eIt = l_pcEdges->begin(); eIt != l_pcEdges->end(); ++eIt)
		{
			if(!(*eIt)->GetDelete())
			{
				wxString l_sExistingEdgeClass=(*eIt)->GetClassName();
				if (l_sExistingEdgeClass.Cmp(l_sEdgeClass) != 0)
				{
					SP_MESSAGEBOX(wxT("Only one edge type at place allowed"),
						  wxT("Error"), wxOK | wxICON_ERROR);
					return false;
				} //end if
			} // end if
		} // end for
	} //end if

	if (l_sEdgeClass.Cmp(wxT("Undirected Edge"))==0){

		if(l_sSrcNodeClass.Cmp(wxT("Transition")) == 0 )
		{
			SP_Graphic* l_pcSrcGr = p_pcClass->GetStartShape();
			long l_nNetNumber = l_pcSrcGr->GetNetnumber();
			SP_Graphic* l_pcTrgGr = p_pcTrg->GetGraphicInSubnet(l_nNetNumber);
			SP_DS_Edge* l_pcNewEdge = p_pcClass->NewElement(l_nNetNumber, l_pcTrgGr, l_pcSrcGr);

			SP_GR_Edge *l_pcEdgeGr =
							dynamic_cast<SP_GR_Edge*>(l_pcNewEdge->AddGraphicInSubnet(l_nNetNumber));
			l_pcEdgeGr->SetSource(l_pcTrgGr);
			l_pcEdgeGr->SetTarget(l_pcSrcGr);

			l_pcNewEdge->ShowOnCanvas(dynamic_cast<SP_MDI_View*>(SP_Core::Instance()->GetRootDocument()->GetFirstView())->GetCanvas(), FALSE);

			return false;
		}
	}

	return true;
}// end EdgeRequirement
