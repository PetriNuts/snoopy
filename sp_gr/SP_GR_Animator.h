//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_ANIMATOR_H__
#define __SP_GR_ANIMATOR_H__

#include "sp_core/base/SP_Graphic.h"
#include "sp_core/base/SP_Animator.h"
#include "sp_core/SP_Set.h"

class SP_GR_Animator: public SP_Graphic, public SP_Animator
{
private:
protected:
public:
    SP_GR_Animator();
    virtual ~SP_GR_Animator();

    virtual bool PreStep(unsigned int p_nSteps) = 0;
    virtual bool Step(unsigned int p_nStep) = 0;
    virtual bool PostStep() = 0;
};

#endif // __SP_GR_ANIMATOR_H__


