//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_LOGICATTRIBUTE_H__
#define __SP_DS_LOGICATTRIBUTE_H__

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"

class SP_DS_LogicAttribute: public SP_DS_BoolAttribute
{
private:
protected:
    wxString m_sReferenceAttribute;
    bool m_bOneShot;

    bool OnChangeState();
    bool SplitElements();

public:
    SP_DS_LogicAttribute(const wxString& p_pchName, const wxString& p_pchRef, bool p_bVal = FALSE);
    virtual ~SP_DS_LogicAttribute() { }

    virtual bool SetValue(bool p_bVal);

    inline const wxString GetReferenceAttribute() { return m_sReferenceAttribute; }
    void SetOneShot(bool p_bVal = TRUE) { m_bOneShot = p_bVal; }

    virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);

    //! get the values right. this is called from the base class, if a split happened
    bool OnSplit(SP_DS_Attribute* p_pcAttribute);
    //! unfortunately need to have this public to be able to access it from SP_DS_Node
    bool JoinElements();
};


#endif // __SP_DS_LOGICATTRIBUTE_H__
