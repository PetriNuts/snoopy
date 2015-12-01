// $Author: md $
// $Version: $
// $Date: 2005/08/20 $
// Short Description: dc for export to framemaker format *.mif
//
//////////////////////////////////////////////////////////////////////
// TODO:	- scaling (page fitting)
//////////////////////////////////////////////////////////////////////

#include "export/SP_EXP_MIFDC.h"
#include "sp_utilities.h"
#include <wx/paper.h>
#include <wx/filename.h>
#include "snoopy.h"

#if wxABI_VERSION > 30000

// MIF objects
enum {
  MIF_ELLIPSE = 0,	// ellipse including circle
	MIF_POLYGON,			// polygon including rectangle and square
	MIF_POLYLINE,			// polyline including single line and dot
	MIF_ARC,					// round and elliptic arc
	MIF_TEXT					// text
};


//-------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(SP_EXP_MIFDC, wxDC)

SP_EXP_MIFDC::SP_EXP_MIFDC()
 : wxDC(new SP_EXP_MIFDCImpl(this))
{
}

SP_EXP_MIFDC::SP_EXP_MIFDC(const wxPrintData& printData)
 : wxDC(new SP_EXP_MIFDCImpl(this, printData))
{
}

//-------------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(SP_EXP_MIFDCImpl, wxDCImpl)

SP_EXP_MIFDCImpl::SP_EXP_MIFDCImpl (SP_EXP_MIFDC *owner)
	: wxDCImpl(owner)
{
	Init();
}

SP_EXP_MIFDCImpl::SP_EXP_MIFDCImpl (SP_EXP_MIFDC *owner, const wxPrintData& printData)
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

SP_EXP_MIFDCImpl::SP_EXP_MIFDCImpl (wxPrinterDC *owner)
	: wxDCImpl(owner)
{
	Init();
}

SP_EXP_MIFDCImpl::SP_EXP_MIFDCImpl (wxPrinterDC *owner, const wxPrintData& printData)
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


void
SP_EXP_MIFDCImpl::Init ()
{
  m_pstream = (FILE*) NULL;

  m_signX =  1;  // default x-axis left to right
  m_signY = 1;  // default y-axis top up -> bottom down

  m_resolution = DEF_RESOLUTION;
  m_scale = 1;

	m_exportColors = SP_MESSAGEBOX(wxT("Do you want color export?"), wxT("Question"), wxYES_NO);
	if (m_exportColors == wxYES) {
		m_msColors[wxT("255,0,0")] = wxT("Red");
		m_msColors[wxT("128,0,0")] = wxT("Red_dark");
		m_msColors[wxT("255,128,128")] = wxT("Red_light");
		m_msColors[wxT("0,255,0")] = wxT("Green");
		m_msColors[wxT("0,128,0")] = wxT("Green_dark");
		m_msColors[wxT("128,255,128")] = wxT("Green_light");
		m_msColors[wxT("0,0,255")] = wxT("Blue");
		m_msColors[wxT("0,0,128")] = wxT("Blue_dark");
		m_msColors[wxT("128,128,255")] = wxT("Blue_light");
		m_msColors[wxT("0,255,255")] = wxT("Cyan");
		m_msColors[wxT("0,128,128")] = wxT("Cyan_dark");
		m_msColors[wxT("128,255,255")] = wxT("Cyan_light");
		m_msColors[wxT("0,128,255")] = wxT("Cyan_Blue");
		m_msColors[wxT("0,255,128")] = wxT("Cyan_Green");
		m_msColors[wxT("255,0,255")] = wxT("Magenta");
		m_msColors[wxT("128,0,128")] = wxT("Magenta_dark");
		m_msColors[wxT("255,128,255")] = wxT("Magenta_light");
		m_msColors[wxT("128,0,255")] = wxT("Magenta_Blue");
		m_msColors[wxT("255,0,128")] = wxT("Magenta_Red");
		m_msColors[wxT("255,255,0")] = wxT("Yellow");
		m_msColors[wxT("128,128,0")] = wxT("Yellow_dark");
		m_msColors[wxT("255,255,128")] = wxT("Yellow_light");
		m_msColors[wxT("128,255,0")] = wxT("Yellow_Green");
		m_msColors[wxT("255,128,0")] = wxT("Yellow_Red");
	}
}


SP_EXP_MIFDCImpl::~SP_EXP_MIFDCImpl ()
{
  if (m_pstream) {
    fclose( m_pstream );
    m_pstream = (FILE*) NULL;
  }
}

//------------------------------------------------------------------------------
bool SP_EXP_MIFDCImpl::StartDoc( const wxString& message )
{
  wxCHECK_MSG( m_ok, FALSE, wxT("invalid mif-dc") );

  if (m_printData.GetFilename() == wxT("")) {
    wxString filename = wxFileName::CreateTempFileName( wxT("mif") );
    SP_LOGDEBUG(wxString::Format(wxT("writing to temporary file %s"),
		  filename.c_str()));
    m_printData.SetFilename(filename);
  }

  m_pstream = wxFopen( m_printData.GetFilename().c_str(), wxT("w+") );

  if (!m_pstream) {
    SP_LOGERROR( _("Cannot open file for output!"));
    m_ok = FALSE;
    return FALSE;
  }

  m_ok = TRUE;

  wxFprintf(m_pstream, wxT("<MIFFile 6.00> # Created by SNOOPY\n"));

	//defining default colors
	if (m_exportColors == wxYES) {
		SP_MapString2String::iterator itC;

		wxFprintf(m_pstream, wxT(" <ColorCatalog\n"));
		for(itC = m_msColors.begin(); itC != m_msColors.end(); ++itC) {
			wxFprintf(m_pstream, wxT("\t<Color\n"));
			wxFprintf(m_pstream, wxT("\t <ColorTag `%s'>\n"), (*itC).second.c_str());
			wxFprintf(m_pstream, wxT("\t <ColorCyan %d>\n"), GetCyan((*itC).first));
			wxFprintf(m_pstream, wxT("\t <ColorMagenta %d>\n"), GetMagenta((*itC).first));
			wxFprintf(m_pstream, wxT("\t <ColorYellow %d>\n"), GetYellow((*itC).first));
			wxFprintf(m_pstream, wxT("\t># End of Color.\n"));
		}
		wxFprintf(m_pstream, wxT(" ># End of ColorCatalog.\n"));
	}

  return TRUE;
}


void SP_EXP_MIFDCImpl::EndDoc ()
{
  wxCHECK_RET( m_ok && m_pstream, wxT("invalid mif-dc") );

	wxFprintf(m_pstream, wxT("# End of MIFFile."));

  fclose( m_pstream );
  m_pstream = (FILE *) NULL;
}


int SP_EXP_MIFDCImpl::GetCyan(const wxString& sVal) {
	int red;
	wxSscanf(sVal.c_str(), wxT("%d"), &red);
	return (255-red)/2.55;
}


int SP_EXP_MIFDCImpl::GetMagenta(const wxString& sVal) {
	int green;
	wxSscanf(sVal.c_str(), wxT("%*d,%d"), &green);
	return (255-green)/2.55;
}


int SP_EXP_MIFDCImpl::GetYellow(const wxString& sVal) {
	int blue;
	wxSscanf(sVal.c_str(), wxT("%*d,%*d,%d"), &blue);
	return (255-blue)/2.55;
}


int SP_EXP_MIFDCImpl::GetFillStyle() {
	int style;

  switch (m_brush.GetStyle()) {
  case wxSOLID:
    if (m_brush.GetColour() == *wxWHITE) {
      style = 7;
    } else {
			style = 0;
    }
    break;
  case wxCROSS_HATCH: 	// Cross hatch.
    style = 4; // not supported by framemaker --> set to solid gray
		break;
  case wxVERTICAL_HATCH: //	Vertical hatch.
    style = 10;
    break;
  case wxHORIZONTAL_HATCH: // 	Horizontal hatch.
    style = 11;
		break;
  case wxCROSSDIAG_HATCH:  // 	Cross-diagonal hatch.
    style = 12;
    break;
  case wxFDIAGONAL_HATCH: // 	Forward diagonal (lb->rt) hatch.
    style = 13;
    break;
  case wxBDIAGONAL_HATCH: // 	Backward diagonal (lt->rb) hatch.
    style = 14;
    break;
  case wxTRANSPARENT: 	//Transparent
    style = 15;
    break;
  default:
    style = 15; // not filled
    SP_LOGDEBUG(wxString::Format(wxT("GetFillStyle: unsupported style %d, set to %d"),
				   m_brush.GetStyle(), style));
  }
  return style;

}


int SP_EXP_MIFDCImpl::GetPenColor() {

  if (m_pen.GetColour() == *wxWHITE) return 7;
  return 0;
}


int SP_EXP_MIFDCImpl::GetObTint() {
	float gray;
	//get shade of object fill-color
	//if there is none, get it from the pen color
	if ((m_brush.GetColour() == *wxWHITE) || (m_brush.GetStyle() == wxBRUSHSTYLE_TRANSPARENT)) {
		gray = (m_pen.GetColour().Green() + m_pen.GetColour().Red() + m_pen.GetColour().Blue()) / 3;
	} else {
		gray = (m_brush.GetColour().Green() + m_brush.GetColour().Red() + m_brush.GetColour().Blue()) / 3;
	}
	//transform gray-tone (0..255) to tint (100%..0%)
	gray = 100 - gray / 2.55;
	//tint percentage is multiplied by 100
	return int(gray * 100);
}


wxString SP_EXP_MIFDCImpl::GetObColor() {
	int red, green, blue;
	wxString rgb;

	if ((m_brush.GetColour() == *wxWHITE) || (m_brush.GetStyle() == wxBRUSHSTYLE_TRANSPARENT)) {
		red = Approx(m_pen.GetColour().Red());
		green = Approx(m_pen.GetColour().Green());
		blue = Approx(m_pen.GetColour().Blue());
	} else {
		red = Approx(m_brush.GetColour().Red());
		green = Approx(m_brush.GetColour().Green());
		blue = Approx(m_brush.GetColour().Blue());
	}

	rgb = wxString::Format(wxT("%d,%d,%d"), red, green, blue);
	SP_MapString2String::iterator l_itColors = m_msColors.find(rgb);
	if (l_itColors != m_msColors.end()) return l_itColors->second;
	return wxT("Black");
}

int SP_EXP_MIFDCImpl::Approx(int Val) {
	int result;

	//setting color value to the nearest multiple of 128
	if (Val <= 64) {
		result = 0;
	} else if (Val <= 192) {
		result = 128;
	} else {
		result = 255;
	}
	return result;
}

void SP_EXP_MIFDCImpl::WriteObjectProperties(int obj, double angle) {

	wxFprintf(m_pstream, wxT("\t<Pen 0>\n"));	//shades are achieved by 'ObTint'-value
	wxFprintf(m_pstream, wxT("\t<Fill %d>\n"), GetFillStyle());
	wxFprintf(m_pstream, wxT("\t<PenWidth %d pt>\n"), m_pen.GetWidth());

	wxString col = wxT("Black");
	if (m_exportColors == wxYES) col = GetObColor();
	wxFprintf(m_pstream, wxT("\t<ObColor `%s'>\n"), col.c_str());

	if (col.CmpNoCase(wxT("Black")) == 0) {
		wxFprintf(m_pstream, wxT("\t<ObTint %d>\n"), GetObTint());
	} else {
		wxFprintf(m_pstream, wxT("\t<ObTint 10000>\n"));
	}
	if (angle != 0) wxFprintf(m_pstream, wxT("\t<Angle %G>\n"), angle);	//default = 0
	wxFprintf(m_pstream, wxT("\t<DashedPattern\n"));
	switch(m_pen.GetStyle()) {
	case wxSOLID:
		wxFprintf(m_pstream, wxT("\t <DashedStyle Solid>\n\t>\n"));
		break;
	case wxDOT:
		wxFprintf(m_pstream, wxT("\t <DashedStyle Dashed>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 2.0 pt>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 4.0 pt>\n\t>\n"));
		break;
	case wxSHORT_DASH:
		wxFprintf(m_pstream, wxT("\t <DashedStyle Dashed>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 4.0 pt>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 4.0 pt>\n\t>\n"));
		break;
	case wxLONG_DASH:
		wxFprintf(m_pstream, wxT("\t <DashedStyle Dashed>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 8.0 pt>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 6.0 pt>\n\t>\n"));
		break;
	case wxDOT_DASH:
		wxFprintf(m_pstream, wxT("\t <DashedStyle Dashed>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 12.0 pt>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 6.0 pt>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 2.0 pt>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 6.0 pt>\n\t>\n"));
		break;
	default:
 		wxFprintf(m_pstream, wxT("\t <DashedStyle Solid>\n\t>\n")); // set to solid
		SP_LOGDEBUG(wxString::Format(wxT("WriteObjectProperties: unsupported pen style %d, set to wxSOLID"),
					 m_brush.GetStyle()));
	}

}


void SP_EXP_MIFDCImpl::WriteFontProperties() {

	wxFprintf(m_pstream, wxT("\t<Font\n"));
	wxFprintf(m_pstream, wxT("\t <FTag `\'>\n"));

	switch(m_font.GetFamily()) {
	case wxTELETYPE:
	case wxMODERN:
		wxFprintf(m_pstream, wxT("\t <FFamily `Courier New'>\n"));
		break;
	case wxROMAN:
		wxFprintf(m_pstream, wxT("\t <FFamily `Times New Roman'>\n"));
		break;
	case wxSCRIPT:
		// might not work properly if wxT("Brush Script") doesn't exist on the system
		wxFprintf(m_pstream, wxT("\t <FFamily `Brush Script'>\n"));
		break;
	case wxDEFAULT:
	case wxSWISS:
	default:
		wxFprintf(m_pstream, wxT("\t <FFamily `Arial'>\n"));
	}

	switch(m_font.GetStyle()) {
	case wxSLANT:
	case wxITALIC:
		wxFprintf(m_pstream, wxT("\t <FAngle `Italic'>\n"));
		break;
	case wxNORMAL:
	default:
		wxFprintf(m_pstream, wxT("\t <FAngle `Regular'>\n"));
	}

	switch(m_font.GetWeight()) {
	case wxBOLD:
		wxFprintf(m_pstream, wxT("\t <FWeight `Bold'>\n"));
		break;
	case wxLIGHT:
	case wxNORMAL:
	default:
		wxFprintf(m_pstream, wxT("\t <FWeight `Regular'>\n"));
	}

	wxFprintf(m_pstream, wxT("\t <FSize %d pt>\n"), GetCharHeight());

	if (m_font.GetUnderlined()) {
		wxFprintf(m_pstream, wxT("\t <FUnderlining FSingle>\n"));
	} else {
		wxFprintf(m_pstream, wxT("\t <FUnderlining FNoUnderlining>\n"));
	}

	wxFprintf(m_pstream, wxT("\t> # end of Font\n"));

}

//-------------------------------------------------------------------------------

void SP_EXP_MIFDCImpl::DoDrawEllipse (wxCoord x, wxCoord y, wxCoord width, wxCoord height) {

  wxCHECK_RET( m_ok && m_pstream, wxT("DrawEllipse: invalid mif-dc") );

  if (m_brush.GetStyle() != wxBRUSHSTYLE_TRANSPARENT || m_pen.GetStyle() != wxPENSTYLE_TRANSPARENT) {
    wxFprintf(m_pstream, wxT(" <Ellipse\n"));
		WriteObjectProperties(MIF_ELLIPSE, 0);
    int scaledX = (int)(x * m_scale);
    int scaledY = (int)(y * m_scale);
    int scaledW = (int)(width * m_scale);
    int scaledH = (int)(height * m_scale);
    wxFprintf(m_pstream, wxT("\t<ShapeRect %d pt %d pt %d pt %d pt>\n"), scaledX, scaledY, scaledW, scaledH);
		wxFprintf(m_pstream, wxT(" > # end of Ellipse\n"));
	}

}


//not implemented yet
void SP_EXP_MIFDCImpl::DoDrawArc (wxCoord x1, wxCoord y1, wxCoord x2,
			   wxCoord y2, wxCoord xc, wxCoord yc) {
  SP_LOGDEBUG(wxT("SP_EXP_MIFDCImpl::DoDrawArc not implemented."));


}

//implemented but not tested yet, because not supported by snoopy
void SP_EXP_MIFDCImpl::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w,
				  wxCoord h, double sa, double ea) {

	wxCHECK_RET(m_ok && m_pstream, wxT("DrawArc: invalid mif-dc"));

	if (m_pen.GetStyle() != wxPENSTYLE_TRANSPARENT) {
		wxFprintf(m_pstream, wxT(" <Arc\n"));
		WriteObjectProperties(MIF_ARC, 0);
		int scaledX = (int)(x * m_scale);
		int scaledY = (int)(y * m_scale);
		int scaledW = (int)(w * m_scale);
		int scaledH = (int)(h * m_scale);
		int startangle = int(sa + 90) % 360;
		int anglelength = int(ea - sa);
		wxFprintf(m_pstream, wxT("\t<ArcRect %d pt %d pt %d pt %d pt>\n"), scaledX, scaledY, scaledW, scaledH);
		wxFprintf(m_pstream, wxT("\t<ArcTheta %d>\n"), startangle);
		wxFprintf(m_pstream, wxT("\t<ArcDTheta %d>\n"), anglelength);
		wxFprintf(m_pstream, wxT(" > # end of Arc\n"));
	}
}


void SP_EXP_MIFDCImpl::DoDrawText( const wxString& text, wxCoord x, wxCoord y ) {

  DoDrawRotatedText (text, x, y, 0);

}


void SP_EXP_MIFDCImpl::DoDrawRotatedText( const wxString& text, wxCoord x, wxCoord y, double angle ) {

	wxCHECK_RET( m_ok && m_pstream, wxT("DrawText: invalid mif-dc") );

	if (m_pen.GetStyle () != wxPENSTYLE_TRANSPARENT) {
    wxFprintf(m_pstream, wxT(" <TextLine\n"));
		WriteObjectProperties(MIF_TEXT, angle);
    int scaledX = (int)(x * m_scale);
    int scaledY = (int)((y + GetCharHeight()) * m_scale);
    wxFprintf(m_pstream, wxT("\t<TLOrigin %d pt %d pt>\n"), scaledX, scaledY);
    wxFprintf(m_pstream, wxT("\t<TLAlignment Left>\n"));	// left justified
		WriteFontProperties();
    wxFprintf(m_pstream, wxT("\t<String `%s\'>\n"), text.c_str());
		wxFprintf(m_pstream, wxT(" > # end of TextLine\n"));
  }

}


void SP_EXP_MIFDCImpl::DoDrawPoint(wxCoord x, wxCoord y) {
  wxPoint tmp[] = {wxPoint(x, y), wxPoint(x, y)};
  DoDrawLines(2, tmp);
}


void SP_EXP_MIFDCImpl::DoDrawLine (wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) {
  wxPoint tmp[] = {wxPoint(x1,y1), wxPoint(x2, y2)};
  DoDrawLines(2, tmp);

}


void SP_EXP_MIFDCImpl::DoDrawLines (int n, const wxPoint points[], wxCoord xoffset, wxCoord yoffset) {

	wxCHECK_RET( m_ok && m_pstream, wxT("DrawLines: invalid mif-dc") );

  if (m_pen.GetStyle () != wxPENSTYLE_TRANSPARENT) {
    wxFprintf(m_pstream, wxT(" <PolyLine\n"));
		m_brush.SetStyle(wxBRUSHSTYLE_TRANSPARENT);
    WriteObjectProperties(MIF_POLYLINE, 0);
    int scaledX, scaledY;
    for (int i = 0; i < n; i++) {
      scaledX = (int)(points[i].x * m_scale);
      scaledY = (int)(points[i].y * m_scale);
      wxFprintf(m_pstream, wxT("\t<Point %d pt %d pt>\n"), scaledX, scaledY);
    }
		wxFprintf(m_pstream, wxT(" > # end of PolyLine\n"));
	}

}


void SP_EXP_MIFDCImpl::DoDrawPolygon (int n, const wxPoint points[],
			       wxCoord xoffset, wxCoord yoffset, wxPolygonFillMode fillStyle) {

  wxCHECK_RET( m_ok && m_pstream, wxT("DoDrawPolygon: invalid mif-dc") );

  if (m_pen.GetStyle() != wxPENSTYLE_TRANSPARENT) {
    wxFprintf(m_pstream, wxT(" <Polygon\n"));
		WriteObjectProperties(MIF_POLYGON, 0);
    int scaledX, scaledY;
    for (int i = 0; i < n; i++) {
      scaledX = (int)(points[i].x * m_scale);
      scaledY = (int)(points[i].y * m_scale);
      wxFprintf (m_pstream, wxT("\t<Point %d pt %d pt>\n"), scaledX, scaledY);
    }
		wxFprintf(m_pstream, wxT(" > # end of Polygon\n"));
	}

}


void SP_EXP_MIFDCImpl::DoDrawRectangle (wxCoord x, wxCoord y, wxCoord width, wxCoord height) {

	wxPoint tmp[] = {wxPoint(x,y), wxPoint(x + width, y),
		    wxPoint(x+width,y+height), wxPoint(x, y+ height),
		    wxPoint(x, y)};
	DoDrawPolygon(5, tmp);

}

void SP_EXP_MIFDCImpl::DoDrawRoundedRectangle (wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius) {

  wxCHECK_RET( m_ok && m_pstream, wxT("DoDrawRoundedRectangle: invalid mif-dc") );

  if (m_pen.GetStyle() != wxPENSTYLE_TRANSPARENT) {
    wxFprintf(m_pstream, wxT(" <RoundRect\n"));
		WriteObjectProperties(MIF_POLYGON, 0);
    int scaledX, scaledY, scaledW, scaledH, scaledR;
		scaledX = (int)(x * m_scale);
		scaledY = (int)(y * m_scale);
		scaledW = (int)(width * m_scale);
		scaledH = (int)(height * m_scale);
		scaledR = (int)(radius * m_scale);
		wxFprintf(m_pstream, wxT("\t<ShapeRect %d pt %d pt %d pt %d pt>\n"), scaledX, scaledY, scaledW, scaledH);
		wxFprintf(m_pstream, wxT("\t<Radius %d pt>\n"), scaledR);
		wxFprintf(m_pstream, wxT(" > # end of RoundRect\n"));
	}
}


//------------------------------------------------------------------------------
void SP_EXP_MIFDCImpl::SetBackground (const wxBrush& brush)
{
  SP_LOGDEBUG(wxT("SetBackground"));
  m_backgroundBrush = brush;
}

void SP_EXP_MIFDCImpl::SetResolution(int ppi)
{
  m_resolution = ppi;
  m_scale = (float)ppi / DEF_RESOLUTION;
}

int SP_EXP_MIFDCImpl::GetResolution() const
{
  return m_resolution;
}


void SP_EXP_MIFDCImpl::SetFont( const wxFont& font )
{
//   SP_LOGDEBUG(wxT("SetFont"));
  m_font = font;
}

void SP_EXP_MIFDCImpl::SetPen( const wxPen& pen )
{
//   SP_LOGDEBUG(wxT("SetPen"));
  m_pen = pen;
}

void SP_EXP_MIFDCImpl::SetBrush( const wxBrush& brush )
{
//   SP_LOGDEBUG(wxT("SetBrush"));
  m_brush = brush;
}

bool SP_EXP_MIFDCImpl::Ok() const
{
  return m_ok;
}


// from wxPostscriptDC, not tested
//------------------------------------------------------------------------------

void SP_EXP_MIFDCImpl::SetDeviceOrigin( wxCoord x, wxCoord y )
{
  SP_LOGDEBUG(wxString::Format(wxT("SetDeviceOrigin %d %d"),
				 x, y));
  wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );

  int h = 0;
  int w = 0;
  GetSize( &w, &h );

  wxDCImpl::SetDeviceOrigin( x, h-y );
}

void SP_EXP_MIFDCImpl::DoGetSize(int* width, int* height) const
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

void SP_EXP_MIFDCImpl::DoGetSizeMM(int *width, int *height) const
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

wxCoord SP_EXP_MIFDCImpl::GetCharHeight() const
{
  //  SP_LOGDEBUG(wxT("GetCharHeight"));
  if (m_font.Ok())
    return m_font.GetPointSize()+1;
  else
    return 12;
}

wxCoord SP_EXP_MIFDCImpl::GetCharWidth() const
{
  //  SP_LOGDEBUG(wxT("GetCharWidth"));
  // Chris Breeze: reasonable approximation using wxMODERN/Courier
  return (wxCoord) (GetCharHeight() * DEF_RESOLUTION / 120.0);
}

// Resolution in pixels per logical inch
wxSize SP_EXP_MIFDCImpl::GetPPI(void) const
{
  //  SP_LOGDEBUG(wxT("GetPPI"));
  return wxSize(m_resolution, m_resolution);
}

// do nothing
//------------------------------------------------------------------------------

void SP_EXP_MIFDCImpl::StartPage()
{
  SP_LOGDEBUG(wxT("StartPage"));
}

void SP_EXP_MIFDCImpl::EndPage ()
{
  SP_LOGDEBUG(wxT("EndPage"));
}


void SP_EXP_MIFDCImpl::DoSetClippingRegion (wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
//   SP_LOGDEBUG(wxString::Format(wxT("DoSetClippingRegion %d %d %d %d"),
// 				 x, y, w, h));
}

// NB: this function works with device coordinates, not the logical ones!
void SP_EXP_MIFDCImpl::DoSetDeviceClippingRegion(const wxRegion& region)
{

}


void SP_EXP_MIFDCImpl::DestroyClippingRegion()
{
  //  SP_LOGDEBUG(wxT("DestroyClippingRegion"));
}


// not implemented
//------------------------------------------------------------------------------

bool SP_EXP_MIFDCImpl::DoBlit(wxCoord xdest, wxCoord ydest,
		       wxCoord fwidth, wxCoord fheight,
		       wxDC *source,
		       wxCoord xsrc, wxCoord ysrc,
		       wxRasterOperationMode rop, bool useMask,
		       wxCoord xsrcMask, wxCoord ysrcMask )
{
  SP_LOGDEBUG(wxT("DoBlit"));
  wxFAIL_MSG( wxT("SP_EXP_MIFDCImpl::DoBlit not implemented.") );
  return FALSE;
}


void SP_EXP_MIFDCImpl::DoGetTextExtent(const wxString& string,
                                     wxCoord *x, wxCoord *y,
                                     wxCoord *descent, wxCoord *externalLeading,
                                     const wxFont *theFont ) const
{
  SP_LOGDEBUG(wxT("DoGetTextExtent"));
  wxFAIL_MSG( wxT("SP_EXP_MIFDCImpl::DoGetTextExtent not implemented.") );
}


void SP_EXP_MIFDCImpl::Clear()
{
  SP_LOGDEBUG(wxT("Clear"));
  wxFAIL_MSG( wxT("SP_EXP_MIFDCImpl::Clear not implemented.") );
}


bool SP_EXP_MIFDCImpl::DoFloodFill (wxCoord x, wxCoord y,
			     const wxColour &col, wxFloodFillStyle style)
{
  SP_LOGDEBUG(wxString::Format(wxT("DoFloodFill %d %d, color: %d %d %d, style %d"),
				 x, y,
				 col.Red(), col.Green(), col.Blue(), style));
  wxFAIL_MSG( wxT("SP_EXP_MIFDCImpl::DoFloodFill not implemented.") );
  return FALSE;
}

bool SP_EXP_MIFDCImpl::DoGetPixel (wxCoord x, wxCoord y,
			    wxColour * col) const
{
  SP_LOGDEBUG(wxString::Format(wxT("DoGetPixel %d %d, color: %d %d %d"),
				 x, y,
		col->Red(), col->Green(), col->Blue()));
  wxFAIL_MSG( wxT("SP_EXP_MIFDCImpl::GetPixel not implemented.") );
  return FALSE;
}

void SP_EXP_MIFDCImpl::DoCrossHair (wxCoord x, wxCoord y)
{
  SP_LOGDEBUG(wxString::Format(wxT("DoCrossHair %d %d"),
				 x, y));
  wxFAIL_MSG( wxT("SP_EXP_MIFDCImpl::DoCrossHair not implemented.") );
}


void SP_EXP_MIFDCImpl::SetLogicalFunction (wxRasterOperationMode function)
{
  SP_LOGDEBUG(wxT("SetLogicalFunction"));
  wxFAIL_MSG( wxT("SP_EXP_MIFDCImpl::SetLogicalFunction not implemented.") );
}


void SP_EXP_MIFDCImpl::DoDrawIcon( const wxIcon& icon, wxCoord x, wxCoord y )
{
  SP_LOGDEBUG(wxString::Format(wxT("DoDrawIcon %d %d"),
				 x, y));
  wxFAIL_MSG( wxT("SP_EXP_MIFDCImpl::DoDrawIcon not implemented.") );
}

void SP_EXP_MIFDCImpl::DoDrawBitmap( const wxBitmap& bitmap,
			      wxCoord x, wxCoord y, bool useMask )
{
  SP_LOGDEBUG(wxString::Format(wxT("DoDrawBitmap %d %d"),
				 x, y));
  wxFAIL_MSG( wxT("SP_EXP_MIFDCImpl::DoDrawBitmap not implemented.") );
}

void SP_EXP_MIFDCImpl::DoDrawSpline(wxList* points)
{
	SP_LOGDEBUG(wxT("DoDrawSpline"));
	wxCHECK_RET( m_ok && m_pstream, wxT("DoDrawSpline: invalid mif-dc") );

	if (m_pen.GetStyle() != wxPENSTYLE_TRANSPARENT)
	{
		wxFprintf(m_pstream, wxT(" <PolyLine\n"));
		m_brush.SetStyle(wxBRUSHSTYLE_TRANSPARENT);
		WriteObjectProperties(MIF_POLYLINE, 0);
		wxFprintf(m_pstream, wxT("\t<Smoothed Yes>\n"));
		int scaledX, scaledY;
		wxList::compatibility_iterator iter = points->GetFirst();
		while(iter)
		{
			wxPoint* current = (wxPoint*)iter->GetData();
			scaledX = (int)(current->x * m_scale);
			scaledY = (int)(current->y * m_scale);
			wxFprintf (m_pstream, wxT("\t<Point %d pt %d pt>\n"), scaledX, scaledY);
			iter = iter->GetNext();
		}
		wxFprintf(m_pstream, wxT(" > # end of Polygon\n"));
	}
}


void SP_EXP_MIFDCImpl::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
  SP_LOGDEBUG(wxString::Format(wxT("SetAxisOrientation %d %d"),
				 xLeftRight, yBottomUp));
  wxFAIL_MSG( wxT("SP_EXP_MIFDCImpl::SetAxisOrientation not implemented.") );

}
#else
// MIF objects
enum {
  MIF_ELLIPSE = 0,	// ellipse including circle
	MIF_POLYGON,			// polygon including rectangle and square
	MIF_POLYLINE,			// polyline including single line and dot
	MIF_ARC,					// round and elliptic arc
	MIF_TEXT					// text
};


//-------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(SP_EXP_MIFDC, wxDC)

//-------------------------------------------------------------------------------

void
SP_EXP_MIFDC::Init ()
{
  m_pstream = (FILE*) NULL;

  m_signX =  1;  // default x-axis left to right
  m_signY = 1;  // default y-axis top up -> bottom down

  m_resolution = DEF_RESOLUTION;
  m_scale = 1;

	m_exportColors = SP_MESSAGEBOX(wxT("Do you want color export?"), wxT("Question"), wxYES_NO);
	if (m_exportColors == wxYES) {
		m_msColors[wxT("255,0,0")] = wxT("Red");
		m_msColors[wxT("128,0,0")] = wxT("Red_dark");
		m_msColors[wxT("255,128,128")] = wxT("Red_light");
		m_msColors[wxT("0,255,0")] = wxT("Green");
		m_msColors[wxT("0,128,0")] = wxT("Green_dark");
		m_msColors[wxT("128,255,128")] = wxT("Green_light");
		m_msColors[wxT("0,0,255")] = wxT("Blue");
		m_msColors[wxT("0,0,128")] = wxT("Blue_dark");
		m_msColors[wxT("128,128,255")] = wxT("Blue_light");
		m_msColors[wxT("0,255,255")] = wxT("Cyan");
		m_msColors[wxT("0,128,128")] = wxT("Cyan_dark");
		m_msColors[wxT("128,255,255")] = wxT("Cyan_light");
		m_msColors[wxT("0,128,255")] = wxT("Cyan_Blue");
		m_msColors[wxT("0,255,128")] = wxT("Cyan_Green");
		m_msColors[wxT("255,0,255")] = wxT("Magenta");
		m_msColors[wxT("128,0,128")] = wxT("Magenta_dark");
		m_msColors[wxT("255,128,255")] = wxT("Magenta_light");
		m_msColors[wxT("128,0,255")] = wxT("Magenta_Blue");
		m_msColors[wxT("255,0,128")] = wxT("Magenta_Red");
		m_msColors[wxT("255,255,0")] = wxT("Yellow");
		m_msColors[wxT("128,128,0")] = wxT("Yellow_dark");
		m_msColors[wxT("255,255,128")] = wxT("Yellow_light");
		m_msColors[wxT("128,255,0")] = wxT("Yellow_Green");
		m_msColors[wxT("255,128,0")] = wxT("Yellow_Red");
	}
}


SP_EXP_MIFDC::SP_EXP_MIFDC ()
{
  Init();
}

SP_EXP_MIFDC::SP_EXP_MIFDC (const wxPrintData& printData)
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

SP_EXP_MIFDC::~SP_EXP_MIFDC ()
{
  if (m_pstream) {
    fclose( m_pstream );
    m_pstream = (FILE*) NULL;
  }
}

//------------------------------------------------------------------------------
bool SP_EXP_MIFDC::StartDoc( const wxString& message )
{
  wxCHECK_MSG( m_ok, FALSE, wxT("invalid mif-dc") );

  if (m_printData.GetFilename() == wxT("")) {
    wxString filename = wxGetTempFileName( wxT("mif") );
    SP_LOGDEBUG(wxString::Format(wxT("writing to temporary file %s"),
		  filename.c_str()));
    m_printData.SetFilename(filename);
  }

  m_pstream = wxFopen( m_printData.GetFilename().c_str(), wxT("w+") );

  if (!m_pstream) {
    SP_LOGERROR( _("Cannot open file for output!"));
    m_ok = FALSE;
    return FALSE;
  }

  m_ok = TRUE;

  wxFprintf(m_pstream, wxT("<MIFFile 6.00> # Created by SNOOPY\n"));

	//defining default colors
	if (m_exportColors == wxYES) {
		SP_MapString2String::iterator itC;

		wxFprintf(m_pstream, wxT(" <ColorCatalog\n"));
		for(itC = m_msColors.begin(); itC != m_msColors.end(); ++itC) {
			wxFprintf(m_pstream, wxT("\t<Color\n"));
			wxFprintf(m_pstream, wxT("\t <ColorTag `%s'>\n"), (*itC).second.c_str());
			wxFprintf(m_pstream, wxT("\t <ColorCyan %d>\n"), GetCyan((*itC).first));
			wxFprintf(m_pstream, wxT("\t <ColorMagenta %d>\n"), GetMagenta((*itC).first));
			wxFprintf(m_pstream, wxT("\t <ColorYellow %d>\n"), GetYellow((*itC).first));
			wxFprintf(m_pstream, wxT("\t># End of Color.\n"));
		}
		wxFprintf(m_pstream, wxT(" ># End of ColorCatalog.\n"));
	}

  return TRUE;
}


void SP_EXP_MIFDC::EndDoc ()
{
  wxCHECK_RET( m_ok && m_pstream, wxT("invalid mif-dc") );

	wxFprintf(m_pstream, wxT("# End of MIFFile."));

  fclose( m_pstream );
  m_pstream = (FILE *) NULL;
}


int SP_EXP_MIFDC::GetCyan(const wxString& sVal) {
	int red;
	wxSscanf(sVal.c_str(), wxT("%d"), &red);
	return (255-red)/2.55;
}


int SP_EXP_MIFDC::GetMagenta(const wxString& sVal) {
	int green;
	wxSscanf(sVal.c_str(), wxT("%*d,%d"), &green);
	return (255-green)/2.55;
}


int SP_EXP_MIFDC::GetYellow(const wxString& sVal) {
	int blue;
	wxSscanf(sVal.c_str(), wxT("%*d,%*d,%d"), &blue);
	return (255-blue)/2.55;
}


int SP_EXP_MIFDC::GetFillStyle() {
	int style;

  switch (m_brush.GetStyle()) {
  case wxSOLID:
    if (m_brush.GetColour() == *wxWHITE) {
      style = 7;
    } else {
			style = 0;
    }
    break;
  case wxCROSS_HATCH: 	// Cross hatch.
    style = 4; // not supported by framemaker --> set to solid gray
		break;
  case wxVERTICAL_HATCH: //	Vertical hatch.
    style = 10;
    break;
  case wxHORIZONTAL_HATCH: // 	Horizontal hatch.
    style = 11;
		break;
  case wxCROSSDIAG_HATCH:  // 	Cross-diagonal hatch.
    style = 12;
    break;
  case wxFDIAGONAL_HATCH: // 	Forward diagonal (lb->rt) hatch.
    style = 13;
    break;
  case wxBDIAGONAL_HATCH: // 	Backward diagonal (lt->rb) hatch.
    style = 14;
    break;
  case wxTRANSPARENT: 	//Transparent
    style = 15;
    break;
  default:
    style = 15; // not filled
    SP_LOGDEBUG(wxString::Format(wxT("GetFillStyle: unsupported style %d, set to %d"),
				   m_brush.GetStyle(), style));
  }
  return style;

}


int SP_EXP_MIFDC::GetPenColor() {

  if (m_pen.GetColour() == *wxWHITE) return 7;
  return 0;
}


int SP_EXP_MIFDC::GetObTint() {
	float gray;
	//get shade of object fill-color
	//if there is none, get it from the pen color
	if ((m_brush.GetColour() == *wxWHITE) || (m_brush.GetStyle() == wxTRANSPARENT)) {
		gray = (m_pen.GetColour().Green() + m_pen.GetColour().Red() + m_pen.GetColour().Blue()) / 3;
	} else {
		gray = (m_brush.GetColour().Green() + m_brush.GetColour().Red() + m_brush.GetColour().Blue()) / 3;
	}
	//transform gray-tone (0..255) to tint (100%..0%)
	gray = 100 - gray / 2.55;
	//tint percentage is multiplied by 100
	return int(gray * 100);
}


wxString SP_EXP_MIFDC::GetObColor() {
	int red, green, blue;
	wxString rgb;

	if ((m_brush.GetColour() == *wxWHITE) || (m_brush.GetStyle() == wxTRANSPARENT)) {
		red = Approx(m_pen.GetColour().Red());
		green = Approx(m_pen.GetColour().Green());
		blue = Approx(m_pen.GetColour().Blue());
	} else {
		red = Approx(m_brush.GetColour().Red());
		green = Approx(m_brush.GetColour().Green());
		blue = Approx(m_brush.GetColour().Blue());
	}

	rgb = wxString::Format(wxT("%d,%d,%d"), red, green, blue);
	SP_MapString2String::iterator l_itColors = m_msColors.find(rgb);
	if (l_itColors != m_msColors.end()) return l_itColors->second;
	return wxT("Black");
}

int SP_EXP_MIFDC::Approx(int Val) {
	int result;

	//setting color value to the nearest multiple of 128
	if (Val <= 64) {
		result = 0;
	} else if (Val <= 192) {
		result = 128;
	} else {
		result = 255;
	}
	return result;
}

void SP_EXP_MIFDC::WriteObjectProperties(int obj, double angle) {

	wxFprintf(m_pstream, wxT("\t<Pen 0>\n"));	//shades are achieved by 'ObTint'-value
	wxFprintf(m_pstream, wxT("\t<Fill %d>\n"), GetFillStyle());
	wxFprintf(m_pstream, wxT("\t<PenWidth %d pt>\n"), m_pen.GetWidth());

	wxString col = wxT("Black");
	if (m_exportColors == wxYES) col = GetObColor();
	wxFprintf(m_pstream, wxT("\t<ObColor `%s'>\n"), col.c_str());

	if (col.CmpNoCase(wxT("Black")) == 0) {
		wxFprintf(m_pstream, wxT("\t<ObTint %d>\n"), GetObTint());
	} else {
		wxFprintf(m_pstream, wxT("\t<ObTint 10000>\n"));
	}
	if (angle != 0) wxFprintf(m_pstream, wxT("\t<Angle %G>\n"), angle);	//default = 0
	wxFprintf(m_pstream, wxT("\t<DashedPattern\n"));
	switch(m_pen.GetStyle()) {
	case wxSOLID:
		wxFprintf(m_pstream, wxT("\t <DashedStyle Solid>\n\t>\n"));
		break;
	case wxDOT:
		wxFprintf(m_pstream, wxT("\t <DashedStyle Dashed>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 2.0 pt>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 4.0 pt>\n\t>\n"));
		break;
	case wxSHORT_DASH:
		wxFprintf(m_pstream, wxT("\t <DashedStyle Dashed>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 4.0 pt>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 4.0 pt>\n\t>\n"));
		break;
	case wxLONG_DASH:
		wxFprintf(m_pstream, wxT("\t <DashedStyle Dashed>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 8.0 pt>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 6.0 pt>\n\t>\n"));
		break;
	case wxDOT_DASH:
		wxFprintf(m_pstream, wxT("\t <DashedStyle Dashed>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 12.0 pt>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 6.0 pt>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 2.0 pt>\n"));
		wxFprintf(m_pstream, wxT("\t <DashSegment 6.0 pt>\n\t>\n"));
		break;
	default:
 		wxFprintf(m_pstream, wxT("\t <DashedStyle Solid>\n\t>\n")); // set to solid
		SP_LOGDEBUG(wxString::Format(wxT("WriteObjectProperties: unsupported pen style %d, set to wxSOLID"),
					 m_brush.GetStyle()));
	}

}


void SP_EXP_MIFDC::WriteFontProperties() {

	wxFprintf(m_pstream, wxT("\t<Font\n"));
	wxFprintf(m_pstream, wxT("\t <FTag `\'>\n"));

  switch(m_font.GetFamily()) {
  case wxDEFAULT:
  case wxSWISS:
    wxFprintf(m_pstream, wxT("\t <FFamily `Arial'>\n"));
    break;
	case wxTELETYPE:
  case wxMODERN:
    wxFprintf(m_pstream, wxT("\t <FFamily `Courier New'>\n"));
    break;
  case wxROMAN:
    wxFprintf(m_pstream, wxT("\t <FFamily `Times New Roman'>\n"));
    break;
  case wxSCRIPT:
		// might not work properly if wxT("Brush Script") doesn't exist on the system
    wxFprintf(m_pstream, wxT("\t <FFamily `Brush Script'>\n"));
  }

	switch(m_font.GetStyle()) {
	case wxNORMAL:
    wxFprintf(m_pstream, wxT("\t <FAngle `Regular'>\n"));
		break;
  case wxSLANT:
  case wxITALIC:
    wxFprintf(m_pstream, wxT("\t <FAngle `Italic'>\n"));
	}

	switch(m_font.GetWeight()) {
  case wxLIGHT:
  case wxNORMAL:
    wxFprintf(m_pstream, wxT("\t <FWeight `Regular'>\n"));
		break;
	case wxBOLD:
    wxFprintf(m_pstream, wxT("\t <FWeight `Bold'>\n"));
	}

	wxFprintf(m_pstream, wxT("\t <FSize %d pt>\n"), GetCharHeight());

	if (m_font.GetUnderlined()) {
		wxFprintf(m_pstream, wxT("\t <FUnderlining FSingle>\n"));
	} else {
		wxFprintf(m_pstream, wxT("\t <FUnderlining FNoUnderlining>\n"));
	}

	wxFprintf(m_pstream, wxT("\t> # end of Font\n"));

}

//-------------------------------------------------------------------------------

void SP_EXP_MIFDC::DoDrawEllipse (wxCoord x, wxCoord y, wxCoord width, wxCoord height) {

  wxCHECK_RET( m_ok && m_pstream, wxT("DrawEllipse: invalid mif-dc") );

  if (m_brush.GetStyle() != wxTRANSPARENT || m_pen.GetStyle() != wxTRANSPARENT) {
    wxFprintf(m_pstream, wxT(" <Ellipse\n"));
		WriteObjectProperties(MIF_ELLIPSE, 0);
    int scaledX = (int)(x * m_scale);
    int scaledY = (int)(y * m_scale);
    int scaledW = (int)(width * m_scale);
    int scaledH = (int)(height * m_scale);
    wxFprintf(m_pstream, wxT("\t<ShapeRect %d pt %d pt %d pt %d pt>\n"), scaledX, scaledY, scaledW, scaledH);
		wxFprintf(m_pstream, wxT(" > # end of Ellipse\n"));
	}

}


//not implemented yet
void SP_EXP_MIFDC::DoDrawArc (wxCoord x1, wxCoord y1, wxCoord x2,
			   wxCoord y2, wxCoord xc, wxCoord yc) {
  SP_LOGDEBUG(wxT("SP_EXP_MIFDC::DoDrawArc not implemented."));


}

//implemented but not tested yet, because not supported by snoopy
void SP_EXP_MIFDC::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w,
				  wxCoord h, double sa, double ea) {

	wxCHECK_RET(m_ok && m_pstream, wxT("DrawArc: invalid mif-dc"));

	if (m_pen.GetStyle() != wxTRANSPARENT) {
		wxFprintf(m_pstream, wxT(" <Arc\n"));
		WriteObjectProperties(MIF_ARC, 0);
		int scaledX = (int)(x * m_scale);
		int scaledY = (int)(y * m_scale);
		int scaledW = (int)(w * m_scale);
		int scaledH = (int)(h * m_scale);
		int startangle = int(sa + 90) % 360;
		int anglelength = int(ea - sa);
		wxFprintf(m_pstream, wxT("\t<ArcRect %d pt %d pt %d pt %d pt>\n"), scaledX, scaledY, scaledW, scaledH);
		wxFprintf(m_pstream, wxT("\t<ArcTheta %d>\n"), startangle);
		wxFprintf(m_pstream, wxT("\t<ArcDTheta %d>\n"), anglelength);
		wxFprintf(m_pstream, wxT(" > # end of Arc\n"));
	}
}


void SP_EXP_MIFDC::DoDrawText( const wxString& text, wxCoord x, wxCoord y ) {

  DoDrawRotatedText (text, x, y, 0);

}


void SP_EXP_MIFDC::DoDrawRotatedText( const wxString& text, wxCoord x, wxCoord y, double angle ) {

	wxCHECK_RET( m_ok && m_pstream, wxT("DrawText: invalid mif-dc") );

	if (m_pen.GetStyle () != wxTRANSPARENT) {
    wxFprintf(m_pstream, wxT(" <TextLine\n"));
		WriteObjectProperties(MIF_TEXT, angle);
    int scaledX = (int)(x * m_scale);
    int scaledY = (int)((y + GetCharHeight()) * m_scale);
    wxFprintf(m_pstream, wxT("\t<TLOrigin %d pt %d pt>\n"), scaledX, scaledY);
    wxFprintf(m_pstream, wxT("\t<TLAlignment Left>\n"));	// left justified
		WriteFontProperties();
    wxFprintf(m_pstream, wxT("\t<String `%s\'>\n"), text.c_str());
		wxFprintf(m_pstream, wxT(" > # end of TextLine\n"));
  }

}


void SP_EXP_MIFDC::DoDrawPoint(wxCoord x, wxCoord y) {
  wxPoint tmp[] = {wxPoint(x, y), wxPoint(x, y)};
  DoDrawLines(2, tmp);
}


void SP_EXP_MIFDC::DoDrawLine (wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) {
  wxPoint tmp[] = {wxPoint(x1,y1), wxPoint(x2, y2)};
  DoDrawLines(2, tmp);

}


void SP_EXP_MIFDC::DoDrawLines (int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset) {

	wxCHECK_RET( m_ok && m_pstream, wxT("DrawLines: invalid mif-dc") );

  if (m_pen.GetStyle () != wxTRANSPARENT) {
    wxFprintf(m_pstream, wxT(" <PolyLine\n"));
		m_brush.SetStyle(wxTRANSPARENT);
    WriteObjectProperties(MIF_POLYLINE, 0);
    int scaledX, scaledY;
    for (int i = 0; i < n; i++) {
      scaledX = (int)(points[i].x * m_scale);
      scaledY = (int)(points[i].y * m_scale);
      wxFprintf(m_pstream, wxT("\t<Point %d pt %d pt>\n"), scaledX, scaledY);
    }
		wxFprintf(m_pstream, wxT(" > # end of PolyLine\n"));
	}

}


void SP_EXP_MIFDC::DoDrawPolygon (int n, wxPoint points[],
			       wxCoord xoffset, wxCoord yoffset, int fillStyle) {

  wxCHECK_RET( m_ok && m_pstream, wxT("DoDrawPolygon: invalid mif-dc") );

  if (m_pen.GetStyle() != wxTRANSPARENT) {
    wxFprintf(m_pstream, wxT(" <Polygon\n"));
		WriteObjectProperties(MIF_POLYGON, 0);
    int scaledX, scaledY;
    for (int i = 0; i < n; i++) {
      scaledX = (int)(points[i].x * m_scale);
      scaledY = (int)(points[i].y * m_scale);
      wxFprintf (m_pstream, wxT("\t<Point %d pt %d pt>\n"), scaledX, scaledY);
    }
		wxFprintf(m_pstream, wxT(" > # end of Polygon\n"));
	}

}


void SP_EXP_MIFDC::DoDrawRectangle (wxCoord x, wxCoord y, wxCoord width, wxCoord height) {

	wxPoint tmp[] = {wxPoint(x,y), wxPoint(x + width, y),
		    wxPoint(x+width,y+height), wxPoint(x, y+ height),
		    wxPoint(x, y)};
	DoDrawPolygon(5, tmp);

}

void SP_EXP_MIFDC::DoDrawRoundedRectangle (wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius) {

  wxCHECK_RET( m_ok && m_pstream, wxT("DoDrawRoundedRectangle: invalid mif-dc") );

  if (m_pen.GetStyle() != wxTRANSPARENT) {
    wxFprintf(m_pstream, wxT(" <RoundRect\n"));
		WriteObjectProperties(MIF_POLYGON, 0);
    int scaledX, scaledY, scaledW, scaledH, scaledR;
		scaledX = (int)(x * m_scale);
		scaledY = (int)(y * m_scale);
		scaledW = (int)(width * m_scale);
		scaledH = (int)(height * m_scale);
		scaledR = (int)(radius * m_scale);
		wxFprintf(m_pstream, wxT("\t<ShapeRect %d pt %d pt %d pt %d pt>\n"), scaledX, scaledY, scaledW, scaledH);
		wxFprintf(m_pstream, wxT("\t<Radius %d pt>\n"), scaledR);
		wxFprintf(m_pstream, wxT(" > # end of RoundRect\n"));
	}
}


//------------------------------------------------------------------------------
void SP_EXP_MIFDC::SetBackground (const wxBrush& brush)
{
  SP_LOGDEBUG(wxT("SetBackground"));
  m_backgroundBrush = brush;
}

void SP_EXP_MIFDC::SetResolution(int ppi)
{
  m_resolution = ppi;
  m_scale = (float)ppi / DEF_RESOLUTION;
}

int SP_EXP_MIFDC::GetResolution()
{
  return m_resolution;
}


void SP_EXP_MIFDC::SetFont( const wxFont& font )
{
//   SP_LOGDEBUG(wxT("SetFont"));
  m_font = font;
}

void SP_EXP_MIFDC::SetPen( const wxPen& pen )
{
//   SP_LOGDEBUG(wxT("SetPen"));
  m_pen = pen;
}

void SP_EXP_MIFDC::SetBrush( const wxBrush& brush )
{
//   SP_LOGDEBUG(wxT("SetBrush"));
  m_brush = brush;
}

bool SP_EXP_MIFDC::Ok() const
{
  return m_ok;
}


// from wxPostscriptDC, not tested
//------------------------------------------------------------------------------

void SP_EXP_MIFDC::SetDeviceOrigin( wxCoord x, wxCoord y )
{
  SP_LOGDEBUG(wxString::Format(wxT("SetDeviceOrigin %d %d"),
				 x, y));
  wxCHECK_RET( m_ok && m_pstream, wxT("invalid xfig dc") );

  int h = 0;
  int w = 0;
  GetSize( &w, &h );

  wxDC::SetDeviceOrigin( x, h-y );
}

void SP_EXP_MIFDC::DoGetSize(int* width, int* height) const
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

void SP_EXP_MIFDC::DoGetSizeMM(int *width, int *height) const
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

wxCoord SP_EXP_MIFDC::GetCharHeight() const
{
  //  SP_LOGDEBUG(wxT("GetCharHeight"));
  if (m_font.Ok())
    return m_font.GetPointSize()+1;
  else
    return 12;
}

wxCoord SP_EXP_MIFDC::GetCharWidth() const
{
  //  SP_LOGDEBUG(wxT("GetCharWidth"));
  // Chris Breeze: reasonable approximation using wxMODERN/Courier
  return (wxCoord) (GetCharHeight() * DEF_RESOLUTION / 120.0);
}

// Resolution in pixels per logical inch
wxSize SP_EXP_MIFDC::GetPPI(void) const
{
  //  SP_LOGDEBUG(wxT("GetPPI"));
  return wxSize(m_resolution, m_resolution);
}

// do nothing
//------------------------------------------------------------------------------

void SP_EXP_MIFDC::StartPage()
{
  SP_LOGDEBUG(wxT("StartPage"));
}

void SP_EXP_MIFDC::EndPage ()
{
  SP_LOGDEBUG(wxT("EndPage"));
}


void SP_EXP_MIFDC::DoSetClippingRegion (wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
//   SP_LOGDEBUG(wxString::Format(wxT("DoSetClippingRegion %d %d %d %d"),
// 				 x, y, w, h));
}


void SP_EXP_MIFDC::DestroyClippingRegion()
{
  //  SP_LOGDEBUG(wxT("DestroyClippingRegion"));
}


// not implemented
//------------------------------------------------------------------------------

bool SP_EXP_MIFDC::DoBlit(wxCoord xdest, wxCoord ydest,
		       wxCoord fwidth, wxCoord fheight,
		       wxDC *source,
		       wxCoord xsrc, wxCoord ysrc,
		       int rop, bool useMask,
		       wxCoord xsrcMask, wxCoord ysrcMask )
{
  SP_LOGDEBUG(wxT("DoBlit"));
  wxFAIL_MSG( wxT("SP_EXP_MIFDC::DoBlit not implemented.") );
  return FALSE;
}


void SP_EXP_MIFDC::DoGetTextExtent(const wxString& string,
                                     wxCoord *x, wxCoord *y,
                                     wxCoord *descent, wxCoord *externalLeading,
                                     wxFont *theFont ) const
{
  SP_LOGDEBUG(wxT("DoGetTextExtent"));
  wxFAIL_MSG( wxT("SP_EXP_MIFDC::DoGetTextExtent not implemented.") );
}


void SP_EXP_MIFDC::Clear()
{
  SP_LOGDEBUG(wxT("Clear"));
  wxFAIL_MSG( wxT("SP_EXP_MIFDC::Clear not implemented.") );
}


bool SP_EXP_MIFDC::DoFloodFill (wxCoord x, wxCoord y,
			     const wxColour &col, int style)
{
  SP_LOGDEBUG(wxString::Format(wxT("DoFloodFill %d %d, color: %d %d %d, style %d"),
				 x, y,
				 col.Red(), col.Green(), col.Blue(), style));
  wxFAIL_MSG( wxT("SP_EXP_MIFDC::DoFloodFill not implemented.") );
  return FALSE;
}

bool SP_EXP_MIFDC::DoGetPixel (wxCoord x, wxCoord y,
			    wxColour * col) const
{
  SP_LOGDEBUG(wxString::Format(wxT("DoGetPixel %d %d, color: %d %d %d"),
				 x, y,
		col->Red(), col->Green(), col->Blue()));
  wxFAIL_MSG( wxT("SP_EXP_MIFDC::GetPixel not implemented.") );
  return FALSE;
}

void SP_EXP_MIFDC::DoCrossHair (wxCoord x, wxCoord y)
{
  SP_LOGDEBUG(wxString::Format(wxT("DoCrossHair %d %d"),
				 x, y));
  wxFAIL_MSG( wxT("SP_EXP_MIFDC::DoCrossHair not implemented.") );
}


void SP_EXP_MIFDC::SetLogicalFunction (int function)
{
  SP_LOGDEBUG(wxT("SetLogicalFunction"));
  wxFAIL_MSG( wxT("SP_EXP_MIFDC::SetLogicalFunction not implemented.") );
}


void SP_EXP_MIFDC::DoDrawIcon( const wxIcon& icon, wxCoord x, wxCoord y )
{
  SP_LOGDEBUG(wxString::Format(wxT("DoDrawIcon %d %d"),
				 x, y));
  wxFAIL_MSG( wxT("SP_EXP_MIFDC::DoDrawIcon not implemented.") );
}

void SP_EXP_MIFDC::DoDrawBitmap( const wxBitmap& bitmap,
			      wxCoord x, wxCoord y, bool useMask )
{
  SP_LOGDEBUG(wxString::Format(wxT("DoDrawBitmap %d %d"),
				 x, y));
  wxFAIL_MSG( wxT("SP_EXP_MIFDC::DoDrawBitmap not implemented.") );
}

void SP_EXP_MIFDC::DoDrawSpline(wxList* points)
{
	SP_LOGDEBUG(wxT("DoDrawSpline"));
	wxCHECK_RET( m_ok && m_pstream, wxT("DoDrawSpline: invalid mif-dc") );

	if (m_pen.GetStyle() != wxTRANSPARENT)
	{
		wxFprintf(m_pstream, wxT(" <PolyLine\n"));
		m_brush.SetStyle(wxTRANSPARENT);
		WriteObjectProperties(MIF_POLYLINE, 0);
		wxFprintf(m_pstream, wxT("\t<Smoothed Yes>\n"));
		int scaledX, scaledY;
		wxList::compatibility_iterator iter = points->GetFirst();
		while(iter)
		{
			wxPoint* current = (wxPoint*)iter->GetData();
			scaledX = (int)(current->x * m_scale);
			scaledY = (int)(current->y * m_scale);
			wxFprintf (m_pstream, wxT("\t<Point %d pt %d pt>\n"), scaledX, scaledY);
			iter = iter->GetNext();
		}
		wxFprintf(m_pstream, wxT(" > # end of Polygon\n"));
	}
}


void SP_EXP_MIFDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
  SP_LOGDEBUG(wxString::Format(wxT("SetAxisOrientation %d %d"),
				 xLeftRight, yBottomUp));
  wxFAIL_MSG( wxT("SP_EXP_MIFDC::SetAxisOrientation not implemented.") );

}
#endif

