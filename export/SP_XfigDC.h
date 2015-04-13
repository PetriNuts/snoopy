/////////////////////////////////////////////////////////////////////////////
// Author:      alextov
// $Date: 2005/06/05 $
// Short Description: dc to write xfig files. Implementation based on
//                    code from wxPostScriptDC
/////////////////////////////////////////////////////////////////////////////

#ifndef __SP_XFIGDC_H__
#define __SP_XFIGDC_H__

#include <wx/dc.h>
#include <wx/cmndata.h>

#if wxABI_VERSION > 30000

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class SP_XfigDC: public wxDC
{
public:
	SP_XfigDC();

  // Recommended constructor
	SP_XfigDC(const wxPrintData& printData);
private:
    DECLARE_DYNAMIC_CLASS(SP_XfigDC)
};

//-----------------------------------------------------------------------------
// SP_XfigDC
//-----------------------------------------------------------------------------

#define DEF_RESOLUTION 72

class SP_XfigDCImpl: public wxDCImpl
{
public:
	SP_XfigDCImpl( wxPrinterDC *owner );
	SP_XfigDCImpl( wxPrinterDC *owner, const wxPrintData& data );
	SP_XfigDCImpl( SP_XfigDC *owner );
	SP_XfigDCImpl( SP_XfigDC *owner, const wxPrintData& data );
  
  ~SP_XfigDCImpl();

  virtual bool Ok() const;

  virtual void BeginDrawing() {}
  virtual void EndDrawing() {}

  bool DoFloodFill(wxCoord x1, wxCoord y1,
		   const wxColour &col, wxFloodFillStyle style=wxFLOOD_SURFACE );
  bool DoGetPixel(wxCoord x1, wxCoord y1, wxColour *col) const;

  void DoDrawLine(wxCoord x1, wxCoord y1,
		  wxCoord x2, wxCoord y2);
  void DoCrossHair(wxCoord x, wxCoord y) ;
  void DoDrawArc(wxCoord x1,wxCoord y1, wxCoord x2,
		 wxCoord y2, wxCoord xc, wxCoord yc);

  void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w,
			 wxCoord h,double sa,double ea);

  void DoDrawPoint(wxCoord x, wxCoord y);
  void DoDrawLines(int n, const wxPoint points[], wxCoord xoffset = 0,
		   wxCoord yoffset = 0);
  void DoDrawPolygon(int n, const wxPoint points[], wxCoord xoffset = 0,
		     wxCoord yoffset = 0, wxPolygonFillMode fillStyle=wxODDEVEN_RULE);

  void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);

  void DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width,
			      wxCoord height, double radius = 20);

  void DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height);

  void DoDrawSpline(const wxList *points);

  bool DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width,
	      wxCoord height, wxDC *source, wxCoord xsrc,
	      wxCoord ysrc, wxRasterOperationMode rop = wxCOPY, bool useMask = FALSE,
	      wxCoord xsrcMask = -1, wxCoord ysrcMask = -1);

  bool CanDrawBitmap() const { return FALSE; }

  void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y );

  void DoDrawBitmap(const wxBitmap& bitmap, wxCoord x,
		     wxCoord y, bool useMask=FALSE);

  void DoDrawText(const wxString& text, wxCoord x, wxCoord y);
  
  void DoDrawRotatedText(const wxString& text, wxCoord x,
			 wxCoord y, double angle);

  void Clear();

  void SetFont(const wxFont& font);
  void SetPen(const wxPen& pen);
  void SetBrush(const wxBrush& brush);

  void SetLogicalFunction(wxRasterOperationMode function);
  void SetBackground(const wxBrush& brush);

  void DoSetClippingRegion(wxCoord x, wxCoord y,
			   wxCoord width, wxCoord height);
  void DestroyClippingRegion();

  void DoSetClippingRegionAsRegion( const wxRegion &clip ) { }

  // NB: this function works with device coordinates, not the logical ones!
  void DoSetDeviceClippingRegion(const wxRegion& region);

  bool StartDoc(const wxString& message);
  void EndDoc();

  void StartPage();
  void EndPage();

  wxCoord GetCharHeight() const;
  wxCoord GetCharWidth() const;
  bool CanGetTextExtent() const { return TRUE; }
  void DoGetTextExtent(const wxString& string, wxCoord *x, wxCoord *y,
                     wxCoord *descent = (wxCoord *) NULL,
                     wxCoord *externalLeading = (wxCoord *) NULL,
                     const wxFont *theFont = (wxFont *) NULL ) const;

  void DoGetSize(int* width, int* height) const;
  void DoGetSizeMM(int *width, int *height) const;

  // Resolution in pixels per logical inch
  wxSize GetPPI() const;

  void SetAxisOrientation( bool xLeftRight, bool yBottomUp );
  void SetDeviceOrigin( wxCoord x, wxCoord y );

  void SetBackgroundMode(int mode) { }
  void SetPalette(const wxPalette& palette) { }

  wxPrintData& GetPrintData() { return m_printData; }
  void SetPrintData(const wxPrintData& data) { m_printData = data; }

  virtual int GetDepth() const { return 24; }
  
  void SetResolution(int ppi);
  int GetResolution() const;
  
protected:
    FILE*             m_pstream;    //  output stream
    wxString          m_title;
    double            m_underlinePosition;
    double            m_underlineThickness;
    wxPrintData       m_printData;

    float m_scale;
    int m_resolution;

    void Init();

    int GetXfigFill();
    int GetXfigJoin();
    int GetXfigLStyle();
    int GetXfigLCap();
    int GetXfigPColor();
    int GetXfigFColor();
    int GetXfigFont();
    int GetXfigLineWidth();
    int GrayFromColor(const wxColour& p_c); 
    
private:
    DECLARE_DYNAMIC_CLASS(SP_XfigDCImpl)
};
#else
//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class SP_XfigDC;

//-----------------------------------------------------------------------------
// SP_XfigDC
//-----------------------------------------------------------------------------

#define DEF_RESOLUTION 72

class SP_XfigDC: public wxDC
{
public:
  SP_XfigDC();

  // Recommended constructor
  SP_XfigDC(const wxPrintData& printData);

  ~SP_XfigDC();

  virtual bool Ok() const;

  virtual void BeginDrawing() {}
  virtual void EndDrawing() {}

  bool DoFloodFill(wxCoord x1, wxCoord y1,
		   const wxColour &col, int style=wxFLOOD_SURFACE );
  bool DoGetPixel(wxCoord x1, wxCoord y1, wxColour *col) const;

  void DoDrawLine(wxCoord x1, wxCoord y1,
		  wxCoord x2, wxCoord y2);
  void DoCrossHair(wxCoord x, wxCoord y) ;
  void DoDrawArc(wxCoord x1,wxCoord y1, wxCoord x2,
		 wxCoord y2, wxCoord xc, wxCoord yc);

  void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w,
			 wxCoord h,double sa,double ea);

  void DoDrawPoint(wxCoord x, wxCoord y);
  void DoDrawLines(int n, wxPoint points[], wxCoord xoffset = 0,
		   wxCoord yoffset = 0);
  void DoDrawPolygon(int n, wxPoint points[], wxCoord xoffset = 0,
		     wxCoord yoffset = 0, int fillStyle=wxODDEVEN_RULE);

  void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);

  void DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width,
			      wxCoord height, double radius = 20);

  void DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height);

  void DoDrawSpline(wxList *points);

  bool DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width,
	      wxCoord height, wxDC *source, wxCoord xsrc,
	      wxCoord ysrc, int rop = wxCOPY, bool useMask = FALSE,
	      wxCoord xsrcMask = -1, wxCoord ysrcMask = -1);

  bool CanDrawBitmap() const { return FALSE; }

  void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y );

  void DoDrawBitmap(const wxBitmap& bitmap, wxCoord x,
		     wxCoord y, bool useMask=FALSE);

  void DoDrawText(const wxString& text, wxCoord x, wxCoord y);

  void DoDrawRotatedText(const wxString& text, wxCoord x,
			 wxCoord y, double angle);

  void Clear();

  void SetFont(const wxFont& font);
  void SetPen(const wxPen& pen);
  void SetBrush(const wxBrush& brush);

  void SetLogicalFunction(int function);
  void SetBackground(const wxBrush& brush);

  void DoSetClippingRegion(wxCoord x, wxCoord y,
			   wxCoord width, wxCoord height);
  void DestroyClippingRegion();

  void DoSetClippingRegionAsRegion( const wxRegion &clip ) { }

  bool StartDoc(const wxString& message);
  void EndDoc();

  void StartPage();
  void EndPage();

  wxCoord GetCharHeight() const;
  wxCoord GetCharWidth() const;
  bool CanGetTextExtent() const { return TRUE; }
  void DoGetTextExtent(const wxString& string, wxCoord *x, wxCoord *y,
                     wxCoord *descent = (wxCoord *) NULL,
                     wxCoord *externalLeading = (wxCoord *) NULL,
                     wxFont *theFont = (wxFont *) NULL ) const;

  void DoGetSize(int* width, int* height) const;
  void DoGetSizeMM(int *width, int *height) const;

  // Resolution in pixels per logical inch
  wxSize GetPPI() const;

  void SetAxisOrientation( bool xLeftRight, bool yBottomUp );
  void SetDeviceOrigin( wxCoord x, wxCoord y );

  void SetBackgroundMode(int mode) { }
  void SetPalette(const wxPalette& palette) { }

  wxPrintData& GetPrintData() { return m_printData; }
  void SetPrintData(const wxPrintData& data) { m_printData = data; }

  virtual int GetDepth() const { return 24; }

  void SetResolution(int ppi);
  int GetResolution();

protected:
    FILE*             m_pstream;    //  output stream
    wxString          m_title;
    double            m_underlinePosition;
    double            m_underlineThickness;
    wxPrintData       m_printData;

    float m_scale;
    int m_resolution;

    void Init();

    int GetXfigFill();
    int GetXfigJoin();
    int GetXfigLStyle();
    int GetXfigLCap();
    int GetXfigPColor();
    int GetXfigFColor();
    int GetXfigFont();
    int GetXfigLineWidth();
    int GrayFromColor(const wxColour& p_c);

private:
    DECLARE_DYNAMIC_CLASS(SP_XfigDC)
};
#endif

#endif // __SP_XFIGDC_H__
