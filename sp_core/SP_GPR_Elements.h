//////////////////////////////////////////////////////////////////////
// $Source: SP_GPR_Elements.h
// $Author: MD $
// $Version: $
// $Revision: $
// $Date: 2005/09/19 $
// Short Description: contains global elements preferences
//////////////////////////////////////////////////////////////////////

#if !defined(__SP_GPR_ELEMENTS_H__)
#define __SP_GPR_ELEMENTS_H__

//TODO: netclass/node depending defaults?
#define DEFAULT_FIXED 0
#define DEFAULT_WIDTH 20
#define DEFAULT_HEIGHT 20
#define BAR_RATIO 4	//aspect ratio between width and height
#define NODE_DIM_MIN 5
#define NODE_DIM_MAX 250

#define NODE_THICKNESS_MIN 1//by george
#define NODE_THICKNESS_MAX 20//by george
#define DEFAULT_THICKNESS 1
#include "sp_core/SP_GPR_Base.h"
#include "sp_gr/shapes/SP_GR_ExtendedDrawnShape.h"
#include "sp_gr/edges/SP_GR_ExtendedEdge.h"
#include "sp_gr/edges/SP_GR_FaultEdge.h"



class SP_DLG_GlobalPreferences;
class wxSpinCtrl;
class wxCheckBox;
class wxColourPickerCtrl;

class SP_GPR_Elements: public SP_GPR_Base {

private:
	//the mapping key is the net class and
	//the entry is a list of editable nodes for this class
	map<wxString, SP_SetString > m_mlClassNodes;

protected:

	//the mapping key is a combination of net class and the node class
	//such as: "<net class>|<node class>"
	SP_MapString2Int m_mnHeight;
	SP_MapString2Int m_mnWidth;
	SP_MapString2Int m_mnThickness;//by george
	SP_MapString2Bool m_mbFixedSize;

	map<wxString, wxString> m_msColorSetColor;
	map<wxString, wxString> m_msMarkingColor;
	map<wxString, wxString> m_msGuardColor;
	map<wxString, wxString> m_msExpressionColor;

	map<wxString, SP_EXTENDED_TYPE> m_mnDesignType;
	map<wxString, SP_EXTENDED_TYPE_EDGE> m_mnLineDesignType;
	map<wxString, SP_EXTENDED_ARROW_TYPE_EDGE> m_mnARLineDesignType;
	map<wxString, SP_EXTENDED_ARROW_TYPE_EDGE> m_mnALLineDesignType;
	SP_EXTENDED_ARC_TYPE m_edgeDesignType;

	SP_MapString2Bool m_mbEdgeSpline;

	//dialog controls
	//the mapping key is the node class
	map<wxString, wxSpinCtrl*> m_mscHeight;
	map<wxString, wxSpinCtrl*> m_mscWidth;

	map<wxString, wxSpinCtrl*> m_mscThickness;//by george
	map<wxString, wxCheckBox*> m_mcbFixed;

	map<wxString, wxColourPickerCtrl*> m_mcpColorSet;
	map<wxString, wxColourPickerCtrl*> m_mcpMarking;
	map<wxString, wxColourPickerCtrl*> m_mcpGuard;
	map<wxString, wxColourPickerCtrl*> m_mcpExpression;
	map<wxString, wxCheckBox*> m_mcbDotExpressionShow;
	SP_MapString2Bool m_mbDotExpressionShow;

	//predefined shapes for transitions in (ext.)PTN
	wxRadioBox *m_rbTransShape;
	//brush style for logical places in (ext.)PTN
	wxRadioBox *m_rbLogicBrush;
	//display Edge as Spline or Line
	wxRadioBox* m_rbEdgeSpline;


	  wxListBox* m_lbListBox;
	  wxListBox* m_lbNodeListBox;
	  wxArrayString m_asListBoxValues;
	  wxArrayString m_ListBoxValues;
	  wxListBox* m_LineListBox;
	  wxListBox* m_ARListBox;
	  wxListBox* m_ALListBox;
	  wxArrayString m_LineListBoxValue;
	  wxArrayString m_ARListBoxValue;
	  wxArrayString m_ALListBoxValue;

public:
	SP_GPR_Elements();
	virtual ~SP_GPR_Elements();

	//setter and getter
	void SetNodeWidth(const wxString& p_sNetClass, const wxString& p_sNodeClass, int p_nVal);
	int GetNodeWidth(const wxString& p_sNetClass, const wxString& p_sNodeClass);

	void SetNodeHeight(const wxString& p_sNetClass, const wxString& p_sNodeClass, int p_nVal);
	int GetNodeHeight(const wxString& p_sNetClass, const wxString& p_sNodeClass);

	//by george
	void SetNodeThickness(const wxString& p_sNetClass, const wxString& p_sNodeClass, int p_nVal);
	int GetNodeThickness(const wxString& p_sNetClass, const wxString& p_sNodeClass);

	void SetNodeFixed(const wxString& p_sNetClass, const wxString& p_sNodeClass, bool p_bVal);
	bool GetNodeFixed(const wxString& p_sNetClass, const wxString& p_sNodeClass);

	void SetNodeDesignType(const wxString& p_sNetClass, SP_EXTENDED_TYPE p_nVal);
	SP_EXTENDED_TYPE GetNodeDesignType(const wxString& p_sNetClass);

	void SetLineEdgeDesignType(const wxString& p_sNetClass, SP_EXTENDED_TYPE_EDGE p_nVal);
	SP_EXTENDED_TYPE_EDGE GetLineEdgeDesignType(const wxString& p_sNetClass);

	void SetArrowEdgeTypeR(const wxString& p_sNetClass, SP_EXTENDED_ARROW_TYPE_EDGE p_nVal);
	SP_EXTENDED_ARROW_TYPE_EDGE GetArrowEdgeTypeR(const wxString& p_sNetClass);

	void SetArrowEdgeTypeL(const wxString& p_sNetClass, SP_EXTENDED_ARROW_TYPE_EDGE p_nVal);
	SP_EXTENDED_ARROW_TYPE_EDGE GetArrowEdgeTypeL(const wxString& p_sNetClass);

	void SetEdgeDesignType(const wxString& p_sNetClass, SP_EXTENDED_ARC_TYPE p_nVal);
	SP_EXTENDED_ARC_TYPE GetEdgeDesignType();

	void SetEdgeSpline(const wxString& p_sNetClass, bool p_bVal=false);
	bool GetEdgeSpline(const wxString& p_sNetClass);

	void SetColorSetColor(const wxString& p_sNetClass, const wxString& p_sNodeClass, wxString p_sVal);
	wxString GetColorSetColor(const wxString& p_sNetClass, const wxString& p_sNodeClass);

	void SetMarkingColor(const wxString& p_sNetClass, const wxString& p_sNodeClass, wxString p_sVal);
	wxString GetMarkingColor(const wxString& p_sNetClass, const wxString& p_sNodeClass);

	//stochastic/continuous transition
	void SetGuardColor(const wxString& p_sNetClass, const wxString& p_sNodeClass, wxString p_sVal);
	wxString GetGuardColor(const wxString& p_sNetClass, const wxString& p_sNodeClass);

	void SetExpressionColor(const wxString& p_sNetClass, wxString p_sVal);
	wxString GetExpressionColor(const wxString& p_sNetClass);


	void SetExpressionShowOption(const wxString& p_sNetClass, bool p_bVal);
	bool GetExpressionShowOption(const wxString& p_sNetClass);




	//this is called from the net class constructor to register its editable node classes
	void RegisterNodeclass(const wxString& p_sNetClass, const wxString& p_pcNodeclass);
	bool AddToDialogPage(const wxString& p_sNetClass, SP_WDG_NotebookPage *p_pcPage);

	bool OnDialogOk(const wxString& p_sNetClass);
	void OnUpdateTransShape();
	void OnUpdateTransWidth();
	void OnUpdateTransHeight();

	bool UpdateFromConfig(wxConfig& p_config);
	bool WriteToConfig(wxConfig& p_config);

};

#endif // !defined(__SP_GPR_ELEMENTS_H__)
