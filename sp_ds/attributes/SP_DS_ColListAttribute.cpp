//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.10 $
// $Date: 2007/06/04 16:35:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"

SP_DS_ColListAttribute::SP_DS_ColListAttribute(const wxString& p_pchName,
		SP_COLLIST_TYPE p_eColType, unsigned int p_nColCount,
		const wxString& p_sGrouping, UIntPtr p_nActiveList, UIntPtr p_nActiveColumn) :
	SP_DS_Attribute(p_pchName, SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_COLLIST),
	m_nRowCount(0),
	m_nColCount(p_nColCount),
	m_eColListType(p_eColType),
	m_nActiveList(p_nActiveList),
	m_nActiveColumn(p_nActiveColumn),
	m_sGrouping(p_sGrouping)
{
	if(!m_nActiveList)
	{
		m_nActiveList = UIntPtr(new unsigned int(0));
	}

	if(!p_nActiveColumn)
		m_nActiveColumn = UIntPtr(new unsigned int(0));


	m_mmValues.clear();
	m_mColHead.clear();
}

SP_DS_ColListAttribute::~SP_DS_ColListAttribute()
{
	m_mmValues.clear();
	m_mColHead.clear();
}

SP_DS_Attribute* SP_DS_ColListAttribute::Clone(bool p_bCloneGr)
{

	SP_DS_ColListAttribute* l_pcNewColList = new SP_DS_ColListAttribute( GetName(), m_eColListType, m_nColCount, m_sGrouping, m_nActiveList,m_nActiveColumn );

	for (SP_MapString2UInt::iterator iter = m_mColHead.begin(); iter != m_mColHead.end(); iter++)
	{
		l_pcNewColList->SetColLabel(iter->second, iter->first);
	}

	for(SP_MapUInt2DString::iterator l_itRow = m_mmValues.begin(); l_itRow != m_mmValues.end(); l_itRow++)
	{
		l_pcNewColList->AppendEmptyRow();
		unsigned int l_nRow = l_itRow->first;
		SP_MapUInt2String& l_mRow = l_itRow->second;
		for(SP_MapUInt2String::iterator l_itCol = l_mRow.begin(); l_itCol != l_mRow.end(); l_itCol++)
		{
			unsigned int l_nCol = l_itCol->first;
			l_pcNewColList->SetCell(l_nRow, l_nCol, l_itCol->second);
		}
	}

	SP_DS_ColListAttribute* l_pcFirstAttr = NULL;
	if ( GetParent()->GetElementType() == SP_ELEMENT_NODE &&
		!dynamic_cast<SP_DS_Node*>(GetParent())->GetNodeclass()->GetElements()->empty())
	{
		SP_DS_Node* l_pcFirst = dynamic_cast<SP_DS_Node*>(GetParent())->GetNodeclass()->GetElements()->front();		
		l_pcFirstAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcFirst->GetAttribute(GetName()));
	}
	else if (GetParent()->GetElementType() == SP_ELEMENT_EDGE &&
			!dynamic_cast<SP_DS_Edge*>(GetParent())->GetEdgeclass()->GetElements()->empty())
	{
		SP_DS_Edge* l_pcFirst = dynamic_cast<SP_DS_Edge*>(GetParent())->GetEdgeclass()->GetElements()->front();
		l_pcFirstAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcFirst->GetAttribute(GetName()));
	}
	else if (GetParent()->GetElementType() == SP_ELEMENT_METADATA &&
			!dynamic_cast<SP_DS_Metadata*>(GetParent())->GetMetadataclass()->GetElements()->empty())
	{
		SP_DS_Metadata* l_pcFirst = dynamic_cast<SP_DS_Metadata*>(GetParent())->GetMetadataclass()->GetElements()->front();
		l_pcFirstAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcFirst->GetAttribute(GetName()));
	}
	if(l_pcFirstAttr)
	{
		for(unsigned int l_nCol = 0; l_nCol < l_pcFirstAttr->GetColCount(); ++l_nCol)
		{
			if(l_nCol >= l_pcNewColList->GetColCount())
			{
				l_pcNewColList->AppendEmptyColum();
			}
			l_pcNewColList->SetColLabel(l_nCol, l_pcFirstAttr->GetColLabel(l_nCol));
		}

		////////////////////////////////////////////////////////////////////////////////////////////////
		//for colored stochastic Petri Nets
		
		if( l_pcNewColList->GetName()==wxT("FunctionList") && l_pcNewColList->GetCell(0,0) != wxT("Main"))
		{
			//l_pcNewColList->SetCell(0, 0, wxT("true"));
			//for(unsigned int l_nColumn = 1; l_nColumn < l_pcNewColList->GetColCount(); l_nColumn++)
			{				
			//	l_pcNewColList->SetCell(0, l_nColumn, wxT("1"));				
			}
		}
		else if( l_pcNewColList->GetName()==wxT("DelayList") && l_pcNewColList->GetCell(0,0) != wxT("Main"))
		{
			//l_pcNewColList->SetCell(0, 0, wxT("true"));
			//for(unsigned int l_nColumn = 1; l_nColumn < l_pcNewColList->GetColCount(); l_nColumn++)
			{				
			//	l_pcNewColList->SetCell(0, l_nColumn, wxT("0"));				
			}
		}
		else if( l_pcNewColList->GetName()==wxT("PeriodicList") && l_pcNewColList->GetCell(0,0) != wxT("Main"))
		{
			//l_pcNewColList->SetCell(0, 0, wxT("true"));
			//for(unsigned int l_nColumn = 1; l_nColumn < l_pcNewColList->GetColCount(); l_nColumn++)
			{				
			//	l_pcNewColList->SetCell(0, l_nColumn, wxT("_SimStart"));	
			//	l_nColumn++;
			//	l_pcNewColList->SetCell(0, l_nColumn, wxT("?"));
			//	l_nColumn++;
			//	l_pcNewColList->SetCell(0, l_nColumn, wxT("_SimEnd"));
			}
		}
		else if( l_pcNewColList->GetName()==wxT("CurveInfo"))
		{
		}
		else
		
		
		////////////////////////////////////////////////////////////////////////////////////////////////

		for(unsigned int l_nRow = l_pcNewColList->GetRowCount(); l_nRow < l_pcFirstAttr->GetRowCount(); l_nRow++)
		{
			unsigned int l_nNewRow = l_pcNewColList->AppendEmptyRow();
			l_pcNewColList->SetCell(l_nNewRow, 0, l_pcFirstAttr->GetCell(l_nNewRow, 0));
			for(unsigned int l_nCol = 1; l_nCol < l_pcNewColList->GetColCount(); l_nCol++)
			{
				l_pcNewColList->SetCell(l_nNewRow, l_nCol, l_pcNewColList->GetCell(0, l_nCol));
			}
		}
		
	}

	// just clone the graphic stuff, too
	return CloneBase(l_pcNewColList, p_bCloneGr);

}

bool SP_DS_ColListAttribute::SetCell(unsigned int p_nRowNumber, unsigned int p_nColNumber, const wxString& p_nVal)
{
	if ( CheckRowIndex(p_nRowNumber) && CheckColIndex(p_nColNumber))
	{
		m_mmValues[p_nRowNumber][p_nColNumber] = p_nVal;
		return true;
	}

	return false;
}

bool SP_DS_ColListAttribute::SetCell(unsigned int p_nRowNumber, const wxString& p_sColKey, const wxString& p_nVal)
{
	unsigned int l_nColNumber = GetColNumberOfColLabel(p_sColKey);

	return SetCell(p_nRowNumber, l_nColNumber, p_nVal);
}

const wxString SP_DS_ColListAttribute::GetCell(unsigned int p_nRowNumber, unsigned int p_nColNumber)
{
	if ( CheckRowIndex(p_nRowNumber) && CheckColIndex(p_nColNumber))
	{
		SP_MapUInt2DString::iterator l_itRow = m_mmValues.find(p_nRowNumber);
		if(l_itRow != m_mmValues.end())
		{
			SP_MapUInt2String& l_mRow = l_itRow->second;
			SP_MapUInt2String::iterator l_itCol = l_mRow.find(p_nColNumber);
			if(l_itCol != l_mRow.end())
			{
				return l_itCol->second;
			}
		}
	}

	return wxT("");
}

const wxString SP_DS_ColListAttribute::GetCell(unsigned int p_nRowNumber, const wxString& p_sColKey)
{
	unsigned int l_nColNumber = GetColNumberOfColLabel(p_sColKey);

	return GetCell(p_nRowNumber, l_nColNumber);
}

bool SP_DS_ColListAttribute::RemoveRow(unsigned int p_nRowNumber)
{
	if (CheckRowIndex(p_nRowNumber))
	{
		m_nRowCount--;
		for (unsigned int i = p_nRowNumber; i < m_nRowCount; i++)
		{
			m_mmValues[ i ] = m_mmValues[ i + 1 ];
		}
		m_mmValues.erase(m_nRowCount);
		return true;
	}

	return false;
}

bool SP_DS_ColListAttribute::RemoveRemainingRows(unsigned int p_nStartRowNumber)
{
	if (CheckRowIndex(p_nStartRowNumber))
	{

		SP_MapUInt2DString::iterator l_itRow;

		l_itRow=m_mmValues.find(p_nStartRowNumber);


		m_mmValues.erase(l_itRow,m_mmValues.end());

		m_nRowCount=m_mmValues.size();

		return true;
	}

	return false;
}

bool SP_DS_ColListAttribute::RemoveCell(unsigned int p_nRowNumber, unsigned int p_nColNumber)
{
	if ( CheckRowIndex(p_nRowNumber) && CheckColIndex(p_nColNumber))
	{
		SP_MapUInt2DString::iterator l_itRow = m_mmValues.find(p_nRowNumber);
		if(l_itRow != m_mmValues.end())
		{
			SP_MapUInt2String& l_mRow = l_itRow->second;
			l_mRow.erase(p_nColNumber);
		}
	}
	return true;
}

bool SP_DS_ColListAttribute::RemoveCell(unsigned int p_nRowNumber, const wxString& p_sColKey)
{
	unsigned int l_nColNumber = GetColNumberOfColLabel(p_sColKey);

	return RemoveCell(p_nRowNumber, l_nColNumber);
}

wxString SP_DS_ColListAttribute::GetColLabel(unsigned int p_nColNumber)
{
	wxString l_sColLabelValue = wxT("");
	if (CheckColIndex(p_nColNumber))
	{
		SP_MapString2UInt::iterator itMap;
		for(itMap = m_mColHead.begin(); itMap != m_mColHead.end(); itMap++)
		{
			if(itMap->second == p_nColNumber)
			{
				l_sColLabelValue = itMap->first;
				break;
			}
		}
	}
	return l_sColLabelValue;
}

bool SP_DS_ColListAttribute::SetColLabel(unsigned int p_nColNumber, const wxString& p_sColLabel)
{
	if (CheckColIndex(p_nColNumber))
	{
		RemoveExistingIndex(p_nColNumber);
		m_mColHead[p_sColLabel] = p_nColNumber;
		return true;
	}

	return false;
}


void SP_DS_ColListAttribute::RemoveExistingIndex(unsigned int p_nColNumber)
{
	SP_MapString2UInt::iterator itMap = m_mColHead.begin();
	while(itMap != m_mColHead.end())
	{
		if(itMap->second == p_nColNumber)
		{
			m_mColHead.erase(itMap);
			itMap = m_mColHead.begin();
		}
		else
		{
			++itMap;
		}
	}	
}



unsigned int SP_DS_ColListAttribute::GetColNumberOfColLabel(const wxString& p_sColKey)
{
	SP_MapString2UInt::iterator iter = m_mColHead.find(p_sColKey);

	if (iter != m_mColHead.end() )
	{
		return iter->second;
	}

	return m_nColCount;
}

wxString SP_DS_ColListAttribute::GetValueString()
{
	wxASSERT_MSG(false, wxT("method not implemented"));
	return wxT("");
}

bool SP_DS_ColListAttribute::SetValueString(const wxString& p_pchVal, bool p_bCheckValue)
{

	return true;

}

unsigned int
SP_DS_ColListAttribute::AppendEmptyRow()
{
	unsigned int l_nReturn = m_nRowCount++;

	return l_nReturn;
}

unsigned int
SP_DS_ColListAttribute::AppendEmptyColum()
{
	unsigned int l_nReturn = m_nColCount++;

	return l_nReturn;
}

void SP_DS_ColListAttribute::RemoveAllColumns()
{
	m_nColCount = 0;
}

void
SP_DS_ColListAttribute::Clear()
{
	m_mmValues.clear();
	m_nRowCount = 0;
}

void
SP_DS_ColListAttribute::SetActiveList(unsigned int p_nActiveList)
{
	if(p_nActiveList < m_nRowCount && *m_nActiveList != p_nActiveList)
	{
		*m_nActiveList = p_nActiveList;

		///////////
		// invalidate function registry, because another list was activated
		///////////
		SP_DS_Graph* l_pcGraph = GetParent()->GetClassObject()->GetParentGraph();
		SP_DS_FunctionRegistry* l_pcFR = l_pcGraph->GetFunctionRegistry();
		l_pcFR->invalidate();
	}
}

unsigned int
SP_DS_ColListAttribute::GetActiveList()
{
	if(*m_nActiveList < m_nRowCount)
		return *m_nActiveList;

	return 0;
}

//for colored spn
void
SP_DS_ColListAttribute::SetActiveColumn(unsigned int p_nActiveColumn)
{
	if(p_nActiveColumn < m_nColCount && *m_nActiveColumn != p_nActiveColumn)
	{
		*m_nActiveColumn = p_nActiveColumn;
	}
}

unsigned int
SP_DS_ColListAttribute::GetActiveColumn()
{
	if(*m_nActiveColumn < m_nColCount)
		return *m_nActiveColumn;

	return 0;
}

const wxString
SP_DS_ColListAttribute::GetActiveCellValue(unsigned int p_nColNumber )
{
	wxString l_sRet = GetCell(*m_nActiveList,p_nColNumber);
	if(l_sRet.IsEmpty())
	{
		l_sRet = GetCell(0,p_nColNumber);
	}
	return l_sRet;
}

long double SP_DS_ColListAttribute::GetActiveCellValueDouble(unsigned int p_nColNumber )
{
	double l_nVal = 0;
	wxString l_sVal = GetCell(*m_nActiveList,p_nColNumber);
	if(l_sVal.IsEmpty())
	{
		l_sVal = GetCell(0,p_nColNumber);
	}
	l_sVal.ToDouble(&l_nVal);
	return l_nVal;
}
void SP_DS_ColListAttribute::SetActiveCellValueDouble(unsigned int p_nColNumber,long double p_nVal )
{
	 double l_nValDouble = (double) p_nVal;
	 wxString l_sVal;
	 l_sVal.Printf(wxT("%.16G"), l_nValDouble);
	 SetCell(*m_nActiveList,p_nColNumber,l_sVal);

}

void
SP_DS_ColListAttribute::UpdateActiveListColumnPtr()
{

	if(!m_sGrouping.IsEmpty())
	{
		SP_DS_Attribute* l_pcAttr = GetParent()->GetAttribute(m_sGrouping);
		wxString l_sMyValueString;
		if(l_pcAttr)
		{
			l_sMyValueString = l_pcAttr->GetValueString();
		}
		SP_DS_ColListAttribute* l_pcColListAttr = NULL;
		if ( GetParent()->GetElementType() == SP_ELEMENT_NODE &&
			dynamic_cast<SP_DS_Node*>(GetParent())->GetNodeclass()->GetElements()->size() > 1)
		{
			SP_DS_Node* l_pcParent = dynamic_cast<SP_DS_Node*>(GetParent());
			SP_ListNode::const_iterator it = l_pcParent->GetNodeclass()->GetElements()->begin();
			SP_ListNode::const_iterator end = l_pcParent->GetNodeclass()->GetElements()->end();
			for(; it != end && l_pcColListAttr == NULL; ++it)
			{
				if(GetParent() != (*it))
				{
					l_pcAttr = (*it)->GetAttribute(m_sGrouping);
					if(l_pcAttr)
					{
						if(l_sMyValueString == l_pcAttr->GetValueString())
						{
							l_pcColListAttr = dynamic_cast<SP_DS_ColListAttribute*>((*it)->GetAttribute(GetName()));
						}
					}
				}
			}
		}
		else if (GetParent()->GetElementType() == SP_ELEMENT_EDGE &&
				dynamic_cast<SP_DS_Edge*>(GetParent())->GetEdgeclass()->GetElements()->size() > 1)
		{
			SP_DS_Edge* l_pcParent = dynamic_cast<SP_DS_Edge*>(GetParent());
			SP_ListEdge::const_iterator it = l_pcParent->GetEdgeclass()->GetElements()->begin();
			SP_ListEdge::const_iterator end = l_pcParent->GetEdgeclass()->GetElements()->end();
			for(; it != end && l_pcColListAttr == NULL; ++it)
			{
				if(GetParent() != (*it))
				{
					l_pcAttr = (*it)->GetAttribute(m_sGrouping);
					if(l_pcAttr)
					{
						if(l_sMyValueString == l_pcAttr->GetValueString())
						{
							l_pcColListAttr = dynamic_cast<SP_DS_ColListAttribute*>((*it)->GetAttribute(GetName()));
						}
					}
				}
			}
		}
		else if (GetParent()->GetElementType() == SP_ELEMENT_METADATA &&
				dynamic_cast<SP_DS_Metadata*>(GetParent())->GetMetadataclass()->GetElements()->size() > 1)
		{
			SP_DS_Metadata* l_pcParent = dynamic_cast<SP_DS_Metadata*>(GetParent());
			SP_ListMetadata::const_iterator it = l_pcParent->GetMetadataclass()->GetElements()->begin();
			SP_ListMetadata::const_iterator end = l_pcParent->GetMetadataclass()->GetElements()->end();
			for(; it != end && l_pcColListAttr == NULL; ++it)
			{
				if(GetParent() != (*it))
				{
					l_pcAttr = (*it)->GetAttribute(m_sGrouping);
					if(l_pcAttr)
					{
						if(l_sMyValueString == l_pcAttr->GetValueString())
						{
							l_pcColListAttr = dynamic_cast<SP_DS_ColListAttribute*>((*it)->GetAttribute(GetName()));
						}
					}
				}
			}
		}
		if(l_pcColListAttr)
		{
			m_nActiveList = l_pcColListAttr->GetActiveListPtr();
			m_nActiveColumn = l_pcColListAttr->GetActiveColumnPtr();
		}
		else
		{
			m_nActiveList = UIntPtr(new unsigned int(0));
			m_nActiveColumn = UIntPtr(new unsigned int(0));
		}
	}
}

bool SP_DS_ColListAttribute::CopyValueFrom(SP_DS_Attribute* p_pcAttribute)
{
	if (! Equals(p_pcAttribute, false))
	{
		return false;
	}

	SP_DS_ColListAttribute* l_pcOldColList = dynamic_cast<SP_DS_ColListAttribute*>(p_pcAttribute);

	Clear();

	for (unsigned int l_nRow = 0; l_nRow < l_pcOldColList->GetRowCount(); l_nRow++)
	{
		//add a new row
		AppendEmptyRow();

		for (unsigned int l_nCol = 0; l_nCol < l_pcOldColList->GetColCount(); l_nCol++)
		{

			wxString l_sValue = l_pcOldColList->GetCell(l_nRow, l_nCol);

			//copy the cell value
			SetCell(l_nRow, l_nCol, l_sValue);
		}
	}

	//copy the active row
	SetActiveList(l_pcOldColList->GetActiveList());

	//copy active column
	SetActiveColumn(l_pcOldColList->GetActiveColumn());

	return true;
}
