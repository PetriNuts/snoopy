//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/netclasses/SP_DS_BipartGraph.h"
#include "sp_gr/shapes/SP_GR_Rectangle.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include "sp_ds/SP_DS_Graph.h"

#include "sp_gr/attributes/SP_GR_TextAttribute.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogText.h"

#include "sp_utilities.h"


SP_DS_BipartGraph::SP_DS_BipartGraph()
:SP_DS_SimpleGraph(SP_DS_BIPARTGRAPH_CLASS)
{
    m_sExtension = wxT("spbipart");
	m_sFilter = wxT("");
}

SP_DS_BipartGraph::SP_DS_BipartGraph(const wxString& p_pchName)
:SP_DS_SimpleGraph(p_pchName)
{
    m_sExtension = wxT("spbipart");
	m_sFilter = wxT("");
}

SP_DS_Graph*
SP_DS_BipartGraph::CreateGraph(SP_DS_Graph* p_pcGraph, SP_MapString2Int p_mgraphicsPosMap)
{
    if (!SP_DS_SimpleGraph::CreateGraph(p_pcGraph))
        return NULL;

    SP_DS_Attribute* l_pcAttr;
    SP_Graphic* l_pcGrAttr;
    SP_DS_Nodeclass* l_pcNC;
    SP_DS_Edgeclass* l_pcEC;

    //////////////////////////////////////////////////////////////////////////////
    l_pcNC = p_pcGraph->RenameNodeclass(wxT("Node"), wxT("Circle"));
    l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General"),1));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);

    //////////////////////////////////////////////////////////////////////////////
    l_pcNC = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Rectangle")));
    l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General"),1));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(25);
    l_pcGrAttr->SetOffsetY(20);
    l_pcGrAttr->SetShow(TRUE);
    l_pcNC->SetGraphic(new SP_GR_Rectangle(l_pcNC->GetPrototype(), 20.0, 20.0));

    //////////////////////////////////////////////////////////////////////////////
    l_pcEC = p_pcGraph->GetEdgeclass(wxT("Edge"));
    l_pcAttr = l_pcEC->AddAttribute(new SP_DS_NameAttribute(wxT("Name")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetShow(TRUE);

    return p_pcGraph;
}

bool
SP_DS_BipartGraph::EdgeRequirement(SP_DS_Edgeclass* p_pcClass,
                                   SP_Data* p_pcSrc,
                                   SP_Data* p_pcTrg)
{
    if (!SP_DS_SimpleGraph::EdgeRequirement(p_pcClass, p_pcSrc, p_pcTrg))
        return FALSE;

    return (p_pcSrc->GetClassObject() != p_pcTrg->GetClassObject());
}

bool
SP_DS_BipartGraph::MetadataRequirement(SP_DS_Metadata* p_pcMetadata)
{
	return SP_DS_SimpleGraph::MetadataRequirement(p_pcMetadata);
}
