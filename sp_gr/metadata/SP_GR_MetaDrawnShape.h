//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Drawn Shape Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_METADRAWNSHAPE_H__
#define __SP_GR_METADRAWNSHAPE_H__

#include "sp_gr/SP_GR_Metadata.h"
#include "sp_gr/base/SP_GR_BaseDrawn.h"

class SP_GR_MetaDrawnShape: public SP_GR_Metadata
{
private:
protected:
    SP_GR_BaseDrawn* m_pcPrimitive;

    virtual bool Draw();
public:
    SP_GR_MetaDrawnShape(SP_DS_Metadata* p_pcParent);
    virtual ~SP_GR_MetaDrawnShape();

    virtual SP_Graphic* Clone(SP_Data* p_pcParent) = 0;

    virtual bool Update(bool p_bLocalOnly = FALSE);
    virtual wxShape* GetPrimitive();
};

#endif // __SP_GR_METADRAWNSHAPE_H__


