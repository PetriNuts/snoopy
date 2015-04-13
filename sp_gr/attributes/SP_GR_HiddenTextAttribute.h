//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Text attribute Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_HIDDENTEXTATTRIBUTE_H__
#define __SP_GR_HIDDENTEXTATTRIBUTE_H__

#include "sp_gr/attributes/SP_GR_TextAttribute.h"

class SP_GR_BaseText;

class SP_GR_HiddenTextAttribute: public SP_GR_TextAttribute
{
private:
protected:
	virtual wxString FormatText();
public:
    SP_GR_HiddenTextAttribute(SP_DS_Attribute* p_pcParent,
        const wxString& p_pchFormat = wxT("%"),
        double p_nWidth = 0.0,
        double p_nHeight = 0.0);
    virtual ~SP_GR_HiddenTextAttribute();

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);
};

#endif // __SP_GR_HIDDENTEXTATTRIBUTE_H__
