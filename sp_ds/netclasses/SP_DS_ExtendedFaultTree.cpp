//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2004/11/12 11:55:00 $
// Short Description: Extended Fault Tree
//////////////////////////////////////////////////////////////////////
#include "sp_ds/netclasses/SP_DS_ExtendedFaultTree.h"
#include "sp_ds/netclasses/SP_DS_FaultTree.h"
#include "sp_ds/netclasses/SP_DS_SimpleGraph.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_ExtIdAttribute.h"
#include "sp_ds/attributes/SP_DS_LogicAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleListAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_FTreeNumberAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

#include "sp_ds/animators/SP_DS_FTreeEventAnimator.h"
#include "sp_ds/animators/SP_DS_FTreeGateAnimator.h"
#include "sp_ds/extensions/SP_DS_FTreePedAnimation.h"

#include "sp_gr/shapes/SP_GR_GateRectangle.h"
#include "sp_gr/shapes/SP_GR_ExtendedParameterCondition.h"
#include "sp_gr/shapes/SP_GR_Rhombus.h"
#include "sp_gr/shapes/SP_GR_Condition.h"

#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gr/attributes/SP_GR_NumberAttribute.h"
#include "sp_gr/attributes/SP_GR_CommentAttribute.h"
#include "sp_gr/attributes/SP_GR_DoubleAttribute.h"
#include "sp_gr/attributes/SP_GR_MarkAttribute.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogText.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogUnsigned.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogBool.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogShowOnly.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogList.h"

#include "sp_gr/edges/SP_GR_ArrowEdge.h"
#include "sp_gr/edges/SP_GR_FaultEdge.h"
#include "sp_core/SP_GPR_Animation.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_defines.h"
#include "sp_core/SP_GPR_Elements.h"
#include "snoopy.h"

SP_DS_ExtendedFaultTree::SP_DS_ExtendedFaultTree()
:SP_DS_FaultTree(SP_DS_EXTFAULTTREE_CLASS)
{
    m_sExtension = wxT("speft");
	m_sFilter = wxT("*.") + m_sExtension;
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Top Event"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Basic Event"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Intermediate Event"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Undeveloped Event"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Gates"));

}

SP_DS_ExtendedFaultTree::SP_DS_ExtendedFaultTree(const wxString& p_pchName)
:SP_DS_FaultTree(p_pchName)
{
    m_sExtension = wxT("speft");
	m_sFilter = wxT("*.") + m_sExtension;
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Top Event"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Basic Event"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Intermediate Event"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Undeveloped Event"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Gates"));
}

SP_DS_Graph*
SP_DS_ExtendedFaultTree::CreateGraph(SP_DS_Graph* p_pcGraph)
{
    if (!SP_DS_FaultTree::CreateGraph(p_pcGraph))
        return NULL;

    SP_DS_Attribute* l_pcAttr;
	SP_GR_Node* l_pcGr;
    SP_Graphic* l_pcGrAttr;
    SP_DS_Nodeclass* l_pcNS;

	//////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("XOR")));
	l_pcNS->SetShortcut(wxT("X"));
	int wdth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(),wxT("Gates"));
	int hght = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(),wxT("Gates"));
	l_pcNS->SetGraphic(new SP_GR_GateRectangle(l_pcNS->GetPrototype(),
														wxT("=1"), wdth, hght));
	l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(50);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NumberAttribute(wxT("Failure-Status"), 0));

	l_pcNS->AddAnimator(new SP_DS_FTreeGateAnimator());
	l_pcNS->AddAnimator(new SP_DS_FTreeEventAnimator(wxT("Failure-Status")));

	//////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("M-OF-N")));
	l_pcNS->SetShortcut(wxT("M"));
	wdth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(),wxT("Gates"));
	hght = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(),wxT("Gates"));

	l_pcNS->SetGraphic(new SP_GR_GateRectangle(l_pcNS->GetPrototype(),
														wxT(">=M"), wdth, hght));
	l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(50);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_FTreeNumberAttribute(wxT("N"), 1));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogUnsigned(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr, wxT("% >= ")));
    l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(FALSE);
    l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NumberAttribute(wxT("Failure-Status"), 0));


	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_FTreeNumberAttribute(wxT("M"), 1));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogUnsigned(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(40);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(FALSE);
    l_pcAttr->SetGlobalShow();

	l_pcNS->AddAnimator(new SP_DS_FTreeEventAnimator(wxT("Failure-Status")));
	l_pcNS->AddAnimator(new SP_DS_FTreeGateAnimator());

	//////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("COND")));
	l_pcNS->SetShortcut(wxT("R"));
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(8);
    l_pcGrAttr->SetShow(TRUE);
	wdth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(),wxT("Gates"));
	hght = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(),wxT("Gates"));
	l_pcGr = new SP_GR_Condition(l_pcNS->GetPrototype(), wdth, hght);
    l_pcNS->SetGraphic(l_pcGr);
    l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNS->GetName()));
    l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NumberAttribute(wxT("Failure-Status"), 0));

	l_pcNS->AddAnimator(new SP_DS_FTreeGateAnimator());
	l_pcNS->AddAnimator(new SP_DS_FTreeEventAnimator(wxT("Failure-Status")));

	//////////////////////////////////////////////////////////////////////////////
    l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("COND-Parameter")));
    l_pcNS->SetShortcut(wxT("Shift+P"));

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_ExtIdAttribute(wxT("ID")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(FALSE);
    l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NumberAttribute(wxT("Condition is complied"), 0));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));
    l_pcAttr->AddGraphic(new SP_GR_MarkAttribute(l_pcAttr));

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Description"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetY(40);

    l_pcNS->SetGraphic(new SP_GR_ExtendedParameterCondition(l_pcNS->GetPrototype(), 20.0));
    l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);
    l_pcAttr->SetGlobalShow();

	l_pcNS->AddAnimator(new SP_DS_FTreeEventAnimator(wxT("Condition is complied")));


    ///////////////////////////////////////////////////////////////////////////////
    l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Undeveloped Event")));
    l_pcNS->SetShortcut(wxT("U"));
    l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);
    l_pcAttr->SetGlobalShow();

    l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Description"),wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetY(40);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NumberAttribute(wxT("Failure-Status"), 0));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));
    l_pcAttr->AddGraphic(new SP_GR_MarkAttribute(l_pcAttr));

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_ExtIdAttribute(wxT("ID")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(FALSE);
    l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleListAttribute(wxT("Failure Rate"), wxT("0.0")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogList(wxT("Rates")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));

    l_pcGrAttr->SetOffsetY(30);
    l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleListAttribute(wxT("Repair Rate"), wxT("0.0")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogList(wxT("Rates")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));

    l_pcGrAttr->SetOffsetY(40);
    l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Probability of Failure (%)"), 0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));

    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("MTTF"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr->SetOffsetY(30);
    l_pcGrAttr->SetShow(FALSE);


	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("MTTR"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);
	l_pcGrAttr->SetOffsetY(30);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("MTBF"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);
	l_pcGrAttr->SetOffsetY(30);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Repairable System - Availability"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);
	l_pcGrAttr->SetOffsetY(30);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Repairable System - Unavailability"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcAttr->SetGlobalShow();
    l_pcGrAttr->SetOffsetY(30);
	l_pcGrAttr->SetShow(FALSE);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Irreparable System - Reliability"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);
	l_pcGrAttr->SetOffsetX(40);
	l_pcGrAttr->SetOffsetY(30);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Irreparable System - Unreliability"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);
	l_pcGrAttr->SetOffsetX(40);
	l_pcGrAttr->SetOffsetY(30);

	l_pcAttr->SetGlobalShow();
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(40);
    l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();

	int width = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(),wxT("Undeveloped Event"));
	int hight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(),wxT("Undeveloped Event"));

	l_pcGr = new SP_GR_Rhombus(l_pcNS->GetPrototype(), width, hight);
	l_pcNS->SetGraphic(l_pcGr);
    l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNS->GetName()));
    l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	l_pcNS->AddAnimator(new SP_DS_FTreeEventAnimator(wxT("Failure-Status")));
	////////////////////////////////////////////////////////////////////////////////////////////

	int	refresh = wxGetApp().GetAnimationPrefs()->GetRefresh(GetName());
	int duration = wxGetApp().GetAnimationPrefs()->GetDuration(GetName());
	SP_ANIM_STEP_T stepping = SP_ANIM_STEP_SINGLE;

    p_pcGraph->SetAnimation(new SP_DS_FTreePedAnimation(refresh, duration, stepping));

    return p_pcGraph;
}

bool
SP_DS_ExtendedFaultTree::NodeRequirement(SP_DS_Node* p_pcNode)
{
    return SP_DS_FaultTree::NodeRequirement(p_pcNode);
}

bool
SP_DS_ExtendedFaultTree::EdgeRequirement(SP_DS_Edgeclass* p_pcClass,
								 SP_Data* p_pcSrc,
								 SP_Data* p_pcTrg)
{
    if (!SP_DS_FaultTree::EdgeRequirement(p_pcClass, p_pcSrc, p_pcTrg))
        return FALSE;

	///////////////////////////////////////////////////////
    SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
	wxDocument* p_pcDoc = l_pcDM->GetCurrentDocument();
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(p_pcDoc);
	//looks for edges that are selected. If one edge is selected, the user
	// want to relocate the edge
	wxList l_lShapes;
    SP_MDI_View* l_pView = dynamic_cast<SP_MDI_View*>(l_pcDoc->GetFirstView());
    l_pView->FindSelectedShapes(l_lShapes);
	//////////////////////////////////////////////////////

    SP_DS_Node* l_pcSrc = dynamic_cast<SP_DS_Node*>(p_pcSrc);
    SP_DS_Node* l_pcTrg = dynamic_cast<SP_DS_Node*>(p_pcTrg);

    if (l_pcSrc->GetNodeclass() == l_pcTrg->GetNodeclass()) {
		if( l_pcSrc->GetClassName()==wxT("Undeveloped Event")) {
			SP_MESSAGEBOX(wxT("Can not be combined with another event!"),
		     wxT("Error"), wxOK | wxICON_ERROR);
			return FALSE;
		}
	}
	SP_ListEdge::const_iterator eIt;
	const SP_ListEdge* edgesSrc;
	edgesSrc = (dynamic_cast<SP_DS_Node*>(l_pcSrc))->GetSourceEdges();

	if (l_pcSrc->GetNodeclass() != l_pcTrg->GetNodeclass()) {


	   if (l_pcSrc->GetClassName()== wxT("Undeveloped Event"))
		 if (l_pcTrg->GetClassName()== wxT("Top Event")
			|| l_pcTrg->GetClassName()== wxT("Intermediate Event")
			|| l_pcTrg->GetClassName()== wxT("Basic Event")) {
			SP_MESSAGEBOX(wxT("Undevelopment Event can not be combined with another event"),
		    wxT("Error"), wxOK | wxICON_ERROR);
			return FALSE;
		 }

	   if (l_pcTrg->GetClassName()== wxT("Undeveloped Event") ) {
		 SP_MESSAGEBOX(wxT(" Undeveloped Events have only exit edges"),
		 wxT("Error"), wxOK | wxICON_ERROR);
		  return false;
	   }
	   if (l_pcTrg->GetClassName()== wxT("COND") ) {
			const SP_ListEdge* inEdges;

			inEdges = (dynamic_cast<SP_DS_Node*>(l_pcTrg))->GetTargetEdges();
			if (inEdges->size() > 0){
				for (eIt = inEdges->begin(); eIt != inEdges->end(); ++eIt) {
					SP_DS_Node* sourceNode = (SP_DS_Node*)(*eIt)->GetSource();
					if (sourceNode->GetClassName() !=  wxT("COND-Parameter")
						&& l_pcSrc->GetClassName() !=  wxT("COND-Parameter")){
						SP_MESSAGEBOX(wxT(" COND-Gates can be combined with only one event or gate."),
						wxT("Error"), wxOK | wxICON_ERROR);
						return false;
					}
				}
			}

	   }
	   if (l_pcTrg->GetClassName()== wxT("COND-Parameter") ) {
		 SP_MESSAGEBOX(wxT(" Parameters have only exit edges"),
		 wxT("Error"), wxOK | wxICON_ERROR);
		  return false;
	   }
	   if (l_pcSrc->GetClassName()== wxT("COND-Parameter") &&
		   l_pcTrg->GetClassName() !=  wxT("COND")) {
		 SP_MESSAGEBOX(wxT(" Parameters can only be combined with Conditions"),
		 wxT("Error"), wxOK | wxICON_ERROR);
		  return false;
	   }
	   if (l_pcTrg->GetClassName()== wxT("M-OF-N") && l_lShapes.GetCount()) {
		  long incomingEdges = 0;
		  long n;
		  const SP_ListEdge* inEdges;
		  inEdges = (dynamic_cast<SP_DS_Node*>(l_pcTrg))->GetTargetEdges();
		  for (eIt = inEdges->begin(); eIt != inEdges->end(); ++eIt) {
				incomingEdges++;
		  }
		  wxString nValue = l_pcTrg->GetAttribute(wxT("N"))->GetValueString();
		  nValue.ToLong(&n);
		  if (incomingEdges == n) {
			SP_MESSAGEBOX(wxT(" M-OF-N-Gates can have at most n incoming edges! Check the value of N."),
			wxT("Error"), wxOK | wxICON_ERROR);
			return false;
		  }
	   }

	}
    return TRUE;
}

