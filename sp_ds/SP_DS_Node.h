//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_NODE_H__
#define __SP_DS_NODE_H__

#include "sp_ds/SP_DS_Attribute.h"

class SP_DS_Nodeclass;
class SP_GR_Node;
class SP_GUI_Canvas;

/**	\brief	class representing a node

*/
class SP_DS_Node: public SP_Error, public SP_Data
{
private:
protected:
    /**	\brief nodeclass, this node is part of
    */
    SP_DS_Nodeclass* m_pcNodeclass;

    /**	\brief list of attributes, this node shows
    */
    SP_ListAttribute m_lAttributes;
    /**	\brief list of edges, which start at this
    */
    SP_ListEdge m_lSourceEdges;
    /**	\brief list of edges, which end up on this
    */
    SP_ListEdge m_lTargetEdges;
    /**	\brief	The SplitEdges function

	    \param	p_pcGraphic	the graphic, already removed from the list of this
	    \param	p_pcNode	the newly created node

	    \return	bool    TRUE in case of succes, FALSE othwerwise

	    Called from within SP_DS_Node::Split(SP_Graphic*) with the
        concrete graphic, that is added to the new node and the new
        node itself.
        Just calls SP_GR_Node::SplitEdges with the new node as
        parameter.
    */
    bool SplitEdges(SP_Graphic* p_pcGraphic, SP_DS_Node* p_pcNode);
    /**
        Called from within SP_DS_Node::Split(SP_Graphic*) with the
        concrete graphic, returns the list of coupled graphics in case of coarse border node.
     */
    void GetCoupledGraphics(SP_ListGraphic& p_plGraphics, SP_Graphic* p_pcGraphic);

public:
    /**	\brief	Constructor

	    \param	p_pcNodeclass	the nodeclass, this node is part of
	    \param	p_nNetnumber    the netnumber, this node should be part of
    */
    SP_DS_Node(SP_DS_Nodeclass* p_pcNodeclass, unsigned int p_nNetnumber = 0);
    virtual ~SP_DS_Node();

    bool Equals(SP_Data* p_pcVal);

    SP_Graphic* NewGraphicInSubnet(unsigned int p_nNetnumber);

    /**	\brief	Accessor to the nodeclass parent

	    \return	the m_pcNodeclass member
    */
    SP_DS_Nodeclass* GetNodeclass();
    SP_DS_Nodeclass* SetNodeclass(SP_DS_Nodeclass* p_pcVal);
    bool ClassNameChanged(const wxString& p_pchName);

    const SP_ListEdge* GetSourceEdges() { return &m_lSourceEdges; }
    const SP_ListEdge* GetTargetEdges() { return &m_lTargetEdges; }

    /**	\brief	Adds a new attribute to the node

	    \param	p_pcAttr	the new attribute

	    \return	the attribute, given in the param
    */
    virtual SP_DS_Attribute* AddAttribute(SP_DS_Attribute* p_pcAttr);
    /**	\brief	Accessor to one of the attributes, given by name

	    \param	p_pchName	name of the attribute

	    \return	the first attribute in list, known by the name, or NULL
    */
    virtual SP_DS_Attribute* GetAttribute(const wxString& p_pchName);
    virtual const SP_ListAttribute* GetAttributes() { return &m_lAttributes; }

    virtual SP_DS_Attribute* RemoveAttribute(SP_DS_Attribute* p_pcVal);

    SP_Data* GetSibling(const wxString& p_pchRefAttribute);

    bool SetNetnumber(unsigned int p_nNewVal, unsigned int p_nOldVal = 0);

    SP_DELETE_STATE RemoveGraphic(unsigned int p_nNetnumber);

    /**	\brief	The MergeData function

	    \param	p_pcElement	object of the same type as this

	    \return	bool    TRUE in case of success, FALSE otherwise

        Reimplemented from SP_Data to take care of all attributes.
        Calls SP_Data::Merge on the end.
    */
    bool MergeData(SP_Data* p_pcElement);

    /**	\brief	The Split function

	    \param	p_plGraphics	list of graphical objects

	    \return	bool    TRUE in case of success, FALSE otherwise

	    This function checks the parameter for elements, that
        are associated to this object and tries to create a new
        node for every one, that is found, by calling Split with
        the single graphic.
    */
    bool Split(SP_ListGraphic* p_plGraphics);
    /**	\brief	The Split function

	    \param	p_pcGraphic	the graphical representation

	    \return	SP_DS_Node* the new Node

	    This function creates a new object of the same nodeclass as
        this, adds it to the list of elements of SP_DS_Nodeclass and
        asks all attributes to move their graphics over to the new
        nodes attributes, too.

        It although removes the graphic from the list, associated
        with this object and calls the protected function SplitEdges
        afterwards.

        All the Split functions apply to the evaluation of "logical"
        nodes, only. So, the newly created node is automatically set
        to not logical, as it wouldn't have been created this way,
        otherwise!
    */
    SP_DS_Node* Split(SP_Graphic* p_pcGraphic);

    /**	\brief	The HasEqualAttribute function

	    \param	p_pcAttribute	the attribute to be checked in this

	    \return	bool TRUE if the attributes are equal, FALSE otherwise

	    The test takes the first attribute with the same name as the parameter
        and calls wxStrcmp on the value of GetValueString of both.
    */
    bool HasEqualAttribute(SP_DS_Attribute* p_pcAttribute);
    virtual bool HasAttributeType(SP_ATTRIBUTE_TYPE p_eVal);
    virtual SP_DS_Attribute* GetFirstAttributeByType(SP_ATTRIBUTE_TYPE p_eVal);
    bool SqueezeIdAttribute();

    /**	\brief	Add an edge to the list of edges, starting at this

	    \param	p_pcEdge	the edge to be added

	    \return	this with the new edge added to the list

        The return value is mostly this object. It just differs for
        nodes with SP_DS_Coarse member set.
    */
    SP_DS_Node* AddSourceEdge(SP_DS_Edge* p_pcEdge);
    /**	\brief	Add an edge to the list of edges, ending up at this

	    \param	p_pcEdge	the edge to be added

	    \return	this with the new edge added to the list

        The return value is mostly this object. It just differs for
        nodes with SP_DS_Coarse member set.
    */
    SP_DS_Node* AddTargetEdge(SP_DS_Edge* p_pcEdge);

    /**	\brief	Removing an edge from the list of edges, starting at this

	    \param	p_pcEdge	the edge to be removed (check for pointer equality)

	    \return	bool TRUE in case of success, FALSE otherwise
    */
    bool RemoveSourceEdge(SP_DS_Edge* p_pcEdge);
    /**	\brief	Removing an edge from the list of edges, ending up at this

	    \param	p_pcEdge	the edge to be removed (check for pointer equality)

	    \return	bool TRUE in case of success, FALSE otherwise
    */
    bool RemoveTargetEdge(SP_DS_Edge* p_pcEdge);

    /**	\brief	The SetNewSource function

	    \param	p_pcNode	the node to be the new source for all edges

	    \return	bool    TRUE in case of succes, FALSE otherwise

	    Iterates over all edges of this node, that start here, remove them
        from the list of sources and adds them to the parameter.
    */
    bool SetNewSource(SP_DS_Node* p_pcNode);
    /**	\brief	The SetNewTarget function

	    \param	p_pcNode	the node to be the new source for all edges

	    \return	bool    TRUE in case of succes, FALSE otherwise

	    Iterates over all edges of this node, that end here, remove them
        from the list of targets and adds them to the parameter.
    */
    bool SetNewTarget(SP_DS_Node* p_pcNode);
    /**
     * Checks whether this is Source of @param or not
     */
    bool IsSourceOf(SP_DS_Node* p_pcNode);
    /**
     * Checks whether this is Target of @param or not
     */
    bool IsTargetOf(SP_DS_Node* p_pcNode);

    /**	\brief	Create an exact copy of this

        used, if creating a new element from the definition of the prototype in the class

        \param  p_bCloneGr  flag to clone the graphic

        \return	the new node
    */
    SP_DS_Node* Clone(bool p_bCloneGr = TRUE);

    /**	\brief	AddToCopyMap function

        \param	p_ppcGraph	a parameter of type SP_DS_Graph**
	    \param	p_pmGraphic2Graphic	map of objects from copied one to copies
	    \param	p_pmData2Data	map of objects from copied one to copies
	    \param	p_pcGraphic	    the graphic, we are the parent of

	    \return	SP_Data* the copied object or NULL in case of an error

	    This method is called from within SP_Graphic::AddToCopyMap with
        ourself as caller (GetParent() of the graphic).
        We need to check the p_pcGraphic parameter for the list of children
        and add clones for everyone of them.
    */
    SP_Data* AddToCopyMap(SP_DS_Graph** p_ppcGraph, SP_Graphic* p_pcGraphic, SP_MapGraphic2Graphic* p_pmGraphic2Graphic, SP_MapData2Data* p_pmData2Data);
    SP_Data* SynchronizeChildren(SP_DS_Node* p_pcNode, SP_Graphic* p_pcGraphic, SP_MapGraphic2Graphic* p_pmGraphic2Graphic);
    bool ResetNetnumbers(SP_MapUInt2UInt* p_pmNet2Net, SP_DS_Graph* p_pcToGraph);
    bool SynchronizeAttributes();

    /**	\brief	traverses the list of attributes and calls all of them to add to the
                given graphical object.

	    \param	p_pcGraphic	the node-graphic-parent

	    \return	TRUE in case of success, FALSE otherwise

        This function will always traverse all atributes, regardles whether one fails to add
        or not.
    */
    bool ShowAttributeGraphic(SP_GR_Node* p_pcGraphic);

    /**	\brief	The ShowOnCanvas function

	    \param	p_pcCanvas	the canvas to add the graphics to
        \param  p_bLocalOnly    flag whethe we should update all of our children
	    \param	p_nX	the x position
	    \param	p_nY	the y position
	    \param	p_nKeys modifiers, pressed while clicking on the canvas

	    \return	bool    TRUE in case of succes, FALSE otherwise

	    Adds all graphical representations of this node to the canvas, i.e.
        calls ShowOnCanvas for all graphics. these will deside itself, whether
        they add them or not.
    */
    bool ShowOnCanvas(SP_GUI_Canvas* p_pcCanvas, bool p_bLocalOnly = FALSE, double p_nX = -1, double p_nY = -1, int p_nKeys = 0);

    bool CheckIntegrity();

    bool GetGraphicsInNet(SP_ListGraphic* p_plGraphics, unsigned int p_nNet, const unsigned int p_eFilter = SP_ELEMENT_ALL);
    bool OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet, SP_ListGraphic* p_plGraphics = NULL);

    bool AddDialogAttributes(SP_MapString2ListAttribute* p_ptmAttributes);

    bool Remove();

    const wxString GetClassName();
    SP_ElementClass* GetClassObject();

    bool Update(bool p_bLocalOnly = FALSE);
};


#endif // __SP_DS_NODE_H__
