//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2005/08/11 16:35:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_LISTATTRIBUTE_H__
#define __SP_DS_LISTATTRIBUTE_H__

#include "sp_ds/SP_DS_Attribute.h"

// Attribute List Types
enum SP_ATTRIBUTELIST_TYPE
{
    SP_ATTRIBUTELIST_TEXT,
    SP_ATTRIBUTELIST_BOOL,
    SP_ATTRIBUTELIST_LOGIC,
    // pointless, signed number
    SP_ATTRIBUTELIST_NUMBER,
    // pointless, signed number to be used as unique ID
    SP_ATTRIBUTELIST_ID,
    SP_ATTRIBUTELIST_MULTIPLICITY,
    // double float, signed number
    SP_ATTRIBUTELIST_DOUBLE,
    // equation pattern
    SP_ATTRIBUTELIST_EQUATIONPATTERN,
	// ftree pattern
    SP_ATTRIBUTELIST_FTREEPATTERN
};

class SP_DS_ListAttribute: public SP_DS_Attribute
{
private:
protected:
  SP_MapLong2String m_pcValues;
  long m_nNumOfValues;
  long m_nChoosenValueIndex;
  SP_ATTRIBUTELIST_TYPE m_eListType;
	bool m_bObeySelection;

public:
  SP_DS_ListAttribute(const wxString& p_pchName, const wxString& p_nVal = wxT(""), bool p_bObeySelection = true);
  virtual ~SP_DS_ListAttribute();

  bool RemoveValue(long p_nIndex = 0);
  bool AddValue(const wxString& p_nVal = wxT(""));
  bool SetValue(const wxString& p_nVal);
  bool SetValue(const wxString& p_nVal, long p_nIndex);
  wxString GetValue();
  wxString GetValue(long p_nIndex);
  long Size() { return m_nNumOfValues; }

  bool SetChoosenValueIndex(long p_nVal);
  long GetChoosenValueIndex();

  SP_ATTRIBUTELIST_TYPE GetAttributeListType() const { return m_eListType; }
  bool SetAttributeListType(SP_ATTRIBUTELIST_TYPE p_eType) { m_eListType = p_eType; return TRUE; }

  virtual wxString GetValueString();
  virtual bool SetValueString(const wxString& p_pchVal, bool p_bCheckValue = true);

  virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);

	//added by MD on 2006/07/19
	//IndexOf(...) returns index of first occurence
	int IndexOf(const wxString& p_sVal);
	bool Contains(const wxString& p_sVal);
	virtual bool Equals(SP_DS_Attribute *p_pcToCheck, bool p_bCheckValue = true, bool p_bCheckName = true, bool p_bCheckType = true);
};


#endif // __SP_DS_LISTATTRIBUTE_H__

