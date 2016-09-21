//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2013/09/12 $
// Short Description: export to CUDA
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportCUDA.h"
#include "sp_utilities.h"


#include "sp_ds/SP_DS_Edge.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

#include <wx/tokenzr.h>

SP_ExportCUDA::SP_ExportCUDA()
{
}

SP_ExportCUDA::~SP_ExportCUDA()
{
}

bool SP_ExportCUDA::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_SPN_CLASS);
}

bool SP_ExportCUDA::DoWrite()
{
	m_file.Write(wxT("// ") + GetNetName(m_fileName) + wxT("\n"));
	m_file.Write(wxT("#define num_places ") + wxString::Format(wxT("%lu"),m_placeIdMap.size()) + wxT("\n"));
	m_file.Write(wxT("#define num_trans ") + wxString::Format(wxT("%lu"),m_trIdMap.size()) + wxT("\n"));
	m_file.Write(wxT("\n"));
	m_file.Write(wxT("\n"));

	WriteConstants();
	WritePlaces();
	WriteTransitions();

	m_file.Write(wxT("\n"));



	return !m_bError;
}

bool SP_ExportCUDA::WritePlaces()
{
	wxString l_sPlaceVector = wxT("const char* places[] = {");
	wxString l_sMarkingVector = wxT("unsigned int initMarking[] = {");
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
		unsigned nMarking = p->m_marking;
		if (p->m_name != nName)
		{
			SP_LOGWARNING(
					wxString::Format(wxT("place name %s was changed to %s"),
							p->m_name.c_str(), nName.c_str()));
		}
		l_sPlaceVector << wxT("\"") << nName << wxT("\"");
		l_sMarkingVector << wxString::Format(wxT("%u"), nMarking);
	}
	l_sPlaceVector << wxT("};\n");
	l_sMarkingVector << wxT("};\n");

	m_file.Write(wxT("// place vector\n"));
	m_file.Write(l_sPlaceVector);
	m_file.Write(wxT("// marking vector\n"));
	m_file.Write(l_sMarkingVector);

	return TRUE;
}

bool SP_ExportCUDA::WriteTransitions()
{
	wxString l_sTransVector = wxT("const char* trans[] = {");

	TransitionIdMap::iterator tIt;
	SP_Transition* t;
	for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++)
	{
		if(tIt != m_trIdMap.begin())
		{
			l_sTransVector << wxT(", ");
		}
		t = (*tIt).second;

		wxString nName = NormalizeString(t->m_name);
		if (t->m_name != nName)
		{
			SP_LOGWARNING(
					wxString::Format(wxT("tr name %s was changed to %s"),
							t->m_name.c_str(), nName.c_str()));
		}
		l_sTransVector << wxT("\"") << nName << wxT("\"");

		if(!t->m_function.IsEmpty())
		{
			wxString l_sFunction = t->m_function;
			l_sFunction.Replace(wxT(" "), wxT(""));
			l_sFunction.Replace(wxT("\t"), wxT(""));
			l_sFunction.Replace(wxT("\r"), wxT(""));
			l_sFunction.Replace(wxT("\n"), wxT(""));
		}
	}
	l_sTransVector << wxT("};\n");
	m_file.Write(wxT("// transition vector\n"));
	m_file.Write(l_sTransVector);

	wxString l_sStructDelta = wxT("struct delta = {\n");
	l_sStructDelta << wxT("\tbool reset;\n");
	l_sStructDelta << wxT("\tint val;\n");
	l_sStructDelta << wxT("};\n");
	//unsigned l_nNumDelta = 0;
	wxString l_sValDelta;

	wxString l_sStructPre = wxT("struct pre = {\n");
	l_sStructPre << wxT("\tint lower;\n");
	l_sStructPre << wxT("\tint upper;\n");
	l_sStructPre << wxT("};\n");
	//unsigned l_nNumPre = 0;
	wxString l_sValPre;

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

		aList = m_prePlMap[p->m_id];
		if (aList)
		{
			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				a = (*aIt);
				post[a->m_id] = a->m_mult;
			}
		}

		wxString l_sNewDelta;
		wxString l_sNewPre;
		for (unsigned i = 0; i < m_trIdMap.size(); ++i)
		{
			if(!l_sNewDelta.IsEmpty())
			{
				l_sNewDelta << wxT(",");
			}
			if(!l_sNewPre.IsEmpty())
			{
				l_sNewPre << wxT(",");
			}
			if(reset[i] || (post[i] - pre[i] != 0))
			{
				l_sNewDelta << wxT("{") << (reset[i] ? wxT("true") : wxT("false")) << wxT(", ") << (post[i] - pre[i]) << wxT("}");
			}
		}

	}

	m_file.Write(l_sStructDelta);
	m_file.Write(l_sStructPre);
	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportCUDA::WriteConstants()
{
	ConstantMap::iterator l_itConst;

	for(l_itConst = m_constantNameMap.begin(); l_itConst != m_constantNameMap.end(); ++l_itConst)
	{
		SP_Constant *pa = l_itConst->second;
		wxString name = l_itConst->first;
		wxString type = pa->m_type;

		wxString l_sBuffer = wxT("const");
		l_sBuffer << wxT(" ") << type;
		l_sBuffer << wxT(" ") << name;
		l_sBuffer << wxT(" = ") << pa->m_value;
		l_sBuffer << wxT(";\n");
		m_file.Write(l_sBuffer);
	}
	m_file.Write(wxT("\n"));

	ParameterMap::iterator l_itParam;

	for(l_itParam = m_parameterNameMap.begin(); l_itParam != m_parameterNameMap.end(); ++l_itParam)
	{
		SP_Parameter *pa = l_itParam->second;
		wxString name = l_itParam->first;

		wxString l_sBuffer = wxT("const double");
		l_sBuffer << wxT(" ") << name;
		l_sBuffer << wxT(" = ") << pa->m_marking;
		l_sBuffer << wxT(";\n");
		m_file.Write(l_sBuffer);
	}
	m_file.Write(wxT("\n"));
	return TRUE;}
