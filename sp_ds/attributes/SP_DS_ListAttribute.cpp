//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl, md $
// $Version: 0.0 $
// $Revision: 1.10 $
// $Date: 2005/08/11 16:35:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/attributes/SP_DS_ListAttribute.h"

SP_DS_ListAttribute::SP_DS_ListAttribute(const wxString& p_pchName,
		const wxString& p_sVal, bool p_bObeySelection) :
	SP_DS_Attribute(p_pchName, SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_LIST), m_nNumOfValues(0),
			m_nChoosenValueIndex(0), m_eListType(SP_ATTRIBUTELIST_TEXT),
			m_bObeySelection(p_bObeySelection)
{
	m_pcValues.clear();
	SetValueString(p_sVal);
}

SP_DS_ListAttribute::~SP_DS_ListAttribute()
{
	m_pcValues.clear();
}

SP_DS_Attribute* SP_DS_ListAttribute::Clone(bool p_bCloneGr)
{
	SP_DS_ListAttribute* l_pcAttr = new SP_DS_ListAttribute(GetName(), GetValueString(), m_bObeySelection);

	// just clone the graphic stuff, too
	return CloneBase(l_pcAttr, p_bCloneGr);
}

bool SP_DS_ListAttribute::AddValue(const wxString& p_sVal)
{
	m_pcValues[m_nNumOfValues] = p_sVal;
	m_nNumOfValues++;
	return TRUE;
}

bool SP_DS_ListAttribute::RemoveValue(long p_nIndex)
{
	if ((0 < m_nNumOfValues) && (p_nIndex < m_nNumOfValues))
	{
		m_nNumOfValues--;
		for (long i = p_nIndex; i < m_nNumOfValues; i++)
		{
			m_pcValues[i] = m_pcValues[i+1];
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool SP_DS_ListAttribute::SetValue(const wxString& p_nVal)
{
	if (m_nChoosenValueIndex < m_nNumOfValues)
	{
		return SetValue(p_nVal, m_nChoosenValueIndex);
	}
	else
	{
		return SetValue(p_nVal, 0);
	}
}

bool SP_DS_ListAttribute::SetValue(const wxString& p_nVal, long p_nIndex)
{
	if (p_nIndex < m_nNumOfValues)
	{
		m_pcValues[p_nIndex] = p_nVal;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

wxString SP_DS_ListAttribute::GetValue()
{
	if (m_nChoosenValueIndex < m_nNumOfValues)
	{
		return GetValue(m_nChoosenValueIndex);
	}
	else if (0 < m_nNumOfValues)
	{
		return GetValue(0);
	}
	else
	{
		return wxT("");
	}
}

wxString SP_DS_ListAttribute::GetValue(long p_nIndex)
{
	if (p_nIndex < m_nNumOfValues)
	{
		return m_pcValues[p_nIndex];
	}
	else
	{
		return wxT("");
	}
}

bool SP_DS_ListAttribute::SetChoosenValueIndex(long p_nVal)
{
	if (p_nVal < m_nNumOfValues)
	{
		m_nChoosenValueIndex = p_nVal;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

long SP_DS_ListAttribute::GetChoosenValueIndex()
{
	if (m_nChoosenValueIndex < m_nNumOfValues)
	{
		return m_nChoosenValueIndex;
	}
	else
	{
		return m_nNumOfValues - 1;
	}
}

wxString SP_DS_ListAttribute::GetValueString()
{
	wxString l_sVal = wxT("");
	map<wxString, unsigned int> l_mMultipleValues;

	if (0 < m_nNumOfValues)
	{
		if (0 == m_nChoosenValueIndex && m_bObeySelection)
		{
			l_sVal.Printf(wxT("# %s"), GetValue(0).c_str());
		}
		else
		{
			l_sVal.Printf(wxT("%s"), GetValue(0).c_str());
		}
		for (long i = 1; i < m_nNumOfValues; i++)
		{
			//added by MD
			//remembering multiple occurences
			if (l_sVal.BeforeFirst(wxT(';')).Cmp(GetValue(i)) == 0 || l_sVal.AfterLast(';').Cmp(wxT(" ") + GetValue(i)) == 0 || l_sVal.Find(wxT("; ") + GetValue(i) + wxT(";")) > -1)
			{
				if (l_mMultipleValues.find(GetValue(i))
						== l_mMultipleValues.end())
				{
					l_mMultipleValues[GetValue(i)] = 2;
				}
				else
				{
					l_mMultipleValues[GetValue(i)]++;
				}
			}
			else
			{
				//---------------------------------------------------------------------
				if (i == m_nChoosenValueIndex && m_bObeySelection)
				{
					l_sVal.Append(wxT(";# "));
				}
				else
				{
					l_sVal.Append(wxT("; "));
				}
				l_sVal.Append(GetValue(i));
			}
		}
		//added by MD
		//now we need to write the collected multiples to l_sVal
		//but we should only do that if selection doesn't matter
		if (!m_bObeySelection)
		{
			wxString OldValue, NewValue;
			for (map<wxString, unsigned int>::iterator it =
					l_mMultipleValues.begin(); it != l_mMultipleValues.end(); ++it)
			{
				OldValue = (*it).first;
				if (l_sVal.BeforeFirst(';').Cmp(OldValue) == 0)
				{
					//here we found the value at the beginning...
					NewValue.Printf(wxT("(%d) %s"), (*it).second, OldValue.c_str());
					l_sVal.Replace(OldValue, NewValue, false);
				}
				else if (l_sVal.AfterLast(';').Cmp(wxT(" " + OldValue)) == 0)
				{
					//...here we found the value at the end...
					NewValue.Printf(wxT("; (%d) %s"), (*it).second, OldValue.c_str());
					l_sVal = l_sVal.BeforeLast(';');
					l_sVal.Append(NewValue);
				}
				else
				{
					//...here it must be somewhere in between.
					NewValue.Printf(wxT("; (%d) %s;"), (*it).second, OldValue.c_str());
					l_sVal.Replace(wxT("; ") + OldValue + wxT(";"), NewValue, false);
				}
			}
		}
		//---------------------------------------------------------------------------
	}
	l_mMultipleValues.clear();
	return l_sVal;
}

bool SP_DS_ListAttribute::SetValueString(const wxString& p_pchVal, bool p_bCheckValue)
{
	m_nNumOfValues = 0;
	wxString l_sInput = p_pchVal;
	wxString l_sVal;
	bool l_bResult= TRUE;
	long count;

	while (wxNOT_FOUND != l_sInput.Find(wxT(";")))
	{
		if ('#' == l_sInput.GetChar(0))
		{
			m_nChoosenValueIndex = m_nNumOfValues;
			l_sInput.Remove(0, 1);
		}
		l_sInput.Trim(FALSE);
		l_sVal = l_sInput.BeforeFirst(';');
		//added by MD to sum up multiple values
		//should only do this if selection doesn't matter
		if (!m_bObeySelection && l_sVal.Matches(wxT("(*)*")))
		{
			l_sVal.Remove(0, 1);
			if (l_sVal.BeforeFirst(')').ToLong(&count) && count >= 0)
			{
				//we get here if the characters in brackets represented an unsigned number
				l_sVal = l_sVal.AfterFirst(')');
				l_sVal.Trim();
				l_sVal.Trim(false);
			}
			else
			{
				//re-adding the removed char wxT("(" and resetting count to "1")
				l_sVal.Prepend(wxT("("));
				count = 1;
			}
		}
		else
		{
			count = 1;
		}
		for (int i = 0; i < count; i++)
		{
			l_bResult = l_bResult && AddValue(l_sVal);
		}
		//----------------------------------------------------------------
		l_sInput.Remove(0, l_sInput.Find(wxT(";")) + 1);
	}
	if (0 < l_sInput.size())
	{
		if ('#' == l_sInput.GetChar(0))
		{
			m_nChoosenValueIndex = m_nNumOfValues;
			l_sInput.Remove(0, 1);
		}
		l_sInput.Trim(FALSE);
		//added by MD to sum up multiple values
		//should only do this if selection doesn't matter
		if (!m_bObeySelection && l_sInput.Matches(wxT("(*)*")))
		{
			l_sInput.Remove(0, 1);
			if (l_sInput.BeforeFirst(')').ToLong(&count) && count >= 0)
			{
				//we get here if the characters in brackets represented an unsigned number
				l_sInput = l_sInput.AfterFirst(')');
				l_sInput.Trim();
				l_sInput.Trim(false);
			}
			else
			{
				//...found no number inside the brackets
				//therefore re-adding the removed char wxT("(" and resetting count to "1")
				l_sInput.Prepend(wxT("("));
				count = 1;
			}
		}
		else
		{
			count = 1;
		}
		for (int i = 0; i < count; i++)
		{
			l_bResult = l_bResult && AddValue(l_sInput);
		}
		//----------------------------------------------------------------
	}
	return l_bResult;
}

bool SP_DS_ListAttribute::Equals(SP_DS_Attribute *p_pcToCheck,
		bool p_bCheckValue, bool p_bCheckName, bool p_bCheckType)
{

	if (!p_pcToCheck)
		return false;

	if (this == p_pcToCheck)
		return true;

	if (p_bCheckName)
	{
		if (GetName() !=  p_pcToCheck->GetName())
			return false;
	}

	if (p_bCheckType)
	{
		if (m_eAttributeType != p_pcToCheck->GetAttributeType())
			return false;
		//more specific checking of list types
		if (m_eListType != (dynamic_cast<SP_DS_ListAttribute*>(p_pcToCheck))->GetAttributeListType())
			return false;
	}

	if (!p_bCheckValue)
		return true;

	return Contains(p_pcToCheck->GetValueString()) && (dynamic_cast<SP_DS_ListAttribute*>(p_pcToCheck))->Contains(GetValueString());
}

int SP_DS_ListAttribute::IndexOf(const wxString& p_sVal)
{

	//returns the first appearance of p_sVal
	for (int i = 0; i < m_nNumOfValues; i++)
	{
		if (m_pcValues[i]== p_sVal)
		{
			return i;
		}
	}
	return -1;
}

bool SP_DS_ListAttribute::Contains(const wxString& p_sVal)
{
	//normally the empty set is always contained, but this is not what we want to know here
	//so we return false
	if (p_sVal.IsNull())
		return true;
	//creating a temporary list-attribute for both (single- and list-value strings)
	//so we can benefit from its member functions
	SP_DS_ListAttribute *l_ToComp = new SP_DS_ListAttribute(wxT("TEMP"), p_sVal, false);

	int index;
	//the standard GetValueString() for comparison cannot be used here,
	//because the order of elements might differ. so we have to compare element by element.
	for (int i = 0; i < m_nNumOfValues; i++)
	{
		index = l_ToComp->IndexOf(m_pcValues[i]);
		if (index != -1)
		{
			l_ToComp->RemoveValue(index);
		}
		//stop if we've found everything
		if (l_ToComp->Size() == 0)
		{
			wxDELETE(l_ToComp);
			return true;
		}
	}
	wxDELETE(l_ToComp);
	return false;
}

