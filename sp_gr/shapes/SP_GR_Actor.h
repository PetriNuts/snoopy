//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: ckruege2 $
// $Version: 0.0 $
// $Revision: 0 $
// $Date: 2009/07/21
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_ACTOR_H__
#define __SP_GR_ACTOR_H__

#include "sp_gr/shapes/SP_GR_DrawnShape.h"

class SP_GR_BaseDrawn;

class SP_GR_Actor: public SP_GR_DrawnShape
{
private:
protected:
	virtual bool Draw();
public:
  SP_GR_Actor(SP_DS_Node* p_pcParent);
  virtual ~SP_GR_Actor();

  virtual SP_Graphic* Clone(SP_Data* p_pcParent);
};

#endif // __SP_GR_ACTOR_H__
