//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Graphical Attribute-Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_ATTRIBUTE_H__
#define __SP_GR_ATTRIBUTE_H__

#include "sp_core/base/SP_Error.h"
#include "sp_core/base/SP_Type.h"
#include "sp_core/base/SP_Graphic.h"
#include "sp_ds/SP_DS_Attribute.h"

class SP_GR_Attribute: public SP_Error, public SP_Graphic
{
private:
protected:
    double m_nOffsetX;
    double m_nOffsetY;
	
public:
    SP_GR_Attribute(SP_DS_Attribute* p_pcDataParent);
    virtual ~SP_GR_Attribute();

    virtual wxShape* GetPrimitive() = 0;

    inline bool SetOffsetX(double p_nVal) { m_nOffsetX = p_nVal; return TRUE; }
    inline bool SetOffsetY(double p_nVal) { m_nOffsetY = p_nVal; return TRUE; }

    inline double GetOffsetX() { return m_nOffsetX; }
    inline double GetOffsetY() { return m_nOffsetY; }

	
    virtual bool SetWidth(double p_nVal);
    virtual double GetWidth();
    virtual bool SetHeight(double p_nVal);
    virtual double GetHeight();

    virtual bool Update(bool p_bLocalOnly = FALSE) = 0;

    virtual bool OnDoubleClick(int p_nKeys);

    virtual SP_Graphic* Clone(SP_Data* p_pcParent) = 0;
    //! for call from within derived_classes::Clone method
    virtual bool CloneBase(SP_Graphic* p_pcCopy);

    virtual bool OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet, SP_ListGraphic* p_plGraphics = NULL);
};

#endif // __SP_GR_ATTRIBUTE_H__
