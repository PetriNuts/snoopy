//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/07/22 10:20:00 $
// Short Description: Double Drawn Circle Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_PLOTSYMBOL_H__
#define __SP_GR_PLOTSYMBOL_H__

#include "sp_gr/metadata/SP_GR_MetaDrawnShape.h"

class SP_GR_PlotSymbol: public SP_GR_MetaDrawnShape
{
protected:
	virtual bool Draw();
public:
  SP_GR_PlotSymbol(SP_DS_Metadata* p_pcParent);
  virtual ~SP_GR_PlotSymbol();

  virtual SP_Graphic* Clone(SP_Data* p_pcParent);
};

#endif // __SP_GR_PLOTSYMBOL_H__


