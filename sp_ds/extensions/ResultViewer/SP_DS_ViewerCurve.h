/*
 * SP_DS_ViewerCurve.h
* $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 21.06.2011
 * Short Description:
 */
//=================================================


#ifndef SP_DS_VIEWERCURVE_H_
#define SP_DS_VIEWERCURVE_H_

#include<wx/wx.h>
#include <wx/string.h>
#include "sp_core/SP_Vector.h"
#include <list>

class SP_DS_ViewerCurve
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
	      const SP_Vector2DDouble*  m_pcResultMatrix;
public:
		  SP_DS_ViewerCurve(const wxString& p_sName,const unsigned long& p_nDataColumn,
				            const wxString& p_nColor=wxT("0"),const int& p_nLineWidth=-1,
				            const int& p_nLineStyle=-1,const SP_Vector2DDouble*  p_pcResultMatrix=NULL);
		  virtual ~SP_DS_ViewerCurve();

		  //curve name
		  void SetName(const wxString& p_sName){m_sName=p_sName;}
		  wxString GetName(){return m_sName;}

		  //Column index
		  void SetDataColumn(const unsigned long& p_nColumn){m_nDataColumn=p_nColumn;}
		  unsigned long GetDataColumn(){return m_nDataColumn;}

		  //color
		  void SetColor(const wxString& p_nColor){m_nColor=p_nColor;}
		  wxString& GetColor() {return m_nColor;}

		  //line width
		  void SetLineWidth(const int& p_nLineWidth){m_nLineWidth=p_nLineWidth;}
		  int GetLineWidth(){return m_nLineWidth;}

		  //line style
		  void SetLineStyle(const int& p_nLineStyle){m_nLineStyle=p_nLineStyle;}
		  int GetLineStyle(){return m_nLineStyle;}

		  //set and get result matrix
		  void SetResultMatrix(const SP_Vector2DDouble*  p_pcResultMatrix){m_pcResultMatrix=p_pcResultMatrix;}
		  const SP_Vector2DDouble* GetResultMatrix(){return m_pcResultMatrix;}

		  //get the curve value at a certain point
		  inline double GetValue(const unsigned long& p_nPoint)
		  {
		      double l_nValue=0.0;

			  if(m_pcResultMatrix!=NULL && p_nPoint<m_pcResultMatrix->size())
			  {
				  l_nValue=(*m_pcResultMatrix)[p_nPoint][m_nDataColumn];
			  }

			return l_nValue;
		  }
};

typedef std::list<SP_DS_ViewerCurve*> SP_ListCurve;
typedef std::vector<SP_DS_ViewerCurve*> SP_VectorCurve;

#endif /* SP_DS_VIEWERCURVE_H_ */
