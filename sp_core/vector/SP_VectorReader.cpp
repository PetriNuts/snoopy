//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: kw $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: node vector reader
//////////////////////////////////////////////////////////////////////
#include "sp_core/vector/SP_VectorReader.h"
#include "sp_core/vector/SP_VectorScanner.h"
#include "sp_core/vector/SP_VectorParser.h"
#include "sp_ds/SP_DS_NodeVector.h"
#include "sp_ds/SP_DS_NodeVectorSystem.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_defines.h"
using namespace std;

SP_VectorReader::SP_VectorReader(SP_DS_Graph* p_pcGraph):
	m_pcParser(NULL), m_pcScanner(NULL), m_pcGraph(p_pcGraph),
			m_pcMapNodeNum2VecNum(NULL)
{
}

SP_VectorReader::~SP_VectorReader()
{
	wxDELETE(m_pcParser);
	wxDELETE(m_pcScanner);
	wxDELETE(m_pcMapNodeNum2VecNum);
}

bool SP_VectorReader::Read(SP_DS_NodeVectorSystem *p_pcNvms,
		const wxString& p_sFile)
{
	CHECK_POINTER(p_pcNvms, return FALSE);

	// because the map<int, int > gives null back when it can't find the key
	// and null is the first position, we add/substract l_ADDING at the rigth places
	const int l_ADDING = 1;

	int l_minOcc = 0;
	int l_maxOcc = 0;

	m_pcScanner = new SP_VectorScanner();
	if (m_pcScanner->Open(p_sFile))
	{
		m_pcParser = new SP_VectorParser(m_pcScanner);
		// get type information and set nodelist
		p_pcNvms->SetInformation(m_pcParser->GetVectorSetInformation());
		SP_VECTOR_TYPE l_nType = m_pcParser->ReadVectorTypeInformation();
		int l_len;
		wxString l_sNodeclass(wxT(""));
		if (l_nType == SP_TRANSITION_TYPE)
		{
			l_sNodeclass = wxT("Transition");
		}
		else if (l_nType == SP_PLACE_TYPE)
		{
			l_sNodeclass = wxT("Place");
		}
		else if (l_nType == SP_NO_NODE_VECTOR)
		{
			return FALSE;
		}

		for(SP_ListNodeclass::const_iterator itNC = m_pcGraph->GetNodeclasses()->begin();
			itNC != m_pcGraph->GetNodeclasses()->end();
			++itNC)
		{
			if((*itNC)->GetName().Contains(l_sNodeclass)
				&& !(*itNC)->GetName().Contains(wxT("Coarse")))
			{
				for(SP_ListNode::const_iterator itN = (*itNC)->GetElements()->begin();
					itN != (*itNC)->GetElements()->end();
					++itN)
				{
					p_pcNvms->AddNodeToNodeList(*itN);
				}
			}
		}

		l_len = p_pcNvms->GetNodes().size();
		if(l_len == 0)
		{
			return false;
		}

		//initialize Map to know which place of vector belongs to nodeNum
		const SP_ListNode& l_pcNodes = p_pcNvms->GetNodes();
		m_pcMapNodeNum2VecNum = new map<int,int>();
		SP_ListNode::const_iterator l_itNode = l_pcNodes.begin();
		int i = 0;
		while (l_itNode != l_pcNodes.end())
		{
			m_pcMapNodeNum2VecNum->insert(pair<int, int>(
					dynamic_cast<SP_DS_IdAttribute*>((*l_itNode)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID))->GetValue(), i +l_ADDING));

			i++;
			l_itNode++;
		}

		long actNum;
		// read in vector informations
		while (m_pcParser->ReadVectorNumber(actNum))
		{
			SP_DS_NodeVector *l_pcNodeVector = new SP_DS_NodeVector(m_pcGraph, p_pcNvms);
			SP_DS_IdAttribute* l_pcAttr = new SP_DS_IdAttribute(wxT("ID"));
			l_pcAttr->SetValue(actNum);
			l_pcNodeVector->AddAttribute(l_pcAttr);
			//SP_LOGDEBUG(wxString::Format(wxT("read in vector number %d "), actNum));

			p_pcNvms->GetNodes().size();
			SP_VectorLong a_vector(l_len, 0);
			long l_val = -1;
			long l_nodeNum = -1;
			wxString s;
			int l_nPosition = 0;
			while (m_pcParser->ReadNodeData(l_nodeNum, s, l_val))
			{
				// string check is possible, but makes no sense when
				// the string was shortened earlier - so its not implemented
				l_nPosition = (*m_pcMapNodeNum2VecNum)[l_nodeNum];
				if (l_nPosition == 0)
				{
					return FALSE;
				}
				a_vector[(*m_pcMapNodeNum2VecNum)[l_nodeNum]-l_ADDING] = l_val;

				if (l_minOcc == l_maxOcc && l_minOcc == 0 && l_val != 0)
				{
					l_minOcc = l_val;
					l_maxOcc = l_val;
				}
				else if (l_val != 0 && l_minOcc > l_val)
				{
					l_minOcc = l_val;
				}
				else if (l_val != 0 && l_maxOcc < l_val)
				{
					l_maxOcc = l_val;
				}
			}
			l_pcNodeVector->SetVector(a_vector);
			l_pcNodeVector->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
			l_pcNodeVector->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));

			p_pcNvms->AddElement(l_pcNodeVector);
		}

		p_pcNvms->SetOccurrenceRangeMax(l_maxOcc);
		p_pcNvms->SetOccurrenceRangeMin(l_minOcc);

		// read in names and additional comments to node vectors
		while (m_pcParser->ReadVectorNumber(actNum) && actNum)
		{
			SP_DS_NodeVector* nodeVector = p_pcNvms->GetNodeVectorById(actNum);
			if (!nodeVector)
			{
				SetLastError(wxString::Format(wxT("Node vector id %d unknown"),
						actNum));
				return FALSE;
			}
			wxString l_name;
			wxString l_description;
			m_pcParser->ReadVectorInformation(l_name, l_description);
			if (nodeVector->GetAttribute(wxT("Name"))
					&& nodeVector->GetAttribute(wxT("Comment")))
			{
				dynamic_cast<SP_DS_NameAttribute*>(nodeVector->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->SetValueString(l_name);
				dynamic_cast<SP_DS_TextAttribute*>(nodeVector->GetAttribute(wxT("Comment")))->SetValueString(l_description);
			}
		}
		m_pcScanner->Close();
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}
