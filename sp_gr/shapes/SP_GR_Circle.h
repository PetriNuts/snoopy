//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Circle Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_CIRCLE_H__
#define __SP_GR_CIRCLE_H__

#include "sp_gr/SP_GR_Node.h"

class SP_GR_BaseCircle;

class SP_GR_Circle: public SP_GR_Node
{
private:
protected:
    SP_GR_BaseCircle* m_pcPrimitive;
public:
    SP_GR_Circle(SP_DS_Node* p_pcParent, double p_nR = 20.0);
    virtual ~SP_GR_Circle();

    virtual bool SetRadius(double p_nVal);
    virtual double GetRadius();

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);

    virtual wxShape* GetPrimitive();
};

#endif // __SP_GR_CIRCLE_H__
