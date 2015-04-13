/*
 * SP_DS_ExtPTTransAnimator.h
 *
 *  Created on: 31.08.2012
 *      Author: dell-notebook
 */

#ifndef SP_DS_EXTPTTRANSANIMATOR_H_
#define SP_DS_EXTPTTRANSANIMATOR_H_

#include "SP_DS_TransAnimator.h"



class SP_DS_ExtPTTransAnimator: public SP_DS_TransAnimator
{
private:

protected:


public:

    SP_DS_ExtPTTransAnimator(SP_DS_Animation* p_pcAnim = NULL, SP_DS_Node* p_pcParent = NULL);
    virtual ~SP_DS_ExtPTTransAnimator();

    bool PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir);
    bool Step(unsigned int p_nStep);
    bool PostStep();

    virtual SP_DS_Animator* Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim);

    bool OnLeftClick();
	bool OnRightClick();

    virtual void CalculateMultiplicity();

};

#endif /* SP_DS_EXTPTTRANSANIMATOR_H_ */
