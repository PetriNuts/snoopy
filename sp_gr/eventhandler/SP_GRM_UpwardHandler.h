//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Event handler that propagates all upwards to its parent
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GRM_UPWARDEHANDLER_H__
#define __SP_GRM_UPWARDEHANDLER_H__

#include "sp_gr/eventhandler/SP_GRM_EventHandler.h"

class SP_GRM_UpwardHandler: public SP_GRM_EventHandler
{
private:
protected:
public:
    SP_GRM_UpwardHandler(wxShapeEvtHandler* p_pcPrev = NULL, wxShape* p_pcShape = NULL);

    virtual void OnLeftClick(double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);
    virtual void OnLeftDoubleClick(double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);
    
    virtual void OnRightClick(double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);
    
    virtual void OnBeginDragLeft(double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);
    virtual void OnDragLeft(bool p_bDraw, double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);
    virtual void OnEndDragLeft(double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);

    virtual void OnBeginDragRight(double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);
    virtual void OnDragRight(bool p_bDraw, double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);
    virtual void OnEndDragRight(double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);

    virtual void OnSizingBeginDragLeft(wxControlPoint* p_pcCP, double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);
    virtual void OnSizingDragLeft(wxControlPoint* p_pcCP, bool p_bDraw, double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);
    virtual void OnSizingEndDragLeft(wxControlPoint* p_pcCP, double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);

    virtual void OnSize(double p_nX, double p_nY);

    virtual void OnDrawOutline(wxDC& p_cDC, double x, double y, double w, double h) { }
};

#endif // __SP_GRM_UPWARDEHANDLER_H__

