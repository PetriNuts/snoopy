//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2013/08/30 $
// Short Description: export to C++
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportCPP.h"
#include "sp_utilities.h"


#include "sp_ds/SP_DS_Edge.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

#include <wx/tokenzr.h>

SP_ExportCPP::SP_ExportCPP()
{
}

SP_ExportCPP::~SP_ExportCPP()
{
}

bool SP_ExportCPP::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (
//			cName == SP_DS_PN_CLASS ||
//			cName == SP_DS_EXTPN_CLASS ||
//			cName == SP_DS_TIMEPN_CLASS ||
			cName == SP_DS_SPN_CLASS
//			|| cName == SP_DS_CONTINUOUSPED_CLASS
//			|| cName == SP_DS_HYBRIDPN_CLASS
			);
}

namespace
{
	wxString newLine = wxT("\n");
	wxString placeCount = wxT("placeCount");
	wxString transCount = wxT("transCount");
	wxString placeNames = wxT("placeNames");
	wxString initMarking = wxT("initMarking");
	wxString transNames = wxT("transNames");
	wxString transRates = wxT("transRates");
	wxString transToUpdate = wxT("transToUpdate");
	wxString delta = wxT("delta");
	wxString pre = wxT("pre");
	wxString infti = wxT("infti");
}

bool SP_ExportCPP::DoWrite()
{
	wxString l_sInclude = wxT("#include <iosfwd>") + newLine;
	l_sInclude << wxT("#include <functional>") + newLine;
	l_sInclude << wxT("#include <string>") + newLine;
	l_sInclude << wxT("#include <array>") + newLine;
	l_sInclude << wxT("#include <vector>") + newLine;
	m_file.Write(l_sInclude);
	m_file.Write(newLine);

	wxString l_sInfti = wxT("#define ");
	l_sInfti << infti << wxT(" ") << std::numeric_limits<unsigned>::max() << newLine;
	m_file.Write(l_sInfti);

	wxString l_sPlaceNr = wxT("#define ");
	l_sPlaceNr << placeCount << wxT(" ") << m_placeIdMap.size() << newLine;
	m_file.Write(l_sPlaceNr);

	wxString l_sTransNr = wxT("#define ");
	l_sTransNr << transCount << wxT(" ") << m_trIdMap.size() << newLine;
	m_file.Write(l_sTransNr);
	m_file.Write(newLine);

	m_file.Write(wxT("typedef unsigned int Token;"));
	m_file.Write(newLine);
	m_file.Write(wxT("typedef std::array<Token, placeCount> Marking;"));
	m_file.Write(newLine);
	m_file.Write(newLine);

	WriteConstants();

	WritePlaces();

	WriteObservers();

	wxString l_sStructDelta = wxT("struct delta_t {\n");
	l_sStructDelta << wxT("  places_t place;\n");
	l_sStructDelta << wxT("  bool reset;\n");
	l_sStructDelta << wxT("  int val;\n");
	l_sStructDelta << wxT("};\n");
	l_sStructDelta << wxT("typedef std::vector<delta_t> VectorDelta;\n");
	m_file.Write(l_sStructDelta);
	m_file.Write(newLine);

	wxString l_sStructPre = wxT("struct pre_t {\n");
	l_sStructPre << wxT("  places_t place;\n");
	l_sStructPre << wxT("  unsigned int lower;\n");
	l_sStructPre << wxT("  unsigned int upper;\n");
	l_sStructPre << wxT("};\n");
	l_sStructPre << wxT("typedef std::vector<pre_t> VectorPre;\n");
	m_file.Write(l_sStructPre);
	m_file.Write(newLine);

	WriteTransitions();

	wxString l_sMain = wxT("int main( int argc, char** argv)") + newLine;
	l_sMain << wxT("{") << newLine;
	m_file.Write(l_sMain);

	m_file.Write(wxT("}"));
	m_file.Write(newLine);



	return !m_bError;
}

bool SP_ExportCPP::WritePlaces()
{
	m_mPlace2Id.clear();
	unsigned int id = 0;
	PlaceIdMap::iterator pIt;
	SP_Place* p;
	wxString l_sPlaces = wxT("enum places_t {");
	wxString l_sPlaceNames = wxT("std::array<std::string, placeCount> ") + placeNames + wxT("{");
	wxString l_sInitMarking = wxT("Marking ") + initMarking + wxT("{");
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

		l_sPlaces << (pIt != m_placeIdMap.begin() ? wxT(", ") : wxT(""))
					<< nName;
		l_sPlaceNames << (pIt != m_placeIdMap.begin() ? wxT(", ") : wxT(""))
					<< wxT("\"") << nName << wxT("\"");
		l_sInitMarking << (pIt != m_placeIdMap.begin() ? wxT(", ") : wxT(""));
		if (p->m_value.IsEmpty())
		{
			l_sInitMarking << p->m_marking;
		} else
		{
			l_sInitMarking << nValue;
		}
		m_mPlace2Id[nName] = id;
		++id;
	}
	l_sPlaces << wxT("};") << newLine;
	l_sPlaceNames << wxT("};") << newLine;
	l_sInitMarking << wxT("};") << newLine;
	m_file.Write(l_sPlaces);
	m_file.Write(l_sPlaceNames);
	m_file.Write(l_sInitMarking);
	m_file.Write(newLine);
	return TRUE;
}

bool SP_ExportCPP::WriteTransitions()
{
	wxString l_sTrans = wxT("enum trans_t {") ;
	wxString l_sTransNames = wxT("std::array<std::string, transCount> ") + transNames + wxT("{") ;
	wxString l_sTransRates = wxT("std::array<std::function<double(const Marking&)>, transCount> ") + transRates + wxT("{\n");
	wxString l_sTransToUpdate = wxT("std::array<std::vector<trans_t>, transCount> ") + transToUpdate + wxT("{{\n");
	wxString l_sValDelta;
	wxString l_sValPre;

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
		l_sTrans << (tIt != m_trIdMap.begin() ? wxT(", ") : wxT(""))
					<< nName;
		l_sTransNames << (tIt != m_trIdMap.begin() ? wxT(", ") : wxT(""))
					<< wxT("\"") << nName << wxT("\"");
		if(!t->m_function.IsEmpty())
		{
			wxString l_sFunction = t->m_function;
			l_sFunction.Replace(wxT(" "), wxT(""));
			l_sFunction.Replace(wxT("\t"), wxT(""));
			l_sFunction.Replace(wxT("\r"), wxT(""));
			l_sFunction.Replace(wxT("\n"), wxT(""));

			wxString l_sResultFunction;
			wxStringTokenizer l_StringTok(l_sFunction, wxT("()+*/^=<>!%&|,-"),  wxTOKEN_RET_EMPTY_ALL );
			while(l_StringTok.HasMoreTokens())
			{
				wxString l_sToken = l_StringTok.GetNextToken();
				if(!l_sToken.IsEmpty())
				{
					SP_MapString2UInt::const_iterator it = m_mPlace2Id.find(l_sToken);
					if(it != m_mPlace2Id.end())
					{
						l_sResultFunction << wxT("m[") << l_sToken << wxT("]");
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
			l_sTransRates << (tIt != m_trIdMap.begin() ? wxT(",\n") : wxT(""))
					<< wxT(" [](const Marking& m) -> double { return ") << l_sResultFunction << wxT("; }");
		}

		list<SP_DiscrArc*>* aList;
		list<SP_DiscrArc*>::iterator aIt;
		SP_DiscrArc* a;
		wxString atype;

		SP_VectorULong pre(m_placeIdMap.size(), 0);
		SP_VectorULong post(m_placeIdMap.size(), 0);
		SP_VectorULong read(m_placeIdMap.size(), 0);
		SP_VectorULong inhib(m_placeIdMap.size(), 0);
		SP_VectorBool reset(m_placeIdMap.size(), false);
		SP_VectorBool modif(m_placeIdMap.size(), false);

		aList = m_preTrMap[t->m_id];
		if(aList)
		{
			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				a = (*aIt);
				if ((a->m_edge->GetClassName()) == wxT("Edge"))
				{
					pre[a->m_id] += a->m_mult;
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

		aList = m_postTrMap[t->m_id];
		if (aList)
		{
			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				a = (*aIt);
				post[a->m_id] = a->m_mult;
			}
		}

		if(!l_sValDelta.IsEmpty())
		{
			l_sValDelta << wxT(",") << newLine;
		}
		if(!l_sValPre.IsEmpty())
		{
			l_sValPre << wxT(",") << newLine;
		}

		wxString l_sNewDelta;
		unsigned l_nNumDelta = 0;
		wxString l_sNewPre;
		unsigned l_nNumPre = 0;
		std::set<wxString> l_toUpdateTrans;
		PlaceIdMap::iterator pIt = m_placeIdMap.begin();
		for (unsigned i = 0; pIt != m_placeIdMap.end(); ++pIt, ++i)
		{
			if(reset[i] || (post[i] - pre[i] != 0))
			{
				if(l_nNumDelta > 0)
				{
					l_sNewDelta << wxT(",");
				}
				l_sNewDelta << wxT("{");
				l_sNewDelta << (*pIt).second->m_name << wxT(", ");
				l_sNewDelta << (reset[i] ? wxT("true") : wxT("false")) << wxT(", ");
				l_sNewDelta << (static_cast<int>(post[i]) - static_cast<int>(pre[i]));
				l_sNewDelta << wxT("}");
				++l_nNumDelta;

				list<SP_DiscrArc*>* arcs = m_postPlMap[(*pIt).first];
				if(arcs)
				{
					list<SP_DiscrArc*>::iterator arcsIt = arcs->begin();
					for(; arcsIt != arcs->end(); ++arcsIt)
					{
						l_toUpdateTrans.insert(m_trIdMap[(*arcsIt)->m_id]->m_name);
					}
				}
			}
			if(modif[i] || reset[i] || pre[i] > 0 || read[i] > 0 || inhib[i] > 0)
			{
				if(l_nNumPre > 0)
				{
					l_sNewPre << wxT(",");
				}
				l_sNewPre << wxT("{");
				l_sNewPre << (*pIt).second->m_name << wxT(", ");
				if(pre[i] > read[i])
				{
					l_sNewPre << pre[i];
				}
				else if(read[i] > 0)
				{
					l_sNewPre << read[i];
				}
				else
				{
					l_sNewPre << wxT("0");
				}
				l_sNewPre << wxT(", ");
				if(inhib[i] > 0)
				{
					l_sNewPre << inhib[i];
				}
				else
				{
					l_sNewPre << infti;
				}
				l_sNewPre << wxT("}");
				++l_nNumPre;
			}
		}
		l_sValDelta << wxT("  {\n");
		l_sValDelta << wxT("    ") << l_sNewDelta << wxT("\n");
		l_sValDelta << wxT("  }");
		l_sValPre << wxT("  {\n");
		l_sValPre << wxT("    ") << l_sNewPre << wxT("\n");
		l_sValPre << wxT("  }");

		l_sTransToUpdate << (tIt != m_trIdMap.begin() ? wxT(",\n") : wxT(""));
		l_sTransToUpdate << wxT("    {");
		std::set<wxString>::iterator uIt = l_toUpdateTrans.begin();
		for(; uIt != l_toUpdateTrans.end(); ++uIt)
		{
			if(uIt != l_toUpdateTrans.begin())
			{
				l_sTransToUpdate << wxT(", ");
			}
			l_sTransToUpdate << *uIt;
		}
		l_sTransToUpdate << wxT("}");
	}
	l_sTrans << wxT("};") << newLine;
	l_sTransNames << wxT("};") << newLine;
	l_sTransRates << wxT("\n};") << newLine;
	l_sTransToUpdate << newLine << wxT("}};") << newLine;

	m_file.Write(l_sTrans);
	m_file.Write(newLine);
	m_file.Write(l_sTransNames);
	m_file.Write(newLine);
	m_file.Write(l_sTransRates);
	m_file.Write(newLine);

	l_sValDelta.Prepend(wxT("std::array<VectorDelta, transCount> ") + delta + wxT("{{\n"));
	l_sValDelta << newLine << wxT("}};") << newLine;
	l_sValPre.Prepend(wxT("std::array<VectorPre, transCount> ") + pre + wxT("{{\n"));
	l_sValPre << newLine << wxT("}};") << newLine;

	m_file.Write(l_sValDelta);
	m_file.Write(newLine);
	m_file.Write(l_sValPre);
	m_file.Write(newLine);
	m_file.Write(l_sTransToUpdate);
	m_file.Write(newLine);


	return TRUE;
}

bool SP_ExportCPP::WriteConstants()
{
	ConstantIdMap::iterator l_itConst;

	for(l_itConst = m_constantIdMap.begin(); l_itConst != m_constantIdMap.end(); ++l_itConst)
	{
		SP_Constant *pa = l_itConst->second;
		wxString name = pa->m_name;
		wxString type = pa->m_type;

		wxString l_sBuffer = wxT("const");
		l_sBuffer << wxT(" ") << type;
		l_sBuffer << wxT(" ") << name;
		l_sBuffer << wxT(" = ") << pa->m_value;
		l_sBuffer << wxT(";\n");
		m_file.Write(l_sBuffer);
	}
	m_file.Write(wxT("\n"));

	ParameterIdMap::iterator l_itParam;

	for(l_itParam = m_parameterIdMap.begin(); l_itParam != m_parameterIdMap.end(); ++l_itParam)
	{
		SP_Parameter *pa = l_itParam->second;
		wxString name = pa->m_name;

		wxString l_sBuffer = wxT("const double");
		l_sBuffer << wxT(" ") << name;
		l_sBuffer << wxT(" = ") << pa->m_marking;
		l_sBuffer << wxT(";\n");
		m_file.Write(l_sBuffer);
	}
	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportCPP::WriteObservers()
{
	//m_file.Write(wxT("observers:\n"));

	//TODO

	//m_file.Write(wxT("\n"));
	return true;
}
