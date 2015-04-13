/////////////////////////////////////////////////////////////////////////////
// Author:      crohr
// $Date: 2011/12/22 $
// Short Description: dc to write latex files. Implementation based on
//                    code from wxPostScriptDC
/////////////////////////////////////////////////////////////////////////////

#include "export/SP_LatexDC.h"
#include "sp_utilities.h"
#include <wx/paper.h>
#include <wx/tokenzr.h>
#include <wx/filename.h>

#define RAD2DEG 57.29577951308

#if wxABI_VERSION > 30000

//-------------------------------------------------------------------------------
// SP_LatexDC
//-------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(SP_LatexDC, wxDC)

SP_LatexDC::SP_LatexDC()
 : wxDC(new SP_LatexDCImpl(this))
{
}

SP_LatexDC::SP_LatexDC(const wxPrintData& printData)
 : wxDC(new SP_LatexDCImpl(this, printData))
{
}

//-------------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(SP_LatexDCImpl, wxDCImpl)

SP_LatexDCImpl::SP_LatexDCImpl (SP_LatexDC *owner)
	: wxDCImpl(owner)
{
	Init();
}

SP_LatexDCImpl::SP_LatexDCImpl (SP_LatexDC *owner, const wxPrintData& printData)
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

SP_LatexDCImpl::SP_LatexDCImpl (wxPrinterDC *owner)
	: wxDCImpl(owner)
{
	Init();
}

SP_LatexDCImpl::SP_LatexDCImpl (wxPrinterDC *owner, const wxPrintData& printData)
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

void SP_LatexDCImpl::Init()
{
	m_pstream = (FILE*) NULL;

	m_underlinePosition = 0.0;
	m_underlineThickness = 0.0;

	m_signX = 1; // default x-axis left to right
	m_signY = 1; // default y-axis top up -> bottom down

	m_scale = 1;
	m_resolution = DEF_RESOLUTION;

	m_ColourDB.clear();
}

SP_LatexDCImpl::~SP_LatexDCImpl()
{
	if (m_pstream)
	{
		fclose(m_pstream);
		m_pstream = (FILE*) NULL;
	}
}



int SP_LatexDCImpl::GrayFromColor(const wxColour& p_c)
{
	int res = (int) (p_c.Green() * 0.59 + p_c.Red() * 0.3 + p_c.Blue() * 0.11);
//   SP_LOGDEBUG(wxString::Format(wxT("GrayFromColor: res=%d (%d, %d, %d)"), res,
// 				 p_c.Red(), p_c.Green(), p_c.Blue()));
	return res;
}

wxString SP_LatexDCImpl::GetLatexFill()
{

	wxString res;
	switch (m_brush.GetStyle())
	{
	case wxTRANSPARENT: //Transparent (no fill).
		res << wxT("fill=none");
		break;
	case wxSOLID: // Solid
/*		if (m_brush.GetColour() == *wxWHITE)
		{
			//white color is set to None Fill,
			//because the black token on the places will disappear if they are white filled
			res << wxT("fill=none");
		}
		else
			if (m_brush.GetColour() == *wxBLACK)
			{
				res << wxT("fill=black"); // Black
			}
			else
			{
				res << wxT("fill=") << GetLatexColor(m_brush.GetColour());
			} //endif
*/
		res << wxT("fill=") << GetLatexColor(m_brush.GetColour());
//     SP_LOGDEBUG(wxString::Format(wxT("GetLatexFill: res=%d"), res));
		break;
	case wxBDIAGONAL_HATCH: // 	Backward diagonal hatch.
		res << wxT("pattern=north west lines"); // 30 degree left diagonal pattern
		res << wxT(", pattern color=") << GetLatexColor(m_brush.GetColour());
		break;
	case wxCROSSDIAG_HATCH: // 	Cross-diagonal hatch.
		res << wxT("pattern=crosshatch"); // 30 degree crosshatch
		res << wxT(", pattern color=") << GetLatexColor(m_brush.GetColour());
		break;
	case wxFDIAGONAL_HATCH: // 	Forward diagonal hatch.
		res << wxT("pattern=north east lines"); // 30 degree right diagonal pattern
		res << wxT(", pattern color=") << GetLatexColor(m_brush.GetColour());
		break;
	case wxCROSS_HATCH: // Cross hatch.
		res << wxT("pattern=grid"); // crosshatch
		res << wxT(", pattern color=") << GetLatexColor(m_brush.GetColour());
		break;
	case wxHORIZONTAL_HATCH: // 	Horizontal hatch.
		res << wxT("pattern=horizontal lines"); //  horizontal lines
		res << wxT(", pattern color=") << GetLatexColor(m_brush.GetColour());
		break;
	case wxVERTICAL_HATCH: //	Vertical hatch.
		res << wxT("pattern=vertical lines"); //vertical lines
		res << wxT(", pattern color=") << GetLatexColor(m_brush.GetColour());
		break;
	default:
		res << wxT("fill=none"); // not filled
		SP_LOGDEBUG(wxString::Format(wxT("GetLatexFill - unsupported fill %d, set to %s"), m_brush.GetStyle(), res.c_str()));
		break;
	}
	return res;
}

wxString SP_LatexDCImpl::GetLatexJoin()
{
	wxString res = wxT("line join=");
	switch (m_pen.GetJoin())
	{
	case wxJOIN_BEVEL:
		res << wxT("bevel");
		break;
	case wxJOIN_ROUND:
		res << wxT("round");
		break;
	case wxJOIN_MITER:
		res << wxT("miter");
		break;
	default:
		res << wxT("miter");
		break;
	}
	return res;
}

wxString SP_LatexDCImpl::GetLatexLStyle()
{
	wxString res;
	switch (m_pen.GetStyle())
	{
	case wxSOLID: // Solid
		res = wxT("solid");
		break;
	case wxDOT:
		res = wxT("loosely dotted");
		break;
	case wxSHORT_DASH:
		res = wxT("loosely dashed");
		break;
	case wxLONG_DASH:
		res = wxT("loosely dashed");
		break;
	case wxDOT_DASH:
		res = wxT("loosely dashdotted");
		break;
	default:
		SP_LOGDEBUG(wxString::Format(wxT("GetLatexLStyle - unsupported style %d, set to %s"), m_brush.GetStyle(), res.c_str()));
		break;
	}
	return res;
}

wxString SP_LatexDCImpl::GetLatexLineWidth()
{
	wxString res = wxT("line width=");
	res << m_pen.GetWidth() << wxT("");
	return res;
}

/**
 * 	Notice: this function writes to the file if a color wasn't used before.
 */
wxString SP_LatexDCImpl::GetLatexColor(const wxColour& color)
{
	wxString l_sColor = color.GetAsString();
	colourDB_t::iterator itC = m_ColourDB.find(l_sColor);
	if(itC != m_ColourDB.end())
	{
		return (*itC).second;
	}
	else
	{
		wxString res = wxTheColourDatabase->FindName(color);
		if(res.IsEmpty())
		{
			res = wxT("");
			res << wxT("r") << color.Red();
			res << wxT("g") << color.Green();
			res << wxT("b") << color.Blue();
		}
		m_ColourDB.insert(make_pair(l_sColor, res));
		wxFprintf(m_pstream, wxT("\\definecolor{%s}{RGB}{%u,%u,%u}\n"), res.c_str(), color.Red(), color.Green(), color.Blue());
		return res;
	}
}

wxString SP_LatexDCImpl::GetLatexPColor()
{
	return GetLatexColor(m_pen.GetColour());
}

wxString SP_LatexDCImpl::GetLatexFColor()
{
	wxString res = wxT("fill=");
	res << GetLatexColor(m_brush.GetColour());
	return res;
}

wxString SP_LatexDCImpl::GetLatexLCap()
{
	wxString res = wxT("line cap=");
	switch (m_pen.GetCap())
	{
	case wxCAP_ROUND:
		res << wxT("round");
		break;
	case wxCAP_PROJECTING:
		res << wxT("rect");
		break;
	case wxCAP_BUTT:
		res << wxT("butt");
		break;
	default:
		res << wxT("butt");
	}
	return res;
}

// latex fonts, font_flags bit 2 should be 0
// TODO: Postscript fonts support
wxString SP_LatexDCImpl::GetLatexFont()
{
	wxString res = wxT("font=");
	switch (m_font.GetFamily())
	{
	case wxFONTFAMILY_DEFAULT:
		res << wxT("\\normalfont"); // default
		break;
	case wxFONTFAMILY_SWISS: // A sans-serif font.
		res << wxT("\\sffamily"); // Sans Serif
		break;
	case wxFONTFAMILY_MODERN:
	case wxFONTFAMILY_SCRIPT:
	case wxFONTFAMILY_TELETYPE: // A monospace font.
		res << wxT("\\ttfamily"); // Teletype
		break;
	case wxFONTFAMILY_DECORATIVE:
	case wxFONTFAMILY_ROMAN:
		res << wxT("\\rmfamily"); // Roman
		break;
	default:
		res << wxT("\\normalfont"); // default
		SP_LOGDEBUG(wxString::Format(wxT("GetLatexFont - unsupported family %d, set to %s"), m_font.GetFamily(), res.c_str()));
		break;
	}

	if (m_font.GetStyle() == wxFONTSTYLE_ITALIC)
	{
		res << wxT("\\itshape"); // italic
		SP_LOGDEBUG(wxT("Font set to italic"));
	}
	else if (m_font.GetStyle() == wxFONTSTYLE_SLANT)
	{
		res << wxT("\\slshape"); // slanted
		SP_LOGDEBUG(wxT("Font set to slanted"));
	}

	if (m_font.GetWeight() == wxFONTWEIGHT_BOLD)
	{
		res << wxT("\\bfseries"); // bold
		SP_LOGDEBUG(wxT("Font set to bold"));
	}
	else if (m_font.GetWeight() == wxFONTWEIGHT_LIGHT)
	{
		res << wxT("\\mdseries"); // medium
		SP_LOGDEBUG(wxT("Font set to medium"));
	}

	switch (m_font.GetPointSize())
	{
		case 5:
		case 6:
			res << wxT("\\tiny");
			break;
		case 7:
			res << wxT("\\scriptsize");
			break;
		case 8:
			res << wxT("\\footnotesize");
			break;
		case 9:
			res << wxT("\\small");
			break;
		case 10:
		case 11:
			res << wxT("\\normalsize");
			break;
		case 12:
		case 13:
			res << wxT("\\large");
			break;
		case 14:
		case 15:
		case 16:
			res << wxT("\\Large");
			break;
		case 17:
		case 18:
		case 19:
			res << wxT("\\LARGE");
			break;
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
			res << wxT("\\huge");
			break;
		case 25:
			res << wxT("\\Huge");
			break;
		default:
			res << wxT("\\normalsize");
			break;
	}

	return res;
}

//------------------------------------------------------------------------------

bool SP_LatexDCImpl::StartDoc(const wxString& message)
{
	//  SP_LOGDEBUG(wxString::Format(wxT("StartDoc %s"), message.c_str()));
	wxCHECK_MSG( m_ok, FALSE, wxT("invalid latex dc"));

	m_bitmapCount = 0;

	if (m_printData.GetFilename() == wxT(""))
	{
		wxString filename = wxFileName::CreateTempFileName(wxT("tex"));
		SP_LOGDEBUG(wxString::Format(wxT("writing to temporary file %s"), filename.c_str()));
		m_printData.SetFilename(filename);
	}

	m_pstream = wxFopen(m_printData.GetFilename().c_str(), wxT("w+"));

	if (!m_pstream)
	{
		SP_LOGERROR(_("Cannot open file for Latex output!"));
		m_ok = FALSE;
		return FALSE;
	}

	m_ok = TRUE;

	wxFprintf(m_pstream, wxT("\\documentclass{article}\n"));
	wxFprintf(m_pstream, wxT("\n"));
	wxFprintf(m_pstream, wxT("\\usepackage{pgf}\n"));
	wxFprintf(m_pstream, wxT("\\usepackage{tikz}\n"));
	wxFprintf(m_pstream, wxT("\\usetikzlibrary{positioning,arrows,shapes,backgrounds,calc,patterns}\n"));
	wxFprintf(m_pstream, wxT("\n"));
	wxFprintf(m_pstream, wxT("\\usepackage[graphics,tightpage,active]{preview}\n"));
	wxFprintf(m_pstream, wxT("\\PreviewEnvironment{tikzpicture}\n"));
	wxFprintf(m_pstream, wxT("\\PreviewEnvironment{equation}\n"));
	wxFprintf(m_pstream, wxT("\\PreviewEnvironment{equation*}\n"));
	wxFprintf(m_pstream, wxT("\\newlength{\\imagewidth}\n"));
	wxFprintf(m_pstream, wxT("\\newlength{\\imagescale}\n"));
	wxFprintf(m_pstream, wxT("\\pagestyle{empty}\n"));
	wxFprintf(m_pstream, wxT("\\thispagestyle{empty}\n"));
	wxFprintf(m_pstream, wxT("\n"));
	wxFprintf(m_pstream, wxT("\\begin{document}\n"));
	wxFprintf(m_pstream, wxT("\n"));
	wxFprintf(m_pstream, wxT("\\begin{tikzpicture}[x=1pt,y=-1pt]\n"));
	wxFprintf(m_pstream, wxT("\n"));
	return TRUE;
}

void SP_LatexDCImpl::EndDoc()
{
	//  SP_LOGDEBUG(wxT("EndDoc"));
	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

	wxFprintf(m_pstream, wxT("\\end{tikzpicture}\n\n"));
	wxFprintf(m_pstream, wxT("\\end{document}\n\n"));

	fclose(m_pstream);
	m_pstream = (FILE *) NULL;
}

//------------------------------------------------------------------------------

void SP_LatexDCImpl::DoDrawBitmap(const wxBitmap& bitmap, wxCoord x, wxCoord y, bool useMask)
{
	SP_LOGDEBUG(wxString::Format(wxT("DoDrawBitmap %d %d"), x, y));

	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

	if(bitmap.IsOk())
	{
		wxString l_sFilename = m_printData.GetFilename().BeforeLast('.');
		l_sFilename << wxT("_") << m_bitmapCount << wxT(".png");
		bitmap.SaveFile(l_sFilename,wxBITMAP_TYPE_PNG);

		double tX = (x * m_scale) + (bitmap.GetWidth()/2 * m_scale);
		double tY = (y * m_scale) + (bitmap.GetHeight()/2 * m_scale);

		wxString out = wxT("\\draw[");
		out << wxT("]\n\t");
		out << wxT("(") << tX << wxT(",") << tY << wxT(")");
		out << wxT(" node[");
		out << wxT("]\n");
		out << wxT("\t{\\includegraphics{") << l_sFilename << wxT("}}");
		out << wxT(";\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}
}

void SP_LatexDCImpl::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
	//  SP_LOGDEBUG(wxString::Format(wxT("DoDrawEllipse %d %d %d %d"),
	//				 x, y,
	//			 width, height));
	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

	if (m_pen.GetStyle() != wxTRANSPARENT && m_pen.GetColour() != *wxWHITE)
	{
		double tX = (x * m_scale);
		double tY = (y * m_scale);
		double radiusX = ((width * m_scale)) / 2;
		double radiusY = ((height * m_scale)) / 2;

		wxString out = wxT("\\draw[");
		out << GetLatexPColor() << wxT(", ");
		out << GetLatexLStyle() << wxT(", ");
		out << GetLatexJoin() << wxT(", ");
		out << GetLatexLCap() << wxT(", ");
		out << GetLatexLineWidth() << wxT(", ");
		out << GetLatexFill();
		out << wxT("]\n\t");
		out << wxT("(") << tX+radiusX << wxT(",") << tY+radiusY << wxT(")");
		out << wxT(" ellipse[");
		out << wxT("x radius=") << radiusX << wxT(", y radius=") << radiusY << wxT("]");
		out << wxT(";\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}
}

void SP_LatexDCImpl::DoDrawArc(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord xc, wxCoord yc)
{
//  SP_LOGDEBUG(wxString::Format(wxT("DoDrawArc %d %d %d %d %d %d"),
//				 x1, y1, x2,
//				 y2, xc,  yc));
	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

	if (m_pen.GetStyle() != wxTRANSPARENT && m_pen.GetColour() != *wxWHITE)
	{
		double dx = x1 - xc;
		double dy = y1 - yc;
		double radius = sqrt( dx*dx+dy*dy );
	    double alpha1, alpha2;

	    if (x1 == x2 && y1 == y2)
	    {
	        alpha1 = 0.0;
	        alpha2 = 360.0;
	    }
	    else if ( wxIsNullDouble(radius) )
	    {
	        alpha1 =
	        alpha2 = 0.0;
	    }
	    else
	    {
	        alpha1 = (x1 - xc == 0) ?
	            (y1 - yc < 0) ? 90.0 : -90.0 :
	                -atan2(double(y1-yc), double(x1-xc)) * RAD2DEG;
	        alpha2 = (x2 - xc == 0) ?
	            (y2 - yc < 0) ? 90.0 : -90.0 :
	                -atan2(double(y2-yc), double(x2-xc)) * RAD2DEG;
	    }
	    while (alpha1 <= 0)   alpha1 += 360;
	    while (alpha2 <= 0)   alpha2 += 360; // adjust angles to be between
	    while (alpha1 > 360)  alpha1 -= 360; // 0 and 360 degree
	    while (alpha2 > 360)  alpha2 -= 360;

		double tX = (xc * m_scale);
		double tY = (yc * m_scale);

		wxString out = wxT("\\draw[");
		out << GetLatexPColor() << wxT(", ");
		out << GetLatexLStyle() << wxT(", ");
		out << GetLatexJoin() << wxT(", ");
		out << GetLatexLCap() << wxT(", ");
		out << GetLatexLineWidth() << wxT(", ");
		out << GetLatexFill();
		out << wxT("]\n\t");
		out << wxT("(") << tX << wxT(",") << tY << wxT(")");
		out << wxT(" arc[");
		out << wxT("start angle=") << alpha1 << wxT(", end angle=") << alpha2 << wxT("]");
		out << wxT(" -- cycle;\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}
}

void SP_LatexDCImpl::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{
//  SP_LOGDEBUG(wxString::Format(wxT("DoDrawText %s %d %d"), text.c_str(),
//				 x, y));

	DoDrawRotatedText(text, x, y, 0);

}

void SP_LatexDCImpl::DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle)
{
//  SP_LOGDEBUG(wxString::Format(wxT("DoDrawRotatedText %s %d %d %g"),
//				 text.c_str(), x, y, angle));
	if (m_pen.GetStyle() != wxTRANSPARENT && m_pen.GetColour() != *wxWHITE)
	{

		wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

		double tX = (x * m_scale);//((x + GetCharWidth() / 2.0) * m_scale);
		double tY = ((y + GetCharHeight() / 2.0) * m_scale);

		wxString escaped_text = text;
		escaped_text.Replace(wxT("\\"), wxT("\\textbackslash "));
		escaped_text.Replace(wxT("{"), wxT("\\{"));
		escaped_text.Replace(wxT("}"), wxT("\\}"));
		escaped_text.Replace(wxT("#"), wxT("\\#"));
		escaped_text.Replace(wxT("$"), wxT("\\$"));
		escaped_text.Replace(wxT("%"), wxT("\\%"));
		escaped_text.Replace(wxT("&"), wxT("\\&"));
		escaped_text.Replace(wxT("_"), wxT("\\_"));
		escaped_text.Replace(wxT("^"), wxT("\\^{}"));
		escaped_text.Replace(wxT("~"), wxT("\\~{}"));
		escaped_text.Replace(wxT("|"), wxT("\\textbar "));
		escaped_text.Replace(wxT("<"), wxT("\\textless "));
		escaped_text.Replace(wxT(">"), wxT("\\textgreater "));

		wxString out = wxT("\\draw[");
		out << GetLatexPColor();
//		out << GetLatexLStyle() << wxT(", ");
//		out << GetLatexJoin() << wxT(", ");
//		out << GetLatexLCap() << wxT(", ");
//		out << GetLatexLineWidth() << wxT(", ");
//		out << GetLatexFill();
		out << wxT("]\n\t");
		out << wxT("(") << tX << wxT(",") << tY << wxT(")");
		out << wxT(" node[");
		out << wxT("rotate=") << angle;
		out << wxT(", ") << GetLatexFont();
		out << wxT(", ") << GetLatexColor(m_textForegroundColour);
		out << wxT(", right=-.25]\n");
		out << wxT("\t{") << escaped_text << wxT("}");
		out << wxT(";\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}
}

void SP_LatexDCImpl::DoDrawLines(int n, const wxPoint points[], wxCoord xoffset, wxCoord yoffset)
{
//     SP_LOGDEBUG(wxString::Format(wxT("DoDrawLines %d %d %d"),
// 		n, xoffset, yoffset));
	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

	if (m_pen.GetStyle() != wxTRANSPARENT && m_pen.GetColour() != *wxWHITE)
	{
		wxString out = wxT("\\draw[");
		out << GetLatexPColor() << wxT(", ");
		out << GetLatexLStyle() << wxT(", ");
		out << GetLatexJoin() << wxT(", ");
		out << GetLatexLCap() << wxT(", ");
		out << GetLatexLineWidth();
		out << wxT("]\n\t");
		double tX, tY;
		for (int i = 0; i < n; i++)
		{
			tX = (points[i].x * m_scale + xoffset * m_scale);
			tY = (points[i].y * m_scale + yoffset * m_scale);
			if(i > 0)
			{
				out << wxT(" -- ");
			}
			out << wxT("(") << tX << wxT(",") << tY << wxT(")");
		}
		out << wxT(";\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}
}

void SP_LatexDCImpl::DoDrawPoint(wxCoord x, wxCoord y)
{
//	SP_LOGDEBUG(wxString::Format(wxT("DoDrawPoint %d %d"), x, y));

	wxPoint tmp[] =
	{ wxPoint(x, y), wxPoint(x, y) };
	DoDrawLines(2, tmp);
}

void SP_LatexDCImpl::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
//	SP_LOGDEBUG(wxString::Format(wxT("DoDrawLine %d %d %d %d"), x1, y1, x2, y2));

	wxPoint tmp[] =
	{ wxPoint(x1, y1), wxPoint(x2, y2) };
	DoDrawLines(2, tmp);
}

void SP_LatexDCImpl::DoDrawPolygon(int n, const wxPoint points[], wxCoord xoffset, wxCoord yoffset, wxPolygonFillMode fillStyle)
{
//    SP_LOGDEBUG(wxString::Format(wxT("DoDrawPolygon %d, style %d tr:%d"),
//  		n, m_pen.GetStyle(), m_pen.GetStyle() == wxTRANSPARENT ));
	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

	if (m_pen.GetStyle() != wxTRANSPARENT && m_pen.GetColour() != *wxWHITE)
	{
		wxString out = wxT("\\draw[");
		out << GetLatexPColor() << wxT(", ");
		out << GetLatexLStyle() << wxT(", ");
		out << GetLatexJoin() << wxT(", ");
		out << GetLatexLCap() << wxT(", ");
		out << GetLatexLineWidth() << wxT(", ");
		out << GetLatexFill();
		out << wxT("]\n\t");
		double tX, tY;
		for (int i = 0; i < n; i++)
		{
			tX = (points[i].x * m_scale + xoffset * m_scale);
			tY = (points[i].y * m_scale + yoffset * m_scale);
			if(i > 0)
			{
				out << wxT(" -- ");
			}
			out << wxT("(") << tX << wxT(",") << tY << wxT(")");
		}
		out << wxT(" -- cycle;\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}
}

void SP_LatexDCImpl::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
//    SP_LOGDEBUG(wxString::Format(wxT("DoDrawRectangle %d %d %d %d"),
//  				 x, y,
//  				 width, height));

	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc") );
	if (m_pen.GetStyle() != wxTRANSPARENT && m_pen.GetColour() != *wxWHITE)
	{
		double tX = (x * m_scale);
		double tY = (y * m_scale);
		double tW = (width * m_scale);
		double tH = (height * m_scale);
		wxString out = wxT("\\draw[");
		out << GetLatexPColor() << wxT(", ");
		out << GetLatexLStyle() << wxT(", ");
		out << GetLatexJoin() << wxT(", ");
		out << GetLatexLCap() << wxT(", ");
		out << GetLatexLineWidth() << wxT(", ");
		out << GetLatexFill();
		out << wxT("]\n\t");
		out << wxT("(") << tX << wxT(",") << tY << wxT(")");
		out << wxT(" rectangle ");
		out << wxT("+(") << tW << wxT(",") << tH << wxT(")");
		out << wxT(";\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}
}

void SP_LatexDCImpl::DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius)
{
//   SP_LOGDEBUG(wxString::Format(wxT("DoDrawRoundedRectangle %d %d %d %d %g"),
// 				 x, y,
// 				 width, height,
// 				 radius));

	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

	if (m_pen.GetStyle() != wxTRANSPARENT && m_pen.GetColour() != *wxWHITE)
	{
		double tX = (x * m_scale);
		double tY = (y * m_scale);
		double tW = (width * m_scale);
		double tH = (height * m_scale);
		wxString out = wxT("\\draw[rounded corners, ");
		out << GetLatexPColor() << wxT(", ");
		out << GetLatexLStyle() << wxT(", ");
		out << GetLatexJoin() << wxT(", ");
		out << GetLatexLCap() << wxT(", ");
		out << GetLatexLineWidth() << wxT(", ");
		out << GetLatexFill();
		out << wxT("]\n\t");
		out << wxT("(") << tX << wxT(",") << tY << wxT(")");
		out << wxT(" rectangle ");
		out << wxT("+(") << tW << wxT(",") << tH << wxT(")");
		out << wxT(";\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}
}

void SP_LatexDCImpl::DoDrawSpline(const wxList *points)
{
//	SP_LOGDEBUG(wxT("DoDrawSpline"));
	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

	if (m_pen.GetStyle() != wxTRANSPARENT && m_pen.GetColour() != *wxWHITE)
	{
	    double c, d, x1, y1, x2, y2, x3, y3;
	    wxPoint *p, *q;

	    wxList::compatibility_iterator node = points->GetFirst();
	    p = (wxPoint *)node->GetData();
	    x1 = p->x;
	    y1 = p->y;

	    node = node->GetNext();
	    p = (wxPoint *)node->GetData();
	    c = p->x;
	    d = p->y;
	    x3 = (double)(x1 + c) / 2;
	    y3 = (double)(y1 + d) / 2;

		wxString out = wxT("\\draw[");
		out << GetLatexPColor() << wxT(", ");
		out << GetLatexLStyle() << wxT(", ");
		out << GetLatexJoin() << wxT(", ");
		out << GetLatexLCap() << wxT(", ");
		out << GetLatexLineWidth();
		out << wxT("]\n\t");
		out << wxT("(") << x1 << wxT(",") << y1 << wxT(")");
		out << wxT(" -- ");
		out << wxT("(") << x3 << wxT(",") << y3 << wxT(")");

		node = node->GetNext();
	    while (node)
	    {
	        q = (wxPoint *)node->GetData();

	        x1 = x3;
	        y1 = y3;
	        x2 = c;
	        y2 = d;
	        c = q->x;
	        d = q->y;
	        x3 = (double)(x2 + c) / 2;
	        y3 = (double)(y2 + d) / 2;

			out << wxT(" .. controls (") << x2 << wxT(",") << y2 << wxT(")");
			out << wxT(" .. ");
			out << wxT("(") << x3 << wxT(",") << y3 << wxT(")");

	        node = node->GetNext();
	    }
		out << wxT(" -- (") << c << wxT(",") << d << wxT(")");
		out << wxT(";\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}
}


//------------------------------------------------------------------------------
void SP_LatexDCImpl::SetBackground(const wxBrush& brush)
{
	SP_LOGDEBUG(wxT("SetBackground"));
	m_backgroundBrush = brush;
}

void SP_LatexDCImpl::SetResolution(int ppi)
{
	m_resolution = ppi;
	m_scale = (float) ppi / DEF_RESOLUTION;
}

int SP_LatexDCImpl::GetResolution() const
{
	return m_resolution;
}

void SP_LatexDCImpl::SetFont(const wxFont& font)
{
//   SP_LOGDEBUG(wxT("SetFont"));
	m_font = font;
}

void SP_LatexDCImpl::SetPen(const wxPen& pen)
{
//   SP_LOGDEBUG(wxT("SetPen"));
	m_pen = pen;
}

void SP_LatexDCImpl::SetBrush(const wxBrush& brush)
{
//   SP_LOGDEBUG(wxT("SetBrush"));
	m_brush = brush;
}

bool SP_LatexDCImpl::Ok() const
{
	return m_ok;
}

// from wxPostscriptDC, not tested
//------------------------------------------------------------------------------

void SP_LatexDCImpl::SetDeviceOrigin(wxCoord x, wxCoord y)
{
	SP_LOGDEBUG(wxString::Format(wxT("SetDeviceOrigin %d %d"), x, y));
	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

	int h = 0;
	int w = 0;
	GetSize(&w, &h);

	wxDCImpl::SetDeviceOrigin(x, h - y);
}

void SP_LatexDCImpl::DoGetSize(int* width, int* height) const
{
	//  SP_LOGDEBUG(wxT("DoGetSize"));
	wxPaperSize id = m_printData.GetPaperId();

	wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(id);

	if (!paper)
		paper = wxThePrintPaperDatabase->FindPaperType(wxPAPER_A4);

	int w = 595;
	int h = 842;
	if (paper)
	{
		w = paper->GetSizeDeviceUnits().x;
		h = paper->GetSizeDeviceUnits().y;
	}

	if (m_printData.GetOrientation() == wxLANDSCAPE)
	{
		int tmp = w;
		w = h;
		h = tmp;
	}

	if (width)
		*width = (int) (w * m_scale);
	if (height)
		*height = (int) (h * m_scale);
}

void SP_LatexDCImpl::DoGetSizeMM(int *width, int *height) const
{
	//  SP_LOGDEBUG(wxT("DoGetSizeMM"));
	wxPaperSize id = m_printData.GetPaperId();

	wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(id);

	if (!paper)
		paper = wxThePrintPaperDatabase->FindPaperType(wxPAPER_A4);

	int w = 210;
	int h = 297;

	if (paper)
	{
		w = paper->GetWidth() / 10;
		h = paper->GetHeight() / 10;
	}

	if (m_printData.GetOrientation() == wxLANDSCAPE)
	{
		int tmp = w;
		w = h;
		h = tmp;
	}

	if (width)
		*width = w;
	if (height)
		*height = h;
}

wxCoord SP_LatexDCImpl::GetCharHeight() const
{
	//  SP_LOGDEBUG(wxT("GetCharHeight"));
	if (m_font.Ok())
		return m_font.GetPointSize() + 1;
	else
		return 12;
}

wxCoord SP_LatexDCImpl::GetCharWidth() const
{
	//  SP_LOGDEBUG(wxT("GetCharWidth"));
	// Chris Breeze: reasonable approximation using wxMODERN/Courier
	return (wxCoord) (GetCharHeight() * DEF_RESOLUTION / 120.0);
}

// Resolution in pixels per logical inch
wxSize SP_LatexDCImpl::GetPPI(void) const
{
	//  SP_LOGDEBUG(wxT("GetPPI"));
	return wxSize(m_resolution, m_resolution);
}

// do nothing
//------------------------------------------------------------------------------

void SP_LatexDCImpl::StartPage()
{
	SP_LOGDEBUG(wxT("StartPage"));
}

void SP_LatexDCImpl::EndPage()
{
	SP_LOGDEBUG(wxT("EndPage"));
}

void SP_LatexDCImpl::DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
//   SP_LOGDEBUG(wxString::Format(wxT("DoSetClippingRegion %d %d %d %d"),
// 				 x, y, w, h));
}

void SP_LatexDCImpl::DoSetDeviceClippingRegion(const wxRegion& r)
{
//   SP_LOGDEBUG(wxString::Format(wxT("DoSetClippingRegion %d %d %d %d"),
// 				 x, y, w, h));
}

void SP_LatexDCImpl::DestroyClippingRegion()
{
	//  SP_LOGDEBUG(wxT("DestroyClippingRegion"));
}

// not implemented
//------------------------------------------------------------------------------

bool SP_LatexDCImpl::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord fwidth, wxCoord fheight, wxDC *source, wxCoord xsrc, wxCoord ysrc, wxRasterOperationMode rop, bool useMask, wxCoord xsrcMask, wxCoord ysrcMask)
{
	SP_LOGDEBUG(wxT("DoBlit"));
	wxFAIL_MSG( wxT("SP_LatexDCImpl::DoBlit not implemented.") );
	return FALSE;
}

void SP_LatexDCImpl::DoGetTextExtent(const wxString& string, wxCoord *x, wxCoord *y, wxCoord *descent, wxCoord *externalLeading, const wxFont *theFont) const
{
	SP_LOGDEBUG(wxT("DoGetTextExtent"));
	wxFAIL_MSG( wxT("SP_LatexDCImpl::DoGetTextExtent not implemented.") );
}

void SP_LatexDCImpl::Clear()
{
	SP_LOGDEBUG(wxT("Clear"));
	wxFAIL_MSG( wxT("SP_LatexDCImpl::Clear not implemented.") );
}

void SP_LatexDCImpl::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h, double sa, double ea)
{
	SP_LOGDEBUG(wxString::Format(wxT("DoDrawEllipticArc %d %d %d %d %g %g"), x, y, w, h, sa, ea));
	wxFAIL_MSG( wxT("SP_LatexDCImpl::DoDrawEllipticArc not implemented.") );
}

bool SP_LatexDCImpl::DoFloodFill(wxCoord x, wxCoord y, const wxColour &col, wxFloodFillStyle style)
{
	SP_LOGDEBUG(wxString::Format(wxT("DoFloodFill %d %d, color: %d %d %d, style %d"), x, y, col.Red(), col.Green(), col.Blue(), style));
	wxFAIL_MSG( wxT("SP_LatexDCImpl::FloodFill not implemented.") );
	return FALSE;
}

bool SP_LatexDCImpl::DoGetPixel(wxCoord x, wxCoord y, wxColour * col) const
{
	SP_LOGDEBUG(wxString::Format(wxT("DoGetPixel %d %d, color: %d %d %d"), x, y, col->Red(), col->Green(), col->Blue()));
	wxFAIL_MSG( wxT("SP_LatexDCImpl::GetPixel not implemented.") );
	return FALSE;
}

void SP_LatexDCImpl::DoCrossHair(wxCoord x, wxCoord y)
{
	SP_LOGDEBUG(wxString::Format(wxT("DoCrossHair %d %d"), x, y));
	wxFAIL_MSG( wxT("SP_LatexDCImpl::CrossHair not implemented.") );
}

void SP_LatexDCImpl::SetLogicalFunction(wxRasterOperationMode function)
{
	SP_LOGDEBUG(wxT("SetLogicalFunction"));
	wxFAIL_MSG( wxT("SP_LatexDCImpl::SetLogicalFunction not implemented.") );
}

void SP_LatexDCImpl::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{
	SP_LOGDEBUG(wxString::Format(wxT("DoDrawIcon %d %d"), x, y));
	wxFAIL_MSG( wxT("SP_LatexDCImpl::DoDrawIcon not implemented.") );
}

void SP_LatexDCImpl::SetAxisOrientation(bool xLeftRight, bool yBottomUp)
{
	SP_LOGDEBUG(wxString::Format(wxT("SetAxisOrientation %d %d"), xLeftRight, yBottomUp));
	wxFAIL_MSG( wxT("SP_LatexDCImpl::SetAxisOrientation not implemented.") );

}
#else
//-------------------------------------------------------------------------------
// SP_LatexDC
//-------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(SP_LatexDC, wxDC)

//-------------------------------------------------------------------------------
int SP_LatexDC::GrayFromColor(const wxColour& p_c)
{
	int res = (int) (p_c.Green() * 0.59 + p_c.Red() * 0.3 + p_c.Blue() * 0.11);
//   SP_LOGDEBUG(wxString::Format(wxT("GrayFromColor: res=%d (%d, %d, %d)"), res,
// 				 p_c.Red(), p_c.Green(), p_c.Blue()));
	return res;
}

wxString SP_LatexDC::GetLatexFill()
{

	wxString res;
	switch (m_brush.GetStyle())
	{
	case wxTRANSPARENT: //Transparent (no fill).
		res << wxT("fill=none");
		break;
	case wxSOLID: // Solid
/*		if (m_brush.GetColour() == *wxWHITE)
		{
			//white color is set to None Fill,
			//because the black token on the places will disappear if they are white filled
			res << wxT("fill=none");
		}
		else
			if (m_brush.GetColour() == *wxBLACK)
			{
				res << wxT("fill=black"); // Black
			}
			else
			{
				res << wxT("fill=") << GetLatexColor(m_brush.GetColour());
			} //endif
*/
		res << wxT("fill=") << GetLatexColor(m_brush.GetColour());
//     SP_LOGDEBUG(wxString::Format(wxT("GetLatexFill: res=%d"), res));
		break;
	case wxBDIAGONAL_HATCH: // 	Backward diagonal hatch.
		res << wxT("pattern=north west lines"); // 30 degree left diagonal pattern
		res << wxT(", pattern color=") << GetLatexColor(m_brush.GetColour());
		break;
	case wxCROSSDIAG_HATCH: // 	Cross-diagonal hatch.
		res << wxT("pattern=crosshatch"); // 30 degree crosshatch
		res << wxT(", pattern color=") << GetLatexColor(m_brush.GetColour());
		break;
	case wxFDIAGONAL_HATCH: // 	Forward diagonal hatch.
		res << wxT("pattern=north east lines"); // 30 degree right diagonal pattern
		res << wxT(", pattern color=") << GetLatexColor(m_brush.GetColour());
		break;
	case wxCROSS_HATCH: // Cross hatch.
		res << wxT("pattern=grid"); // crosshatch
		res << wxT(", pattern color=") << GetLatexColor(m_brush.GetColour());
		break;
	case wxHORIZONTAL_HATCH: // 	Horizontal hatch.
		res << wxT("pattern=horizontal lines"); //  horizontal lines
		res << wxT(", pattern color=") << GetLatexColor(m_brush.GetColour());
		break;
	case wxVERTICAL_HATCH: //	Vertical hatch.
		res << wxT("pattern=vertical lines"); //vertical lines
		res << wxT(", pattern color=") << GetLatexColor(m_brush.GetColour());
		break;
	default:
		res << wxT("fill=none"); // not filled
		SP_LOGDEBUG(wxString::Format(wxT("GetLatexFill - unsupported fill %d, set to %s"), m_brush.GetStyle(), res.c_str()));
		break;
	}
	return res;
}

wxString SP_LatexDC::GetLatexJoin()
{
	wxString res = wxT("line join=");
	switch (m_pen.GetJoin())
	{
	case wxJOIN_BEVEL:
		res << wxT("bevel");
		break;
	case wxJOIN_ROUND:
		res << wxT("round");
		break;
	case wxJOIN_MITER:
		res << wxT("miter");
		break;
	}
	return res;
}

wxString SP_LatexDC::GetLatexLStyle()
{
	wxString res;
	switch (m_pen.GetStyle())
	{
	case wxSOLID: // Solid
		res = wxT("solid");
		break;
	case wxDOT:
		res = wxT("loosely dotted");
		break;
	case wxSHORT_DASH:
		res = wxT("loosely dashed");
		break;
	case wxLONG_DASH:
		res = wxT("loosely dashed");
		break;
	case wxDOT_DASH:
		res = wxT("loosely dashdotted");
		break;
	default:
		SP_LOGDEBUG(wxString::Format(wxT("GetLatexLStyle - unsupported style %d, set to %s"), m_brush.GetStyle(), res.c_str()));
		break;
	}
	return res;
}

wxString SP_LatexDC::GetLatexLineWidth()
{
	wxString res = wxT("line width=");
	res << m_pen.GetWidth() << wxT("");
	return res;
}

/**
 * 	Notice: this function writes to the file if a color wasn't used before.
 */
wxString SP_LatexDC::GetLatexColor(const wxColour& color)
{
	wxString l_sColor = color.GetAsString();
	colourDB_t::iterator itC = m_ColourDB.find(l_sColor);
	if(itC != m_ColourDB.end())
	{
		return (*itC).second;
	}
	else
	{
		wxString res = wxTheColourDatabase->FindName(color);
		if(res.IsEmpty())
		{
			res = wxT("");
			res << wxT("r") << color.Red();
			res << wxT("g") << color.Green();
			res << wxT("b") << color.Blue();
		}
		m_ColourDB.insert(make_pair(l_sColor, res));
		wxFprintf(m_pstream, wxT("\\definecolor{%s}{RGB}{%u,%u,%u}\n"), res.c_str(), color.Red(), color.Green(), color.Blue());
		return res;
	}
}

wxString SP_LatexDC::GetLatexPColor()
{
	return GetLatexColor(m_pen.GetColour());
}

wxString SP_LatexDC::GetLatexFColor()
{
	wxString res = wxT("fill=");
	res << GetLatexColor(m_brush.GetColour());
	return res;
}

wxString SP_LatexDC::GetLatexLCap()
{
	wxString res = wxT("line cap=");
	switch (m_pen.GetCap())
	{
	case wxCAP_ROUND:
		res << wxT("round");
		break;
	case wxCAP_PROJECTING:
		res << wxT("rect");
		break;
	case wxCAP_BUTT:
		res << wxT("butt");
		break;
	}
	return res;
}

// latex fonts, font_flags bit 2 should be 0
// TODO: Postscript fonts support
wxString SP_LatexDC::GetLatexFont()
{
	wxString res = wxT("font=");
	switch (m_font.GetFamily())
	{
	case wxFONTFAMILY_DEFAULT:
		res << wxT("\\normalfont"); // default
		break;
	case wxFONTFAMILY_SWISS: // A sans-serif font.
		res << wxT("\\sffamily"); // Sans Serif
		break;
	case wxFONTFAMILY_MODERN:
	case wxFONTFAMILY_SCRIPT:
	case wxFONTFAMILY_TELETYPE: // A monospace font.
		res << wxT("\\ttfamily"); // Teletype
		break;
	case wxFONTFAMILY_DECORATIVE:
	case wxFONTFAMILY_ROMAN:
		res << wxT("\\rmfamily"); // Roman
		break;
	default:
		res << wxT("\\normalfont"); // default
		SP_LOGDEBUG(wxString::Format(wxT("GetLatexFont - unsupported family %d, set to %s"), m_font.GetFamily(), res.c_str()));
		break;
	}

	if (m_font.GetStyle() == wxFONTSTYLE_ITALIC)
	{
		res << wxT("\\itshape"); // italic
		SP_LOGDEBUG(wxT("Font set to italic"));
	}
	else if (m_font.GetStyle() == wxFONTSTYLE_SLANT)
	{
		res << wxT("\\slshape"); // slanted
		SP_LOGDEBUG(wxT("Font set to slanted"));
	}

	if (m_font.GetWeight() == wxFONTWEIGHT_BOLD)
	{
		res << wxT("\\bfseries"); // bold
		SP_LOGDEBUG(wxT("Font set to bold"));
	}
	else if (m_font.GetWeight() == wxFONTWEIGHT_LIGHT)
	{
		res << wxT("\\mdseries"); // medium
		SP_LOGDEBUG(wxT("Font set to medium"));
	}

	switch (m_font.GetPointSize())
	{
		case 5:
		case 6:
			res << wxT("\\tiny");
			break;
		case 7:
			res << wxT("\\scriptsize");
			break;
		case 8:
			res << wxT("\\footnotesize");
			break;
		case 9:
			res << wxT("\\small");
			break;
		case 10:
		case 11:
			res << wxT("\\normalsize");
			break;
		case 12:
		case 13:
			res << wxT("\\large");
			break;
		case 14:
		case 15:
		case 16:
			res << wxT("\\Large");
			break;
		case 17:
		case 18:
		case 19:
			res << wxT("\\LARGE");
			break;
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
			res << wxT("\\huge");
			break;
		case 25:
			res << wxT("\\Huge");
			break;
		default:
			res << wxT("\\normalsize");
			break;
	}

	return res;
}

//-------------------------------------------------------------------------------

void SP_LatexDC::Init()
{
	m_pstream = (FILE*) NULL;

	m_underlinePosition = 0.0;
	m_underlineThickness = 0.0;

	m_signX = 1; // default x-axis left to right
	m_signY = 1; // default y-axis top up -> bottom down

	m_scale = 1;
	m_resolution = DEF_RESOLUTION;

	m_ColourDB.clear();
}

SP_LatexDC::SP_LatexDC()
{
	Init();
}

SP_LatexDC::SP_LatexDC(const wxPrintData& printData)
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

SP_LatexDC::~SP_LatexDC()
{
	if (m_pstream)
	{
		fclose(m_pstream);
		m_pstream = (FILE*) NULL;
	}
}

//------------------------------------------------------------------------------

bool SP_LatexDC::StartDoc(const wxString& message)
{
	//  SP_LOGDEBUG(wxString::Format(wxT("StartDoc %s"), message.c_str()));
	wxCHECK_MSG( m_ok, FALSE, wxT("invalid latex dc"));

	if (m_printData.GetFilename() == wxT(""))
	{
		wxString filename = wxGetTempFileName(wxT("tex"));
		SP_LOGDEBUG(wxString::Format(wxT("writing to temporary file %s"), filename.c_str()));
		m_printData.SetFilename(filename);
	}

	m_pstream = wxFopen(m_printData.GetFilename().c_str(), wxT("w+"));

	if (!m_pstream)
	{
		SP_LOGERROR(_("Cannot open file for Latex output!"));
		m_ok = FALSE;
		return FALSE;
	}

	m_ok = TRUE;

	wxFprintf(m_pstream, wxT("\\documentclass{article}\n"));
	wxFprintf(m_pstream, wxT("\n"));
	wxFprintf(m_pstream, wxT("\\usepackage{pgf}\n"));
	wxFprintf(m_pstream, wxT("\\usepackage{tikz}\n"));
	wxFprintf(m_pstream, wxT("\\usetikzlibrary{positioning,arrows,shapes,backgrounds,calc,patterns}\n"));
	wxFprintf(m_pstream, wxT("\n"));
	wxFprintf(m_pstream, wxT("\\usepackage[graphics,tightpage,active]{preview}\n"));
	wxFprintf(m_pstream, wxT("\\PreviewEnvironment{tikzpicture}\n"));
	wxFprintf(m_pstream, wxT("\\PreviewEnvironment{equation}\n"));
	wxFprintf(m_pstream, wxT("\\PreviewEnvironment{equation*}\n"));
	wxFprintf(m_pstream, wxT("\\newlength{\\imagewidth}\n"));
	wxFprintf(m_pstream, wxT("\\newlength{\\imagescale}\n"));
	wxFprintf(m_pstream, wxT("\\pagestyle{empty}\n"));
	wxFprintf(m_pstream, wxT("\\thispagestyle{empty}\n"));
	wxFprintf(m_pstream, wxT("\n"));
	wxFprintf(m_pstream, wxT("\\begin{document}\n"));
	wxFprintf(m_pstream, wxT("\n"));
	wxFprintf(m_pstream, wxT("\\begin{tikzpicture}[x=1pt,y=-1pt]\n"));
	wxFprintf(m_pstream, wxT("\n"));
	return TRUE;
}

void SP_LatexDC::EndDoc()
{
	//  SP_LOGDEBUG(wxT("EndDoc"));
	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

	wxFprintf(m_pstream, wxT("\\end{tikzpicture}\n\n"));
	wxFprintf(m_pstream, wxT("\\end{document}\n\n"));

	fclose(m_pstream);
	m_pstream = (FILE *) NULL;
}

//------------------------------------------------------------------------------

void SP_LatexDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
	//  SP_LOGDEBUG(wxString::Format(wxT("DoDrawEllipse %d %d %d %d"),
	//				 x, y,
	//			 width, height));
	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

	if (m_pen.GetStyle() != wxTRANSPARENT && m_pen.GetColour() != *wxWHITE)
	{
		double tX = (x * m_scale);
		double tY = (y * m_scale);
		double radiusX = ((width * m_scale)) / 2;
		double radiusY = ((height * m_scale)) / 2;

		wxString out = wxT("\\draw[");
		out << GetLatexPColor() << wxT(", ");
		out << GetLatexLStyle() << wxT(", ");
		out << GetLatexJoin() << wxT(", ");
		out << GetLatexLCap() << wxT(", ");
		out << GetLatexLineWidth() << wxT(", ");
		out << GetLatexFill();
		out << wxT("]\n\t");
		out << wxT("(") << tX+radiusX << wxT(",") << tY+radiusY << wxT(")");
		out << wxT(" ellipse[");
		out << wxT("x radius=") << radiusX << wxT(", y radius=") << radiusY << wxT("]");
		out << wxT(";\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}
}

void SP_LatexDC::DoDrawArc(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord xc, wxCoord yc)
{
//  SP_LOGDEBUG(wxString::Format(wxT("DoDrawArc %d %d %d %d %d %d"),
//				 x1, y1, x2,
//				 y2, xc,  yc));
	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

	if (m_pen.GetStyle() != wxTRANSPARENT && m_pen.GetColour() != *wxWHITE)
	{
		double dx = x1 - xc;
		double dy = y1 - yc;
		double radius = sqrt( dx*dx+dy*dy );
	    double alpha1, alpha2;

	    if (x1 == x2 && y1 == y2)
	    {
	        alpha1 = 0.0;
	        alpha2 = 360.0;
	    }
	    else if ( wxIsNullDouble(radius) )
	    {
	        alpha1 =
	        alpha2 = 0.0;
	    }
	    else
	    {
	        alpha1 = (x1 - xc == 0) ?
	            (y1 - yc < 0) ? 90.0 : -90.0 :
	                -atan2(double(y1-yc), double(x1-xc)) * RAD2DEG;
	        alpha2 = (x2 - xc == 0) ?
	            (y2 - yc < 0) ? 90.0 : -90.0 :
	                -atan2(double(y2-yc), double(x2-xc)) * RAD2DEG;
	    }
	    while (alpha1 <= 0)   alpha1 += 360;
	    while (alpha2 <= 0)   alpha2 += 360; // adjust angles to be between
	    while (alpha1 > 360)  alpha1 -= 360; // 0 and 360 degree
	    while (alpha2 > 360)  alpha2 -= 360;

		double tX = (xc * m_scale);
		double tY = (yc * m_scale);

		wxString out = wxT("\\draw[");
		out << GetLatexPColor() << wxT(", ");
		out << GetLatexLStyle() << wxT(", ");
		out << GetLatexJoin() << wxT(", ");
		out << GetLatexLCap() << wxT(", ");
		out << GetLatexLineWidth() << wxT(", ");
		out << GetLatexFill();
		out << wxT("]\n\t");
		out << wxT("(") << tX << wxT(",") << tY << wxT(")");
		out << wxT(" arc[");
		out << wxT("start angle=") << alpha1 << wxT(", end angle=") << alpha2 << wxT("]");
		out << wxT(" -- cycle;\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}
}

void SP_LatexDC::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{
//  SP_LOGDEBUG(wxString::Format(wxT("DoDrawText %s %d %d"), text.c_str(),
//				 x, y));

	DoDrawRotatedText(text, x, y, 0);

}

void SP_LatexDC::DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle)
{
//  SP_LOGDEBUG(wxString::Format(wxT("DoDrawRotatedText %s %d %d %g"),
//				 text.c_str(), x, y, angle));
	if (m_pen.GetStyle() != wxTRANSPARENT && m_pen.GetColour() != *wxWHITE)
	{

		wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

		double tX = (x * m_scale);//((x + GetCharWidth() / 2.0) * m_scale);
		double tY = ((y + GetCharHeight() / 2.0) * m_scale);

		wxString escaped_text = text;
		escaped_text.Replace(wxT("\\"), wxT("\\textbackslash "));
		escaped_text.Replace(wxT("{"), wxT("\\{"));
		escaped_text.Replace(wxT("}"), wxT("\\}"));
		escaped_text.Replace(wxT("#"), wxT("\\#"));
		escaped_text.Replace(wxT("$"), wxT("\\$"));
		escaped_text.Replace(wxT("%"), wxT("\\%"));
		escaped_text.Replace(wxT("&"), wxT("\\&"));
		escaped_text.Replace(wxT("_"), wxT("\\_"));
		escaped_text.Replace(wxT("^"), wxT("\\^{}"));
		escaped_text.Replace(wxT("~"), wxT("\\~{}"));
		escaped_text.Replace(wxT("|"), wxT("\\textbar "));
		escaped_text.Replace(wxT("<"), wxT("\\textless "));
		escaped_text.Replace(wxT(">"), wxT("\\textgreater "));

		wxString out = wxT("\\draw[");
		out << GetLatexPColor();
//		out << GetLatexLStyle() << wxT(", ");
//		out << GetLatexJoin() << wxT(", ");
//		out << GetLatexLCap() << wxT(", ");
//		out << GetLatexLineWidth() << wxT(", ");
//		out << GetLatexFill();
		out << wxT("]\n\t");
		out << wxT("(") << tX << wxT(",") << tY << wxT(")");
		out << wxT(" node[");
		out << wxT("rotate=") << angle;
		out << wxT(", ") << GetLatexFont();
		out << wxT(", ") << GetLatexColor(m_textForegroundColour);
		out << wxT(", right=-.25]\n");
		out << wxT("\t{") << escaped_text << wxT("}");
		out << wxT(";\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}
}

void SP_LatexDC::DoDrawLines(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset)
{
//     SP_LOGDEBUG(wxString::Format(wxT("DoDrawLines %d %d %d"),
// 		n, xoffset, yoffset));
	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

	if (m_pen.GetStyle() != wxTRANSPARENT && m_pen.GetColour() != *wxWHITE)
	{
		wxString out = wxT("\\draw[");
		out << GetLatexPColor() << wxT(", ");
		out << GetLatexLStyle() << wxT(", ");
		out << GetLatexJoin() << wxT(", ");
		out << GetLatexLCap() << wxT(", ");
		out << GetLatexLineWidth();
		out << wxT("]\n\t");
		double tX, tY;
		for (int i = 0; i < n; i++)
		{
			tX = (points[i].x * m_scale + xoffset * m_scale);
			tY = (points[i].y * m_scale + yoffset * m_scale);
			if(i > 0)
			{
				out << wxT(" -- ");
			}
			out << wxT("(") << tX << wxT(",") << tY << wxT(")");
		}
		out << wxT(";\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}
}

void SP_LatexDC::DoDrawPoint(wxCoord x, wxCoord y)
{
//	SP_LOGDEBUG(wxString::Format(wxT("DoDrawPoint %d %d"), x, y));

	wxPoint tmp[] =
	{ wxPoint(x, y), wxPoint(x, y) };
	DoDrawLines(2, tmp);
}

void SP_LatexDC::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
//	SP_LOGDEBUG(wxString::Format(wxT("DoDrawLine %d %d %d %d"), x1, y1, x2, y2));

	wxPoint tmp[] =
	{ wxPoint(x1, y1), wxPoint(x2, y2) };
	DoDrawLines(2, tmp);
}

void SP_LatexDC::DoDrawPolygon(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, int fillStyle)
{
//    SP_LOGDEBUG(wxString::Format(wxT("DoDrawPolygon %d, style %d tr:%d"),
//  		n, m_pen.GetStyle(), m_pen.GetStyle() == wxTRANSPARENT ));
	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

	if (m_pen.GetStyle() != wxTRANSPARENT && m_pen.GetColour() != *wxWHITE)
	{
		wxString out = wxT("\\draw[");
		out << GetLatexPColor() << wxT(", ");
		out << GetLatexLStyle() << wxT(", ");
		out << GetLatexJoin() << wxT(", ");
		out << GetLatexLCap() << wxT(", ");
		out << GetLatexLineWidth() << wxT(", ");
		out << GetLatexFill();
		out << wxT("]\n\t");
		double tX, tY;
		for (int i = 0; i < n; i++)
		{
			tX = (points[i].x * m_scale + xoffset * m_scale);
			tY = (points[i].y * m_scale + yoffset * m_scale);
			if(i > 0)
			{
				out << wxT(" -- ");
			}
			out << wxT("(") << tX << wxT(",") << tY << wxT(")");
		}
		out << wxT(" -- cycle;\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}
}

void SP_LatexDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
//    SP_LOGDEBUG(wxString::Format(wxT("DoDrawRectangle %d %d %d %d"),
//  				 x, y,
//  				 width, height));

	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc") );
	if (m_pen.GetStyle() != wxTRANSPARENT && m_pen.GetColour() != *wxWHITE)
	{
		double tX = (x * m_scale);
		double tY = (y * m_scale);
		double tW = (width * m_scale);
		double tH = (height * m_scale);
		wxString out = wxT("\\draw[");
		out << GetLatexPColor() << wxT(", ");
		out << GetLatexLStyle() << wxT(", ");
		out << GetLatexJoin() << wxT(", ");
		out << GetLatexLCap() << wxT(", ");
		out << GetLatexLineWidth() << wxT(", ");
		out << GetLatexFill();
		out << wxT("]\n\t");
		out << wxT("(") << tX << wxT(",") << tY << wxT(")");
		out << wxT(" rectangle ");
		out << wxT("+(") << tW << wxT(",") << tH << wxT(")");
		out << wxT(";\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}
}

void SP_LatexDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius)
{
//   SP_LOGDEBUG(wxString::Format(wxT("DoDrawRoundedRectangle %d %d %d %d %g"),
// 				 x, y,
// 				 width, height,
// 				 radius));

	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

	if (m_pen.GetStyle() != wxTRANSPARENT && m_pen.GetColour() != *wxWHITE)
	{
		double tX = (x * m_scale);
		double tY = (y * m_scale);
		double tW = (width * m_scale);
		double tH = (height * m_scale);
		wxString out = wxT("\\draw[rounded corners, ");
		out << GetLatexPColor() << wxT(", ");
		out << GetLatexLStyle() << wxT(", ");
		out << GetLatexJoin() << wxT(", ");
		out << GetLatexLCap() << wxT(", ");
		out << GetLatexLineWidth() << wxT(", ");
		out << GetLatexFill();
		out << wxT("]\n\t");
		out << wxT("(") << tX << wxT(",") << tY << wxT(")");
		out << wxT(" rectangle ");
		out << wxT("+(") << tW << wxT(",") << tH << wxT(")");
		out << wxT(";\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}
}

void SP_LatexDC::DoDrawSpline(wxList *points)
{
//	SP_LOGDEBUG(wxT("DoDrawSpline"));
	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

	if (m_pen.GetStyle() != wxTRANSPARENT && m_pen.GetColour() != *wxWHITE)
	{
	    double c, d, x1, y1, x2, y2, x3, y3;
	    wxPoint *p, *q;

	    wxList::compatibility_iterator node = points->GetFirst();
	    p = (wxPoint *)node->GetData();
	    x1 = p->x;
	    y1 = p->y;

	    node = node->GetNext();
	    p = (wxPoint *)node->GetData();
	    c = p->x;
	    d = p->y;
	    x3 = (double)(x1 + c) / 2;
	    y3 = (double)(y1 + d) / 2;

		wxString out = wxT("\\draw[");
		out << GetLatexPColor() << wxT(", ");
		out << GetLatexLStyle() << wxT(", ");
		out << GetLatexJoin() << wxT(", ");
		out << GetLatexLCap() << wxT(", ");
		out << GetLatexLineWidth();
		out << wxT("]\n\t");
		out << wxT("(") << x1 << wxT(",") << y1 << wxT(")");
		out << wxT(" -- ");
		out << wxT("(") << x3 << wxT(",") << y3 << wxT(")");

		node = node->GetNext();
	    while (node)
	    {
	        q = (wxPoint *)node->GetData();

	        x1 = x3;
	        y1 = y3;
	        x2 = c;
	        y2 = d;
	        c = q->x;
	        d = q->y;
	        x3 = (double)(x2 + c) / 2;
	        y3 = (double)(y2 + d) / 2;

			out << wxT(" .. controls (") << x2 << wxT(",") << y2 << wxT(")");
			out << wxT(" .. ");
			out << wxT("(") << x3 << wxT(",") << y3 << wxT(")");

	        node = node->GetNext();
	    }
		out << wxT(" -- (") << c << wxT(",") << d << wxT(")");
		out << wxT(";\n");
		wxFprintf(m_pstream, wxT("%s"), out.c_str());
	}
}


//------------------------------------------------------------------------------
void SP_LatexDC::SetBackground(const wxBrush& brush)
{
	SP_LOGDEBUG(wxT("SetBackground"));
	m_backgroundBrush = brush;
}

void SP_LatexDC::SetResolution(int ppi)
{
	m_resolution = ppi;
	m_scale = (float) ppi / DEF_RESOLUTION;
}

int SP_LatexDC::GetResolution()
{
	return m_resolution;
}

void SP_LatexDC::SetFont(const wxFont& font)
{
//   SP_LOGDEBUG(wxT("SetFont"));
	m_font = font;
}

void SP_LatexDC::SetPen(const wxPen& pen)
{
//   SP_LOGDEBUG(wxT("SetPen"));
	m_pen = pen;
}

void SP_LatexDC::SetBrush(const wxBrush& brush)
{
//   SP_LOGDEBUG(wxT("SetBrush"));
	m_brush = brush;
}

bool SP_LatexDC::Ok() const
{
	return m_ok;
}

// from wxPostscriptDC, not tested
//------------------------------------------------------------------------------

void SP_LatexDC::SetDeviceOrigin(wxCoord x, wxCoord y)
{
	SP_LOGDEBUG(wxString::Format(wxT("SetDeviceOrigin %d %d"), x, y));
	wxCHECK_RET( m_ok && m_pstream, wxT("invalid latex dc"));

	int h = 0;
	int w = 0;
	GetSize(&w, &h);

	wxDC::SetDeviceOrigin(x, h - y);
}

void SP_LatexDC::DoGetSize(int* width, int* height) const
{
	//  SP_LOGDEBUG(wxT("DoGetSize"));
	wxPaperSize id = m_printData.GetPaperId();

	wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(id);

	if (!paper)
		paper = wxThePrintPaperDatabase->FindPaperType(wxPAPER_A4);

	int w = 595;
	int h = 842;
	if (paper)
	{
		w = paper->GetSizeDeviceUnits().x;
		h = paper->GetSizeDeviceUnits().y;
	}

	if (m_printData.GetOrientation() == wxLANDSCAPE)
	{
		int tmp = w;
		w = h;
		h = tmp;
	}

	if (width)
		*width = (int) (w * m_scale);
	if (height)
		*height = (int) (h * m_scale);
}

void SP_LatexDC::DoGetSizeMM(int *width, int *height) const
{
	//  SP_LOGDEBUG(wxT("DoGetSizeMM"));
	wxPaperSize id = m_printData.GetPaperId();

	wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(id);

	if (!paper)
		paper = wxThePrintPaperDatabase->FindPaperType(wxPAPER_A4);

	int w = 210;
	int h = 297;

	if (paper)
	{
		w = paper->GetWidth() / 10;
		h = paper->GetHeight() / 10;
	}

	if (m_printData.GetOrientation() == wxLANDSCAPE)
	{
		int tmp = w;
		w = h;
		h = tmp;
	}

	if (width)
		*width = w;
	if (height)
		*height = h;
}

wxCoord SP_LatexDC::GetCharHeight() const
{
	//  SP_LOGDEBUG(wxT("GetCharHeight"));
	if (m_font.Ok())
		return m_font.GetPointSize() + 1;
	else
		return 12;
}

wxCoord SP_LatexDC::GetCharWidth() const
{
	//  SP_LOGDEBUG(wxT("GetCharWidth"));
	// Chris Breeze: reasonable approximation using wxMODERN/Courier
	return (wxCoord) (GetCharHeight() * DEF_RESOLUTION / 120.0);
}

// Resolution in pixels per logical inch
wxSize SP_LatexDC::GetPPI(void) const
{
	//  SP_LOGDEBUG(wxT("GetPPI"));
	return wxSize(m_resolution, m_resolution);
}

// do nothing
//------------------------------------------------------------------------------

void SP_LatexDC::StartPage()
{
	SP_LOGDEBUG(wxT("StartPage"));
}

void SP_LatexDC::EndPage()
{
	SP_LOGDEBUG(wxT("EndPage"));
}

void SP_LatexDC::DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
//   SP_LOGDEBUG(wxString::Format(wxT("DoSetClippingRegion %d %d %d %d"),
// 				 x, y, w, h));
}

void SP_LatexDC::DestroyClippingRegion()
{
	//  SP_LOGDEBUG(wxT("DestroyClippingRegion"));
}

// not implemented
//------------------------------------------------------------------------------

bool SP_LatexDC::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord fwidth, wxCoord fheight, wxDC *source, wxCoord xsrc, wxCoord ysrc, int rop, bool useMask, wxCoord xsrcMask, wxCoord ysrcMask)
{
	SP_LOGDEBUG(wxT("DoBlit"));
	wxFAIL_MSG( wxT("SP_LatexDC::DoBlit not implemented.") );
	return FALSE;
}

void SP_LatexDC::DoGetTextExtent(const wxString& string, wxCoord *x, wxCoord *y, wxCoord *descent, wxCoord *externalLeading, wxFont *theFont) const
{
	SP_LOGDEBUG(wxT("DoGetTextExtent"));
	wxFAIL_MSG( wxT("SP_LatexDC::DoGetTextExtent not implemented.") );
}

void SP_LatexDC::Clear()
{
	SP_LOGDEBUG(wxT("Clear"));
	wxFAIL_MSG( wxT("SP_LatexDC::Clear not implemented.") );
}

void SP_LatexDC::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h, double sa, double ea)
{
	SP_LOGDEBUG(wxString::Format(wxT("DoDrawEllipticArc %d %d %d %d %g %g"), x, y, w, h, sa, ea));
	wxFAIL_MSG( wxT("SP_LatexDC::DoDrawEllipticArc not implemented.") );
}

bool SP_LatexDC::DoFloodFill(wxCoord x, wxCoord y, const wxColour &col, int style)
{
	SP_LOGDEBUG(wxString::Format(wxT("DoFloodFill %d %d, color: %d %d %d, style %d"), x, y, col.Red(), col.Green(), col.Blue(), style));
	wxFAIL_MSG( wxT("SP_LatexDC::FloodFill not implemented.") );
	return FALSE;
}

bool SP_LatexDC::DoGetPixel(wxCoord x, wxCoord y, wxColour * col) const
{
	SP_LOGDEBUG(wxString::Format(wxT("DoGetPixel %d %d, color: %d %d %d"), x, y, col->Red(), col->Green(), col->Blue()));
	wxFAIL_MSG( wxT("SP_LatexDC::GetPixel not implemented.") );
	return FALSE;
}

void SP_LatexDC::DoCrossHair(wxCoord x, wxCoord y)
{
	SP_LOGDEBUG(wxString::Format(wxT("DoCrossHair %d %d"), x, y));
	wxFAIL_MSG( wxT("SP_LatexDC::CrossHair not implemented.") );
}

void SP_LatexDC::SetLogicalFunction(int function)
{
	SP_LOGDEBUG(wxT("SetLogicalFunction"));
	wxFAIL_MSG( wxT("SP_LatexDC::SetLogicalFunction not implemented.") );
}

void SP_LatexDC::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{
	SP_LOGDEBUG(wxString::Format(wxT("DoDrawIcon %d %d"), x, y));
	wxFAIL_MSG( wxT("SP_LatexDC::DoDrawIcon not implemented.") );
}

void SP_LatexDC::DoDrawBitmap(const wxBitmap& bitmap, wxCoord x, wxCoord y, bool useMask)
{
	SP_LOGDEBUG(wxString::Format(wxT("DoDrawBitmap %d %d"), x, y));
	wxFAIL_MSG( wxT("SP_LatexDC::DoDrawBitmap not implemented.") );
}

void SP_LatexDC::SetAxisOrientation(bool xLeftRight, bool yBottomUp)
{
	SP_LOGDEBUG(wxString::Format(wxT("SetAxisOrientation %d %d"), xLeftRight, yBottomUp));
	wxFAIL_MSG( wxT("SP_LatexDC::SetAxisOrientation not implemented.") );

}
#endif
