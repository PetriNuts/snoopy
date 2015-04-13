//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/01 14:50:00 $
// Short Description: Multiplicity graphic attribute Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_DOUBLEMULTIPLICITYATTRIBUTE_H__
#define __SP_GR_DOUBLEMULTIPLICITYATTRIBUTE_H__

#include "sp_gr/attributes/SP_GR_DoubleAttribute.h"

class SP_GR_DoubleMultiplicityAttribute: public SP_GR_DoubleAttribute
{
private:
protected:
	virtual wxString FormatText();
public:
    SP_GR_DoubleMultiplicityAttribute(SP_DS_Attribute* p_pcParent,
        const wxString& p_pchFormat = wxT("%"),
        double p_nWidth = 0.0,
        double p_nHeight = 0.0);

    virtual bool Update(bool p_bLocalOnly = FALSE);
    virtual SP_Graphic* Clone(SP_Data* p_pcParent);
};

#endif // __SP_GR_DOUBLEMULTIPLICITYATTRIBUTE_H__
