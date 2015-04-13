//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Attribute Event handler
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GRM_EVENTHANDLER_H__
#define __SP_GRM_EVENTHANDLER_H__

#include "sp_gui/windows/SP_GUI_Canvas.h"

class SP_GRM_EventHandler: public wxShapeEvtHandler
{
private:
	double g_nDragOffsetX = 0.0;
	double g_nDragOffsetY = 0.0;
	double g_nDragStartX = 0.0;
	double g_nDragStartY = 0.0;

protected:

    virtual bool CheckSensitivity(int p_nOperation);

    virtual void DoSnap(double* p_pnX, double* p_pnY, int p_nKeys = 0);

    virtual SP_GUI_Canvas* UnSelectAll(int p_nKeys);

public:
    SP_GRM_EventHandler(wxShapeEvtHandler* p_pcPrev = NULL, wxShape* p_pcShape = NULL);

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
};

#endif // __SP_GRM_EVENTHANDLER_H__

