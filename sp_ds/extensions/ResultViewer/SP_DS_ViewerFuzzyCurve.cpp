/*
* SP_DS_ViewerCurve.cpp
* $Author: G.Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 7.02.2019
* Short Description:
*/
//=================================================


#include "sp_ds/extensions/ResultViewer/SP_DS_ViewerFuzzyCurve.h"

SP_DS_ViewerFuzzyCurve::SP_DS_ViewerFuzzyCurve(const wxString& p_sName, const unsigned long& p_nDataColumn, const vector<SP_Vector2DDouble>  p_pcResultMatrix,const  ResultFuzzyBand fBand, SP_VectorDouble alphaLevels,
	const wxString& p_sColor, const int& p_nLineWidth, const int& p_nLineStyle,unsigned long levels,unsigned long points,unsigned int fn)
	:m_sName(p_sName),
	m_nDataColumn(p_nDataColumn),
	m_pcResultMatrix(p_pcResultMatrix),
	m_FuzzyBandResult(fBand),
	m_nAlphalevels(alphaLevels),
	m_nColor(p_sColor),
	m_nLineWidth(p_nLineWidth),
	m_nLineStyle(p_nLineStyle),
	m_levels(levels),
	m_points(points),
	m_fuzzyNum(fn)
	
{

}

SP_DS_ViewerFuzzyCurve::~SP_DS_ViewerFuzzyCurve() {};