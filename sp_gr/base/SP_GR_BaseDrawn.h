//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Drawn Shape Primitive Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_BASEDRAWN_H__
#define __SP_GR_BASEDRAWN_H__

#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/ogl/ogl.h>
#include <wx/ogl/drawn.h>

#include "sp_core/base/SP_Error.h"

class SP_GR_BaseDrawn: public wxDrawnShape, public SP_Error
{
private:
protected:
    wxPen* m_pcSavedPen;
    wxPen* m_pcLastSetPen;
    //! Dont react on hit test nor draw the outlines
    bool m_bHideDimensions;
public:
    SP_GR_BaseDrawn();
    virtual ~SP_GR_BaseDrawn();

    bool SetHideDimensions(bool p_bVal) { m_bHideDimensions = p_bVal; return TRUE; }
    bool GetHideDimensions() { return m_bHideDimensions; }

    virtual void Select(bool p_bSelect = TRUE, wxDC* p_pcDC = NULL);
    virtual void MakeControlPoints();
    virtual void DeleteControlPoints(wxDC *p_pcDC = NULL);
    virtual void ResetControlPoints();

    //! special, if we hide our dimensions
    bool HitTest(double p_nX, double p_nY, int* p_pnAttach, double* p_pnDistance);
    void GetBoundingBoxMin(double* p_pnWidth, double* p_pnHeight);

    bool CalculateDimensions(const wxString& p_sVal);

};


#endif // __SP_GR_BASEDRAWN_H__

