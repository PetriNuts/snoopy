//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_MARKANIMATOR_H__
#define __SP_GR_MARKANIMATOR_H__

#include "sp_gr/SP_GR_Animator.h"
#include "sp_gr/base/SP_GR_BaseCircle.h"

class SP_GR_Edge;

class SP_GR_MarkAnimator: public SP_GR_Animator
{
private:
protected:
    SP_GR_BaseCircle* m_pcPrimitive;

    list<wxRealPoint*> m_lWaypoints;
    list<wxRealPoint*>::iterator m_itPath;
    bool CalculatePath(SP_GR_Edge* p_pcEdge, unsigned int p_nSteps, SP_DIRECTION_T p_nDir);
    void ClearPath();

    SP_Graphic* m_pcGrParent;
public:
    SP_GR_MarkAnimator();
    virtual ~SP_GR_MarkAnimator();

    // individuals
    bool SetPath(SP_Graphic* p_pcPath, unsigned int p_nSteps, SP_DIRECTION_T p_nDir);

    // 'must have'
    virtual bool PreStep(unsigned int p_nSteps);
    virtual bool Step(unsigned int p_nStep);
    virtual bool PostStep();

    virtual SP_Graphic* Clone(SP_Data* p_pcParent);
    bool AddToCanvas(SP_GUI_Canvas* p_pcCanvas, double p_nX = -1, double p_nY = -1, int p_nKeys = 0);
    SP_GUI_Canvas* GetCanvas();

    virtual inline wxShape* GetPrimitive() { return m_pcPrimitive; }
};

#endif // __SP_GR_MARKANIMATOR_H__


