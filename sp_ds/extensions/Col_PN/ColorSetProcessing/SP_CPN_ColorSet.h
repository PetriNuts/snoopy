//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/10/10 10:20:00 $
// Short Description: colored colorset class
//////////////////////////////////////////////////////////////////////

#ifndef __SP_CPN_ColorSet__
#define __SP_CPN_ColorSet__

#include <wx/wx.h>
#include "sp_core/SP_Vector.h"
#include "sp_core/SP_Map.h"

// Data types that colored Petri Nets support

enum SP_CPN_DATATYPE
{
	CPN_DOT,
    CPN_INTEGER,
    CPN_STRING,
    CPN_BOOLEAN,
    CPN_ENUM,
    CPN_INDEX,
    CPN_PRODUCT,    
    CPN_UNION,
	CPN_NULL,
	CPN_DOUBLE//added by george for continious nets
};

class SP_CPN_ColorSetClass;

class SP_CPN_ColorSet
{

private:
	wxString m_sName;									// Name of colorset
	SP_CPN_DATATYPE m_DataType;							// Datatype of colorset	
	vector<wxString> m_ColorValues;						// Store colors with any data type as string
	vector<wxString> m_RealColorValues;                 //by george

	vector<int> m_vIntColorValues;                      //for CSP solver 23.08.2011
	bool m_bIntSuccessive;

	vector<wxString> m_Alias;							//not realized so far	// Record the alias names of this color set	
	wxString m_sFatherSet;								// Only for subset color set
	vector<wxString> m_vSubSet;							//Childrens
	wxString m_sIndexName;								// Only for index type
	
	map<wxString, SP_CPN_ColorSet*> m_ComponentMap;		// Only for product and union colorset
	vector<wxString> m_ComponentName;					// Store current-level ordered component names
	vector<wxString> m_AllComponentName;				//Store all components with different levels

	SP_CPN_ColorSetClass* m_pcColorSetClass;

private:
	bool TraverseUnionComponents(map<wxString, SP_CPN_ColorSet*>* p_pComponentMap, vector<SP_CPN_ColorSet*>* p_pColorsetVector, bool p_bAll = false);	
	bool TraverseComponents(map<wxString, SP_CPN_ColorSet*>* p_pComponentMap, vector<SP_CPN_ColorSet*>* p_pColorsetVector);

	vector<wxString> GenerateColorsForMarkingPredicat(map<wxString, wxString> p_mColorSetToValue);

	bool Check(wxString p_sCompSets,SP_CPN_ColorSet* p_pcColorSet, wxString p_sErrorPosition);


public:
	SP_CPN_ColorSet();
	~SP_CPN_ColorSet();

	wxString Successor(wxString p_color);				// Successor function
	wxString Predecessor(wxString p_color);				// predecessor function	

	bool GenerateProduct();								// Generating product colors	
	void GenerateUnionValues();

	vector<wxString> GetColorsForMarkingPredicat(map<wxString, wxString> p_mColorSetToValue);
	bool CheckVariableInSubset(wxString p_sColorSet);	
	
	bool IsColor(wxString p_color);
	bool IsVariableofColorSet(wxString p_sColorSetName);
	
	bool IsUnionComponent(wxString p_sName);

	SP_CPN_DATATYPE GetUnionDataType(wxString p_sColor);
	SP_CPN_DATATYPE GetUnionDataType();	

	wxString GetProductElement(wxString p_sColor, int p_nPos);

	bool CheckProductSyntax( wxString p_sCompSets, wxString p_sErrorPosition) ;	


public:
	// The following is the set and get functions
	void SetName(wxString p_sName)
	{
		m_sName = p_sName;
	}
	
	wxString GetName()
	{
		return m_sName;
	}

	void SetDataType(SP_CPN_DATATYPE p_DataType)
	{
		m_DataType = p_DataType;
	}

	SP_CPN_DATATYPE GetDataType()
	{
		return m_DataType;
	}	

	void SetStringValue(vector<wxString> p_StringValue)
	{
		m_ColorValues.clear();
		m_ColorValues = p_StringValue;		
	}


	vector<wxString> GetStringValue()
	{
		return m_ColorValues;
	}

	void SetRealColor(vector<wxString> p_StringValue)//by george
	{
		m_RealColorValues.clear();
		m_RealColorValues = p_StringValue;;

	}

	vector<wxString> GetRealColorStringValue()//by george
	{
		return m_RealColorValues;
	}

	void SetIntColorValues( vector<int> p_vIntColorValues )
	{
		m_vIntColorValues = p_vIntColorValues;
	}

	vector<int> GetIntColorValues()
	{
		return m_vIntColorValues;
	}

	
	void SetAlias(vector<wxString> p_Alias)
	{
		m_Alias.clear();
		for(unsigned i = 0; i < p_Alias.size(); i++)
			m_Alias.push_back( p_Alias[i] );
	}
	vector<wxString> GetAlias()
	{
		return m_Alias;
	}

	void SetFatherSet(wxString p_sFatherSet)
	{
		m_sFatherSet = p_sFatherSet;
	}

	wxString GetFatherSet()
	{
		return m_sFatherSet;
	}

	void SetIndexName(wxString p_sIndexName)
	{
		m_sIndexName = p_sIndexName;
	}

	wxString GetIndexName()
	{
		return m_sIndexName;
	}

	void SetComponentMap(const map<wxString, SP_CPN_ColorSet*>& p_ComponentMap)
	{
		map<wxString, SP_CPN_ColorSet*>::const_iterator it;
		for(it = p_ComponentMap.begin(); it != p_ComponentMap.end(); it++)
		{
			m_ComponentMap.insert(*it);
		}
	
		m_ComponentMap = p_ComponentMap;
	}

	void AddComponentMap(wxString p_sName, SP_CPN_ColorSet* p_pcColorSet)
	{
		m_ComponentMap[p_sName] = p_pcColorSet;
	}

	

	map<wxString, SP_CPN_ColorSet*>& GetComponentMap()
	{
		return m_ComponentMap;
	}
	
	void SetComponentName(vector<wxString> p_ComponentName)
	{
		m_ComponentName.clear();
		for(unsigned i = 0; i < p_ComponentName.size(); i++)
			m_ComponentName.push_back( p_ComponentName[i] );
	}

	vector<wxString> GetComponentName()
	{
		return m_ComponentName;
	}

	void SetColorSetClass(SP_CPN_ColorSetClass* p_pcColorSetClass)
	{
		m_pcColorSetClass = p_pcColorSetClass;
	}

	void SetSubSet(wxString p_sSubSet)
	{
		m_vSubSet.push_back(p_sSubSet);
	}

	void SetIntSuccessiveFlag(bool p_bIntSuccessive)
	{
		m_bIntSuccessive = p_bIntSuccessive;
	}

	bool GetIntSuccessiveFlag()
	{
		return m_bIntSuccessive;
	}


};



#endif // __SP_CPN_ColorSet__

