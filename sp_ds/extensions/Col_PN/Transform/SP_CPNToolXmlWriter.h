//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2010/6/17 11:55:00 $
// Short Description: XML Writer, writes graph structures actually
//////////////////////////////////////////////////////////////////////
#ifndef __SP_CPNToolXmlWriter_H__
#define __SP_CPNToolXmlWriter_H__

#include <wx/xml/xml.h>
#include "sp_core/SP_Writer.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSet.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"
#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_ColorProcessing.h"

#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Driver.h"
#include "sp_ds/extensions/Col_PN/FlexBison/SP_CPN_Parse_Context.h"

class SP_CPNToolXmlWriter: public SP_Writer
{
private:

    SP_DS_Graph* m_pcGraph;
	SP_CPN_ValueAssign m_ValueAssign;	
	SP_CPN_ColorSetClass m_ColorSetClass;

	int m_nIDCount;
	wxString m_sInstanceID;
	wxString m_sPageID;
	map<wxString,wxString> m_mNodeIDMap;

protected:

	//bool WriteRedColourInformation(SP_Graphic* p_pcVal, wxXmlNode* p_pcNode);

	bool WriteDeclaration(SP_DS_Graph* p_pcVal, wxXmlNode* p_pcRoot);

	bool WriteCPNet(SP_DS_Graph* p_pcVal, wxXmlNode* p_pcRoot);
	bool WritePage(SP_DS_Graph* p_pcVal, wxXmlNode* p_pcRoot);
	bool WriteNetSize(SP_DS_Graph* p_pcVal, wxXmlNode* p_pcRoot);

	bool WriteDeclColorSet(SP_CPN_ColorSet* p_pcColorSet, wxXmlNode* p_pcRoot);	
	bool WriteDelcVariable( wxString p_sVariableName, SP_CPN_Variable_Constant p_cVaribale, wxXmlNode* p_pcRoot );
	bool WriteDeclConstant( wxString p_sVariableName, SP_CPN_Variable_Constant p_cVaribale, wxXmlNode* p_pcRoot );
	bool WriteDeclFunction( wxString p_sFunctionName, SP_CPN_Function p_cFunction, wxXmlNode* p_pcRoot );

	bool WriteNormalEdge(SP_DS_Edge* p_pcVal, wxXmlNode* p_pcRoot);
	bool WritePlaceNode(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);
	bool WriteTransitionNode(SP_DS_Node* p_pcVal, wxXmlNode* p_pcRoot);

	wxString ProcessExpression( wxString p_sExpression);

public:
    SP_CPNToolXmlWriter();
    virtual ~SP_CPNToolXmlWriter();

    virtual bool Write(SP_DS_Graph* p_pcGraph, const wxString& p_sFile);
};

#endif // __SP_CPNToolXmlWriter_H__

