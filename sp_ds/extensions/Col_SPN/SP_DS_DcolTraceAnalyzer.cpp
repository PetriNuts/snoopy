//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include <wx/valgen.h>
#include <wx/tipwin.h>


#include "sp_ds/extensions/Col_SPN/SP_DS_DColTraceAnalyzer.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/SP_DS_Node.h"

#include "sp_core/SP_Vector.h"


#include "sp_gui/dialogs/dia_CPN/SP_DLG_ColorSetSelection.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ValueAssign.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"
#include <wx/busyinfo.h> 

#include <wx/colordlg.h>
#include "sp_gui/dialogs/dia_ColSPN/TraceAnalyzer/SP_DLG_ColColoredPlaceSelection.h"

#include <wx/busyinfo.h> 


SP_DS_DColTraceAnalyzer::SP_DS_DColTraceAnalyzer(SP_DS_ColPN_Coloring* p_pcColored, const SP_Vector2DDouble& p_aanResults,
	SP_Vector2DDouble& p_aanUserResults, vector<wxString>&  p_vUserVariable)
	:m_pcColored(p_pcColored),
	m_aanResults(p_aanResults),
	m_aanUserResults(p_aanUserResults),
	m_vUserVariable(p_vUserVariable)
{
	GetColoredData();
}



bool SP_DS_DColTraceAnalyzer::GetColoredData()
{
	m_pvsColoredPlaceNames = m_pcColored->GetColoredPlaceNames();

	for (unsigned i = 0; i < m_pvsColoredPlaceNames->size(); i++)
	{
		wxString l_sPlName = (*m_pvsColoredPlaceNames)[i];
		m_mColoredPlNames2Pos[l_sPlName] = i;
	}

	//m_pnsPlaceCountById = m_pcColored->GetPlaceCountById();

	m_pvsUnPlaceNames = m_pcColored->GetColoredPlaceNames();

	//m_pmsColPlName2ColSet = m_pcColored->GetColPlName2ColSet();

	//m_pvsPlaceColor = m_pcColored->GetColorsOfPlace();

	//m_pcColorSetClass = m_pcColored->GetColorSetClass();

	return true;
}


bool SP_DS_DColTraceAnalyzer::Compute()
{
	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if (!m_pcGraph)
		return false;

	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_AUXILIARVARIABLESCLASS);

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AuxiliaryVariableList")));

	if (l_pcColList->GetRowCount() == 0)
		return true;

	for (unsigned int l_nRow = 0; l_nRow < l_pcColList->GetRowCount(); l_nRow++)
	{
		//variable
		m_sVarName = l_pcColList->GetCell(l_nRow, 0);
		if (m_sVarName == wxT(""))
		{
			wxString l_sError = wxT("The variable name should not be empty.");
			SP_MESSAGEBOX(l_sError, wxT("Trace analyzer"), wxOK | wxICON_ERROR);
			//return false;
			continue; // We ignore those variables where there are errors.
		}

		//predicate
		m_sCurPredicate = l_pcColList->GetCell(l_nRow, 3);
		m_PredType = GetPredicateType(m_sCurPredicate);

		wxString l_sUsedSign;
		l_sUsedSign << m_sCurPredicate << wxT(",") << m_PredType;

		//selected places
		wxString l_sSelColPlaceName = l_pcColList->GetCell(l_nRow, 1);
		if (!ProcessSelPlaces(l_sSelColPlaceName))
			//return false;
			continue;


		//function
		m_sCurFunction = l_pcColList->GetCell(l_nRow, 2);

		//predicate
		if (m_PredType == NEIGHBOR1D || m_PredType == NEIGHBOR2D)
		{
			//for the special function Neighbor1D and Neighbor2D		
			if (m_mExistingCell2NBNum.find(l_sUsedSign) == m_mExistingCell2NBNum.end())
			{
				if (!ComputeNeighborNumber())
					//return false;
					continue;

				m_mExistingCell2NBNum[l_sUsedSign] = m_vmCell2NBNum;
			}
			else
			{
				m_vmCell2NBNum.clear();
				m_vmCell2NBNum = m_mExistingCell2NBNum[l_sUsedSign];
			}

			if (!SumNeighbors())
				//return false;
				continue;
		}
		else
		{
			//for other general cases
			if (!ComputeGeneral())
				//return false;
				continue;
		}
		m_aanOriginalUserResults.push_back(m_vSingleTrace);
		m_vUserVariable.push_back(m_sVarName);
	}

	ReverseMatrix();

	return true;
}

void SP_DS_DColTraceAnalyzer::ReverseMatrix()
{
	unsigned int l_nRowNum = m_aanOriginalUserResults.size();
	if (l_nRowNum == 0)
		return;
	unsigned int l_nColNum = m_aanOriginalUserResults[0].size();
	if (l_nColNum == 0)
		return;

	m_aanUserResults.resize(l_nColNum);
	for (unsigned int i = 0; i < m_aanUserResults.size(); i++)
	{
		m_aanUserResults[i].resize(l_nRowNum);
	}

	for (unsigned int i = 0; i < l_nRowNum; i++)
	{
		for (unsigned int j = 0; j < l_nColNum; j++)
		{
			double l_nijVal = m_aanOriginalUserResults[i][j];
			m_aanUserResults[j][i] = l_nijVal;
		}
	}
}


SP_ColPN_NeighborDim SP_DS_DColTraceAnalyzer::GetPredicateType(wxString p_sPredicate)
{
	if (p_sPredicate.Find(wxT("Neighbor1D")) != wxNOT_FOUND)
		return NEIGHBOR1D;
	else if (p_sPredicate.Find(wxT("Neighbor2D")) != wxNOT_FOUND)
		return NEIGHBOR2D;
	else
		return NONEFUNCTION;
}


bool SP_DS_DColTraceAnalyzer::ProcessSelPlaces(wxString p_sSelColPlaceName)
{
	if (p_sSelColPlaceName == wxT(""))
	{
		wxString l_sError = wxT("The selected colored places should not be empty.");
		SP_MESSAGEBOX(l_sError, wxT("Trace analyzer"), wxOK | wxICON_ERROR);
		return false;
	}

	//to get colored places
	vector<wxString> l_vColoredPlName;
	while (p_sSelColPlaceName.Find(wxT(",")) != wxNOT_FOUND)
	{
		wxString l_sPlName = p_sSelColPlaceName.BeforeFirst(wxT(','));
		l_vColoredPlName.push_back(l_sPlName);
		p_sSelColPlaceName = p_sSelColPlaceName.AfterFirst(wxT(','));
	}
	l_vColoredPlName.push_back(p_sSelColPlaceName);

	//to check if these places have an identical color set	
	wxString l_sFirstColSet;
	for (unsigned i = 0; i < l_vColoredPlName.size(); i++)
	{
		wxString l_sColPlName = l_vColoredPlName[i];
		wxString l_sColSet = (*m_pmsColPlName2ColSet)[l_sColPlName];
		if (i == 0)
		{
			l_sFirstColSet = l_sColSet;
		}
		if (l_sFirstColSet != l_sColSet)
		{
			wxString l_sError = wxT("The selected colored places should have an identical color set.");
			SP_MESSAGEBOX(l_sError, wxT("Trace analyzer"), wxOK | wxICON_ERROR);
			return false;
		}
	}

	m_sSelColorSet = l_sFirstColSet;

	SP_CPN_ColorSet* l_pcColorSet = m_pcColorSetClass->LookupColorSet(m_sSelColorSet);
	if (!l_pcColorSet)
		return false;

	SP_CPN_DATATYPE l_DataType = l_pcColorSet->GetDataType();

	if (m_PredType == NEIGHBOR1D && l_DataType != CPN_INTEGER)
	{
		wxString l_sError = wxT("The color set of selected places mismatches the function Neighbor(x)");
		SP_MESSAGEBOX(l_sError, wxT("Trace analyzer"), wxOK | wxICON_ERROR);
		return false;
	}
	if (m_PredType == NEIGHBOR1D && l_DataType == CPN_INTEGER)
	{
		unsigned int l_nSize = l_pcColorSet->GetStringValue().size();
		if (l_nSize == 0)
			m_n1Dimenstion = 0;
		else
		{
			wxString l_sLast = (l_pcColorSet->GetStringValue())[l_nSize - 1];
			l_sLast.ToLong(&m_n1Dimenstion);
		}
		//take out of the last value
	}

	if (m_PredType == NEIGHBOR2D && l_DataType != CPN_PRODUCT)
	{
		//also we can further test its subcomponents
		wxString l_sError = wxT("The color set of selected places mismatches the function Neighbor(x,y)");
		SP_MESSAGEBOX(l_sError, wxT("Trace analyzer"), wxOK | wxICON_ERROR);
		return false;
	}
	if (m_PredType == NEIGHBOR2D && l_DataType == CPN_PRODUCT)
	{
		map<wxString, SP_CPN_ColorSet*> l_mCSName2Pointer;
		l_mCSName2Pointer = l_pcColorSet->GetComponentMap();
		if (l_mCSName2Pointer.size() != 2)
			return false;
		map<wxString, SP_CPN_ColorSet*>::iterator itMap;
		itMap = l_mCSName2Pointer.begin();
		vector<wxString> l_sColors1 = itMap->second->GetStringValue();
		if (l_sColors1.size() == 0)
			return false;
		wxString l_sLast1 = l_sColors1[l_sColors1.size() - 1];
		l_sLast1.ToLong(&m_n1Dimenstion);
		itMap++;
		vector<wxString> l_sColors2 = itMap->second->GetStringValue();
		if (l_sColors2.size() == 0)
			return false;
		wxString l_sLast2 = l_sColors2[l_sColors1.size() - 1];
		l_sLast2.ToLong(&m_n2Dimenstion);
	}

	//get the position of each unfoled place for these selected colored places
	m_sSelUPlPos2Color.clear();
	for (unsigned i = 0; i < l_vColoredPlName.size(); i++)
	{
		wxString l_sColPlName = l_vColoredPlName[i];
		long l_nColPlPos = m_mColoredPlNames2Pos[l_sColPlName];
		SP_CPN_CountInterval l_nnInterval = (*m_pnsPlaceCountById)[l_nColPlPos];
		for (unsigned int j = l_nnInterval.m_nLow; j <= l_nnInterval.m_nUp; j++)
		{
			wxString l_sUnPlColor = (*m_pvsPlaceColor)[j];
			m_sSelUPlPos2Color[j] = l_sUnPlColor;
		}
	}

	return true;
}






/*-------------------------------------------------------------------------------- */
//for the general sum case
bool SP_DS_DColTraceAnalyzer::ComputeGeneral()
{
	vector<wxString> l_vColors;
	SP_CPN_SyntaxChecking l_cSyntaxChecking;

	if (!l_cSyntaxChecking.GenerateSubSetColorSet(wxT(""), m_sCurPredicate, m_sSelColorSet, m_pcColorSetClass, l_vColors))
		return false;

	vector<long> l_vnSelPlPos;
	for (unsigned int i = 0; i < l_vColors.size(); i++)
	{
		wxString l_sColor = l_vColors[i];
		map<long, wxString>::iterator itMap;
		for (itMap = m_sSelUPlPos2Color.begin(); itMap != m_sSelUPlPos2Color.end(); itMap++)
		{
			if (itMap->second == l_sColor)
			{
				l_vnSelPlPos.push_back(itMap->first);
			}
		}
	}

	if (m_sCurFunction == wxT("SUM"))
	{
		GetSumSelUnPlaces(l_vnSelPlPos);
	}

	return true;
}


bool SP_DS_DColTraceAnalyzer::GetSumSelUnPlaces(vector<long>& p_vSelPlPos)
{
	unsigned long l_nTimePointCount = m_aanResults.size();

	m_vSingleTrace.clear();

	for (unsigned long l_nRow = 0; l_nRow < l_nTimePointCount; l_nRow++)
	{
		double l_dPlTokenVal = 0;
		for (unsigned l_n = 0; l_n < p_vSelPlPos.size(); l_n++)
		{
			long l_nCol = p_vSelPlPos[l_n];
			l_dPlTokenVal = l_dPlTokenVal + m_aanResults[l_nRow][l_nCol];
		}
		m_vSingleTrace.push_back(l_dPlTokenVal);
	}


	return true;
}



/*-------------------------------------------------------------------------------- */







/*-------------------------------------------------------------------------------- */
//for the special function Neighbor1D and Neighbor2D
//for 1D and 2D
bool SP_DS_DColTraceAnalyzer::ComputeNeighborNumber()
{
	unsigned long l_nTimePointCount = m_aanResults.size();
	unsigned long l_nPlaceCount = m_pvsPlaceColor->size();

	m_vmCell2NBNum.clear();

	for (unsigned long l_nRow = 0; l_nRow < l_nTimePointCount; l_nRow++)
	{
		map<wxString, bool> l_mCellExistToken;
		for (unsigned long l_nCol = 0; l_nCol < l_nPlaceCount; l_nCol++)
		{
			map<long, wxString>::iterator itMap;
			itMap = m_sSelUPlPos2Color.find(l_nCol);
			if (itMap == m_sSelUPlPos2Color.end())
				continue;
			//if match
			double l_dPlTokenVal = m_aanResults[l_nRow][l_nCol];

			wxString l_sColor = itMap->second;

			//only record those cells with tokens
			if (l_dPlTokenVal > 0.000001)
			{
				l_mCellExistToken[l_sColor] = true;
			}
		}

		//begin to compute		
		if (!GetCellNeighbors(l_mCellExistToken))
			return false;

		//if( ! SumNeighbors() )
		//  return false;
	}

	return true;
}




bool SP_DS_DColTraceAnalyzer::GetCellNeighbors(map<wxString, bool> &p_mCellExistToken)
{
	map<wxString, int> l_mCell2NBNum;
	map<wxString, bool>::iterator itMap;
	for (itMap = p_mCellExistToken.begin(); itMap != p_mCellExistToken.end(); itMap++)
	{
		wxString l_sCell = itMap->first;
		l_mCell2NBNum[l_sCell] = 0;

		map<wxString, bool>::iterator itMap2;
		for (itMap2 = p_mCellExistToken.begin(); itMap2 != p_mCellExistToken.end(); itMap2++)
		{
			//can not count itself
			if (itMap == itMap2)
				continue;
			wxString l_sNBCell = itMap2->first;

			//Evaluate if l_sNBCell is a neighbor of l_sCell;
			if (Match(l_sCell, l_sNBCell))
			{
				l_mCell2NBNum[l_sCell] = l_mCell2NBNum[l_sCell] + 1;
			}
		}
	}

	m_vmCell2NBNum.push_back(l_mCell2NBNum);

	return true;
}


bool SP_DS_DColTraceAnalyzer::Match(wxString p_sCell, wxString p_sNBCell)
{
	//(a=x | a = x+1 | a = x -1)  & (!(a=x) ) & (a <= D1) & (a >= 1)

	//(a=x | a = x+1 | a = x-1) &	(b=y | b = y+1 | b = y-1) &	(!(a=x & b=y)) & (a <= D1 & b <= D2) & (a >= 1 & b >= 1)

	if (m_PredType == NEIGHBOR1D)
	{
		long x;
		p_sCell.ToLong(&x);
		long a;
		p_sNBCell.ToLong(&a);

		if ((a == x || a == x + 1 || a == x - 1) && (!(a == x)) && (a <= m_n1Dimenstion) && (a >= 1))
			return true;

	}



	if (m_PredType == NEIGHBOR2D)
	{
		wxString l_sX = p_sCell.BeforeFirst(wxT(','));
		l_sX = l_sX.AfterFirst(wxT('('));
		wxString l_sY = p_sCell.AfterFirst(wxT(','));
		l_sY = l_sY.BeforeLast(wxT(')'));
		long x, y;
		l_sX.ToLong(&x);
		l_sY.ToLong(&y);

		wxString l_sA = p_sNBCell.BeforeFirst(wxT(','));
		l_sA = l_sA.AfterFirst(wxT('('));
		wxString l_sB = p_sNBCell.AfterFirst(wxT(','));
		l_sB = l_sB.BeforeLast(wxT(')'));
		long a, b;
		l_sA.ToLong(&a);
		l_sB.ToLong(&b);

		if ((a == x || a == x + 1 || a == x - 1) && (b == y || b == y + 1 || b == y - 1) && (!(a == x && b == y)) && (a <= m_n1Dimenstion && b <= m_n2Dimenstion) && (a >= 1 && b >= 1))
			return true;
	}


	return false;
}



bool SP_DS_DColTraceAnalyzer::ProcessPredicate()
{
	wxString p_sPredicate = m_sCurPredicate;
	m_pcParseContext = make_CPN_Parse_Context();
	m_pcDriver = make_CPN_Driver((*m_pcParseContext));

	//Set the ColorSetClass and colorset name for the parsing context	
	m_pcParseContext->SetErrorPosition(p_sPredicate);
	m_pcParseContext->SetColorSetClass(m_pcColorSetClass);
	m_pcParseContext->SetColorSetName(wxT(""));
	m_pcParseContext->SetFunctionFlag(false);
	//m_pcParseContext->SetNetClassName( wxT("") );
	m_pcParseContext->SetPlaceType(wxT("Place"));


	std::string l_ParseString;
	l_ParseString = string(p_sPredicate.mb_str());
	bool l_bSuccess = m_pcDriver->parse_string(l_ParseString, "input");

	if (!l_bSuccess)
	{
		wxString l_sParseError = m_pcDriver->GetError();
		wxString l_sError = l_sParseError + wxT(" | ") + p_sPredicate;
		SP_LOGERROR(l_sError);
		return false;
	}

	// call the check function
	if (!m_pcParseContext->CheckExpression())
		return false;

	if (m_pcParseContext->GetHighLevelNodeDataType() != CPN_BOOLEAN)
	{
		wxString l_sError = wxT("predicate should be a boolean expression. ");
		SP_LOGERROR(l_sError);
		return false;
	}

	return true;
}

bool SP_DS_DColTraceAnalyzer::SumNeighbors()
{
	//Initialize and check parser
	if (!ProcessPredicate())
		return false;

	m_vSingleTrace.clear();

	for (unsigned int i = 0; i < m_vmCell2NBNum.size(); i++)
	{
		map<wxString, int> l_mCell2NBNum;
		l_mCell2NBNum = m_vmCell2NBNum[i];

		//begin to feed l_mCell2NBNum into the predicate parser
		map<wxString, int>::iterator itCNBMap;
		int l_nCount = 0;
		for (itCNBMap = l_mCell2NBNum.begin(); itCNBMap != l_mCell2NBNum.end(); itCNBMap++)
		{
			wxString l_sCell = itCNBMap->first;
			int l_nNBUnumber = itCNBMap->second;

			//bind value to function
			if (!m_pcColorSetClass->SetNeighbor12DFuncValue(l_nNBUnumber))
				return false;
			//evaluate
			vector<SP_CPN_EvaluatedSingleValue> l_EvaluateResultArray;
			if (!m_pcParseContext->EvaluateExpression(l_EvaluateResultArray))
				return false;
			if (l_EvaluateResultArray.size() == 0)
				return false;
			if (l_EvaluateResultArray[0].m_ColorValue == wxT("true"))
				l_nCount++;
		}

		m_vSingleTrace.push_back(l_nCount);
	}

	return true;
}

/*-------------------------------------------------------------------------------- */





