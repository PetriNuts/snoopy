//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/01/30 10:34:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREELOGICATTRIBUTE_H__
#define __SP_DS_FTREELOGICATTRIBUTE_H__

class SP_DS_FTreeLogicAttribute: public SP_DS_LogicAttribute
{
private:
protected:
	bool checkStatusFTree(bool p_bVal);
public:
    SP_DS_FTreeLogicAttribute(const wxString& p_pchName, const wxString& p_pchRef, bool p_bVal = FALSE);
    virtual ~SP_DS_FTreeLogicAttribute() { }

    virtual bool SetValue(bool p_bVal);

    virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);

};


#endif // __SP_DS_FTREELOGICATTRIBUTE_H__
