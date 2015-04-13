//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2013/04/30 $
// Short Description: export to Matlab
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportSPN2Matlab.h"
#include "sp_defines.h"

#include "sp_ds/SP_DS_Edge.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

#include <wx/tokenzr.h>

SP_ExportSPN2Matlab::SP_ExportSPN2Matlab()
{
}

SP_ExportSPN2Matlab::~SP_ExportSPN2Matlab()
{
}

bool SP_ExportSPN2Matlab::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_SPN_CLASS || cName == SP_DS_PN_CLASS || cName == SP_DS_EXTPN_CLASS);
}

bool SP_ExportSPN2Matlab::DoWrite()
{
	m_file.Write(wxT("%% ") + GetNetName(m_fileName) + wxT("\n"));
	m_file.Write(wxT("\n"));
	m_file.Write(wxT("clear all;\n"));
	m_file.Write(wxT("close all;\n"));
	m_file.Write(wxT("clc;\n"));
	m_file.Write(wxT("\n"));
	m_file.Write(wxT("%% Petri net\n"));
	m_file.Write(wxT("pn.Name = '") + GetNetName(m_fileName) + wxT("';\n"));
	m_file.Write(wxT("\n"));

	WriteConstants();
	WritePlaces();
	WriteTransitions();

	m_file.Write(wxT("\n"));
	m_file.Write(wxT("\n"));
	m_file.Write(wxT("\n"));
	m_file.Write(wxT("% Simulation parameter\n"));
	m_file.Write(wxT("pn.t_start = 0; %start time\n"));
	m_file.Write(wxT("pn.t_end = 100; %end time\n"));
	m_file.Write(wxT("pn.t_step = 1; %sample interval for gathering data (optimal, only for SPN)\n"));
	m_file.Write(wxT("pn.runs = 1; % number of simulation runs (optimal, only for SPN)\n"));
	m_file.Write(wxT("\n"));
	m_file.Write(wxT("%% show PN\n"));
	m_file.Write(wxT("pn\n"));
	m_file.Write(wxT("\n"));
	m_file.Write(wxT("%CPN2Matlab(pn);\n"));
	m_file.Write(wxT("%SPN2Matlab(pn);\n"));



	return !m_bError;
}

bool SP_ExportSPN2Matlab::WritePlaces()
{
	wxString l_sPlaceVector = wxT("pn.P = {");
	wxString l_sMarkingVector = wxT("pn.m0 = [");
	PlaceIdMap::iterator pIt;
	SP_Place* p;
	for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++)
	{
		if(pIt != m_placeIdMap.begin())
		{
			l_sPlaceVector << wxT(", ");
			l_sMarkingVector << wxT(", ");
		}
		p = (*pIt).second;

		wxString nName = NormalizeString(p->m_name);
		wxString nValue = p->m_value;
		if (p->m_name != nName)
		{
			SP_LOGWARNING(
					wxString::Format(wxT("place name %s was changed to %s"),
							p->m_name.c_str(), nName.c_str()));
		}
		l_sPlaceVector << wxT("'") << nName << wxT("'");
//		unsigned nMarking = p->m_marking;
//		l_sMarkingVector << wxString::Format(wxT("%u"), nMarking);
		l_sMarkingVector << SubstituteNames(nValue);
	}
	l_sPlaceVector << wxT("};\n");
	l_sMarkingVector << wxT("];\n");

	m_file.Write(wxT("% place vector\n"));
	m_file.Write(l_sPlaceVector);
	m_file.Write(wxT("% marking vector\n"));
	m_file.Write(l_sMarkingVector);

	return TRUE;
}

bool SP_ExportSPN2Matlab::WriteTransitions()
{
	wxString l_sTransVector = wxT("pn.T = {");
	wxString l_sFunctions;
	wxString l_sFuncVector = wxT("pn.F = {");

	TransitionIdMap::iterator tIt;
	SP_Transition* t;
	for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++)
	{
		if(tIt != m_trIdMap.begin())
		{
			l_sTransVector << wxT(", ");
			l_sFuncVector << wxT(", ");
		}
		t = (*tIt).second;

		wxString nName = NormalizeString(t->m_name);
		if (t->m_name != nName)
		{
			SP_LOGWARNING(
					wxString::Format(wxT("tr name %s was changed to %s"),
							t->m_name.c_str(), nName.c_str()));
		}
		l_sTransVector << wxT("'") << nName << wxT("'");
		l_sFuncVector << wxT("pn.") << nName;
		wxString l_sFunction = wxT("1");
		if(!t->m_function.IsEmpty())
		{
			l_sFunction = t->m_function;
			l_sFunction.Replace(wxT(" "), wxT(""));
			l_sFunction.Replace(wxT("\t"), wxT(""));
			l_sFunction.Replace(wxT("\r"), wxT(""));
			l_sFunction.Replace(wxT("\n"), wxT(""));

			l_sFunction = SubstituteNames(l_sFunction);
			if(l_sFunction.Contains(wxT("MassAction")))
			{
				l_sFunction = l_sFunction.BeforeLast(')');
				l_sFunction << wxT(", m, pn.L(:,") << t->m_id+1 << wxT(")')");
			}
		}
		l_sFunctions << wxT("pn.") << nName << wxT(" = @(m) ") << l_sFunction << wxT(";\n");
	}
	l_sTransVector << wxT("};\n");
	l_sFuncVector << wxT("};\n");
	m_file.Write(wxT("% transition vector\n"));
	m_file.Write(l_sTransVector);

	wxString l_sPreTrans = wxT("pn.PreArcs = [\n");
	wxString l_sPostTrans = wxT("pn.PostArcs = [\n");
	wxString l_sRead = wxT("pn.ReadArcs = [\n");
	wxString l_sInhib = wxT("pn.InhibitorArcs = [\n");
	wxString l_sReset = wxT("pn.ResetArcs = [\n");
	wxString l_sModif = wxT("pn.ModifierArcs = [\n");

	PlaceIdMap::iterator pIt;
	SP_Place* p;
	list<SP_DiscrArc*>* aList;
	list<SP_DiscrArc*>::iterator aIt;
	SP_DiscrArc* a;
	wxString atype;

	for(pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); ++pIt)
	{
		SP_VectorULong pre(m_trIdMap.size(), 0);
		SP_VectorULong post(m_trIdMap.size(), 0);
		SP_VectorULong read(m_trIdMap.size(), 0);
		SP_VectorULong inhib(m_trIdMap.size(), 0);
		SP_VectorBool reset(m_trIdMap.size(), false);
		SP_VectorBool modif(m_trIdMap.size(), false);

		p = pIt->second;
		aList = m_postPlMap[p->m_id];
		if(aList)
		{
			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				a = (*aIt);
				if ((a->m_edge->GetClassName()) == wxT("Edge"))
				{
					pre[a->m_id] = a->m_mult;
				}
				else if ((a->m_edge->GetClassName()) == wxT("Read Edge"))
				{
					read[a->m_id] = a->m_mult;
				}
				else if ((a->m_edge->GetClassName()) == wxT("Inhibitor Edge"))
				{
					inhib[a->m_id] = a->m_mult;
				}
				else if ((a->m_edge->GetClassName()) == wxT("Reset Edge"))
				{
					reset[a->m_id] = true;
				}
				else if ((a->m_edge->GetClassName()) == wxT("Equal Edge"))
				{
					read[a->m_id] = a->m_mult;
					inhib[a->m_id] = a->m_mult+1;
				}
				else if ((a->m_edge->GetClassName()) == wxT("Modifier Edge"))
				{
					modif[a->m_id] = true;
				}
			}
		}

		aList = m_prePlMap[p->m_id];
		if (aList)
		{
			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				a = (*aIt);
				post[a->m_id] = a->m_mult;
			}
		}

		for (unsigned i = 0; i < m_trIdMap.size(); ++i)
		{
			if(i > 0)
			{
				l_sPreTrans << wxT(",");
				l_sPostTrans << wxT(",");
				l_sRead << wxT(",");
				l_sInhib << wxT(",");
				l_sReset << wxT(",");
				l_sModif << wxT(",");
			}

			l_sPreTrans << pre[i];
			l_sPostTrans << post[i];
			l_sRead << read[i];
			l_sInhib << inhib[i];
			l_sReset << reset[i];
			l_sModif << modif[i];
		}
		l_sPreTrans << wxT(";\n");
		l_sPostTrans << wxT(";\n");
		l_sRead << wxT(";\n");
		l_sInhib << wxT(";\n");
		l_sReset << wxT(";\n");
		l_sModif << wxT(";\n");

	}
	l_sPreTrans << wxT("];\n");
	l_sPostTrans << wxT("];\n");
	l_sRead << wxT("];\n");
	l_sInhib << wxT("];\n");
	l_sReset << wxT("];\n");
	l_sModif << wxT("];\n");

	m_file.Write(l_sPreTrans);
	m_file.Write(l_sPostTrans);
	m_file.Write(wxT("% stochiometric matrix (standard edges with weights)\n"));
	m_file.Write(wxT("pn.S = pn.PostArcs - pn.PreArcs;\n"));
	m_file.Write(l_sRead);
	m_file.Write(wxT("pn.L = max(pn.PreArcs, pn.ReadArcs);\n"));
	m_file.Write(l_sInhib);
	m_file.Write(l_sReset);
	m_file.Write(l_sModif);
	m_file.Write(wxT("\n"));
	m_file.Write(wxT("% functions\n"));
	m_file.Write(l_sFunctions);
	m_file.Write(wxT("% function vector\n"));
	m_file.Write(l_sFuncVector);
	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportSPN2Matlab::WriteConstants()
{
	wxString l_sConstants = wxT("% definition of constants\n");

	ConstantIdMap::iterator l_itConst;

	for(l_itConst = m_constantIdMap.begin(); l_itConst != m_constantIdMap.end(); ++l_itConst)
	{
		SP_Constant *pa = l_itConst->second;
		wxString name = pa->m_name;
		wxString type = pa->m_type;

		l_sConstants << wxT("pn.") << name;
		l_sConstants << wxT(" = ") << pa->m_value;
		l_sConstants << wxT(";\n");
	}

	ParameterIdMap::iterator l_itParam;

	for(l_itParam = m_parameterIdMap.begin(); l_itParam != m_parameterIdMap.end(); ++l_itParam)
	{
		SP_Parameter *pa = l_itParam->second;
		wxString name = pa->m_name;

		l_sConstants << wxT(" pn.") << name;
		l_sConstants << wxT(" = ") << pa->m_marking;
		l_sConstants << wxT(";\n");
	}
	m_file.Write(l_sConstants);
	m_file.Write(wxT("\n"));
	return TRUE;
}

wxString
SP_ExportSPN2Matlab::SubstituteNames(const wxString& p_Func)
{
	wxString l_sResultFunction;
	wxStringTokenizer l_StringTok(p_Func, wxT("()+*/^=<>!%&|,-"),  wxTOKEN_RET_EMPTY_ALL );
	while(l_StringTok.HasMoreTokens())
	{
		wxString l_sToken = l_StringTok.GetNextToken();
		if(!l_sToken.IsEmpty())
		{
			PlaceMap::const_iterator it = m_placeNameMap.find(l_sToken);
			if(it != m_placeNameMap.end())
			{
				l_sResultFunction << wxT("m(") << it->second->m_id+1 << wxT(")");
			}
			else if(m_constantNameMap.find(l_sToken) != m_constantNameMap.end())
			{
				l_sResultFunction << wxT("pn.") << l_sToken;
			}
			else if(m_parameterNameMap.find(l_sToken) != m_parameterNameMap.end())
			{
				l_sResultFunction << wxT("pn.") << l_sToken;
			}
			else
			{
				l_sResultFunction << l_sToken;
			}
		}
		wxChar l_chDelim = l_StringTok.GetLastDelimiter();
		if(l_chDelim != '\0')
		{
			l_sResultFunction << l_chDelim;
		}
	}
	return l_sResultFunction;
}
