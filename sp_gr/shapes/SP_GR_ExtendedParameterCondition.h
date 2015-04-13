//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: ckruege2 $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/07/28 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_PARAMETERCONDITION_H__
#define __SP_GR_PARAMETERCONDITION_H__

#include "sp_gr/shapes/SP_GR_ExtendedDrawnShape.h"

class SP_GR_BaseDrawn;

class SP_GR_ExtendedParameterCondition: public SP_GR_ExtendedDrawnShape
{
private:
protected:
	virtual bool Draw();
public:
  
  SP_GR_ExtendedParameterCondition(SP_DS_Node* p_pcParent,int p_bWidth = 40,
	  SP_EXTENDED_TYPE p_nType = SP_EXTENDED_TYPE_DEFAULT, size_t p_nThickness=1,wxColour p_pcColur= wxColour(0, 0, 0));
  virtual ~SP_GR_ExtendedParameterCondition();

  virtual SP_Graphic* Clone(SP_Data* p_pcParent);

  virtual  bool Merge(SP_Graphic* p_pcTarget);
};

#endif // __SP_GR_PARAMETERCONDITION_H__


