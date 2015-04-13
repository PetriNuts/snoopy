//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.2 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Rectangle Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_METARECTANGLE_H__
#define __SP_GR_METARECTANGLE_H__

#include "sp_gr/SP_GR_Metadata.h"

class SP_GR_BaseRectangle;

class SP_GR_MetaRectangle: public SP_GR_Metadata
{
private:
protected:
    double m_nRadius;
    SP_GR_BaseRectangle* m_pcPrimitive;
public:
    SP_GR_MetaRectangle(SP_DS_Metadata* p_pcParent,
		    double p_nW = 20.0,
		    double p_nH = 20.0,
		    double p_nR = 0.0);
    virtual ~SP_GR_MetaRectangle();

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);

    virtual double GetRadius();
    virtual bool SetRadius(double p_radius);

    virtual wxShape* GetPrimitive();

};

#endif // __SP_GR_METARECTANGLE_H__
