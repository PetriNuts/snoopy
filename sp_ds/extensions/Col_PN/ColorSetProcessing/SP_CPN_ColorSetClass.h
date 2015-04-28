//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/10/10 10:20:00 $
// Short Description: colored colorsetclass class
//////////////////////////////////////////////////////////////////////


#ifndef __SP_CPN_ColorSetClass__
#define __SP_CPN_ColorSetClass__

#pragma once

#include <wx/wx.h>
#include "sp_core/SP_Vector.h"
#include "sp_core/SP_Map.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSet.h"

#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Driver.h"


// This struct is used to store the information of a constant or variable

struct SP_CPN_Variable_Constant
{
	SP_CPN_DATATYPE m_DataType;
	wxString m_ColorSet;
	
	int m_IntegerValue;		
	wxString m_StringValue;
	bool m_BooeanValue;

};


struct SP_CPN_Parameter
{
	unsigned m_nPosition;
	wxString m_sParaColorSet;
	SP_CPN_DATATYPE m_DataType;

	wxString m_sRealVar;   //added on 23.08.2011

	int m_IntegerValue;		
	wxString m_StringValue;
	bool m_BooeanValue;

};



struct SP_CPN_Function
{
	wxString m_sFunctionName;
	wxString m_sReturnColorSet;
	SP_CPN_DATATYPE m_ReturnDataType;
	map<wxString,SP_CPN_Parameter> m_ParameterMap;

	wxString m_sFunctionBody;

	std::shared_ptr<SP_CPN_Parse_Context> 	m_pParseContext;
	//SP_CPN_Parse_Context_ptr 	m_pParseContext;
	SP_CPN_Driver_ptr 	m_pDriver;

};



class SP_CPN_ColorSetClass
{

private:
	
	vector<SP_CPN_ColorSet*> *m_pColorSetVector;	// Store Colorset for simulation or parser
	map<wxString, SP_CPN_Variable_Constant> *m_pVariableMap;
	map<wxString, SP_CPN_Variable_Constant> *m_pConstantMap;
	map<wxString, SP_CPN_Function> *m_pFunctionMap;

	map<wxString, int> m_mNeighbor12DFunc; 	

public:
	SP_CPN_ColorSetClass();
	~SP_CPN_ColorSetClass();

	map<wxString, SP_CPN_Function>* GetFunctionMap()
	{
		return m_pFunctionMap;
	}

	vector<SP_CPN_ColorSet*>* GetColorSetVector()
	{
		return m_pColorSetVector;
	}
	map<wxString, SP_CPN_Variable_Constant>* GetVariableMap()
	{
		return m_pVariableMap;
	}
	map<wxString, SP_CPN_Variable_Constant>* GetConstantMap()
	{
		return m_pConstantMap;
	}

	map<wxString, int>* GetNeighbor12DFunc()
	{
		return &m_mNeighbor12DFunc; 
	}

	void SetColorSetVector(vector<SP_CPN_ColorSet*> *p_ColorSetVector)
	{
		m_pColorSetVector = p_ColorSetVector;
	}
	void SetVariableMap(map<wxString, SP_CPN_Variable_Constant> *p_VariableMap)
	{
		m_pVariableMap = p_VariableMap;
	}
	void SetConstantMap(map<wxString, SP_CPN_Variable_Constant> *p_ConstantMap)
	{
		m_pConstantMap = p_ConstantMap;
	}
	void SetFunctionMap(map<wxString, SP_CPN_Function> *p_FunctionMap)
	{
		m_pFunctionMap = p_FunctionMap;
	}

	SP_CPN_ColorSet* LookupColorSet(wxString p_ColosetName);

	bool SetVariableValue(wxString p_VariableName, wxString p_VariableValue);

	bool SetVariableValue(wxString p_VariableName, unsigned int p_nVarVal);

	void SetFunctionVariableValue(wxString p_sFunctionName, wxString p_VariableName, wxString p_VariableValue);

	void SetParameterValues(wxString p_sFunctionName,wxString p_sArgumentList);

	bool CheckParaTypeMatch(wxString p_sFunctionName,wxString p_sArgumentList, wxString p_sCSList);

	bool SetNeighbor12DFuncValue(int p_nFuncValue);

};

#endif // __SP_CPN_ColorSetClass__

