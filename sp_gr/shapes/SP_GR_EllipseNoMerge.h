//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Ellipse Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_ELLIPSENOMERGE_H__
#define __SP_GR_ELLIPSENOMERGE_H__

#include "sp_gr/shapes/SP_GR_Ellipse.h"

class SP_GR_EllipseNoMerge: public SP_GR_Ellipse
{
private:
protected:
public:
    SP_GR_EllipseNoMerge(SP_DS_Node* p_pcParent, double p_nW = 20.0, double p_nH = 20.0);
    virtual ~SP_GR_EllipseNoMerge();

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);

	virtual  bool Merge(SP_Graphic* p_pcTarget);
};

#endif // __SP_GR_ELLIPSENOMERGE_H__
