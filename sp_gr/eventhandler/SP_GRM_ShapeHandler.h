//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Shape handler header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GRM_SHAPEHANDLER_H__
#define __SP_GRM_SHAPEHANDLER_H__

#include "sp_gr/eventhandler/SP_GRM_EventHandler.h"

class SP_GRM_ShapeHandler: public SP_GRM_EventHandler
{
private:
protected:
public:
    SP_GRM_ShapeHandler(wxShapeEvtHandler* p_pcPrev = NULL, wxShape* p_pcShape = NULL);

    void OnBeginDragLeft(double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);
    void OnDragLeft(bool p_bDraw, double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);
    void OnEndDragLeft(double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);

    void OnEndSize(double p_nX, double p_nY);
};

#endif // __SP_GRM_SHAPEHANDLER_H__

