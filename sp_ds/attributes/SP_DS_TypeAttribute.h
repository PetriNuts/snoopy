/*
 * SP_DS_TypeAttribute.h
 *
 *  Created on: 16.10.2012
 *      Author: Steffen Laarz
 */

#ifndef SP_DS_TYPEATTRIBUTE_H_
#define SP_DS_TYPEATTRIBUTE_H_

#include "sp_ds/attributes/SP_DS_TextAttribute.h"

class SP_DS_TypeAttribute: public SP_DS_TextAttribute
{
private:
protected:
	SP_SetString m_sTypes;
public:
    SP_DS_TypeAttribute(const wxString& p_pchName, const wxString& p_sType);
    virtual ~SP_DS_TypeAttribute() { }

    void AddPossibleValue(const wxString& p_pchVal);
    const SP_SetString& GetPossibleValues() { return m_sTypes;}

    virtual bool SetValueString(const wxString& p_pchVal, bool p_bCheckValue = true);

    virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);
};


#endif /* SP_DS_TYPEATTRIBUTE_H_ */
