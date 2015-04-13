/////////////////////////////////////////////////////////////////////////////
// Author:      alextov
// $Date: 2005/06/07 $
// Short Description: dc to write eps files. inherits from wxPostScriptDC
//                    and improves it a bit
/////////////////////////////////////////////////////////////////////////////

#include "export/SP_EPSDC.h"
#include "sp_defines.h"

#include <string.h>
#include <wx/filename.h>
#include <wx/generic/prntdlgg.h>
#include <wx/stdpaths.h>

#if wxABI_VERSION > 30000

IMPLEMENT_DYNAMIC_CLASS(SP_EPSDC, wxDC)

SP_EPSDC::SP_EPSDC()
 : wxDC(new SP_EPSDCImpl(this))
{
}

SP_EPSDC::SP_EPSDC(const wxPrintData& printData)
 : wxDC(new SP_EPSDCImpl(this, printData))
{
}

IMPLEMENT_ABSTRACT_CLASS(SP_EPSDCImpl, wxPostScriptDCImpl)

SP_EPSDCImpl::SP_EPSDCImpl (SP_EPSDC *owner)
	: wxPostScriptDCImpl((wxPostScriptDC*)owner)
{
	Init();
}

SP_EPSDCImpl::SP_EPSDCImpl (SP_EPSDC *owner, const wxPrintData& printData)
	: wxPostScriptDCImpl((wxPostScriptDC*)owner, printData)
{
	Init();
}

SP_EPSDCImpl::SP_EPSDCImpl (wxPrinterDC *owner)
	: wxPostScriptDCImpl(owner)
{
	Init();
}

SP_EPSDCImpl::SP_EPSDCImpl (wxPrinterDC *owner, const wxPrintData& printData)
	: wxPostScriptDCImpl(owner, printData)
{
	Init();
}

void
SP_EPSDCImpl::Init()
{
  if (m_printData.GetFilename() == wxT("")) {
	m_origFilename = wxFileName::CreateTempFileName(wxT("eps"));
	SP_LOGDEBUG(wxString::Format(wxT("writing to temporary file %s"),
				   m_origFilename.c_str()));
  } else {
	m_origFilename = m_printData.GetFilename();
  }

  m_printData.SetFilename(wxFileName::CreateTempFileName(wxT("eps")));
  m_ok = TRUE;

  m_x1 = m_y1 = INT_MAX;
  m_x2 = m_y2 = INT_MIN;

  //wxPostScriptDC::wxPostScriptDC();
#if wxMAC_USE_CORE_GRAPHICS
	// we need at least a measuring context because people start measuring before a page
	// gets printed at all
	SetGraphicsContext( wxGraphicsContext::Create());
#endif
}

#define BUFSIZE 1024

void SP_EPSDCImpl::EndDoc ()
{
  wxPostScriptDCImpl::EndDoc();

  FILE* tfile = wxFopen(m_printData.GetFilename().c_str(), wxT("r")); // TODO readonly open
  CHECK_POINTER(tfile, return);

  // buffer to read
  wxChar buf[BUFSIZE];

  bool err = TRUE;

  // skip comments written
  while (wxFgets(buf, BUFSIZE, tfile)) {
    if (wxStrstr(buf, wxT("EndComments"))) {
      err = FALSE;
      break;
    }
  }

  if (err) {
    fclose(tfile);
    CHECK_BOOL(! err, return);
  }

  m_pstream = wxFopen(m_origFilename.c_str(), wxT("w"));
  CHECK_POINTER(m_pstream, return);

  // write our own comments
  wxFprintf(m_pstream, wxT("%%!PS-Adobe-2.0 EPSF-2.0\n") );
  wxFprintf(m_pstream, wxT("%%%%Title: %s\n"), wxT("none"));//m_title.c_str() );
  wxFprintf(m_pstream, wxT("%%%%Creator: wxWindows PostScript renderer + Snoopy\n") );
  wxFprintf(m_pstream, wxT("%%%%CreationDate: %s\n"), wxNow().c_str() );
  if (m_printData.GetOrientation() == wxLANDSCAPE)
    wxFprintf( m_pstream, wxT("%%%%Orientation: Landscape\n") );
  else
    wxFprintf(m_pstream, wxT("%%%%Orientation: Portrait\n") );
  wxFprintf(m_pstream, wxT("%%%%BoundingBox: %d %d %d %d\n"),
	  m_x1, m_y1, m_x2+1, m_y2+1);
  wxFprintf(m_pstream, wxT("%%%%EndComments\n\n") );


  // now just copy everything from the old to the new file
  while (wxFgets(buf, BUFSIZE, tfile))
  {
	  wxFputs(buf, m_pstream);
  }


  fclose(tfile);
  fclose( m_pstream );
  m_pstream = (FILE *) NULL;

  wxRemoveFile(m_printData.GetFilename());

}

SP_EPSDCImpl::~SP_EPSDCImpl ()
{
}

void
SP_EPSDCImpl::SetBrush( const wxBrush& brush )
{
  if (!brush.Ok()) return;

  // wxPostScriptDC does not care about fill styles :(
  // we will have to implement them, at the moment just
  // set color to light gray if fill style is not wxTRANSPARENT
  // or wxSOLID it prevents from drawin black filled shapes
  if (brush.GetStyle() != wxTRANSPARENT &&
      brush.GetStyle() != wxSOLID &&
      brush.GetColour() == *wxBLACK)
  {
    wxBrush t = brush;
    t.SetColour(*wxLIGHT_GREY);
    wxPostScriptDCImpl::SetBrush(t);
  } else {
    wxPostScriptDCImpl::SetBrush(brush);
  }
}

void
SP_EPSDCImpl::CalcBoundingBox(wxCoord x, wxCoord y)
{
  if (LogicalToDeviceX(x) < m_x1) m_x1 = LogicalToDeviceX(x);
  if (LogicalToDeviceX(x) > m_x2) m_x2 = LogicalToDeviceX(x);
  if (LogicalToDeviceY(y) < m_y1) m_y1 = LogicalToDeviceY(y);
  if (LogicalToDeviceY(y) > m_y2) m_y2 = LogicalToDeviceY(y);
  wxPostScriptDCImpl::CalcBoundingBox(x, y);
}

void
SP_EPSDCImpl::DoDrawBitmap(const wxBitmap& bitmap, wxCoord x, wxCoord y, bool useMask)
{
    if (m_pen.GetStyle() == wxTRANSPARENT) return;

    wxPostScriptDCImpl::DoDrawBitmap(bitmap, x, y, useMask);
    CalcBoundingBox(x, y);
    CalcBoundingBox(x + bitmap.GetWidth(), y + bitmap.GetHeight());
}

void
SP_EPSDCImpl::DoDrawLines (int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset)
{
    wxCHECK_RET( m_ok && m_pstream, wxT("invalid postscript dc") );

    if (m_pen.GetStyle() == wxTRANSPARENT) return;

    if (n <= 0) return;

    SetPen (m_pen);

    int i;
    for ( i =0; i < n ; i++ )
    {
      CalcBoundingBox( points[i].x+xoffset,
		       points[i].y+yoffset);

    //	was bug in wxPostscriptDC!
    //         CalcBoundingBox( LogicalToDeviceX(points[i].x+xoffset),
    // 			 LogicalToDeviceY(points[i].y+yoffset));
    }

    wxString psData;
    psData.Printf(wxT("newpath\n")
				 wxT("%d %d moveto\n"),
				 LogicalToDeviceX(points[0].x+xoffset),
				 LogicalToDeviceY(points[0].y+yoffset));
    PsPrint(psData);

    for (i = 1; i < n; i++)
    {
    	psData.Printf(
		 wxT("%d %d lineto\n"),
		 LogicalToDeviceX(points[i].x+xoffset),
		 LogicalToDeviceY(points[i].y+yoffset) );
        PsPrint(psData);
    }

    PsPrint( wxT("stroke\n") );
}

void
SP_EPSDCImpl::DoDrawText( const wxString& text, wxCoord x, wxCoord y )
{
    if (m_pen.GetStyle() == wxTRANSPARENT) return;

	wxPostScriptDCImpl::DoDrawText(text, x, y);
	int l_nSize = m_font.GetPointSize();
	CalcBoundingBox( x, y );
	CalcBoundingBox( x + l_nSize * text.Length(), y + l_nSize);
}

void
SP_EPSDCImpl::DoDrawEllipse (wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    if (m_pen.GetStyle() == wxTRANSPARENT) return;

  wxPostScriptDCImpl::DoDrawEllipse(x, y, width, height);
  CalcBoundingBox(x - width - m_pen.GetWidth(),
		  y - height - m_pen.GetWidth());
  CalcBoundingBox(x + width + m_pen.GetWidth(),
		  y + height + m_pen.GetWidth());
}


void
SP_EPSDCImpl::DoDrawRectangle (wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    if (m_pen.GetStyle() == wxTRANSPARENT) return;

  wxPostScriptDCImpl::DoDrawRectangle(x, y, width, height);
  CalcBoundingBox(x - m_pen.GetWidth(),
		  y - m_pen.GetWidth());
  CalcBoundingBox(x + width + m_pen.GetWidth(),
		  y + height + m_pen.GetWidth());
}

void SP_EPSDCImpl::DoDrawRoundedRectangle (wxCoord x, wxCoord y, wxCoord width,
				       wxCoord height, double radius)
{
    if (m_pen.GetStyle() == wxTRANSPARENT) return;

  wxPostScriptDCImpl::DoDrawRoundedRectangle(x, y, width, height, radius);
  CalcBoundingBox(x - m_pen.GetWidth(),
		  y - m_pen.GetWidth());
  CalcBoundingBox(x + width + m_pen.GetWidth(),
		  y + height + m_pen.GetWidth());

}


void SP_EPSDCImpl::DoDrawSpline(const wxPointList *points)
{
    if (m_pen.GetStyle() == wxTRANSPARENT) return;

    wxPostScriptDCImpl::DoDrawSpline(points);
}

void SP_EPSDCImpl::DoDrawPolygon (int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, wxPolygonFillMode fillStyle)
{
    if (m_pen.GetStyle() == wxTRANSPARENT) return;

    int l_nStyle = m_pen.GetStyle();
    //m_pen.SetStyle(wxTRANSPARENT);

	wxPostScriptDCImpl::DoDrawPolygon (n, points, xoffset, yoffset, fillStyle);
	m_pen.SetStyle(l_nStyle);
}
#else
IMPLEMENT_DYNAMIC_CLASS(SP_EPSDC, wxPostScriptDC)

SP_EPSDC::SP_EPSDC ()
  : wxPostScriptDC()
{
}

SP_EPSDC::SP_EPSDC (const wxPrintData& printData)
{
  if (printData.GetFilename() == wxT("")) {
    m_origFilename = wxGetTempFileName( wxT("eps"));
    SP_LOGDEBUG(wxString::Format(wxT("writing to temporary file %s"),
				   m_origFilename.c_str()));
  } else {
    m_origFilename = printData.GetFilename();
  }

  m_printData = printData;
  m_printData.SetFilename(wxGetTempFileName(wxT("eps")));
  m_ok = TRUE;

  m_x1 = m_y1 = INT_MAX;
  m_x2 = m_y2 = INT_MIN;

  //wxPostScriptDC::wxPostScriptDC();
#if wxMAC_USE_CORE_GRAPHICS
	// we need at least a measuring context because people start measuring before a page
	// gets printed at all
	SetGraphicsContext( wxGraphicsContext::Create());
#endif
}

#define BUFSIZE 1024

void SP_EPSDC::EndDoc ()
{
  wxPostScriptDC::EndDoc();

  FILE* tfile = wxFopen(m_printData.GetFilename().c_str(), wxT("r")); // TODO readonly open
  CHECK_POINTER(tfile, return);

  // buffer to read
  wxChar buf[BUFSIZE];

  bool err = TRUE;

  // skip comments written
  while (wxFgets(buf, BUFSIZE, tfile)) {
    if (wxStrstr(buf, wxT("EndComments"))) {
      err = FALSE;
      break;
    }
  }

  if (err) {
    fclose(tfile);
    CHECK_BOOL(! err, return);
  }

  m_pstream = wxFopen(m_origFilename.c_str(), wxT("w"));
  CHECK_POINTER(m_pstream, return);

  // write our own comments
  wxFprintf(m_pstream, wxT("%%!PS-Adobe-2.0 EPSF-2.0\n") );
  wxFprintf(m_pstream, wxT("%%%%Title: %s\n"), m_title.c_str() );
  wxFprintf(m_pstream, wxT("%%%%Creator: wxWindows PostScript renderer + Snoopy\n") );
  wxFprintf(m_pstream, wxT("%%%%CreationDate: %s\n"), wxNow().c_str() );
  if (m_printData.GetOrientation() == wxLANDSCAPE)
    wxFprintf( m_pstream, wxT("%%%%Orientation: Landscape\n") );
  else
    wxFprintf(m_pstream, wxT("%%%%Orientation: Portrait\n") );
  wxFprintf(m_pstream, wxT("%%%%BoundingBox: %d %d %d %d\n"),
	  m_x1, m_y1, m_x2+1, m_y2+1);
  wxFprintf(m_pstream, wxT("%%%%EndComments\n\n") );


  // now just copy everything from the old to the new file
  while (wxFgets(buf, BUFSIZE, tfile))
  {
	  wxFputs(buf, m_pstream);
  }


  fclose(tfile);
  fclose( m_pstream );
  m_pstream = (FILE *) NULL;

  wxRemoveFile(m_printData.GetFilename());

}

SP_EPSDC::~SP_EPSDC ()
{
}

void
SP_EPSDC::SetBrush( const wxBrush& brush )
{
  if (!brush.Ok()) return;

  // wxPostScriptDC does not care about fill styles :(
  // we will have to implement them, at the moment just
  // set color to light gray if fill style is not wxTRANSPARENT
  // or wxSOLID it prevents from drawin black filled shapes
  if (brush.GetStyle() != wxTRANSPARENT &&
      brush.GetStyle() != wxSOLID &&
      brush.GetColour() == *wxBLACK)
  {
    wxBrush t = brush;
    t.SetColour(*wxLIGHT_GREY);
    wxPostScriptDC::SetBrush(t);
  } else {
    wxPostScriptDC::SetBrush(brush);
  }
}

void
SP_EPSDC::CalcBoundingBox(wxCoord x, wxCoord y)
{
  if (LogicalToDeviceX(x) < m_x1) m_x1 = LogicalToDeviceX(x);
  if (LogicalToDeviceX(x) > m_x2) m_x2 = LogicalToDeviceX(x);
  if (LogicalToDeviceY(y) < m_y1) m_y1 = LogicalToDeviceY(y);
  if (LogicalToDeviceY(y) > m_y2) m_y2 = LogicalToDeviceY(y);
  wxPostScriptDC::CalcBoundingBox(x, y);
}

void
SP_EPSDC::DoDrawBitmap(const wxBitmap& bitmap, wxCoord x, wxCoord y, bool useMask)
{
    if (m_pen.GetStyle() == wxTRANSPARENT) return;

    wxPostScriptDC::DoDrawBitmap(bitmap, x, y, useMask);
    CalcBoundingBox(x, y);
    CalcBoundingBox(x + bitmap.GetWidth(), y + bitmap.GetHeight());
}

void
SP_EPSDC::DoDrawLines (int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset)
{
    wxCHECK_RET( m_ok && m_pstream, wxT("invalid postscript dc") );

    if (m_pen.GetStyle() == wxTRANSPARENT) return;

    if (n <= 0) return;

    SetPen (m_pen);

    int i;
    for ( i =0; i < n ; i++ )
    {
      CalcBoundingBox( points[i].x+xoffset,
		       points[i].y+yoffset);

    //	was bug in wxPostscriptDC!
    //         CalcBoundingBox( LogicalToDeviceX(points[i].x+xoffset),
    // 			 LogicalToDeviceY(points[i].y+yoffset));
    }

    PsPrintf(
	     wxT("newpath\n")
	     wxT("%d %d moveto\n"),
	     LogicalToDeviceX(points[0].x+xoffset),
	     LogicalToDeviceY(points[0].y+yoffset) );

    for (i = 1; i < n; i++)
    {
        PsPrintf(
		 wxT("%d %d lineto\n"),
		 LogicalToDeviceX(points[i].x+xoffset),
		 LogicalToDeviceY(points[i].y+yoffset) );
    }

    PsPrint( wxT("stroke\n") );
}

void
SP_EPSDC::DoDrawText( const wxString& text, wxCoord x, wxCoord y )
{
    if (m_pen.GetStyle() == wxTRANSPARENT) return;

	wxPostScriptDC::DoDrawText(text, x, y);
	int l_nSize = m_font.GetPointSize();
	CalcBoundingBox( x, y );
	CalcBoundingBox( x + l_nSize * text.Length(), y + l_nSize);
}

void
SP_EPSDC::DoDrawEllipse (wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    if (m_pen.GetStyle() == wxTRANSPARENT) return;

  wxPostScriptDC::DoDrawEllipse(x, y, width, height);
  CalcBoundingBox(x - width - m_pen.GetWidth(),
		  y - height - m_pen.GetWidth());
  CalcBoundingBox(x + width + m_pen.GetWidth(),
		  y + height + m_pen.GetWidth());
}


void
SP_EPSDC::DoDrawRectangle (wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    if (m_pen.GetStyle() == wxTRANSPARENT) return;

  wxPostScriptDC::DoDrawRectangle(x, y, width, height);
  CalcBoundingBox(x - m_pen.GetWidth(),
		  y - m_pen.GetWidth());
  CalcBoundingBox(x + width + m_pen.GetWidth(),
		  y + height + m_pen.GetWidth());
}

void SP_EPSDC::DoDrawRoundedRectangle (wxCoord x, wxCoord y, wxCoord width,
				       wxCoord height, double radius)
{
    if (m_pen.GetStyle() == wxTRANSPARENT) return;

  wxPostScriptDC::DoDrawRoundedRectangle(x, y, width, height, radius);
  CalcBoundingBox(x - m_pen.GetWidth(),
		  y - m_pen.GetWidth());
  CalcBoundingBox(x + width + m_pen.GetWidth(),
		  y + height + m_pen.GetWidth());

}


void SP_EPSDC::DoDrawSpline(wxList *points)
{
    if (m_pen.GetStyle() == wxTRANSPARENT) return;

    wxPostScriptDC::DoDrawSpline(points);
}

void SP_EPSDC::DoDrawPolygon (int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, int fillStyle)
{
    if (m_pen.GetStyle() == wxTRANSPARENT) return;

    int l_nStyle = m_pen.GetStyle();
    //m_pen.SetStyle(wxTRANSPARENT);

	wxPostScriptDC::DoDrawPolygon (n, points, xoffset, yoffset, fillStyle);
	m_pen.SetStyle(l_nStyle);
}
#endif
