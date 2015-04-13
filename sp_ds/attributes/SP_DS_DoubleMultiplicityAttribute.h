//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/07 13:05:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_DOUBLEMULTIPLICITYATTRIBUTE_H__
#define __SP_DS_DOUBLEMULTIPLICITYATTRIBUTE_H__

#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"

class SP_DS_DoubleMultiplicityAttribute: public SP_DS_DoubleAttribute
{
private:
protected:
public:
    SP_DS_DoubleMultiplicityAttribute(const wxString& p_pchName);
    virtual ~SP_DS_DoubleMultiplicityAttribute() { }

    virtual bool SetValue(long double p_nVal);

    virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);
};


#endif // __SP_DS_DOUBLEMULTIPLICITYATTRIBUTE_H__

