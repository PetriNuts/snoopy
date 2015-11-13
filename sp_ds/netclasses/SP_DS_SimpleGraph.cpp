//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/netclasses/SP_DS_SimpleGraph.h"
#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Metadataclass.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_LogicAttribute.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_core/SP_Core.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"

#include "sp_gr/shapes/SP_GR_Ellipse.h"
#include "sp_gr/metadata/SP_GR_MetaRectangle.h"
#include "sp_gr/edges/SP_GR_ArrowEdge.h"
#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gr/attributes/SP_GR_CommentAttribute.h"
#include "sp_gr/attributes/SP_GR_ImageAttribute.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogText.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogShowOnly.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogFilename.h"

#include "sp_defines.h"

SP_DS_SimpleGraph::SP_DS_SimpleGraph()
:SP_DS_Netclass(SP_DS_SIMPLEGRAPH_CLASS)
{
    m_sExtension = wxT("spgraph");
	m_sFilter = wxT("*.") + m_sExtension;
}

SP_DS_SimpleGraph::SP_DS_SimpleGraph(const wxString& p_pchName)
:SP_DS_Netclass(p_pchName)
{
    m_sExtension = wxT("spgraph");
	m_sFilter = wxT("*.") + m_sExtension;
}

SP_DS_Graph*
SP_DS_SimpleGraph::CreateGraph(SP_DS_Graph* p_pcGraph)
{
	if (!SP_DS_Netclass::CreateGraph(p_pcGraph))
        return NULL;

    SP_DS_Nodeclass* l_pcNC;
    SP_DS_Edgeclass* l_pcEC;
    SP_DS_Metadataclass* l_pcMC;
    SP_DS_Attribute* l_pcAttr;
    SP_GR_Metadata* l_pcGr;
    SP_Graphic* l_pcGrAttr;

    //////////////////////////////////////////////////////////////////////////////
    l_pcNC = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Node")));
    l_pcNC->SetGraphic(new SP_GR_Ellipse(l_pcNC->GetPrototype(), 20.0, 20.0));

    ///////////////////////////////////////////////////////////////////////////////
    l_pcEC = p_pcGraph->AddEdgeclass(new SP_DS_Edgeclass(p_pcGraph, wxT("Edge")));
    l_pcEC->SetGraphic(new SP_GR_ArrowEdge(l_pcEC->GetPrototype()));

    ///////////////////////////////////////////////////////////////////////////////
    l_pcMC = p_pcGraph->AddMetadataclass(new SP_DS_Metadataclass(p_pcGraph, wxT("General")));
	l_pcMC->SetShowInElementTree(false);

	l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TextAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General"),1));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_CommentAttribute(l_pcAttr));

	l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TextAttribute(wxT("Created"), wxDateTime::Now().FormatISODate() + wxT(" ") +wxDateTime::Now().FormatISOTime()));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr, wxT("Created: %")));
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetY(20);
	l_pcGrAttr->SetShow(false);

	l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TextAttribute(wxT("Authors"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr, wxT("Author(s): %")));
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetY(40);

	l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TextAttribute(wxT("Keywords"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("Description")));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr, wxT("Tags: %")));
	l_pcGrAttr->SetOffsetY(25);
	l_pcGrAttr->SetOffsetX(40);

	l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TextAttribute(wxT("Description"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("Description")));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr, wxT("Description: %")));
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetY(40);

	l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TextAttribute(wxT("References"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("Description")));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr, wxT("References: %")));
	l_pcGrAttr->SetOffsetX(25);
	l_pcGrAttr->SetOffsetY(40);

	l_pcGr = new SP_GR_MetaRectangle(l_pcMC->GetPrototype(), 2, 2);
    l_pcGr->SetFixedSize(FALSE);
    l_pcGr->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(255,255,255), 1, wxPENSTYLE_TRANSPARENT ));
    l_pcGr->SetDefaultBrush(wxTheBrushList->FindOrCreateBrush(wxColour(255,255,255), wxBRUSHSTYLE_TRANSPARENT ));
    l_pcMC->SetGraphic(l_pcGr);
	l_pcMC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	///////////////////////////////////////////////////////////////////////////////
    l_pcMC = p_pcGraph->AddMetadataclass(new SP_DS_Metadataclass(p_pcGraph, wxT("Comment")));
    l_pcMC->SetShortcut(wxT("C"));
    l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("Comment")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_CommentAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(0);
	l_pcGrAttr->SetShow(TRUE);

	l_pcGr = new SP_GR_MetaRectangle(l_pcMC->GetPrototype(), 2, 2);
    l_pcGr->SetFixedSize(FALSE);
    l_pcGr->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(255,255,255), 1 ));
    l_pcMC->SetGraphic(l_pcGr);
	l_pcMC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	///////////////////////////////////////////////////////////////////////////////
//#ifdef __WXDEBUG__
    l_pcMC = p_pcGraph->AddMetadataclass(new SP_DS_Metadataclass(p_pcGraph, wxT("Image")));
//    l_pcMC->SetShortcut(wxT("U"));
    l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TextAttribute(wxT("Filename")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogFilename(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_ImageAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(0);
    l_pcGrAttr->SetOffsetY(0);
	l_pcGrAttr->SetShow(TRUE);

    l_pcAttr = l_pcMC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(20);
    l_pcGrAttr->SetOffsetY(20);
	l_pcGrAttr->SetShow(TRUE);

	l_pcGr = new SP_GR_MetaRectangle(l_pcMC->GetPrototype(), 2, 2);
    l_pcGr->SetFixedSize(true);
    l_pcGr->SetDefaultPen(wxThePenList->FindOrCreatePen(wxColour(255,255,255), 1, wxPENSTYLE_TRANSPARENT ));
    l_pcGr->SetDefaultBrush(wxTheBrushList->FindOrCreateBrush(wxColour(255,255,255), wxBRUSHSTYLE_TRANSPARENT ));
    l_pcMC->SetGraphic(l_pcGr);
	l_pcMC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
//#endif

	return p_pcGraph;
}

bool SP_DS_SimpleGraph::CreateInitialElements(SP_DS_Graph* p_pcGraph)
{
	if(!SP_DS_Netclass::CreateInitialElements(p_pcGraph))
	{
		return false;
	}

	SP_DS_Metadataclass* l_pcMC = p_pcGraph->GetMetadataclass(wxT("General"));
	SP_DS_Metadata* l_pcGeneral = l_pcMC->NewElement(1);
	if(l_pcGeneral)
	{
		l_pcGeneral->GetGraphics()->front()->SetPosXY(20,20);
	}
	return true;
}

bool
SP_DS_SimpleGraph::MetadataRequirement(SP_DS_Metadata* p_pcMetadata)
{

	if (p_pcMetadata->GetClassName() == wxT("General") &&
		!p_pcMetadata->GetMetadataclass()->GetElements()->empty())
	{
		return false;
	}

	return true;
}
