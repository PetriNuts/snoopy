//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GRAPHIC_H__
#define __SP_GRAPHIC_H__

#include "sp_core/base/SP_NetElement.h"
//! for SP_Data and SP_MapString2ListAttribute
#include "sp_core/base/SP_Data.h"
#include "sp_utilities.h"


#include "sp_gui/windows/SP_GUI_Canvas.h"

#include <wx/object.h>
#include <wx/xml/xml.h>
#include <wx/ogl/ogl.h>

// Element Graphics
enum SP_GRAPHIC_TYPE
{
    SP_GRAPHIC_NULL,
    SP_GRAPHIC_NODE,
    SP_GRAPHIC_METADATA,
    SP_GRAPHIC_NODEVECTOR,
    SP_GRAPHIC_EDGE,
    SP_GRAPHIC_ATTRIBUTE,
    // special
    SP_GRAPHIC_ANIMATOR

};

enum SP_GRAPHIC_STATE
{
    SP_STATE_NORMAL = 1,
    SP_STATE_MASTER = 2, //!< Master graphic, removing this means, to delete everything. this is special to edges, that connect coarse-border-nodes
    SP_STATE_COARSEBORDERTOP = 4, //!< connected to or connecting a COARSECOARSE
    SP_STATE_COARSEBORDERDOWN = 8, //!< depends to COARSEBORDERTOP
    SP_STATE_COARSECOARSE = 16, //!< the graphic of the coarsenode itself

    //! the following are somewhat special for filtering
    SP_STATE_ALL = 255,
    SP_STATE_NONE = 0,
    SP_STATE_SINGLE = 256,
	SP_STATE_NEG
};

enum SP_EQUALTEST_FLAG
{
    SP_EQUALTEST_ALL = 255,
    SP_EQUALTEST_DATA = 1,
    SP_EQUALTEST_GRAPHICSTATE = 2,
    SP_EQUALTEST_GRAPHICTYPE = 4
};

class SP_Graphic;

/**	\brief	The SP_Graphic class

  This is the base class for all objects, that deal with displaying data in the GUI.
  It encapsulated core functionality for most of its functions, but exposes its interface
  to any of its derivations to be reimplemented there.

  A lot of derived classes (i.e. SP_GR_Node, SP_GR_Edge, etc.) do some special stuff
  within their overloaded methods, but almost everytime exit with a call to the
  base implementation right here.

  Please note, that none of the classes in the "sp_gr/base" directory is derived from SP_Graphic
  intentionally! This approach was skipped by the author after dealing with it for a long time in
  Snoopy 1. Nowadays the SP_Graphic derived classes represent a close connection to the
  datastructure part (SP_Data and derivations) and hold only lose connections to the concrete
  objects displayed to the user via their m_pcPrimitive members (see the classes within the
  "sp_gr/shapes", "sp_gr/attributes" and "sp_gr/edges" directories of the source tree.
*/
class SP_Graphic: public SP_NetElement, public SP_IdCounter
{
private:
protected:
	SP_ListGraphic m_lGraphicChildren;
    SP_GRAPHIC_TYPE m_eGraphicType;
    //! show on canvas
    bool m_bShow;
    //! flag for special states, will be mostly SP_STATE_NORMAL
    SP_GRAPHIC_STATE m_eGraphicState;
    //! individual default pen and brush members
    wxBrush* m_pcDefaultBrush;
    wxPen* m_pcDefaultPen;
    //! individual pen and brush members
    wxBrush* m_pcBrush;
    wxPen* m_pcPen;
    bool m_bBrushSet;
    bool m_bPenSet;
    SP_Data* m_pcParent;
    SP_Graphic* m_pcGraphicParent;
	wxString m_sTextColor;
public:
    SP_Graphic(SP_GRAPHIC_TYPE p_eType);
    virtual ~SP_Graphic();

    /**	\brief	The GetGraphicType function. Accessor
	    \return	SP_GRAPHIC_TYPE
    */
    inline SP_GRAPHIC_TYPE GetGraphicType() const { return m_eGraphicType; }
    /**	\brief	The SetGraphicState function. Accessor
	    \param	p_eVal	a parameter of type SP_GRAPHIC_STATE
	    \return	bool
    */
    bool SetGraphicState(SP_GRAPHIC_STATE p_eVal);

    virtual bool ResetGraphicState();
    /**	\brief	The GetGraphicState function. Accessor
	    \return	SP_GRAPHIC_STATE
    */
    inline SP_GRAPHIC_STATE GetGraphicState() const { return m_eGraphicState; }

    /**	\brief	The Equals function

	    \param	p_pcVal	a parameter of type SP_Graphic*
        \param  p_eFlag flag to filter, what to test

	    \return	bool TRUE if they are both the same or FALSE otherwise

	    This method compares its own object with the content of the parameter
        and will be used within merge operations.
        The flag is used in combination with SP_GUI_Canvas::MergeAll to avoid testing
        the graphic state.
    */
    virtual bool Equals(SP_Graphic* p_pcVal, const int p_eFlag = SP_EQUALTEST_ALL);

    /**	\brief	The SetDataParent function

	    \param	p_pcParent	SP_Data derived object, acting as parent

	    \return	SP_Data* the parameter

    */
    SP_Data* SetDataParent(SP_Data* p_pcParent);
    /**	\brief	The GetParent function. Simple accessor


	    \return	SP_Data* the parent if any, or NULL
    */
    SP_Data* GetParent();

    /**	\brief	The AddSourceEdge function

	    \param	p_pcEdge	the graphic of the edge to be added
	    \param	p_pcSource	the datastructure of the edge to be added

	    \return	SP_Graphic*     itself in case of success, or NULL otherwise

	    Just interface and reimplemented in SP_GR_Node only for now.
    */
    virtual SP_Graphic* AddSourceEdge(SP_Graphic* p_pcEdge, SP_Data* p_pcSource) { return NULL; }
    /**	\brief	The AddTargetEdge function

	    \param	p_pcEdge	the graphic of the edge to be added
	    \param	p_pcTarget	the datastructure of the edge to be added

	    \return	SP_Graphic*     itself in case of success, or NULL otherwise

	    Just interface and reimplemented in SP_GR_Node only for now.
    */
    virtual SP_Graphic* AddTargetEdge(SP_Graphic* p_pcEdge, SP_Data* p_pcTarget) { return NULL; }
    /**	\brief	The SetNewSource function

	    \param	p_pcSource	the datastructure of the edge to be added

	    \return	bool    TRUE in case of succes, FALSE otherwise

	    Just interface and reimplemented in SP_GR_Node only for now.
    */
    virtual bool SetNewSource(SP_Graphic* p_pcSource) { return false; }
    /**	\brief	The SetNewTarget function

	    \param	p_pcTarget	the datastructure of the edge to be added

	    \return	bool    TRUE in case of succes, FALSE otherwise

	    Just interface and reimplemented in SP_GR_Node only for now.
    */
    virtual bool SetNewTarget(SP_Graphic* p_pcTarget) { return false; }

    /**	\brief	The RemoveSourceEdge function

	    \param	p_pcEdge	edge to be removed

	    \return	bool    TRUE in case of success, FALSE otherwise

	    Just interface and reimplemented in SP_GR_Node only for now.
    */
    virtual bool RemoveSourceEdge(SP_Graphic* p_pcEdge, bool p_bInDs = TRUE) { return FALSE; }
    /**	\brief	The RemoveTargetEdge function

	    \param	p_pcEdge	edge to be removed

	    \return	bool    TRUE in case of success, FALSE otherwise

	    Just interface and reimplemented in SP_GR_Node only for now.
    */
    virtual bool RemoveTargetEdge(SP_Graphic* p_pcEdge, bool p_bInDs = TRUE) { return FALSE; }

    /**	\brief	The ConsumeSourceEdge function

	    \param	p_pcEdge	edge to be 'consumed'

	    \return	bool    TRUE in case of success, FALSE otherwise

	    Just interface and reimplemented in SP_GR_Node only for now.
    */
    virtual bool ConsumeSourceEdge(SP_Graphic* p_pcEdge) { return FALSE; }
    /**	\brief	The ConsumeTargetEdge function

	    \param	p_pcEdge	edge to be 'consumed'

	    \return	bool    TRUE in case of success, FALSE otherwise

	    Just interface and reimplemented in SP_GR_Node only for now.
    */
    virtual bool ConsumeTargetEdge(SP_Graphic* p_pcEdge) { return FALSE; }

    /**	\brief	The EditProperties function

        Just an interface.
    */
    virtual bool EditProperties() { return TRUE; }

    /**	\brief	The AddChildToCanvas function

	    \param	p_pcChild	a SP_Graphic derived object to append to the list of children of this and to add it to the canvas

	    \return	bool        TRUE if succesful, FALSE otherwise

	    Appends another graphical object to the list of children of this object if not
        already added and displays it in the canvas.
    */
    virtual bool AddChildToCanvas(SP_Graphic* p_pcChild);

    /**	\brief	The Update function

        Just an interface.
    */
    virtual bool Update(bool p_bLocalOnly = FALSE) { return TRUE; }

    /**	\brief	The ShowAttributeGraphic function

        Just an interface.
    */
    virtual bool ShowAttributeGraphic() { return TRUE; }

    /**	\brief	The OnDoubleClick function

        Just an interface.
    */
    virtual bool OnDoubleClick(int p_nKeys) { return TRUE; }
    /**	\brief	The Select function

	    \param	p_bVal	value to determine the state
	    \param	p_pcDC  the device context

	    called from within the datastructure to switch the state of selection
    */
    virtual void Select(bool p_bVal, wxDC* p_pcDC = NULL);

    /**	\brief	The GetPrimitive function

        Just an interface.
    */
    virtual wxShape* GetPrimitive() = 0;

    /**	\brief	The GetGraphicParent function. Simple accessor


	    \return	SP_Graphic* the m_pcGraphicParent member
    */
    virtual SP_Graphic* GetGraphicParent() { return m_pcGraphicParent; }
    /**	\brief	The SetGraphicParent function. Simple accessor

	    \param	p_pcVal	    value to set the m_pcGraphicParent member to

	    \return	SP_Graphic* the parameter
    */
    virtual SP_Graphic* SetGraphicParent(SP_Graphic* p_pcVal) { m_pcGraphicParent = p_pcVal; return p_pcVal; }

    /**	\brief	The SetPosX function. Accessor

	    \param	p_nVal	new value
    */
    virtual bool    SetPosX(double p_nVal);
    /**	\brief	The GetPosX function. Accessor

	    \return	double  the value
    */
    virtual double  GetPosX();
    /**	\brief	The SetPosY function. Accessor

	    \param	p_nVal	new value
    */
    virtual bool    SetPosY(double p_nVal);
    /**	\brief	The GetPosY function. Accessor

	    \return	double  the value
    */
    virtual double  GetPosY();
    /**	\brief	The SetPosXY function. Accessor

	    \param	p_nVal1	a parameter of type double
	    \param	p_nVal2	a parameter of type double

	    \return	bool
    */
    virtual bool    SetPosXY(double p_nVal1, double p_nVal2);
    /**	\brief	The GetPosXY function. Accessor

	    \param	p_pnVal1	pointer to the variable to write the value in
	    \param	p_pnVal2	pointer to the variable to write the value in
    */
    virtual void    GetPosXY(double* p_pnVal1, double* p_pnVal2);
    /**	\brief	The GetBoundingBox function. Accessor

	    \param	p_pnX1	pointer to the variable to write the value in
	    \param	p_pnY1	pointer to the variable to write the value in
	    \param	p_pnX2	pointer to the variable to write the value in
	    \param	p_pnY2	pointer to the variable to write the value in
    */
    virtual void    GetBoundingBox(double* p_pnX1, double* p_pnY1, double* p_pnX2, double* p_pnY2);

    virtual bool SetWidth(double p_nVal) { return TRUE; }
    virtual double GetWidth() { return 10.0; }
    virtual bool SetHeight(double p_nVal) { return TRUE; }
    virtual double GetHeight() { return 10.0; }

    /**	\brief	The SetPosAttributesX function. Accessor

	    \param	p_nVal	new value
    */
    virtual bool    SetPosAttributesX(double p_nVal) { return FALSE; }
    /**	\brief	The GetPosAttributesX function. Accessor

	    \return	double  the value
    */
    virtual double  GetPosAttributesX() { return GetPosX(); }
    /**	\brief	The SetPosAttributesY function. Accessor

	    \param	p_nVal	new value
    */
    virtual bool    SetPosAttributesY(double p_nVal) { return FALSE; }
    /**	\brief	The GetPosAttributesY function. Accessor

	    \return	double  the value
    */
    virtual double  GetPosAttributesY() { return GetPosY(); }
    /**	\brief	The GetPosAttributesXY function. Accessor

	    \param	p_pnVal1	pointer to the variable to write the value in
	    \param	p_pnVal2	pointer to the variable to write the value in
    */
    virtual void    GetPosAttributesXY(double* p_pnVal1, double* p_pnVal2) { GetPosXY(p_pnVal1, p_pnVal2); }

    /**	\brief	The SetShow function. Accessor

	    \param	p_bVal	value of the show flag

	    \return	bool    TRUE in case of success, FALSE otherwise

        Switch displaying of this object (via its m_pcPrimitive meber) on or off.
        Mainly used for attributes
    */
    bool SetShow(bool p_bVal);
    /**	\brief	The GetShow function. Accessor

	    \return	bool  the value
    */
    inline bool GetShow() const { return m_bShow; }

    /**	\brief	The SetOffsetX function. Interface for SP_GR_Attribute

	    \param	p_nVal	value

	    \return	bool    TRUE in case of succes, FALSE otherwise
    */
    virtual bool SetOffsetX(double p_nVal) { return FALSE; }
    /**	\brief	The SetOffsetY function. Interface for SP_GR_Attribute

	    \param	p_nVal	value

	    \return	bool    TRUE in case of succes, FALSE otherwise
    */
    virtual bool SetOffsetY(double p_nVal) { return FALSE; }

    /**	\brief	The GetOffsetX function. Interface for SP_GR_Attribute

	    \return	double  The value

        Reimplemented in SP_GR_Attribute
    */
    virtual double GetOffsetX() { return 0.0; }
    /**	\brief	The GetOffsetY function. Interface for SP_GR_Attribute

	    \return	double  The value

        Reimplemented in SP_GR_Attribute
    */
    virtual double GetOffsetY() { return 0.0; }

    virtual double GetLength() { return 0.0; }

    wxBrush* GetDefaultBrush() { return m_pcDefaultBrush; }
    wxPen* GetDefaultPen() { return m_pcDefaultPen; }

    void SetDefaultBrush(wxBrush* p_pcBrush)
    {
    	if(p_pcBrush != NULL)
    	{
    		m_pcDefaultBrush = p_pcBrush;
    		SetBrush(p_pcBrush);
    	}
    }

    void SetDefaultPen(wxPen* p_pcPen)
    {
    	if(p_pcPen != NULL)
    	{
    		m_pcDefaultPen = p_pcPen;
    		SetPen(p_pcPen);
    	}
    }

	virtual wxColour GetBrushColour();
    virtual wxBrush* GetBrush();
	virtual wxColour GetPenColour();
    virtual wxPen* GetPen();
    inline bool GetPenSet() const { return m_bPenSet; }
    inline bool GetBrushSet() const { return m_bBrushSet; }

    inline void SetBrush(wxBrush* p_pcVal)
    {
		//Delete is not necessary, because of reference counting
    	m_pcBrush = p_pcVal;
    	m_bBrushSet = (m_pcBrush != NULL);
	}
	inline void SetBrushColour(wxColour p_pcVal)
	{
		SetBrush(wxTheBrushList->FindOrCreateBrush(p_pcVal, GetBrush()->GetStyle()));
	}

	inline void SetPen(wxPen* p_pcVal)
	{
		//Delete is not necessary, because of reference counting
		m_pcPen = p_pcVal;
		m_bPenSet = TRUE;
	}
	inline void SetPenColour(wxColour p_pcVal)
	{
		SetPen(wxThePenList->FindOrCreatePen(p_pcVal, GetPen()->GetWidth(), GetPen()->GetStyle()));
	}

/* changed by dscheibl 23.05.2005 13:38 */
    /**	\brief	The SetDesignType function. Interface for SP_GR_ExtendedDrawnShape

	    \param	p_nDesignType	value

	    \return	bool    TRUE in case of succes, FALSE otherwise
    */
    virtual bool SetDesignType(long p_nDesignType) { return FALSE; };
    virtual long GetDesignType() { return 0; };
/* changing end */

	virtual bool SetDesignArrowTypeRight(long m_nArrowTypeRight) { return FALSE; };
    virtual long GetDesignArrowTypeRight() { return 0; };

	virtual bool SetDesignArrowTypeLeft(long m_nArrowTypeLeft) { return FALSE; };
    virtual long GetDesignArrowTypeLeft() { return 0; };

	virtual bool SetThickness(size_t p_nThickness) { return FALSE; };
    virtual size_t GetThickness() { return 0; };
	virtual bool SetArrowRightThickness(size_t p_nThickness) { return FALSE; };
    virtual size_t GetArrowRightThickness() { return 0; };
	virtual bool SetArrowLeftThickness(size_t p_nThickness) { return FALSE; };
    virtual size_t GetArrowLeftThickness() { return 0; };

    /**	\brief	The GetGraphicChildren function. Accessor

	    \return	SP_ListGraphic*   pointer
    */
    SP_ListGraphic* GetGraphicChildren();

    /**	\brief	The AddGraphicChildren function

	    \param	p_pcChild	new child

	    \return	bool    TRUE in case of success, FALSE otherwise

	    Called from within AddChildToCanvas for all children. Fills the m_lGraphicChildren
        member.
    */
    bool AddGraphicChildren(SP_Graphic* p_pcChild);

    /**	\brief	The RemoveGraphicChildren function

	    \param	p_pcChild	new child

	    \return	bool    TRUE in case of success, FALSE otherwise

	    Removes the member from m_lGraphicChildren.
    */
    bool RemoveGraphicChildren(SP_Graphic* p_pcChild);

    /**	\brief	The AddToCanvas function

	    \param	p_pcCanvas	the canvas to add our primitive to
	    \param	p_nX    position
	    \param	p_nY    position
	    \param	p_nKeys key modifiers if any

	    \return	bool TRUE in case of succes, FALSE otherwise

	    This method is used from interactive drawing with the mouse (that's why
        there are the position and key parameters) and while loading or displaying
        subnets, too.
    */
    virtual bool AddToCanvas(SP_GUI_Canvas* p_pcCanvas, double p_nX = -1, double p_nY = -1, int p_nKeys = 0) { return TRUE; }

    virtual bool CheckIntegrity();

    /**	\brief	The AddToDeleteQueue function

	    \param	p_eFilter	flag, whether we are allowed to veto our deletion

	    \return	bool    TRUE in case of success, FALSE otherwise

	    If this graphic is selected by the user an doomed to deletion it has the chance
        to veto it's deletion as sure as to mark more connected objects (see SP_GR_Edge, SP_GR_Node).

        The concrete queue, we are added to, is helt in SP_Core::m_pmlDeleteDataGraphic and within
        SP_Core::RemoveQueuedElements all queued objects are deleted.
    */
    virtual bool RemoveFromCanvas();
	inline SP_GUI_Canvas* GetCanvas()
	{
		if (GetPrimitive())
		{
			return static_cast<SP_GUI_Canvas*>(GetPrimitive()->GetCanvas());
		}
		else
		{
			return NULL;
		}
	}

	virtual bool Flatten(unsigned int p_nNewNet, SP_ListGraphic* p_plGraphics = NULL) { if (GetParent()) return GetParent()->Flatten(p_nNewNet, p_plGraphics); return FALSE; }
    virtual bool OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet, SP_ListGraphic* p_plGraphics = NULL);
    virtual bool Coarse(unsigned int p_nNewNet, SP_Graphic* p_pcCoarseGr, SP_ListGraphic* p_plShapes);
    virtual bool OnCoarse(unsigned int p_nNewNet, SP_Graphic* p_pcCoarseGr, SP_ListGraphic* p_plShapes);

    virtual bool AddToDeleteQueue(SP_GRAPHIC_STATE p_eFilter = SP_STATE_ALL);
    /**	\brief	The AddAllToDeleteQueue function

	    \param	p_eFilter	flag, whether we are allowed to veto our deletion

	    \return	bool    TRUE in case of success, FALSE otherwise

	    Convinience function to add all graphics of our parent for deletion.
    */
    virtual bool AddAllToDeleteQueue(SP_GRAPHIC_STATE p_eFilter = SP_STATE_ALL);

    /**	\brief	The HasUndeletedEdges function. Interface

        \return	bool        the state

        reimplemented in SP_GR_Node.
    */
    virtual bool HasUndeletedEdges() { return TRUE; }

    /**	\brief	The CollectDialogAttributes function

	    \param	p_ptmAttributes	    pointer to map to add all the attributes of our parent to

	    \return	bool    TRUE in case of success, FALSE otherwise

	    If the user double clicks a graphical object in the GUI oder choses 'Edit properties'
        from the menu, the event originates from an object of SP_Graphic. That's why, we enter
        the process of collecting the attributes right here.
    */
    bool CollectDialogAttributes(SP_MapString2ListAttribute* p_ptmAttributes);

    /**	\brief	The SetNetnumber function

	    \param	p_nNewVal	a parameter of type unsigned int
	    \param	p_nOldVal   the old value to limit the renumbering

	    \return	virtual bool

	    See SP_NetElement::SetNetnumber for the meaning of the parameters.
        This method calls SetNetnumber for all of its children and the implementation
        of SP_NetElement for itself.
    */
    virtual bool SetNetnumber(unsigned int p_nNewVal, unsigned int p_nOldVal = 0);

    /**	\brief	The CloneBase function

	    \param	p_pcCopy	already cloned graphic

	    \return	bool    TRUE in case of success, FALSE otherwise

	    This is the place to assign some values, we store right here to the
        new object, before it is returned to the caller from it's special
        Clone method.
    */
    virtual bool CloneBase(SP_Graphic* p_pcCopy);
    /**	\brief	The Clone function. Interface

	    \param	p_pcParent	the new parent object

	    \return	SP_Graphic* the new graphical object

	    This method has to be implemented by all derivations to ensure, that
        every new graphic is able to create an exact clone (same class, same value)
        as the origin was.
        Within every implementation a call to SP_Graphic::CloneBase would make sense
        to give the new objects the chance to have some base properties assigned without
        implementing it in the overloaded Clone method.
    */
    virtual SP_Graphic* Clone(SP_Data* p_pcParent) = 0;
    /**	\brief	The Rotate function

	    \param	p_nX	y point of the center of the rotation
	    \param	p_nY	x point of the center of the rotation
	    \param	p_nAngle	the angle to rotate this object

	    \return	bool    TRUE in case of success, FALSE otherwise

	    Real calculation and (fake) rotation of this object around the given center.
        Reimplemented by SP_GR_Edge, beacause their line control points need to
        get rotated, too.

        The 'rotation' itself is realized just via a translation to the new center.
        No graphic is really rotated, as this will end up in some strange looking
        effects.
    */
    virtual bool Rotate(double p_nX, double p_nY, double p_nAngle);
    virtual bool Flip(double p_nX);
    virtual bool Mirror(double p_nY);

    virtual bool Merge(SP_Graphic* p_pcTarget);

    /**	\brief	The Translate function

	    \param	p_nX	amount of moving in X
	    \param	p_nY	amount of moving in Y

	    \return	bool    TRUE in case of success, FALSE otherwise

	    Used from within SP_Graphic::Rotate to get the object to the new
        position, just calculated and from within SP_Core::Paste to move
        objects in the active view by some amount to avoid overlying.

        Reimplemented in SP_GR_Edge::Translate to get the line control points moved, too.
    */
    virtual bool Translate(double p_nX, double p_nY);

    /**	\brief	The AddToCopyMaps function

	    \param	p_ppcGraph	a parameter of type SP_DS_Graph**
	    \param	p_pmGraphic2Graphic	a parameter of type SP_Graphic*>*
	    \param	p_pmData2Data	a parameter of type SP_Data*>*

	    \return	bool    TRUE in case of success, FALSE otherwise

	    Start copying myself to the given graph and build up my history in the given maps.
        This is used from within SP_Core::Copy as well as SP_Core::Paste just differentiate
        between the graphs we copy from and to.

        Reimplemented in SP_GR_Edge::AddToCopyMaps to check the source and target members.
    */
    virtual bool AddToCopyMaps(SP_DS_Graph** p_ppcGraph, SP_MapGraphic2Graphic* p_pmGraphic2Graphic, SP_MapData2Data* p_pmData2Data);
    /**	\brief	The SynchronizeChildren function

	    \param	p_pmGraphic2Graphic	a parameter of type SP_MapGraphic2Graphic*

	    \return	bool

	    Called after copying any graphical object to the new graph to restore the
        GraphicChildren() <-> m_pcGraphicParent association in the copied objects.
    */
    bool SynchronizeChildren(SP_MapGraphic2Graphic* p_pmGraphic2Graphic);
    /**	\brief	The ResetNetnumbers function

	    \param	p_pmNet2Net	mapping of netnumbers
	    \param	p_pcToGraph	the graph to be used for new netnumber

	    \return	bool TRUE in case of success, FALSE otherwise

	    Before we can take our copied elements in SP_Core::Paste, we need to ensure,
        that we doesn't mix any net number in our copy graph with one in the graph we
        actually paste into.
        Therefore, the p_pmNet2Net member is checked for an entry, matching our actual netnumber
        and, if not found, generating a new one by use of the SP_DS_Graph::GetNewNetnumber method
        of the second parameter.
    */
    virtual bool ResetNetnumbers(SP_MapUInt2UInt* p_pmNet2Net, SP_DS_Graph* p_pcToGraph);

    virtual bool ShowDebug(unsigned int = 3);

    virtual bool UpdateChildrenOffsets();
    /**
     * toggles the alpha of the graphic of any netelement
     * if it is hidden or not
     *
     * @param p_bHide
     */
    void ToggleHide(bool p_bHide);


	inline void SetTextColor(wxString p_sTextColor) { m_sTextColor = p_sTextColor; }
	inline wxString GetTextColor() { return m_sTextColor; }


};

#endif // __SP_GRAPHIC_H__
