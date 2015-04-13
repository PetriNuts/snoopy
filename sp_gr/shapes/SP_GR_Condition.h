//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2006/05/03 09:30:00 $
// Short Description: Condition Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_CONDITION_H__
#define __SP_GR_CONDITION_H__

#include "sp_gr/shapes/SP_GR_DrawnShape.h"

class SP_GR_BaseDrawn;

class SP_GR_Condition: public SP_GR_DrawnShape
{
protected:
	virtual bool Draw();
public:
  SP_GR_Condition(SP_DS_Node* p_pcParent, double p_nWidth = 20.0, double p_nHeight = 20.0);
  virtual ~SP_GR_Condition();

  virtual SP_Graphic* Clone(SP_Data* p_pcParent);
};

#endif // __SP_GR_CONDITION_H__


