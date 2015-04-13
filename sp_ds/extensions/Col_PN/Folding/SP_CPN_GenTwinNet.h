//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2010/08/01 10:20:00 $
// Short Description: generate twin nets class
//////////////////////////////////////////////////////////////////////

#ifndef __SP_CPN_GenTwinNet__
#define __SP_CPN_GenTwinNet__

#include <wx/wx.h>
#include "sp_core/SP_Vector.h"
#include "sp_core/SP_Map.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"




class SP_CPN_GenTwinNet
{

private:
	wxString m_sFilePath;
	SP_DS_Graph* m_pcGraph;
	SP_GUI_Canvas* m_pcCanvas;
	SP_MDI_Doc*  m_pcDoc;
	SP_MDI_View* m_pcView;
	wxString m_sColorMax;

	wxString m_sColorSetName;
	wxString m_sType;
	wxString m_sColors;

	wxString m_sVariableName;


	wxString m_sFusionPlaceName;
	wxString m_sScheduleTransitionName;
	wxString m_sCoarsePlaceName;

	wxString m_sNewPlaceName;
	wxString m_sNewTransitionName;

	wxString m_sParameterName;

	wxString m_sParameterValue;

	bool m_bGenerateSideConditions;
	
	double m_dOffValue;

	map< wxString, vector<wxString> > m_mvTransitionToColors;


	double m_dNetMaxX;
	double m_dNetMaxY;

	SP_ListGraphic m_lShapes;
	SP_ListGraphic m_lEdges;

	vector<SP_DS_Node*> m_vPlaceNodes;
	vector<SP_DS_Node*> m_vTransNodes;
	vector<SP_DS_Edge*> m_vEdges;


public:
	SP_CPN_GenTwinNet();
	~SP_CPN_GenTwinNet();

	bool GenerateTwinNet(SP_DS_Graph* p_pcGraph,SP_GUI_Canvas* p_pcCanvas,SP_MDI_Doc* p_pcDoc, SP_MDI_View* p_pcView);
	bool GenerateAnyNet(SP_DS_Graph* p_pcGraph,SP_MDI_View* p_pcView);

private:
	bool GenerateColorSets();
	bool GenerateNewNet();
	bool GenerateAnyColorNet();
	bool GetSelectedNodes(SP_MDI_View* p_pcView);
	

	void OnCoarse(SP_ListGraphic* p_plShapes, SP_ListGraphic* p_plEdges);
	bool DoCoarse(SP_ListGraphic* p_plShapes, double p_nX,double p_nY, const wxString& p_sCoarseNode);

	void GetNetSize();
	


};

#endif // __SP_CPN_GenTwinNet__

