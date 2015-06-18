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
	m_pcGraph(p_pcGraph)
{
}

SP_VectorReader::~SP_VectorReader()
{
}

bool SP_VectorReader::Read(SP_DS_NodeVectorSystem *p_pcNvms,
		const wxString& p_sFile)
{
	CHECK_POINTER(p_pcNvms, return FALSE);

	// because the map<int, int > gives null back when it can't find the key
	// and null is the first position, we add/subtract l_ADDING at the right places
	const int l_ADDING = 1;

	int l_minOcc = 0;
	int l_maxOcc = 0;

	SP_VectorScanner l_pcScanner;
	if (l_pcScanner.Open(p_sFile))
	{
		SP_VectorParser l_pcParser(l_pcScanner);
		// get type information and set nodelist
		p_pcNvms->SetInformation(l_pcParser.GetVectorSetInformation());
		SP_VECTOR_TYPE l_nType = l_pcParser.ReadVectorTypeInformation();
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

		//initialize Map to know which place of vector belongs to nodeNum
		SP_MapInt2Int l_pcMapNodeNum2VecNum;
		SP_MapString2Int l_pcMapNodeName2NodeNum;
		int i = 0;
		for(SP_DS_Nodeclass* l_pcNC : *(m_pcGraph->GetNodeclasses()))
		{
			if(l_pcNC->GetName().Contains(l_sNodeclass)
				&& !l_pcNC->GetName().Contains(wxT("Coarse")))
			{
				for(SP_DS_Node* l_pcN : *(l_pcNC->GetElements()))
				{
					p_pcNvms->AddNodeToNodeList(l_pcN);
					wxString name = dynamic_cast<SP_DS_NameAttribute*>(l_pcN->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					int id = dynamic_cast<SP_DS_IdAttribute*>(l_pcN->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID))->GetValue();
					l_pcMapNodeNum2VecNum.insert(std::make_pair(id, i+l_ADDING));
					l_pcMapNodeName2NodeNum.insert(std::make_pair(name, id));
					i++;
				}
			}
		}

		l_len = p_pcNvms->GetNodes().size();
		if(l_len == 0)
		{
			return false;
		}

		long actNum;
		// read in vector informations
		while (l_pcParser.ReadVectorNumber(actNum))
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
			while (l_pcParser.ReadNodeData(l_nodeNum, s, l_val))
			{
				// string check is possible, but makes no sense when
				// the string was shortened earlier - so its not implemented
				if(l_nodeNum == -1)
				{
					l_nodeNum = l_pcMapNodeName2NodeNum[s];
				}
				l_nPosition = l_pcMapNodeNum2VecNum[l_nodeNum];
				if (l_nPosition == 0)
				{
					return FALSE;
				}
				a_vector[l_pcMapNodeNum2VecNum[l_nodeNum]-l_ADDING] = l_val;

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
		while (l_pcParser.ReadVectorNumber(actNum) && actNum)
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
			l_pcParser.ReadVectorInformation(l_name, l_description);
			if (nodeVector->GetAttribute(wxT("Name"))
					&& nodeVector->GetAttribute(wxT("Comment")))
			{
				dynamic_cast<SP_DS_NameAttribute*>(nodeVector->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->SetValueString(l_name);
				dynamic_cast<SP_DS_TextAttribute*>(nodeVector->GetAttribute(wxT("Comment")))->SetValueString(l_description);
			}
		}
		l_pcScanner.Close();
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}
