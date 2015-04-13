//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Multiplicity graphic attribute Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_MULTIPLICITYATTRIBUTE_H__
#define __SP_GR_MULTIPLICITYATTRIBUTE_H__

#include "sp_gr/attributes/SP_GR_NumberAttribute.h"

class SP_GR_MultiplicityAttribute: public SP_GR_NumberAttribute
{
private:
protected:
	virtual wxString FormatText();
public:
    SP_GR_MultiplicityAttribute(SP_DS_Attribute* p_pcParent,
        const wxString& p_pchFormat = wxT("%"),
        double p_nWidth = 0.0,
        double p_nHeight = 0.0);


   virtual SP_Graphic* Clone(SP_Data* p_pcParent);
};

#endif // __SP_GR_MultiplicityAttribute_H__
