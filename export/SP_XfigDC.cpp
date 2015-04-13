/////////////////////////////////////////////////////////////////////////////
// Author:      alextov
// $Date: 2005/06/05 $
// Short Description: dc to write xfig files.
//                 A DRAFT Implementation based on code from wxPostScriptDC
/////////////////////////////////////////////////////////////////////////////

#include "export/SP_XfigDC.h"
#include "sp_utilities.h"
#include <wx/paper.h>
#include <wx/filename.h>

#if wxABI_VERSION > 30000

// Xfig objects
enum XfigObjects{
  XFIG_COLOR = 0, // Color pseudo-object
  XFIG_ELLIPSE,   // Ellipse which is a generalization of circle.
  XFIG_POLYLINE,  // Polyline which includes polygon and box
  XFIG_SPLINE,    // Spline includes closed/open approximated/interpolated/x-spline spline.
  XFIG_TEXT,      // Text
  XFIG_ARC,       // Arc
  XFIG_COMPOUND  // Compound object which is composed of one or more objects.
};

// line stile
enum XfigLineStyle{
  XFIG_LSTYLE_DEFAULT = -1, // -1 = Default
  XFIG_LSTYLE_SOLID,  //       0 = Solid
  XFIG_LSTYLE_DASHED, //       1 = Dashed
  XFIG_LSTYLE_DOTTED, //       2 = Dotted
  XFIG_LSTYLE_DASH_DOTTED, //       3 = Dash-dotted
  XFIG_LSTYLE_DASH_DOUBLE_DOTTED, //    4 = Dash-double-dotted
  XFIG_LSTYLE_DASH_TRIPLE_DOTTED //   5 = Dash-triple-dotted
};

// arrows
enum XfigArrows{
  XFIG_ARROW_STICK = 0,
  XFIG_ARROW_CTRIANGLE,
  XFIG_ARROW_INDENTED_BUTT,
  XFIG_ARROW_POINTED_BUTT
};


enum XfigArrowStyle{
  XFIG_ARROW_HOLLOW_STYLE = 0, // actually filled with white
  XFIG_ARROW_FILLED_STYLE
};

enum XfigColor{
  XFIG_COLOR_DEFAULT = -1, // = Default
  XFIG_COLOR_BLACK, // 0 = Black
  XFIG_COLOR_BLUE, //  1 = Blue
  XFIG_COLOR_GREEN, // 2 = Green
  XFIG_COLOR_CYAN, //  3 = Cyan
  XFIG_COLOR_RED,  //  4 = Red
  XFIG_COLOR_MAGENTA, // 5 = Magenta
  XFIG_COLOR_YELLOW, //  6 = Yellow
  XFIG_COLOR_WHITE, //   7 = White
};

//-------------------------------------------------------------------------------
// SP_XfigDC
//-------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(SP_XfigDC, wxDC)

SP_XfigDC::SP_XfigDC()
 : wxDC(new SP_XfigDCImpl(this))
{
}

SP_XfigDC::SP_XfigDC(const wxPrintData& printData)
 : wxDC(new SP_XfigDCImpl(this, printData))
{
}

//-------------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(SP_XfigDCImpl, wxDCImpl)

SP_XfigDCImpl::SP_XfigDCImpl (SP_XfigDC *owner)
	: wxDCImpl(owner)
{
	Init();
}

SP_XfigDCImpl::SP_XfigDCImpl (SP_XfigDC *owner, const wxPrintData& printData)
	: wxDCImpl(owner)
{
	Init();
  m_printData = printData;
  m_ok = TRUE;
#if wxMAC_USE_CORE_GRAPHICS
	// we need at least a measuring context because people start measuring before a page
	// gets printed at all
	SetGraphicsContext( wxGraphicsContext::Create());
#endif
}

SP_XfigDCImpl::SP_XfigDCImpl (wxPrinterDC *owner)
	: wxDCImpl(owner)
{
	Init();
}

SP_XfigDCImpl::SP_XfigDCImpl (wxPrinterDC *owner, const wxPrintData& printData)
	: wxDCImpl(owner)
{
	Init();
  m_printData = printData;
  m_ok = TRUE;
#if wxMAC_USE_CORE_GRAPHICS
	// we need at least a measuring context because people start measuring before a page
	// gets printed at all
	SetGraphicsContext( wxGraphicsContext::Create());
#endif
}


//-------------------------------------------------------------------------------
int SP_XfigDCImpl::GrayFromColor(const wxColour& p_c)
{
  int res = (int) (p_c.Green() * 0.5 + p_c.Red() * 0.25 + p_c.Blue() * 0.25);
//   SP_LOGDEBUG(wxString::Format(wxT("GrayFromColor: res=%d (%d, %d, %d)"), res,
// 				 p_c.Red(), p_c.Green(), p_c.Blue()));
  return res;
}

int SP_XfigDCImpl::GetXfigFill()
{

  int res;
  switch (m_brush.GetStyle()) {
  case wxTRANSPARENT: 	//Transparent (no fill).
    res = -1;
    break;
  case wxSOLID:  // Solid
	  if (m_brush.GetColour() == *wxWHITE){
		  //white color is set to None Fill,
		  //because the black token on the places will disappear if they are white filled
		  res= -1;
	  } else if (m_brush.GetColour() == *wxBLACK) {
				res = 20;  // Black
			} else {
				int gr = GrayFromColor(m_brush.GetColour());
				res = 1 + (int) (gr / 13.4);
				}  //endif

//     SP_LOGDEBUG(wxString::Format(wxT("GetXfigFill: res=%d"), res));
    break;
  case wxBDIAGONAL_HATCH: // 	Backward diagonal hatch.
    res = 41; // 30 degree left diagonal pattern
    break;
  case wxCROSSDIAG_HATCH:  // 	Cross-diagonal hatch.
    res = 43;  // 30 degree crosshatch
    break;
  case wxFDIAGONAL_HATCH: // 	Forward diagonal hatch.
    res = 46; // 45 degree crosshatch
    break;
  case wxCROSS_HATCH: 	// Cross hatch.
    res = 51; // crosshatch
    break;
  case wxHORIZONTAL_HATCH: // 	Horizontal hatch.
    res = 49; //  horizontal lines
    break;
  case wxVERTICAL_HATCH: //	Vertical hatch.
    res = 50;  //vertical lines
    break;
  default:
    res = -1; // not filled
    SP_LOGDEBUG(wxString::Format(wxT("GetXfigFill - unsupported fill %d, set to %d"),
				   m_brush.GetStyle(), res));
    break;
  }
  return res;
}

int SP_XfigDCImpl::GetXfigJoin()
{
  int res = -1;
  switch (m_pen.GetJoin()) {
  case wxJOIN_BEVEL:
    res = 2;
    break;
  case wxJOIN_ROUND:
    res = 1;
    break;
  case wxJOIN_MITER:
    res = 0;
    break;
  default:
	res = 0;
	break;
  }
  return res;
}

int SP_XfigDCImpl::GetXfigLStyle()
{
  int res;
  switch (m_pen.GetStyle()) {
  case wxSOLID:  // Solid
    res = XFIG_LSTYLE_SOLID;
    break;
  case wxDOT:
    res = XFIG_LSTYLE_DOTTED;
    break;
  case wxSHORT_DASH:
  case wxLONG_DASH:
    res = XFIG_LSTYLE_DASHED;
    break;
  case wxDOT_DASH:
    res = XFIG_LSTYLE_DASH_DOTTED;
    break;
  default:
    res = -1; // default
    SP_LOGDEBUG(wxString::Format(wxT("GetXfigLStyle - unsupported style %d, set to %d"),
				   m_brush.GetStyle(), res));
    break;
  }
  return res;
}


int SP_XfigDCImpl::GetXfigLineWidth()
{
  return m_pen.GetWidth();
}


int SP_XfigDCImpl::GetXfigPColor()
{
  if (m_pen.GetColour() == *wxWHITE) return XFIG_COLOR_WHITE;
  return XFIG_COLOR_DEFAULT;  // TODO
}


int SP_XfigDCImpl::GetXfigFColor()
{
  if (m_brush.GetStyle() != wxTRANSPARENT &&
      m_brush.GetStyle() != wxSOLID) return XFIG_COLOR_WHITE;

  if (m_brush.GetColour() == *wxWHITE) return XFIG_COLOR_WHITE;
  if (m_brush.GetColour() == *wxBLACK) return XFIG_COLOR_BLACK;
  return XFIG_COLOR_WHITE;
}

int SP_XfigDCImpl::GetXfigLCap()
{
  int res = -1;
  switch (m_pen.GetCap()) {
  case wxCAP_ROUND:
    res = 1;
    break;
  case wxCAP_PROJECTING:
    res = 2;
    break;
  case wxCAP_BUTT:
    res = 0;
    break;
  default:
	res = 0;
	break;
  }
  return res;
}

// latex fonts, font_flags bit 2 should be 0
// TODO: Postscript fonts support
int SP_XfigDCImpl::GetXfigFont()
{
  int res;
  switch (m_font.GetFamily()) {
  case wxDEFAULT:
    res = 0;  // default
    break;
  case wxSWISS:  // A sans-serif font.
    res = 4;    // Sans Serif
    break;
  case wxMODERN:
    res = 5;
    break;     // Typewriter
  case wxROMAN:
    res = 1;   // Roman
    break;
  default:
    res = 0;
    SP_LOGDEBUG(wxString::Format(wxT("GetXfigFont - unsupported family %d, set to %d"),
				   m_font.GetFamily(), res));
    break;
  }

  if (m_font.GetStyle() == wxITALIC) {
    res = 3; // italic
    SP_LOGDEBUG(wxT("Font set to italic (3)"));
  }

  if (m_font.GetWeight() == wxBOLD) {
    res = 2; // bold
    SP_LOGDEBUG(wxT("Font set to bold (2)"));
  }

  return res;
}

//-------------------------------------------------------------------------------

void
SP_XfigDCImpl::Init ()
{
  m_pstream = (FILE*) NULL;

  m_underlinePosition = 0.0;
  m_underlineThickness = 0.0;

  m_signX =  1;  // default x-axis left to right
  m_signY = 1;  // default y-axis top up -> bottom down

  m_scale = 1;
  m_resolution = DEF_RESOLUTION;
}


SP_XfigDCImpl::~SP_XfigDCImpl ()
{
  if (m_pstream) {
    fclose( m_pstream );
    m_pstream = (FILE*) NULL;
  }
}

//------------------------------------------------------------------------------

bool SP_XfigDCImpl::StartDoc( const wxString& message )
{
  //  SP_LOGDEBUG(wxString::Format(wxT("StartDoc %s"), message.c_str()));
  wxCHECK_MSG( m_ok, FALSE, wxT("invalid xfig dc") );

  if (m_printData.GetFilename() == wxT("")) {
    wxString filename = wxFileName::CreateTempFileName( wxT("xfig") );
    SP_LOGDEBUG(wxString::Format(wxT("writing to temporary file %s"),
		  filename.c_str()));
    m_printData.SetFilename(filename);
  }

  m_pstream = wxFopen( m_printData.GetFilename().c_str(), wxT("w+") );

  if (!m_pstream) {
    SP_LOGERROR( _("Cannot open file for Xfig output!"));
    m_ok = FALSE;
    return FALSE;
  }

  m_ok = TRUE;

  wxFprintf (m_pstream, wxT("#FIG 3.2\n"));
  if (m_printData.GetOrientation() == wxLANDSCAPE) {
    wxFprintf(m_pstream, wxT("Landscape\n"));
  } else {
    wxFprintf(m_pstream, wxT("Portrait\n"));
  }

  // other possibility wxT("Center")
  wxFprintf(m_pstream, wxT("Flush Left\n"));

  // units
  wxFprintf(m_pstream, wxT("Metric\n"));

  wxString paper = wxT("A4");
  switch (m_printData.GetPaperId())  {
  case wxPAPER_LETTER:
    paper = wxT("Letter"); // Letter: paper ""; 8 1/2 by 11 inches
    break;
  case wxPAPER_LEGAL: // Legal, 8 1/2 by 14 inches
    paper = wxT("Legal");
    break;
  case wxPAPER_A4: // A4 Sheet, 210 by 297 millimeters
    paper = wxT("A4");
    break;
  case wxPAPER_TABLOID: // Tabloid, 11 by 17 inches
    paper = wxT("Tabloid");
    break;
  case wxPAPER_LEDGER: // Ledger, 17 by 11 inches
    paper = wxT("Ledger");
    break;
  case wxPAPER_A3: // A3 sheet, 297 by 420 millimeters
    paper = wxT("A3");
    break;
  case wxPAPER_A2:
    paper = wxT("A2");
    break;
  case wxPAPER_B5: // B5 sheet, 182-by-257-millimeter paper
    paper = wxT("B5");
    break;
  default:
    paper = wxT("A4");
    SP_LOGDEBUG(wxT("Unsupported paper size, size is set to A4"));
    break;
  }

  wxFprintf(m_pstream, wxT("%s\n"), paper.c_str());

  // magnification
  wxFprintf(m_pstream, wxT("100.00\n"));

  // multiple-page?
  wxFprintf(m_pstream, wxT("Single\n"));

  // transparent color for GIF export. -3=background, -2=None, -1=Default
  wxFprintf(m_pstream, wxT("-2\n"));

  //  resolution coord_system (2: upper left)
  wxFprintf(m_pstream, wxString::Format(wxT("%d 2\n"), GetResolution()));

  return TRUE;
}

void SP_XfigDCImpl::EndDoc ()
{
  //  SP_LOGDEBUG(wxT("EndDoc"));
  wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );

  fclose( m_pstream );
  m_pstream = (FILE *) NULL;
}

//------------------------------------------------------------------------------

void SP_XfigDCImpl::DoDrawEllipse (wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
  //  SP_LOGDEBUG(wxString::Format(wxT("DoDrawEllipse %d %d %d %d"),
  //				 x, y,
  //			 width, height));
  wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );

  if (m_brush.GetStyle() != wxTRANSPARENT ||
      m_pen.GetStyle() != wxTRANSPARENT) {
    wxFprintf (m_pstream, wxT("%d"), XFIG_ELLIPSE);
    if (width == height) {
      wxFprintf (m_pstream, wxT(" 2")); // circle defined by radius
    } else {
      wxFprintf (m_pstream, wxT(" 1")); // ellipse defined by radius
    }

    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());

    wxFprintf (m_pstream, wxT(" %d"), GetXfigLineWidth()); // thickness
    wxFprintf (m_pstream, wxT(" %d"), GetXfigPColor());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFColor());
    wxFprintf (m_pstream, wxT(" 1")); // depth
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFill());
    wxFprintf (m_pstream, wxT(" -1.0")); //  TODO: float style_val (1/80 inch) ???
    wxFprintf (m_pstream, wxT(" 1")); //  direction
    wxFprintf (m_pstream, wxT(" 0.0")); //  radians, the angle of the x-axis

    int tX = (int) (x * m_scale);
    int tY = (int) (y * m_scale);
    int radiusX = ((int) (width * m_scale)) / 2;
    int radiusY = ((int) (height * m_scale)) / 2;

    wxFprintf (m_pstream, wxT(" %d %d"), tX + radiusX,
	     tY + radiusX); //  center_x, center_y
    wxFprintf (m_pstream, wxT(" %d %d"), radiusX, radiusY); //  radius_x, radius_y
    wxFprintf (m_pstream, wxT(" %d %d"), tX + width,
	     tY + height); //  1st point entered
    wxFprintf (m_pstream, wxT(" %d %d\n"), tX + width,
	     tY + height); //  last point entered
  }
}


void SP_XfigDCImpl::DoDrawArc (wxCoord x1, wxCoord y1, wxCoord x2,
			   wxCoord y2, wxCoord xc, wxCoord yc)
{
//  SP_LOGDEBUG(wxString::Format(wxT("DoDrawArc %d %d %d %d %d %d"),
//				 x1, y1, x2,
//				 y2, xc,  yc));
  wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );

  if (m_brush.GetStyle () != wxTRANSPARENT ||
      m_pen.GetStyle () != wxTRANSPARENT) {
    wxFprintf (m_pstream, wxT("%d"), XFIG_ARC);
    wxFprintf (m_pstream, wxT(" 1")); // open ended arc
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLineWidth()); // thickness
    wxFprintf (m_pstream, wxT(" %d"), GetXfigPColor());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFColor());
    wxFprintf (m_pstream, wxT(" 1")); // depth
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFill());
    wxFprintf (m_pstream, wxT(" -1.0")); //  TODO: float style_val (1/80 inch) ???
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLCap()); //  cap_style
    wxFprintf (m_pstream, wxT(" 0")); //  direction
    wxFprintf (m_pstream, wxT(" 0")); //  forward_arrow
    wxFprintf (m_pstream, wxT(" 0")); //  backward_arrow
    wxFprintf (m_pstream, wxT(" %d %d"),
	     (int) (xc * m_scale), (int) (yc *m_scale)); // center of the arc
    wxFprintf (m_pstream, wxT(" %d %d"), (int) (x1 * m_scale),
	     (int) (y1 * m_scale)); //  1st point entered

    wxFprintf (m_pstream, wxT(" %d %d"), (int) (x2 * m_scale),
	     (int) (y2 * m_scale)); //  2nd point entered

    wxFprintf (m_pstream, wxT(" %d %d\n"), (int) (x2 * m_scale),
	     (int) (y2 * m_scale)); //  last point entered
  }
}



void SP_XfigDCImpl::DoDrawText( const wxString& text, wxCoord x, wxCoord y )
{
//  SP_LOGDEBUG(wxString::Format(wxT("DoDrawText %s %d %d"), text.c_str(),
//				 x, y));

  DoDrawRotatedText (text, x, y, 0);

}

void SP_XfigDCImpl::DoDrawRotatedText( const wxString& text, wxCoord x,
				   wxCoord y, double angle )
{
//  SP_LOGDEBUG(wxString::Format(wxT("DoDrawRotatedText %s %d %d %f"),
//				 text.c_str(), x, y, angle));
  if (m_pen.GetStyle () != wxTRANSPARENT) {

    wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );

    wxFprintf (m_pstream, wxT("%d"), XFIG_TEXT);
    wxFprintf (m_pstream, wxT(" 0")); // Left justified
    wxFprintf (m_pstream, wxT(" %d"), GetXfigPColor());
    wxFprintf (m_pstream, wxT(" 0")); // depth
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFont());
    wxFprintf (m_pstream, wxT(" %d"), GetCharHeight());
    wxFprintf (m_pstream, wxT(" %f"), angle);
    wxFprintf (m_pstream, wxT(" 2")); // font_flags Special text (for LaTeX)
    wxFprintf (m_pstream, wxT(" -1.0")); //  height
    wxFprintf (m_pstream, wxT(" -1.0")); //  length

    int tX = (int) ((x + GetCharWidth() / 2.0) * m_scale);
    int tY = (int) ((y + GetCharHeight() / 2.0)  * m_scale);

    wxFprintf (m_pstream, wxT(" %d %d"), tX, tY); //  center_x, center_y
    wxFprintf (m_pstream, wxT(" %s\\001\n"), text.c_str());
  }
}


void SP_XfigDCImpl::DoDrawLines (int n, const wxPoint points[],
			     wxCoord xoffset, wxCoord yoffset)
{
//     SP_LOGDEBUG(wxString::Format(wxT("DoDrawLines %d %d %d"),
// 		n, xoffset, yoffset));
    wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );

  if (m_pen.GetStyle () != wxTRANSPARENT) {
    wxFprintf (m_pstream, wxT("%d"), XFIG_POLYLINE);
    wxFprintf (m_pstream, wxT(" 1")); // polyline
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLineWidth()); // thickness
    wxFprintf (m_pstream, wxT(" %d"), GetXfigPColor());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFColor());
    wxFprintf (m_pstream, wxT(" 1")); // depth
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" -1"));  // area_fill  -1 = no fill
    wxFprintf (m_pstream, wxT(" -1.0")); //  TODO: float style_val (1/80 inch) ???

    wxFprintf (m_pstream, wxT(" %d"), GetXfigJoin());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLCap());
    wxFprintf (m_pstream, wxT(" 0")); //  radius of arc-boxes
    wxFprintf (m_pstream, wxT(" 0")); //  forward_arrow
    wxFprintf (m_pstream, wxT(" 0")); //  backward_arrow
    wxFprintf (m_pstream, wxT(" %d\n"), n); //  npoints

    int tX, tY;
    for (int i = 0; i < n; i++) {
      tX = (int) (points[i].x * m_scale + xoffset * m_scale);
      tY = (int) (points[i].y * m_scale + yoffset * m_scale);
      wxFprintf (m_pstream, wxT("%d %d\n"), tX, tY);
    }
  }
}


void SP_XfigDCImpl::DoDrawPoint(wxCoord x, wxCoord y)
{
  SP_LOGDEBUG(wxString::Format(wxT("DoDrawPoint %d %d"),
				  x, y));
  wxPoint tmp[] = {wxPoint(x,y), wxPoint(x, y)};
  DoDrawLines(2, tmp);
}

void SP_XfigDCImpl::DoDrawLine (wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
  SP_LOGDEBUG(wxString::Format(wxT("DoDrawLine %d %d %d %d"),
				 x1, y1,
				 x2, y2));

  wxPoint tmp[] = {wxPoint(x1,y1), wxPoint(x2, y2)};
  DoDrawLines(2, tmp);
}


void SP_XfigDCImpl::DoDrawPolygon (int n, const wxPoint points[],
			       wxCoord xoffset, wxCoord yoffset,
			       wxPolygonFillMode fillStyle)
{
//    SP_LOGDEBUG(wxString::Format(wxT("DoDrawPolygon %d, style %d tr:%d"),
//  		n, m_pen.GetStyle(), m_pen.GetStyle() == wxTRANSPARENT ));
  wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );


  if (m_pen.GetStyle() != wxTRANSPARENT &&
      m_pen.GetColour() != *wxWHITE) {
    wxFprintf (m_pstream, wxT("%d"), XFIG_POLYLINE);
    wxFprintf (m_pstream, wxT(" 3")); // polygon
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLineWidth()); // thickness
    wxFprintf (m_pstream, wxT(" %d"), GetXfigPColor());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFColor());
    wxFprintf (m_pstream, wxT(" 1")); // depth
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFill());
    wxFprintf (m_pstream, wxT(" -1.0")); //  TODO: float style_val (1/80 inch) ???

    wxFprintf (m_pstream, wxT(" %d"), GetXfigJoin());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLCap());
    wxFprintf (m_pstream, wxT(" 0")); //  radius of arc-boxes
    wxFprintf (m_pstream, wxT(" 0")); //  forward_arrow
    wxFprintf (m_pstream, wxT(" 0")); //  backward_arrow
    wxFprintf (m_pstream, wxT(" %d\n"), n); //  npoints

    int tX, tY;
    for (int i = 0; i < n; i++) {
      tX = (int) (points[i].x * m_scale + xoffset * m_scale);
      tY = (int) (points[i].y * m_scale + yoffset * m_scale);
      wxFprintf (m_pstream, wxT("%d %d\n"), tX, tY);
    }
  }
}



void SP_XfigDCImpl::DoDrawRectangle (wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
//    SP_LOGDEBUG(wxString::Format(wxT("DoDrawRectangle %d %d %d %d"),
//  				 x, y,
//  				 width, height));

    wxPoint tmp[] = {wxPoint(x,y), wxPoint(x + width, y),
		    wxPoint(x+width,y+height), wxPoint(x, y+ height),
		    wxPoint(x, y)};
    DoDrawPolygon(5, tmp);

// wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );
//   if (m_pen.GetStyle () != wxTRANSPARENT) {
//     wxFprintf (m_pstream, wxT("%d"), XFIG_POLYLINE);
//     wxFprintf (m_pstream, wxT(" 2")); // box
//     wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
//     wxFprintf (m_pstream, wxT(" 1")); // TODO thickness
//     wxFprintf (m_pstream, wxT(" %d"), GetXfigPColor());
//     wxFprintf (m_pstream, wxT(" %d"), GetXfigFColor());
//     wxFprintf (m_pstream, wxT(" 1")); // depth
//     wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
//     wxFprintf (m_pstream, wxT(" %d"), GetXfigFill());
//     wxFprintf (m_pstream, wxT(" -1.0")); //  TODO: float style_val (1/80 inch) ???

//     wxFprintf (m_pstream, wxT(" %d"), GetXfigJoin());
//     wxFprintf (m_pstream, wxT(" %d"), GetXfigLCap());
//     wxFprintf (m_pstream, wxT(" 0")); //  radius of arc-boxes
//     wxFprintf (m_pstream, wxT(" 0")); //  forward_arrow
//     wxFprintf (m_pstream, wxT(" 0")); //  backward_arrow
//     wxFprintf (m_pstream, wxT(" 5\n")); //  npoints

//     int tX = (int) (x * m_scale);
//     int tY = (int) (y * m_scale);
//     int tW = (int) (width * m_scale);
//     int tH = (int) (height * m_scale);
//     wxFprintf (m_pstream, wxT("%d %d\n%d %d\n%d %d\n%d %d\n%d %d\n"),
// 	     tX, tY,
// 	     tX + tW, tY,
// 	     tX+tW, tY+tH,
// 	     tX, tY+tH,
// 	     tX, tY);
//  }
}


void SP_XfigDCImpl::DoDrawRoundedRectangle (wxCoord x, wxCoord y,
					wxCoord width, wxCoord height,
					double radius)
{
//   SP_LOGDEBUG(wxString::Format(wxT("DoDrawRoundedRectangle %d %d %d %d %f"),
// 				 x, y,
// 				 width, height,
// 				 radius));

  wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );


  if (m_pen.GetStyle () != wxTRANSPARENT) {
    wxFprintf (m_pstream, wxT("%d"), XFIG_POLYLINE);
    wxFprintf (m_pstream, wxT(" 4")); // polygon
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLineWidth()); // thickness
    wxFprintf (m_pstream, wxT(" %d"), GetXfigPColor());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFColor());
    wxFprintf (m_pstream, wxT(" 1")); // depth
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFill());
    wxFprintf (m_pstream, wxT(" -1.0")); //  TODO: float style_val (1/80 inch) ???

    wxFprintf (m_pstream, wxT(" %d"), GetXfigJoin());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLCap());
    wxFprintf (m_pstream, wxT(" %d"), (int) radius); //  radius of arc-boxes
    wxFprintf (m_pstream, wxT(" 0")); //  forward_arrow
    wxFprintf (m_pstream, wxT(" 0")); //  backward_arrow
    wxFprintf (m_pstream, wxT(" 5\n")); //  npoints

    int tX = (int) (x * m_scale);
    int tY = (int) (y * m_scale);
    int tW = (int) (width * m_scale);
    int tH = (int) (height * m_scale);
    wxFprintf (m_pstream, wxT("%d %d\n%d %d\n%d %d\n%d %d\n%d %d\n"),
	     tX, tY,
	     tX + tW, tY,
	     tX+tW, tY+tH,
	     tX, tY+tH,
	     tX, tY);
  }
}



//------------------------------------------------------------------------------
void SP_XfigDCImpl::SetBackground (const wxBrush& brush)
{
  SP_LOGDEBUG(wxT("SetBackground"));
  m_backgroundBrush = brush;
}

void SP_XfigDCImpl::SetResolution(int ppi)
{
  m_resolution = ppi;
  m_scale = (float)ppi / DEF_RESOLUTION;
}

int SP_XfigDCImpl::GetResolution() const
{
  return m_resolution;
}


void SP_XfigDCImpl::SetFont( const wxFont& font )
{
//   SP_LOGDEBUG(wxT("SetFont"));
  m_font = font;
}

void SP_XfigDCImpl::SetPen( const wxPen& pen )
{
//   SP_LOGDEBUG(wxT("SetPen"));
  m_pen = pen;
}

void SP_XfigDCImpl::SetBrush( const wxBrush& brush )
{
//   SP_LOGDEBUG(wxT("SetBrush"));
  m_brush = brush;
}

bool SP_XfigDCImpl::Ok() const
{
  return m_ok;
}


// from wxPostscriptDC, not tested
//------------------------------------------------------------------------------

void SP_XfigDCImpl::SetDeviceOrigin( wxCoord x, wxCoord y )
{
  SP_LOGDEBUG(wxString::Format(wxT("SetDeviceOrigin %d %d"),
				 x, y));
  wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );

  int h = 0;
  int w = 0;
  GetSize( &w, &h );

  wxDCImpl::SetDeviceOrigin( x, h-y );
}

void SP_XfigDCImpl::DoGetSize(int* width, int* height) const
{
  //  SP_LOGDEBUG(wxT("DoGetSize"));
  wxPaperSize id = m_printData.GetPaperId();

  wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(id);

  if (!paper) paper = wxThePrintPaperDatabase->FindPaperType(wxPAPER_A4);

  int w = 595;
  int h = 842;
  if (paper) {
    w = paper->GetSizeDeviceUnits().x;
    h = paper->GetSizeDeviceUnits().y;
  }

  if (m_printData.GetOrientation() == wxLANDSCAPE) {
    int tmp = w;
    w = h;
    h = tmp;
  }

  if (width) *width = (int)(w * m_scale);
  if (height) *height = (int)(h * m_scale);
}

void SP_XfigDCImpl::DoGetSizeMM(int *width, int *height) const
{
  //  SP_LOGDEBUG(wxT("DoGetSizeMM"));
  wxPaperSize id = m_printData.GetPaperId();

  wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(id);

  if (!paper) paper = wxThePrintPaperDatabase->FindPaperType(wxPAPER_A4);

  int w = 210;
  int h = 297;

  if (paper) {
        w = paper->GetWidth() / 10;
        h = paper->GetHeight() / 10;
  }

  if (m_printData.GetOrientation() == wxLANDSCAPE) {
    int tmp = w;
    w = h;
    h = tmp;
  }

  if (width) *width = w;
  if (height) *height = h;
}

wxCoord SP_XfigDCImpl::GetCharHeight() const
{
  //  SP_LOGDEBUG(wxT("GetCharHeight"));
  if (m_font.Ok())
    return m_font.GetPointSize()+1;
  else
    return 12;
}

wxCoord SP_XfigDCImpl::GetCharWidth() const
{
  //  SP_LOGDEBUG(wxT("GetCharWidth"));
  // Chris Breeze: reasonable approximation using wxMODERN/Courier
  return (wxCoord) (GetCharHeight() * DEF_RESOLUTION / 120.0);
}

// Resolution in pixels per logical inch
wxSize SP_XfigDCImpl::GetPPI(void) const
{
  //  SP_LOGDEBUG(wxT("GetPPI"));
  return wxSize(m_resolution, m_resolution);
}

// do nothing
//------------------------------------------------------------------------------

void SP_XfigDCImpl::StartPage()
{
  SP_LOGDEBUG(wxT("StartPage"));
}

void SP_XfigDCImpl::EndPage ()
{
  SP_LOGDEBUG(wxT("EndPage"));
}


void SP_XfigDCImpl::DoSetClippingRegion (wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
//   SP_LOGDEBUG(wxString::Format(wxT("DoSetClippingRegion %d %d %d %d"),
// 				 x, y, w, h));
}

void SP_XfigDCImpl::DoSetDeviceClippingRegion(const wxRegion& r)
{
//   SP_LOGDEBUG(wxString::Format(wxT("DoSetClippingRegion %d %d %d %d"),
// 				 x, y, w, h));
}

void SP_XfigDCImpl::DestroyClippingRegion()
{
  //  SP_LOGDEBUG(wxT("DestroyClippingRegion"));
}


// not implemented
//------------------------------------------------------------------------------

bool SP_XfigDCImpl::DoBlit(wxCoord xdest, wxCoord ydest,
		       wxCoord fwidth, wxCoord fheight,
		       wxDC *source,
		       wxCoord xsrc, wxCoord ysrc,
		       wxRasterOperationMode rop, bool useMask,
		       wxCoord xsrcMask, wxCoord ysrcMask )
{
  SP_LOGDEBUG(wxT("DoBlit"));
  wxFAIL_MSG( wxT("SP_XfigDCImpl::DoBlit not implemented.") );
  return FALSE;
}


void SP_XfigDCImpl::DoGetTextExtent(const wxString& string,
                                     wxCoord *x, wxCoord *y,
                                     wxCoord *descent, wxCoord *externalLeading,
                                     const wxFont *theFont ) const
{
  SP_LOGDEBUG(wxT("DoGetTextExtent"));
  wxFAIL_MSG( wxT("SP_XfigDCImpl::DoGetTextExtent not implemented.") );
}


void SP_XfigDCImpl::Clear()
{
  SP_LOGDEBUG(wxT("Clear"));
  wxFAIL_MSG( wxT("SP_XfigDCImpl::Clear not implemented.") );
}


void SP_XfigDCImpl::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w,
				  wxCoord h, double sa, double ea)
{
  SP_LOGDEBUG(wxString::Format(wxT("DoDrawEllipticArc %d %d %d %d %f %f"),
				 x, y, w,
				 h, sa,  ea));
  wxFAIL_MSG( wxT("SP_XfigDCImpl::FloodFill not implemented.") );
}


bool SP_XfigDCImpl::DoFloodFill (wxCoord x, wxCoord y,
			     const wxColour &col, wxFloodFillStyle style)
{
  SP_LOGDEBUG(wxString::Format(wxT("DoFloodFill %d %d, color: %d %d %d, style %d"),
				 x, y,
				 col.Red(), col.Green(), col.Blue(), style));
  wxFAIL_MSG( wxT("SP_XfigDCImpl::FloodFill not implemented.") );
  return FALSE;
}

bool SP_XfigDCImpl::DoGetPixel (wxCoord x, wxCoord y,
			    wxColour * col) const
{
  SP_LOGDEBUG(wxString::Format(wxT("DoGetPixel %d %d, color: %d %d %d"),
				 x, y,
		col->Red(), col->Green(), col->Blue()));
  wxFAIL_MSG( wxT("SP_XfigDCImpl::GetPixel not implemented.") );
  return FALSE;
}

void SP_XfigDCImpl::DoCrossHair (wxCoord x, wxCoord y)
{
  SP_LOGDEBUG(wxString::Format(wxT("DoCrossHair %d %d"),
				 x, y));
  wxFAIL_MSG( wxT("SP_XfigDCImpl::CrossHair not implemented.") );
}


void SP_XfigDCImpl::SetLogicalFunction (wxRasterOperationMode function)
{
  SP_LOGDEBUG(wxT("SetLogicalFunction"));
  wxFAIL_MSG( wxT("SP_XfigDCImpl::SetLogicalFunction not implemented.") );
}


void SP_XfigDCImpl::DoDrawIcon( const wxIcon& icon, wxCoord x, wxCoord y )
{
  SP_LOGDEBUG(wxString::Format(wxT("DoDrawIcon %d %d"),
				 x, y));
  wxFAIL_MSG( wxT("SP_XfigDCImpl::DoDrawIcon not implemented.") );
}

void SP_XfigDCImpl::DoDrawBitmap( const wxBitmap& bitmap,
			      wxCoord x, wxCoord y, bool useMask )
{
  SP_LOGDEBUG(wxString::Format(wxT("DoDrawBitmap %d %d"),
				 x, y));
  wxFAIL_MSG( wxT("SP_XfigDCImpl::DoDrawBitmap not implemented.") );
}

void SP_XfigDCImpl::DoDrawSpline( const wxList *points )
{
  SP_LOGDEBUG(wxT("DoDrawSpline"));
  wxFAIL_MSG( wxT("SP_XfigDCImpl::DoDrawSpline not implemented.") );
}


void SP_XfigDCImpl::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
  SP_LOGDEBUG(wxString::Format(wxT("SetAxisOrientation %d %d"),
				 xLeftRight, yBottomUp));
  wxFAIL_MSG( wxT("SP_XfigDCImpl::SetAxisOrientation not implemented.") );

}
#else
// Xfig objects
enum {
  XFIG_COLOR = 0, // Color pseudo-object
  XFIG_ELLIPSE,   // Ellipse which is a generalization of circle.
  XFIG_POLYLINE,  // Polyline which includes polygon and box
  XFIG_SPLINE,    // Spline includes closed/open approximated/interpolated/x-spline spline.
  XFIG_TEXT,      // Text
  XFIG_ARC,       // Arc
  XFIG_COMPOUND  // Compound object which is composed of one or more objects.
};

// line stile
enum {
  XFIG_LSTYLE_DEFAULT = -1, // -1 = Default
  XFIG_LSTYLE_SOLID,  //       0 = Solid
  XFIG_LSTYLE_DASHED, //       1 = Dashed
  XFIG_LSTYLE_DOTTED, //       2 = Dotted
  XFIG_LSTYLE_DASH_DOTTED, //       3 = Dash-dotted
  XFIG_LSTYLE_DASH_DOUBLE_DOTTED, //    4 = Dash-double-dotted
  XFIG_LSTYLE_DASH_TRIPLE_DOTTED //   5 = Dash-triple-dotted
};

// arrows
enum {
  XFIG_ARROW_STICK = 0,
  XFIG_ARROW_CTRIANGLE,
  XFIG_ARROW_INDENTED_BUTT,
  XFIG_ARROW_POINTED_BUTT
};


enum {
  XFIG_ARROW_HOLLOW_STYLE = 0, // actually filled with white
  XFIG_ARROW_FILLED_STYLE
};

enum {
  XFIG_COLOR_DEFAULT = -1, // = Default
  XFIG_COLOR_BLACK, // 0 = Black
  XFIG_COLOR_BLUE, //  1 = Blue
  XFIG_COLOR_GREEN, // 2 = Green
  XFIG_COLOR_CYAN, //  3 = Cyan
  XFIG_COLOR_RED,  //  4 = Red
  XFIG_COLOR_MAGENTA, // 5 = Magenta
  XFIG_COLOR_YELLOW, //  6 = Yellow
  XFIG_COLOR_WHITE, //   7 = White
};

//-------------------------------------------------------------------------------
// SP_XfigDC
//-------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(SP_XfigDC, wxDC)

//-------------------------------------------------------------------------------
int SP_XfigDC::GrayFromColor(const wxColour& p_c)
{
  int res = (int) (p_c.Green() * 0.5 + p_c.Red() * 0.25 + p_c.Blue() * 0.25);
//   SP_LOGDEBUG(wxString::Format(wxT("GrayFromColor: res=%d (%d, %d, %d)"), res,
// 				 p_c.Red(), p_c.Green(), p_c.Blue()));
  return res;
}

int SP_XfigDC::GetXfigFill()
{

  int res;
  switch (m_brush.GetStyle()) {
  case wxTRANSPARENT: 	//Transparent (no fill).
    res = -1;
    break;
  case wxSOLID:  // Solid
	  if (m_brush.GetColour() == *wxWHITE){
		  //white color is set to None Fill,
		  //because the black token on the places will disappear if they are white filled
		  res= -1;
	  } else if (m_brush.GetColour() == *wxBLACK) {
				res = 20;  // Black
			} else {
				int gr = GrayFromColor(m_brush.GetColour());
				res = 1 + (int) (gr / 13.4);
				}  //endif

//     SP_LOGDEBUG(wxString::Format(wxT("GetXfigFill: res=%d"), res));
    break;
  case wxBDIAGONAL_HATCH: // 	Backward diagonal hatch.
    res = 41; // 30 degree left diagonal pattern
    break;
  case wxCROSSDIAG_HATCH:  // 	Cross-diagonal hatch.
    res = 43;  // 30 degree crosshatch
    break;
  case wxFDIAGONAL_HATCH: // 	Forward diagonal hatch.
    res = 46; // 45 degree crosshatch
    break;
  case wxCROSS_HATCH: 	// Cross hatch.
    res = 51; // crosshatch
    break;
  case wxHORIZONTAL_HATCH: // 	Horizontal hatch.
    res = 49; //  horizontal lines
    break;
  case wxVERTICAL_HATCH: //	Vertical hatch.
    res = 50;  //vertical lines
    break;
  default:
    res = -1; // not filled
    SP_LOGDEBUG(wxString::Format(wxT("GetXfigFill - unsupported fill %d, set to %d"),
				   m_brush.GetStyle(), res));
    break;
  }
  return res;
}

int SP_XfigDC::GetXfigJoin()
{
  int res = -1;
  switch (m_pen.GetJoin()) {
  case wxJOIN_BEVEL:
    res = 2;
    break;
  case wxJOIN_ROUND:
    res = 1;
    break;
  case wxJOIN_MITER:
    res = 0;
    break;
  }
  return res;
}

int SP_XfigDC::GetXfigLStyle()
{
  int res;
  switch (m_pen.GetStyle()) {
  case wxSOLID:  // Solid
    res = XFIG_LSTYLE_SOLID;
    break;
  case wxDOT:
    res = XFIG_LSTYLE_DOTTED;
    break;
  case wxSHORT_DASH:
  case wxLONG_DASH:
    res = XFIG_LSTYLE_DASHED;
    break;
  case wxDOT_DASH:
    res = XFIG_LSTYLE_DASH_DOTTED;
    break;
  default:
    res = -1; // default
    SP_LOGDEBUG(wxString::Format(wxT("GetXfigLStyle - unsupported style %d, set to %d"),
				   m_brush.GetStyle(), res));
    break;
  }
  return res;
}


int SP_XfigDC::GetXfigLineWidth()
{
  return m_pen.GetWidth();
}


int SP_XfigDC::GetXfigPColor()
{
  if (m_pen.GetColour() == *wxWHITE) return XFIG_COLOR_WHITE;
  return XFIG_COLOR_DEFAULT;  // TODO
}


int SP_XfigDC::GetXfigFColor()
{
  if (m_brush.GetStyle() != wxTRANSPARENT &&
      m_brush.GetStyle() != wxSOLID) return XFIG_COLOR_WHITE;

  if (m_brush.GetColour() == *wxWHITE) return XFIG_COLOR_WHITE;
  if (m_brush.GetColour() == *wxBLACK) return XFIG_COLOR_BLACK;
  return XFIG_COLOR_WHITE;
}

int SP_XfigDC::GetXfigLCap()
{
  int res = -1;
  switch (m_pen.GetCap()) {
  case wxCAP_ROUND:
    res = 1;
    break;
  case wxCAP_PROJECTING:
    res = 2;
    break;
  case wxCAP_BUTT:
    res = 0;
    break;
  }
  return res;
}

// latex fonts, font_flags bit 2 should be 0
// TODO: Postscript fonts support
int SP_XfigDC::GetXfigFont()
{
  int res;
  switch (m_font.GetFamily()) {
  case wxDEFAULT:
    res = 0;  // default
    break;
  case wxSWISS:  // A sans-serif font.
    res = 4;    // Sans Serif
    break;
  case wxMODERN:
    res = 5;
    break;     // Typewriter
  case wxROMAN:
    res = 1;   // Roman
    break;
  default:
    res = 0;
    SP_LOGDEBUG(wxString::Format(wxT("GetXfigFont - unsupported family %d, set to %d"),
				   m_font.GetFamily(), res));
    break;
  }

  if (m_font.GetStyle() == wxITALIC) {
    res = 3; // italic
    SP_LOGDEBUG(wxT("Font set to italic (3)"));
  }

  if (m_font.GetWeight() == wxBOLD) {
    res = 2; // bold
    SP_LOGDEBUG(wxT("Font set to bold (2)"));
  }

  return res;
}

//-------------------------------------------------------------------------------

void
SP_XfigDC::Init ()
{
  m_pstream = (FILE*) NULL;

  m_underlinePosition = 0.0;
  m_underlineThickness = 0.0;

  m_signX =  1;  // default x-axis left to right
  m_signY = 1;  // default y-axis top up -> bottom down

  m_scale = 1;
  m_resolution = DEF_RESOLUTION;
}


SP_XfigDC::SP_XfigDC ()
{
  Init();
}

SP_XfigDC::SP_XfigDC (const wxPrintData& printData)
{
  Init();

  m_printData = printData;
  m_ok = TRUE;
#if wxMAC_USE_CORE_GRAPHICS
	// we need at least a measuring context because people start measuring before a page
	// gets printed at all
	SetGraphicsContext( wxGraphicsContext::Create());
#endif
}

SP_XfigDC::~SP_XfigDC ()
{
  if (m_pstream) {
    fclose( m_pstream );
    m_pstream = (FILE*) NULL;
  }
}

//------------------------------------------------------------------------------

bool SP_XfigDC::StartDoc( const wxString& message )
{
  //  SP_LOGDEBUG(wxString::Format(wxT("StartDoc %s"), message.c_str()));
  wxCHECK_MSG( m_ok, FALSE, wxT("invalid xfig dc") );

  if (m_printData.GetFilename() == wxT("")) {
    wxString filename = wxGetTempFileName( wxT("xfig") );
    SP_LOGDEBUG(wxString::Format(wxT("writing to temporary file %s"),
		  filename.c_str()));
    m_printData.SetFilename(filename);
  }

  m_pstream = wxFopen( m_printData.GetFilename().c_str(), wxT("w+") );

  if (!m_pstream) {
    SP_LOGERROR( _("Cannot open file for Xfig output!"));
    m_ok = FALSE;
    return FALSE;
  }

  m_ok = TRUE;

  wxFprintf (m_pstream, wxT("#FIG 3.2\n"));
  if (m_printData.GetOrientation() == wxLANDSCAPE) {
    wxFprintf(m_pstream, wxT("Landscape\n"));
  } else {
    wxFprintf(m_pstream, wxT("Portrait\n"));
  }

  // other possibility wxT("Center")
  wxFprintf(m_pstream, wxT("Flush Left\n"));

  // units
  wxFprintf(m_pstream, wxT("Metric\n"));

  wxString paper = wxT("A4");
  switch (m_printData.GetPaperId())  {
  case wxPAPER_LETTER:
    paper = wxT("Letter"); // Letter: paper ""; 8 1/2 by 11 inches
    break;
  case wxPAPER_LEGAL: // Legal, 8 1/2 by 14 inches
    paper = wxT("Legal");
    break;
  case wxPAPER_A4: // A4 Sheet, 210 by 297 millimeters
    paper = wxT("A4");
    break;
  case wxPAPER_TABLOID: // Tabloid, 11 by 17 inches
    paper = wxT("Tabloid");
    break;
  case wxPAPER_LEDGER: // Ledger, 17 by 11 inches
    paper = wxT("Ledger");
    break;
  case wxPAPER_A3: // A3 sheet, 297 by 420 millimeters
    paper = wxT("A3");
    break;
  case wxPAPER_A2:
    paper = wxT("A2");
    break;
  case wxPAPER_B5: // B5 sheet, 182-by-257-millimeter paper
    paper = wxT("B5");
    break;
  default:
    paper = wxT("A4");
    SP_LOGDEBUG(wxT("Unsupported paper size, size is set to A4"));
    break;
  }

  wxFprintf(m_pstream, wxT("%s\n"), paper.c_str());

  // magnification
  wxFprintf(m_pstream, wxT("100.00\n"));

  // multiple-page?
  wxFprintf(m_pstream, wxT("Single\n"));

  // transparent color for GIF export. -3=background, -2=None, -1=Default
  wxFprintf(m_pstream, wxT("-2\n"));

  //  resolution coord_system (2: upper left)
  wxFprintf(m_pstream, wxString::Format(wxT("%d 2\n"), GetResolution()));

  return TRUE;
}

void SP_XfigDC::EndDoc ()
{
  //  SP_LOGDEBUG(wxT("EndDoc"));
  wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );

  fclose( m_pstream );
  m_pstream = (FILE *) NULL;
}

//------------------------------------------------------------------------------

void SP_XfigDC::DoDrawEllipse (wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
  //  SP_LOGDEBUG(wxString::Format(wxT("DoDrawEllipse %d %d %d %d"),
  //				 x, y,
  //			 width, height));
  wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );

  if (m_brush.GetStyle() != wxTRANSPARENT ||
      m_pen.GetStyle() != wxTRANSPARENT) {
    wxFprintf (m_pstream, wxT("%d"), XFIG_ELLIPSE);
    if (width == height) {
      wxFprintf (m_pstream, wxT(" 2")); // circle defined by radius
    } else {
      wxFprintf (m_pstream, wxT(" 1")); // ellipse defined by radius
    }

    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());

    wxFprintf (m_pstream, wxT(" %d"), GetXfigLineWidth()); // thickness
    wxFprintf (m_pstream, wxT(" %d"), GetXfigPColor());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFColor());
    wxFprintf (m_pstream, wxT(" 1")); // depth
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFill());
    wxFprintf (m_pstream, wxT(" -1.0")); //  TODO: float style_val (1/80 inch) ???
    wxFprintf (m_pstream, wxT(" 1")); //  direction
    wxFprintf (m_pstream, wxT(" 0.0")); //  radians, the angle of the x-axis

    int tX = (int) (x * m_scale);
    int tY = (int) (y * m_scale);
    int radiusX = ((int) (width * m_scale)) / 2;
    int radiusY = ((int) (height * m_scale)) / 2;

    wxFprintf (m_pstream, wxT(" %d %d"), tX + radiusX,
	     tY + radiusX); //  center_x, center_y
    wxFprintf (m_pstream, wxT(" %d %d"), radiusX, radiusY); //  radius_x, radius_y
    wxFprintf (m_pstream, wxT(" %d %d"), tX + width,
	     tY + height); //  1st point entered
    wxFprintf (m_pstream, wxT(" %d %d\n"), tX + width,
	     tY + height); //  last point entered
  }
}


void SP_XfigDC::DoDrawArc (wxCoord x1, wxCoord y1, wxCoord x2,
			   wxCoord y2, wxCoord xc, wxCoord yc)
{
//  SP_LOGDEBUG(wxString::Format(wxT("DoDrawArc %d %d %d %d %d %d"),
//				 x1, y1, x2,
//				 y2, xc,  yc));
  wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );

  if (m_brush.GetStyle () != wxTRANSPARENT ||
      m_pen.GetStyle () != wxTRANSPARENT) {
    wxFprintf (m_pstream, wxT("%d"), XFIG_ARC);
    wxFprintf (m_pstream, wxT(" 1")); // open ended arc
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLineWidth()); // thickness
    wxFprintf (m_pstream, wxT(" %d"), GetXfigPColor());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFColor());
    wxFprintf (m_pstream, wxT(" 1")); // depth
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFill());
    wxFprintf (m_pstream, wxT(" -1.0")); //  TODO: float style_val (1/80 inch) ???
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLCap()); //  cap_style
    wxFprintf (m_pstream, wxT(" 0")); //  direction
    wxFprintf (m_pstream, wxT(" 0")); //  forward_arrow
    wxFprintf (m_pstream, wxT(" 0")); //  backward_arrow
    wxFprintf (m_pstream, wxT(" %d %d"),
	     (int) (xc * m_scale), (int) (yc *m_scale)); // center of the arc
    wxFprintf (m_pstream, wxT(" %d %d"), (int) (x1 * m_scale),
	     (int) (y1 * m_scale)); //  1st point entered

    wxFprintf (m_pstream, wxT(" %d %d"), (int) (x2 * m_scale),
	     (int) (y2 * m_scale)); //  2nd point entered

    wxFprintf (m_pstream, wxT(" %d %d\n"), (int) (x2 * m_scale),
	     (int) (y2 * m_scale)); //  last point entered
  }
}



void SP_XfigDC::DoDrawText( const wxString& text, wxCoord x, wxCoord y )
{
//  SP_LOGDEBUG(wxString::Format(wxT("DoDrawText %s %d %d"), text.c_str(),
//				 x, y));

  DoDrawRotatedText (text, x, y, 0);

}

void SP_XfigDC::DoDrawRotatedText( const wxString& text, wxCoord x,
				   wxCoord y, double angle )
{
//  SP_LOGDEBUG(wxString::Format(wxT("DoDrawRotatedText %s %d %d %f"),
//				 text.c_str(), x, y, angle));
  if (m_pen.GetStyle () != wxTRANSPARENT) {

    wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );

    wxFprintf (m_pstream, wxT("%d"), XFIG_TEXT);
    wxFprintf (m_pstream, wxT(" 0")); // Left justified
    wxFprintf (m_pstream, wxT(" %d"), GetXfigPColor());
    wxFprintf (m_pstream, wxT(" 0")); // depth
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFont());
    wxFprintf (m_pstream, wxT(" %d"), GetCharHeight());
    wxFprintf (m_pstream, wxT(" %f"), angle);
    wxFprintf (m_pstream, wxT(" 2")); // font_flags Special text (for LaTeX)
    wxFprintf (m_pstream, wxT(" -1.0")); //  height
    wxFprintf (m_pstream, wxT(" -1.0")); //  length

    int tX = (int) ((x + GetCharWidth() / 2.0) * m_scale);
    int tY = (int) ((y + GetCharHeight() / 2.0)  * m_scale);

    wxFprintf (m_pstream, wxT(" %d %d"), tX, tY); //  center_x, center_y
    wxFprintf (m_pstream, wxT(" %s\\001\n"), text.c_str());
  }
}


void SP_XfigDC::DoDrawLines (int n, wxPoint points[],
			     wxCoord xoffset, wxCoord yoffset)
{
//     SP_LOGDEBUG(wxString::Format(wxT("DoDrawLines %d %d %d"),
// 		n, xoffset, yoffset));
    wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );

  if (m_pen.GetStyle () != wxTRANSPARENT) {
    wxFprintf (m_pstream, wxT("%d"), XFIG_POLYLINE);
    wxFprintf (m_pstream, wxT(" 1")); // polyline
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLineWidth()); // thickness
    wxFprintf (m_pstream, wxT(" %d"), GetXfigPColor());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFColor());
    wxFprintf (m_pstream, wxT(" 1")); // depth
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" -1"));  // area_fill  -1 = no fill
    wxFprintf (m_pstream, wxT(" -1.0")); //  TODO: float style_val (1/80 inch) ???

    wxFprintf (m_pstream, wxT(" %d"), GetXfigJoin());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLCap());
    wxFprintf (m_pstream, wxT(" 0")); //  radius of arc-boxes
    wxFprintf (m_pstream, wxT(" 0")); //  forward_arrow
    wxFprintf (m_pstream, wxT(" 0")); //  backward_arrow
    wxFprintf (m_pstream, wxT(" %d\n"), n); //  npoints

    int tX, tY;
    for (int i = 0; i < n; i++) {
      tX = (int) (points[i].x * m_scale + xoffset * m_scale);
      tY = (int) (points[i].y * m_scale + yoffset * m_scale);
      wxFprintf (m_pstream, wxT("%d %d\n"), tX, tY);
    }
  }
}


void SP_XfigDC::DoDrawPoint(wxCoord x, wxCoord y)
{
  SP_LOGDEBUG(wxString::Format(wxT("DoDrawPoint %d %d"),
				  x, y));
  wxPoint tmp[] = {wxPoint(x,y), wxPoint(x, y)};
  DoDrawLines(2, tmp);
}

void SP_XfigDC::DoDrawLine (wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
  SP_LOGDEBUG(wxString::Format(wxT("DoDrawLine %d %d %d %d"),
				 x1, y1,
				 x2, y2));

  wxPoint tmp[] = {wxPoint(x1,y1), wxPoint(x2, y2)};
  DoDrawLines(2, tmp);
}


void SP_XfigDC::DoDrawPolygon (int n, wxPoint points[],
			       wxCoord xoffset, wxCoord yoffset,
			       int fillStyle)
{
//    SP_LOGDEBUG(wxString::Format(wxT("DoDrawPolygon %d, style %d tr:%d"),
//  		n, m_pen.GetStyle(), m_pen.GetStyle() == wxTRANSPARENT ));
  wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );


  if (m_pen.GetStyle() != wxTRANSPARENT &&
      m_pen.GetColour() != *wxWHITE) {
    wxFprintf (m_pstream, wxT("%d"), XFIG_POLYLINE);
    wxFprintf (m_pstream, wxT(" 3")); // polygon
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLineWidth()); // thickness
    wxFprintf (m_pstream, wxT(" %d"), GetXfigPColor());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFColor());
    wxFprintf (m_pstream, wxT(" 1")); // depth
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFill());
    wxFprintf (m_pstream, wxT(" -1.0")); //  TODO: float style_val (1/80 inch) ???

    wxFprintf (m_pstream, wxT(" %d"), GetXfigJoin());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLCap());
    wxFprintf (m_pstream, wxT(" 0")); //  radius of arc-boxes
    wxFprintf (m_pstream, wxT(" 0")); //  forward_arrow
    wxFprintf (m_pstream, wxT(" 0")); //  backward_arrow
    wxFprintf (m_pstream, wxT(" %d\n"), n); //  npoints

    int tX, tY;
    for (int i = 0; i < n; i++) {
      tX = (int) (points[i].x * m_scale + xoffset * m_scale);
      tY = (int) (points[i].y * m_scale + yoffset * m_scale);
      wxFprintf (m_pstream, wxT("%d %d\n"), tX, tY);
    }
  }
}



void SP_XfigDC::DoDrawRectangle (wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
//    SP_LOGDEBUG(wxString::Format(wxT("DoDrawRectangle %d %d %d %d"),
//  				 x, y,
//  				 width, height));

    wxPoint tmp[] = {wxPoint(x,y), wxPoint(x + width, y),
		    wxPoint(x+width,y+height), wxPoint(x, y+ height),
		    wxPoint(x, y)};
    DoDrawPolygon(5, tmp);

// wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );
//   if (m_pen.GetStyle () != wxTRANSPARENT) {
//     wxFprintf (m_pstream, wxT("%d"), XFIG_POLYLINE);
//     wxFprintf (m_pstream, wxT(" 2")); // box
//     wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
//     wxFprintf (m_pstream, wxT(" 1")); // TODO thickness
//     wxFprintf (m_pstream, wxT(" %d"), GetXfigPColor());
//     wxFprintf (m_pstream, wxT(" %d"), GetXfigFColor());
//     wxFprintf (m_pstream, wxT(" 1")); // depth
//     wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
//     wxFprintf (m_pstream, wxT(" %d"), GetXfigFill());
//     wxFprintf (m_pstream, wxT(" -1.0")); //  TODO: float style_val (1/80 inch) ???

//     wxFprintf (m_pstream, wxT(" %d"), GetXfigJoin());
//     wxFprintf (m_pstream, wxT(" %d"), GetXfigLCap());
//     wxFprintf (m_pstream, wxT(" 0")); //  radius of arc-boxes
//     wxFprintf (m_pstream, wxT(" 0")); //  forward_arrow
//     wxFprintf (m_pstream, wxT(" 0")); //  backward_arrow
//     wxFprintf (m_pstream, wxT(" 5\n")); //  npoints

//     int tX = (int) (x * m_scale);
//     int tY = (int) (y * m_scale);
//     int tW = (int) (width * m_scale);
//     int tH = (int) (height * m_scale);
//     wxFprintf (m_pstream, wxT("%d %d\n%d %d\n%d %d\n%d %d\n%d %d\n"),
// 	     tX, tY,
// 	     tX + tW, tY,
// 	     tX+tW, tY+tH,
// 	     tX, tY+tH,
// 	     tX, tY);
//  }
}


void SP_XfigDC::DoDrawRoundedRectangle (wxCoord x, wxCoord y,
					wxCoord width, wxCoord height,
					double radius)
{
//   SP_LOGDEBUG(wxString::Format(wxT("DoDrawRoundedRectangle %d %d %d %d %f"),
// 				 x, y,
// 				 width, height,
// 				 radius));

  wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );


  if (m_pen.GetStyle () != wxTRANSPARENT) {
    wxFprintf (m_pstream, wxT("%d"), XFIG_POLYLINE);
    wxFprintf (m_pstream, wxT(" 4")); // polygon
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLineWidth()); // thickness
    wxFprintf (m_pstream, wxT(" %d"), GetXfigPColor());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFColor());
    wxFprintf (m_pstream, wxT(" 1")); // depth
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLStyle());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigFill());
    wxFprintf (m_pstream, wxT(" -1.0")); //  TODO: float style_val (1/80 inch) ???

    wxFprintf (m_pstream, wxT(" %d"), GetXfigJoin());
    wxFprintf (m_pstream, wxT(" %d"), GetXfigLCap());
    wxFprintf (m_pstream, wxT(" %d"), (int) radius); //  radius of arc-boxes
    wxFprintf (m_pstream, wxT(" 0")); //  forward_arrow
    wxFprintf (m_pstream, wxT(" 0")); //  backward_arrow
    wxFprintf (m_pstream, wxT(" 5\n")); //  npoints

    int tX = (int) (x * m_scale);
    int tY = (int) (y * m_scale);
    int tW = (int) (width * m_scale);
    int tH = (int) (height * m_scale);
    wxFprintf (m_pstream, wxT("%d %d\n%d %d\n%d %d\n%d %d\n%d %d\n"),
	     tX, tY,
	     tX + tW, tY,
	     tX+tW, tY+tH,
	     tX, tY+tH,
	     tX, tY);
  }
}



//------------------------------------------------------------------------------
void SP_XfigDC::SetBackground (const wxBrush& brush)
{
  SP_LOGDEBUG(wxT("SetBackground"));
  m_backgroundBrush = brush;
}

void SP_XfigDC::SetResolution(int ppi)
{
  m_resolution = ppi;
  m_scale = (float)ppi / DEF_RESOLUTION;
}

int SP_XfigDC::GetResolution()
{
  return m_resolution;
}


void SP_XfigDC::SetFont( const wxFont& font )
{
//   SP_LOGDEBUG(wxT("SetFont"));
  m_font = font;
}

void SP_XfigDC::SetPen( const wxPen& pen )
{
//   SP_LOGDEBUG(wxT("SetPen"));
  m_pen = pen;
}

void SP_XfigDC::SetBrush( const wxBrush& brush )
{
//   SP_LOGDEBUG(wxT("SetBrush"));
  m_brush = brush;
}

bool SP_XfigDC::Ok() const
{
  return m_ok;
}


// from wxPostscriptDC, not tested
//------------------------------------------------------------------------------

void SP_XfigDC::SetDeviceOrigin( wxCoord x, wxCoord y )
{
  SP_LOGDEBUG(wxString::Format(wxT("SetDeviceOrigin %d %d"),
				 x, y));
  wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );

  int h = 0;
  int w = 0;
  GetSize( &w, &h );

  wxDC::SetDeviceOrigin( x, h-y );
}

void SP_XfigDC::DoGetSize(int* width, int* height) const
{
  //  SP_LOGDEBUG(wxT("DoGetSize"));
  wxPaperSize id = m_printData.GetPaperId();

  wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(id);

  if (!paper) paper = wxThePrintPaperDatabase->FindPaperType(wxPAPER_A4);

  int w = 595;
  int h = 842;
  if (paper) {
    w = paper->GetSizeDeviceUnits().x;
    h = paper->GetSizeDeviceUnits().y;
  }

  if (m_printData.GetOrientation() == wxLANDSCAPE) {
    int tmp = w;
    w = h;
    h = tmp;
  }

  if (width) *width = (int)(w * m_scale);
  if (height) *height = (int)(h * m_scale);
}

void SP_XfigDC::DoGetSizeMM(int *width, int *height) const
{
  //  SP_LOGDEBUG(wxT("DoGetSizeMM"));
  wxPaperSize id = m_printData.GetPaperId();

  wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(id);

  if (!paper) paper = wxThePrintPaperDatabase->FindPaperType(wxPAPER_A4);

  int w = 210;
  int h = 297;

  if (paper) {
        w = paper->GetWidth() / 10;
        h = paper->GetHeight() / 10;
  }

  if (m_printData.GetOrientation() == wxLANDSCAPE) {
    int tmp = w;
    w = h;
    h = tmp;
  }

  if (width) *width = w;
  if (height) *height = h;
}

wxCoord SP_XfigDC::GetCharHeight() const
{
  //  SP_LOGDEBUG(wxT("GetCharHeight"));
  if (m_font.Ok())
    return m_font.GetPointSize()+1;
  else
    return 12;
}

wxCoord SP_XfigDC::GetCharWidth() const
{
  //  SP_LOGDEBUG(wxT("GetCharWidth"));
  // Chris Breeze: reasonable approximation using wxMODERN/Courier
  return (wxCoord) (GetCharHeight() * DEF_RESOLUTION / 120.0);
}

// Resolution in pixels per logical inch
wxSize SP_XfigDC::GetPPI(void) const
{
  //  SP_LOGDEBUG(wxT("GetPPI"));
  return wxSize(m_resolution, m_resolution);
}

// do nothing
//------------------------------------------------------------------------------

void SP_XfigDC::StartPage()
{
  SP_LOGDEBUG(wxT("StartPage"));
}

void SP_XfigDC::EndPage ()
{
  SP_LOGDEBUG(wxT("EndPage"));
}


void SP_XfigDC::DoSetClippingRegion (wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
//   SP_LOGDEBUG(wxString::Format(wxT("DoSetClippingRegion %d %d %d %d"),
// 				 x, y, w, h));
}


void SP_XfigDC::DestroyClippingRegion()
{
  //  SP_LOGDEBUG(wxT("DestroyClippingRegion"));
}


// not implemented
//------------------------------------------------------------------------------

bool SP_XfigDC::DoBlit(wxCoord xdest, wxCoord ydest,
		       wxCoord fwidth, wxCoord fheight,
		       wxDC *source,
		       wxCoord xsrc, wxCoord ysrc,
		       int rop, bool useMask,
		       wxCoord xsrcMask, wxCoord ysrcMask )
{
  SP_LOGDEBUG(wxT("DoBlit"));
  wxFAIL_MSG( wxT("SP_XfigDC::DoBlit not implemented.") );
  return FALSE;
}


void SP_XfigDC::DoGetTextExtent(const wxString& string,
                                     wxCoord *x, wxCoord *y,
                                     wxCoord *descent, wxCoord *externalLeading,
                                     wxFont *theFont ) const
{
  SP_LOGDEBUG(wxT("DoGetTextExtent"));
  wxFAIL_MSG( wxT("SP_XfigDC::DoGetTextExtent not implemented.") );
}


void SP_XfigDC::Clear()
{
  SP_LOGDEBUG(wxT("Clear"));
  wxFAIL_MSG( wxT("SP_XfigDC::Clear not implemented.") );
}


void SP_XfigDC::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w,
				  wxCoord h, double sa, double ea)
{
  SP_LOGDEBUG(wxString::Format(wxT("DoDrawEllipticArc %d %d %d %d %f %f"),
				 x, y, w,
				 h, sa,  ea));
  wxFAIL_MSG( wxT("SP_XfigDC::FloodFill not implemented.") );
}


bool SP_XfigDC::DoFloodFill (wxCoord x, wxCoord y,
			     const wxColour &col, int style)
{
  SP_LOGDEBUG(wxString::Format(wxT("DoFloodFill %d %d, color: %d %d %d, style %d"),
				 x, y,
				 col.Red(), col.Green(), col.Blue(), style));
  wxFAIL_MSG( wxT("SP_XfigDC::FloodFill not implemented.") );
  return FALSE;
}

bool SP_XfigDC::DoGetPixel (wxCoord x, wxCoord y,
			    wxColour * col) const
{
  SP_LOGDEBUG(wxString::Format(wxT("DoGetPixel %d %d, color: %d %d %d"),
				 x, y,
		col->Red(), col->Green(), col->Blue()));
  wxFAIL_MSG( wxT("SP_XfigDC::GetPixel not implemented.") );
  return FALSE;
}

void SP_XfigDC::DoCrossHair (wxCoord x, wxCoord y)
{
  SP_LOGDEBUG(wxString::Format(wxT("DoCrossHair %d %d"),
				 x, y));
  wxFAIL_MSG( wxT("SP_XfigDC::CrossHair not implemented.") );
}


void SP_XfigDC::SetLogicalFunction (int function)
{
  SP_LOGDEBUG(wxT("SetLogicalFunction"));
  wxFAIL_MSG( wxT("SP_XfigDC::SetLogicalFunction not implemented.") );
}


void SP_XfigDC::DoDrawIcon( const wxIcon& icon, wxCoord x, wxCoord y )
{
  SP_LOGDEBUG(wxString::Format(wxT("DoDrawIcon %d %d"),
				 x, y));
  wxFAIL_MSG( wxT("SP_XfigDC::DoDrawIcon not implemented.") );
}

void SP_XfigDC::DoDrawBitmap( const wxBitmap& bitmap,
			      wxCoord x, wxCoord y, bool useMask )
{
  SP_LOGDEBUG(wxString::Format(wxT("DoDrawBitmap %d %d"),
				 x, y));
  wxFAIL_MSG( wxT("SP_XfigDC::DoDrawBitmap not implemented.") );
}

void SP_XfigDC::DoDrawSpline( wxList *points )
{
  SP_LOGDEBUG(wxT("DoDrawSpline"));
  wxFAIL_MSG( wxT("SP_XfigDC::DoDrawSpline not implemented.") );
}


void SP_XfigDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
  SP_LOGDEBUG(wxString::Format(wxT("SetAxisOrientation %d %d"),
				 xLeftRight, yBottomUp));
  wxFAIL_MSG( wxT("SP_XfigDC::SetAxisOrientation not implemented.") );

}
#endif
