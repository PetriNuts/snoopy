/*
* SP_DS_ViewerCurve.h
* $Author:G.Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date:  
* Short Description:
*/
//=================================================


#ifndef _SP_DS_VIEWER_FUZZY_BAND_CURVE_H_
#define _SP_DS_VIEWER_FUZZY_BAND_CURVE_H_

#include<wx/wx.h>
#include <wx/string.h>
#include "sp_core/SP_Vector.h"
#include "sp_gui/dialogs/dia_FPN/SP_Compressed_Fuzzy_Band.h"
#include <list>

class SP_DS_Viewer_Fuzzy_Curve
{
private:
	//Curve name
	wxString m_sName;

	//the corresponding column in the result matrix
	unsigned long m_nDataColumn;

	//curve color in case of xy plot
	wxString m_nColor;

	int m_nLineWidth;
	int m_nLineStyle;

	//pointer to the result matrix
	  SP_Compressed_Fuzzy_Band*  m_pcResultMatrix;
public:
	SP_DS_Viewer_Fuzzy_Curve(const wxString& p_sName, const unsigned long& p_nDataColumn,
		const wxString& p_nColor = wxT("0"), const int& p_nLineWidth = -1,
		const int& p_nLineStyle = -1,  SP_Compressed_Fuzzy_Band*  p_pcResultMatrix = NULL);
	virtual ~SP_DS_Viewer_Fuzzy_Curve();

	//curve name
	void SetName(const wxString& p_sName) { m_sName = p_sName; }
	wxString GetName() { return m_sName; }

	//Column index
	void SetDataColumn(const unsigned long& p_nColumn) { m_nDataColumn = p_nColumn; }
	unsigned long GetDataColumn() { return m_nDataColumn; }

	//color
	void SetColor(const wxString& p_nColor) { m_nColor = p_nColor; }
	wxString& GetColor() { return m_nColor; }

	//line width
	void SetLineWidth(const int& p_nLineWidth) { m_nLineWidth = p_nLineWidth; }
	int GetLineWidth() { return m_nLineWidth; }

	//line style
	void SetLineStyle(const int& p_nLineStyle) { m_nLineStyle = p_nLineStyle; }
	int GetLineStyle() { return m_nLineStyle; }

	//set and get result matrix
///	void SetResultMatrix( SP_Compressed_Fuzzy_Band*  p_pcResultMatrix) { m_pcResultMatrix = p_pcResultMatrix; }
	const SP_Compressed_Fuzzy_Band GetResultMatrix() { return (*m_pcResultMatrix); }

	//get the curve value at a certain point
	inline double GetValue(const unsigned long& p_nPoint, const SP_Vector2DDouble& resMatrix)
	{
		double l_nValue = 0.0;


		if (resMatrix.data() != NULL && p_nPoint< resMatrix.size())
		{
			l_nValue = resMatrix[p_nPoint][m_nDataColumn];
		}



		return l_nValue;
	}
};

typedef std::list<SP_DS_Viewer_Fuzzy_Curve*> SP_ListFuzzyCurves;
//typedef std::vector<SP_DS_ViewerCurve*> SP_VectorCurve;
typedef std::vector<SP_DS_Viewer_Fuzzy_Curve*> SP_VectorFuzzyCurve;
#endif /* _SP_DS_VIEWER_FUZZY_BAND_CURVE_H_ */
