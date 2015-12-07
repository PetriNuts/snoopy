//////////////////////////////////////////////////////////////////////
// $Author: fei liu $
// $Version: 0.1 $
// $Date: 2010/05/26 $
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportColSPN2CSV.h"
#include "sp_utilities.h"

#include "sp_ds/SP_DS_Edge.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include <wx/tokenzr.h>

#include "sp_ds/attributes/SP_DS_NameAttribute.h"

SP_ExportColSPN2CSV::SP_ExportColSPN2CSV()
{
}

SP_ExportColSPN2CSV::~SP_ExportColSPN2CSV()
{
}

bool SP_ExportColSPN2CSV::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return ( cName == SP_DS_COLSPN_CLASS || 
		     cName == SP_DS_COLCPN_CLASS || 
			 cName == SP_DS_COLHPN_CLASS || 
			 cName == SP_DS_COLEPN_CLASS || 
			 cName == SP_DS_COLPN_CLASS);
}

bool SP_ExportColSPN2CSV::DoWrite()
{
	WriteDeclarations();

	WritePlaces();
	WriteTransitions();
	WriteParameters();

	return !m_bError;
}

bool SP_ExportColSPN2CSV::WritePlaces()
{
	m_file.Write(wxT("Place:\n\n"));

	WritePlaceType(SP_DS_CONTINUOUS_PLACE);
	m_file.Write(wxT("\n"));

	WritePlaceType(SP_DS_DISCRETE_PLACE);
	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportColSPN2CSV::WritePlaceType(wxString p_sNodeType)
{
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass= m_graph->GetNodeclass( p_sNodeType );
	if(l_pcNodeclass)
	{
		if(l_pcNodeclass->GetElements()->size() > 0 )
		{
			SP_DS_Node* l_pcNode = l_pcNodeclass->GetElements()->front();
			SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("MarkingList")));
			wxString l_sLine = p_sNodeType;
			for(unsigned long i = 0; i < l_pcAttr->GetColCount(); i++)
			{
				l_sLine << wxT(";") << l_pcAttr->GetColLabel(i);
			}
			l_sLine << wxT("\n");
			m_file.Write(l_sLine);
		}

		for (SP_DS_Node* l_pcNode : *(l_pcNodeclass->GetElements()))
		{
			wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			wxString l_sLine = l_sPlaceName;
			SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("MarkingList")));
			for(unsigned long i = 0; i < l_pcAttr->GetRowCount(); i++)
			{
				for(unsigned long j = 0; j < l_pcAttr->GetColCount(); j++)
				{
					l_sLine << wxT(";") << l_pcAttr->GetCell(i,j);
				}
				l_sLine << wxT("\n");
			}
			l_sLine << wxT("\n");
			m_file.Write(l_sLine);

		}
	}

	return true;
}

bool SP_ExportColSPN2CSV::WriteTransType(wxString p_sNodeType)
{
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass= m_graph->GetNodeclass( p_sNodeType );
	if(l_pcNodeclass)
	{
		if(l_pcNodeclass->GetElements()->size() > 0 )
		{
			SP_DS_Node* l_pcNode = l_pcNodeclass->GetElements()->front();
			SP_DS_ColListAttribute* l_pcAttr;
			wxString l_sLine;
			if(p_sNodeType==SP_DS_DETERMINISTIC_TRANS)
			{
				l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("DelayList")));				
			}
			else if(p_sNodeType==SP_DS_SCHEDULED_TRANS)
			{
				l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("PeriodicList")));				
			}
			else
			{
				l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));
			}
			if(l_pcAttr)
			{
				l_sLine = p_sNodeType;
				for(unsigned long i = 0; i < l_pcAttr->GetColCount(); i++)
				{
					l_sLine << wxT(";") << l_pcAttr->GetColLabel(i);
				}
			}
			l_sLine << wxT("\n");
			m_file.Write(l_sLine);
		}

		for (SP_DS_Node* l_pcNode : *(l_pcNodeclass->GetElements()))
		{
			wxString l_sNodeName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			wxString l_sLine = l_sNodeName;
			SP_DS_ColListAttribute* l_pcAttr;
			if(p_sNodeType==SP_DS_DETERMINISTIC_TRANS)
			{
				l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("DelayList")));				
			}
			else if(p_sNodeType==SP_DS_SCHEDULED_TRANS)
			{
				l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("PeriodicList")));				
			}
			else
			{
				l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));
			}
			if(l_pcAttr)
			{
				for(unsigned long i = 0; i < l_pcAttr->GetRowCount(); i++)
				{
					for(unsigned long j = 0; j < l_pcAttr->GetColCount(); j++)
					{
						l_sLine << wxT(";") << l_pcAttr->GetCell(i,j);
					}
					l_sLine << wxT("\n");
				}
			}
			l_sLine << wxT("\n");
			m_file.Write(l_sLine);
		}
	}

	return true;
}


bool SP_ExportColSPN2CSV::WriteTransitions()
{
	m_file.Write(wxT("Transition:\n\n"));

	WriteTransType(SP_DS_CONTINUOUS_TRANS);
	m_file.Write(wxT("\n"));
	WriteTransType(SP_DS_STOCHASTIC_TRANS);
	m_file.Write(wxT("\n"));
	WriteTransType(SP_DS_IMMEDIATE_TRANS);
	m_file.Write(wxT("\n"));
	WriteTransType(SP_DS_DETERMINISTIC_TRANS);
	m_file.Write(wxT("\n"));
	WriteTransType(SP_DS_SCHEDULED_TRANS);
	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportColSPN2CSV::WriteParameters()
{
	m_file.Write(wxT("Parameter:\n\n"));

	CHECK_BOOL(!m_parameterIdMap.empty(), return false);

	ParameterIdMap::iterator pIt = m_parameterIdMap.begin();
	SP_Parameter* p = pIt->second;
	SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(p->m_node->GetAttribute(wxT("ParameterList")));
	wxString l_sLine = wxT("Parameter");
	for(unsigned long i = 0; i < l_pcAttr->GetRowCount(); i++)
	{
		l_sLine << wxT(";") << l_pcAttr->GetCell(i,0);
	}
	l_sLine << wxT("\n");
	m_file.Write(l_sLine);
	for (pIt = m_parameterIdMap.begin(); pIt != m_parameterIdMap.end(); pIt++)
	{
		p = (*pIt).second;

		wxString nName = NormalizeString(p->m_name);
		if (p->m_name != nName)
		{
			SP_LOGWARNING(
					wxString::Format(wxT("parameter name %s was changed to %s"),
							p->m_name.c_str(), nName.c_str()));
		}
		l_sLine = nName;
		l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(p->m_node->GetAttribute(wxT("ParameterList")));
		for(unsigned long i = 0; i < l_pcAttr->GetRowCount(); i++)
		{
			l_sLine << wxT(";") << l_pcAttr->GetCell(i,1);
		}
		l_sLine << wxT("\n");
		m_file.Write(l_sLine);
	}

	m_file.Write(wxT("\n"));
	return TRUE;
}
