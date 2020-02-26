//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/09/28 10:20:00 $
// Short Description: colored PN animation class
//////////////////////////////////////////////////////////////////////

#ifndef _SP_CPN_ColorProcessing_H_
#define _SP_CPN_ColorProcessing_H_

#include <wx/object.h>
#include "sp_core/SP_Vector.h"
#include "sp_core/SP_Map.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"

struct SP_CPN_Constant_TypeValue
{
	wxString m_sType;
	wxString m_sValue;
};

class SP_CPN_ColorProcessing : public wxObject
{

private:

	wxString m_sString;			// the tokenized string
	int	 m_peekc;
	unsigned m_iChar;

	SP_CPN_ColorSetClass* m_pcColorSetClass;
	bool m_bColorSetClass;

private:

	inline wxChar GetChar()
	{
		if (m_iChar < m_sString.size())
			return m_sString[m_iChar++];
		else
		{
			m_iChar++;
			return '\0';
		}


	}

	bool GetTokenforInt(wxString p_sColorsetName, wxString & p_ParseString, vector<wxString> & p_StoredString);
	bool GetTokenforEnum(wxString p_sColorsetName, wxString & p_ParseString, vector<wxString> & p_StoredString);
	bool GetTokenforIndex(wxString p_sColorsetName, wxString & p_ParseString, vector<wxString> & p_StoredString);
	bool GetTokenforString(wxString p_sColorsetName, wxString & p_ParseString, vector<wxString> & l_StoredString);

	bool ProcessingStringSub(wxString p_sColorsetName, vector<wxString> &p_vInputString, vector<wxString> &p_vString);
	bool GenerateAllString(wxString SA, wxString SZ, vector<wxString> *StringVector);
	bool GenerateString(wxString p_sColorsetName, wxString SL, wxString SH, vector<wxString> *StringVector);
	bool GenerateDigitalRange(wxString p_sColorsetName, long p_nLowBound, long p_nUpBound, vector<wxString> * p_vString);

	bool GetConstant(map<wxString, SP_CPN_Variable_Constant> &m_mConstant);



public:

	SP_CPN_ColorProcessing();

	void SetColorSetClass(SP_CPN_ColorSetClass* p_pcColorSetClass) { m_pcColorSetClass = p_pcColorSetClass; }
	void SetColorSetClassFlag(bool p_bColorSetClass) { m_bColorSetClass = p_bColorSetClass; }

	bool ProcessingInt(wxString p_sColorsetName, wxString & p_ParseString, vector<unsigned> &p_vResult, bool &p_bSuccessive); //Processing integer color set definition
	bool ProcessingString(wxString p_sColorsetName, wxString & p_ParseString, vector<wxString> &p_vResult); //Processing string color set definition
	bool ProcessingEnum(wxString p_sColorsetName, wxString & p_ParseString, vector<wxString> &p_vResult); //Processing enum color set definition
	bool ProcessingIndex(wxString p_sColorsetName, wxString & p_ParseString, wxString & p_IndexName, vector<unsigned> &p_vResult); //Processing index color set definition
	bool ProcessingComponent(wxString p_sColorsetName, wxString & p_ParseString, vector<wxString> &p_vResult); //Processing integer color set definition
	bool ProcessingIdentifier(wxString p_identifier, wxString &p_sError); // Processing identifier
	bool ProcessingParameter(wxString p_sFunctionName, wxString p_sParameter, vector<wxString> &p_vResult);
	bool ProcessingBool(wxString & p_ParseString, vector<wxString> &p_vResult);


public:
	//New processing functions, we do not use them yet

	// for integer
	bool DecomposeIntegerDef(wxString p_sColorString, vector<wxString> &p_vResult);
	bool GetIntegerToken_Dash(wxString p_sParseString, int &p_nMinValue, wxString &p_sMaxValue);
	bool GetIntegerToken_Comma(wxString p_sParseString, vector<wxString> & p_vStoredString);

	bool ProcessingInteger(wxString p_sColorString, vector<wxString> &p_vResult, SP_MapInt2String & p_mResult, bool &p_bFinite);
	bool ProcessingIntegerSubset(wxString p_sColorString, vector<wxString> &p_vResult, wxString &p_sPredicate);

	// for string
	bool DecomposeStringDef(wxString p_sColorString, vector<wxString> &p_vResult);
	bool GetTokenforString_Comma(wxString & p_ParseString, vector<wxString> &p_StoredString);
	bool GetTokenforString_Dash(wxString p_sParseString, wxString &p_sMinValue, wxString &p_sMaxValue);

};

#endif //_SP_CPN_ColorProcessing_H_
