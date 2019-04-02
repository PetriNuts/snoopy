/*
 * SP_DS_ResultViewer.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 06.06.2011
 * Short Description:
 */ //=================================================
#ifndef SP_DS_RESULTVIEWER_H_
#define SP_DS_RESULTVIEWER_H_

#include <list>
#include <sp_core/SP_Vector.h>
#include <sp_core/base/SP_Error.h>
#include <sp_core/SP_Map.h>
#include "sp_ds/extensions/ResultViewer/AttributeTypes/SP_DS_ViewerAttribute.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_ViewerCurve.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_ViewerFuzzyCurve.h"//Added By G.A
#include "sp_ds/extensions/ResultViewer/SP_DS_Viewer_Fuzzy_Curve.h"//Added By G.A
#include "sp_gui/dialogs/dia_FPN/SP_Compressed_Fuzzy_Band.h"//Added By G.A

#include "spsa/spsa.h"

/*
 * Base class for showing the simulation output either using table or
 * plot view. To select one of these viewers use one of the children
 * classes.
 */

typedef std::list<SP_DS_ViewerAttribute*> SP_ViewerAttributeList;

class SP_DS_Metadata;

class SP_DS_ResultViewer: public SP_Error
{
protected:
	/*
	 * pointer to the parent window
	 */
	wxWindow* m_pcParent;
	/*
	 * Pointer to a sizer where the control will be added
	 */
	wxSizer* m_pcSizer;

	/*
	 * pointer to the time vector (simulation time CPU time ... etc)
	 * It will be added to the x -axes in case of plot and in the index in case of table
	 */
	const SP_VectorDouble* m_pcXAxisValues;

	const SP_VectorDouble* m_pcYAxisValues;//for fuzzy membership function

	/*
	 * a list of shown curves
	 */

	std::list<unsigned int> m_apcShownCurvesList;

	/*
	 * all viewer curves
	 */
	SP_VectorCurve m_aPlotCurves;

	SP_VectorFuzzyCurve m_aPlotFuzzyCurves;// Added by G.A


	/*
	 * Name of the viewer to be displayed
	 */
	wxString m_sViewerName;

	/*
	 * Viewer properties
	 */
	SP_ViewerAttributeList* m_pcAttributeList;

	//Attribute classification
	SP_VectorString m_asAttributeCategories;

	/*
	 * Viewer title
	 */
	wxString m_sTitle;

	/*
	 * store a list of available extension for the current result viewer
	 */
	wxString m_sSupportedSaveExtensions;

	//Map curve name to positions
	SP_MapString2Long m_mCurveName2Postion;
protected:

public:
	//Set a reference to the time serie
	virtual void SetXAxisValues(const SP_VectorDouble* p_pcXAxisValues)
	{
		m_pcXAxisValues = p_pcXAxisValues;
	}
	//set a ref to the alphalevels
	virtual void SetYAxisValues(const SP_VectorDouble* p_pcYAxisValues)
	{
		m_pcYAxisValues = p_pcYAxisValues;
	}

	//Add curve to the list of curves
	virtual void AddCurve(const wxString& p_sCurveName, const unsigned long& p_nDataColumn, const SP_Vector2DDouble* p_pcResultMatrix, const wxString& p_sColor = wxT(""), const int& p_nLineWidth = -1,
			const int& p_nLineStyle = -1);
	virtual void AddFuzzyCurve(const wxString& p_sCurveName, const unsigned long& p_nDataColumn, const std::vector<SP_Vector2DDouble> p_pcResultMatrix, const ResultFuzzyBand fBand, SP_VectorDouble alphaLevels, unsigned long levels, unsigned long points, unsigned int fn, const wxString& p_sColor = wxT(""), const int& p_nLineWidth = -1,
		const int& p_nLineStyle = -1);//Added by G.A
	virtual void AddFuzzyCurves(const wxString& p_sCurveName, const unsigned long& p_nDataColumn, SP_Compressed_Fuzzy_Band* p_pcCompressedResult, const wxString& p_sColor = wxT(""), const int& p_nLineWidth = -1,
		const int& p_nLineStyle = -1);//Added by G.A
	//removes a curve with a specified name from the curves vector

	virtual void ClearCurves();

	virtual void ClearShownCurves()
	{
		m_apcShownCurvesList.clear();
	}

	//Update the current view. It has to be implemented in the derived class
	virtual void Update()=0;

	virtual void UpdateMembershipViewer(double TimePoint) = 0;

	/*
	 * Create a viewer
	 */
	virtual void Create()=0;

	/*
	 * Destroy a viewer
	 */
	virtual void Destroy()=0;

	virtual wxString GetName()
	{
		return m_sViewerName;
	}

	virtual SP_ViewerAttributeList* GetAttributes()
	{
		return m_pcAttributeList;
	}

	virtual SP_ViewerAttributeList GetAttributesByCategory(const wxString& p_sCategory);

	//Add a new attribute category
	virtual void AddViewerAttributeCategory(const wxString& p_sCategory)
	{
		m_asAttributeCategories.push_back(p_sCategory);
	}

	//Get a reference to the attribute categories
	virtual SP_VectorString& GetAttributeCategories()
	{
		return m_asAttributeCategories;
	}
	/*
	 * Update the viewer attributes. It will be called automatically when the
	 * ok or apply buttons pressed from the properties dialog
	 */
	virtual void UpdateAttributes();

	//set the attribute values to the current one
	virtual void LoadCurrentAttributeValues();

	//get attribute by name
	virtual SP_DS_ViewerAttribute* GetAttributeByName(const wxString& p_sName);

	//Set curve color
	virtual void SetCurveColor(const unsigned long& p_nIndex, const wxString& p_sColor);
	virtual void SetCurveColor(const wxString& l_nName, const wxString& p_sColor);

	//get curve colour
	virtual wxString GetCurveColor(const unsigned long& p_nIndex);
	virtual wxString GetCurveColor(const wxString& p_sName)
	{
		return GetCurveColor(m_mCurveName2Postion[p_sName]);
	}

	virtual void SetCurveLineWidth(const unsigned long& p_nIndex, const int& p_nLineWidth);
	virtual void SetCurveLineWidth(const wxString& l_nName, const int& p_nLineWidth);

	//get line width
	virtual int GetCurveLineWidth(const unsigned long& p_nIndex);
	virtual int GetCurveLineWidth(const wxString& p_sName)
	{
		return GetCurveLineWidth(m_mCurveName2Postion[p_sName]);
	}

	virtual void SetCurveLineStyle(const unsigned long& p_nIndex, const int& p_nLineSyle);
	virtual void SetCurveLineStyle(const wxString& l_nName, const int& p_nLineSyle);

	//Get line style
	virtual int GetCurveLineStyle(const unsigned long& p_nIndex);
	virtual int GetCurveLineStyle(const wxString& p_sName)
	{
		return GetCurveLineStyle(m_mCurveName2Postion[p_sName]);
	}

	/*
	 * show/hide curve.
	 */
	virtual void ShowCurve(const unsigned long& index, const bool& p_bShow);
	virtual void ShowCurve(const wxString& p_sName, const bool& p_bShow);

	unsigned int GetCurveCount()
	{
		return m_aPlotCurves.size();
	}

	SP_DS_ResultViewer(wxWindow* p_pcParent, wxSizer* p_pcSizer);
	virtual ~SP_DS_ResultViewer();

	std::list<unsigned int>& GetShownCurveList()
	{
		return m_apcShownCurvesList;
	}

	/*
	 * get a list of supported extensions
	 */
	wxString GetSupportedSaveExtensions()
	{
		return m_sSupportedSaveExtensions;
	}

	/*
	 * saves the current result view output
	 */
	virtual void SaveOutput(const wxString& p_sFileName, const int& p_sFileType);

	virtual void SaveViewToServerFormat(spsa::ModelView* p_pcServerSideView);
	virtual void LoadViewFromServerFormat(spsa::ModelView* p_pcServerSideView);

	virtual void SaveViewToSnoopyFormat(SP_DS_Metadata* p_pcView);
	virtual void LoadViewFromSnoopyFormat(SP_DS_Metadata* p_pcView);

	wxString GetColourString(unsigned int p_nIndex);
};

typedef std::vector<SP_DS_ResultViewer*> SP_VectorViewer;

#endif /* SP_DS_RESULTVIEWER_H_ */
