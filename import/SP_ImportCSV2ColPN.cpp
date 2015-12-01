//////////////////////////////////////////////////////////////////////
// $Author: fei liu $
// $Version: 0.1 $
// $Date: 2010/05/26 $
//////////////////////////////////////////////////////////////////////

#include "import/SP_ImportCSV2ColPN.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gr/SP_GR_Edge.h"
#include "snoopy.h"

#include "sp_defines.h"

#include <wx/spinctrl.h>

#include <algorithm>

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "snoopy.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

// ============================================================================
// implementations
// ============================================================================

SP_ImportCSV2ColPN::SP_ImportCSV2ColPN()
{

}

SP_ImportCSV2ColPN::~SP_ImportCSV2ColPN()
{

}

bool SP_ImportCSV2ColPN::ReadFile(const wxString& p_sFile)
{
	SP_MDI_Doc* l_pcDoc = SP_Core::Instance()->GetRootDocument();
	if( !l_pcDoc )
		return false;

	m_pcGraph = l_pcDoc->GetGraph();
	if( !m_pcGraph )
		return false;

	g_bError = FALSE;
	g_sMessage = wxT("");

	wxTextFile l_cFile;
	if (l_cFile.Open(p_sFile))
	{	
		bool l_bFlag = true;

		if( ! Parse(l_cFile) )
			l_bFlag = false;

		l_cFile.Close();

		if(!l_bFlag)
			return false;
	}

	return TRUE;
}

/////////////////////////////////////Parsing TINA/////////////////////////////////////////////////////////

bool SP_ImportCSV2ColPN::Parse(wxTextFile& p_cFile)
{
	wxString l_sLog = wxT("");
	SP_LOGDEBUG(wxString(p_cFile.GetName()).AfterLast(wxFILE_SEP_PATH));

	wxString l_sLine = p_cFile.GetFirstLine();
	wxStringTokenizer l_cST(l_sLine,wxT(" \n\r\t"),wxTOKEN_STRTOK);
	if (!l_cST.CountTokens())
		return false;
	if( l_cST.GetNextToken() != wxT("Declarations:"))
		return false;

	for (; !g_bError && (p_cFile.GetCurrentLine()
			< (p_cFile.GetLineCount() - 2)); l_sLine = p_cFile.GetNextLine())
	{		
		//l_sLine = p_cFile.GetNextLine();
		wxStringTokenizer l_cST(l_sLine,wxT(" \n\r\t"),wxTOKEN_STRTOK);
		if (!l_cST.CountTokens())
		{
			continue;
		}
		wxString l_sDeclType = l_cST.GetNextToken();		
		
		if( l_sDeclType == wxT("Colorset") )
			g_bError = !ParseColorset(l_sLine);
			
		if( l_sDeclType == wxT("Constant") )
			g_bError = !ParseConstant(l_sLine);
			
		if( l_sDeclType == wxT("Variable"))
			g_bError = !ParseVariable(l_sLine);
			
		if( l_sDeclType == wxT("Function") )
			g_bError = !ParseFunction(l_sLine);
			
		if( l_sDeclType == wxT("Place") )
			g_bError = !ParsePlace(l_sLine);

		if( l_sDeclType == wxT("Transition") )
			g_bError = !ParseTransition(l_sLine);

	}

	if (g_bError)
		SP_LOGDEBUG(wxString::Format(wxT(" error in line %lu"), p_cFile.GetCurrentLine()));

	return g_bError;
}





bool SP_ImportCSV2ColPN::ParseColorset(wxString& p_sLine)
{
	wxString l_sLine = p_sLine;
	wxStringTokenizer l_cST(l_sLine,wxT(" \n\r\t"),wxTOKEN_STRTOK);

	l_cST.GetNextToken(); //ignore the first token: colorset
	wxString l_sCSName = l_cST.GetNextToken();
	l_cST.GetNextToken(); //ignore the = operator
	wxString l_sDataType = l_cST.GetNextToken();
	l_cST.GetNextToken(); //ignore with
	wxString l_sColors = l_cST.GetNextToken();
	l_sColors = l_sColors.BeforeLast(wxT(';'));

	if( l_sDataType == wxT("dot")||
		l_sDataType == wxT("string")||
		l_sDataType == wxT("int")||
		l_sDataType == wxT("bool")||
		l_sDataType == wxT("enum")||
		l_sDataType == wxT("index"))
	{
		SP_DS_Metadataclass* l_pcMetadataclass;	
		l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
		if(!l_pcMetadataclass)
			return false;	
		SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		if(!l_pcNewMetadata)
			return false;	
		SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
		if(!l_pcColList)
			return false;
		unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

		l_pcColList->SetCell(l_nNewRow,0,l_sCSName);
		l_pcColList->SetCell(l_nNewRow,1,l_sDataType);
		l_pcColList->SetCell(l_nNewRow,2,l_sColors);
	}

	if( l_sDataType == wxT("product")||
		l_sDataType == wxT("union") )
	{
		SP_DS_Metadataclass* l_pcMetadataclass;	
		l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
		if(!l_pcMetadataclass)
			return false;	
		SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		if(!l_pcNewMetadata)
			return false;	
		SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));
		if(!l_pcColList)
			return false;
		unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

		l_pcColList->SetCell(l_nNewRow,0,l_sCSName);
		l_pcColList->SetCell(l_nNewRow,1,l_sDataType);
		l_pcColList->SetCell(l_nNewRow,2,l_sColors);
	}
		

	return true;
}
bool SP_ImportCSV2ColPN::ParseConstant(wxString& p_sLine)
{
	wxString l_sLine = p_sLine;
	wxStringTokenizer l_cST(l_sLine,wxT(" \n\r\t"),wxTOKEN_STRTOK);

	l_cST.GetNextToken(); //ignore the first token: colorset
	wxString l_sCSName = l_cST.GetNextToken();
	l_cST.GetNextToken(); //ignore the = operator
	wxString l_sDataType = l_cST.GetNextToken();
	l_cST.GetNextToken(); //ignore with
	wxString l_sColors = l_cST.GetNextToken();
	l_sColors = l_sColors.BeforeLast(wxT(';'));

	SP_DS_Metadataclass* l_pcMetadataclass;	
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);
	if(!l_pcMetadataclass)
		return false;	
	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;	
	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));
	if(!l_pcColList)
		return false;
	unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

	l_pcColList->SetCell(l_nNewRow,0,l_sCSName);
	l_pcColList->SetCell(l_nNewRow,1,l_sDataType);
	l_pcColList->SetCell(l_nNewRow,2,l_sColors);

	return true;
}
bool SP_ImportCSV2ColPN::ParseVariable(wxString& p_sLine)
{
	wxString l_sLine = p_sLine;
	wxStringTokenizer l_cST(l_sLine,wxT(" \n\r\t"),wxTOKEN_STRTOK);
	

	l_cST.GetNextToken(); //ignore the first token: variable
	wxString l_sVarName = l_cST.GetNextToken();
	l_cST.GetNextToken(); //ignore the : operator
	wxString l_sDataType = l_cST.GetNextToken();
	l_sDataType = l_sDataType.BeforeLast(wxT(';'));

	SP_DS_Metadataclass* l_pcMetadataclass;	
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);
	if(!l_pcMetadataclass)
		return false;	
	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;	
	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
	if(!l_pcColList)
		return false;
	unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

	l_pcColList->SetCell(l_nNewRow,0,l_sVarName);
	l_pcColList->SetCell(l_nNewRow,1,l_sDataType);

	

	return true;
}
bool SP_ImportCSV2ColPN::ParseFunction(wxString& p_sLine)
{
	wxString l_sLine = p_sLine;
	wxStringTokenizer l_cST(l_sLine,wxT(" \n\r\t"),wxTOKEN_STRTOK);

	l_cST.GetNextToken(); //ignore the first token: function
	wxString l_sReturnType = l_cST.GetNextToken();	
	wxString l_sFuncName = l_cST.GetNextToken();
	l_sFuncName = l_sFuncName.BeforeLast(wxT('('));
	
	wxString l_sParameterList = wxT("");
	while(true)
	{
		wxString l_sNext = l_cST.GetNextToken();
		if( l_sNext == wxT("{") )
			break;
		l_sParameterList = l_sParameterList + wxT(" ") + l_sNext;
	}
	l_sParameterList = l_sParameterList.BeforeLast(wxT(')'));

	wxString l_sFuncBody = wxT("");
	while(true)
	{
		wxString l_sNext = l_cST.GetNextToken();
		if( l_sNext == wxT("}") )
			break;
		l_sFuncBody = l_sFuncBody + wxT(" ") + l_sNext;
	}
	l_sFuncBody = l_sFuncBody.BeforeLast(wxT(';'));

	SP_DS_Metadataclass* l_pcMetadataclass;	
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
	if(!l_pcMetadataclass)
		return false;	
	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;	
	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
	if(!l_pcColList)
		return false;
	unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

	l_pcColList->SetCell(l_nNewRow,0,l_sReturnType);
	l_pcColList->SetCell(l_nNewRow,1,l_sFuncName);
	l_pcColList->SetCell(l_nNewRow,2,l_sParameterList);
	l_pcColList->SetCell(l_nNewRow,3,l_sFuncBody);


	return true;
}

bool SP_ImportCSV2ColPN::ParsePlace(wxString& p_sLine)
{
	wxString l_sLine = p_sLine;
	wxStringTokenizer l_cST(l_sLine,wxT(" \n\r\t"),wxTOKEN_STRTOK);

	l_cST.GetNextToken(); //ignore the first token: place
	wxString l_sName = l_cST.GetNextToken();
	l_cST.GetNextToken(); //ignore the = operator
	wxString l_sColourset = l_cST.GetNextToken();
	l_cST.GetNextToken(); //ignore with
	wxString l_sTokens = l_cST.GetNextToken();
	l_sTokens = l_sTokens.BeforeLast(wxT(';'));

	SP_VectorString l_Nodeclasses = {SP_DS_DISCRETE_PLACE, SP_DS_CONTINUOUS_PLACE};
	for(auto l_sNC : l_Nodeclasses)
	{
		SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(l_sNC);
		if(!l_pcNodeclass)
			continue;
		for(SP_DS_Node* l_pcNode : *(l_pcNodeclass->GetElements()))
		{
			SP_DS_NameAttribute* l_pcAttr = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));
			if(l_pcAttr && l_sName == l_pcAttr->GetValue())
			{
				SP_DS_Attribute* l_pcCSAttr = l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME);
				if(l_pcCSAttr)
				{
					l_pcCSAttr->SetValueString(l_sColourset);
				}

				SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(SP_DS_CPN_MARKINGLIST));
				if(l_pcColList)
				{
					l_pcColList->Clear();
					size_t i = 0;
					size_t p = l_sTokens.find(wxT("++"));
					while( i != wxString::npos)
					{
						wxString l_sToken = l_sTokens.substr(i,p-i);
						unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

						l_pcColList->SetCell(l_nNewRow,0,l_sToken.After('`'));
						l_pcColList->SetCell(l_nNewRow,1,l_sToken.Before('`'));

						i = (p != wxString::npos ? p+2 : p);
						p = l_sTokens.find(wxT("++"), i);
					}
				}
				return true;
			}
		}
	}
	return false;
}

bool SP_ImportCSV2ColPN::ParseTransition(wxString& p_sLine)
{
	wxString l_sLine = p_sLine;
	wxStringTokenizer l_cST(l_sLine,wxT(" \n\r\t"),wxTOKEN_STRTOK);

	l_cST.GetNextToken(); //ignore the first token: place
	wxString l_sName = l_cST.GetNextToken();
	l_cST.GetNextToken(); //ignore the = operator
	wxString l_sGuard = l_cST.GetNextToken();
	l_cST.GetNextToken(); //ignore with
	wxString l_sRatefunctions = l_cST.GetNextToken();
	l_sRatefunctions = l_sRatefunctions.BeforeLast(wxT(';'));

	SP_VectorString l_Nodeclasses = {SP_DS_STOCHASTIC_TRANS, SP_DS_CONTINUOUS_TRANS, SP_DS_IMMEDIATE_TRANS, SP_DS_DETERMINISTIC_TRANS, SP_DS_SCHEDULED_TRANS};
	for(auto l_sNC : l_Nodeclasses)
	{
		SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(l_sNC);
		if(!l_pcNodeclass)
			continue;
		for(SP_DS_Node* l_pcNode : *(l_pcNodeclass->GetElements()))
		{
			SP_DS_NameAttribute* l_pcAttr = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));
			if(l_pcAttr && l_sName == l_pcAttr->GetValue())
			{
				SP_DS_ColListAttribute* l_pcColList;
				l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(SP_DS_CPN_GUARDLIST));
				if(l_pcColList)
				{
					l_pcColList->Clear();
					unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

					l_pcColList->SetCell(l_nNewRow,0,wxT("Main"));
					l_pcColList->SetCell(l_nNewRow,1,l_sGuard);
				}

				if(l_sNC == SP_DS_DETERMINISTIC_TRANS)
				{
					l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("DelayList")));
				}
				else if(l_sNC == SP_DS_SCHEDULED_TRANS)
				{
					l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(wxT("PeriodicList")));
				}
				else
				{
					l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNode->GetAttribute(SP_DS_CPN_RATEFUNCTIONLIST));
				}

				if(l_pcColList)
				{
					l_pcColList->Clear();
					size_t i = 0;
					size_t p = l_sRatefunctions.find(wxT("++"));
					while( i != wxString::npos)
					{
						wxString l_sRF = l_sRatefunctions.substr(i,p-i);
						unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

						if(l_sRF.StartsWith(wxT("[")))
						{
							wxString l_sGuard = l_sRF.BeforeFirst(']');
							l_pcColList->SetCell(l_nNewRow,0,l_sGuard.AfterFirst('['));
							l_sRF = l_sRF.AfterFirst(']');
						}
						if(l_sNC == SP_DS_SCHEDULED_TRANS)
						{
							l_pcColList->SetCell(l_nNewRow,1,l_sRF.BeforeFirst(','));
							l_pcColList->SetCell(l_nNewRow,2,l_sRF.AfterFirst(',').BeforeFirst(','));
							l_pcColList->SetCell(l_nNewRow,3,l_sRF.AfterLast(','));
						}
						else
						{
							l_pcColList->SetCell(l_nNewRow,1,l_sRF);
						}


						i = (p != wxString::npos ? p+2 : p);
						p = l_sRatefunctions.find(wxT("++"), i);
					}
				}
				return true;
			}
		}
	}
	return false;
}


