/*
 * SP_DS_ViewerCurve.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 21.06.2011
 * Short Description:
 */
//=================================================


#include "sp_ds/extensions/ResultViewer/SP_DS_ViewerCurve.h"

SP_DS_ViewerCurve:: SP_DS_ViewerCurve(const wxString& p_sName,const unsigned long& p_nDataColumn,
                     const wxString& p_sColor,const int& p_nLineWidth,const int& p_nLineStyle,
                     const SP_Vector2DDouble*  p_pcResultMatrix)
:m_sName(p_sName),
 m_nDataColumn(p_nDataColumn),
 m_nColor(p_sColor),
 m_nLineWidth(p_nLineWidth),
 m_nLineStyle(p_nLineStyle),
 m_pcResultMatrix(p_pcResultMatrix)
{

}

SP_DS_ViewerCurve::~SP_DS_ViewerCurve()
{

}
