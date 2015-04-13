//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Drawn Shape Primitive Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_BASEBITMAP_H__
#define __SP_GR_BASEBITMAP_H__

#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/ogl/ogl.h>

#include "sp_core/base/SP_Error.h"

class SP_GR_BaseBitmap: public wxBitmapShape, public SP_Error
{
private:
protected:
    //! Dont react on hitt test nor draw the outlines
    bool m_bHideDimensions;
public:
    SP_GR_BaseBitmap();
    virtual ~SP_GR_BaseBitmap();

    virtual void Select(bool p_bSelect = TRUE, wxDC* p_pcDC = NULL);
    virtual void MakeControlPoints();
    virtual void DeleteControlPoints(wxDC *p_pcDC = NULL);
    virtual void ResetControlPoints();

    bool SetHideDimensions(bool p_bVal) { m_bHideDimensions = p_bVal; return TRUE; }
    bool GetHideDimensions() { return m_bHideDimensions; }

    //! override the OGL implementation to respect the 'show' flag
    void OnDrawOutline(wxDC& p_cDC, double p_nX, double p_nY, double p_nW, double p_nH);
    //! special, if we hide our dimensions
    bool HitTest(double p_nX, double p_nY, int* p_pnAttach, double* p_pnDistance);
};


#endif // __SP_GR_BASEBITMAP_H__

