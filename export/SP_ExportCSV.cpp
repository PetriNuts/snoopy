//////////////////////////////////////////////////////////////////////
// $Author: cr $
// $Version: 0.1 $
// $Date: 2009/07/03 $
// Short Description: export to csv
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportCSV.h"
#include "sp_utilities.h"

#include "sp_ds/SP_DS_Edge.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include <wx/tokenzr.h>

SP_ExportCSV::SP_ExportCSV()
{
}

SP_ExportCSV::~SP_ExportCSV()
{
}

bool SP_ExportCSV::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return ( cName == SP_DS_SPN_CLASS );
}

bool SP_ExportCSV::DoWrite()
{

	WritePlaces();
	WriteTransitions();
	WriteParameters();

	return !m_bError;
}

bool SP_ExportCSV::WritePlaces()
{
	CHECK_BOOL(!m_placeIdMap.empty(), return false);

	PlaceIdMap::iterator pIt = m_placeIdMap.begin();
	SP_Place* p = pIt->second;
	wxString l_sLine = SP_DS_DISCRETE_PLACE;
	l_sLine << wxT(";Marking");
	l_sLine << wxT("\n");
	m_file.Write(l_sLine);
	for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++)
	{
		p = (*pIt).second;

		wxString nName = NormalizeString(p->m_name);
		if (p->m_name != nName)
		{
			SP_LOGWARNING(
					wxString::Format(wxT("place name %s was changed to %s"),
							p->m_name.c_str(), nName.c_str()));
		}
		l_sLine = nName;
		l_sLine << wxT(";");
		l_sLine << p->m_value;
		l_sLine << wxT("\n");
		m_file.Write(l_sLine);
	}

	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportCSV::WriteTransitions()
{
	CHECK_BOOL(!m_trIdMap.empty(), return false);

	TransitionIdMap::iterator tIt = m_trIdMap.begin();
	SP_Transition* t = tIt->second;
	SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(t->m_node->GetAttribute(wxT("FunctionList")));
	wxString l_sLine = SP_DS_DISCRETE_TRANS;
	for(unsigned long i = 0; i < l_pcAttr->GetRowCount(); i++)
	{
		l_sLine << wxT(";") << l_pcAttr->GetCell(i,0);
	}
	l_sLine << wxT("\n");
	m_file.Write(l_sLine);
	for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++)
	{
		t = (*tIt).second;

		wxString nName = NormalizeString(t->m_name);
		if (t->m_name != nName)
		{
			SP_LOGWARNING(
					wxString::Format(wxT("transition name %s was changed to %s"),
							t->m_name.c_str(), nName.c_str()));
		}
		l_sLine = nName;
		l_sLine << wxT(";");
		l_sLine << t->m_function;
/*
		l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(t->m_node->GetAttribute(wxT("FunctionList")));
		if(l_pcAttr)
		{
			for(unsigned long i = 0; i < l_pcAttr->GetRowCount(); i++)
			{
				wxString l_sFunction = l_pcAttr->GetCell(i,1);
				l_sFunction.Replace(wxT("\n"), wxT(""));
				l_sLine << wxT(";") << l_sFunction;
			}
		}
*/
		l_sLine << wxT("\n");
		m_file.Write(l_sLine);
	}

	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportCSV::WriteParameters()
{
	if(!m_constantIdMap.empty())
	{
		ConstantIdMap::iterator pIt = m_constantIdMap.begin();
		SP_Constant* p = pIt->second;
		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(p->m_node->GetAttribute(wxT("ValueList")));
		wxString l_sLine = SP_DS_META_CONSTANT;
		for(unsigned long i = 0; i < l_pcAttr->GetRowCount(); i++)
		{
			l_sLine << wxT(";") << l_pcAttr->GetCell(i,0);
		}
		l_sLine << wxT("\n");
		m_file.Write(l_sLine);
		for (pIt = m_constantIdMap.begin(); pIt != m_constantIdMap.end(); pIt++)
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
			l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(p->m_node->GetAttribute(wxT("ValueList")));
			for(unsigned long i = 0; i < l_pcAttr->GetRowCount(); i++)
			{
				l_sLine << wxT(";") << l_pcAttr->GetCell(i,1);
			}
			l_sLine << wxT("\n");
			m_file.Write(l_sLine);
		}
		m_file.Write(wxT("\n"));
	}
/*
	if(!m_parameterIdMap.empty())
	{
		ParameterIdMap::iterator pIt = m_parameterIdMap.begin();
		SP_Parameter* p = pIt->second;
		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(p->m_node->GetAttribute(wxT("ParameterList")));
		wxString l_sLine = SP_DS_PARAM;
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
	}
*/
	return TRUE;
}
