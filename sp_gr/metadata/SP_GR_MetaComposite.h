//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr	 $
// $Version: 0.2 $
// $Date: 2015/01/12 11:55:00 $
// Short Description: Composite Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_METACOMPOSITE_H__
#define __SP_GR_METACOMPOSITE_H__

#include "sp_gr/SP_GR_Metadata.h"
#include "sp_core/SP_Vector.h"
#include <memory>

class SP_GR_BaseRectangle;
using SP_GR_BaseRectangle_Ptr = std::unique_ptr<SP_GR_BaseRectangle>;
using SP_VectorGraphics = std::vector<SP_Graphic*>;

class SP_GR_Attribute;

class SP_GR_MetaComposite: public SP_GR_Metadata
{
private:
protected:
    double m_nRadius;
    SP_GR_BaseRectangle_Ptr m_pcPrimitive;
    SP_VectorGraphics m_pcGraphics;

public:
    SP_GR_MetaComposite(SP_DS_Metadata* p_pcParent,
		    double p_nW = 20.0,
		    double p_nH = 20.0,
		    double p_nR = 0.0);
    virtual ~SP_GR_MetaComposite();

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);

    virtual double GetRadius() const;
    virtual bool SetRadius(double p_radius);

    virtual wxShape* GetPrimitive();
    virtual bool Update(bool p_bLocalOnly = false);

};

#endif // __SP_GR_METARECTANGLE_H__
