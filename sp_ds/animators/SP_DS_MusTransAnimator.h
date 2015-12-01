//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dbayer $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2008/09/01 11:55:00 $
// Short Description: Animation class for transitions of music petri net that
//					  play a sound during stepping
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_MUSTRANSANIMATOR_H__
#define __SP_DS_MUSTRANSANIMATOR_H__

#include "SP_DS_TransAnimator.h"
#include <irrKlang.h>

class SP_DS_MusTransAnimator: public SP_DS_TransAnimator
{
private:
	bool m_bFirstStep;

protected:

	bool PlaySound();

public:

    SP_DS_MusTransAnimator(SP_DS_Animation* p_pcAnim = NULL, SP_DS_Node* p_pcParent = NULL);
    virtual ~SP_DS_MusTransAnimator();

    bool Step(unsigned int p_nStep);
    bool PostStep();

    virtual SP_DS_Animator* Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim);

    bool OnLeftClick();
	bool OnRightClick();
};

#endif // __SP_DS_MUSTRANSANIMATOR_H__


