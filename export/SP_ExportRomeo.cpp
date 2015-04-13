//////////////////////////////////////////////////////////////////////
// $Author: ms $
// $Version: 0.1 $
// $Date: 2007/06/7 $
// Short Description: export to romeo
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportRomeo.h"
#include "sp_defines.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"

SP_ExportRomeo::SP_ExportRomeo()
{
}

SP_ExportRomeo::~SP_ExportRomeo()
{
}

bool SP_ExportRomeo::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_TIMEPN_CLASS);
}

bool SP_ExportRomeo::DoWrite()
{
	m_file.Write(wxT("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
	m_file.Write(wxT("<TPN name=\"test.xml\">\n"));
	WritePlaces();
	WriteTransitions();

	WriteArcs();
	m_file.Write(wxT("</TPN>"));


	return !m_bError;
}

bool SP_ExportRomeo::WritePlaces()
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
		m_file.Write(wxString::Format(
				wxT("\n<place id=\"%ld\" label=\"%s\" initialMarking=\"%u\">\n"),
				p->m_id+1, nName.c_str(), p->m_marking));
		//m_file.Write(wxString::Format(wxT("\t<graphics>\n")));
		//m_file.Write(wxString::Format(wxT("<position x=\"%f\" y=\"%f\">\n") ,p->m_node->GetGraphicInSubNet(0)->GetPosX() ,p->m_node->GetGraphicInSubnet(0)->GetPosY()));

		//graphics

		//m_file.Write(wxString::Format(wxT("\t</graphics>\n")));

		m_file.Write(wxString::Format(wxT("</place>\n")));
	}
	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportRomeo::WriteTransitions()
{

	TransitionIdMap::iterator tIt;
	SP_Transition* t;
	for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++)
	{
		t = (*tIt).second;

		wxString nName = NormalizeString(t->m_name);
		if (t->m_name != nName)
		{
			SP_LOGWARNING(
					wxString::Format(wxT("tr name %s was changed to %s"),
							t->m_name.c_str(), nName.c_str()));
		}

		wxString lft = t->m_node->GetAttribute(wxT("LFT"))->GetValueString();
		if (lft.Cmp(wxT("?"))==0)
		{
			lft = wxT("infini");
		}
		else
		{
			lft =t->m_node->GetAttribute(wxT("LFT"))->GetValueString();
		}
		wxString eft = t->m_node->GetAttribute(wxT("EFT"))->GetValueString();
		if (eft.Cmp(wxT("?"))==0)
		{
			eft = wxT("0");
		}
		else
		{
			eft =t-> m_node->GetAttribute(wxT("EFT"))->GetValueString();
		}

		wxString toWrite = wxString::Format(
				wxT("\n<transition id=\"%ld\" label=\"%s\""), t->m_id+1,
				nName.c_str());
		toWrite+= wxT("eft=\"");
		toWrite+ eft;
		toWrite+=wxT("\" lft=\"");
		toWrite+=lft;
		toWrite+=wxT("\">\n");
		m_file.Write(toWrite);

		m_file.Write(wxString::Format(wxT("</transition>\n")));

	}

	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportRomeo::WriteArcs()
{
	TransitionIdMap::iterator tIt;
	SP_Transition* t;
	list<SP_DiscrArc*>* aList;
	list<SP_DiscrArc*>::iterator aIt;
	SP_DiscrArc* a;
	unsigned aNum = 0;
	wxString atype;

	for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++)
	{
		t = (*tIt).second;
		aList = m_preTrMap[t->m_id];
		if (aList)
		{
			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				a = (*aIt);

				atype = wxT("ordinary");
				if (a->m_edge->GetClassName() == wxT("Read Edge"))
				{
					atype = wxT("readarc");
				}
				else if (a->m_edge->GetClassName() == wxT("Inhibitor Edge"))
				{
					atype = wxT("inhibitor");
				}
				else if (a->m_edge->GetClassName() == wxT("Reset Edge"))
				{
					atype = wxT("resetarc");
				}
				else if (a->m_edge->GetClassName() == wxT("Equal Edge"))
				{
					atype = wxT("equalarc");
				}
				m_file.Write(wxString::Format(
						wxT("\n<arc place=\"%ld\" transition=\"%ld\" type=\"PlaceTransition\" weight=\"%u\">\n"),
						a->m_id+1, t->m_id+1, a->m_mult));
				m_file.Write(wxT("</arc>\n"));
				aNum++;
			}
		}
		aList = m_postTrMap[t->m_id];
		if (aList)
		{
			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				a = (*aIt);
				m_file.Write(wxString::Format(
						wxT("\n<arc place=\"%ld\" transition=\"%ld\" type=\"TransitionPlace\" weight=\"%u\">\n"),
						a->m_id+1, t->m_id+1, a->m_mult));
				m_file.Write(wxT("</arc>\n"));
				aNum++;
			}
		}
	}
	m_file.Write(wxT("\n"));
	return TRUE;
}
