//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Text Primitive Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_BASETEXT_H__
#define __SP_GR_BASETEXT_H__

#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/ogl/ogl.h>

class SP_GR_BaseText: public wxTextShape
{
private:
protected:
    //! Dont react on hitt test nor draw the outlines
    bool m_bHideDimensions;
    wxFont *m_selectFont;
    wxFont *m_normalFont;	
public:
    SP_GR_BaseText(const wxString& p_sText, double p_nWidth = 0.0, double p_nHeight = 0.0);
    virtual ~SP_GR_BaseText();

    bool SetHideDimensions(bool p_bVal) { m_bHideDimensions = p_bVal; return TRUE; }
    bool GetHideDimensions() { return m_bHideDimensions; }

    virtual void Select(bool p_bSelect = TRUE, wxDC* p_pcDC = NULL);
    virtual void MakeControlPoints();
    virtual void DeleteControlPoints(wxDC *p_pcDC = NULL);
    virtual void ResetControlPoints();
    virtual void Move(wxDC& p_cDC, double p_nX, double p_nY, bool p_bDisplay = TRUE);

    bool CalculateDimensions(const wxString& p_sVal);

    //! override the OGL implementation to respect the 'show' flag
    void OnDrawOutline(wxDC& p_cDC, double p_nX, double p_nY, double p_nW, double p_nH);
    //! special, if we hide our dimensions
    bool HitTest(double p_nX, double p_nY, int* p_pnAttach, double* p_pnDistance);

    wxFont* const GetSelectFont();
    wxFont* const GetNormalFont();

    void SetSelectFont(wxFont* p_font);
    void SetNormalFont(wxFont* p_font);
};


#endif // __SP_GR_BaseText_H__
