//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ANIMATOR_H__
#define __SP_ANIMATOR_H__

enum SP_ANIM_TYPE
{
    SP_ANIM_NULL,
    SP_ANIM_DS,
    SP_ANIM_GR,
    SP_ANIM_GUI
};

#include "sp_core/SP_List.h"
#include "sp_core/base/SP_Error.h"
//#include "snoopy.h"

class SP_Animator: public SP_Error
{
private:
    SP_ANIM_TYPE m_eType;
protected:
public:
    SP_Animator(SP_ANIM_TYPE p_eType);
    virtual ~SP_Animator();

    inline SP_ANIM_TYPE GetType() const { return m_eType; }

    virtual bool OnLeftClick() { return TRUE; }
    virtual bool OnRightClick() { return TRUE; }
};

#endif // __SP_ANIMATOR_H__


