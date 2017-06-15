//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_UTILITIES_H__
#define __SP_UTILITIES_H__

#include "sp_defines.h"

#include <dssd/auxi/auxi.h>
#include <dssd/auxi/exceptions.h>

#include <wx/gdicmn.h>
#include <wx/dialog.h>
#include <wx/grid.h>

#define SP_LOGDEBUG(A) \
        SP_LOGDEBUG_(A)
#define SP_LOGMESSAGE(A) \
        SP_LOGMESSAGE_(A)
#define SP_LOGWARNING(A) \
        SP_LOGWARNING_(wxString::Format(wxT("%s(%d):  %s"), file_name(__FILE__), __LINE__, A))
#define SP_LOGERROR(A) \
        SP_LOGERROR_(wxString::Format(wxT("%s(%d):  %s"), file_name(__FILE__), __LINE__, A))

void SP_LOGDEBUG_(const wxString& s);
void SP_LOGMESSAGE_(const wxString& s);
void SP_LOGWARNING_(const wxString& s);
void SP_LOGERROR_(const wxString& s);


/**	\brief	The SP_RectAroundOrigin function

	\param	p_nX1	X coord of the first point
	\param	p_nY1	Y coord of the first point
	\param	p_nX2	X coord of the second point
	\param	p_nY2	Y coord of the second point
	\param	p_nMarg margin to be used as tolerance

	\return	bool    TRUE if the points build up a rect around (,0) FALSE otherwise

	Helperfunction to test, whether two points span a rectangle, the origin (0,0)
    lies in completely or on one of the side lines.
    This function is mainly used to add control points to edges by left clicking
    on an edge with CTRL key hold down.
*/
bool SP_RectAroundOrigin(double p_nX1, double p_nY1,
                         double p_nX2, double p_nY2,
                         int p_nMarg = 2);

double SP_CalculateDistance(wxRealPoint*, wxRealPoint*);

// wrapperfunction for wxMessageBox()
// creates custom MessageBox on MacOSX
// calls wxMessageBox on other Systems (GTK, MSW, ...)
int SP_MESSAGEBOX(const wxString& message,
                        const wxString& caption = wxT("Default caption"),
                        long style = wxOK | wxCENTRE,
                        wxWindow *parent = NULL,
                        int x = wxDefaultCoord, int y = wxDefaultCoord);

// global function to auto size the rowlabels of a wxgrid
void SP_AutoSizeRowLabelSize(wxGrid* p_Grid);

/**
 * @return: random double value from [0,1]
 */
double SP_RandomDouble();

/**
 * @return: random double value from [0,1)
 */
double SP_RandomDoubleExc();

/**
 * @return: random double value from (0,1)
 */
double SP_RandomDoubleDblExc();

/**
 * @return: random unsigned long value from [0,p_nUpperbound)
 */
unsigned long SP_RandomLong(unsigned long p_nUpperbound);

/**
 * convert Num to std::string
 * @return: std::string
 */
template<typename T>std::string SP_ToString(T p_Param)
{
	std::stringstream l_StringStream;
	std::string l_String;
	try
	{
		l_StringStream << p_Param;
	}catch(...)
	{
		assert("Error in Function SP_ToString!");
	}
	l_String = l_StringStream.str();
	return l_String;
}

template<typename T>
wxString SP_ToWxString(T p_Param)
{
	wxString r;
	r << p_Param;
	return r;
}

/**
 * Add Thousands Separators
 */
wxString SP_FormatWithComma(const wxString& s);

/**	\brief	replaces all non-alphanum chars with "_"
	\param	string to normalize
	\return	wxString
*/
wxString SP_NormalizeString (const wxString& p_sS, bool p_bCheckFirst = true);

/**
 * extract xml attribute from text
 */
wxString SP_ExtractAttribute(const wxString& p_sName, const wxString& p_sSource);
/**
 * extract xml node from text
 */
wxString SP_ExtractNode(const wxString& p_sName, const wxString& p_sSource);

#endif //__SP_UTILITIES_H__
