//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Attribute Event handler
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GRM_ATTRIBUTEHANDLER_H__
#define __SP_GRM_ATTRIBUTEHANDLER_H__

#include "sp_gr/eventhandler/SP_GRM_EventHandler.h"

class SP_GRM_AttributeHandler: public SP_GRM_EventHandler
{
private:
protected:
    void DoSnap(double* p_pnX, double* p_pnY, int p_nKeys = 0);
    
public:
    SP_GRM_AttributeHandler(wxShapeEvtHandler* p_pcPrev = NULL, wxShape* p_pcShape = NULL);

    void OnBeginDragLeft(double p_nX, double p_nY, int p_nKeys = 0, int p_nAttach = 0);
};

#endif // __SP_GRM_ATTRIBUTEHANDLER_H__

