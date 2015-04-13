//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: ak $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREENUMBERATTRIBUTE_H__
#define __SP_DS_FTREENUMBERATTRIBUTE_H__

#include "sp_ds/SP_DS_Attribute.h"

class SP_DS_FTreeNumberAttribute: public SP_DS_Attribute
{
private:
protected:
    long m_nValue;
public:
    SP_DS_FTreeNumberAttribute(const wxString& p_pchName, long p_nVal = 1);
    virtual ~SP_DS_FTreeNumberAttribute() { }

    virtual bool SetValue(long p_nVal) { m_nValue = p_nVal; return TRUE; }
    virtual long GetValue() { return m_nValue; }

    virtual wxString GetValueString();
    virtual bool SetValueString(const wxString& p_pchVal, bool p_bCheckValue = true);

    virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);
};


#endif // __SP_DS_FTreeNUMBERATTRIBUTE_H__

