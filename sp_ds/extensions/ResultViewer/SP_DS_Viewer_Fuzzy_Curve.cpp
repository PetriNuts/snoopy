/*
* SP_DS_Viewer_Fuzzy_Curve.cpp
* $Author: G.Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 
* Short Description:
*/
//=================================================


#include "sp_ds/extensions/ResultViewer/SP_DS_Viewer_Fuzzy_Curve.h"

SP_DS_Viewer_Fuzzy_Curve::SP_DS_Viewer_Fuzzy_Curve(const wxString& p_sName, const unsigned long& p_nDataColumn,
	const wxString& p_sColor, const int& p_nLineWidth, const int& p_nLineStyle,
	  SP_Compressed_Fuzzy_Band*  p_pcResultMatrix)
	:m_sName(p_sName),
	m_nDataColumn(p_nDataColumn),
	m_nColor(p_sColor),
	m_nLineWidth(p_nLineWidth),
	m_nLineStyle(p_nLineStyle),
	m_pcResultMatrix(p_pcResultMatrix)
{

}

SP_DS_Viewer_Fuzzy_Curve::~SP_DS_Viewer_Fuzzy_Curve()
{

}
