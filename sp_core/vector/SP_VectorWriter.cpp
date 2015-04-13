//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: kw $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: node vector reader
//////////////////////////////////////////////////////////////////////
#include "sp_core/vector/SP_VectorWriter.h"
#include "sp_ds/SP_DS_NodeVector.h"
#include "sp_ds/SP_DS_NodeVectorSystem.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

#include "sp_defines.h"

SP_VectorWriter::SP_VectorWriter() :
			m_bOccurrencesWritten(FALSE)
{
}

SP_VectorWriter::~SP_VectorWriter()
{
}

bool SP_VectorWriter::Visit(SP_DS_NodeVector * p_pcNodeVector)
{
	//write node vector informations

	if (!m_bOccurrencesWritten)
	{
		// the occurrences
		const SP_ListNode& l_pclAllNodes = m_pcNvs->GetNodes();

		SP_VectorLong::const_iterator l_aVector = p_pcNodeVector->GetVector().begin();
		if (!p_pcNodeVector->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID))
		{
			return FALSE;
		}

		int num = dynamic_cast<SP_DS_IdAttribute*> (p_pcNodeVector->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID))->GetValue();

		m_fFile.Write(wxString::Format(wxT("\t%d |\t"), num));

		int l_n = 0;
		// count the not-null entrys
		for (m_nodeIt = l_pclAllNodes.begin(); m_nodeIt != l_pclAllNodes.end(); m_nodeIt++)
		{
			if (*l_aVector > 0)
			{
				l_n++;
			}
			l_aVector++;
		}

		l_aVector = p_pcNodeVector->GetVector().begin();

		int l_counter = 0;
		for (m_nodeIt = l_pclAllNodes.begin(); m_nodeIt != l_pclAllNodes.end(); m_nodeIt++)
		{
			if (*l_aVector > 0)
			{
				if (!(*m_nodeIt)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID))
				{
					// there has to be a id
					return FALSE;
				}
				if ((*m_nodeIt)->GetAttribute(wxT("Name")))
				{
					m_fFile.Write(wxString::Format(wxT("%ld.%s\t\t\t:\t%d"), dynamic_cast<SP_DS_IdAttribute*> ((*m_nodeIt)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID))->GetValue(),
							(*m_nodeIt)->GetAttribute(wxT("Name"))->GetValueString().c_str(), *l_aVector));
				}
				else
				{
					m_fFile.Write(wxString::Format(wxT("%s\t\t\t\t:\t%d"), (*m_nodeIt)->GetAttribute(wxT("Name"))->GetValueString().c_str(), *l_aVector));
				}
				if (l_counter != l_n - 1)
				{
					// @todo eventuell nach betriebssystem richten
					m_fFile.Write(wxT(",\n\t  |\t"));
				}
				else
				{
					m_fFile.Write(wxT("\n\n"));
				}
				l_counter++;
			}

			l_aVector++;
		}
	}
	else
	{
		// the additional informations
		if (!p_pcNodeVector->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID)
			|| !p_pcNodeVector->GetAttribute(wxT("Name"))
			|| !p_pcNodeVector->GetAttribute(wxT("Comment")))
		{
			return FALSE;
		}

		int num = dynamic_cast<SP_DS_IdAttribute*> (p_pcNodeVector-> GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID))->GetValue();

		m_fFile.Write(wxString::Format(wxT("\t%d |%s|%s|\n"), num, p_pcNodeVector->GetAttribute(wxT("Name"))->GetValueString().c_str(),
				p_pcNodeVector->GetAttribute(wxT("Comment"))->GetValueString().c_str()));
	}
	return true;
}

bool SP_VectorWriter::Write(SP_DS_NodeVectorSystem *p_pcNvs, const wxString& p_sFile)
{
	CHECK_POINTER(p_pcNvs, return FALSE);
	m_pcNvs = p_pcNvs;

	// open file
	if (p_sFile.empty())
	{
		return FALSE;
	}
	m_fFile.Open(wxString::Format(wxT("%s"), p_sFile.c_str()), wxT("w"));
	if (!m_fFile.IsOpened())
	{
		return FALSE;
	}

	// write comments
	m_fFile.Write(p_pcNvs->GetInformation().c_str());

	// visit all node vectors for occurrences infos
	m_bOccurrencesWritten = false;

	p_pcNvs->AcceptAll(this);

	m_fFile.Write(wxString::Format(wxT("\n\n@\n\n")));

	// visit all node vectors for additional infos
	m_bOccurrencesWritten = true;
	p_pcNvs->AcceptAll(this);
	m_bOccurrencesWritten = false;

	// close file
	m_fFile.Close();
	//wxDELETE(m_fFile);
	return true;
}
