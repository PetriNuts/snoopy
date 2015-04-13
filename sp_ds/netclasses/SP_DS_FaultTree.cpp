//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2004/11/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/netclasses/SP_DS_FaultTree.h"

#include "sp_ds/animators/SP_DS_FTreeEventAnimator.h"
#include "sp_ds/animators/SP_DS_FTreeGateAnimator.h"
#include "sp_ds/extensions/SP_DS_FTreePedAnimation.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_ExtTextAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleListAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_ExtIdAttribute.h"
#include "sp_ds/attributes/SP_DS_LogicAttribute.h"

#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gr/attributes/SP_GR_NumberAttribute.h"
#include "sp_gr/attributes/SP_GR_CommentAttribute.h"
#include "sp_gr/attributes/SP_GR_DoubleAttribute.h"
#include "sp_gr/attributes/SP_GR_MarkAttribute.h"

#include "sp_gr/shapes/SP_GR_Circle.h"

#include "sp_gr/shapes/SP_GR_ExtendedEllipse.h"
#include "sp_gr/shapes/SP_GR_Ellipse.h"
#include "sp_gr/shapes/SP_GR_EllipseNoMerge.h"
#include "sp_gr/shapes/SP_GR_ExtendedCircle.h"

#include "sp_gr/shapes/SP_GR_GateRectangle.h"
#include "sp_gr/shapes/SP_GR_Rectangle.h"
#include "sp_gr/shapes/SP_GR_RectangleNoMerge.h"


#include "sp_gr/edges/SP_GR_ArrowEdge.h"

#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "snoopy.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogText.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogUnsigned.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogBool.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogShowOnly.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogList.h"

#include "sp_gr/edges/SP_GR_FaultEdge.h"
#include "sp_core/SP_GPR_Animation.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_defines.h"
#include "sp_core/SP_GPR_Elements.h"
#include "snoopy.h"

SP_DS_FaultTree::SP_DS_FaultTree()
:SP_DS_SimpleGraph(SP_DS_FAULTTREE_CLASS)
{
    m_sExtension = wxT("spft");
	m_sFilter = wxT("*.") + m_sExtension;
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Top Event"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Basic Event"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Intermediate Event"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Gates"));

}

SP_DS_FaultTree::SP_DS_FaultTree(const wxString& p_pchName)
:SP_DS_SimpleGraph(p_pchName)
{
    m_sExtension = wxT("spft");
	m_sFilter = wxT("*.") + m_sExtension;
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Top Event"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Basic Event"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Intermediate Event"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(),wxT("Gates"));
}

SP_DS_Graph*
SP_DS_FaultTree::CreateGraph(SP_DS_Graph* p_pcGraph)
{
    if (!SP_DS_SimpleGraph::CreateGraph(p_pcGraph))
        return NULL;

    SP_DS_Attribute* l_pcAttr;
    SP_GR_Node* l_pcGr;
    SP_Graphic* l_pcGrAttr;
    SP_DS_Nodeclass* l_pcNS;
	SP_DS_Coarse* l_pcCoarse;


	///////////////////////////////////////////////////////////////////////////////
    l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Top Event")));
    l_pcNS->SetShortcut(wxT("T"));
    l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(8);
    l_pcGrAttr->SetShow(TRUE);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_ExtIdAttribute(wxT("ID")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);

    l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Description"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(40);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NumberAttribute(wxT("Failure-Status"), 0));
    //l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));
    l_pcAttr->AddGraphic(new SP_GR_MarkAttribute(l_pcAttr));

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Failure Rate"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));

    l_pcGrAttr->SetOffsetX(40);
	l_pcGrAttr->SetOffsetY(10);
    l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Repair Rate"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(40);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Probability of Failure (%)"), 0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(40);

    l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("MTTF"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);
	l_pcGrAttr->SetOffsetX(40);
	l_pcGrAttr->SetOffsetY(30);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("MTTR"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);
	l_pcGrAttr->SetOffsetX(40);
	l_pcGrAttr->SetOffsetY(30);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("MTBF"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);
	l_pcGrAttr->SetOffsetX(40);
	l_pcGrAttr->SetOffsetY(30);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Repairable System - Availability"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);
	l_pcGrAttr->SetOffsetX(40);
	l_pcGrAttr->SetOffsetY(30);



	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Repairable System - Unavailability"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr->SetOffsetX(40);
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

	int pradius = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), wxT("Top Event"));

/* change ckruege2 20.07.09 */
	l_pcGr = new SP_GR_ExtendedCircle(l_pcNS->GetPrototype(), pradius, SP_EXTENDED_TYPE_TWO, 1);
	//l_pcGr = new SP_GR_ExtDoubleCircle(l_pcNS->GetPrototype(), pradius);
/*change end */

    l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNS->GetName()));
    l_pcNS->SetGraphic(l_pcGr);
    l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	l_pcNS->AddAnimator(new SP_DS_FTreeEventAnimator(wxT("Failure-Status")));


    //////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Intermediate Event")));
    l_pcNS->SetShortcut(wxT("I"));
    l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(8);
    l_pcGrAttr->SetShow(TRUE);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_ExtIdAttribute(wxT("ID")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(FALSE);
    l_pcAttr->SetGlobalShow();

    l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Description"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(40);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NumberAttribute(wxT("Failure-Status"), 0));
    //l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));
    l_pcAttr->AddGraphic(new SP_GR_MarkAttribute(l_pcAttr));

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Failure Rate"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(40);
    l_pcGrAttr->SetOffsetY(10);
    l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Repair Rate"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(40);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Probability of Failure (%)"), 0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(40);

    l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("MTTF"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);
	l_pcGrAttr->SetOffsetX(40);
	l_pcGrAttr->SetOffsetY(30);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("MTTR"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);
	l_pcGrAttr->SetOffsetX(40);
	l_pcGrAttr->SetOffsetY(30);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("MTBF"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);
	l_pcGrAttr->SetOffsetX(40);
	l_pcGrAttr->SetOffsetY(30);

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

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Repairable System - Availability"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);
	l_pcGrAttr->SetOffsetX(40);
	l_pcGrAttr->SetOffsetY(30);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Repairable System - Unavailability"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr->SetOffsetX(40);
	l_pcGrAttr->SetOffsetY(30);
	l_pcGrAttr->SetShow(FALSE);

	int x = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), wxT("Intermediate Event"));
	if (x <= 0) x = 100;
	int y = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), wxT("Intermediate Event"));
	if (y <= 0) y = 30;

	l_pcGr = new SP_GR_RectangleNoMerge(l_pcNS->GetPrototype(), x, y, y/2);
    l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNS->GetName()));
    l_pcNS->SetGraphic(l_pcGr);

    l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
	l_pcNS->AddAnimator(new SP_DS_FTreeEventAnimator(wxT("Failure-Status")));

    ////////////////////////////////////////////////////////////////////////////////

	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph,
							 wxT("Coarse Event")));
    l_pcNS->SetShortcut(wxT("Shift+C"));
    l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NameAttribute(wxT("Name")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);
    l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(40);
	l_pcGrAttr->SetShow(TRUE);


	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_ExtIdAttribute(wxT("ID")));

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NumberAttribute(wxT("Failure-Status"), 0));
    //l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));
    l_pcAttr->AddGraphic(new SP_GR_MarkAttribute(l_pcAttr));

    // subnet property for this class
    l_pcCoarse = new SP_DS_Coarse(wxT("Subnet"), wxT("Intermediate Event"), p_pcGraph, FALSE);
    l_pcCoarse->SetLabelAttribute(wxT("Name"));
    l_pcNS->SetCoarse(l_pcCoarse);
/* change Ckruege2 20.07.09 */
	l_pcGr = new SP_GR_ExtendedEllipse(l_pcNS->GetPrototype(), 40,20,TRUE, SP_EXTENDED_TYPE_TWO,1 );
    //l_pcGr = new SP_GR_DoubleEllipse(l_pcNS->GetPrototype(), 40.0);
/*change end */
    l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNS->GetName()));
    l_pcNS->SetGraphic(l_pcGr);
    l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));


	////////////////////////////////////////////////////////////////////////////////

    l_pcNS = p_pcGraph->RenameNodeclass(wxT("Node"), wxT("Basic Event"));
    l_pcNS->SetShortcut(wxT("B"));
    //l_pcAttr = l_pcNS->GetPrototype()->GetAttribute(wxT("Name"));
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_ExtTextAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(8);
    l_pcGrAttr->SetShow(TRUE);

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

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Description"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetY(40);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));


	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleListAttribute(wxT("Failure Rate"), wxT("0.0")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogList(wxT("Rates")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetY(30);
    l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleListAttribute(wxT("Repair Rate"), wxT("0.0")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogList(wxT("Rates")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(0);
    l_pcGrAttr->SetOffsetY(40);
    l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Probability of Failure (%)"), 0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(0);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(FALSE);
	l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("MTTF"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);
	l_pcGrAttr->SetOffsetX(0);
    l_pcGrAttr->SetOffsetY(40);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("MTTR"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);
	l_pcGrAttr->SetOffsetX(0);
    l_pcGrAttr->SetOffsetY(40);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("MTBF"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);
	l_pcGrAttr->SetOffsetY(40);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Repairable System - Availability"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(FALSE);
	l_pcGrAttr->SetOffsetY(40);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_DoubleAttribute(wxT("Repairable System - Unavailability"), 0.0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("Parameter")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_DoubleAttribute(l_pcAttr));
	l_pcAttr->SetGlobalShow();
	//l_pcGrAttr->SetOffsetX(10);
    l_pcGrAttr->SetOffsetY(40);
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

	int width = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(),wxT("Basic Event"));
	int height = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(),wxT("Basic Event"));

	l_pcGr = new SP_GR_EllipseNoMerge(l_pcNS->GetPrototype(), width, height);
    l_pcNS->SetGraphic(l_pcGr);
    l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNS->GetName()));
    l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	l_pcNS->AddAnimator(new SP_DS_FTreeEventAnimator(wxT("Failure-Status")));

	//////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("AND")));
	l_pcNS->SetShortcut(wxT("A"));
	int wdth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(),wxT("Gates"));
	int hght = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(),wxT("Gates"));
	l_pcNS->SetGraphic(new SP_GR_GateRectangle(l_pcNS->GetPrototype(),
													wxT("&"), wdth, hght));
	l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(50);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NumberAttribute(wxT("Failure-Status"), 0));
    //l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));
  // l_pcAttr->AddGraphic(new SP_GR_MarkAttribute(l_pcAttr));

	l_pcNS->AddAnimator(new SP_DS_FTreeGateAnimator());
	l_pcNS->AddAnimator(new SP_DS_FTreeEventAnimator(wxT("Failure-Status")));


	//////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("OR")));
	l_pcNS->SetShortcut(wxT("O"));
	wdth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(),wxT("Gates"));
	hght = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(),wxT("Gates"));
	l_pcNS->SetGraphic(new SP_GR_GateRectangle(l_pcNS->GetPrototype(),
													wxT(">=1"), wdth, hght));
	l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(50);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NumberAttribute(wxT("Failure-Status"), 0));
	//l_pcAttr->AddGraphic(new SP_GR_MarkAttribute(l_pcAttr));

	l_pcNS->AddAnimator(new SP_DS_FTreeGateAnimator());
	l_pcNS->AddAnimator(new SP_DS_FTreeEventAnimator(wxT("Failure-Status")));

	//////////////////////////////////////////////////////////////////////////////
	l_pcNS = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("NEG")));
	l_pcNS->SetShortcut(wxT("N"));
	wdth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(),wxT("Gates"));
	hght = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(),wxT("Gates"));
	l_pcNS->SetGraphic(new SP_GR_GateRectangle(l_pcNS->GetPrototype(),
														wxT("1"), wdth, hght));
	l_pcNS->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(50);
	//SetGraphicState(SP_STATE_NEG);

	l_pcAttr = l_pcNS->AddAttribute(new SP_DS_NumberAttribute(wxT("Failure-Status"), 0));
	l_pcNS->AddAnimator(new SP_DS_FTreeEventAnimator(wxT("Failure-Status")));

	l_pcNS->AddAnimator(new SP_DS_FTreeGateAnimator());


    ///////////////////////////////////////////////////////////////////////////////
    SP_DS_Edgeclass* l_pcEC = p_pcGraph->GetEdgeclass(wxT("Edge"));
	l_pcEC->SetGraphic(new SP_GR_FaultEdge(l_pcEC->GetPrototype()));

    l_pcEC->SetShortcut(wxT("E"));
    l_pcAttr = l_pcEC->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(8);
    l_pcGrAttr->SetShow(TRUE);

    l_pcAttr = l_pcEC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"),wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr  = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr ->SetOffsetX(40);
	l_pcEC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
	l_pcAttr->SetGlobalShow();

	int	refresh = wxGetApp().GetAnimationPrefs()->GetRefresh(GetName());
		int duration = wxGetApp().GetAnimationPrefs()->GetDuration(GetName());
		SP_ANIM_STEP_T stepping = SP_ANIM_STEP_SINGLE;

    p_pcGraph->SetAnimation(new SP_DS_FTreePedAnimation(refresh, duration, stepping));

    return p_pcGraph;
}

bool
SP_DS_FaultTree::NodeRequirement(SP_DS_Node* p_pcNode)
{
    if ( p_pcNode->GetClassName()== wxT("Top Event")){
		SP_DS_Nodeclass* topEventClass = p_pcNode->GetNodeclass();
		const SP_ListNode* listElements = topEventClass->GetElements();
		SP_ListNode::const_iterator l_Iter;

		for (l_Iter = listElements->begin(); l_Iter != listElements->end(); ++l_Iter){

			SP_ListGraphic* l_lsGraphic = (*l_Iter)->GetGraphics();
			SP_ListGraphic::iterator sIt;
				 sIt = l_lsGraphic->begin();
				 //GetCanvas (like in Edgerequirement) doesnt work here in connection with undo
				 //so we use Getshow to recognize if there is another topevent
				 if (  (*sIt)->GetShow() ){
		 			   	SP_MESSAGEBOX(wxT("There is only one Top Event!"),
						wxT("Error"), wxOK | wxICON_ERROR);
					    return FALSE;
					}

		} //end for
	}

	return TRUE;
}

bool
SP_DS_FaultTree::EdgeRequirement(SP_DS_Edgeclass* p_pcClass,
								 SP_Data* p_pcSrc,
								 SP_Data* p_pcTrg)
{

	//TODO:
	//after solving a bug when deleting a edge (this edge was not deleted in the corresponding nodes )
	// this method should be refactored.

	if (!SP_DS_Netclass::EdgeRequirement(p_pcClass, p_pcSrc, p_pcTrg))
        return FALSE;

    if (p_pcSrc->GetElementType() != SP_ELEMENT_NODE ||
        p_pcTrg->GetElementType() != SP_ELEMENT_NODE)
        return FALSE;

	if (p_pcSrc->GetClassName()== wxT("Comment"))
        return FALSE;
    if (p_pcTrg->GetClassName()== wxT("Comment"))
        return FALSE;


	//////////////////////////////////////////////////////
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

	if (l_pcSrc->GetCoarse() && l_pcTrg->GetCoarse())
        return FALSE;
    if (l_pcSrc->GetCoarse())
		if (l_pcSrc->GetNodeclass()->GetName()== wxT("Coarse Event")){
			if (l_pcSrc->GetCoarse()->GetInnerClass()== l_pcTrg->GetNodeclass()->GetName()
				|| l_pcTrg->GetNodeclass()->GetName()== wxT("Basic Event")
				|| l_pcTrg->GetNodeclass()->GetName()== wxT("Top Event")
				|| l_pcTrg->GetNodeclass()->GetName()== wxT("Undeveloped Event") ) {
				SP_MESSAGEBOX(wxT("can not be combined with another event"),
				 wxT("Error"), wxOK | wxICON_ERROR);
				return FALSE;
			}
		}

    if (l_pcTrg->GetCoarse())
		if (l_pcTrg->GetNodeclass()->GetName()== wxT("Coarse Event")) {
			if (l_pcTrg->GetCoarse()->GetInnerClass()== l_pcSrc->GetNodeclass()->GetName() ||
				l_pcSrc->GetNodeclass()->GetName()== wxT("Basic Event")  ||
				l_pcSrc->GetNodeclass()->GetName()== wxT("Top Event")  ||
				l_pcSrc->GetNodeclass()->GetName()== wxT("Undeveloped Event") ) {
				SP_MESSAGEBOX(wxT("can not be combined with another event"),
				 wxT("Error"), wxOK | wxICON_ERROR);
				return FALSE;
			}
		}
	if (l_pcSrc == l_pcTrg)
		return FALSE;

    if (l_pcSrc->GetNodeclass() == l_pcTrg->GetNodeclass()) {
		if (l_pcSrc->GetClassName()==wxT("Top Event")
			|| l_pcSrc->GetClassName()==wxT("Basic Event")
			|| l_pcSrc->GetClassName()==wxT("Intermediate Event")) {
			SP_MESSAGEBOX(wxT("can not be combined with another event"),
		     wxT("Error"), wxOK | wxICON_ERROR);
			return FALSE;
		}
	}
	SP_ListEdge::const_iterator eIt;
	const SP_ListEdge* edgesSrc;
	edgesSrc = (dynamic_cast<SP_DS_Node*>(l_pcSrc))->GetSourceEdges();

	if (l_pcSrc->GetNodeclass() != l_pcTrg->GetNodeclass()) {
	   if (l_pcSrc->GetClassName()== wxT("Top Event") && l_lShapes.GetCount()) {
	 	 SP_MESSAGEBOX(wxT("Top Event has only one incoming edge"),
		 wxT("Error"), wxOK | wxICON_ERROR);
		 return FALSE;
	   }
	   if (l_pcSrc->GetClassName()== wxT("Intermediate Event"))
		 if (l_pcTrg->GetClassName()== wxT("Basic Event")
			||l_pcTrg->GetClassName()== wxT("Top Event")) {
			 SP_MESSAGEBOX(wxT("Intermediate Event can not be combined")
			wxT(" with another event"),wxT("Error"), wxOK | wxICON_ERROR);
			 return FALSE;
		 }
	   if (l_pcSrc->GetClassName()== wxT("Basic Event"))
		 if (l_pcTrg->GetClassName()== wxT("Top Event")
			|| l_pcTrg->GetClassName()== wxT("Intermediate Event")) {
			SP_MESSAGEBOX(wxT("Basic Event can not be combined with another event"),
		    wxT("Error"), wxOK | wxICON_ERROR);
			return FALSE;
		 }
	   if (l_pcTrg->GetClassName()== wxT("Basic Event")) {
		 SP_MESSAGEBOX(wxT(" Basic Events have only exit edges"),
		 wxT("Error"), wxOK | wxICON_ERROR);
		  return false;
	   }
	   if (l_pcSrc->GetClassName()== wxT("NEG")) {

		int outgoingEdges =0;
		for (eIt = edgesSrc->begin(); eIt != edgesSrc->end(); ++eIt) {
			if ((*eIt)->GetSource() == l_pcSrc  ) {


			  SP_ListGraphic* sGraphic = (*eIt)->GetGraphics();
			  SP_ListGraphic::iterator sIt;
	          sIt = sGraphic->begin();

				if (  NULL != (*sIt)->GetCanvas() )
				  	outgoingEdges++;
			}//end if
		}
		if (outgoingEdges != 0){
			SP_MESSAGEBOX(wxT("There is only one exit edge!"),
			wxT("Error"), wxOK | wxICON_ERROR);
			return FALSE;
		}


		}
	   if(l_pcTrg->GetClassName()== wxT("Top Event")
		 || l_pcTrg->GetClassName()== wxT("Intermediate Event")
		 || l_pcTrg->GetClassName()== wxT("NEG")
		 || l_pcTrg->GetClassName()== wxT("Coarse Intermediate Event") ) {

		 const SP_ListEdge* edges;
		 edges = (dynamic_cast<SP_DS_Node*>(l_pcTrg))->GetSourceEdges();
		 for (eIt = edges->begin(); eIt != edges->end(); ++eIt) {
			if ((*eIt)->GetSource() == l_pcTrg
				&& (*eIt)->GetTarget() == l_pcSrc){

				 SP_ListGraphic* sGraphic = (*eIt)->GetGraphics();
				 SP_ListGraphic::iterator sIt;
				 sIt = sGraphic->begin();
					if (  NULL != (*sIt)->GetCanvas() ){
						SP_MESSAGEBOX(wxT(" There is already an egde between Gate/Event!"),
						wxT("Error"), wxOK | wxICON_ERROR);
						return false;
					}
			}
		 }
		 edges = (dynamic_cast<SP_DS_Node*>(l_pcTrg))->GetTargetEdges();
		 for (eIt = edges->begin(); eIt != edges->end(); ++eIt) {
			if ((*eIt)->GetTarget() == l_pcTrg && l_lShapes.GetCount() == 0) {

				 SP_ListGraphic* sGraphic = (*eIt)->GetGraphics();
				 SP_ListGraphic::iterator sIt;
				 sIt = sGraphic->begin();

				 if (  NULL != (*sIt)->GetCanvas() ){
					SP_MESSAGEBOX(wxT(" There is only one incoming edge!"),
					wxT("Error"), wxOK | wxICON_ERROR);
					return false;
				 }
			}
		 }
	   }
	}
	if( l_pcSrc->GetClassName() !=  wxT("Top Event")
		&& !l_pcSrc->GetLogical() && l_lShapes.GetCount() == 0) {
		int outgoingEdges =0;
		for (eIt = edgesSrc->begin(); eIt != edgesSrc->end(); ++eIt) {
			if ((*eIt)->GetSource() == l_pcSrc  ) {


			  SP_ListGraphic* sGraphic = (*eIt)->GetGraphics();
			  SP_ListGraphic::iterator sIt;
	          sIt = sGraphic->begin();

				if (  NULL != (*sIt)->GetCanvas() ) outgoingEdges++;
			}//end if
		}
		if (outgoingEdges != 0){
			SP_MESSAGEBOX(wxT("There is only one exit edge!"),
			wxT("Error"), wxOK | wxICON_ERROR);
			return FALSE;
		}
	}

	//specific characteristic of NEG-Gates: Edges between NEG-Gate and another gate/event
	//have the design of inhibitor-edges.
	if (l_pcSrc->GetClassName()== wxT("NEG") ) {
		SP_ListGraphic* l_pcGraphic = l_pcSrc->GetGraphics();
		SP_ListGraphic::iterator l_Iter;

		if (l_pcGraphic) {
			l_Iter = l_pcGraphic->begin();
			(*l_Iter)->SetGraphicState(SP_STATE_NEG);
		}
	}
	if (l_pcTrg->GetClassName()== wxT("NEG") ) {
		SP_ListGraphic* l_pcGraphic = l_pcTrg->GetGraphics();
		SP_ListGraphic::iterator l_Iter;
		if (l_pcGraphic){
			l_Iter = l_pcGraphic->begin();
			(*l_Iter)->SetGraphicState(SP_STATE_NORMAL);
		}
	}

    return TRUE;
}

