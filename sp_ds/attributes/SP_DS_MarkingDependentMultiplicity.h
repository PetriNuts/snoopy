//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: Steffen Laarz $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2012/07/31 12:37:00 $
// Short Description: to handle expression and constants for edges
//////////////////////////////////////////////////////////////////////
#ifndef SP_DS_MARKINGDEPENDENTMULTIPLICITY_H_
#define SP_DS_MARKINGDEPENDENTMULTIPLICITY_H_

#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"

class SP_DS_MarkingDependentMultiplicity: public SP_DS_MultiplicityAttribute
{
private:

protected:

    virtual bool UpdateValue();

public:

    SP_DS_MarkingDependentMultiplicity(const wxString& p_pchName, const wxString& p_pchVal);
    SP_DS_MarkingDependentMultiplicity(const wxString& p_pchName, long p_nVal = 1);
    virtual ~SP_DS_MarkingDependentMultiplicity() { }

    virtual SP_DS_Attribute* Clone(bool p_bCloneGr = TRUE);

	virtual bool CheckIntegrity();

};


#endif /* SP_DS_MARKINGDEPENDENTMULTIPLICITY_H_ */
