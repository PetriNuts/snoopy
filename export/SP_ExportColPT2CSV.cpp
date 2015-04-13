//////////////////////////////////////////////////////////////////////
// $Author: fei liu $
// $Version: 0.1 $
// $Date: 2010/05/26 $
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportColPT2CSV.h"
#include "sp_utilities.h"

#include "sp_ds/SP_DS_Edge.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include <wx/tokenzr.h>

SP_ExportColPT2CSV::SP_ExportColPT2CSV()
{
}

SP_ExportColPT2CSV::~SP_ExportColPT2CSV()
{
}

bool SP_ExportColPT2CSV::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return ( cName == SP_DS_COLPN_CLASS );
}

bool SP_ExportColPT2CSV::DoWrite()
{
	WriteDeclarations();

	WritePlaces();

	return !m_bError;
}

bool SP_ExportColPT2CSV::WritePlaces()
{
	CHECK_BOOL(!m_placeIdMap.empty(), return false);

	PlaceIdMap::iterator pIt = m_placeIdMap.begin();
	SP_Place* p = pIt->second;
	SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(p->m_node->GetAttribute(wxT("MarkingList")));
	wxString l_sLine = wxT("Place");
	
	for(unsigned long i = 0; i < l_pcAttr->GetColCount(); i++)
	{
		l_sLine << wxT(";") << l_pcAttr->GetColLabel(i);
	}

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
		l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(p->m_node->GetAttribute(wxT("MarkingList")));
		
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

	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportColPT2CSV::WriteTransitions()
{
	CHECK_BOOL(!m_trIdMap.empty(), return false);

	TransitionIdMap::iterator tIt = m_trIdMap.begin();
	SP_Transition* t = tIt->second;
	SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(t->m_node->GetAttribute(wxT("FunctionList")));
	wxString l_sLine = wxT("Transition");
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
		l_sLine << wxT("\n");
		m_file.Write(l_sLine);
	}

	m_file.Write(wxT("\n"));
	return TRUE;
}


