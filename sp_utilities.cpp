//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: utilitie functions
//////////////////////////////////////////////////////////////////////
#include <limits>
#include <wx/wx.h>
#include "sp_utilities.h"
#include <wx/utils.h>
#include <wx/regex.h>
#include "sp_core/tools/MersenneTwister.h"

bool
SP_RectAroundOrigin(double p_nX1, double p_nY1,
                      double p_nX2, double p_nY2, int p_nMarg)
{
    double l_nX1 = wxMin(p_nX1, p_nX2);
    double l_nX2 = wxMax(p_nX1, p_nX2);
    double l_nY1 = wxMin(p_nY1, p_nY2);
    double l_nY2 = wxMax(p_nY1, p_nY2);

    l_nX1 -= p_nMarg;
    l_nY1 -= p_nMarg;
    l_nX2 += p_nMarg;
    l_nY2 += p_nMarg;

    return (l_nX1 < 0 && l_nX2 > 0 && l_nY1 < 0 && l_nY2 > 0);
}

double
SP_CalculateDistance(wxRealPoint* p_pcPoint1, wxRealPoint* p_pcPoint2)
{
    if (!p_pcPoint1 || !p_pcPoint2)
        return 0.0;

    double l_nDistX = p_pcPoint1->x - p_pcPoint2->x;
    double l_nDistY = p_pcPoint1->y - p_pcPoint2->y;
    l_nDistX *= l_nDistX;
    l_nDistY *= l_nDistY;
    return (sqrt(l_nDistX + l_nDistY));
}

int SP_MESSAGEBOX(const wxString& message,
                             const wxString& caption,
                             long style,
                             wxWindow *parent,
                             int x, int y)
{
#if !defined(__WXMAC__) || wxABI_VERSION > 30000
	return wxMessageBox(message, caption, style, parent, x, y);
#else
	wxDialog* l_pcDlg = new wxDialog(parent, -1, caption, wxDefaultPosition, wxDefaultSize, style | wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP);

	wxBoxSizer* l_cTextpane = new wxBoxSizer(wxVERTICAL);
	wxStaticText* l_cMsg = new wxStaticText(l_pcDlg, -1, message);
	l_cTextpane->Add (l_cMsg, 0,  wxALL, 10);
	l_cTextpane->Add(l_pcDlg->CreateButtonSizer(style), 0, wxEXPAND | wxALL, 10);

	if( style & wxNO )
		l_pcDlg->SetEscapeId(wxID_NO);

	l_pcDlg->SetSizerAndFit(l_cTextpane);
	l_pcDlg->Centre();
	int l_nReturn = l_pcDlg->ShowModal();
	switch (l_nReturn) {
		case wxID_OK:
			l_nReturn = wxOK;
			break;
		case wxID_YES:
			l_nReturn = wxYES;
			break;
		case wxID_NO:
			l_nReturn = wxNO;
			break;
		default:
			l_nReturn = wxCANCEL;
			break;
	}
	l_pcDlg->Destroy();
	return l_nReturn;
#endif
}

void SP_AutoSizeRowLabelSize(wxGrid* p_Grid)
{
	wxScreenDC dc;
	dc.SetFont(p_Grid->GetLabelFont());

	int maxWidth = 0;
	int curRow = p_Grid->GetNumberRows() - 1;
	while(curRow >= 0)
	{
		int curWidth = dc.GetTextExtent(wxT("M")+p_Grid->GetRowLabelValue(curRow)).GetWidth();
		if(curWidth > maxWidth)
			maxWidth = curWidth;
		curRow--;
	}
	p_Grid->SetRowLabelSize(maxWidth);
}

//Mersenne Twister Pseudo Random Number Generator;
MTRand& SP_MTRand()
{
	static MTRand l_cMTRand;

	return l_cMTRand;
}

double SP_RandomDouble()
{
	return SP_MTRand().rand();
}

double SP_RandomDoubleExc()
{
	return SP_MTRand().randExc();
}

double SP_RandomDoubleDblExc()
{
	return SP_MTRand().randDblExc();
}

unsigned long SP_RandomLong(unsigned long p_nUpperbound)
{
	if(p_nUpperbound > 0 && p_nUpperbound <= std::numeric_limits<unsigned long>::max())
	{
		return SP_MTRand().randInt(--p_nUpperbound);
	}
	return std::numeric_limits<unsigned long>::max();
}

/**
 * Add Thousands Separators
 */
wxString
SP_FormatWithComma(const wxString& s)
{
	if(s.length() < 4)
		return s;

	double v;
	if(!s.ToDouble(&v))
		return s;

    wxChar thousandsSep = wxT(',');
    wxChar decimalSep = wxT('.');

	size_t pos = s.find(decimalSep);
	if ( pos == wxString::npos )
	{
		// Start grouping at the end of an integer number.
		pos = s.length();
	}

	wxString res{s};
	res.resize(s.length() + pos/3);

	// We currently group digits by 3 independently of the locale.
	const size_t GROUP_LEN = 3;

	while ( pos > GROUP_LEN )
	{
		pos -= GROUP_LEN;
		res.insert(pos, thousandsSep);
	}

	return res;
}

void SP_LOGDEBUG(const wxString& s)
{
	//use of ("%s",s) is important, otherwise '%' is not printed
	wxLogVerbose("%s",s);
}

void SP_LOGMESSAGE(const wxString& s)
{
	//use of ("%s",s) is important, otherwise '%' is not printed
	wxLogMessage("%s",s);
}

void SP_LOGWARNING(const wxString& s)
{
	//use of ("%s",s) is important, otherwise '%' is not printed
	wxLogWarning("%s",s);
}

void SP_LOGERROR(const wxString& s)
{
	//use of ("%s",s) is important, otherwise '%' is not printed
	wxLogError("%s",s);
}

wxString SP_NormalizeString(const wxString& p_S, bool p_bCheckFirst)
{
	if (p_S.IsEmpty())
		return p_S;

	wxString tmp = p_S;
	for (size_t i = 0; i < p_S.length(); i++)
	{
		wxChar c = tmp.GetChar(i);
		if (!wxIsalnum(c) && c != '_')
		{
			tmp.SetChar(i, '_');
		}
	}
	if(p_bCheckFirst && wxIsdigit(tmp.GetChar(0)))
	{
		tmp.Prepend("_");
	}
	return tmp;
}

wxString SP_ExtractAttribute(const wxString& p_sName, const wxString& p_sSource)
{
	wxString l_sPattern = p_sName+wxT("\\s*=\\s*\"([^\"]+)\"");
	wxRegEx l_cRegEx(l_sPattern, wxRE_ADVANCED);
	if(l_cRegEx.Matches(p_sSource))
	{
		return l_cRegEx.GetMatch(p_sSource, 1);
	}
	return {};
}

wxString SP_ExtractNode(const wxString& p_sName, const wxString& p_sSource)
{
	int l_Start = p_sSource.Find(wxT("<")+p_sName+wxT(">"));
	int l_End = p_sSource.Find(wxT("</")+p_sName+wxT(">"));
	//SP_LOGMESSAGE(wxString::Format(wxT("start: %i end: %i"), l_Start, l_End));
	if(l_Start != wxNOT_FOUND && l_Start < l_End)
	{
		return p_sSource.Mid(l_Start, l_End - l_Start + p_sName.length() + 3);
	}
	return {};
}


