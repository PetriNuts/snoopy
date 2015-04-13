//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Edge Primitive Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_BASEEDGE_H__
#define __SP_GR_BASEEDGE_H__

#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/ogl/ogl.h>

#include "sp_core/base/SP_Error.h"

class SP_GR_BaseEdge: public SP_Error, public wxLineShape
{
private:
    DECLARE_DYNAMIC_CLASS(SP_GR_BaseEdge)
protected:
    wxPen* m_pcSavedPen;
public:
    SP_GR_BaseEdge();
    virtual ~SP_GR_BaseEdge();

    virtual void Select(bool p_bSelect = TRUE, wxDC* p_pcDC = NULL);

    virtual bool InsertControlPoint(double p_nX, double p_nY);

    void GetBoundingBoxMin(double* p_pnWidth, double* p_pnHeight);
};


#endif // __SP_GR_BASEEDGE_H__
