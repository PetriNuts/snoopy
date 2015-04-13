//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2005/08/11 15:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_DOUBLELISTATTRIBUTE_H__
#define __SP_DS_DOUBLELISTATTRIBUTE_H__

#include "sp_ds/attributes/SP_DS_ListAttribute.h"

class SP_DS_DoubleListAttribute: public SP_DS_ListAttribute
{
private:
protected:
public:
  SP_DS_DoubleListAttribute(const wxString& p_pchName, const wxString& p_nVal = wxT("0.0"));
  virtual ~SP_DS_DoubleListAttribute() { }

  bool AddValueDouble(long double p_nVal = 0.0);
  bool SetValueDouble(long double p_nVal);
  bool SetValueDouble(long double p_nVal, long p_nIndex);
  long double GetValueDouble();
  long double GetValueDouble(long p_nIndex);

  virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);
};


#endif // __SP_DS_DOUBLELISTATTRIBUTE_H__

