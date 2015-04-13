//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/12/17 11:55:00 $
// Short Description: XML Writer, writes graph structures actually
//////////////////////////////////////////////////////////////////////
#include <wx/fs_zip.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/textfile.h>

#include "sp_core/SP_XmlWriter.h"
#include "sp_defines.h"
#include "sp_revision.h"
#include "sp_gr/SP_GR_Node.h"
#include "sp_gr/SP_GR_Metadata.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

SP_XmlWriter::SP_XmlWriter():
m_pcGraph(NULL)
{
	Init();
}

void
SP_XmlWriter::Init()
{
}

SP_XmlWriter::~SP_XmlWriter()
{
}

bool SP_XmlWriter::Write(SP_DS_Graph* p_pcGraph, const wxString& p_sFile)
{
	bool l_bReturn = false;
	wxFFileOutputStream out(p_sFile);
	if(out.IsOk())
	{
		if(p_pcGraph->GetParentDoc()->GetCompress())
		{
			wxZipOutputStream zip(out);
			zip.PutNextEntry("model.xml");
			if(zip.IsOk())
				l_bReturn = Write(p_pcGraph, zip);
		}
		else
		{
			l_bReturn = Write(p_pcGraph, out);
		}
	}
#if wxABI_VERSION < 30000
		//workaround for wxXML_PI_NODE
		if(!m_bError)
		{
			wxTextFile l_File(p_sFile);
			if(l_File.Open())
			{
				wxString l_sProcessingInstruction = wxT("<?xml-stylesheet type=\"text/xsl\" href=\"/xsl/spped2svg.xsl\"?>");
				l_File.InsertLine(l_sProcessingInstruction, 1);
				l_File.Write();
			}
		}
#endif
	return l_bReturn;
}

bool
SP_XmlWriter::Write(SP_DS_Graph* p_pcGraph, wxOutputStream& p_OutputStream)
{
    CHECK_POINTER(p_pcGraph, return FALSE);
    CHECK_BOOL((p_OutputStream.IsOk()), return FALSE);

    m_pcGraph = p_pcGraph;

    wxXmlDocument* l_pcDoc = new wxXmlDocument();

#if wxABI_VERSION > 30000
    //write processing instructions with wxXML
	wxXmlNode* l_pcProcessingInstructions = new wxXmlNode(nullptr, wxXML_PI_NODE, wxT("xml-stylesheet"), wxT("type=\"text/xsl\" href=\"/xsl/spped2svg.xsl\""));
	l_pcDoc->AppendToProlog(l_pcProcessingInstructions);
#endif

    wxXmlNode* l_pcRootElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, SP_APP_SHORT_NAME);
    l_pcRootElem->AddAttribute(wxT("version"), SP_APP_VERSION);
    l_pcRootElem->AddAttribute(wxT("revision"), SP_APP_REVISION);
    l_pcDoc->SetRoot(l_pcRootElem);

    WriteNetclass(p_pcGraph->GetNetclass(), l_pcRootElem);
    WriteClasses(p_pcGraph, l_pcRootElem);

    if(!l_pcDoc->Save(p_OutputStream))
    {
        m_sMessage << wxT("An error occurred creating the document") << wxT("\n");
        m_bError = TRUE;
    }
    wxDELETE(l_pcDoc);

    m_pcGraph = NULL;
    return !m_bError;
}

bool
SP_XmlWriter::WriteNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);
    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
    l_pcElem->AddAttribute(wxT("name"), p_pcVal->GetName());
    p_pcRoot->AddChild(l_pcElem);

    return TRUE;
}

bool
SP_XmlWriter::WriteClasses(SP_DS_Graph* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    SP_ListNodeclass::const_iterator l_NIter;
    const SP_ListNodeclass* l_plNC = p_pcVal->GetNodeclasses();
    SP_ListEdgeclass::const_iterator l_EIter;
    const SP_ListEdgeclass* l_plEC = p_pcVal->GetEdgeclasses();
    SP_ListMetadataclass::const_iterator l_MIter;
    const SP_ListMetadataclass* l_plMC = p_pcVal->GetMetadataclasses();

    CHECK_POINTER(l_plNC, return FALSE);
    CHECK_POINTER(l_plEC, return FALSE);
    CHECK_POINTER(l_plMC, return FALSE);

    wxXmlNode* l_pcElemNC = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclasses"));
    l_pcElemNC->AddAttribute(wxT("count"), wxString::Format(wxT("%lu"), l_plNC->size()));
    p_pcRoot->AddChild(l_pcElemNC);

    for (l_NIter = l_plNC->begin(); l_NIter != l_plNC->end(); ++l_NIter)
        WriteNodeclass((*l_NIter), l_pcElemNC);

    wxXmlNode* l_pcElemEC = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edgeclasses"));
    l_pcElemEC->AddAttribute(wxT("count"), wxString::Format(wxT("%lu"), l_plEC->size()));
    p_pcRoot->AddChild(l_pcElemEC);

    for (l_EIter = l_plEC->begin(); l_EIter != l_plEC->end(); ++l_EIter)
        WriteEdgeclass((*l_EIter), l_pcElemEC);

    wxXmlNode* l_pcElemMC = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("metadataclasses"));
    l_pcElemMC->AddAttribute(wxT("count"), wxString::Format(wxT("%lu"), l_plMC->size()));
    p_pcRoot->AddChild(l_pcElemMC);

    for (l_MIter = l_plMC->begin(); l_MIter != l_plMC->end(); ++l_MIter)
        WriteMetadataclass((*l_MIter), l_pcElemMC);

    return TRUE;
}

bool
SP_XmlWriter::WriteNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);


    SP_ListNode::const_iterator l_Iter;
    const SP_ListNode* l_plElements = p_pcVal->GetElements();
    CHECK_POINTER(l_plElements, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("nodeclass"));
    l_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%lu"), l_plElements->size()));
    l_pcElem->AddAttribute(wxT("name"), p_pcVal->GetName());
    p_pcRoot->AddChild(l_pcElem);

    for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
        WriteNode((*l_Iter), l_pcElem);

    return TRUE;
}

bool
SP_XmlWriter::WriteEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);


    SP_ListEdge::const_iterator l_Iter;
    const SP_ListEdge* l_plElements = p_pcVal->GetElements();
    CHECK_POINTER(l_plElements, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edgeclass"));
    l_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%lu"), l_plElements->size()));
    l_pcElem->AddAttribute(wxT("name"), p_pcVal->GetName());
    p_pcRoot->AddChild(l_pcElem);

    for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
        WriteEdge((*l_Iter), l_pcElem);

    return TRUE;
}

bool
SP_XmlWriter::WriteMetadataclass(SP_DS_Metadataclass* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);


    SP_ListMetadata::const_iterator l_Iter;
    const SP_ListMetadata* l_plElements = p_pcVal->GetElements();
    CHECK_POINTER(l_plElements, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("metadataclass"));
    l_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%lu"), l_plElements->size()));
    l_pcElem->AddAttribute(wxT("name"), p_pcVal->GetName());
    p_pcRoot->AddChild(l_pcElem);

    for (l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter)
        WriteMetadata((*l_Iter), l_pcElem);

    return TRUE;
}

bool
SP_XmlWriter::WriteNode(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);


    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
    CHECK_POINTER(l_plAttributes, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("node"));
    p_pcRoot->AddChild(l_pcElem);

    for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
        WriteAttribute((*l_Iter), l_pcElem);

    return WriteData(p_pcVal, l_pcElem);
}

bool
SP_XmlWriter::WriteEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);


    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
    CHECK_POINTER(l_plAttributes, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edge"));
    if (p_pcVal->GetSource())
        l_pcElem->AddAttribute(wxT("source"), wxString::Format(wxT("%ld"), p_pcVal->GetSource()->GetId()));
    if (p_pcVal->GetTarget())
        l_pcElem->AddAttribute(wxT("target"), wxString::Format(wxT("%ld"), p_pcVal->GetTarget()->GetId()));
    p_pcRoot->AddChild(l_pcElem);

    for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
        WriteAttribute((*l_Iter), l_pcElem);

    return WriteData(p_pcVal, l_pcElem);
}

bool
SP_XmlWriter::WriteMetadata(SP_DS_Metadata* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);


    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
    CHECK_POINTER(l_plAttributes, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("metadata"));
    p_pcRoot->AddChild(l_pcElem);

    for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
        WriteAttribute((*l_Iter), l_pcElem);

    return WriteData(p_pcVal, l_pcElem);
}

/* inserted by dscheibl 22.06.2005 */
/*
 * This method does nearly the same as method WriteEdge
 * - edges will saved in the opposite direction as the given one
 */
bool
SP_XmlWriter::WriteOppositeEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);


    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
    CHECK_POINTER(l_plAttributes, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edge"));
/* changing to WriteEdgeGraphic */
    if (p_pcVal->GetSource())
        l_pcElem->AddAttribute(wxT("target"), wxString::Format(wxT("%ld"), p_pcVal->GetSource()->GetId()));
    if (p_pcVal->GetTarget())
        l_pcElem->AddAttribute(wxT("source"), wxString::Format(wxT("%ld"), p_pcVal->GetTarget()->GetId()));
/* changing end */
    p_pcRoot->AddChild(l_pcElem);

    for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
        WriteAttribute((*l_Iter), l_pcElem);

/* changing to WriteEdgeGraphic */
    return WriteOppositeData(p_pcVal, l_pcElem);
/* changing end */
}
/* insertion end */

/* inserted by dscheibl 22.06.2005 */
/*
 * This method does nearly the same as method WriteEdge
 * - edges will saved in color RED
 */
bool
SP_XmlWriter::WriteRedEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);


    SP_ListAttribute::const_iterator l_Iter;
    const SP_ListAttribute* l_plAttributes = p_pcVal->GetAttributes();
    CHECK_POINTER(l_plAttributes, return FALSE);

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("edge"));
    if (p_pcVal->GetSource())
        l_pcElem->AddAttribute(wxT("source"), wxString::Format(wxT("%ld"), p_pcVal->GetSource()->GetId()));
    if (p_pcVal->GetTarget())
        l_pcElem->AddAttribute(wxT("target"), wxString::Format(wxT("%ld"), p_pcVal->GetTarget()->GetId()));
    p_pcRoot->AddChild(l_pcElem);

    for (l_Iter = l_plAttributes->begin(); l_Iter != l_plAttributes->end(); ++l_Iter)
        WriteAttribute((*l_Iter), l_pcElem);

/* changing to WriteEdgeGraphic */
    return WriteRedData(p_pcVal, l_pcElem);
/* changing end */
}
/* insertion end */

bool
SP_XmlWriter::WriteAttribute( SP_DS_Attribute* p_pcVal, wxXmlNode* p_pcRoot )
{
    CHECK_POINTER( p_pcVal, return FALSE );
    CHECK_POINTER( p_pcRoot, return FALSE );

    if( p_pcVal->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST ) {

      return WriteColListAttribute( dynamic_cast< SP_DS_ColListAttribute* >( p_pcVal ), p_pcRoot );

    }

    wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("attribute"));
    wxString l_sVal = p_pcVal->GetValueString();

    l_pcElem->AddAttribute(wxT("name"), p_pcVal->GetName());
    p_pcRoot->AddChild(l_pcElem);

    wxXmlNode* l_pcCDATA = new wxXmlNode(NULL, wxXML_CDATA_SECTION_NODE, wxT(""), l_sVal);
    l_pcElem->AddChild(l_pcCDATA);

    return WriteData( p_pcVal, l_pcElem );

}

bool
SP_XmlWriter::WriteData(SP_Data* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);


    SP_ListGraphic::iterator l_Iter;
    SP_ListGraphic* l_plGraphics = p_pcVal->GetGraphics();

    p_pcRoot->AddAttribute(wxT("id"), wxString::Format(wxT("%ld"), p_pcVal->GetId()));
    p_pcRoot->AddAttribute(wxT("net"), wxString::Format(wxT("%d"), p_pcVal->GetNetnumber()));
    if (p_pcVal->GetLogical())
        p_pcRoot->AddAttribute(wxT("logic"), wxT("1"));

    if (p_pcVal->GetCoarse())
    {
        p_pcRoot->AddAttribute(wxT("coarse"), wxString::Format(wxT("%d"), p_pcVal->GetCoarse()->GetNetnumber()));
        if (p_pcVal->GetCoarse()->GetGraph() != m_pcGraph)
        {
            WriteNetclass(p_pcVal->GetCoarse()->GetGraph()->GetNetclass(), p_pcRoot);
            WriteClasses(p_pcVal->GetCoarse()->GetGraph(), p_pcRoot);
        }
    }

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("graphics"));
    l_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%lu"), (l_plGraphics?l_plGraphics->size():0)));
    p_pcRoot->AddChild(l_pcElem);

    if (l_plGraphics)
        for (l_Iter = l_plGraphics->begin(); l_Iter != l_plGraphics->end(); ++l_Iter)
            WriteGraphic((*l_Iter), l_pcElem);

    return TRUE;
}

/* inserted by dscheibl 22.06.2005 */
/*
 * This method does nearly the same as method WriteData
 * - edges will saved in the opposite direction as the given one
 */
bool
SP_XmlWriter::WriteOppositeData(SP_Data* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);


    SP_ListGraphic::iterator l_Iter;
    SP_ListGraphic* l_plGraphics = p_pcVal->GetGraphics();

    p_pcRoot->AddAttribute(wxT("id"), wxString::Format(wxT("%ld"), p_pcVal->GetId()));
    p_pcRoot->AddAttribute(wxT("net"), wxString::Format(wxT("%d"), p_pcVal->GetNetnumber()));
    if (p_pcVal->GetLogical())
        p_pcRoot->AddAttribute(wxT("logic"), wxT("1"));

    if (p_pcVal->GetCoarse())
    {
        p_pcRoot->AddAttribute(wxT("coarse"), wxString::Format(wxT("%d"), p_pcVal->GetCoarse()->GetNetnumber()));
        if (p_pcVal->GetCoarse()->GetGraph() != m_pcGraph)
        {
            WriteNetclass(p_pcVal->GetCoarse()->GetGraph()->GetNetclass(), p_pcRoot);
            WriteClasses(p_pcVal->GetCoarse()->GetGraph(), p_pcRoot);
        }
    }

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("graphics"));
    l_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%lu"), (l_plGraphics?l_plGraphics->size():0)));
    p_pcRoot->AddChild(l_pcElem);

    if (l_plGraphics)
        for (l_Iter = l_plGraphics->begin(); l_Iter != l_plGraphics->end(); ++l_Iter)
            WriteOppositeGraphic((*l_Iter), l_pcElem);    // changing to WriteData

    return TRUE;
}
/* insertion end */

/* inserted by dscheibl 22.06.2005 */
/*
 * This method does nearly the same as method WriteData
 * - edges will saved in color RED
 */
bool
SP_XmlWriter::WriteRedData(SP_Data* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);


    SP_ListGraphic::iterator l_Iter;
    SP_ListGraphic* l_plGraphics = p_pcVal->GetGraphics();

    p_pcRoot->AddAttribute(wxT("id"), wxString::Format(wxT("%ld"), p_pcVal->GetId()));
    p_pcRoot->AddAttribute(wxT("net"), wxString::Format(wxT("%d"), p_pcVal->GetNetnumber()));
    if (p_pcVal->GetLogical())
        p_pcRoot->AddAttribute(wxT("logic"), wxT("1"));

    if (p_pcVal->GetCoarse())
    {
        p_pcRoot->AddAttribute(wxT("coarse"), wxString::Format(wxT("%d"), p_pcVal->GetCoarse()->GetNetnumber()));
        if (p_pcVal->GetCoarse()->GetGraph() != m_pcGraph)
        {
            WriteNetclass(p_pcVal->GetCoarse()->GetGraph()->GetNetclass(), p_pcRoot);
            WriteClasses(p_pcVal->GetCoarse()->GetGraph(), p_pcRoot);
        }
    }

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("graphics"));
    l_pcElem->AddAttribute(wxT("count"), wxString::Format(wxT("%lu"), (l_plGraphics?l_plGraphics->size():0)));
    p_pcRoot->AddChild(l_pcElem);

    if (l_plGraphics)
        for (l_Iter = l_plGraphics->begin(); l_Iter != l_plGraphics->end(); ++l_Iter)
          WriteRedGraphic((*l_Iter), l_pcElem);    // changing to WriteData

    return TRUE;
}
/* insertion end */

bool
SP_XmlWriter::WriteGraphic(SP_Graphic* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);


    if (p_pcVal->GetGraphicType() == SP_GRAPHIC_EDGE)
	{
	   return WriteEdgeGraphic(dynamic_cast<SP_GR_Edge*>(p_pcVal), p_pcRoot);
	}

    wxXmlNode*  l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("graphic"));
    if (p_pcVal->GetOffsetX() != 0.0)
        l_pcElem->AddAttribute(wxT("xoff"), wxString::Format( wxT("%.2f"), p_pcVal->GetOffsetX()));
    if (p_pcVal->GetOffsetY() != 0.0)
        l_pcElem->AddAttribute(wxT("yoff"), wxString::Format( wxT("%.2f"), p_pcVal->GetOffsetY()));
    if (p_pcVal->GetPosX() != 0.0)
        l_pcElem->AddAttribute(wxT("x"), wxString::Format( wxT("%.2f"), p_pcVal->GetPosX()));
    if (p_pcVal->GetPosY() != 0.0)
        l_pcElem->AddAttribute(wxT("y"), wxString::Format( wxT("%.2f"), p_pcVal->GetPosY()));
    l_pcElem->AddAttribute(wxT("id"), wxString::Format( wxT("%ld"), p_pcVal->GetId()));
    l_pcElem->AddAttribute(wxT("net"), wxString::Format( wxT("%d"), p_pcVal->GetNetnumber()));
    if (p_pcVal->GetShow())
        l_pcElem->AddAttribute(wxT("show"), wxT("1"));
    else
        l_pcElem->AddAttribute(wxT("show"), wxT("0"));
    if (p_pcVal->GetGraphicType() == SP_GRAPHIC_NODE)
    {
        SP_GR_Node* l_pcNode = dynamic_cast<SP_GR_Node*>(p_pcVal);
        if (!l_pcNode->GetFixedSize())
        {
            l_pcElem->AddAttribute(wxT("w"), wxString::Format( wxT("%.2f"), l_pcNode->GetWidth()));
            l_pcElem->AddAttribute(wxT("h"), wxString::Format( wxT("%.2f"), l_pcNode->GetHeight()));
        }
    }
    else if (p_pcVal->GetGraphicType() == SP_GRAPHIC_METADATA)
    {
        SP_GR_Metadata* l_pcMetadata = dynamic_cast<SP_GR_Metadata*>(p_pcVal);
        if (!l_pcMetadata->GetFixedSize())
        {
            l_pcElem->AddAttribute(wxT("w"), wxString::Format( wxT("%.2f"), l_pcMetadata->GetWidth()));
            l_pcElem->AddAttribute(wxT("h"), wxString::Format( wxT("%.2f"), l_pcMetadata->GetHeight()));
        }
    }

    if (p_pcVal->GetGraphicParent() != p_pcVal)
        l_pcElem->AddAttribute(wxT("grparent"), wxString::Format( wxT("%ld"), p_pcVal->GetGraphicParent()->GetId()));
    l_pcElem->AddAttribute(wxT("state"), wxString::Format( wxT("%d"), p_pcVal->GetGraphicState()));
    // colours
    WriteColourInformation(p_pcVal, l_pcElem);

    if (p_pcVal->GetDesignType() != 0) {
        l_pcElem->AddAttribute(wxT("designtype"), wxString::Format( wxT("%ld"), p_pcVal->GetDesignType()));
    }
	if (p_pcVal->GetThickness() != 0) {
    	l_pcElem->AddAttribute(wxT("thickness"),  wxString::Format( wxT("%ld"), p_pcVal->GetThickness()));
	}

    p_pcRoot->AddChild(l_pcElem);

    return TRUE;
}

/* inserted by dscheibl 22.06.2005 */
/*
 * This method does nearly the same as method WriteGraphic
 * - edges will saved in the opposite direction as the given one
 */
bool
SP_XmlWriter::WriteOppositeGraphic(SP_Graphic* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    if (p_pcVal->GetGraphicType() == SP_GRAPHIC_EDGE)
        return WriteOppositeEdgeGraphic(dynamic_cast<SP_GR_Edge*>(p_pcVal), p_pcRoot);

    return WriteGraphic(p_pcVal, p_pcRoot);
}
/* insertion end */

/* inserted by dscheibl 22.06.2005 */
/*
 * This method does nearly the same as method WriteGraphic
 * - edges will saved in color RED
 */
bool
SP_XmlWriter::WriteRedGraphic(SP_Graphic* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    if (p_pcVal->GetGraphicType() == SP_GRAPHIC_EDGE)
        return WriteRedEdgeGraphic(dynamic_cast<SP_GR_Edge*>(p_pcVal), p_pcRoot);

    return WriteGraphic(p_pcVal, p_pcRoot);
}
/* insertion end */

bool
SP_XmlWriter::WriteEdgeGraphic(SP_GR_Edge* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);


    // if control points were added/moved then the offsets are wrong and have to be updated
    p_pcVal->UpdateChildrenOffsets();

    wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("graphic"));
    l_pcElem->AddAttribute(wxT("id"), wxString::Format( wxT("%ld"), p_pcVal->GetId()));
    l_pcElem->AddAttribute(wxT("net"), wxString::Format( wxT("%d"), p_pcVal->GetNetnumber()));
    if (p_pcVal->GetGraphicParent() != p_pcVal)
        l_pcElem->AddAttribute(wxT("grparent"), wxString::Format( wxT("%ld"), p_pcVal->GetGraphicParent()->GetId()));
    if (p_pcVal->GetSource())
        l_pcElem->AddAttribute(wxT("source"), wxString::Format(wxT("%ld"), p_pcVal->GetSource()->GetId()));
    if (p_pcVal->GetTarget())
        l_pcElem->AddAttribute(wxT("target"), wxString::Format(wxT("%ld"), p_pcVal->GetTarget()->GetId()));
    l_pcElem->AddAttribute(wxT("state"), wxString::Format( wxT("%d"), p_pcVal->GetGraphicState()));
    if (p_pcVal->GetShow())
        l_pcElem->AddAttribute(wxT("show"), wxT("1"));
    else
        l_pcElem->AddAttribute(wxT("show"), wxT("0"));
    // colour
    WriteColourInformation(p_pcVal, l_pcElem);

    p_pcRoot->AddChild(l_pcElem);

    // write the line control points
    wxXmlNode* l_pcElem2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("points"));
    l_pcElem->AddChild(l_pcElem2);

    wxXmlNode* l_pcPointE;
    wxList* l_pcList = dynamic_cast<wxLineShape*>(p_pcVal->GetPrimitive())->GetLineControlPoints();
    l_pcElem2->AddAttribute(wxT("count"), wxString::Format( wxT("%lu"), (l_pcList?l_pcList->GetCount():0)));
    if (l_pcList)
    {
        wxNode* l_pcNode = l_pcList->GetFirst();
        wxRealPoint* l_pcPoint;
        while (l_pcNode)
        {
            l_pcPoint = (wxRealPoint*)(l_pcNode->GetData());
            l_pcPointE = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("point"));
            l_pcPointE->AddAttribute(wxT("x"), wxString::Format( wxT("%.2f"), l_pcPoint->x));
            l_pcPointE->AddAttribute(wxT("y"), wxString::Format( wxT("%.2f"), l_pcPoint->y));
            l_pcElem2->AddChild(l_pcPointE);

            l_pcNode = l_pcNode->GetNext();
        }
    }

    if (p_pcVal->GetDesignType() != 0) {
        l_pcElem->AddAttribute(wxT("edge_designtype"), wxString::Format( wxT("%ld"), p_pcVal->GetDesignType()));
    }

	if (p_pcVal->GetThickness() != 0)
    {
		l_pcElem->AddAttribute(wxT("thickness"), wxString::Format( wxT("%ld"), p_pcVal->GetThickness()));
    }
	if (p_pcVal->GetDesignArrowTypeLeft() != 0 )
    {
		l_pcElem->AddAttribute(wxT("ArrowLeft"), wxString::Format( wxT("%ld"), p_pcVal->GetDesignArrowTypeLeft()));
    }
	if (p_pcVal->GetDesignArrowTypeRight() != 0 )
    {
		l_pcElem->AddAttribute(wxT("ArrowRight"), wxString::Format( wxT("%ld"), p_pcVal->GetDesignArrowTypeRight()));
    }
	if (p_pcVal->GetArrowLeftThickness() != 0)
    {
		l_pcElem->AddAttribute(wxT("ArrowLeftThickness"), wxString::Format( wxT("%ld"), p_pcVal->GetArrowLeftThickness()));
    }
	if (p_pcVal->GetArrowRightThickness() != 0)
    {
		l_pcElem->AddAttribute(wxT("ArrowRightThickness"), wxString::Format( wxT("%ld"), p_pcVal->GetArrowRightThickness()));
    }

	return TRUE;
}

/* inserted by dscheibl 22.06.2005 */
/*
 * This method does nearly the same as method WriteEdgeGraphic
 * - edges will saved in the opposite direction as the given one
 */
bool
SP_XmlWriter::WriteOppositeEdgeGraphic(SP_GR_Edge* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);


    wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("graphic"));
    l_pcElem->AddAttribute(wxT("id"), wxString::Format( wxT("%ld"), p_pcVal->GetId()));
    l_pcElem->AddAttribute(wxT("net"), wxString::Format( wxT("%d"), p_pcVal->GetNetnumber()));
    if (p_pcVal->GetGraphicParent() != p_pcVal)
        l_pcElem->AddAttribute(wxT("grparent"), wxString::Format( wxT("%ld"), p_pcVal->GetGraphicParent()->GetId()));
/* changing to WriteEdgeGraphic */
    if (p_pcVal->GetSource())
        l_pcElem->AddAttribute(wxT("target"), wxString::Format( wxT("%ld"), p_pcVal->GetSource()->GetId()));
    if (p_pcVal->GetTarget())
        l_pcElem->AddAttribute(wxT("source"), wxString::Format( wxT("%ld"), p_pcVal->GetTarget()->GetId()));
/* changing end */
    l_pcElem->AddAttribute(wxT("state"), wxString::Format( wxT("%d"), p_pcVal->GetGraphicState()));
    if (p_pcVal->GetShow())
        l_pcElem->AddAttribute(wxT("show"), wxT("1"));
    else
        l_pcElem->AddAttribute(wxT("show"), wxT("0"));
    // colour
    WriteColourInformation(p_pcVal, l_pcElem);

    p_pcRoot->AddChild(l_pcElem);

    // write the line control points
    wxXmlNode* l_pcElem2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("points"));
    l_pcElem->AddChild(l_pcElem2);

    wxXmlNode* l_pcPointE;
    wxList* l_pcList = dynamic_cast<wxLineShape*>(p_pcVal->GetPrimitive())->GetLineControlPoints();
    l_pcElem2->AddAttribute(wxT("count"), wxString::Format( wxT("%lu"), (l_pcList?l_pcList->GetCount():0)));
    if (l_pcList)
    {
/* changing to WriteEdgeGraphic */
        wxNode* l_pcNode;
        wxRealPoint* l_pcPoint;
        int i;
        for (i = l_pcList->GetCount()-1; i >= 0; i--)
        {
            l_pcNode = l_pcList->Item(i);
            l_pcPoint = (wxRealPoint*)(l_pcNode->GetData());
            l_pcPointE = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("point"));
            l_pcPointE->AddAttribute(wxT("x"), wxString::Format( wxT("%.2f"), l_pcPoint->x));
            l_pcPointE->AddAttribute(wxT("y"), wxString::Format( wxT("%.2f"), l_pcPoint->y));
            l_pcElem2->AddChild(l_pcPointE);
        }
/* changing end */
    }

    return TRUE;
}
/* insertion end */

/* inserted by dscheibl 22.06.2005 */
/*
 * This method does nearly the same as method WriteEdgeGraphic
 * - edges will saved in color RED
 */
bool
SP_XmlWriter::WriteRedEdgeGraphic(SP_GR_Edge* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);


    wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("graphic"));
    l_pcElem->AddAttribute(wxT("id"), wxString::Format( wxT("%ld"), p_pcVal->GetId()));
    l_pcElem->AddAttribute(wxT("net"), wxString::Format( wxT("%d"), p_pcVal->GetNetnumber()));
    if (p_pcVal->GetGraphicParent() != p_pcVal)
        l_pcElem->AddAttribute(wxT("grparent"), wxString::Format( wxT("%ld"), p_pcVal->GetGraphicParent()->GetId()));
    if (p_pcVal->GetSource())
        l_pcElem->AddAttribute(wxT("source"), wxString::Format(wxT("%ld"), p_pcVal->GetSource()->GetId()));
    if (p_pcVal->GetTarget())
        l_pcElem->AddAttribute(wxT("target"), wxString::Format(wxT("%ld"), p_pcVal->GetTarget()->GetId()));
    l_pcElem->AddAttribute(wxT("state"), wxString::Format( wxT("%d"), p_pcVal->GetGraphicState()));
    if (p_pcVal->GetShow())
        l_pcElem->AddAttribute(wxT("show"), wxT("1"));
    else
        l_pcElem->AddAttribute(wxT("show"), wxT("0"));
    // colour
/* changing to WriteEdgeGraphic */
    WriteRedColourInformation(p_pcVal, l_pcElem);
/* changing end */

    p_pcRoot->AddChild(l_pcElem);

    // write the line control points
    wxXmlNode* l_pcElem2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("points"));
    l_pcElem->AddChild(l_pcElem2);

    wxXmlNode* l_pcPointE;
    wxList* l_pcList = dynamic_cast<wxLineShape*>(p_pcVal->GetPrimitive())->GetLineControlPoints();
    l_pcElem2->AddAttribute(wxT("count"), wxString::Format( wxT("%lu"), (l_pcList?l_pcList->GetCount():0)));
    if (l_pcList)
    {
        wxNode* l_pcNode = l_pcList->GetFirst();
        wxRealPoint* l_pcPoint;
        while (l_pcNode)
        {
            l_pcPoint = (wxRealPoint*)(l_pcNode->GetData());
            l_pcPointE = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("point"));
            l_pcPointE->AddAttribute(wxT("x"), wxString::Format( wxT("%.2f"), l_pcPoint->x));
            l_pcPointE->AddAttribute(wxT("y"), wxString::Format( wxT("%.2f"), l_pcPoint->y));
            l_pcElem2->AddChild(l_pcPointE);

            l_pcNode = l_pcNode->GetNext();
        }
    }

    return TRUE;
}
/* insertion end */

bool
SP_XmlWriter::WriteColourInformation(SP_Graphic* p_pcVal, wxXmlNode* p_pcNode)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcNode, return FALSE);

    wxString l_sValue;
    wxColour l_pcCol;
    if (p_pcVal->GetPenSet())
    {
        l_pcCol = p_pcVal->GetPen()->GetColour();
        l_sValue = wxString::Format(wxT("%d,%d,%d"), l_pcCol.Red(), l_pcCol.Green(), l_pcCol.Blue());
        p_pcNode->AddAttribute(wxT("pen"), l_sValue);
    }
    if (p_pcVal->GetBrushSet())
    {
        l_pcCol = p_pcVal->GetBrush()->GetColour();
        l_sValue = wxString::Format(wxT("%d,%d,%d"), l_pcCol.Red(), l_pcCol.Green(), l_pcCol.Blue());
        p_pcNode->AddAttribute(wxT("brush"), l_sValue);
    }
    return TRUE;
}

/* inserted by dscheibl 22.06.2005 */
/*
 * This method does nearly the same as method WriteColorInformation
 * - edges will saved in color RED
 */
bool
SP_XmlWriter::WriteRedColourInformation(SP_Graphic* p_pcVal, wxXmlNode* p_pcNode)
{
    //CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcNode, return FALSE);

    wxString l_sValue;
    l_sValue = wxString::Format(wxT("%d,%d,%d"), 255, 0, 0);
    p_pcNode->AddAttribute(wxT("pen"), l_sValue);
    l_sValue = wxString::Format(wxT("%d,%d,%d"), 255, 0, 0);
    p_pcNode->AddAttribute(wxT("brush"), l_sValue);
    return TRUE;
}
/* insertion end */


/* inserted by slehrack 06.06.2007 */
/*
 * In this method the particular SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST attribute will be
 * saved.
 */
bool
SP_XmlWriter::WriteColListAttribute( SP_DS_ColListAttribute* p_pcColList, wxXmlNode* p_pcRoot )
{
    wxXmlNode* l_pcCDATA = 0;

    wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("attribute"));
    l_pcElem->AddAttribute( wxT("name") ,p_pcColList->GetName());
    l_pcElem->AddAttribute( wxT("type"), wxT("ColList"));
    p_pcRoot->AddChild(l_pcElem );

    wxXmlNode* l_pcColListElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("colList")) ;
    l_pcColListElem->AddAttribute(wxT("row_count"), wxString::Format( wxT("%d"),  p_pcColList->GetRowCount()));
    l_pcColListElem->AddAttribute(wxT("col_count"), wxString::Format( wxT("%d"),  p_pcColList->GetColCount()));
    l_pcColListElem->AddAttribute(wxT("active_row"), wxString::Format( wxT("%d"),  p_pcColList->GetActiveList()));
    l_pcColListElem->AddAttribute(wxT("active_col"), wxString::Format( wxT("%d"),  p_pcColList->GetActiveColumn()));
    l_pcElem->AddChild(l_pcColListElem );

    wxXmlNode* l_pcColList_colLabelElem = 0;

    wxXmlNode* l_pcColList_rowElem = 0;
    wxXmlNode* l_pcColList_colElem = 0;

    // build table head

    wxXmlNode* l_pcColList_headElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("colList_head"));
    l_pcColListElem->AddChild(l_pcColList_headElem );

    for(unsigned int i = 0; i < p_pcColList->GetColCount(); i++ )
    {
      l_pcColList_colLabelElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("colList_colLabel"));
      l_pcCDATA = new wxXmlNode(NULL, wxXML_CDATA_SECTION_NODE, wxT(""), p_pcColList->GetColLabel(i));
      l_pcColList_colLabelElem->AddChild(l_pcCDATA );
      l_pcColList_headElem->AddChild(l_pcColList_colLabelElem );
    }

    // build table body

    wxXmlNode* l_pcColList_bodyElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("colList_body"));
    l_pcColListElem->AddChild(l_pcColList_bodyElem);

    // build table row

    for(unsigned int i = 0; i < p_pcColList->GetRowCount(); i++ )
    {
      l_pcColList_rowElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("colList_row"));
      l_pcColList_rowElem->AddAttribute(wxT("nr"), wxString::Format(wxT("%d"), i));

      for(unsigned int j = 0; j < p_pcColList->GetColCount(); j++ )
      {
        l_pcColList_colElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("colList_col"));
        l_pcColList_colElem->AddAttribute( wxT("nr"), wxString::Format( wxT("%d"), j));

        l_pcCDATA = new wxXmlNode(NULL, wxXML_CDATA_SECTION_NODE, wxT(""), p_pcColList->GetCell(i,j));

        l_pcColList_colElem->AddChild(l_pcCDATA);
        l_pcColList_rowElem->AddChild(l_pcColList_colElem);
      }

      l_pcColList_bodyElem->AddChild(l_pcColList_rowElem);
    }

    return WriteData( p_pcColList, l_pcElem );
}
