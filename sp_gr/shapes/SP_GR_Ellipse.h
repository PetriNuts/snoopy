//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Ellipse Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_ELLIPSE_H__
#define __SP_GR_ELLIPSE_H__

#include "sp_gr/SP_GR_Node.h"

class SP_GR_BaseEllipse;

class SP_GR_Ellipse: public SP_GR_Node
{
private:
protected:
    SP_GR_BaseEllipse* m_pcPrimitive;
public:
    SP_GR_Ellipse(SP_DS_Node* p_pcParent, double p_nW = 20.0, double p_nH = 20.0);
    virtual ~SP_GR_Ellipse();

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);

    virtual wxShape* GetPrimitive();
};

#endif // __SP_GR_ELLIPSE_H__
