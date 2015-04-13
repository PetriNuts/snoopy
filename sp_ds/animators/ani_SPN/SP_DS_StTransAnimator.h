//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_STTRANSANIMATOR_H__
#define __SP_DS_STTRANSANIMATOR_H__

#include "sp_ds/animators/SP_DS_TransAnimator.h"

class SP_DS_StTransAnimator: public SP_DS_TransAnimator
{
private:
protected:
public:

    SP_DS_StTransAnimator(SP_DS_Animation* p_pcAnim = NULL, SP_DS_Node* p_pcParent = NULL);
    virtual ~SP_DS_StTransAnimator();

    virtual SP_DS_Animator* Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim);

};

#endif // __SP_DS_TRANSANIMATOR_H__


