//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: ckruege2 $
// $Version: 0.0 $
// $Revision: 0 $
// $Date: 2009/06/19
// Short Description: parralellogram implementation
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_EXTENDEDPARALLELOGRAM_H__
#define __SP_GR_EXTENDEDPARALLELOGRAM_H__

#include "sp_gr/shapes/SP_GR_ExtendedDrawnShape.h"

class SP_GR_BaseDrawn;

class SP_GR_ExtendedParallelogram: public SP_GR_ExtendedDrawnShape
{
private:

protected:
	virtual bool Draw();
public:
  SP_GR_ExtendedParallelogram(SP_DS_Node* p_pcParent ,int p_nWidth = 40,int p_nHeight = 40,SP_EXTENDED_TYPE p_nType = SP_EXTENDED_TYPE_DEFAULT,size_t p_nThickness=1,wxColour p_pcColur= wxColour(0, 0, 0));
  virtual ~SP_GR_ExtendedParallelogram();

  virtual SP_Graphic* Clone(SP_Data* p_pcParent);
};

#endif // __SP_GR_EXTENDEDPARALLELOGRAM_H__
