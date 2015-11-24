//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DATA_H__
#define __SP_DATA_H__

#include "sp_utilities.h"
#include "sp_core/base/SP_Type.h"
#include "sp_core/base/SP_NetElement.h"

class SP_Data;
//! include not possible
class SP_DS_Attribute;
enum class SP_ATTRIBUTE_TYPE;
class SP_Graphic;
class SP_DS_Edge;
class SP_DS_Node;
class SP_DS_Metadata;
class SP_DS_Coarse;
class SP_DS_Graph;
class SP_ElementClass;
class SP_DS_Animator;

typedef list<SP_Data*> SP_ListData;
typedef list<SP_Graphic*> SP_ListGraphic;
typedef list<SP_DS_Attribute*> SP_ListAttribute;
typedef list<SP_DS_Edge*> SP_ListEdge;
typedef list<SP_DS_Node*> SP_ListNode;
typedef list<SP_DS_Metadata*> SP_ListMetadata;
typedef list<SP_DS_Animator*> SP_ListAnimator;

typedef SP_ListData::iterator SP_DataIter;
typedef SP_ListData::const_iterator SP_DataCIter;
typedef SP_ListGraphic::iterator SP_GraphicIter;
typedef SP_ListGraphic::const_iterator SP_GraphicCIter;
typedef SP_ListAttribute::iterator SP_AttributeIter;
typedef SP_ListAttribute::const_iterator SP_AttributeCIter;
typedef SP_ListEdge::iterator SP_EdgeIter;
typedef SP_ListEdge::const_iterator SP_EdgeCIter;
typedef SP_ListNode::iterator SP_NodeIter;
typedef SP_ListNode::const_iterator SP_NodeCIter;
typedef SP_ListMetadata::iterator SP_MetadataIter;
typedef SP_ListMetadata::const_iterator SP_MetadataCIter;
typedef SP_ListAnimator::iterator SP_AnimatorIter;
typedef SP_ListAnimator::const_iterator SP_AnimatorCIter;

typedef map<SP_Data*, SP_Data*> SP_MapData2Data;
typedef map<SP_Graphic*, SP_Graphic*> SP_MapGraphic2Graphic;
typedef map<wxString, SP_DS_Node*> SP_MapString2Node;

enum SP_DELETE_STATE
{
    SP_NO_MORE_OBJECTS,
    SP_MORE_OBJECTS,
    SP_DELETE_ERROR

};

enum SP_IA_STATE {
	SP_IA_NORMAL,
	SP_IA_SELECTED,
	SP_IA_MAP_SELECTED
};

/**	\brief	Redefinition of map<wxString, SP_ListAttribute > as SP_MapString2ListAttribute;

  This type is used in the collection of attributes to be shown in the dialoges
  and therefore in SP_GR_* SP_DS_*, too.
*/
typedef map<wxString, SP_ListAttribute > SP_MapString2ListAttribute;

/**	\brief	Class for all datastructure elements

  All parts of the DS should be derived from this
*/
class SP_Data: public SP_NetElement, public SP_Type, public SP_IdCounter
{
private:

	SP_Data():SP_Type(SP_ELEMENT_NULL),
    m_bLogical(false),
    m_pcCoarse(NULL),
    m_eIAState(SP_IA_NORMAL)
    { }

protected:

    //list of graphics
	SP_ListGraphic m_lGraphics;
    //! this is made of multiple element graphics
    bool m_bLogical;
    /**	\brief extension for subnets, this data can display
    */
    SP_DS_Coarse* m_pcCoarse;

	//added by MD
	SP_IA_STATE m_eIAState;

public:

	SP_Data(SP_ELEMENT_TYPE p_eType);
    virtual ~SP_Data();

    virtual bool Equals(SP_Data* p_pcVal);

    /**	\brief	Adds a graphical representation to the list

	    \param	p_pcGraphic     The graphic object

	    \return	the same pointer as the parameter

    */
    virtual SP_Graphic* AddGraphic(SP_Graphic* p_pcGraphic);

    /**	\brief	The AddGraphicInSubnet function

	    \param	p_nNetnumber	net number we create or require the graphic from

	    \return	SP_Graphic  the new graphical object

	    This method searches the list of graphical representations, associated with this data,
        for any object in the subnet given by the parameter. If found, it returns this
        object without any modification.
        Otherwise it creates a new graphic in the subnet, adds it to the list of graphical
        representations and returns this new one.
    */
    virtual SP_Graphic* AddGraphicInSubnet(unsigned int p_nNetnumber);
    /**	\brief	The NewGraphicInSubnet function

	    \param	p_nNetnumber	net number we create or require the graphic from

	    \return	SP_Graphic  the new graphical object

     *  It creates a new graphic in the subnet, adds it to the list of graphical
        representations and returns this new one.
     */
    virtual SP_Graphic* NewGraphicInSubnet(unsigned int p_nNetnumber);
    virtual SP_Graphic* GetGraphicInSubnet(unsigned int p_nNetnumber);
    virtual bool OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet, SP_ListGraphic* p_plGraphics = NULL);

    /**	\brief	Removes a graphical representation from the list

	    \param	p_plGraphic the list of graphic
        \param  p_bDelete   call delete on the objects

	    \return	SP_DELETE_STATE whether there are more objects or not
    */
    virtual SP_DELETE_STATE RemoveGraphic(SP_ListGraphic* p_plGraphic, bool p_bDelete = TRUE);

    /**	\brief	The RemoveGraphic function

	    \param	p_nNetnumber	number of the net that should be cleaned

	    \return	SP_DELETE_STATE

	    Cleans up all graphics in the designated sub net. Clears up all
        data, if they haven't any more graphics in other subnets, too.
    */
    virtual SP_DELETE_STATE RemoveGraphic(unsigned int p_nNetnumber);

    /**	\brief	The RemoveGraphic function

	    \param	p_pcGraphic	    the graphical object to be removed from the list
	    \param	p_bDelete       TRUE if 'delete' should be called on the parameter

	    \return	bool            TRUE in case of success, FALSE otherwise

	    Tests the list of graphics of the existence of the parameter, removes
        it, if found and calls delete, if called with p_bDelete == TRUE
    */
    virtual bool RemoveGraphic(SP_Graphic* p_pcGraphic, bool p_bDelete = TRUE);
    /**	\brief	The RemoveAllGraphics function

                Removes all graphical associations to this

	    \return	bool    TRUE in case of success, FALSE otherwise
    */
    virtual bool RemoveAllGraphics(bool p_bDelete = FALSE);
    /**	\brief	Returns the list of graphics

	    \return	The list of graphical representations

    */
    virtual SP_ListGraphic* GetGraphics();

    virtual bool GetGraphicsInNet(SP_ListGraphic* p_plGraphics, unsigned int p_nNet, const unsigned int p_eFilter = SP_ELEMENT_ALL);

    /**	\brief	The SetNetnumber function

	    \param	p_nNewVal	the new netnumber
	    \param	p_nOldVal	the old netnumber, only objects matching this should get the new one

	    \return	bool TRUE in case of success, FALSE otherwise

	    This is reimplemented from SP_NetElement::SetNetnumber but calls this method for itself
        if it succesfully changed the numbers for all it's graphical children.
        If p_nOldVal is != 0 only objetcs, that have this number set atcually, will be assigned
        the new netnumber.
    */
    bool SetNetnumber(unsigned int p_nNewVal, unsigned int p_nOldVal = 0);

    /**	\brief	The AddDialogAttributes function

	    \param	p_ptmAttributes	Map of attribute names to attributes

	    \return	bool    TRUE in case of success, FALSE otherwise

	    This function is called from SP_Graphic::EditProperties and adds
        all attributes to the map in order to get them shown in the
        properties-dialog
    */
    virtual bool AddDialogAttributes(SP_MapString2ListAttribute* p_ptmAttributes) { return TRUE; }
    /**	\brief	The Remove function

	    \return	bool    TRUE in case of success, FALSE otherwise

	    This function must be implemented by all derived classes. For SP_DS_Node
        it disconnects the object from it's edges and removes the element from
        the list in SP_DS_Nodeclass.

        For SP_DS_Edge, it informs their source and target of not beeing
        available any more and removes itself from the list in SP_DS_Edgeclass.
        For SP_DS_Attribute, it just removes itself from the list of attributes
        in the parent object.
    */
    virtual bool Remove() = 0;
    virtual bool Flatten(unsigned int p_nNewNet, SP_ListGraphic* p_plGraphics = NULL);
    virtual bool Coarse(unsigned int p_nNewNet, SP_Graphic* p_pcGraphic, SP_Graphic* p_pcCoarseGr, SP_ListGraphic* p_plShapes);

    /**	\brief	The GetClassName function

	    \return	const wxString     The name

	    This is a convinience function returning the class name for nodes and edges and
        the attribute names for attributes.
    */
    virtual const wxString GetClassName() = 0;
    virtual SP_ElementClass* GetClassObject() { return NULL; }

    /**	\brief	The AddToCopyMap function

	    \param	p_ppcGraph	the graph to copy ourself into
	    \param	p_pcGraphic	    graphic, this object is called by to be added to the map
	    \param	p_pmGraphic2Graphic	the mapping between old objects in the source and new ones
	    \param	p_pmData2Data	the mapping between old objects in the source and new ones

	    \return	SP_Data*    the copied object if any

        This method is called from within SP_Graphic::AddToCopyMaps and the p_ppcGraph and
        the map parameters with all nodes, that need to be copied, but doesn't do anything
        here.
        It is reimplemented in SP_DS_Node::AddToCopyMap, SP_DS_Edge::AddToCopyMap and
        SP_DS_Attribute::AddToCopyMap to take care of the associated attributes
    */
    virtual SP_Data* AddToCopyMap(SP_DS_Graph** p_ppcGraph, SP_Graphic* p_pcGraphic, SP_MapGraphic2Graphic* p_pmGraphic2Graphic, SP_MapData2Data* p_pmData2Data) { return NULL; }
    /**	\brief	The AddToCopyMap function

	    \param	p_ppcGraph	the graph to copy ourself into
	    \param	p_nNetnumber	the net number we copy from
	    \param	p_pmGraphic2Graphic	the mapping between old objects in the source and new ones
	    \param	p_pmData2Data	the mapping between old objects in the source and new ones

	    \return	bool TRUE in case of success, FALSE otherwise

	    This method does the same as AddToCopyMap but takes only children and attributes
        if they match the given netnumber. This is used from within data objects that have
        the m_pcCoarse member set and therefore will copy only elements, that belong to them.

        Called from within SP_DS_Nodeclass::AddToCopyMap and SP_DS_Edgeclass::AddToCopyMap via
        SP_DS_Graph::AddToCopyMap (called for every coarse member, that needs to get copeid)
        and reimplemented in SP_DS_Node::AddToCopyMap and SP_DS_Edge::AddToCopyMap
    */
    virtual bool AddToCopyMap(SP_DS_Graph** p_ppcGraph, unsigned int p_nNetnumber, SP_MapGraphic2Graphic* p_pmGraphic2Graphic, SP_MapData2Data* p_pmData2Data);
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
    /**	\brief	The SynchronizeAttributes function

        \return	bool TRUE in case of success, FALSE otherwise

         This method is empty right here, as SP_Data objects doesn't know about attributes.
         But for SP_DS_Node and SP_DS_Edge objects, we call this method via this interface
         to check for special attributes that need to be taken care of after a paste operation.
         That is, for now, only the setting of m_bLogical
    */
    virtual bool SynchronizeAttributes() { return FALSE; }

    /**	\brief	The GetSibling function

	    \param	p_pchRefAttribute	name of the attribute to be tested

	    \return	SP_Data*    the object if any, or NULL

	    This function searches the list of elements of their own class
        for being equal in the value of the named attribute.
        It returns the SP_Data derived object, that isn't the same as this
        and as the same value in the named attribute (tested with GetValueString)
    */
    virtual SP_Data* GetSibling(const wxString& p_pchRefAttribute) { return NULL; }
    /**	\brief	The MergeData function

	    \param	p_pcElement	object of the same type as this

	    \return	bool    TRUE in case of success, FALSE otherwise

	    This function takes all graphical representations of the parameter and
        adds them to their own list, if the parameter is of the same
        SP_ELEMENT_TYPE.

        If there isn't any graphical representation or all graphics were
        successfully moved over, it calls RemoveAllGraphics on the parameter.
        This function only returns FALSE, if this object and the parameter
        are of different SP_ELEMENT_TYPE.
    */
    virtual bool MergeData(SP_Data* p_pcElement);

    /**	\brief	The AddSourceEdge function

	    \param	p_pcEdge	object to be added to the list of edges, starting on this

	    \return	SP_DS_Node* the node, if successfully added, NULL otherwise

	    This function doesn't do anything in here, but is reimplemented
        in SP_DS_Node to add edges.
    */
    virtual SP_DS_Node* AddSourceEdge(SP_DS_Edge* p_pcEdge) { return NULL; }
    /**	\brief	The AddTargetEdge function

	    \param	p_pcEdge	object to be added to the list of edges, ending on this

	    \return	SP_DS_Node* the node, we connected too or NULL in case of an error

	    This function doesn't do anything in here, but is reimplemented
        in SP_DS_Node to add edges.
    */
    virtual SP_DS_Node* AddTargetEdge(SP_DS_Edge* p_pcEdge) { return NULL; }

    /**	\brief	The RemoveSourceEdge function

	    \param	p_pcEdge	edge to be removed from the list

	    \return	bool    TRUE in case of success, FALSE otherwise

	    This function always returns TRUE, but is reimplemented in SP_DS_Node.
    */
    virtual bool RemoveSourceEdge(SP_DS_Edge* p_pcEdge) { return TRUE; }
    /**	\brief	The RemoveTargetEdge function

	    \param	p_pcEdge	edge to be removed from the list

	    \return	bool    TRUE in case of success, FALSE otherwise

	    This function always returns TRUE, but is reimplemented in SP_DS_Node.
    */
    virtual bool RemoveTargetEdge(SP_DS_Edge* p_pcEdge) { return TRUE; }

    /**	\brief	The SetShow function

	    \param	p_bVal	TRUE if all graphics should be shown, FALSE otherwise

	    \return	bool    TRUE in case of success, FALSE otherwise

	    SetShow(TRUE) means, that all graphical representations of this
        object should be drawn as implemented. FALSE means, don't show them.
        The value of the flag itself isn't stored in this class, as visibility
        only applies to graphical objects. \sa SP_Graphic::SetShow
    */
    virtual bool SetShow(bool p_bVal);
    /**	\brief	The GetShow function

	    \return	bool    TRUE if all graphics should be shown, FALSE otherwise

	    This function logical connects all values of GetShow called on their
        graphics with AND and returns the value.
        It even returns TRUE, if there are no graphics for this object.
    */
    virtual bool GetShow();

    /**	\brief	The SetLogical function

	    \param	p_bVal	TRUE if this object should be logical, FALSE otherwise

	    \return	bool TRUE in case of success, FALSE otherwise

	    This function informs this object of beeing 'logical', meaning it
        shows more than one graphical representation on the screen.
    */
    inline bool SetLogical(bool p_bVal) { m_bLogical = p_bVal; return TRUE; }
    /**	\brief	The GetLogical function

	    \return	bool TRUE if this object is set to 'logical', FALSE otherwise

	    This functions returns the state of the member variable m_bLogical.
        Used to reflect this special setting in all graphical objects.
    */
    inline bool GetLogical() const { return m_bLogical; }

    /**	\brief	The SetCoarse function. Simple accessor.

	    \param	p_pcVal	the coarse member

	    \return	SP_DS_Coarse* the parameter

	    Sets the m_pcCoarse member of this class
    */
    SP_DS_Coarse* SetCoarse(SP_DS_Coarse* p_pcVal);
    /**	\brief	The GetCoarse function. Simple accessor

	    \return	SP_DS_Coarse* the m_pcCoarse member of this class
    */
    inline SP_DS_Coarse* GetCoarse() const 
	{ 
		return m_pcCoarse; 
	}

    /**	\brief	The OnDoubleClick function

	    \param	p_pcTarget	the object that initiated the double click event

	    \return	bool TRUE in case of success, FALSE otherwise

	    Called from within the GUI when a double click occured.
    */
    virtual bool OnDoubleClick(SP_Graphic* p_pcTarget, int p_nKeys);
    /**	\brief	The HasUndeletedGraphics function

	    \return	bool TRUE if there are undeleted children of FALSE otherwise

	    Helperfunction that searches the list of graphical objects, associated with
        objects of this class for any member, that doesn't have the SP_NetElement::m_bDelete
        member set to TRUE.
        This happens, if any of the graphics of this class are selected by the user for
        deletion, but while executing this delete, we have to ensure, that i.e. edges
        are deleted, even if they doesn't got selected, but source or target node dissappear.
        The other way round, every node in the SP_STATE_COARSEBORDERDOWN state will
        disappear if it doesn't hold any more lines, connected to them, too.
    */
    bool HasUndeletedGraphics();

    /**	\brief	The Update function

	    \param	p_bLocalOnly flag, passed to the m_pcCoarse members Update function, if any

	    \return	bool TRUE in case of success, FALSE otherwise

	    Calls Update for all graphical children and the m_pcCoarse member, too, if set.
    */
    virtual bool Update(bool p_bLocalOnly = FALSE);
    virtual bool ResetCoarseTree();

    virtual bool ShowDebug();

	//added by MD
    virtual bool HasAttributeType(SP_ATTRIBUTE_TYPE p_eVal) { return false; }
    virtual SP_DS_Attribute* GetFirstAttributeByType(SP_ATTRIBUTE_TYPE p_eVal) { return 0; }
    virtual SP_DS_Attribute* GetAttribute(const wxString& p_pchName) { return 0; }
    virtual const SP_ListAttribute* GetAttributes() { return 0; }
    virtual SP_DS_Attribute* AddAttribute(SP_DS_Attribute *p_pcAttrib) { return 0; }
    virtual SP_DS_Attribute* RemoveAttribute(SP_DS_Attribute *p_pcAttrib) { return 0; }
    void SetIAState(SP_IA_STATE p_eState) { m_eIAState = p_eState; }
    SP_IA_STATE GetIAState() const { return m_eIAState; }
};

#endif // __SP_DATA_H__
