//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Text attribute Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_COLLISTATTRIBUTE_H__
#define __SP_GR_COLLISTATTRIBUTE_H__

#include "sp_gr/SP_GR_Attribute.h"

class SP_GR_BaseText;

class SP_GR_ColListAttribute: public SP_GR_Attribute
{
private:
protected:
    SP_GR_BaseText* m_pcPrimitive;
    wxString m_sFormat;
/* changed by dscheibl 23.06.2005 */
    virtual wxString FormatText();

	wxString m_sPropertyName;
	wxString m_sNodeClassName;
	wxString m_sNetClassName;
/* changing end */
public:
    SP_GR_ColListAttribute(SP_DS_Attribute* p_pcParent,
        const wxString& p_pchFormat = wxT("%"),
        double p_nWidth = 0.0,
        double p_nHeight = 0.0);
    virtual ~SP_GR_ColListAttribute();

    virtual bool Update(bool p_bLocalOnly = FALSE);

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);

    virtual wxShape* GetPrimitive();
};

#endif // __SP_GR_COLLISTATTRIBUTE_H__
