/////////////////////////////////////////////////////////////////////////////
// Author:      alextov
// $Date: 2005/06/07 $
// Short Description: dc to write eps files. inherits from wxPostScriptDC
//                    and improves it a bit
/////////////////////////////////////////////////////////////////////////////

#ifndef __SP_EPSDC_H__
#define __SP_EPSDC_H__

#include <wx/dcps.h>
#include <wx/cmndata.h>
#include "sp_core/base/SP_Error.h"

#if wxABI_VERSION > 30000
//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class SP_EPSDC: public wxDC
{
 public:
  SP_EPSDC();

  // Recommended constructor
  SP_EPSDC(const wxPrintData& printData);
 private:
     DECLARE_DYNAMIC_CLASS(SP_EPSDC)
};

//-----------------------------------------------------------------------------
// SP_EPSDC
//-----------------------------------------------------------------------------

class SP_EPSDCImpl: public wxPostScriptDCImpl,  public SP_Error
{
 public:
	SP_EPSDCImpl( wxPrinterDC *owner );
	SP_EPSDCImpl( wxPrinterDC *owner, const wxPrintData& data );
	SP_EPSDCImpl( SP_EPSDC *owner );
	SP_EPSDCImpl( SP_EPSDC *owner, const wxPrintData& data );

  ~SP_EPSDCImpl();

  void Init();

  void SetBrush( const wxBrush& brush );
  void EndDoc();

  void CalcBoundingBox(wxCoord x, wxCoord y);

  // methods with corrected calls of CalcBoundingBox
  void DoDrawBitmap(const wxBitmap& bitmap, wxCoord x, wxCoord y, bool useMask = false);
  void DoDrawText( const wxString& text, wxCoord x, wxCoord y );
  void DoDrawEllipse (wxCoord x, wxCoord y, wxCoord width, wxCoord height);
  void DoDrawRectangle (wxCoord x, wxCoord y, wxCoord width, wxCoord height);
  void DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius);
  void DoDrawPolygon (int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, wxPolygonFillMode fillStyle);
  void DoDrawSpline(const wxPointList *points);

 protected:
  wxString m_origFilename;

  // store here the "real" bounding box
  wxCoord m_x1, m_y1, m_x2, m_y2;

 private:
    DECLARE_DYNAMIC_CLASS(SP_EPSDCImpl)
};
#else
//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class SP_EPSDC;

//-----------------------------------------------------------------------------
// SP_EPSDC
//-----------------------------------------------------------------------------

class SP_EPSDC: public wxPostScriptDC,  public SP_Error
{
 public:
  SP_EPSDC();

  // Recommended constructor
  SP_EPSDC(const wxPrintData& printData);

  ~SP_EPSDC();

  void SetBrush( const wxBrush& brush );
  void EndDoc();

  void CalcBoundingBox(wxCoord x, wxCoord y);

  // methods with corrected calls of CalcBoundingBox
  void DoDrawBitmap(const wxBitmap& bitmap, wxCoord x, wxCoord y, bool useMask = false);
  void DoDrawLines (int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset);
  void DoDrawText( const wxString& text, wxCoord x, wxCoord y );
  void DoDrawEllipse (wxCoord x, wxCoord y, wxCoord width, wxCoord height);
  void DoDrawRectangle (wxCoord x, wxCoord y, wxCoord width, wxCoord height);
  void DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius);
  void DoDrawPolygon (int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, int fillStyle);
  void DoDrawSpline(wxList *points);

 protected:
  wxString m_origFilename;

  // store here the wxT("real") bounding box
  wxCoord m_x1, m_y1, m_x2, m_y2;

 private:
    DECLARE_DYNAMIC_CLASS(SP_EPSDC)
};
#endif

#endif // __SP_EPSDC_H__
