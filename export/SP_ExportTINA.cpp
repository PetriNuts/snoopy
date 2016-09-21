//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/11/15 $
// Short Description: export of P/T nets to TINA
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportTINA.h"
#include "sp_utilities.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

SP_ExportTINA::SP_ExportTINA()
{
}

SP_ExportTINA::~SP_ExportTINA()
{
}

bool SP_ExportTINA::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_PN_CLASS ||
			cName == SP_DS_TIMEPN_CLASS);

}

bool SP_ExportTINA::DoWrite()
{
	m_file.Write(wxT("net ") + GetNetName(m_fileName) + wxT("\n"));

	WriteTransitions();
	WritePlaces();


	return !m_bError;
}

bool SP_ExportTINA::WriteTransitions()
{

	TransitionIdMap::iterator tIt;
	SP_Transition* t;
	list<SP_DiscrArc*>* aList;
	list<SP_DiscrArc*>::iterator aIt;
	SP_DiscrArc* a;

	wxString nName;
	for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++)
	{
		t = (*tIt).second;

		nName = NormalizeString(t->m_name);
		if (t->m_name != nName)
		{
			SP_LOGWARNING(
					wxString::Format(wxT("tr name %s was changed to %s"),
							t->m_name.c_str(), nName.c_str()));
		}

		wxString l_strInterval = wxT("[0,0]");
		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(t->m_node->GetAttribute(wxT("Interval")));
		if(l_pcAttr)
		{
			l_strInterval = wxT("[") + l_pcAttr->GetCell(l_pcAttr->GetActiveList(),1) + wxT(",") + l_pcAttr->GetCell(l_pcAttr->GetActiveList(),2) + wxT("]");
			l_strInterval.Replace(wxT("oo]"), wxT("w["));
		}
		m_file.Write(wxString::Format(wxT("tr %s %s "), nName.c_str(), l_strInterval.c_str()));

		aList = m_preTrMap[t->m_id];
		if (aList)
		{
			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				a = (*aIt);
				nName = NormalizeString(m_placeIdMap[a->m_id]->m_name);
				if (a->m_mult > 1)
				{
					m_file.Write(wxString::Format(wxT("%s*%u "), nName.c_str(),
							a->m_mult));
				}
				else
				{
					m_file.Write(wxString::Format(wxT("%s "), nName.c_str()));
				}
			}
		}
		m_file.Write(wxT("-> "));
		aList = m_postTrMap[t->m_id];
		if (aList)
		{
			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				a = (*aIt);
				nName = NormalizeString(m_placeIdMap[a->m_id]->m_name);
				if (a->m_mult > 1)
				{
					m_file.Write(wxString::Format(wxT("%s*%u "), nName.c_str(),
							a->m_mult));
				}
				else
				{
					m_file.Write(wxString::Format(wxT("%s "), nName.c_str()));
				}
			}
		}
		m_file.Write(wxT("\n"));
	}
	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportTINA::WritePlaces()
{

	PlaceIdMap::iterator pIt;
	SP_Place* p;
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
		m_file.Write(wxString::Format(wxT("pl %s (%u)\n"), nName.c_str(),
				p->m_marking));
	}

	m_file.Write(wxT("\n"));
	return TRUE;
}

