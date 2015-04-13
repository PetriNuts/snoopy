//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/netclasses/SP_DS_SimplePed.h"
#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"

#include "sp_ds/SP_DS_Metadataclass.h"

#include "sp_ds/animators/SP_DS_PlaceAnimator.h"
#include "sp_ds/animators/SP_DS_TransAnimator.h"
#include "sp_ds/animators/SP_DS_CoarseTransAnimator.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_LogicAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
//by sl
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingDependentMultiplicity.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"

#include "sp_ds/attributes/SP_DS_EquationAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_ds/extensions/SP_DS_PedAnimation.h"

#include "sp_gr/shapes/SP_GR_Ellipse.h"
#include "sp_gr/shapes/SP_GR_Circle.h"
#include "sp_gr/shapes/SP_GR_Rectangle.h"
#include "sp_gr/shapes/SP_GR_ExtendedCircle.h"
#include "sp_gr/shapes/SP_GR_ExtendedRectangle.h"
#include "sp_gr/metadata/SP_GR_MetaComposite.h"

#include "sp_gr/edges/SP_GR_ArrowEdge.h"
#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gr/attributes/SP_GR_NumberAttribute.h"
#include "sp_gr/attributes/SP_GR_MultiplicityAttribute.h"
#include "sp_gr/attributes/SP_GR_MarkAttribute.h"
#include "sp_gr/attributes/SP_GR_CommentAttribute.h"
#include "sp_gr/attributes/SP_GR_HiddenTextAttribute.h"
#include "sp_gr/attributes/SP_GR_ColListAttribute.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogText.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogUnsigned.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogBool.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogShowOnly.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogColList.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogExtendedGraphic.h"

#include "sp_defines.h"
#include "snoopy.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_core/SP_GPR_Animation.h"


SP_DS_SimplePed::SP_DS_SimplePed()
:SP_DS_BipartGraph(SP_DS_PN_CLASS)
{
	m_sExtension = wxT("pn");
	m_sFilter = wxT("*.pn;*.spped");

	//register node classes for editing their graphic properties
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Transition"));

}

SP_DS_SimplePed::SP_DS_SimplePed(const wxString& p_pchName)
:SP_DS_BipartGraph(p_pchName)
{
	m_sExtension = wxT("pn");
	m_sFilter = wxT("*.pn;*.spped");

	//register node classes for editing their graphic properties
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Place"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Transition"));
}

SP_DS_Graph*
SP_DS_SimplePed::CreateGraph(SP_DS_Graph* p_pcGraph)
{
		if (!SP_DS_BipartGraph::CreateGraph(p_pcGraph))
        return NULL;

    SP_DS_Attribute* l_pcAttr;
    SP_DS_Coarse* l_pcCoarse;
    SP_GR_Node* l_pcGr;
    SP_Graphic* l_pcGrAttr;
    SP_DS_Nodeclass* l_pcNC;
    SP_GR_Metadata* l_pcGrMeta;

    //////////////////////////////////////////////////////////////////////////////
    l_pcNC = p_pcGraph->RenameNodeclass(wxT("Circle"), wxT("Place"));
    l_pcNC->SetShortcut(wxT("P"));

    l_pcAttr = l_pcNC->GetPrototype()->GetAttribute(wxT("Name"));
    l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_IdAttribute(wxT("ID")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr, wxT("_p%_")));
    l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(FALSE);
    l_pcAttr->SetGlobalShow();


    l_pcAttr = l_pcNC->AddAttribute(new SP_DS_MarkingAttribute(wxT("Marking"), 0));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcAttr->AddGraphic(new SP_GR_MarkAttribute(l_pcAttr));


    l_pcAttr = l_pcNC->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));

    l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetY(40);

		int pwidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), wxT("Place"));
		int pheight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), wxT("Place"));

	l_pcGr = new SP_GR_Ellipse(l_pcNC->GetPrototype(), pwidth, pheight);
    l_pcNC->SetGraphic(l_pcGr);
    l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
    l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

    // animator
    l_pcNC->AddAnimator(new SP_DS_PlaceAnimator(wxT("Marking")));

    //////////////////////////////////////////////////////////////////////////////
    l_pcNC = p_pcGraph->RenameNodeclass(wxT("Rectangle"), wxT("Transition"));
    l_pcNC->SetShortcut(wxT("T"));

    l_pcAttr = l_pcNC->GetPrototype()->GetAttribute(wxT("Name"));
    l_pcAttr->SetGlobalShow();

    l_pcAttr = l_pcNC->AddAttribute(new SP_DS_IdAttribute(wxT("ID")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr, wxT("_t%_")));
    l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(FALSE);
    l_pcAttr->SetGlobalShow();

    l_pcAttr = l_pcNC->AddAttribute(new SP_DS_LogicAttribute(wxT("Logic"), wxT("Name")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));

    l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(40);

		int twidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), wxT("Transition"));
		int theight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), wxT("Transition"));

    l_pcGr = new SP_GR_Rectangle(l_pcNC->GetPrototype(), twidth, theight);
    l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
    l_pcNC->SetGraphic(l_pcGr);
    l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

    // animator
    l_pcNC->AddAnimator(new SP_DS_TransAnimator());

    ///////////////////////////////////////////////////////////////////////////////
    l_pcNC = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Coarse Place")));
    l_pcNC->SetShortcut(wxT("Shift+P"));

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);
    l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(40);
	l_pcGrAttr->SetShow(TRUE);


    // subnet property for this class
    l_pcCoarse = new SP_DS_Coarse(wxT("Subnet"), wxT("Place"), p_pcGraph, FALSE);
    l_pcCoarse->SetLabelAttribute(wxT("Name"));
    l_pcNC->SetCoarse(l_pcCoarse);

    l_pcGr = new SP_GR_ExtendedCircle(l_pcNC->GetPrototype(), pwidth, SP_EXTENDED_TYPE_TWO, 1);
    l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), wxT("Place")));
    l_pcNC->SetGraphic(l_pcGr);
    l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

    ///////////////////////////////////////////////////////////////////////////////
    l_pcNC = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Coarse Transition")));
    l_pcNC->SetShortcut(wxT("Shift+T"));

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute(wxT("Name")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);
    l_pcAttr->SetGlobalShow();

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(40);
	l_pcGrAttr->SetShow(TRUE);


    // subnet property for this class
    l_pcCoarse = new SP_DS_Coarse(wxT("Subnet"), wxT("Transition"), p_pcGraph, FALSE);
    l_pcCoarse->SetLabelAttribute(wxT("Name"));
    l_pcNC->SetCoarse(l_pcCoarse);

    l_pcGr = new SP_GR_ExtendedRectangle(l_pcNC->GetPrototype(),twidth , theight,TRUE, SP_EXTENDED_TYPE_TWO,1);
    l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), wxT("Transition")));
    l_pcNC->SetGraphic(l_pcGr);
    l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

    // animator
    l_pcNC->AddAnimator(new SP_DS_CoarseTransAnimator());

    ///////////////////////////////////////////////////////////////////////////////
    SP_DS_Edgeclass* l_pcEC = p_pcGraph->GetEdgeclass(wxT("Edge"));
    l_pcEC->SetShortcut(wxT("E"));
    l_pcAttr = l_pcEC->GetPrototype()->GetAttribute(wxT("Name"));
    l_pcAttr->SetGlobalShow();

    //special wish from mh: edges have no names, but this conflicts with
    //the compatibility of the nets we have
    if(l_pcAttr->Remove())
    {
    	wxDELETE(l_pcAttr);
    }

    l_pcAttr = l_pcEC->AddAttribute(new SP_DS_MultiplicityAttribute(wxT("Multiplicity"), wxT("1")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_MultiplicityAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(20);
    l_pcGrAttr->SetShow(TRUE);
    l_pcAttr->SetGlobalShow();

    l_pcAttr = l_pcEC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"),wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(40);


    l_pcEC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));



    /////////////////////////////////////////////////////////////////////////////////////
    SP_DS_Metadataclass* l_pcMC;

    /////////////////////////////////////////////////////////////////////////////////////
	l_pcMC = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, SP_DS_META_CONSTANT ) );

	l_pcMC->SetDisplayName(wxT("Constants"));

	l_pcMC->SetShowInElementTree( false );
	l_pcMC->SetShowInDeclarationTreeOther( true );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_IdAttribute( wxT("ID") ) );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Group") ) );
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr, wxT("%:")));
	l_pcGrAttr->SetShow(false);

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TypeAttribute( wxT("Type"), wxT("int") ) );
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(false);

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NameAttribute( wxT("Name") ) );
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetShow(false);

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_ColListAttribute( wxT("ValueList"), SP_COLLIST_STRING, 2 ,wxT("Group")) );
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogColList(wxT("Values")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ColListAttribute( l_pcAttr, wxT("=  [ % ];") ));
	l_pcGrAttr->SetShow(false);
	SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcAttr );
	l_pcColList->SetColLabel( 0, wxT("Value set") );
	l_pcColList->SetColLabel( 1, wxT("Value") );
	unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(l_nNewRow, 0, wxT("Main"));
	l_pcColList->SetCell(l_nNewRow, 1, wxT(""));

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Comment") ) );
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr, wxT("//%")));
	l_pcGrAttr->SetShow(false);

	l_pcGrMeta = new SP_GR_MetaComposite(l_pcMC->GetPrototype());
    l_pcGrMeta->SetFixedSize(true);
    l_pcGrMeta->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(255,255,255), 1, wxSOLID ));
    l_pcGrMeta->SetShow(false);
    l_pcMC->SetGraphic(l_pcGrMeta);
	l_pcMC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

    /////////////////////////////////////////////////////////////////////////////////////
	l_pcMC = p_pcGraph->AddMetadataclass( new SP_DS_Metadataclass( p_pcGraph, SP_DS_META_FUNCTION ) );

	l_pcMC->SetDisplayName(wxT("Functions"));

	l_pcMC->SetShowInElementTree( false );
	l_pcMC->SetShowInDeclarationTreeOther( true );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_NameAttribute( wxT("Name") ) );
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr, wxT("%")));
	l_pcGrAttr->SetShow(false);
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_IdAttribute( wxT("ID") ) );
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Parameter") ) );
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr, wxT("(%)")));
	l_pcGrAttr->SetShow(false);
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_EquationAttribute( wxT("Body") ) );
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr, wxT("=  %;")));
	l_pcGrAttr->SetShow(false);
	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TypeAttribute( wxT("Return"), wxT("int") ) );

	l_pcAttr = l_pcMC->AddAttribute( new SP_DS_TextAttribute( wxT("Comment") ) );
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr, wxT("//%")));
	l_pcGrAttr->SetShow(false);

	l_pcGrMeta = new SP_GR_MetaComposite(l_pcMC->GetPrototype());
    l_pcGrMeta->SetFixedSize(true);
    l_pcGrMeta->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(255,255,255), 1, wxSOLID ));
    l_pcGrMeta->SetShow(false);
    l_pcMC->SetGraphic(l_pcGrMeta);
	l_pcMC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	//////////////////////////////////////////////////////////////////////////
		int	refresh = wxGetApp().GetAnimationPrefs()->GetRefresh(GetName());
		int duration = wxGetApp().GetAnimationPrefs()->GetDuration(GetName());
		SP_ANIM_STEP_T stepping = wxGetApp().GetAnimationPrefs()->GetStepping(GetName());

	    p_pcGraph->SetAnimation(new SP_DS_PedAnimation(refresh, duration, stepping));


	return p_pcGraph;
}

bool
SP_DS_SimplePed::NodeRequirement(SP_DS_Node* p_pcNode)
{
    return SP_DS_BipartGraph::NodeRequirement(p_pcNode);
}

bool
SP_DS_SimplePed::EdgeRequirement(SP_DS_Edgeclass* p_pcClass, SP_Data* p_pcSrc, SP_Data* p_pcTrg)
{
    if (!SP_DS_Netclass::EdgeRequirement(p_pcClass, p_pcSrc, p_pcTrg))
        return FALSE;

    if (p_pcSrc->GetElementType() != SP_ELEMENT_NODE ||
        p_pcTrg->GetElementType() != SP_ELEMENT_NODE)
        return FALSE;

    if (p_pcSrc->GetClassName()== wxT("Comment"))
        return FALSE;
    if (p_pcTrg->GetClassName()== wxT("Comment"))
        return FALSE;

    SP_DS_Node* l_pcSrc = dynamic_cast<SP_DS_Node*>(p_pcSrc);
    SP_DS_Node* l_pcTrg = dynamic_cast<SP_DS_Node*>(p_pcTrg);
    if (l_pcSrc->GetNodeclass() == l_pcTrg->GetNodeclass())
        return FALSE;

    if (l_pcSrc->GetCoarse() && l_pcTrg->GetCoarse())
        return FALSE;
    if (l_pcSrc->GetCoarse())
        return (l_pcSrc->GetCoarse()->GetInnerClass() !=  l_pcTrg->GetNodeclass()->GetName());

    if (l_pcTrg->GetCoarse())
        return (l_pcTrg->GetCoarse()->GetInnerClass() !=  l_pcSrc->GetNodeclass()->GetName());

	SP_ListEdge::const_iterator eIt;
	const SP_ListEdge* edges;
	edges = (dynamic_cast<SP_DS_Node*> (p_pcSrc))->GetSourceEdges();

	//check if there are another arc of the same type between these nodes
	for (eIt = edges->begin(); eIt != edges->end(); ++eIt)
	{
		SP_LOGDEBUG(wxString::Format(wxT("%p->%p (%p), %s"), (*eIt)->GetSource(), (*eIt)->GetTarget(), p_pcTrg, (*eIt)->GetClassName().c_str()));

		if ((*eIt)->GetTarget() == p_pcTrg && (*eIt)->GetClassName() == p_pcClass->GetName())
		{
			SP_MESSAGEBOX(wxString::Format(wxT("Only one edge of the type \"%s\" ")
			wxT("is allowed between these nodes"), (*eIt)->GetClassName().c_str()), wxT("Error"), wxOK | wxICON_ERROR);
			return false;
		}
	}

    return TRUE;
}

bool
SP_DS_SimplePed::MetadataRequirement(SP_DS_Metadata* p_pcMetadata)
{
    return SP_DS_BipartGraph::MetadataRequirement(p_pcMetadata);
}
