//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_STPLACEANIMATOR_H__
#define __SP_DS_STPLACEANIMATOR_H__

#include "sp_ds/animators/SP_DS_ExtPTPlaceAnimator.h"

class SP_DS_StPlaceAnimator: public SP_DS_ExtPTPlaceAnimator
{
private:
protected:
public:

    SP_DS_StPlaceAnimator(const wxString& p_pchAttributeName, SP_DS_Animation* p_pcAnim = NULL, SP_DS_Node* p_pcParent = NULL);
    virtual ~SP_DS_StPlaceAnimator();

    virtual SP_DS_Animator* Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim);

    virtual bool ResetMarking();

};

#endif // __SP_DS_PLACEANIMATOR_H__


