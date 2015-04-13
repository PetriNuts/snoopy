//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_IDATTRIBUTE_H__
#define __SP_DS_IDATTRIBUTE_H__

#include "sp_ds/attributes/SP_DS_NumberAttribute.h"

#define NOT_SET_YET -2

class SP_DS_IdAttribute: public SP_DS_NumberAttribute
{
private:
protected:
public:
    SP_DS_IdAttribute(const wxString& p_pchName);
    virtual ~SP_DS_IdAttribute() { }

    virtual long GetValue();
    virtual bool SetValue(long p_nVal);

    virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);

    // renumbering takes place
    bool Squeeze();
};


#endif // __SP_DS_IDATTRIBUTE_H__

