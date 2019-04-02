/*
* SP_DS_ViewerCurve.h
* $Author: G.Assaf
* $Version: 0.0 $
* $Revision: 0.0 $
* $Date: 7.02.2019
* Short Description:
*/
//=================================================


#ifndef SP_DS_VIEWERFUZZYCURVE_H_
#define SP_DS_VIEWERFUZZYCURVE_H_

#include<wx/wx.h>
#include <wx/string.h>
#include "sp_core/SP_Vector.h"

#include <list>
/*
struct TraceElement {
	std::vector<double> sample;
	double currentLevel;
	int levelNum;
	SP_Vector2DDouble fuzzyTrace;

};
typedef std::vector<TraceElement>  ResultFuzzyBand;
*/

class SP_DS_ViewerFuzzyCurve
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
	unsigned long m_levels;
	unsigned long m_points;
	unsigned int m_fuzzyNum;
	SP_VectorDouble m_nAlphalevels;

	//pointer to the result matrix
	const vector<SP_Vector2DDouble>  m_pcResultMatrix;
	const ResultFuzzyBand m_FuzzyBandResult;
public:
	SP_DS_ViewerFuzzyCurve(const wxString& p_sName, const unsigned long& p_nDataColumn, const vector<SP_Vector2DDouble>  p_pcResultMatrix,const ResultFuzzyBand fBand, SP_VectorDouble m_AlphaLevels,
		const wxString& p_nColor = wxT("0"), const int& p_nLineWidth = -1,
		const int& p_nLineStyle = -1,unsigned long levels=11,unsigned long points=10 ,unsigned int fn=0);
	virtual ~SP_DS_ViewerFuzzyCurve();

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
	//number of alpha levels
	void SetLevels(const int levels) { m_levels = levels; }
	unsigned long GetLevels() { return m_levels; }
	//number of Points
	void SetPoints(const int points) { m_points = points; }
	unsigned long GetPoints() { return m_points; }
	//number of fuzzy numbers
	unsigned int GetFnCount() { return m_fuzzyNum; }

	void SetAlphaLevels(SP_VectorDouble levels) { m_nAlphalevels = levels; }
	SP_VectorDouble GetAlphaLevels() { return m_nAlphalevels; }

	//set and get result matrix
	//void SetFuzzyResultMatrix(const vector<SP_Vector2DDouble> p_pcResultMatrix) { m_pcResultMatrix = p_pcResultMatrix; }
	const vector<SP_Vector2DDouble> GetFuzzyResultMatrix() { return m_pcResultMatrix; }
	const ResultFuzzyBand GetFuzzyBand() { return m_FuzzyBandResult; }
	//get the curve value at a certain point and certain result Matrix
	inline double GetValue(const unsigned long& p_nPoint,const SP_Vector2DDouble& resMatrix)
	{
		double l_nValue = 0.0;
		 

		if (resMatrix.data() != NULL && p_nPoint< resMatrix.size())
		{
			l_nValue = resMatrix[p_nPoint][m_nDataColumn];
		}

		

		return l_nValue;
	}
};

typedef std::list<SP_DS_ViewerFuzzyCurve*> SP_ListFuzzyCurve;
//typedef std::vector<SP_DS_Viewer_Fuzzy_Curve*> SP_VectorFuzzyCurve;

#endif /* SP_DS_VIEWERFUZZYCURVE_H_ */
