//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: XML Writer, writes graph structures actually
//////////////////////////////////////////////////////////////////////
#include "sp_core/SP_Writer.h"
#include "sp_utilities.h"


SP_Writer::SP_Writer()
:m_pcGraph(NULL)
{
    if (!m_bError)
        Init();
}

void
SP_Writer::Init()
{
}

SP_Writer::~SP_Writer()
{
}

bool
SP_Writer::Write(SP_DS_Graph* p_pcGraph, const wxString& p_sFile)
{
    CHECK_POINTER(p_pcGraph, return FALSE);
    CHECK_BOOL((!p_sFile.IsEmpty()), return FALSE);

    m_pcGraph = p_pcGraph;

    WriteNetclass(p_pcGraph->GetNetclass());
    WriteClasses(p_pcGraph);

    m_pcGraph = NULL;

    return !m_bError;
}

bool
SP_Writer::WriteNetclass(SP_DS_Netclass* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    // do something

    return TRUE;
}

bool
SP_Writer::WriteClasses(SP_DS_Graph* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    SP_ListNodeclass::const_iterator l_NIter;
    const SP_ListNodeclass* l_plNC = p_pcVal->GetNodeclasses();
    SP_ListEdgeclass::const_iterator l_EIter;
    const SP_ListEdgeclass* l_plEC = p_pcVal->GetEdgeclasses();
    SP_ListMetadataclass::const_iterator l_MIter;
    const SP_ListMetadataclass* l_plMC = p_pcVal->GetMetadataclasses();

    CHECK_POINTER(l_plNC, return FALSE);
    CHECK_POINTER(l_plEC, return FALSE);
    CHECK_POINTER(l_plMC, return FALSE);

    // do something for NC in general

    for (l_NIter = l_plNC->begin(); l_NIter != l_plNC->end(); ++l_NIter)
        WriteNodeclass((*l_NIter));

    // do something for EC in general

    for (l_EIter = l_plEC->begin(); l_EIter != l_plEC->end(); ++l_EIter)
        WriteEdgeclass((*l_EIter));

    // do something for MC in general

    for (l_MIter = l_plMC->begin(); l_MIter != l_plMC->end(); ++l_MIter)
        WriteMetadataclass((*l_MIter));

    return TRUE;
}

bool
SP_Writer::WriteNodeclass(SP_DS_Nodeclass* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    SP_ListNode::const_iterator l_Iter;
    const SP_ListNode* l_plElements = p_pcVal->GetElements();

    CHECK_POINTER(l_plElements, return FALSE);

    // do something

    for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
        WriteNode((*l_Iter));

    return TRUE;
}

bool
SP_Writer::WriteEdgeclass(SP_DS_Edgeclass* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    SP_ListEdge::const_iterator l_Iter;
    const SP_ListEdge* l_plElements = p_pcVal->GetElements();

    CHECK_POINTER(l_plElements, return FALSE);

    // do something

    for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
        WriteEdge((*l_Iter));

    return TRUE;
}

bool
SP_Writer::WriteMetadataclass(SP_DS_Metadataclass* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    SP_ListMetadata::const_iterator l_Iter;
    const SP_ListMetadata* l_plElements = p_pcVal->GetElements();

    CHECK_POINTER(l_plElements, return FALSE);

    // do something

    for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
        WriteMetadata((*l_Iter));

    return TRUE;
}

bool
SP_Writer::WriteNode(SP_DS_Node* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();

    CHECK_POINTER(l_plAttributes, return FALSE);

    // do something

    for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
        WriteAttribute((*l_Iter));

    return WriteData(p_pcVal);
}

bool
SP_Writer::WriteEdge(SP_DS_Edge* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();

    CHECK_POINTER(l_plAttributes, return FALSE);

    // do something

    for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
        WriteAttribute((*l_Iter));

    return WriteData(p_pcVal);
}

bool
SP_Writer::WriteMetadata(SP_DS_Metadata* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();

    CHECK_POINTER(l_plAttributes, return FALSE);

    // do something

    for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
        WriteAttribute((*l_Iter));

    return WriteData(p_pcVal);
}

bool
SP_Writer::WriteAttribute(SP_DS_Attribute* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    wxString l_sVal = p_pcVal->GetValueString();

    // do something

    return WriteData(p_pcVal);
}

bool
SP_Writer::WriteData(SP_Data* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    SP_ListGraphic::iterator l_Iter;
    SP_ListGraphic* l_plGraphics = p_pcVal->GetGraphics();

    // do something

    if (p_pcVal->GetCoarse())
    {
        if (p_pcVal->GetCoarse()->GetGraph() != m_pcGraph)
        {
            WriteNetclass(p_pcVal->GetCoarse()->GetGraph()->GetNetclass());
            WriteClasses(p_pcVal->GetCoarse()->GetGraph());
        }
    }

    if (l_plGraphics)
        for (l_Iter = l_plGraphics->begin(); l_Iter != l_plGraphics->end(); ++l_Iter)
            WriteGraphic((*l_Iter));

    return TRUE;
}

bool
SP_Writer::WriteGraphic(SP_Graphic* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    if (p_pcVal->GetGraphicType() == SP_GRAPHIC_EDGE)
        return WriteEdgeGraphic(dynamic_cast<SP_GR_Edge*>(p_pcVal));

    // do something

    // colours
    WriteColourInformation(p_pcVal);

    return TRUE;
}

bool
SP_Writer::WriteEdgeGraphic(SP_GR_Edge* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    wxList* l_pcList = dynamic_cast<wxLineShape*>(p_pcVal->GetPrimitive())->GetLineControlPoints();
    if (l_pcList)
    {
        wxNode* l_pcNode = l_pcList->GetFirst();
        wxRealPoint* l_pcPoint;
        while (l_pcNode)
        {
            l_pcPoint = (wxRealPoint*)(l_pcNode->GetData());
            // value in l_pcPoint->x and l_pcPoint->y
            // do something
            l_pcNode = l_pcNode->GetNext();
        }
    }

    return TRUE;
}

bool
SP_Writer::WriteColourInformation(SP_Graphic* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    wxString l_sValue;
    wxColour l_pcCol;
    if (p_pcVal->GetPenSet())
    {
        l_pcCol = p_pcVal->GetPen()->GetColour();
        l_sValue = wxString::Format(wxT("%d,%d,%d"), l_pcCol.Red(), l_pcCol.Green(), l_pcCol.Blue());
        // do something
    }
    if (p_pcVal->GetBrushSet())
    {
        l_pcCol = p_pcVal->GetBrush()->GetColour();
        l_sValue = wxString::Format(wxT("%d,%d,%d"), l_pcCol.Red(), l_pcCol.Green(), l_pcCol.Blue());
        // do something
    }
    return TRUE;
}

