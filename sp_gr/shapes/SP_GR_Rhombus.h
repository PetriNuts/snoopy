//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2006/04/29 13:42:00 $
// Short Description: Rhombus Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_RHOMBUS_H__
#define __SP_GR_RHOMBUS_H__

#include "sp_gr/shapes/SP_GR_DrawnShape.h"

class SP_GR_BaseDrawn;

class SP_GR_Rhombus: public SP_GR_DrawnShape
{
protected:
	virtual bool Draw();
public:
  SP_GR_Rhombus(SP_DS_Node* p_pcParent, double p_nWidth = 20.0,
			double p_nHeight = 20.0);
  virtual ~SP_GR_Rhombus();

  virtual SP_Graphic* Clone(SP_Data* p_pcParent);

  virtual  bool Merge(SP_Graphic* p_pcTarget);
};

#endif // __SP_GR_RHOMBUS_H__


