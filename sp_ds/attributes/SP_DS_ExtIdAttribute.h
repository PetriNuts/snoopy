//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/03/24 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_EXTIDATTRIBUTE_H__
#define __SP_DS_EXTIDATTRIBUTE_H__

#include "sp_ds/attributes/SP_DS_NumberAttribute.h"

#define NOT_SET_YET 0

class SP_DS_ExtIdAttribute: public SP_DS_NumberAttribute
{
private:
protected:
public:
    SP_DS_ExtIdAttribute(const wxString& p_pchName);
    virtual ~SP_DS_ExtIdAttribute() { }

    virtual long GetValue();
    virtual bool SetValue(long p_nVal);
	virtual bool ExistID(long p_nVal);

    virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);

    // renumbering takes place
    bool Squeeze();
};


#endif // __SP_DS_EXTIDATTRIBUTE_H__

