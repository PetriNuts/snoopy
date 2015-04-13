//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.2 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Rectangle Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_RECTANGLE_H__
#define __SP_GR_RECTANGLE_H__

#include "sp_gr/SP_GR_Node.h"

class SP_GR_BaseRectangle;

class SP_GR_Rectangle: public SP_GR_Node
{
private:
protected:
    double m_nRadius;
    SP_GR_BaseRectangle* m_pcPrimitive;
public:
    SP_GR_Rectangle(SP_DS_Node* p_pcParent,
		    double p_nW = 20.0,
		    double p_nH = 20.0,
		    double p_nR = 0.0);
    virtual ~SP_GR_Rectangle();

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);

    virtual double GetRadius();
    virtual bool SetRadius(double p_radius);

    virtual wxShape* GetPrimitive();
	virtual  bool Merge(SP_Graphic* p_pcTarget);

};

#endif // __SP_GR_RECTANGLE_H__
