//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Edge handler header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GRM_EDGEHANDLER_H__
#define __SP_GRM_EDGEHANDLER_H__

#include "sp_gr/eventhandler/SP_GRM_EventHandler.h"

#include <wx/ogl/basicp.h>
#include <wx/ogl/linesp.h>

class SP_GRM_EdgeHandler: public SP_GRM_EventHandler
{
private:
protected:
public:
    SP_GRM_EdgeHandler(wxShapeEvtHandler* p_pcPrev = NULL, wxShape* p_pcShape = NULL);

    void OnLeftClick(double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);

    //! special to edges
    void OnSizingBeginDragLeft(wxControlPoint* p_pcCP, 
        double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);
    void OnSizingDragLeft(wxControlPoint* p_pcCP, bool p_bDraw,
        double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);
    void OnSizingEndDragLeft(wxControlPoint* p_pcCP, 
        double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);
    
    bool OnMoveMiddleControlPoint(wxLineControlPoint* p_pcCP, double p_nX, double p_nY);
    bool OnMoveLineEndPoint(wxLineControlPoint* p_pcCP, double p_nX, double p_nY);
};

#endif // __SP_GRM_EDGEHANDLER_H__

