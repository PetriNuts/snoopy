//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dbayer $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2008/07/08 10:20:00 $
// Short Description: symbol of music transition
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_MusicTransition_H__
#define __SP_GR_MusicTransition_H__

#include "sp_gr/shapes/SP_GR_DrawnShape.h"

class SP_GR_BaseDrawn;

class SP_GR_MusicTransition: public SP_GR_DrawnShape
{
protected:
	virtual bool Draw();
public:
  SP_GR_MusicTransition(SP_DS_Node* p_pcParent);
  virtual ~SP_GR_MusicTransition();

  virtual SP_Graphic* Clone(SP_Data* p_pcParent);
};

#endif // __SP_GR_MusicTransition_H__


