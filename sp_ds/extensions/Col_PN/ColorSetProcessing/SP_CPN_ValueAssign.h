//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/10/10 10:20:00 $
// Short Description: colored valueassign class
//////////////////////////////////////////////////////////////////////

#ifndef __SP_CPN_ValueAssign__
#define __SP_CPN_ValueAssign__

#include <wx/wx.h>
#include "sp_core/SP_Vector.h"
#include "sp_core/SP_Map.h"


#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"


#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSet.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_ColorProcessing.h"

#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Driver.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"


struct SP_CPN_Collist_Declarations
{
	wxString m_sName;
	wxString m_sType;
	wxString m_sColors;
	wxString m_sCompound_Colorsets;
	//wxString m_sAliasOriginalColorset;
	wxString m_sConstantvalue;

	wxString m_sFunctionname;
	wxString m_sFunctionReturntype;
	wxString m_sFunctionParameterlist;
	wxString m_sFunctionBody;
};




class SP_CPN_ValueAssign
{

private:
	// Visit the net elements
	SP_DS_Graph* l_pcGraph;
	SP_DS_Metadataclass* l_pcMetadataclass;
	SP_DS_Metadata* l_pcNewMetadata;
	SP_DS_ColListAttribute *m_pcColList;

	SP_CPN_Parse_Context_ptr m_pParseContext;
	SP_CPN_Driver_ptr  m_pDriver;

	SP_CPN_ColorSetClass* m_pcColorSetClass;

	map<wxString, vector<SP_CPN_Collist_Declarations> > m_svDeclarations; //for multithreads

	wxString m_sNetClassName;


private:
	SP_CPN_ColorSet* m_pColorSet;

	vector<SP_CPN_ColorSet*> m_ColorSetVector;				//Store all defined color sets	
	map<wxString, SP_CPN_Variable_Constant> m_VariableMap;	//Store all defined variables
	map<wxString, SP_CPN_Variable_Constant> m_ConstantMap;	//Store all defined constants
	map<wxString, SP_CPN_Function> m_FunctionMap;

	SP_CPN_ColorProcessing m_cColorProcessing;					// Call the colorprocessing class to deal and get all the information above

public:
	SP_CPN_ValueAssign();
	~SP_CPN_ValueAssign();
	
	bool InitializeColorset(SP_CPN_ColorSetClass &p_ColorSetClass,bool p_bDeclarations = true);

	bool CollectAllDeclarations();
	map<wxString, vector<SP_CPN_Collist_Declarations> > GetAllDeclarations(){return m_svDeclarations;}
	void SetAllDeclarations(map<wxString, vector<SP_CPN_Collist_Declarations> >& p_svDeclarations)
	{ 
		m_svDeclarations = p_svDeclarations;
	}

	vector<SP_CPN_ColorSet*>* GetColorSetVector()				{	return &m_ColorSetVector;	}
	map<wxString, SP_CPN_Variable_Constant>* GetVariableMap()	{	return &m_VariableMap;		}
	map<wxString, SP_CPN_Variable_Constant>* GetConstantMap()	{	return &m_ConstantMap;		}
	map<wxString, SP_CPN_Function>* GetFunctionMap()			{	return &m_FunctionMap;	}

private:
	bool InitializeBasicColorset();
	bool InitializeStructuredColorset();
	bool InitializeSubsetColorset();
	bool InitializeAliasColorset();

	bool InitializeVariable();
	bool InitializeConstant();
	bool InitializeBasicTypeConstant();

	bool GetIntConstantValue(wxString p_sConstExpr, int& p_nValue);
	
	bool Incremental_Init_Variable(SP_CPN_ColorSet* p_pcColorSet);
	bool Incremental_Init_Constant(SP_CPN_ColorSet* p_pcColorSet);

	bool InitializeFunction(SP_CPN_ColorSetClass &p_ColorSetClass);
	bool InitializeFunctionParseTree(SP_CPN_ColorSetClass &p_ColorSetClass);
	
};

#endif // __SP_CPN_ValueAssign__

