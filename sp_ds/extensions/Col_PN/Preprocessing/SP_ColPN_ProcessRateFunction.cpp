//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2010/6/17 11:55:00 $
// Short Description: XML Writer, writes graph structures actually
//////////////////////////////////////////////////////////////////////

#include <wx/regex.h>
#include "sp_ds/extensions/Col_PN/Preprocessing/SP_ColPN_ProcessRateFunction.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StParser.h"
#include <algorithm>
#include "sp_ds/attributes/SP_DS_NameAttribute.h"


SP_ColPN_ProcessRateFunction::SP_ColPN_ProcessRateFunction()
{
	
}

SP_ColPN_ProcessRateFunction::~SP_ColPN_ProcessRateFunction()
{

}

bool SP_ColPN_ProcessRateFunction::ProceedRateFunction(wxString &p_sRateFunction, SP_DS_Node* p_pcNode, map<wxString, SP_ColPN_RateFunction>* p_pmColPlace2Color)
{

	if( !TransformRateFunction(p_sRateFunction,p_pmColPlace2Color))
		return false;

	if( p_sRateFunction.Find( wxT('?') ) != wxNOT_FOUND )
	{
		if( !GetColoredPlaces(p_sRateFunction, p_pcNode,p_pmColPlace2Color) )
			return false;
	}

	return true;
}

bool SP_ColPN_ProcessRateFunction::TransformRateFunction(wxString &p_sRateFunction, map<wxString, SP_ColPN_RateFunction>* p_pmColPlace2Color)
{
	//also allow [val(x)], which returns the value of the color x

	map<wxString, SP_ColPN_RateFunction> l_mColPlace2Color;
	wxString l_sNewRateF = p_sRateFunction;
	int l_nCount = 0;
	while( l_sNewRateF.Find( wxT('[') ) != wxNOT_FOUND )
	{
		//replace
		wxString l_sBefore = l_sNewRateF.BeforeFirst( wxT('[')  );
		wxString l_sAfter = l_sNewRateF.AfterFirst( wxT(']')  );

		wxString l_sColor = l_sNewRateF.AfterFirst( wxT('[')  );
		l_sColor = l_sColor.BeforeFirst( wxT(']')  );

		wxString l_sReplaceSymbol = wxT('?') + wxString::Format(wxT("%d"),l_nCount) + wxT('?');

		SP_ColPN_RateFunction l_structRateFunction;
		l_structRateFunction.m_sFormula = wxString(l_sColor.c_str());		

		l_mColPlace2Color[l_sReplaceSymbol] = l_structRateFunction;

		l_sNewRateF = l_sBefore + l_sReplaceSymbol + l_sAfter;	
		
		l_nCount++;
	}

	p_sRateFunction = l_sNewRateF; 	
	*p_pmColPlace2Color = l_mColPlace2Color;
		
	return true;
}


bool SP_ColPN_ProcessRateFunction::CheckColoredRateFunction(wxString p_sCheckFormula,SP_DS_Node* p_pcNode)
{
	wxString l_sTransitionName = dynamic_cast<SP_DS_NameAttribute*>(p_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
	//the first step for checking
	wxString l_sNewRateF = p_sCheckFormula;
	if( l_sNewRateF.Find(wxT('[')) == wxNOT_FOUND || l_sNewRateF.Find(wxT(']')) == wxNOT_FOUND)
	{
		if(l_sNewRateF.Find(wxT('[')) != wxNOT_FOUND || l_sNewRateF.Find(wxT('[')) != wxNOT_FOUND )
		{
			wxString l_sError = wxT("[ ] mismatch in the rate function: ") + l_sNewRateF + wxT(" in transition: ") + l_sTransitionName;
			SP_LOGERROR(l_sError);
			return false;
		}		
	}

	//the second step
	while( l_sNewRateF.Find( wxT('[') ) != wxNOT_FOUND )
	{
		//check
		wxString l_sCheckBefore = l_sNewRateF.BeforeFirst( wxT('[')  );
		wxString l_sCheckAfter = l_sNewRateF.AfterFirst( wxT(']')  );
		wxString l_sValfunc = l_sNewRateF.AfterFirst( wxT('[')  );
		l_sValfunc = l_sValfunc.BeforeFirst( wxT(']')  );
		if(l_sValfunc.Find(wxT("val(")) == wxNOT_FOUND)
		    l_sNewRateF = l_sCheckBefore + l_sCheckAfter; // for check arbitrary value
		else 
			l_sNewRateF = l_sCheckBefore + wxT("1000")+ l_sCheckAfter; // for check
	}
#if 0
	SP_DS_StParser l_cParser;
	if (!(l_cParser.CheckFormulaFunction(l_sNewRateF, p_pcNode,true)))
	{
		wxString l_sError = wxT("Error in the rate function: ") + l_sNewRateF + wxT(" in transition: ") + l_sTransitionName;
		SP_LOGERROR(l_sError);
		return false;
	}
#endif
	return true;
}

bool SP_ColPN_ProcessRateFunction::GetColoredPlaces(wxString p_sFormula, SP_DS_Node* p_pcNode,map<wxString, SP_ColPN_RateFunction>* p_pmColPlace2Color)
{
#if 0
	SP_DS_StParser l_cParser;
	l_cParser.CheckFormulaFunction(p_sFormula, p_pcNode,true);
	wxArrayString* l_plMissingVariableList = l_cParser.GetMissingVariableList();
	vector<wxString> l_vPreplaces;
	for (unsigned int i = 0; i < l_plMissingVariableList->Count(); i++)
	{
		wxString l_sVariables = (*l_plMissingVariableList)[i];
		l_vPreplaces.push_back(l_sVariables);
	}
#else
	wxRegEx rePlaces(wxT("(\\w+\\?\\d+\\?+)"), wxRE_ADVANCED);
	vector<wxString> l_vPreplaces;
	wxString l_sFormula(p_sFormula);
	while(rePlaces.Matches(l_sFormula))
	{
		size_t start, len;
		rePlaces.GetMatch(&start, &len);
		wxString l_sVariables = l_sFormula.substr(start, len);
		l_sFormula.Remove(start, len);
		l_vPreplaces.push_back(l_sVariables);
	}
#endif
	map<wxString, SP_ColPN_RateFunction>::iterator itMap;
	for(itMap = p_pmColPlace2Color->begin(); itMap != p_pmColPlace2Color->end(); itMap++)
	{
		bool l_bFound = false;
		for(unsigned i = 0; i < l_vPreplaces.size(); i++)
		{
			if( l_vPreplaces[i].Find(itMap->first) != wxNOT_FOUND )
			{
				l_bFound = true;
				wxString l_sPlaceName = l_vPreplaces[i];
				l_sPlaceName = l_sPlaceName.BeforeFirst(wxT('?'));
				itMap->second.m_sColPlace = l_sPlaceName;
				break;
			}
		}
		//also allow [val(x)], which returns the value of the color x
		if( !l_bFound && itMap->second.m_sFormula.Find(wxT("val(")) == wxNOT_FOUND)
		{
			return false;
		}
	}


	return true;
}

bool SP_ColPN_ProcessRateFunction::ReplaceRateFunction(wxString &p_sFormula,map<wxString, SP_ColPN_RateFunction>* p_pmColPlace2Color, bool p_bExport)
{
	map<wxString, SP_ColPN_RateFunction>::iterator itMap;
	for(itMap = p_pmColPlace2Color->begin(); itMap != p_pmColPlace2Color->end(); itMap++)
	{
		wxString l_sColor = itMap->second.m_sValue;
		//if(p_bExport)
		{
			l_sColor.Replace(wxT("("),wxT("_"));
			l_sColor.Replace(wxT(")"),wxT("_"));
			l_sColor.Replace(wxT(","),wxT("_"));
		}
		if( itMap->second.m_sFormula.Find(wxT("val(")) != wxNOT_FOUND)
		{
			p_sFormula.Replace(itMap->first,l_sColor);
		}
		else
		{
			p_sFormula.Replace(itMap->first,wxT("_") + l_sColor);
		}
	}

	return true;
}

