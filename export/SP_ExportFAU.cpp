//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2008/05/01 $
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportFAU.h"
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <wx/sstream.h>

#include "sp_core/base/SP_Graphic.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

bool
SP_ExportFAU::WriteGraph()
{
	bool l_bReturn = true;
	m_sContent << wxT("<model>") << wxT("\n");
	m_sContent << wxT("\t") << wxT("<NR>") << m_graph->GetNodeclass(wxT("Transition"))->GetElements()->size() << wxT("</NR>") << wxT("\n");
	m_sContent << wxT("\t") << wxT("<NC>") << m_graph->GetNodeclass(wxT("Place"))->GetElements()->size() << wxT("</NC>") << wxT("\n");
	m_sContent << wxT("\t") << wxT("<name2>") << m_graph->GetName() << wxT("</name2>") << wxT("\n");

	WritePlaces();

	WriteTransitions();

	WriteParameters();

	m_sContent << wxT("</model>") << wxT("\n");
	return l_bReturn;
}

bool SP_ExportFAU::WritePlaces()
{
	m_sContent << wxT("\t") << wxT("<species>") << wxT("\n");

	bool l_bReturn = true;
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
		m_sContent << wxT("\t") << wxT("\t") << wxT("<value>") << nName << wxT("</value>") << wxT("\n");
	}

	m_sContent << wxT("\t") << wxT("</species>") << wxT("\n");

	return l_bReturn;
}

bool SP_ExportFAU::WriteTransitions()
{
	bool l_bReturn = true;

	TransitionIdMap::iterator tIt;
	SP_Transition* t;
	for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++)
	{
		m_sContent << wxT("\t") << wxT("<class>") << wxT("\n");

		t = (*tIt).second;

		wxString nName = NormalizeString(t->m_name);
		if (t->m_name != nName)
		{
			SP_LOGWARNING(
					wxString::Format(wxT("tr name %s was changed to %s"),
							t->m_name.c_str(), nName.c_str()));
		}
		m_sContent << wxT("\t") << wxT("\t") << wxT("<name2>") << nName << wxT("</name2>") << wxT("\n");
		m_sContent << wxT("\t") << wxT("\t") << wxT("<description>") << nName << wxT("</description>") << wxT("\n");

		m_sContent << wxT("\t") << wxT("\t") << wxT("<reactants>") << wxT("\n");
		SP_DS_Node* l_pcTransition = t->m_node;
		for (SP_ListEdge::const_iterator itE = l_pcTransition->GetTargetEdges()->begin(); itE != l_pcTransition->GetTargetEdges()->end(); itE++)
		{
			SP_DS_Edge* l_pcEdge = (*itE);
			wxString l_sMultiplicity;
			long l_nMultiplicity = 0;
			if(l_pcEdge->GetAttribute(wxT("Multiplicity")))
			{
				l_sMultiplicity = l_pcEdge->GetAttribute(wxT("Multiplicity"))->GetValueString();
				l_sMultiplicity.ToLong(&l_nMultiplicity);
			}
			SP_DS_Node* l_pcSource = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetSource());
			wxString l_sSourceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcSource->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			for(long i = 0; i < l_nMultiplicity; ++i)
			{
				m_sContent << wxT("\t") << wxT("\t") << wxT("\t") << wxT("<value>") << l_sSourceName << wxT("</value>") << wxT("\n");
			}
		}
		m_sContent << wxT("\t") << wxT("\t") << wxT("</reactants>") << wxT("\n");


		m_sContent << wxT("\t") << wxT("\t") << wxT("<products>") << wxT("\n");
		for (SP_ListEdge::const_iterator itE = l_pcTransition->GetSourceEdges()->begin(); itE != l_pcTransition->GetSourceEdges()->end(); itE++)
		{
			SP_DS_Edge* l_pcEdge = (*itE);
			wxString l_sMultiplicity = l_pcEdge->GetAttribute(wxT("Multiplicity"))->GetValueString();
			long l_nMultiplicity;
			l_sMultiplicity.ToLong(&l_nMultiplicity);
			SP_DS_Node* l_pcTarget = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetTarget());
			wxString l_sTargetName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTarget->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			for(long i = 0; i < l_nMultiplicity; ++i)
			{
				m_sContent << wxT("\t") << wxT("\t") << wxT("\t") << wxT("<value>") << l_sTargetName << wxT("</value>") << wxT("\n");
			}
		}
		m_sContent << wxT("\t") << wxT("\t") << wxT("</products>") << wxT("\n");

		m_sContent << wxT("\t") << wxT("</class>") << wxT("\n");
	}

	return l_bReturn;

}

bool SP_ExportFAU::WriteParameters()
{
	bool l_bReturn = true;

	m_sContent << wxT("\t") << wxT("<parameters>") << wxT("\n");
	m_sContent << wxT("\t") << wxT("\t") << wxT("<set>") << wxT("\n");

	m_sContent << wxT("\t") << wxT("\t") << wxT("\t") << wxT("<name2>") << wxT("Set_0") << wxT("</name2>") << wxT("\n");
	m_sContent << wxT("\t") << wxT("\t") << wxT("\t") << wxT("<time>") << wxT("1") << wxT("</time>") << wxT("\n");
	m_sContent << wxT("\t") << wxT("\t") << wxT("\t") << wxT("<dump>") << wxT("1") << wxT("</dump>") << wxT("\n");
	m_sContent << wxT("\t") << wxT("\t") << wxT("\t") << wxT("<lambda>") << wxT("10000") << wxT("</lambda>") << wxT("\n");

	m_sContent << wxT("\t") << wxT("\t") << wxT("\t") << wxT("<initial>") << wxT("\n");
	PlaceIdMap::iterator pIt;
	SP_Place* p;
	for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++)
	{
		p = (*pIt).second;
		m_sContent << wxT("\t") << wxT("\t") << wxT("\t") << wxT("\t") << wxT("<value>") << p->m_marking << wxT("</value>") << wxT("\n");
	}
	m_sContent << wxT("\t") << wxT("\t") << wxT("\t") << wxT("</initial>") << wxT("\n");

	m_sContent << wxT("\t") << wxT("\t") << wxT("\t") << wxT("<deterministic>") << wxT("\n");
	for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++)
	{
		m_sContent << wxT("\t") << wxT("\t") << wxT("\t") << wxT("\t") << wxT("<value>") << wxT("false") << wxT("</value>") << wxT("\n");
	}
	m_sContent << wxT("\t") << wxT("\t") << wxT("\t") << wxT("</deterministic>") << wxT("\n");

	m_sContent << wxT("\t") << wxT("\t") << wxT("\t") << wxT("<constants>") << wxT("\n");
	TransitionIdMap::iterator tIt;
	SP_Transition* t;
	for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++)
	{
		t = (*tIt).second;
		wxString l_sResultFunction;

		if(!t->m_function.IsEmpty())
		{
			wxString l_sFunction = t->m_function;
			l_sFunction.Replace(wxT(" "), wxT(""));
			l_sFunction.Replace(wxT("\t"), wxT(""));
			l_sFunction.Replace(wxT("\r"), wxT(""));
			l_sFunction.Replace(wxT("\n"), wxT(""));

			if(l_sFunction.StartsWith(wxT("MassAction"), &l_sResultFunction))
			{
				l_sResultFunction.Replace(wxT("("), wxT(""));
				l_sResultFunction.Replace(wxT(")"), wxT(""));
				l_sFunction = l_sResultFunction;
				l_sResultFunction.Clear();
				ParameterMap::iterator l_itParam = m_parameterNameMap.find(l_sFunction);
				if(l_itParam != m_parameterNameMap.end())
				{
					l_sResultFunction << l_itParam->second->m_marking;
				}
				else if(l_sFunction.IsNumber())
				{
					l_sResultFunction << l_sFunction;
				}
			}
			else if(l_sFunction.IsNumber())
			{
				l_sResultFunction = l_sFunction;
			}

		}
		m_sContent << wxT("\t") << wxT("\t") << wxT("\t") << wxT("\t") << wxT("<value>") << l_sResultFunction << wxT("</value>") << wxT("\n");
	}
	m_sContent << wxT("\t") << wxT("\t") << wxT("\t") << wxT("</constants>") << wxT("\n");

	m_sContent << wxT("\t") << wxT("\t") << wxT("</set>") << wxT("\n");
	m_sContent << wxT("\t") << wxT("</parameters>") << wxT("\n");


	return l_bReturn;

}

bool SP_ExportFAU::DoWrite()
{
	bool l_bReturn = true;
	m_sContent = wxT("<?xml version='1.0'?>\n");
	if((l_bReturn = WriteGraph()))
	{
		l_bReturn = m_file.Write(m_sContent);
	}



	return l_bReturn;
}

bool SP_ExportFAU::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_PN_CLASS ||
			cName == SP_DS_EXTPN_CLASS ||
			cName == SP_DS_SPN_CLASS);
}

