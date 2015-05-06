//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/12/17 11:55:00 $
// Short Description: XML Reader, reads graph structures
//////////////////////////////////////////////////////////////////////
#include <iostream>
#include <memory>
#include <wx/fs_zip.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

#include "sp_core/SP_XmlReader.h"
#include "sp_defines.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

static wxXmlNode*
GetNextChild(wxXmlNode* p_pcVal, wxXmlNodeType p_eType = wxXML_ELEMENT_NODE)
{
    if (!p_pcVal)
        return NULL;

    for(wxXmlNode* l_pcNode = p_pcVal->GetChildren();
    		l_pcNode;
    		l_pcNode = l_pcNode->GetNext())
    {
        if (l_pcNode->GetType() == p_eType)
            return l_pcNode;
    }
    return NULL;
}

static wxXmlNode*
GetNextSibling(wxXmlNode* p_pcVal, wxXmlNodeType p_eType = wxXML_ELEMENT_NODE)
{
    if (!p_pcVal)
        return NULL;

    for(wxXmlNode* l_pcNode = p_pcVal->GetNext();
    		l_pcNode;
    		l_pcNode = l_pcNode->GetNext())
    {
        if (l_pcNode->GetType() == p_eType)
            return l_pcNode;
    }
    return GetNextSibling(p_pcVal->GetNext());
}

static wxString
GetAttributeChar(wxXmlNode* p_pcNode, const wxString& p_pchAttr)
{
    if(!p_pcNode)
        return wxT("");

    for(wxXmlAttribute* l_pcAttributeNode = p_pcNode->GetAttributes();
    		l_pcAttributeNode;
    		l_pcAttributeNode = l_pcAttributeNode->GetNext())
    {
    	if(l_pcAttributeNode->GetName() == p_pchAttr)
    	{
    		return l_pcAttributeNode->GetValue();
    	}
    }
    return wxT("");
}

//

static void
SetAttributeChar(wxXmlNode* p_pcNode, const wxString& p_pchAttr,const wxString& p_pchValue)
{
    if(!p_pcNode)
        return ;

    for(wxXmlAttribute* l_pcAttributeNode = p_pcNode->GetAttributes();
    		l_pcAttributeNode;
    		l_pcAttributeNode = l_pcAttributeNode->GetNext())
    {
    	if(l_pcAttributeNode->GetName() == p_pchAttr)
    	{
    		l_pcAttributeNode->SetValue(p_pchValue);
    		break;
    	}
    }
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

static bool
HasAttribute(wxXmlNode* p_pcNode, const wxString& p_pchAttr)
{
    if (!p_pcNode)
        return FALSE;

    for(wxXmlAttribute* l_pcAttributeNode = p_pcNode->GetAttributes();
    		l_pcAttributeNode;
    		l_pcAttributeNode = l_pcAttributeNode->GetNext())
    {
    	if(l_pcAttributeNode->GetName() == p_pchAttr)
    	{
    		return true;
    	}
    }
    return FALSE;
}

static bool
GetColourAttribute(wxXmlNode* p_pcNode, const wxString& p_pchAttr, unsigned char* p_pchR, unsigned char* p_pchG, unsigned char* p_pchB)
{
    if (!p_pcNode || !p_pchAttr || !p_pchR || !p_pchG || !p_pchB)
        return FALSE;

    wxString l_sVal(GetAttributeChar(p_pcNode, p_pchAttr));
    long l_nTemp;
    if (l_sVal.IsEmpty() || !l_sVal.Matches(wxT("?*,?*,?*")))
        return FALSE;

    wxString l_sTemp = l_sVal.BeforeFirst(',');
    l_sVal = l_sVal.AfterFirst(',');
    if (!l_sTemp.ToLong(&l_nTemp))
        return FALSE;
    *p_pchR = l_nTemp;

    l_sTemp = l_sVal.BeforeFirst(',');
    l_sVal = l_sVal.AfterFirst(',');
    if (!l_sTemp.ToLong(&l_nTemp))
        return FALSE;
    *p_pchG = l_nTemp;

    if (!l_sVal.ToLong(&l_nTemp))
        return FALSE;
    *p_pchB = l_nTemp;

    return TRUE;
}

static unsigned int
GetAttributeUInt(wxXmlNode* p_pcNode, const wxString& p_pchAttr)
{
    if (!p_pcNode)
        return 0;

    long l_nRet = 0;
    for(wxXmlAttribute* l_pcAttributeNode = p_pcNode->GetAttributes();
    		l_pcAttributeNode;
    		l_pcAttributeNode = l_pcAttributeNode->GetNext())
    {
    	if(l_pcAttributeNode->GetName() == p_pchAttr)
    	{
    		wxString l_sV(l_pcAttributeNode->GetValue());
			if (!l_sV.ToLong(&l_nRet))
				return 0;
			return l_nRet;
    	}
    }
    return l_nRet;
}

static unsigned long
GetAttributeLong(wxXmlNode* p_pcNode, const wxString& p_pchAttr)
{
    if (!p_pcNode)
        return 0;

    unsigned long l_nRet = 0;
    for(wxXmlAttribute* l_pcAttributeNode = p_pcNode->GetAttributes();
    		l_pcAttributeNode;
    		l_pcAttributeNode = l_pcAttributeNode->GetNext())
    {
    	if(l_pcAttributeNode->GetName() == p_pchAttr)
    	{
    		wxString l_sV(l_pcAttributeNode->GetValue());
			if (!l_sV.ToULong(&l_nRet))
				return 0;
			return l_nRet;
    	}
    }
    return l_nRet;
}

static double
GetAttributeDouble(wxXmlNode* p_pcNode, const wxString& p_pchAttr)
{
    if (!p_pcNode)
        return 0.0;

    double l_nRet = 0.0;
    for(wxXmlAttribute* l_pcAttributeNode = p_pcNode->GetAttributes();
    		l_pcAttributeNode;
    		l_pcAttributeNode = l_pcAttributeNode->GetNext())
    {
    	if(l_pcAttributeNode->GetName() == p_pchAttr)
    	{
    		wxString l_sV(l_pcAttributeNode->GetValue());
			if (!l_sV.ToDouble(&l_nRet))
				return 0.0;
			return l_nRet;
    	}
    }
    return l_nRet;
}

static bool
GetAttributeBool(wxXmlNode* p_pcNode, const wxString& p_pchAttr)
{
    if (!p_pcNode)
        return FALSE;

    for(wxXmlAttribute* l_pcAttributeNode = p_pcNode->GetAttributes();
    		l_pcAttributeNode;
    		l_pcAttributeNode = l_pcAttributeNode->GetNext())
    {
    	if(l_pcAttributeNode->GetName() == p_pchAttr)
    	{
    		return (l_pcAttributeNode->GetValue() == wxT("1"));
    	}
    }
    return FALSE;
}

static SP_GRAPHIC_STATE
GetAttributeGraphicState(wxXmlNode* p_pcNode, const wxString& p_pchAttr)
{
    if (!p_pcNode)
        return SP_STATE_NORMAL;

    SP_GRAPHIC_STATE l_nRet = SP_STATE_NORMAL;
    long l_nTemp;
    for(wxXmlAttribute* l_pcAttributeNode = p_pcNode->GetAttributes();
    		l_pcAttributeNode;
    		l_pcAttributeNode = l_pcAttributeNode->GetNext())
    {
    	if(l_pcAttributeNode->GetName() == p_pchAttr)
    	{
    		wxString l_sV(l_pcAttributeNode->GetValue());
			if (l_sV.ToLong(&l_nTemp))
			{
				return (SP_GRAPHIC_STATE)l_nTemp;
			}
    	}
    }
    return l_nRet;
}

SP_XmlReader::SP_XmlReader()
{
        Init();
}

void
SP_XmlReader::Init()
{
}

SP_XmlReader::~SP_XmlReader()
{
}

bool
SP_XmlReader::Read(SP_DS_Graph* p_pcGraph, const wxString& p_sFile)
{
    wxFileSystem fs;
    std::shared_ptr<wxFSFile> f(fs.OpenFile(p_sFile + wxT("#zip:model.xml")));
    if (f)
    {
    	p_pcGraph->GetParentDoc()->SetCompress(true);
    }
    else
    {
    	f.reset(fs.OpenFile(p_sFile));
    	p_pcGraph->GetParentDoc()->SetCompress(false);
    }

	if(f->GetStream()->IsOk())
		return Read(p_pcGraph, *(f->GetStream()));
	else
		return false;
}

bool
SP_XmlReader::Read(SP_DS_Graph* p_pcGraph, wxInputStream& p_InputStream)
{
    CHECK_POINTER(p_pcGraph, return FALSE);
    CHECK_BOOL((p_InputStream.IsOk()), return FALSE);

    wxStopWatch l_StopWatch;
    wxXmlDocument l_pcDocument;

    if(l_pcDocument.Load(p_InputStream, wxString(wxT("UTF-8")), wxXMLDOC_NONE))
    {
    	// remove all initial elements
    	RemoveInitialElements(p_pcGraph);

		wxXmlNode* l_pcRoot = l_pcDocument.GetRoot();

		CHECK_BOOL(CheckRoot(l_pcRoot), return false)

		wxXmlNode* l_pcElem = GetNextChild(l_pcRoot);

		// the next child should be the netclass
		ReadNetclass(p_pcGraph->GetNetclass(), l_pcElem);

		while((l_pcElem = GetNextSibling(l_pcElem)))
		{
		  // the first child schould be wxT("nodeclasses")
		  // the second child schould be wxT("edgeclasses")
		  // the third child schould be wxT("metadataclasses")
		  ReadClasses(p_pcGraph, l_pcElem);
		}

		p_pcGraph->ResetCoarseTree();

        long l_nDuration = l_StopWatch.Time();
        if (!m_bError)
        {
            // Print out the stats that we collected and time taken.
            m_sMessage << wxT("File loaded in ") << l_nDuration << wxT(" ms\n");
        }
    }
    else
    {
    	m_sMessage << wxT("\nUnexpected exception during parsing\n");
        SP_LOGERROR( m_sMessage);
    	m_bError = TRUE;
    }

    return !m_bError;
}

bool
SP_XmlReader::CheckRoot(wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcRoot, return FALSE);

    if (p_pcRoot->GetName() != wxT("Snoopy"))
    {
        m_sMessage << wxT("No Snoopy net!");
        m_bError = TRUE;
    }
    //TODO: adapt this method each time the snoopy file format changes
    unsigned l_nFileVersion = GetAttributeUInt(p_pcRoot, wxT("version"));
    // we only support Snoopy files with version >= 2
    if (l_nFileVersion < 2)
    {
        m_sMessage << wxT("Snoopy version ") << l_nFileVersion << wxT(" is not supported anymore!");
        m_bError = TRUE;
    }
    double l_nFileRevision = GetAttributeDouble(p_pcRoot, wxT("revision"));
    double l_nSnoopyRevision;
    SP_APP_REVISION.ToDouble(&l_nSnoopyRevision);
    if(l_nSnoopyRevision < l_nFileRevision)
    {
    	wxString l_sMsg = wxT("You are trying to load a file that is newer than your Snoopy!\n\n");
    	l_sMsg << wxT("This is not supposed to work!\n\n");
    	l_sMsg << wxT("Tool: Snoopy ") << SP_APP_VERSION << wxT(" Version: ") << SP_APP_REVISION << wxT("\n");
    	l_sMsg << wxT("File: Snoopy ") << l_nFileVersion << wxT(" Version: ") << l_nFileRevision << wxT("\n");
    	int l_Answer = SP_MESSAGEBOX(l_sMsg + wxT("\nDo you want to continue?"), wxT("Question..."), wxYES_NO);
    	if(l_Answer != wxYES)
    	{
            m_sMessage << l_sMsg;
            m_bError = TRUE;
    	}
    }
    else if (l_nFileRevision < l_nSnoopyRevision)
    {
    	wxString l_sMsg = wxT("You are trying to load a file that is older than your Snoopy!\n");
    	l_sMsg << wxT("This may result in some conversions.\n");
    	l_sMsg << wxT("Please check the loaded file and save it.\n\n");
    	l_sMsg << wxT("Tool: Snoopy ") << SP_APP_VERSION << wxT(" Version: ") << SP_APP_REVISION << wxT("\n");
    	l_sMsg << wxT("File: Snoopy ") << l_nFileVersion << wxT(" Version: ") << l_nFileRevision << wxT("\n");
    	int l_Answer = SP_MESSAGEBOX(l_sMsg + wxT("\nDo you want to continue?"), wxT("Question..."), wxYES_NO);
    	if(l_Answer != wxYES)
    	{
            m_sMessage << l_sMsg;
            m_bError = TRUE;
    	}
    }


    return !m_bError;
}

bool
SP_XmlReader::ReadNetclass(SP_DS_Netclass* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    if (p_pcRoot->GetName() != wxT("netclass"))
    {
        m_sMessage << wxT("No netclass.");
        m_bError = TRUE;
    }

    wxString l_pchVal = GetAttributeChar(p_pcRoot, wxT("name"));
    if(l_pchVal != p_pcVal->GetName())
    {
        m_sMessage << wxT("Wrong netclass.");
		m_bError = TRUE;
    }

    return !m_bError;
}

bool
SP_XmlReader::ReadClasses(SP_DS_Graph* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    //Get the Net class Name for dealing with old continuous petri net files
    wxString l_sNetClassName=p_pcVal->GetNetclass()->GetName();
    bool l_bMetaDataIgnore=false;
    wxXmlNode* l_pcNode = GetNextChild(p_pcRoot);
    while (l_pcNode)
    {
	    wxString l_pchName = GetAttributeChar( l_pcNode, wxT("name") );

	    //a special code for dealing with old continuous class files
    	if(l_sNetClassName==SP_DS_CONTINUOUSPED_CLASS || l_sNetClassName==SP_DS_COLCPN_CLASS)
    	{
    	    /*
    	     * if the old parameter name (Parameter, Continuous), rename it with the new name (Parameter)
    	     */
    	   if(l_pchName== wxT("Parameter, Continuous"))
    	   {
    		   SetAttributeChar( l_pcNode, wxT("name"),SP_DS_PARAM);
    		   l_bMetaDataIgnore=true;
    	   }

    	   //Read old coarse transition names
    	   else if(l_pchName== wxT("Coarse Transition, Continuous"))
		   {
			   SetAttributeChar( l_pcNode, wxT("name"),SP_DS_COARSE_TRANSITION);
			   l_bMetaDataIgnore=true;
		   }
    	   //Read old coarse place names
    	   else if(l_pchName== wxT("Coarse Place, Continuous"))
		   {
			   SetAttributeChar( l_pcNode, wxT("name"),SP_DS_COARSE_PLACE);
			   l_bMetaDataIgnore=true;
		   }
    	   //Read old coarse Parameter names
    	   else if(l_pchName== wxT("Coarse Parameter, Continuous"))
    	   {
    		   SetAttributeChar( l_pcNode, wxT("name"),SP_DS_COARSE_PARAMETER);
    		   l_bMetaDataIgnore=true;
    	   }

    	   //Read old Edge names
    	   else if(l_pchName== wxT("Edge, Continuous"))
		   {
			   SetAttributeChar( l_pcNode, wxT("name"),SP_DS_EDGE);
			   l_bMetaDataIgnore=true;
		   }

    	   //Read old inhibitor Edge names
    	   else if(l_pchName== wxT("Inhibitor Edge, Continuous"))
		   {
			   SetAttributeChar( l_pcNode, wxT("name"),SP_DS_INHIBITOR_EDGE);
			   l_bMetaDataIgnore=true;
		   }
    	   //Read old Read Edge names
    	   else if(l_pchName== wxT("Test Edge, Continuous"))
		   {
			   SetAttributeChar( l_pcNode, wxT("name"),SP_DS_READ_EDGE);
			   l_bMetaDataIgnore=true;
		   }
    	   /*
    	    * if it is Inhibitors parameter edge or test parameter edge, then those no longer exist in the new class.
    	    */
    	   if(l_pchName== wxT("Parameter Inhibitor Edge, Continuous") ||l_pchName== wxT("Parameter Test Edge, Continuous"))
    	   {
    		   l_pcNode = GetNextSibling(l_pcNode);

    		   //then it is old class file
    		   l_bMetaDataIgnore=true;
    		   continue;
    	   }

    	   if(l_bMetaDataIgnore==true && (l_pchName== wxT("Plot") || l_pchName== wxT("Table")))
    	   {
    		   l_pcNode = GetNextSibling(l_pcNode);
    		   continue;
    	   }

    	   /*
    	    * in the new CPN class comment belongs to Metadata not to the node class
    	    * so we should take care of this transformation
    	    */
    	}
    	else     //a special code for dealing with old hybrid class files
        	if(l_sNetClassName==SP_DS_HYBRIDPN_CLASS || l_sNetClassName==SP_DS_COLHPN_CLASS)
		{
			/*
			 * if the old "Self-modifying arc", rename it with the "Edge"
			 */
		   if(l_pchName== wxT("Self-modifying arc"))
		   {
			   SetAttributeChar( l_pcNode, wxT("name"), SP_DS_EDGE);
		   }
		}

    	if( (l_pchName == wxT("Comment") || l_pchName == wxT("Plot") || l_pchName == wxT("Table"))
    			&& l_pcNode->GetName() != wxT("metadataclass") )
		{
		   l_pcNode->SetName(wxT("metadataclass"));
		}

		if (l_pcNode->GetName() == wxT("nodeclass"))
        {
			wxString l_pchVal = GetAttributeChar(l_pcNode, wxT("name"));
            //load parameter as constants for qpn,xpn,cpn,spn,hpn
            if (l_pchVal == wxT("Parameter") && (!l_sNetClassName.Contains(wxT("Colored")))  )
            {
            	ReadOldNodeClass(p_pcVal->GetMetadataclass(SP_DS_META_CONSTANT), l_pcNode);
			}
            else
            {
                ReadNodeclass( p_pcVal->GetNodeclass(l_pchVal), l_pcNode );
            }
        }
        else if (l_pcNode->GetName() == wxT("edgeclass"))
        {
            wxString l_pchVal = GetAttributeChar(l_pcNode, wxT("name"));
			ReadEdgeclass(p_pcVal->GetEdgeclass(l_pchVal), l_pcNode);
        }
        else if (l_pcNode->GetName() == wxT("metadataclass"))
        {
            wxString l_pchVal = GetAttributeChar(l_pcNode, wxT("name"));
			ReadMetadataclass(p_pcVal->GetMetadataclass(l_pchVal), l_pcNode);
        }
        l_pcNode = GetNextSibling(l_pcNode);
    }
    return TRUE;
}

bool
SP_XmlReader::ReadNodeclass(SP_DS_Nodeclass* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);
    CHECK_POINTER(p_pcVal->GetPrototype(), return FALSE);

	
    SP_DS_Node* l_pcSpNode;
    wxXmlNode* l_pcNode = GetNextChild(p_pcRoot);
    while (l_pcNode)
    {
    	if (l_pcNode->GetName() == wxT("node"))
        {
			// new node
			l_pcSpNode = p_pcVal->GetPrototype()->Clone();
			p_pcVal->AddElement(l_pcSpNode);

			ReadNode(l_pcSpNode, l_pcNode);
		}


        l_pcNode = GetNextSibling(l_pcNode);

    }

    return TRUE;
}

bool
SP_XmlReader::ReadEdgeclass(SP_DS_Edgeclass* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    SP_DS_Edge* l_pcEdge;
    wxXmlNode* l_pcNode = GetNextChild(p_pcRoot);
    while (l_pcNode)
    {
        if (l_pcNode->GetName() == wxT("edge"))
        {
            // new edge
            l_pcEdge = p_pcVal->GetPrototype()->Clone();
            if (ReadEdge(l_pcEdge, l_pcNode))
            {
            	p_pcVal->AddElement(l_pcEdge);
            }
            else
            {
            	wxDELETE(l_pcEdge);
            }
        }
        l_pcNode = GetNextSibling(l_pcNode);
    }

    return TRUE;
}

bool
SP_XmlReader::ReadMetadataclass(SP_DS_Metadataclass* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);
    CHECK_POINTER(p_pcVal->GetPrototype(), return FALSE);

    SP_DS_Metadata* l_pcSpMetadata;
    wxXmlNode* l_pcNode = GetNextChild(p_pcRoot);
    while (l_pcNode)
    {
        if (l_pcNode->GetName() == wxT("metadata") ||
        	// read old metadata from nodes
        	l_pcNode->GetName() == wxT("node"))
        {
            // new metadata
        	l_pcSpMetadata = p_pcVal->GetPrototype()->Clone();
        	p_pcVal->AddElement(l_pcSpMetadata);
            ReadMetadata(l_pcSpMetadata, l_pcNode);
        }
        l_pcNode = GetNextSibling(l_pcNode);
    }

    return TRUE;
}

bool
SP_XmlReader::ReadNode(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);
    wxString l_sNcName=p_pcVal->GetNodeclass()->GetName();
    m_bError = !ReadData(p_pcVal, p_pcRoot);


    //holds the node name
    wxString l_sValue = wxT("");
    for(wxXmlNode* node = GetNextChild(p_pcRoot); node; node = GetNextSibling(node))
    {
		if ( node->GetName() == wxT("attribute"))
		{
			// new attribute
			wxString l_pchName = GetAttributeChar( node, wxT("name") );
			if( l_pchName == wxT("Name"))
			{
				l_sValue = wxT("n_") + GetNodeCData(node);
			}
		}
    }

    wxString l_sNetClass = p_pcVal->GetNodeclass()->GetParentGraph()->GetNetclass()->GetName();

    wxXmlNode* l_pcNode = GetNextChild(p_pcRoot);
    while (l_pcNode)
    {

      if ( l_pcNode->GetName() == wxT("attribute"))
      {
            // new attribute
            wxString l_pchName = GetAttributeChar( l_pcNode, wxT("name") );
            wxString l_pchType = GetAttributeChar( l_pcNode, wxT("type") );

            //attribute are colList
            if( l_pchType == wxT("ColList") )
			{
            	SP_DS_ColListAttribute* l_pcAttr = dynamic_cast< SP_DS_ColListAttribute* >( p_pcVal->GetAttribute( l_pchName ) );
            	if (!l_sNetClass.Contains(wxT("Colored")))
				{
					if ((l_sNcName == SP_DS_DISCRETE_PLACE || l_sNcName == SP_DS_CONTINUOUS_PLACE)
							&& l_pchName == wxT("MarkingList"))
					{
						//read values as new contants
						if(ReadNewConstant(p_pcVal, l_sValue, l_pcNode))
						{
							//set the name as marking
							SP_DS_Attribute* l_pcAttr = p_pcVal->GetAttribute(wxT("Marking"));
							l_pcAttr->SetValueString(l_sValue, false);
						}
					}
					else
					{
						//read other collist
						ReadColListAttribute( l_pcAttr, l_pcNode );
					}

				}
            	else
            	{
					ReadColListAttribute( l_pcAttr, l_pcNode );
				}
           	}
			else
			{
				bool l_bOldCPN=false;

				//if(p_pcVal->GetNodeclass()->GetParentGraph()->GetNetclass()->GetName()==SP_DS_CONTINUOUSPED_CLASS)
				if(l_sNetClass == SP_DS_CONTINUOUSPED_CLASS)
				{
					//A special code for dealing with old CPN's version
					
					/*
					if(l_sNcName==SP_DS_CONTINUOUS_PLACE && l_pchName==wxT("Marking"))//place
					{
						l_pchName=wxT("MarkingList");
						l_bOldCPN=true;
					}
					else */
						if(l_sNcName==SP_DS_PARAM && l_pchName==wxT("Marking"))//parameter
					 {
						  l_pchName=wxT("ParameterList");
						  l_bOldCPN=true;

					 }
					else if(l_sNcName==SP_DS_CONTINUOUS_TRANS && l_pchName==wxT("Equation"))//transition
					  {
					 		l_pchName=wxT("FunctionList");
					 		l_bOldCPN=true;

					  }
				}
				//then read the attribute
				SP_DS_Attribute* l_pcAttr = p_pcVal->GetAttribute(l_pchName);
				ReadAttribute(l_pcAttr, l_pcNode,l_bOldCPN);
			}
        }

      l_pcNode = GetNextSibling(l_pcNode);

    }




    return !m_bError;
}

bool
SP_XmlReader::ReadEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    m_bError = !ReadEdgeData(p_pcVal, p_pcRoot);

    wxXmlNode* l_pcNode = GetNextChild(p_pcRoot);
    while (l_pcNode)
    {
        if (l_pcNode->GetName() == wxT("attribute"))
        {

            // new attribute
            // new attribute
            wxString l_pchName = GetAttributeChar( l_pcNode, wxT("name") );
            wxString l_pchType = GetAttributeChar( l_pcNode, wxT("type") );

			if( l_pchType == wxT("ColList") )
			{
				SP_DS_ColListAttribute* l_pcAttr = dynamic_cast< SP_DS_ColListAttribute* >( p_pcVal->GetAttribute( l_pchName ) );
				ReadColListAttribute( l_pcAttr, l_pcNode );
			}
			else
			{
				//Old transformation of continuous Petri Nets
				if(l_pchName==wxT("Equation"))
				{
					l_pchName=wxT("Multiplicity");
				}

				SP_DS_Attribute* l_pcAttr = p_pcVal->GetAttribute(l_pchName);
				ReadAttribute(l_pcAttr, l_pcNode);
			}
        }

        l_pcNode = GetNextSibling(l_pcNode);

    }

    // source and target
	if (!GetDataById(p_pcRoot, wxT("source")))
	{
		if (HasAttribute(p_pcRoot, wxT("id")))
		{
			SP_LOGDEBUG(wxString::Format(wxT("ignored edge id %ld without source"), GetAttributeLong(p_pcRoot, wxT("id"))));
		}
		else
		{
			SP_LOGDEBUG(wxT("ignored edge without source"));
		}
		return false;
	}
	if (!GetDataById(p_pcRoot, wxT("target")))
	{
		if (HasAttribute(p_pcRoot, wxT("id")))
		{
			SP_LOGDEBUG(wxString::Format(wxT("ignored edge id %ld without target"), GetAttributeLong(p_pcRoot, wxT("id"))));
		}
		else
		{
			SP_LOGDEBUG(wxT("ignored edge without target"));
		}
		return false;
	}
	p_pcVal->SetNodes(GetDataById(p_pcRoot, wxT("source")), GetDataById(p_pcRoot, wxT("target")));

    return !m_bError;
}

bool
SP_XmlReader::ReadMetadata(SP_DS_Metadata* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    m_bError = !ReadData(p_pcVal, p_pcRoot);

    wxXmlNode* l_pcNode = GetNextChild(p_pcRoot);
    while (l_pcNode)
    {
      if ( l_pcNode->GetName() == wxT("attribute"))
      {
            // new attribute
            wxString l_pchName = GetAttributeChar( l_pcNode, wxT("name") );
            wxString l_pchType = GetAttributeChar( l_pcNode, wxT("type") );

			if( l_pchType == wxT("ColList"))
			{
				SP_DS_ColListAttribute* l_pcAttr = dynamic_cast< SP_DS_ColListAttribute* >( p_pcVal->GetAttribute( l_pchName ) );
				ReadColListAttribute( l_pcAttr, l_pcNode );

			}
			else
			{
				SP_DS_Attribute* l_pcAttr = p_pcVal->GetAttribute(l_pchName);
				ReadAttribute(l_pcAttr, l_pcNode);
			}
        }

      l_pcNode = GetNextSibling(l_pcNode);

    }

    return !m_bError;
}

SP_Data*
SP_XmlReader::GetDataById(wxXmlNode* p_pcRoot, const wxString& p_pchAttr)
{
    CHECK_POINTER(p_pcRoot, return FALSE);

    unsigned long l_nId = GetAttributeLong(p_pcRoot, p_pchAttr);
    if(l_nId == 0)
    	return NULL;

    map<unsigned long, SP_Data*>::iterator l_It = m_mId2Data.find(l_nId);
    if (l_It != m_mId2Data.end())
        return l_It->second;

    return NULL;
}

SP_Graphic*
SP_XmlReader::GetGraphicById(wxXmlNode* p_pcRoot, const wxString& p_pchAttr)
{
    CHECK_POINTER(p_pcRoot, return FALSE);

    unsigned long l_nId = GetAttributeLong(p_pcRoot, p_pchAttr);
    if(l_nId == 0)
    	return NULL;

    map<unsigned long, SP_Graphic*>::iterator l_It = m_mId2Graphic.find(l_nId);
    if ( l_It != m_mId2Graphic.end())
        return l_It->second;

    return NULL;
}

bool
SP_XmlReader::ReadAttribute(SP_DS_Attribute* p_pcVal, wxXmlNode* p_pcRoot,bool p_bOldCPN)
{

	if (!p_pcVal)
		return TRUE;

	//SNOOPy crashed when  an attribute is not there, but it is expected
	//CHECK_POINTER doesnt work
	// Todo: look what is going on in CHECK_POINTER and why snooppy gives
	//an error message when an attribute is not there
	//Work arround: the if construction above
	//original code:
    //CHECK_POINTER(p_pcVal, return TRUE);

	CHECK_POINTER(p_pcRoot, return FALSE);

    // need the CDATA section
    wxString l_pchTmp = GetNodeCData(p_pcRoot);
    p_pcVal->SetValueString(l_pchTmp, false);

	//A special code for dealing with old CPN's version
   if(p_bOldCPN)
	   TransformOldCPN(p_pcVal,l_pchTmp);

   return ReadData(p_pcVal, p_pcRoot);
}

void
SP_XmlReader::TransformOldCPN(SP_DS_Attribute* p_pcVal,wxString p_sVal)
{
	wxString l_sRemender,l_sNumber;
	unsigned int l_nRowIndex=0;
	wxString l_sParam1,l_sParam2,l_sParam3;
	wxString l_sEquation;
	SP_DS_ColListAttribute *l_pcColList;
	l_pcColList=dynamic_cast<SP_DS_ColListAttribute*>(p_pcVal);
		if(p_pcVal->GetName()==wxT("MarkingList")||p_pcVal->GetName()==wxT("ParameterList"))//place or Parameter transformation
		{
			l_sRemender=p_sVal.AfterFirst(wxT('#'));
			l_sNumber=l_sRemender.BeforeFirst(wxT(';'));
			if(l_sNumber!=wxT(""))
				l_pcColList->SetCell(l_nRowIndex++,1,l_sNumber);
			while(l_sNumber!=wxT(""))
			{
				l_sRemender=l_sRemender.AfterFirst(wxT(';'));
				l_sNumber=l_sRemender.BeforeFirst(wxT(';'));
				if(l_pcColList->GetRowCount()<=l_nRowIndex && l_sNumber!=wxT(""))
				{
					l_pcColList->AppendEmptyRow();
					l_pcColList->SetCell(l_nRowIndex, 0, wxT("Set"));
				}
				if(l_sNumber!=wxT(""))
				{
				 l_pcColList->SetCell(l_nRowIndex++, 1,l_sNumber);
				}
			}

		}
		else
			//transition transformation
			if(p_pcVal->GetName()==wxT("FunctionList"))
			{
				l_sRemender=p_sVal.AfterFirst(wxT('#'));
				l_sNumber=l_sRemender.BeforeFirst(wxT(';'));
				unsigned long l_nFunctionType;
				l_sNumber.ToULong(&l_nFunctionType);
				l_sRemender=l_sRemender.AfterFirst(wxT(';'));
				switch(l_nFunctionType)
				{
				 case 0://no Pattern. this is equation
					   l_sParam1=l_sRemender.BeforeFirst(wxT(';'));
					   l_pcColList->SetCell(0,1,l_sParam1);
					   break;
				 case 1://Michaelis-Menten-Equation 3-Param
					   l_sParam1=l_sRemender.BeforeFirst(wxT(';'));
					   l_sRemender=l_sRemender.AfterFirst(wxT(';'));
					   l_sParam2=l_sRemender.BeforeFirst(wxT(';'));
					   l_sRemender=l_sRemender.After(wxT(';'));
					   l_sParam3=l_sRemender;
					   l_sEquation=wxT("MichaelisMenten(")+l_sParam1+wxT(",")+l_sParam3+wxT(")");
					   l_pcColList->SetCell(0,1,l_sEquation);
				 	   break;
				 case 2://Mass Action 2-Param
					  l_sParam1=l_sRemender.BeforeFirst(wxT(';'));
					  l_sEquation=wxT("MassAction(")+l_sParam1+wxT(")");
					  l_pcColList->SetCell(0,1,l_sEquation);
				 	   break;
				 default:
				      SP_MESSAGEBOX(wxT("Error reading the source file"));
				      break;


				}
			}
}
bool
SP_XmlReader::ReadData(SP_Data* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    unsigned int l_nNet = GetAttributeUInt(p_pcRoot, wxT("net"));
    if (l_nNet <= 0)
    {
    	l_nNet = 1;
    }
    p_pcVal->SetNetnumber(l_nNet);
    if (HasAttribute(p_pcRoot, wxT("id")))
        p_pcVal->SetId(GetAttributeLong(p_pcRoot, wxT("id")));
    p_pcVal->SetLogical(GetAttributeBool(p_pcRoot, wxT("logic")));

    if (p_pcVal->GetCoarse())
    {
        unsigned int l_nNr = GetAttributeUInt(p_pcRoot, wxT("coarse"));
        if (l_nNr == 0)
        {
            m_sMessage << wxT("Coarsed Data read with no attribute \"coarse\" or attribute \"coarse\" set to \"0\"!\n");
            m_bError = TRUE;
        }
        p_pcVal->GetCoarse()->SetNetnumber(l_nNr);
    }

    FillInMap(p_pcVal);

    for(wxXmlNode* node = GetNextChild(p_pcRoot); node; node = GetNextSibling(node))
    {
        if (node->GetName() == wxT("graphics"))
        {
            // new graphic
            return ReadGraphics(p_pcVal, node);
        }
    }

    return TRUE;
}

bool
SP_XmlReader::ReadEdgeData(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    unsigned int l_nNet = GetAttributeUInt(p_pcRoot, wxT("net"));
    if (l_nNet <= 0)
    {
    	l_nNet = 1;
    }
    p_pcVal->SetNetnumber(l_nNet);
    if (HasAttribute(p_pcRoot, wxT("id")))
        p_pcVal->SetId(GetAttributeLong(p_pcRoot, wxT("id")));
    p_pcVal->SetLogical(GetAttributeBool(p_pcRoot, wxT("logic")));

    wxXmlNode* l_pcNode = GetNextChild(p_pcRoot);
    while (l_pcNode)
    {
        if (l_pcNode->GetName() == wxT("graphics"))
        {
            // new graphic
            return ReadEdgeGraphics(p_pcVal, l_pcNode);
        }
        l_pcNode = GetNextSibling(l_pcNode);
    }

    return TRUE;
}

bool
SP_XmlReader::ReadGraphics(SP_Data* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    unsigned int l_nGrCount = 0;

    for(wxXmlNode* node = GetNextChild(p_pcRoot); node; node = GetNextSibling(node))
    {
        if (node->GetName() == wxT("graphic"))
        {			
			if(!p_pcVal->GetGraphics()->empty())
        	{
				// new graphic
				SP_Graphic* l_pcNewGr = *(p_pcVal->GetGraphics()->begin());	
				if(l_nGrCount > 0)
        		{
        			l_pcNewGr = p_pcVal->AddGraphic(l_pcNewGr->Clone(p_pcVal));
        		}
				ReadGraphic(l_pcNewGr, node);
				++l_nGrCount;
			}            
        }
    }	

    return TRUE;
}

bool
SP_XmlReader::ReadEdgeGraphics(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    wxXmlNode* l_pcNode = GetNextChild(p_pcRoot);
    unsigned int l_nGrCount = 0;
	while (l_pcNode)
	{
		if (l_pcNode->GetName() == wxT("graphic"))
		{
			// new graphic
        	SP_Graphic* l_pcNewGr = *(p_pcVal->GetGraphics()->begin());
        	if(l_nGrCount > 0)
        	{
        		l_pcNewGr = p_pcVal->AddGraphic(l_pcNewGr->Clone(p_pcVal));
        	}
 			ReadEdgeGraphic(dynamic_cast<SP_GR_Edge*>(l_pcNewGr), l_pcNode);
            l_nGrCount++;
		}
		l_pcNode = GetNextSibling(l_pcNode);
	}

    return TRUE;
}

bool
SP_XmlReader::ReadGraphic(SP_Graphic* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    unsigned int l_nNet = GetAttributeUInt(p_pcRoot, wxT("net"));
    if (l_nNet <= 0)
    {
    	l_nNet = 1;
    }
    p_pcVal->SetNetnumber(l_nNet);
    if (HasAttribute(p_pcRoot, wxT("id")))
        p_pcVal->SetId(GetAttributeLong(p_pcRoot, wxT("id")));
    p_pcVal->SetPosX(GetAttributeDouble(p_pcRoot, wxT("x")));
    p_pcVal->SetPosY(GetAttributeDouble(p_pcRoot, wxT("y")));
    p_pcVal->SetOffsetX(GetAttributeDouble(p_pcRoot, wxT("xoff")));
    p_pcVal->SetOffsetY(GetAttributeDouble(p_pcRoot, wxT("yoff")));
    SP_Graphic* l_pcGrParent = GetGraphicById(p_pcRoot, wxT("grparent"));
    if (l_pcGrParent)
    	l_pcGrParent->AddGraphicChildren(p_pcVal);
    if (HasAttribute(p_pcRoot, wxT("pen")))
        ReadPenColour(p_pcVal, p_pcRoot);
    if (HasAttribute(p_pcRoot, wxT("brush")))
        ReadBrushColour(p_pcVal, p_pcRoot);
    if (p_pcVal->GetGraphicType() == SP_GRAPHIC_NODE)
    {
        SP_GR_Node* l_pcNode = dynamic_cast<SP_GR_Node*>(p_pcVal);
        if (HasAttribute(p_pcRoot, wxT("w")))
            l_pcNode->SetWidth(GetAttributeDouble(p_pcRoot, wxT("w")));
        if (HasAttribute(p_pcRoot, wxT("h")))
            l_pcNode->SetHeight(GetAttributeDouble(p_pcRoot, wxT("h")));
    }
    else if (p_pcVal->GetGraphicType() == SP_GRAPHIC_METADATA)
    {
        SP_GR_Metadata* l_pcMetadata = dynamic_cast<SP_GR_Metadata*>(p_pcVal);
        if (HasAttribute(p_pcRoot, wxT("w")))
            l_pcMetadata->SetWidth(GetAttributeDouble(p_pcRoot, wxT("w")));
        if (HasAttribute(p_pcRoot, wxT("h")))
            l_pcMetadata->SetHeight(GetAttributeDouble(p_pcRoot, wxT("h")));
    }

    p_pcVal->SetGraphicState(GetAttributeGraphicState(p_pcRoot, wxT("state")));
    if (HasAttribute(p_pcRoot, wxT("show")))
        p_pcVal->SetShow(GetAttributeBool(p_pcRoot, wxT("show")));
    if (HasAttribute(p_pcRoot, wxT("designtype")))
    	p_pcVal->SetDesignType(GetAttributeLong(p_pcRoot, wxT("designtype")));
    if (HasAttribute(p_pcRoot, wxT("thickness")))
    	p_pcVal->SetThickness(GetAttributeUInt(p_pcRoot, wxT("thickness")));

    FillInMap(p_pcVal);

    return TRUE;
}

bool
SP_XmlReader::ReadEdgeGraphic(SP_GR_Edge* p_pcVal, wxXmlNode* p_pcRoot)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcRoot, return FALSE);

    unsigned int l_nNet = GetAttributeUInt(p_pcRoot, wxT("net"));
    if (l_nNet <= 0)
    {
    	l_nNet = 1;
    }
    p_pcVal->SetNetnumber(l_nNet);
    if (HasAttribute(p_pcRoot, wxT("id")))
        p_pcVal->SetId(GetAttributeLong(p_pcRoot, wxT("id")));
    p_pcVal->SetSource(GetGraphicById(p_pcRoot, wxT("source")));
    p_pcVal->SetTarget(GetGraphicById(p_pcRoot, wxT("target")));
    p_pcVal->SetOffsetX(GetAttributeDouble(p_pcRoot, wxT("xoff")));
    p_pcVal->SetOffsetY(GetAttributeDouble(p_pcRoot, wxT("yoff")));
    if (GetGraphicById(p_pcRoot, wxT("grparent")))
        GetGraphicById(p_pcRoot, wxT("grparent"))->AddGraphicChildren(p_pcVal);
    if (HasAttribute(p_pcRoot, wxT("pen")))
        ReadPenColour(p_pcVal, p_pcRoot);
    if (HasAttribute(p_pcRoot, wxT("brush")))
        ReadBrushColour(p_pcVal, p_pcRoot);

    p_pcVal->SetGraphicState(GetAttributeGraphicState(p_pcRoot, wxT("state")));
    if (HasAttribute(p_pcRoot, wxT("show")))
        p_pcVal->SetShow(GetAttributeBool(p_pcRoot, wxT("show")));

    wxList l_lPoints;
    double l_nX, l_nY;
    // skip the points count='X' child
    wxXmlNode* l_pcNode = GetNextChild(GetNextChild(p_pcRoot));
    while (l_pcNode)
    {
        if (l_pcNode->GetName() == wxT("point"))
        {
            // new point
            l_nX = GetAttributeDouble(l_pcNode, wxT("x"));
            l_nY = GetAttributeDouble(l_pcNode, wxT("y"));
            l_lPoints.Append((wxObject*)new wxRealPoint(l_nX, l_nY));
        }
        l_pcNode = GetNextSibling(l_pcNode);
    }

    p_pcVal->SetControlPoints(&l_lPoints);
    wxNode* l_pcLNode = l_lPoints.GetFirst();
    while (l_pcLNode)
    {
        wxRealPoint* pt = (wxRealPoint*)l_pcLNode->GetData();
        wxDELETE(pt);
        l_pcLNode = l_pcLNode->GetNext();
    }
	if (HasAttribute(p_pcRoot, wxT("edge_designtype"))) {
		p_pcVal->SetDesignType(GetAttributeLong(p_pcRoot, wxT("edge_designtype")));
	}
	if (HasAttribute(p_pcRoot, wxT("thickness"))) {
		p_pcVal->SetThickness(((size_t)GetAttributeUInt(p_pcRoot, wxT("thickness"))));
	}
	if (HasAttribute(p_pcRoot, wxT("ArrowRight"))) {
		p_pcVal->SetDesignArrowTypeRight(GetAttributeLong(p_pcRoot, wxT("ArrowRight")));
	}
	if (HasAttribute(p_pcRoot, wxT("ArrowLeft"))) {
		p_pcVal->SetDesignArrowTypeLeft(GetAttributeLong(p_pcRoot, wxT("ArrowLeft")));
	}
	if (HasAttribute(p_pcRoot, wxT("ArrowLeftThickness"))) {
		p_pcVal->SetArrowLeftThickness(((size_t)GetAttributeUInt(p_pcRoot, wxT("ArrowLeftThickness"))));
	}
	if (HasAttribute(p_pcRoot, wxT("ArrowRightThickness"))) {
		p_pcVal->SetArrowRightThickness(((size_t)GetAttributeUInt(p_pcRoot, wxT("ArrowRightThickness"))));
	}
	
    return TRUE;
}

//------------------------------------------------------------------------
bool
SP_XmlReader::FillInMap(SP_Data* p_pcData)
{
    if (!p_pcData)
        return FALSE;

    if( p_pcData->GetElementType() == SP_ELEMENT_NODE )
    {
		pair<map<unsigned long, SP_Data*>::iterator, bool> l_Return=
			m_mId2Data.insert(make_pair(p_pcData->GetId(),p_pcData));
		if(l_Return.second == false)
		{
			SP_LOGERROR(wxString::Format(wxT("Error ID: %d already in m_mId2Data!"), p_pcData->GetId()));
		}
    }
    return TRUE;
}

bool
SP_XmlReader::FillInMap(SP_Graphic* p_pcGraphic)
{
    if (!p_pcGraphic)
        return FALSE;

	if(	p_pcGraphic->GetGraphicType() == SP_GRAPHIC_NODE )
    {
		pair<map<unsigned long, SP_Graphic*>::iterator, bool> l_Return=
			m_mId2Graphic.insert(make_pair(p_pcGraphic->GetId(),p_pcGraphic));
		if(l_Return.second == false)
		{
			SP_LOGERROR(wxString::Format(wxT("Error ID: %lu already in m_mId2Graphic!"), p_pcGraphic->GetId()));
		}
		else
		{
			SP_LOGDEBUG(wxString::Format(wxT("Graphic ID: %lu added in m_mId2Graphic!"), p_pcGraphic->GetId()));
		}
    }
    return TRUE;
}

bool
SP_XmlReader::ReadPenColour(SP_Graphic* p_pcVal, wxXmlNode* p_pcNode)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcNode, return FALSE);

    unsigned char l_nR, l_nG, l_nB;
    if (!GetColourAttribute(p_pcNode, wxT("pen"), &l_nR, &l_nG, &l_nB))
        return FALSE;

    p_pcVal->SetPen(wxThePenList->FindOrCreatePen(wxColour(l_nR, l_nG, l_nB), 1, wxSOLID));
	p_pcVal->SetPenColour(wxColour(l_nR, l_nG, l_nB));
    return TRUE;
}

bool
SP_XmlReader::ReadBrushColour(SP_Graphic* p_pcVal, wxXmlNode* p_pcNode)
{
    CHECK_POINTER(p_pcVal, return FALSE);
    CHECK_POINTER(p_pcNode, return FALSE);

    unsigned char l_nR, l_nG, l_nB;
    if (!GetColourAttribute(p_pcNode, wxT("brush"), &l_nR, &l_nG, &l_nB))
        return FALSE;

    p_pcVal->SetBrush(wxTheBrushList->FindOrCreateBrush(wxColour(l_nR, l_nG, l_nB), wxSOLID));

    return TRUE;
}


bool
SP_XmlReader::ReadColListAttribute( SP_DS_ColListAttribute* p_pcColList, wxXmlNode* p_pcRoot )
{
	if (!p_pcColList)
	{
		return TRUE;
	}

	wxXmlNode* l_pcColList_colNode = 0;

	wxString l_pchTmp = wxT("");
	unsigned int l_nColNumber = 0;

	CHECK_POINTER( p_pcRoot, return FALSE );

	wxXmlNode* l_pcColList = GetNextChild(p_pcRoot);

	wxXmlNode* l_pcColList_colLabelNode = GetNextChild(GetNextChild(l_pcColList));


	//begin to transform old colored PN 
	if(p_pcColList->GetName()==SP_DS_CPN_MARKINGLIST &&
		l_pcColList_colLabelNode->GetName() == wxT("colList_colLabel") &&
		(GetNodeCData(l_pcColList_colLabelNode) ==  wxT("Color/Predicate/Function") || 
		GetNodeCData(l_pcColList_colLabelNode) ==  wxT("Color") ))
	{
		SP_DS_Node* l_pcNode = dynamic_cast<SP_DS_Node*> (p_pcColList->GetParent()); 
		if(l_pcNode->GetNodeclass()->GetParentGraph()->GetNetclass()->GetName()==SP_DS_COLSPN_CLASS ||
			l_pcNode->GetNodeclass()->GetParentGraph()->GetNetclass()->GetName()==SP_DS_COLCPN_CLASS)
		{
			while (l_pcColList_colLabelNode)
			{				
				if (l_pcColList_colLabelNode->GetName() == wxT("colList_colLabel"))
				{
					l_pchTmp = GetNodeCData(l_pcColList_colLabelNode);

					if(l_nColNumber >= p_pcColList->GetColCount())
					{
						p_pcColList->AppendEmptyColum();
						p_pcColList->AppendEmptyColum();
					}
					if(l_nColNumber >= 2)
					{						
						wxString l_sNewLabel = l_pchTmp.BeforeFirst(wxT(':'))+ wxT(": Color(s)");
						p_pcColList->SetColLabel(l_nColNumber, l_sNewLabel);
						l_nColNumber++;
					}
					
					if(l_nColNumber == 0)
						p_pcColList->SetColLabel(l_nColNumber, wxT("Main: Color(s)"));
					else
						p_pcColList->SetColLabel(l_nColNumber, l_pchTmp);
					
					l_nColNumber++;
				}
				l_pcColList_colLabelNode = GetNextSibling(l_pcColList_colLabelNode);
			}

			wxXmlNode* l_pcColList_rowNode = GetNextChild(GetNextSibling(GetNextChild(l_pcColList)));

			p_pcColList->Clear();
			unsigned int l_nColCount = p_pcColList->GetColCount();

			while (l_pcColList_rowNode)
			{
				l_pcColList_colNode = GetNextChild(l_pcColList_rowNode);
				l_nColNumber = 0;
				unsigned int l_nNewRow = p_pcColList->AppendEmptyRow();

				wxString l_sColor = l_pchTmp = GetNodeCData(l_pcColList_colNode);

				while ((l_pcColList_colNode) && (l_nColNumber < l_nColCount))
				{
					if(l_nColNumber >= 2)
					{
						p_pcColList->SetCell(l_nNewRow, l_nColNumber, l_sColor);
						l_nColNumber++;
					}
					l_pchTmp = GetNodeCData(l_pcColList_colNode);
					p_pcColList->SetCell(l_nNewRow, l_nColNumber, l_pchTmp);
					l_nColNumber++;
					l_pcColList_colNode = GetNextSibling(l_pcColList_colNode);
				}
				l_pcColList_rowNode = GetNextSibling(l_pcColList_rowNode);
			}

			p_pcColList->UpdateActiveListColumnPtr();

			if(HasAttribute(l_pcColList, wxT("active_row")))
			{
				p_pcColList->SetActiveList(GetAttributeUInt(l_pcColList, wxT("active_row")));
			}

			if(HasAttribute(l_pcColList, wxT("active_col")))
			{
				p_pcColList->SetActiveColumn(GetAttributeUInt(l_pcColList, wxT("active_col")));
				p_pcColList->SetActiveColumn(0);
			}

			//SP_LOGMESSAGE( wxT("\n The colored net has been transformed into a new version. \n "));		

			return ReadData(p_pcColList, p_pcRoot);			
		}

	}

	while (l_pcColList_colLabelNode)
		{
			if(l_nColNumber >= p_pcColList->GetColCount())
			{
				p_pcColList->AppendEmptyColum();
			}
			if (l_pcColList_colLabelNode->GetName() == wxT("colList_colLabel"))
			{
				l_pchTmp = GetNodeCData(l_pcColList_colLabelNode);
				p_pcColList->SetColLabel(l_nColNumber++, l_pchTmp);
			}

			l_pcColList_colLabelNode = GetNextSibling(l_pcColList_colLabelNode);
		}

		wxXmlNode* l_pcColList_rowNode = GetNextChild(GetNextSibling(GetNextChild(l_pcColList)));

		p_pcColList->Clear();
		unsigned int l_nColCount = p_pcColList->GetColCount();

		while (l_pcColList_rowNode)
		{
			l_pcColList_colNode = GetNextChild(l_pcColList_rowNode);
			l_nColNumber = 0;
			unsigned int l_nNewRow = p_pcColList->AppendEmptyRow();

			while ((l_pcColList_colNode) && (l_nColNumber < l_nColCount))
			{
				l_pchTmp = GetNodeCData(l_pcColList_colNode);
				p_pcColList->SetCell(l_nNewRow, l_nColNumber, l_pchTmp);
				l_nColNumber++;
				l_pcColList_colNode = GetNextSibling(l_pcColList_colNode);
			}
			l_pcColList_rowNode = GetNextSibling(l_pcColList_rowNode);
		}

		p_pcColList->UpdateActiveListColumnPtr();

		if(HasAttribute(l_pcColList, wxT("active_row")))
		{
			p_pcColList->SetActiveList(GetAttributeUInt(l_pcColList, wxT("active_row")));
		}

		if(HasAttribute(l_pcColList, wxT("active_col")))
		{
			p_pcColList->SetActiveColumn(GetAttributeUInt(l_pcColList, wxT("active_col")));
			p_pcColList->SetActiveColumn(0);
		}

		return ReadData(p_pcColList, p_pcRoot);

}


bool
SP_XmlReader::ReadOldNodeClass(SP_DS_Metadataclass* p_pcVal, wxXmlNode* p_pcRoot) {
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);
	CHECK_POINTER(p_pcVal->GetPrototype(), return FALSE);

    SP_DS_Metadata* l_pcSpMetadata;
	wxXmlNode* l_pcNode = GetNextChild(p_pcRoot);
	while (l_pcNode)
	{
		// read old parameter values
		if (l_pcNode->GetName() == wxT("node"))
		{
			// new metadata
			l_pcSpMetadata = p_pcVal->GetPrototype()->Clone();
			p_pcVal->AddElement(l_pcSpMetadata);
			ReadOldNode(l_pcSpMetadata, l_pcNode);
		}
		l_pcNode = GetNextSibling(l_pcNode);
	}

	return true;
}


bool
SP_XmlReader::ReadOldNode(SP_DS_Metadata* p_pcVal, wxXmlNode* p_pcRoot) {
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);

	wxXmlNode* l_pcNode = GetNextChild(p_pcRoot);
	while (l_pcNode)
	{
	  if ( l_pcNode->GetName() == wxT("attribute"))
	  {
			// new attribute
			wxString l_pchName = GetAttributeChar( l_pcNode, wxT("name") );
			wxString l_pchType = GetAttributeChar( l_pcNode, wxT("type") );

			if( l_pchName == wxT("Name")) {
				SP_DS_Attribute* l_pcAttr = p_pcVal->GetAttribute(l_pchName);
				ReadOldAttribute(l_pcAttr, l_pcNode);

				l_pcAttr = p_pcVal->GetAttribute(wxT("Group"));
				l_pcAttr->SetValueString(wxT("parameter"), false);
				l_pcAttr = p_pcVal->GetAttribute(wxT("Type"));
				l_pcAttr->SetValueString(wxT("double"), false);
			}

			if( l_pchType == wxT("ColList"))
			{
				SP_DS_ColListAttribute* l_pcAttr = dynamic_cast< SP_DS_ColListAttribute* >( p_pcVal->GetAttribute( wxT("ValueList") ) );
				//ReadColListAttribute( l_pcAttr, l_pcNode );
				ReadValueList( l_pcAttr, l_pcNode );
			}
			else
			{
				//SP_DS_Attribute* l_pcAttr = p_pcVal->GetAttribute(l_pchName);
				//ReadOldAttribute(l_pcAttr, l_pcNode);
			}
		}

	  l_pcNode = GetNextSibling(l_pcNode);

	}

	return !m_bError;



	return true;
}


bool
SP_XmlReader::ReadOldAttribute(SP_DS_Attribute* p_pcVal, wxXmlNode* p_pcRoot) {
	if (!p_pcVal)
		return TRUE;

	//SNOOPy crashed when  an attribute is not there, but it is expected
	//CHECK_POINTER doesnt work
	// Todo: look what is going on in CHECK_POINTER and why snooppy gives
	//an error message when an attribute is not there
	//Work arround: the if construction above
	//original code:
	//CHECK_POINTER(p_pcVal, return TRUE);

	CHECK_POINTER(p_pcRoot, return FALSE);

	// need the CDATA section
	wxString l_pchTmp = GetNodeCData(p_pcRoot);
	p_pcVal->SetValueString(l_pchTmp, false);

	//A special code for dealing with old CPN's version

   //return ReadData(p_pcVal, p_pcRoot);
	return true;
}


bool SP_XmlReader::ReadNewConstant(SP_DS_Node* p_pcVal, const wxString& p_pchAttr, wxXmlNode* p_pcRoot)
{

	SP_DS_Graph* p_pcGraph = p_pcVal->GetNodeclass()->GetParentGraph();
	SP_DS_Metadataclass* p_pcMeta = p_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	SP_DS_Nodeclass* p_pcNode = p_pcVal->GetNodeclass();

	SP_DS_Metadata* l_pcSpMetadata = p_pcMeta->GetPrototype()->Clone();

	SP_DS_Attribute* l_pcAttr = l_pcSpMetadata->GetAttribute(wxT("Name"));
	l_pcAttr->SetValueString(p_pchAttr);

	l_pcAttr = l_pcSpMetadata->GetAttribute(wxT("Group"));
	l_pcAttr->SetValueString(wxT("marking"));

	l_pcAttr = l_pcSpMetadata->GetAttribute(wxT("Type"));
	if (p_pcNode->GetName().Contains(wxT("Continuous")))
	{
		l_pcAttr->SetValueString(wxT("double"));
	}
	else
	{
		l_pcAttr->SetValueString(wxT("int"));
	}

	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcSpMetadata->GetAttribute(wxT("ValueList")));
	ReadValueList(l_pcColList, p_pcRoot);

	bool unique = true;
	wxString first = l_pcColList->GetCell(0, 1);
	for (unsigned r = 1; r < l_pcColList->GetRowCount(); ++r)
	{
		wxString v = l_pcColList->GetCell(r, 1);
		if (!v.IsEmpty() && v != first)
		{
			unique = false;
			break;
		}
	}

	if (unique)
	{
		SP_DS_Attribute* l_pcAttr = p_pcVal->GetAttribute(wxT("Marking"));
		l_pcAttr->SetValueString(first, false);

		wxDELETE(l_pcSpMetadata);
		return false;
	}
	else
	{
		p_pcMeta->AddElement(l_pcSpMetadata);
		return true;
	}
}



bool SP_XmlReader::ReadValueList(SP_DS_ColListAttribute* p_pcColList, wxXmlNode* p_pcRoot)
{

	if (!p_pcColList)
	{
		return TRUE;
	}

	wxXmlNode* l_pcColList_colNode = 0;

	wxString l_pchTmp = wxT("");
	unsigned int l_nColNumber = 0;

	CHECK_POINTER( p_pcRoot, return FALSE);

	wxXmlNode* l_pcColList = GetNextChild(p_pcRoot);

	wxXmlNode* l_pcColList_colLabelNode = GetNextChild(GetNextChild(l_pcColList));

	while (l_pcColList_colLabelNode)
	{
		if (l_nColNumber >= p_pcColList->GetColCount())
		{
			p_pcColList->AppendEmptyColum();
		}
		if (l_pcColList_colLabelNode->GetName() == wxT("colList_colLabel"))
		{
			l_pchTmp = GetNodeCData(l_pcColList_colLabelNode);
			if (l_pchTmp == wxT("Parameter"))
			{
				p_pcColList->SetColLabel(l_nColNumber++, wxT("Value set"));
			}
			else if (l_pchTmp == wxT("Parameter set"))
			{
				p_pcColList->SetColLabel(l_nColNumber++, wxT("Value"));
			}
			else if (l_pchTmp == wxT("Marking set"))
			{
				p_pcColList->SetColLabel(l_nColNumber++, wxT("Value set"));
			}
			else if (l_pchTmp == wxT("Marking"))
			{
				p_pcColList->SetColLabel(l_nColNumber++, wxT("Value"));
			}
			else
			{
				p_pcColList->SetColLabel(l_nColNumber++, l_pchTmp);
			}
		}

		l_pcColList_colLabelNode = GetNextSibling(l_pcColList_colLabelNode);
	}

	wxXmlNode* l_pcColList_rowNode = GetNextChild(GetNextSibling(GetNextChild(l_pcColList)));

	p_pcColList->Clear();
	unsigned int l_nColCount = p_pcColList->GetColCount();

	while (l_pcColList_rowNode)
	{
		l_pcColList_colNode = GetNextChild(l_pcColList_rowNode);
		l_nColNumber = 0;
		unsigned int l_nNewRow = p_pcColList->AppendEmptyRow();

		while ((l_pcColList_colNode) && (l_nColNumber < l_nColCount))
		{
			//don't load same values
			if (l_nNewRow > 0)
			{

				l_pchTmp = GetNodeCData(l_pcColList_colNode);
				if (p_pcColList->GetCell(0, l_nColNumber) == l_pchTmp)
				{
					l_nColNumber++;
					l_pcColList_colNode = GetNextSibling(l_pcColList_colNode);
				}
				else
				{
					if (l_nColNumber == 0)
					{
						wxString l_sValue = wxT("V-Set ");
						l_sValue << l_nNewRow;
						p_pcColList->SetCell(l_nNewRow, l_nColNumber, l_sValue);
					}

					if (l_nColNumber != 0)
						p_pcColList->SetCell(l_nNewRow, l_nColNumber, l_pchTmp);

					l_nColNumber++;
					l_pcColList_colNode = GetNextSibling(l_pcColList_colNode);
				}

			}
			else
			{
				l_pchTmp = GetNodeCData(l_pcColList_colNode);
				p_pcColList->SetCell(l_nNewRow, l_nColNumber, l_pchTmp);
				l_nColNumber++;
				l_pcColList_colNode = GetNextSibling(l_pcColList_colNode);
			}

		}
		l_pcColList_rowNode = GetNextSibling(l_pcColList_rowNode);
	}

	p_pcColList->UpdateActiveListColumnPtr();

	if (HasAttribute(l_pcColList, wxT("active_row")))
	{
		p_pcColList->SetActiveList(GetAttributeUInt(l_pcColList, wxT("active_row")));
	}

	if (HasAttribute(l_pcColList, wxT("active_col")))
	{
		p_pcColList->SetActiveColumn(GetAttributeUInt(l_pcColList, wxT("active_col")));
		p_pcColList->SetActiveColumn(0);
	}

	return true;
}

void
SP_XmlReader::RemoveInitialElements(SP_DS_Graph* p_pcGraph)
{
	SP_ListMetadataclass::const_iterator mcIt = p_pcGraph->GetMetadataclasses()->begin();
	SP_ListMetadataclass::const_iterator mcEnd = p_pcGraph->GetMetadataclasses()->end();
	for(; mcIt != mcEnd; ++mcIt)
	{
		SP_DS_Metadataclass* mc = *mcIt;
		mc->RemoveAll();
	}
}

