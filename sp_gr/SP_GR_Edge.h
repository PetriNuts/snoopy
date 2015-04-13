//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Graphical Edge-Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_EDGE_H__
#define __SP_GR_EDGE_H__

#include "sp_core/base/SP_Error.h"
#include "sp_core/base/SP_Type.h"
#include "sp_core/base/SP_Graphic.h"

#include "sp_core/SP_List.h"

#include "sp_ds/SP_DS_Edge.h"

class SP_GR_Edge: public SP_Error, public SP_Graphic
{
private:
protected:
    SP_Graphic* m_pcSource;
    SP_Graphic* m_pcTarget;

    bool SynchronizePrim();

    bool ResetState(SP_Graphic* p_pcGraphic);
public:
    SP_GR_Edge(SP_DS_Edge* p_pcDataParent);
    ~SP_GR_Edge();

    //-----------------------------------------------------
    // Accessors
    //! we need something special here
    virtual double  GetPosAttributesX();
    virtual double  GetPosAttributesY();
    virtual void    GetPosAttributesXY(double* p_pnVal1, double* p_pnVal2);

    virtual double  GetPosX();
    virtual double  GetPosY();

    virtual double GetLength();

    virtual wxShape* GetPrimitive() = 0;

    //-----------------------------------------------------
    //! - in base class
    bool ShowAttributeGraphic();
    bool AddToCanvas(SP_GUI_Canvas* p_pcCanvas, double p_nX = -1, double p_nY = -1, int p_nKeys = 0);
    bool AddToDeleteQueue(SP_GRAPHIC_STATE p_eFilter = SP_STATE_ALL);

    virtual SP_Graphic* Clone(SP_Data* p_pcParent) = 0;

    virtual bool OnDoubleClick(int p_nKeys);

    bool EditProperties();

    virtual bool Update(bool p_bLocalOnly = FALSE);
    bool OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet, SP_ListGraphic* p_plGraphics = NULL);
    bool OnCoarse(unsigned int p_nNewNet, SP_Graphic* p_pcCoarseGr, SP_ListGraphic* p_plShapes);

    //! special to edges
    /**	\brief	The SetSource function

	    \param	p_pcSource	new graphic to act as source

	    \return	SP_GR_Edge* ourself

	    Sets the member m_pcSource to the value of the parameter, regardless,
        if it is NULL.
        Calls SynchronizePrim afterwards.
    */
    SP_GR_Edge* SetSource(SP_Graphic* p_pcSource);
    /**	\brief	The SetTarget function

	    \param	p_pcTarget	new graphic to act as target

	    \return	SP_GR_Edge* ourself

	    Sets the member m_pcTarget to the value of the parameter, regardless,
        if it is NULL.
        Calls SynchronizePrim afterwards.
    */
    SP_GR_Edge* SetTarget(SP_Graphic* p_pcTarget);

    /**	\brief	The GetSource function

	    \return	SP_Graphic* the object of class SP_Graphic, acting as source

        Accessor
    */
    inline SP_Graphic* GetSource() const { return m_pcSource; }
    /**	\brief	The GetTarget function


	    \return	SP_Graphic* the object of class SP_Graphic, acting as target

	    Accessor
    */
    inline SP_Graphic* GetTarget() const { return m_pcTarget; }

    /**	\brief	The ReleaseSource function

	    \return	bool    TRUE in case of sucess, FALSE otherwise

	    Calls RemoveSourceEdge on the source node and sets the member
        m_pcSource = NULL, to have it stay in valid condition if the
        object may be deleted.
    */
    bool ReleaseSource();
    /**	\brief	The ReleaseTarget function


	    \return	bool    TRUE in case of sucess, FALSE otherwise

	    Calls RemoveTargetEdge on the target node and sets the member
        m_pcTarget = NULL, to have it stay in valid condition if the
        object may be deleted.
    */
    bool ReleaseTarget();

    /**	\brief	The Split function

	    \param	p_pcOldNode	the former parent (to decide src/target association)
	    \param	p_pcGrNode	the graphic, this edge is connected to
	    \param	p_pcNewNode	the new node

	    \return	bool TRUE in case of success, FALSE otherwise

	    Called from within SP_GR_Node::SplitEdges. Removes the parent (SP_DS_Edge)
        from the list of edges at either source or target and inserts the third
        parameter in the same roll (source or target).
    */
    bool Split(SP_Data* p_pcOldNode, SP_Graphic* p_pcGrNode, SP_Data* p_pcNewNode);

    /**	\brief	The OnConsumeEdge function

	    \param	p_pcHit	the target of the event handling
	    \param	p_nType	CONTROL_POINT_ENDPOINT_FROM or CONTROL_POINT_ENDPOINT_TO

	    \return	bool TRUE in case of success, FALSE otherwise

    	Called from within the edges event handler, if the user tried to
        move a control point on either end of the edge.
        In here, we test the p_pcHit object for validity and call
        ConsumeSourceEdge or ConsumeTargetEdge on the p_pcHit object,
        depending on the state of p_nType.
    */
    virtual bool OnConsumeEdge(wxShape* p_pcHit, int p_nType);

    //! set the line control points for the OGL primitive
    /**	\brief	The SetControlPoints function

	    \param	p_pcCanvas	the canvas, possibly holding a temp list

	    \return	bool TRUE in case of success, FALSE otherwise

	    Just testing the parameter for existence and returning the
        state of SetControlPoints with the list of control points, stored
        in the canvas.
        This usually happens, if the user doesn't draw a line directly, but
        with some waypoints in between.
    */
    bool SetControlPoints(SP_GUI_Canvas* p_pcCanvas);
    /**	\brief	The SetControlPoints function

	    \param	p_pcPoints	the pointer to the list of points
        \param  p_pnOffset  additional offset to be used while adding the points

	    \return	bool    TRUE in case of success, FALSE otherwise

	    Copies the values of the coordinates in the list to the control
        points of the associated primite (derived from wxLineShape!).
    */
    bool SetControlPoints(wxList* p_pcPoints, double p_pnOffset = 0.0);

    //! for call from within derived_classes::Clone method
    virtual bool CloneBase(SP_Graphic* p_pcCopy);

    bool Rotate(double p_nX, double p_nY, double p_nAngle = 90.0);
    bool Flip(double p_nX);
    bool Mirror(double p_nY);
    bool Translate(double p_nX, double p_nY);
    bool AddToCopyMaps(SP_DS_Graph** p_ppcGraph, SP_MapGraphic2Graphic* p_pmGraphic2Graphic, SP_MapData2Data* p_pmData2Data);
    virtual wxBrush* GetBrush();
};

#endif // __SP_GR_EDGE_H__

