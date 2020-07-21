/*
 * SP_ImportCSV.cpp
 *
 *  Created on: May 26, 2010
 *      Author: crohr
 */
#include "wx/textfile.h"
#include "wx/tokenzr.h"

#include "SP_ImportCSV.h"
#include "snoopy.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

SP_ImportCSV::SP_ImportCSV():
m_pcGraph(NULL), m_Separator(wxT(",;:\t"))
{
	Clear();
}

SP_ImportCSV::~SP_ImportCSV()
{
}

void
SP_ImportCSV::Clear()
{
	m_Csv2Id.clear();
	m_ResultMatrix.clear();
	m_Place2Id.clear();
	m_Trans2Id.clear();
	m_Param2Id.clear();
	m_ColCount = 0;
	m_RowLength = 0;
	m_IntervalStart = SP_INFINITY;
	m_IntervalEnd = 0.0;
	m_OutputStepCount = 0;
}

bool
SP_ImportCSV::ReadFile(const wxString& fileName,SP_ImportRoutine* p_sIR)
{
	Clear();

	ResetErrors();

	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(wxGetApp().GetDocmanager()->GetCurrentDocument());

	if(l_pcDoc)
	{
		m_pcGraph = l_pcDoc->GetGraph();
//		if(m_pcGraph->GetNetclass()->GetName() == SP_DS_SPN_CLASS)
		{
			LoadPlaces();
			LoadTransitions();
			LoadParameters();

			DoReadFile(fileName);
		}
/*		else
		{
			wxString l_sMsg = wxT("Error: Only ");
			l_sMsg << SP_DS_SPN_CLASS << wxT(" netclass allowed!");
			SP_MESSAGEBOX(l_sMsg, wxT("Error"));
			SetLastError(l_sMsg);
		}
*/	}
	else
	{
		wxString l_sMsg = wxT("Error: No document open.");
		SP_MESSAGEBOX(l_sMsg, wxT("Error"));
		SetLastError(l_sMsg);
	}

	return !GetErrorSet();
}

bool
SP_ImportCSV::ReadFile(const wxString& fileName, spsim::Simulator* simulator)
{
	CHECK_POINTER(simulator, return false);

	Clear();

	ResetErrors();
	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(wxGetApp().GetDocmanager()->GetCurrentDocument());

	if(l_pcDoc)
	{
		m_pcGraph = l_pcDoc->GetGraph();
	}

	if(m_pcGraph && m_pcGraph->GetNetclass()->GetName() == SP_DS_SPN_CLASS)
	{
		LoadPlaces();
		LoadTransitions();
		LoadParameters();
	}
	else
	{
		SP_VectorString* l_Places = simulator->GetPlaceNames();
		m_RowLength = simulator->GetPlaceCount();
		for(unsigned long i = 0; i < m_RowLength; ++i)
		{
			m_Place2Id.insert(make_pair((*l_Places)[i], i));
		}
	}
	DoReadFile(fileName);

	return !GetErrorSet();
}

bool SP_ImportCSV::DoReadFile(const wxString& fileName)
{
	wxTextFile l_File;
	if(l_File.Open(fileName))
	{
		wxString str = l_File.GetFirstLine();
		if(str.Find(',') != wxNOT_FOUND)
		{
			m_Separator = wxT(',');
		}
		else if(str.Find(';') != wxNOT_FOUND)
		{
			m_Separator = wxT(';');
		}
		else if(str.Find('\t') != wxNOT_FOUND)
		{
			m_Separator = wxT('\t');
		}
		else if(str.Find(' ') != wxNOT_FOUND)
		{
			m_Separator = wxT(' ');
		}
		else if(str.Find(':') != wxNOT_FOUND)
		{
			m_Separator = wxT(':');
		}
		if(ParseLine(str))
		{
			if(m_ArrayString[0].CmpNoCase(wxT("time")) != 0)
			{
				wxString l_sMsg = wxT("Error: wrong data format.");
				SP_MESSAGEBOX(l_sMsg, wxT("Error"));
				SetLastError(l_sMsg);
				return false;
			}
			m_ColCount = m_ArrayString.size();
			m_Csv2Id.assign(m_ColCount, -1);
			for(unsigned long i = 1; i < m_ColCount; ++i)
			{
				wxString l_sName = m_ArrayString[i];
				SP_MapString2ULong::const_iterator it = m_Place2Id.find(l_sName);
				if(it != m_Place2Id.end())
				{
					m_Csv2Id[i] = (*it).second;
					m_sClassname = wxT("Place");
				}
				else
				{
					it = m_Trans2Id.find(l_sName);
					if(it != m_Trans2Id.end())
					{
						m_Csv2Id[i] = (*it).second;
						m_sClassname = wxT("Transition");
					}
					else
					{
						wxString l_sMsg = wxT(" \"");
						l_sMsg << l_sName << wxT("\" not found.");
						SP_LOGWARNING( l_sMsg);
						//SetLastError(l_sMsg);
						SP_MESSAGEBOX(l_sMsg, wxT("Warning"));
						//return false;
					}
				}
			}
			if(m_sClassname == wxT("Place"))
			{
				m_RowLength = m_Place2Id.size();
			}
			else
			{
				m_RowLength = m_Trans2Id.size();
			}
		}
		for ( str = l_File.GetNextLine(); !l_File.Eof(); str = l_File.GetNextLine() )
		{
		    if(ParseLine(str) && m_ColCount == m_ArrayString.size())
		    {
		    	double val = 0.0;
		    	if(m_ArrayString[0].ToDouble(&val))
		    	{
			    	m_IntervalStart = std::min(m_IntervalStart, val);
			    	m_IntervalEnd = std::max(m_IntervalEnd, val);
		    	}
		    	SP_VectorDouble l_Row(m_RowLength, 0.0);
				for(unsigned long i = 1; i < m_ColCount; ++i)
				{

					if(m_ArrayString[i].ToDouble(&val))
					{
						long id = m_Csv2Id[i];
						if(id >= 0)
						{
							l_Row[id] = val;
						}
					}
					else
					{
						wxString l_sMsg = wxT("Error: no double value \"");
						l_sMsg << m_ArrayString[i] << wxT("\".");
						SP_MESSAGEBOX(l_sMsg, wxT("Error"));
						SetLastError(l_sMsg);
						return false;
					}
				}
		    	m_ResultMatrix.push_back(l_Row);
		    	++m_OutputStepCount;
		    }
		}
	}
	else
	{
		wxString l_sMsg = wxT("Error: file not found  ");
		l_sMsg << fileName;
		SP_MESSAGEBOX(l_sMsg, wxT("Error"));
		SetLastError(l_sMsg);
	}
	return !GetErrorSet();
}

bool
SP_ImportCSV::ParseLine(const wxString& p_sValue)
{
	m_ArrayString.clear();
	wxString l_sValue = p_sValue;
	l_sValue.Trim();
	l_sValue.Trim(false);

	if(l_sValue.IsEmpty()){ return false; }

	wxStringTokenizer l_cTokenizer(l_sValue, m_Separator, wxTOKEN_RET_EMPTY_ALL);

	if(l_cTokenizer.CountTokens() < 2)
	{
		return false;
	}

	while(l_cTokenizer.HasMoreTokens())
	{
		wxString l_sToken = l_cTokenizer.GetNextToken();
		l_sToken.Trim();
		l_sToken.Trim(false);
		m_ArrayString.push_back(l_sToken);
	}

	return true;
}



void
SP_ImportCSV::LoadPlaces()
{
	SP_DS_Nodeclass* l_pcNodeclass = NULL;

	unsigned long id = 0;
	SP_ListNode::const_iterator l_itElem;
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
	if(l_pcNodeclass)
	{
		for(l_itElem = l_pcNodeclass->GetElements()->begin();
			l_itElem != l_pcNodeclass->GetElements()->end();
			++l_itElem)
		{
			SP_DS_Node* l_pcNode = (*l_itElem);
			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			m_Place2Id[l_sPlaceName] = id;
			++id;
		}
	}
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
	if(l_pcNodeclass)
	{
		for(l_itElem = l_pcNodeclass->GetElements()->begin();
			l_itElem != l_pcNodeclass->GetElements()->end();
			++l_itElem)
		{
			SP_DS_Node* l_pcNode = (*l_itElem);
			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			m_Place2Id[l_sPlaceName] = id;
			++id;
		}
	}
}

void
SP_ImportCSV::LoadTransitions()
{
	unsigned long id = 0;
	SP_ListNode::const_iterator l_itElem;
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);
	if(l_pcNodeclass)
	{
		for(l_itElem = l_pcNodeclass->GetElements()->begin();
			l_itElem != l_pcNodeclass->GetElements()->end();
			++l_itElem)
		{
			SP_DS_Node* l_pcNode = (*l_itElem);
			wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			m_Trans2Id[l_sName] = id;
			++id;
		}
	}
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
	if(l_pcNodeclass)
	{
		for(l_itElem = l_pcNodeclass->GetElements()->begin();
			l_itElem != l_pcNodeclass->GetElements()->end();
			++l_itElem)
		{
			SP_DS_Node* l_pcNode = (*l_itElem);
			wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			m_Trans2Id[l_sName] = id;
			++id;
		}
	}
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_IMMEDIATE_TRANS);
	if(l_pcNodeclass)
	{
		for(l_itElem = l_pcNodeclass->GetElements()->begin();
			l_itElem != l_pcNodeclass->GetElements()->end();
			++l_itElem)
		{
			SP_DS_Node* l_pcNode = (*l_itElem);
			wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			m_Trans2Id[l_sName] = id;
			++id;
		}
	}
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS);
	if(l_pcNodeclass)
	{
		for(l_itElem = l_pcNodeclass->GetElements()->begin();
			l_itElem != l_pcNodeclass->GetElements()->end();
			++l_itElem)
		{
			SP_DS_Node* l_pcNode = (*l_itElem);
			wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			m_Trans2Id[l_sName] = id;
			++id;
		}
	}
	l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_SCHEDULED_TRANS);
	if(l_pcNodeclass)
	{
		for(l_itElem = l_pcNodeclass->GetElements()->begin();
			l_itElem != l_pcNodeclass->GetElements()->end();
			++l_itElem)
		{
			SP_DS_Node* l_pcNode = (*l_itElem);
			wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			m_Trans2Id[l_sName] = id;
			++id;
		}
	}
}

void
SP_ImportCSV::LoadParameters()
{
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(SP_DS_PARAM);

	unsigned long id = 0;

	if(l_pcNodeclass)
	{
		SP_ListNode::const_iterator l_itElem;
		for(l_itElem = l_pcNodeclass->GetElements()->begin();
			l_itElem != l_pcNodeclass->GetElements()->end();
			++l_itElem )
		{
			SP_DS_Node* l_pcNode = (*l_itElem);
			wxString l_sParameterName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			m_Param2Id[l_sParameterName] = id;
			++id;
		}
	}

	SP_DS_Metadataclass* l_pcMC = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

	if(l_pcMC)
	{
		SP_ListMetadata::const_iterator l_itElem;
		for(l_itElem = l_pcMC->GetElements()->begin();
			l_itElem != l_pcMC->GetElements()->end();
			++l_itElem )
		{
			SP_DS_Metadata* l_pcNode = (*l_itElem);
			wxString l_sParameterName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

			m_Param2Id[l_sParameterName] = id;
			++id;
		}
	}
}
