//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/03/27 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_METADATA_H__
#define __SP_DS_METADATA_H__

#include "sp_ds/SP_DS_Attribute.h"

class SP_DS_Metadataclass;
class SP_GR_Metadata;
class SP_GUI_Canvas;

/**	\brief	class representing a node

*/
class SP_DS_Metadata: public SP_Error, public SP_Data
{
private:
protected:
    /**	\brief nodeclass, this node is part of
    */
    SP_DS_Metadataclass* m_pcMetadataclass;

    /**	\brief list of attributes, this node shows
    */
    SP_ListAttribute m_lAttributes;
    /**	\brief list of edges, which start at this
    */
public:
    /**	\brief	Constructor

	    \param	p_pcMetadataclass	the nodeclass, this node is part of
	    \param	p_nNetnumber    the netnumber, this node should be part of
    */
    SP_DS_Metadata(SP_DS_Metadataclass* p_pcMetadataclass, unsigned int p_nNetnumber = 0);
    virtual ~SP_DS_Metadata();

    bool Equals(SP_Data* p_pcVal);

    SP_Graphic* AddGraphicInSubnet(unsigned int p_nNetnumber);
    /**	\brief	Accessor to the nodeclass parent

	    \return	the m_pcMetadataclass member
    */
    SP_DS_Metadataclass* GetMetadataclass();
    SP_DS_Metadataclass* SetMetadataclass(SP_DS_Metadataclass* p_pcVal);
    bool ClassNameChanged(const wxString& p_pchName);

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

	    \return	SP_DS_Metadata* the new Metadata

	    This function creates a new object of the same nodeclass as
        this, adds it to the list of elements of SP_DS_Metadataclass and
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
    SP_DS_Metadata* Split(SP_Graphic* p_pcGraphic);

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

    /**	\brief	Create an exact copy of this

        used, if creating a new element from the definition of the prototype in the class

        \param  p_bCloneGr  flag to clone the graphic

        \return	the new node
    */
    SP_DS_Metadata* Clone(bool p_bCloneGr = TRUE);

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
    SP_Data* SynchronizeChildren(SP_DS_Metadata* p_pcMetadata, SP_Graphic* p_pcGraphic, SP_MapGraphic2Graphic* p_pmGraphic2Graphic);
    bool ResetNetnumbers(SP_MapUInt2UInt* p_pmNet2Net, SP_DS_Graph* p_pcToGraph);
    bool SynchronizeAttributes();

    /**	\brief	traverses the list of attributes and calls all of them to add to the
                given graphical object.

	    \param	p_pcGraphic	the node-graphic-parent

	    \return	TRUE in case of success, FALSE otherwise

        This function will always traverse all atributes, regardles whether one fails to add
        or not.
    */
    bool ShowAttributeGraphic(SP_GR_Metadata* p_pcGraphic);

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


#endif // __SP_DS_METADATA_H__
