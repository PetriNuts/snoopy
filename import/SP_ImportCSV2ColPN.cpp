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

#include "sp_utilities.h"


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

	g_bError = false;
	g_sMessage = wxT("");

	wxTextFile l_cFile;
	if (l_cFile.Open(p_sFile))
	{	
		bool l_bFlag = true;

		if( ! Parse(l_cFile) )
			l_bFlag = false;

		l_cFile.Close();

	    dynamic_cast<SP_MDI_View*>(l_pcDoc->GetFirstView())->DoRefresh();

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

	//TODO: bug in GetNextLine() if file is eof
	for (wxString l_sLine = p_cFile.GetFirstLine();
		!g_bError && (!p_cFile.Eof());
		l_sLine = p_cFile.GetNextLine())
	{		
		//l_sLine = p_cFile.GetNextLine();
		wxStringTokenizer l_cST(l_sLine,wxT(" \n\r\t"),wxTOKEN_STRTOK);
		if (!l_cST.CountTokens())
		{
			continue;
		}
		wxString token = l_cST.GetNextToken();
		if(token == wxT("Declarations:"))
		{
			g_bError = !ParseDeclarations(p_cFile);
		}
		else if(token == wxT("Place:"))
		{
			g_bError = !ParsePlacesTransitions(p_cFile);
		}
		else if(token == wxT("Transition:"))
		{
			g_bError = !ParsePlacesTransitions(p_cFile);
		}
		else if(token == wxT("Parameter:"))
		{
			g_bError = !ParseParameters(p_cFile);
		}
		if(p_cFile.Eof())
			break;
	}

	if (g_bError)
		SP_LOGDEBUG(wxString::Format(wxT(" error in line %lu"), p_cFile.GetCurrentLine()));

	return g_bError;
}


bool SP_ImportCSV2ColPN::ParseDeclarations(wxTextFile& p_cFile)
{
	for (wxString l_sLine = p_cFile.GetNextLine();
		!g_bError && (!p_cFile.Eof());
		l_sLine = p_cFile.GetNextLine())
	{
		//l_sLine = p_cFile.GetNextLine();
		wxStringTokenizer l_cST(l_sLine,wxT(" \n\r\t"),wxTOKEN_STRTOK);
		if (!l_cST.CountTokens())
		{
			continue;
		}
		wxString l_sDeclType = l_cST.GetNextToken();

		if( l_sDeclType == wxT("Colorset") )
		{
			g_bError = !ParseColorset(l_sLine);
		}
		else if( l_sDeclType == wxT("Constant") )
		{
			g_bError = !ParseConstant(l_sLine);
		}
		else if( l_sDeclType == wxT("Variable"))
		{
			g_bError = !ParseVariable(l_sLine);
		}
		else if( l_sDeclType == wxT("Function") )
		{
			g_bError = !ParseFunction(l_sLine);
		}
		else
		{
			p_cFile.GetPrevLine();
			return true;
		}
	}
	return !g_bError;
}

bool SP_ImportCSV2ColPN::ParsePlacesTransitions(wxTextFile& p_cFile)
{
	wxString l_sNodeclass;
	SP_DS_Node* l_pcPlace = nullptr;
	for (wxString l_sLine = p_cFile.GetNextLine();
		!g_bError && (!p_cFile.Eof());
		l_sLine = p_cFile.GetNextLine())
	{
		l_sLine.Trim();
		if(l_sLine.IsEmpty()){ continue; }

		wxStringTokenizer l_cST(l_sLine, wxT(";"), wxTOKEN_RET_EMPTY_ALL);
		if(l_cST.CountTokens() == 1)
		{
			p_cFile.GetPrevLine();
			return true;
		}
		while ( l_cST.HasMoreTokens() )
		{
		    wxString token = l_cST.GetNextToken();
		    token.Trim();
		    // process token here
		    if(token == SP_DS_DISCRETE_PLACE
		    	|| token == SP_DS_CONTINUOUS_PLACE
		    	|| token == SP_DS_STOCHASTIC_TRANS
		    	|| token == SP_DS_IMMEDIATE_TRANS
		    	|| token == SP_DS_CONTINUOUS_TRANS
		    	|| token == SP_DS_DETERMINISTIC_TRANS
		    	|| token == SP_DS_SCHEDULED_TRANS
				)
		    {
		    	l_sNodeclass = token;
		    	wxArrayString l_Labels;
				while ( l_cST.HasMoreTokens() )
				{
				    token = l_cST.GetNextToken();
				    l_Labels.Add(token);
				}
				SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(l_sNodeclass);
				if(!l_pcNodeclass)
					continue;
				for(SP_DS_Node* l_pcNode : *(l_pcNodeclass->GetElements()))
				{
					SP_DS_ColListAttribute* l_pcAttr = nullptr;
					if(l_sNodeclass == SP_DS_DISCRETE_PLACE
						|| l_sNodeclass == SP_DS_CONTINUOUS_PLACE)
					{
					    l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("MarkingList")));
					}
					else if(l_sNodeclass == SP_DS_STOCHASTIC_TRANS
							|| l_sNodeclass == SP_DS_IMMEDIATE_TRANS
							|| l_sNodeclass == SP_DS_CONTINUOUS_TRANS)
					{
						l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));
					}
					else if(l_sNodeclass==SP_DS_DETERMINISTIC_TRANS)
					{
						l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("DelayList")));
					}
					else if(l_sNodeclass==SP_DS_SCHEDULED_TRANS)
					{
						l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("PeriodicList")));
					}
					if(l_pcAttr)
					{
						bool reset = true;
						if (l_pcAttr->GetColCount() == l_Labels.size())
						{
							reset = false;
							for(size_t i = 0; i < l_Labels.size(); ++i)
							{
								reset |= (l_pcAttr->GetColLabel(i) != l_Labels[i]);
							}
						}
						if(reset)
						{
							l_pcAttr->Clear();
							l_pcAttr->RemoveAllColumns();
							for(size_t i = 0; i < l_Labels.size(); ++i)
							{
								l_pcAttr->AppendEmptyColum();
								l_pcAttr->SetColLabel(i, l_Labels[i]);
							}
						}
					}
				}
		    }
		    else
		    {
		    	if(l_sNodeclass.IsEmpty())
		    	{
		    		SP_LOGERROR(wxT("SP_ImportCSV2ColPN::ParsePlaces: no node class defined!"));
		    		return false;
		    	}
		    	else if(!token.IsEmpty())
		    	{
					SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(l_sNodeclass);
					if(!l_pcNodeclass)
						continue;
					for(SP_DS_Node* l_pcNode : *(l_pcNodeclass->GetElements()))
					{
						SP_DS_NameAttribute* l_pcAttr = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));
						if(l_pcAttr && token == l_pcAttr->GetValue())
						{
							l_pcPlace = l_pcNode;
							SP_DS_ColListAttribute* l_pcAttr = nullptr;
							if(l_sNodeclass == SP_DS_DISCRETE_PLACE
								|| l_sNodeclass == SP_DS_CONTINUOUS_PLACE)
							{
							    l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcPlace->GetAttribute(wxT("MarkingList")));
							}
							else if(l_sNodeclass == SP_DS_STOCHASTIC_TRANS
									|| l_sNodeclass == SP_DS_IMMEDIATE_TRANS
									|| l_sNodeclass == SP_DS_CONTINUOUS_TRANS)
							{
								l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcPlace->GetAttribute(wxT("FunctionList")));
							}
							else if(l_sNodeclass==SP_DS_DETERMINISTIC_TRANS)
							{
								l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcPlace->GetAttribute(wxT("DelayList")));
							}
							else if(l_sNodeclass==SP_DS_SCHEDULED_TRANS)
							{
								l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcPlace->GetAttribute(wxT("PeriodicList")));
							}
						    if(l_pcAttr)
						    {
						    	l_pcAttr->Clear();
						    }
							break;
						}
					}
		    	}
				SP_DS_ColListAttribute* l_pcAttr = nullptr;
				if(l_sNodeclass == SP_DS_DISCRETE_PLACE
					|| l_sNodeclass == SP_DS_CONTINUOUS_PLACE)
				{
				    l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcPlace->GetAttribute(wxT("MarkingList")));
				}
				else if(l_sNodeclass == SP_DS_STOCHASTIC_TRANS
						|| l_sNodeclass == SP_DS_IMMEDIATE_TRANS
						|| l_sNodeclass == SP_DS_CONTINUOUS_TRANS)
				{
					l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcPlace->GetAttribute(wxT("FunctionList")));
				}
				else if(l_sNodeclass==SP_DS_DETERMINISTIC_TRANS)
				{
					l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcPlace->GetAttribute(wxT("DelayList")));
				}
				else if(l_sNodeclass==SP_DS_SCHEDULED_TRANS)
				{
					l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcPlace->GetAttribute(wxT("PeriodicList")));
				}
			    if(l_pcAttr)
			    {
			    	size_t i = 0;
			    	size_t r = l_pcAttr->AppendEmptyRow();
					while ( l_cST.HasMoreTokens() )
					{
					    token = l_cST.GetNextToken();
					    token.Trim();
					    l_pcAttr->SetCell(r, i, token);
					    ++i;
					}
			    }
		    }
		}
	}
	return true;
}

bool SP_ImportCSV2ColPN::ParseParameters(wxTextFile& p_cFile)
{
	wxArrayString l_Labels;
	wxString l_sNodeclass;
	SP_DS_Node* l_pcPlace = nullptr;
	for (wxString l_sLine = p_cFile.GetNextLine();
		!g_bError && (!p_cFile.Eof());
		l_sLine = p_cFile.GetNextLine())
	{
		l_sLine.Trim();
		if(l_sLine.IsEmpty()){ continue; }

		wxStringTokenizer l_cST(l_sLine, wxT(";"), wxTOKEN_RET_EMPTY_ALL);
		if(l_cST.CountTokens() == 1)
		{
			p_cFile.GetPrevLine();
			return true;
		}
		while ( l_cST.HasMoreTokens() )
		{
		    wxString token = l_cST.GetNextToken();
		    token.Trim();
		    // process token here
		    if(token == SP_DS_PARAM)
		    {
		    	l_sNodeclass = token;
				while ( l_cST.HasMoreTokens() )
				{
				    token = l_cST.GetNextToken();
				    l_Labels.Add(token);
				}
				SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(l_sNodeclass);
				if(!l_pcNodeclass)
					continue;
				for(SP_DS_Node* l_pcNode : *(l_pcNodeclass->GetElements()))
				{
					SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("ParameterList")));
					if(l_pcAttr)
					{
						bool reset = true;
						if (l_pcAttr->GetRowCount() == l_Labels.size())
						{
							reset = false;
							for(size_t i = 0; i < l_Labels.size(); ++i)
							{
								reset |= (l_pcAttr->GetCell(i, 0) != l_Labels[i]);
							}
						}
						if(reset)
						{
							l_pcAttr->Clear();
							for(size_t i = 0; i < l_Labels.size(); ++i)
							{
								l_pcAttr->AppendEmptyRow();
								l_pcAttr->SetCell(i, 0, l_Labels[i]);
							}
						}
					}
				}
		    }
		    else
		    {
		    	if(l_sNodeclass.IsEmpty())
		    	{
		    		SP_LOGERROR(wxT("SP_ImportCSV2ColPN::ParsePlaces: no node class defined!"));
		    		return false;
		    	}
		    	else if(!token.IsEmpty())
		    	{
					SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass(l_sNodeclass);
					if(!l_pcNodeclass)
						continue;
					for(SP_DS_Node* l_pcNode : *(l_pcNodeclass->GetElements()))
					{
						SP_DS_NameAttribute* l_pcAttr = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME));
						if(l_pcAttr && token == l_pcAttr->GetValue())
						{
							l_pcPlace = l_pcNode;
							break;
						}
					}
		    	}
				SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcPlace->GetAttribute(wxT("ParameterList")));
			    if(l_pcAttr)
			    {
			    	size_t r = 0;
					while ( l_cST.HasMoreTokens() )
					{
					    token = l_cST.GetNextToken();
					    token.Trim();
					    l_pcAttr->SetCell(r, 1, token);
					    ++r;
					}
			    }
		    }
		}
	}
	return true;
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

		bool substituted = false;
		for(unsigned r = 0; r < l_pcColList->GetRowCount(); ++r)
		{
			if(l_pcColList->GetCell(r,0) == l_sCSName)
			{
				l_pcColList->SetCell(r,1,l_sDataType);
				l_pcColList->SetCell(r,2,l_sColors);

				substituted = true;
				break;
			}
		}
		if(!substituted)
		{
			unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

			l_pcColList->SetCell(l_nNewRow,0,l_sCSName);
			l_pcColList->SetCell(l_nNewRow,1,l_sDataType);
			l_pcColList->SetCell(l_nNewRow,2,l_sColors);
		}
	}
	else
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
		bool substituted = false;
		for(unsigned r = 0; r < l_pcColList->GetRowCount(); ++r)
		{
			if(l_pcColList->GetCell(r,0) == l_sCSName)
			{
				l_pcColList->SetCell(r,1,l_sDataType);
				l_pcColList->SetCell(r,2,l_sColors);

				substituted = true;
				break;
			}
		}
		if(!substituted)
		{
			unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

			l_pcColList->SetCell(l_nNewRow,0,l_sCSName);
			l_pcColList->SetCell(l_nNewRow,1,l_sDataType);
			l_pcColList->SetCell(l_nNewRow,2,l_sColors);
		}
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
	bool substituted = false;
	for(unsigned r = 0; r < l_pcColList->GetRowCount(); ++r)
	{
		if(l_pcColList->GetCell(r,0) == l_sCSName)
		{
			l_pcColList->SetCell(r,1,l_sDataType);
			l_pcColList->SetCell(r,2,l_sColors);

			substituted = true;
			break;
		}
	}
	if(!substituted)
	{
		unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

		l_pcColList->SetCell(l_nNewRow,0,l_sCSName);
		l_pcColList->SetCell(l_nNewRow,1,l_sDataType);
		l_pcColList->SetCell(l_nNewRow,2,l_sColors);
	}

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
	bool substituted = false;
	for(unsigned r = 0; r < l_pcColList->GetRowCount(); ++r)
	{
		if(l_pcColList->GetCell(r,0) == l_sVarName)
		{
			l_pcColList->SetCell(r,1,l_sDataType);

			substituted = true;
			break;
		}
	}
	if(!substituted)
	{
		unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

		l_pcColList->SetCell(l_nNewRow,0,l_sVarName);
		l_pcColList->SetCell(l_nNewRow,1,l_sDataType);
	}

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
	bool substituted = false;
	for(unsigned r = 0; r < l_pcColList->GetRowCount(); ++r)
	{
		if(l_pcColList->GetCell(r,1) == l_sFuncName)
		{
			l_pcColList->SetCell(r,0,l_sReturnType);
			l_pcColList->SetCell(r,1,l_sFuncName);
			l_pcColList->SetCell(r,2,l_sParameterList);
			l_pcColList->SetCell(r,3,l_sFuncBody);

			substituted = true;
			break;
		}
	}
	if(!substituted)
	{
		unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

		l_pcColList->SetCell(l_nNewRow,0,l_sReturnType);
		l_pcColList->SetCell(l_nNewRow,1,l_sFuncName);
		l_pcColList->SetCell(l_nNewRow,2,l_sParameterList);
		l_pcColList->SetCell(l_nNewRow,3,l_sFuncBody);
	}

	return true;
}


