//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2013/04/30 $
// Short Description: export to Matlab
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportCPN2Matlab.h"
#include "sp_utilities.h"


#include "sp_ds/SP_DS_Edge.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"

#include "sp_ds/extensions/continuous/SP_DS_PlaceODE.h"

#include <wx/tokenzr.h>
#include <wx/filename.h>

SP_ExportCPN2Matlab::SP_ExportCPN2Matlab()
{
}

SP_ExportCPN2Matlab::~SP_ExportCPN2Matlab()
{
}

bool SP_ExportCPN2Matlab::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_CONTINUOUSPED_CLASS);
}

bool SP_ExportCPN2Matlab::DoWrite()
{
	wxString l_sFilename = wxFileName(m_fileName).GetName();

	wxString l_sFunction = wxT("function ") + l_sFilename + wxT(" ()\n");
	wxString l_sConstants = wxT("% definition of constants\n");
	wxString l_sConst;
	ConstantIdMap::iterator l_itConst;

	for(l_itConst = m_constantIdMap.begin(); l_itConst != m_constantIdMap.end(); ++l_itConst)
	{
		SP_Constant *pa = l_itConst->second;
		wxString name = pa->m_name;

		l_sConstants << wxT("  ") << name;
		l_sConstants << wxT(" = ") << pa->m_value;
		l_sConstants << wxT(";\n");

		l_sConst << wxT(", ") << name;
	}

	l_sFunction << l_sConstants;
	l_sFunction << wxT("\n");

	const SP_ListNode *l_places = m_graph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements();

	SP_DS_PlaceODE* l_pcPlaceODEConstructor = new SP_DS_PlaceODE(m_graph);

	if (!l_pcPlaceODEConstructor->Initialize())
	{
		SP_MESSAGEBOX(wxT("Error: Can not Initialize Place ODEs"));
		return false;
	}

	wxString l_sODE = wxT("% ode\n function [xdot] = f (t, x)\n\n");
	wxString l_sPlacenames = wxT("% place names\n p = {");
	wxString l_sPlaces = wxT("% place variables for initial marking\n");
	wxString l_sInitMarking = wxT("% initial marking\n m0 = [");
	long l_nPlacePos = 0;
	//Iterate for all the places
	for (auto l_itPlace : *l_places)
	{
		wxString l_sName = (dynamic_cast<SP_DS_NameAttribute*>(l_itPlace->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)))->GetValue();
		l_sPlacenames << (l_nPlacePos > 0 ? wxT(",") : wxEmptyString )
						<< wxT("'") << l_sName << wxT("'");
		l_sPlaces << l_sName << wxT(" = ")
						<< l_itPlace->GetAttribute(wxT("Marking"))->GetValueString()
						<< wxT(";\n");
		l_sInitMarking << (l_nPlacePos > 0 ? wxT(",") : wxEmptyString )
						<< l_sName;
		l_sODE << wxT("xdot(") << (l_nPlacePos+1) << wxT(") = ");
		wxString l_sResultFunction;
		if((dynamic_cast<SP_DS_BoolAttribute*>(l_itPlace->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_BOOL)))->GetValue())
		{
			l_sResultFunction = wxT("0");
		}
		else
		{
			wxString l_sTmp = l_pcPlaceODEConstructor->ConstructODE(l_nPlacePos);
			wxStringTokenizer l_StringTok(l_sTmp, wxT("()+*/^=<>!%&|,-"),  wxTOKEN_RET_EMPTY_ALL );
			while(l_StringTok.HasMoreTokens())
			{
				wxString l_sToken = l_StringTok.GetNextToken();
				if(!l_sToken.IsEmpty())
				{
					ContPlaceMap::const_iterator it = m_contPlaceNameMap.find(l_sToken);
					if(it != m_contPlaceNameMap.end())
					{
						l_sResultFunction << wxT("x(") << it->second->m_id+1 << wxT(")");
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
		}
		l_sODE << l_sResultFunction << wxT(";\n");
		l_nPlacePos++;
	}
	l_sPlacenames << wxT("};\n");
	l_sInitMarking << wxT("];\n");
	l_sODE << wxT("\nxdot = xdot';\n\n");
	l_sODE << wxT("end\n\n");

	l_sFunction << l_sPlacenames << wxT("\n");
	l_sFunction << l_sPlaces << wxT("\n");
	l_sFunction << l_sInitMarking << wxT("\n");

	l_sFunction << wxT("% time & output specification\n");
	l_sFunction << wxT("StartT = 0\n");
	l_sFunction << wxT("StopT = 100\n");
	l_sFunction << wxT("OutputSteps = 100\n");
	l_sFunction << wxT("\n");
	l_sFunction << wxT("t = linspace (StartT, StopT, OutputSteps);\n");
	l_sFunction << wxT("\n");
	l_sFunction << wxT("% call the ode solver\n");
	l_sFunction << wxT("[t,x] = ode15s(@f, t, m0, []);\n\n");

	l_sFunction << wxT("%% write results to csv\n");
	l_sFunction << wxT("%fid = fopen('") << l_sFilename << wxT(".csv', 'wt');\n");
	l_sFunction << wxT("%fprintf(fid, 'time');\n");
	l_sFunction << wxT("%fprintf(fid, ',%s', p{:});\n");
	l_sFunction << wxT("%fprintf(fid, '\\n');\n");
	l_sFunction << wxT("%fclose(fid);\n");
	l_sFunction << wxT("%csvwrite('") << l_sFilename << wxT(".csv', [t(:) x], '-append');\n\n");

	l_sFunction << wxT("%% plot results\n");
	l_sFunction << wxT("graphics_toolkit ('fltk')\n");
	l_sFunction << wxT("figure(1)\n");
	l_sFunction << wxT("plot (t, x)\n");
	l_sFunction << wxT("legend (p)\n");
	l_sFunction << wxT("xlabel ('time')\n");
	l_sFunction << wxT("ylabel ('token')\n");
	l_sFunction << wxT("\n");
	l_sFunction << wxT("%print ('") << l_sFilename << wxT(".pdf');\n\n");

	l_sFunction << l_sODE;

	l_sFunction << wxT("end\n\n");

	m_file.Write(l_sFunction);
	m_file.Write(wxT("\n"));

	wxDELETE(l_pcPlaceODEConstructor);

	return !m_bError;
}

