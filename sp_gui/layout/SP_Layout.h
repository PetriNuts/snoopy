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
	unsigned long Id = 0;
	std::string Name = "";
	double X = 0.0;
	double Y = 0.0;
	double W = 20.0;
	double H = 20.0;
	SP_GR_Node* NodeGr = nullptr;
};

struct EdgeData
{
	unsigned long Id = 0;
	unsigned long SourceId = 0;
	unsigned long TargetId = 0;
	SP_GR_Edge* EdgeGr = nullptr;
	std::vector<double> X;
	std::vector<double> Y;
};

typedef map<unsigned long, NodeData> MapNodeData;
typedef map<unsigned long, EdgeData> MapEdgeData;

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


