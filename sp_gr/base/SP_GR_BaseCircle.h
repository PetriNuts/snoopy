//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Circle Primitive Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_BASECIRCLE_H__
#define __SP_GR_BASECIRCLE_H__

#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/ogl/ogl.h>

#include "sp_core/base/SP_Error.h"

class SP_GR_BaseCircle: public wxCircleShape, public SP_Error
{
private:
protected:
    wxPen* m_pcSavedPen;
    wxPen* m_pcLastSetPen;
public:
    SP_GR_BaseCircle(double p_nR);
    virtual ~SP_GR_BaseCircle();

    virtual void Select(bool p_bSelect = TRUE, wxDC* p_pcDC = NULL);
    virtual void MakeControlPoints();
    virtual void DeleteControlPoints(wxDC *p_pcDC = NULL);
    virtual void ResetControlPoints();

    virtual void OnEndSize(double p_nX, double p_nY);
};


#endif // __SP_GR_BASECIRCLE_H__
