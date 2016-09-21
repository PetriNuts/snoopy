//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: XML Readr, writes graph structures actually
//////////////////////////////////////////////////////////////////////
#include "sp_gui/interaction/SP_IA_XmlReader.h"
#include "sp_utilities.h"


static wxXmlNode*
GetNextChild(wxXmlNode* p_pcVal, wxXmlNodeType p_eType = wxXML_ELEMENT_NODE)
{
    if (!p_pcVal)
        return NULL;

    wxXmlNode* l_pcNode = p_pcVal->GetChildren();
    while (l_pcNode)
    {
        if (l_pcNode->GetType() == p_eType)
            return l_pcNode;

        l_pcNode = l_pcNode->GetNext();
    }
    return NULL;
}

static wxXmlNode*
GetNextSibling(wxXmlNode* p_pcVal, wxXmlNodeType p_eType = wxXML_ELEMENT_NODE)
{
    if (!p_pcVal)
        return NULL;

    wxXmlNode* l_pcNode = p_pcVal->GetNext();
    while (l_pcNode)
    {
        if (l_pcNode->GetType() == p_eType)
            return l_pcNode;

        l_pcNode = l_pcNode->GetNext();
    }
    return GetNextSibling(p_pcVal->GetNext());
}

static wxString
GetAttributeChar(wxXmlNode* p_pcNode, const wxString& p_pchAttr)
{
    if(!p_pcNode)
        return wxT("");

    wxXmlAttribute* l_pcAttributeNode = p_pcNode->GetAttributes();
    while(l_pcAttributeNode)
    {
    	if(l_pcAttributeNode->GetName() == p_pchAttr)
    	{
    		return l_pcAttributeNode->GetValue();
    	}
    	l_pcAttributeNode = l_pcAttributeNode->GetNext();
    }
    return wxT("");
}

static wxString
GetNodeCData(wxXmlNode* p_pcNode)
{
    if (!p_pcNode)
        return wxT("");

    // need the CDATA section
    wxXmlNode* l_pcCData = GetNextChild(p_pcNode, wxXML_CDATA_SECTION_NODE);
    if (l_pcCData)
    {
    	//TODO: bug in wxXML (is fixed in wx3.0), workaround
    	wxString l_sCData = l_pcCData->GetContent();
    	if(!(wxCHECK_VERSION(2,9,0)))
    	{
    		int l_nPos = l_sCData.Find(wxT('\n'), true);
    		if(l_nPos >= 0) l_sCData.Truncate(l_nPos);
    	}
        return l_sCData;
    }
    return wxT("");
}

SP_IA_XMLReader::SP_IA_XMLReader()
{
        Init();
}

void
SP_IA_XMLReader::Init()
{
}

SP_IA_XMLReader::~SP_IA_XMLReader()
{
}

bool
SP_IA_XMLReader::Read(const wxString& p_sFile)
{
    CHECK_BOOL((!p_sFile.IsEmpty()), return FALSE);

    wxXmlDocument l_pcDocument;
    if(l_pcDocument.Load(p_sFile, wxString(wxT("UTF-8")), wxXMLDOC_NONE))
    {
		wxXmlNode* l_pcRoot = l_pcDocument.GetRoot();

        CheckRoot(l_pcRoot);
        wxXmlNode* l_pcElem = GetNextChild(l_pcRoot);
        // the next child should be the connected nets
        ReadConnectedNets(l_pcElem);
        l_pcElem = GetNextSibling(l_pcElem);
        // the next child should be the first connection
        ReadConnection(l_pcElem);
        l_pcElem = GetNextSibling(l_pcElem);
        // the next child should be the second connection (if there is one at all)
        if (l_pcElem) ReadConnection(l_pcElem);
    }
    else
    {
    	m_sMessage << wxT("\nUnexpected exception during parsing: '") << p_sFile << wxT("'\n");
        SP_LOGERROR( m_sMessage);
    	m_bError = TRUE;
    }

    return !m_bError;
}

bool
SP_IA_XMLReader::CheckRoot(wxXmlNode* p_pcRoot)
{
    if (m_bError)   return FALSE;

    CHECK_POINTER(p_pcRoot, return FALSE);

    if (p_pcRoot->GetName() == wxT("configuration"))
    {
        m_sMessage << wxT("No configuration file.");
        m_bError = TRUE;
    }

    return !m_bError;
}

bool
SP_IA_XMLReader::ReadConnectedNets(wxXmlNode* p_pcNode)
{
    if (m_bError)   return FALSE;

    CHECK_POINTER(p_pcNode, return FALSE);

    if(p_pcNode->GetName() == wxT("connected_nets"))
    {
        m_sMessage << wxT("Couldn't find connected nets. Possibly wrong format.");
        m_bError = TRUE;
    }

    wxXmlNode *l_pcNode = GetNextChild(p_pcNode);
	//reading first net
	ReadNet(l_pcNode);
	//reading second net
	ReadNet(GetNextSibling(l_pcNode));

    return !m_bError;
}

bool
SP_IA_XMLReader::ReadNet(wxXmlNode* p_pcNetElem) {

  if (m_bError)   return FALSE;

	CHECK_POINTER(p_pcNetElem, return FALSE);

	if (p_pcNetElem->GetName() == wxT("net_1"))
	{
		wxString l_sNetclass = GetAttributeChar(p_pcNetElem, wxT("netclass"));
		wxString l_sFile = GetAttributeChar(p_pcNetElem, wxT("file"));
		m_pcNet1 = SP_PairString(l_sNetclass, l_sFile);
	}
	else if (p_pcNetElem->GetName() == wxT("net_2"))
	{
		wxString l_sNetclass = GetAttributeChar(p_pcNetElem, wxT("netclass"));
		wxString l_sFile = GetAttributeChar(p_pcNetElem, wxT("file"));
		m_pcNet2 = SP_PairString(l_sNetclass, l_sFile);
	}
	else
	{
		m_sMessage << wxT("No net. Possibly wrong format.");
		m_bError = TRUE;
	}

	return !m_bError;
}

bool
SP_IA_XMLReader::ReadConnection(wxXmlNode* p_pcConnectionElem)
{
    if (m_bError)   return FALSE;

    CHECK_POINTER(p_pcConnectionElem, return FALSE);

	wxString source = GetAttributeChar(p_pcConnectionElem, wxT("source"));
    if (source.Cmp(wxT("net_1")) == 0)
	{
		m_asCommands1.Clear();
	} else if (source.Cmp(wxT("net_2")) == 0)
	{
		m_asCommands2.Clear();
	} else
	{
		m_sMessage << wxT("Unknown source net.");
		m_bError = TRUE;
		return false;
	}

	wxXmlNode* l_pcCmdNode = GetNextChild(p_pcConnectionElem);

	while (l_pcCmdNode)
	{
		if (l_pcCmdNode->GetName() == wxT("command"))
		{
			wxString l_pchVal = GetNodeCData(l_pcCmdNode);
			if(!l_pchVal.IsEmpty())
			{
				if (source.Cmp(wxT("net_1")) == 0)
				{
					m_asCommands1.Add(l_pchVal);
				}
				else if (source.Cmp(wxT("net_2")) == 0)
				{
					m_asCommands2.Add(l_pchVal);
				}
			}
		}
		l_pcCmdNode = GetNextSibling(l_pcCmdNode);
    }
    return TRUE;
}


const SP_PairString&
SP_IA_XMLReader::GetNet(bool p_bNet1) {

	if (p_bNet1) return m_pcNet1;

	return m_pcNet2;
}


wxArrayString
SP_IA_XMLReader::GetCommandNames(bool p_bNet1) {

	if (p_bNet1) return m_asCommands1;

	return m_asCommands2;
}
