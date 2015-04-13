//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Extension for netelements (only nodes actually)
//////////////////////////////////////////////////////////////////////
#ifndef __SP_LAYOUT_H__
#define __SP_LAYOUT_H__

#include <string>

#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gr/SP_GR_Edge.h"
#include "sp_gr/SP_GR_Node.h"

#include "sp_gui/dialogs/SP_DLG_LayoutProperties.h"

struct NodeData
{
	unsigned long Id;
	std::string Name;
	double X;
	double Y;
	double W;
	double H;
	SP_GR_Node* NodeGr;
	NodeData():Id(0), Name(""), X(0), Y(0), W(20), H(20), NodeGr(NULL) {}
};

struct EdgeData
{
	unsigned long Id;
	unsigned long SourceId;
	unsigned long TargetId;
	SP_GR_Edge* EdgeGr;
	std::vector<double> X;
	std::vector<double> Y;
	EdgeData():Id(0), SourceId(0), TargetId(0), EdgeGr(NULL) {}
};

typedef map<unsigned long, NodeData*> MapNodeData;
typedef map<unsigned long, EdgeData*> MapEdgeData;

class SP_Layout: public SP_Error
{
private:
	SP_MapString2String m_lAttributes;

	double m_nMaxX;
	double m_nMaxY;

protected:
    //! Graph to show
    SP_DS_Graph* m_pcGraph;
	SP_MDI_Doc* m_pcDoc;

	wxString m_sLayout;

	MapNodeData m_mNodes;
	MapEdgeData m_mEdges;


	void Init(unsigned int p_nNetNumber);
	void Clear();

	bool ResetNodes();
	bool RefreshNodes();
	bool ResetEdgesAttributes();
	bool ResetEdges();

	virtual bool DoVisualization() = 0;

public:

    SP_Layout();
    virtual ~SP_Layout();

    bool Visualization(SP_DLG_LayoutProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc);

};

#endif // __SP_LAYOUT_H__


