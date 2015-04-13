//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: Steffen Laarz $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2012/07/31 12:37:00 $
// Short Description: to handle expression and constants for cpn edges
//////////////////////////////////////////////////////////////////////
#ifndef SP_DS_DOUBLEMARKINGDEPENDENTMULTIPLICITY_H_
#define SP_DS_DOUBLEMARKINGDEPENDENTMULTIPLICITY_H_

#include "sp_ds/attributes/SP_DS_ExpressionAttribute.h"


class SP_DS_DoubleMarkingDependentMultiplicity: public SP_DS_ExpressionAttribute
{
private:
	double m_nValue;					//for numeric value in animation

protected:

    virtual bool UpdateValue();

public:

	SP_DS_DoubleMarkingDependentMultiplicity(const wxString& p_pchName, const wxString& p_pchVal);
	SP_DS_DoubleMarkingDependentMultiplicity(const wxString& p_pchName, double p_nVal = 1);
    virtual ~SP_DS_DoubleMarkingDependentMultiplicity() { }

    virtual bool SetValue(double l_nValue);
    virtual double GetValue(bool p_bUpdate = false);

    virtual wxString GetValueString();
    virtual bool SetValueString(const wxString& p_pchVal, bool p_bCheckValue = true);

    virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);

	virtual bool CheckIntegrity();

};


#endif /* SP_DS_DOUBLEMARKINGDEPENDENTMULTIPLICITY_H_ */

