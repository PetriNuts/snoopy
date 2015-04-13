//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/05/22 $
// Short Description: export to apnn
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportAPNN.h"
#include "sp_defines.h"

#include "sp_ds/SP_DS_Edge.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"

#include <wx/tokenzr.h>

SP_ExportAPNN::SP_ExportAPNN()
{
}

SP_ExportAPNN::~SP_ExportAPNN()
{
}

bool SP_ExportAPNN::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_PN_CLASS ||
			cName == SP_DS_EXTPN_CLASS ||
			cName == SP_DS_TIMEPN_CLASS ||
			cName == SP_DS_SPN_CLASS );
}

bool SP_ExportAPNN::DoWrite()
{
	m_file.Write(wxT("\\beginnet{") + GetNetName(m_fileName) + wxT("}\n\n"));
	WriteConstants();
	WriteParameters();
	WritePlaces();
	WriteTransitions();
	WriteArcs();



	m_file.Write(wxT("\n"));
	m_file.Write(wxT("\\endnet\n"));
	return !m_bError;
}

bool SP_ExportAPNN::WritePlaces()
{
	PlaceIdMap::iterator pIt;
	SP_Place* p;
	for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++)
	{
		p = (*pIt).second;

		wxString nName = NormalizeString(p->m_name);
		wxString nValue = p->m_value;
		if (p->m_name != nName)
		{
			SP_LOGWARNING(
					wxString::Format(wxT("place name %s was changed to %s"),
							p->m_name.c_str(), nName.c_str()));
		}

		if (p->m_value.IsEmpty())
		{
			m_file.Write(wxString::Format(wxT("\\place{P_%ld}{\\name{%s}")
				wxT("\\init{%u}}\n"), p->m_id, nName.c_str(), p->m_marking));
		} else
		{
			m_file.Write(wxString::Format(wxT("\\place{P_%ld}{\\name{%s}")
				wxT("\\init{%s}}\n"), p->m_id, nName.c_str(), nValue.c_str()));
		}
	}

	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportAPNN::WriteTransitions()
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
		wxString l_sBuffer;
		l_sBuffer << wxT("\\transition{T_") << t->m_id << wxT("}{");
		l_sBuffer << wxT("\\name{") << nName << wxT("}");
		if(!t->m_function.IsEmpty())
		{
			wxString l_sFunction = t->m_function;
			l_sFunction.Replace(wxT(" "), wxT(""));
			l_sFunction.Replace(wxT("\t"), wxT(""));
			l_sFunction.Replace(wxT("\r"), wxT(""));
			l_sFunction.Replace(wxT("\n"), wxT(""));

			l_sBuffer << wxT(" \\function{") << l_sFunction << wxT("}");
/*
			wxString l_sResultFunction;
			wxString l_sToken;
			for(size_t i = 0; i < l_sFunction.Len(); i++)
			{
				wxString l_pchCurrent = l_sFunction.Mid(i,1);
				if(l_pchCurrent == wxT("(") ||
					l_pchCurrent == wxT(")") ||
					l_pchCurrent == wxT("*") ||
					l_pchCurrent == wxT("+") ||
					l_pchCurrent == wxT(",") ||
					l_pchCurrent == wxT("-") ||
					l_pchCurrent == wxT("/") ||
					l_pchCurrent == wxT("^"))
				{
					if(!l_sToken.IsEmpty())
					{
						ParameterMap::iterator l_itParam = m_parameterNameMap.find(l_sToken);
						if(l_itParam != m_parameterNameMap.end())
						{
							l_sResultFunction << l_itParam->second->m_marking;
						}
						else
						{
							l_sResultFunction << l_sToken;
						}
						l_sToken.Clear();
					}
					l_sResultFunction << l_pchCurrent;
				}
				else
				{
					l_sToken << l_pchCurrent;
				}
			}
			if(!l_sToken.IsEmpty())
			{
				ParameterMap::iterator l_itParam = m_parameterNameMap.find(l_sToken);
				if(l_itParam != m_parameterNameMap.end())
				{
					l_sResultFunction << l_itParam->second->m_marking;
				}
				else
				{
					l_sResultFunction << l_sToken;
				}
			}
			l_sBuffer << wxT(" \\function{") << l_sResultFunction << wxT("}");
*/
		}
		l_sBuffer << wxT("}\n");
		m_file.Write(l_sBuffer);
	}

	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportAPNN::WriteConstants()
{
	ConstantMap::iterator l_itConst;

	for(l_itConst = m_constantNameMap.begin(); l_itConst != m_constantNameMap.end(); ++l_itConst)
	{
		SP_Constant *pa = l_itConst->second;
		wxString name = l_itConst->first;
		wxString type = pa->m_type;
		if (type.Cmp(wxT("int")) == 0) {
			type = wxT("integer");
		}

		wxString l_sBuffer = wxT("\\const{");
		l_sBuffer << wxT("\\type{") << type << wxT("}");
		l_sBuffer << wxT("\\name{") << name << wxT("}");
		l_sBuffer << wxT("\\value{") << pa->m_value << wxT("}");
		l_sBuffer << wxT("}\n");
		m_file.Write(l_sBuffer);
	}
	m_file.Write(wxT("\n"));

	return TRUE;
}

bool SP_ExportAPNN::WriteParameters()
{
	ParameterMap::iterator l_itParam;

	for(l_itParam = m_parameterNameMap.begin(); l_itParam != m_parameterNameMap.end(); ++l_itParam)
	{
		SP_Parameter *pa = l_itParam->second;
		wxString name = l_itParam->first;

		wxString l_sBuffer = wxT("\\const{");
		l_sBuffer << wxT("\\type{double}");
		l_sBuffer << wxT("\\name{") << name << wxT("}");
		l_sBuffer << wxT("\\value{") << pa->m_marking << wxT("}");
		l_sBuffer << wxT("}\n");
		m_file.Write(l_sBuffer);
	}
	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportAPNN::WriteArcs()
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
				if ((a->m_edge->GetClassName()) == wxT("Read Edge"))
				{
					atype = wxT("readarc");
				}
				else if ((a->m_edge->GetClassName()) == wxT("Inhibitor Edge"))
				{
					atype = wxT("inhibitor");
				}
				else if ((a->m_edge->GetClassName()) == wxT("Reset Edge"))
				{
					atype = wxT("resetarc");
				}
				else if ((a->m_edge->GetClassName()) == wxT("Equal Edge"))
				{
					atype = wxT("equalarc");
				}
				else if ((a->m_edge->GetClassName()) == wxT("Modifier Edge"))
				{
					atype = wxT("modifierarc");
				}

				//changed by sl
#if 1
				 	m_file.Write(wxString::Format(
						wxT("\\arc{A_%u}{\\from{P_%ld} \\to{T_%ld}")
							wxT(" \\weight{%u} \\type{%s}}\n"), aNum, a->m_id,
						t->m_id, a->m_mult, atype.c_str()));
#else
				m_file.Write(wxString::Format(
					wxT("\\arc{A_%u}{\\from{P_%ld} \\to{T_%ld}")
						wxT(" \\weight{%s} \\type{%s}}\n"), aNum, a->m_id,
					t->m_id, a->m_value.c_str() , atype.c_str()));
#endif
				aNum++;
			}
		}
		aList = m_postTrMap[t->m_id];
		if (aList)
		{
			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				a = (*aIt);

				//changed by sl
#if 1
				m_file.Write(wxString::Format(
						wxT("\\arc{A_%u}{\\from{T_%ld} \\to{P_%ld}")
							wxT(" \\weight{%u} \\type{ordinary}}\n"), aNum, t->m_id,
						a->m_id, a->m_mult));
#else
				m_file.Write(wxString::Format(
					wxT("\\arc{A_%u}{\\from{T_%ld} \\to{P_%ld}")
						wxT(" \\weight{%s} \\type{ordinary}}\n"), aNum, t->m_id,
					a->m_id, a->m_value.c_str()));
#endif
				aNum++;
			}
		}
	}
	m_file.Write(wxT("\n"));
	return TRUE;
}
