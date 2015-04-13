//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_ANIMATOR_H__
#define __SP_DS_ANIMATOR_H__

enum SP_DS_ANIMATOR_TYPE
{
    SP_DS_ANIMATOR_NULL,
    SP_DS_ANIMATOR_PLACE,
    SP_DS_ANIMATOR_TRANS,
/* changed by dscheibl 24.08.2005 */
    SP_DS_ANIMATOR_PARAM,
/* changed by dscheibl 12.09.2005 */
    SP_DS_ANIMATOR_PLOT,
/* changing end */
/* changed by akurth 15.06.2006 */
    SP_DS_ANIMATOR_EVENT,
	SP_DS_ANIMATOR_TOP_EVENT,
/* changed by akurth 15.06.2006 */
    SP_DS_ANIMATOR_GATE,
/* changing end */
    SP_DS_ANIMATOR_COARSE_TRANS,
    SP_DS_ANIMATOR_COARSE_PLACE,
/* changed by dbayer 20.06.2008 */
	SP_DS_ANIMATOR_MUSIC_TRANS
};

#include "sp_core/base/SP_Animator.h"
#include "sp_core/base/SP_Data.h"

class SP_DS_Animation;

class SP_DS_Animator: public SP_Data, public SP_Animator
{
private:
protected:
    SP_DS_ANIMATOR_TYPE m_eAnimatorType;

    SP_Data* m_pcParent;
    SP_DS_Animation* m_pcAnimation;

    unsigned int m_nSteps;
    bool m_bEnabled;
public:
    SP_DS_Animator(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim);
    virtual ~SP_DS_Animator();

    virtual bool PreStep(unsigned int p_nSteps, SP_DIRECTION_T p_nDir = FORWARD) = 0;
    virtual bool Step(unsigned int p_nStep) = 0;
    virtual bool PostStep() = 0;

    virtual SP_DS_Animator* Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim) = 0;

    // SP_Data 'must have'
    bool Remove() { return TRUE; }
    const wxString GetClassName() { return wxT(""); }
    SP_Data* GetAnimObject() const { return m_pcParent; }

    inline void SetEnabled(bool p_bVal) { m_bEnabled = p_bVal; }
    inline bool GetEnabled() const { return m_bEnabled; }
    
    inline void SetAnimatorType(SP_DS_ANIMATOR_TYPE p_eVal) { m_eAnimatorType = p_eVal; }
    inline SP_DS_ANIMATOR_TYPE GetAnimatorType() const { return m_eAnimatorType; }
};

#endif // __SP_DS_ANIMATOR_H__


