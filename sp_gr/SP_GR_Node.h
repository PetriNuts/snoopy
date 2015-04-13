//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Graphical Node-Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_NODE_H__
#define __SP_GR_NODE_H__

#include "sp_core/base/SP_Error.h"
#include "sp_core/base/SP_Type.h"
#include "sp_core/base/SP_Graphic.h"

#include "sp_core/SP_List.h"

//#include "sp_ds/SP_DS_Node.h"
class SP_DS_Node;

class SP_GR_Node: public SP_Error, public SP_Graphic
{
private:
protected:
    // indicates, whether the shape is editable
    // default is TRUE
    bool m_bFixedSize;
public:
    SP_GR_Node(SP_DS_Node* p_pcDataParent);
    virtual ~SP_GR_Node();

    //-----------------------------------------------------
    // Accessors
    virtual bool SetWidth(double p_nVal);
    virtual double GetWidth();
    virtual bool SetHeight(double p_nVal);
    virtual double GetHeight();

    virtual wxShape* GetPrimitive() = 0;

    //-----------------------------------------------------
    //! - in base class
    void SetFixedSize(bool p_bVal);

    inline bool GetFixedSize() { return m_bFixedSize; }

    virtual SP_Graphic* AddSourceEdge(SP_Graphic* p_pcEdge, SP_Data* p_pcSource);
    virtual SP_Graphic* AddTargetEdge(SP_Graphic* p_pcEdge, SP_Data* p_pcTarget);

    virtual bool SetNewSource(SP_Graphic* p_pcSource);
    virtual bool SetNewTarget(SP_Graphic* p_pcTarget);

    virtual bool RemoveSourceEdge(SP_Graphic* p_pcEdge, bool p_bInDs = TRUE);
    virtual bool RemoveTargetEdge(SP_Graphic* p_pcEdge, bool p_bInDs = TRUE);

    virtual bool ConsumeSourceEdge(SP_Graphic* p_pcEdge);
    virtual bool ConsumeTargetEdge(SP_Graphic* p_pcEdge);

    /**	\brief	The SplitEdges function

	    \param	p_pcNode	a parameter of type SP_DS_Node*

	    \return	bool    TRUE in case of success, FALSE otherwise

	    Called from the protected member function of
        SP_DS_Node::SplitEdges(SP_Graphic*, SP_DS_Node*) to set the associations
        in the DS of all graphical edges, starting or ending at this node, right.

    */
    bool SplitEdges(SP_DS_Node* p_pcNode);

    bool ShowAttributeGraphic();

    bool AddToCanvas(SP_GUI_Canvas* p_pcCanvas, double p_nX = -1, double p_nY = -1, int p_nKeys = 0);
    bool AddToDeleteQueue(SP_GRAPHIC_STATE p_eFilter = SP_STATE_ALL);
    bool HasUndeletedEdges();

    virtual bool Merge(SP_Graphic* p_pcTarget);
    virtual bool Coarse(unsigned int p_nNewNet, SP_Graphic* p_pcCoarseGr, SP_ListGraphic* p_plShapes);

    virtual SP_Graphic* Clone(SP_Data* p_pcParent) = 0;

    virtual bool OnDoubleClick(int p_nKeys);

    bool EditProperties();

    virtual bool Update(bool p_bLocalOnly = FALSE);
    virtual bool ResetGraphicState();

    //! for call from within derived_classes::Clone method
    virtual bool CloneBase(SP_Graphic* p_pcCopy);
};

#endif // __SP_GR_NODE_H__
