#include <wx/file.h>
#include "sp_gui/interaction/SP_IA_XmlWriter.h"
#include "snoopy.h"

SP_IA_XMLWriter::SP_IA_XMLWriter():
m_pcMgr(NULL)
{
        Init();
}

void
SP_IA_XMLWriter::Init()
{
}

SP_IA_XMLWriter::~SP_IA_XMLWriter()
{
}

bool
SP_IA_XMLWriter::Write(const SP_PairString &p_pcNets, const wxString& p_sFile)
{
    CHECK_BOOL((!p_sFile.IsEmpty()), return FALSE);

    m_pcMgr = wxGetApp().GetIAManager();
	m_pcNets = p_pcNets;

    wxXmlDocument* l_pcDoc = new wxXmlDocument();

    wxXmlNode* l_pcRootElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("configuration"));
    l_pcDoc->SetRoot(l_pcRootElem);

    WriteNets(l_pcRootElem);
	WriteConnection(l_pcRootElem, wxT("net_1"));
	WriteConnection(l_pcRootElem, wxT("net_2"));

    if(!l_pcDoc->Save(p_sFile))
    {
        m_sMessage << wxT("An error occurred creating the document") << wxT("\n");
        m_bError = TRUE;
    }
    wxDELETE(l_pcDoc);

    return !m_bError;
}

bool
SP_IA_XMLWriter::WriteNets(wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcRoot, return FALSE);

    wxXmlNode *l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("connected_nets"));
	p_pcRoot->AddChild(l_pcElem);

	wxXmlNode *l_pcElemNet = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("net_1"));
    if (!m_pcNets.first.StartsWith(wxT("unnamed")))
    {
		l_pcElemNet->AddAttribute(wxT("file"), m_pcNets.first);
    }
    l_pcElemNet->AddAttribute(wxT("netclass"), m_pcMgr->GetGraphFromFilename(m_pcNets.first)->GetNetclass()->GetName());
	l_pcElem->AddChild(l_pcElemNet);

	l_pcElemNet = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("net_2"));
    if (!m_pcNets.second.StartsWith(wxT("unnamed")))
    {
		l_pcElemNet->AddAttribute(wxT("file"), m_pcNets.second);
	}
    l_pcElemNet->AddAttribute(wxT("netclass"), m_pcMgr->GetGraphFromFilename(m_pcNets.second)->GetNetclass()->GetName());
	l_pcElem->AddChild(l_pcElemNet);

    return TRUE;
}


bool
SP_IA_XMLWriter::WriteConnection(wxXmlNode* p_pcRoot, const wxString& p_sSource)
{
    CHECK_POINTER(p_pcRoot, return FALSE);

    wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("connection"));
    l_pcElem->AddAttribute(wxT("source"), p_sSource);
	p_pcRoot->AddChild(l_pcElem);

	wxXmlNode *l_pcElem2;
	wxArrayString cmds;
	if (p_sSource == wxT("net_1"))
	{
		cmds = m_pcMgr->GetActiveCommandNames(m_pcNets.first, m_pcNets.second);
	}
	else
	{
		cmds = m_pcMgr->GetActiveCommandNames(m_pcNets.second, m_pcNets.first);
	}
	for (unsigned int i = 0; i < cmds.GetCount(); i++)
	{
		l_pcElem2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("command"));
		l_pcElem->AddChild(l_pcElem2);
		wxXmlNode* l_pcCData = new wxXmlNode(NULL, wxXML_CDATA_SECTION_NODE, wxT(""), cmds[i]);
		l_pcElem2->AddChild(l_pcCData);
	}

    return TRUE;
}
