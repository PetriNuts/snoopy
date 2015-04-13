//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_EDGE_H__
#define __SP_DS_EDGE_H__

#include "sp_ds/SP_DS_Attribute.h"

class SP_DS_Edgeclass;
class SP_GR_Edge;
class SP_GUI_Canvas;

/**	\brief	class representing a edge

*/
class SP_DS_Edge: public SP_Error, public SP_Data
{
private:
    SP_Data* m_pcSource;
    SP_Data* m_pcTarget;
protected:
    SP_DS_Edgeclass* m_pcEdgeclass;

    SP_ListAttribute m_lAttributes;

    /**	\brief	The CompleteCoarse function called on leave of SetNodes(SP_Graphic*, SP_Graphic*, SP_Graphic*)

	    \param	p_pcSource	the graphic of the source
	    \param	p_pcTarget	the graphic of the target
	    \param	p_pcSelf	the graphic we just connected

	    \return	the third parameter or NULL in case of an error

	    That's only to be used from within SetNodes(SP_Graphic*, SP_Graphic*, SP_Graphic*)
        to complete special preperations for coarse net connections.

        If one of the end nodes has the m_pcCoarse member set, the stored member
        m_pcSource or m_pcTarget will already point to the correct DS element (in subnet).
        Now we just need to build up a new graphic for the edge in the subnet, connected
        with the one of the corresponding coarse element and a newly to be created graphic
        for the other end.
    */
    SP_Graphic* CompleteCoarse(SP_Graphic* p_pcSource, SP_Graphic* p_pcTarget, SP_Graphic* p_pcSelf);

    SP_Graphic* CompleteCoarseBorder(SP_Graphic* p_pcSource, SP_Graphic* p_pcTarget, SP_Graphic* p_pcSelf);

    bool m_bCoarseCompleted;
public:
    SP_DS_Edge(SP_DS_Edgeclass* p_pcEdgeclass, unsigned int p_nNetnumber = 0);
    virtual ~SP_DS_Edge();

    SP_Graphic* NewGraphicInSubnet(unsigned int p_nNetnumber);

    SP_DS_Edgeclass* GetEdgeclass();
    SP_DS_Edgeclass* SetEdgeclass(SP_DS_Edgeclass* p_pcVal) { m_pcEdgeclass = p_pcVal; return m_pcEdgeclass; }
    bool ClassNameChanged(const wxString& p_pchName);

    virtual SP_DS_Attribute* AddAttribute(SP_DS_Attribute* p_pcAttr);
    virtual SP_DS_Attribute* GetAttribute(const wxString& p_pchName);
    virtual bool HasAttributeType(SP_ATTRIBUTE_TYPE p_eVal);
    virtual SP_DS_Attribute* GetFirstAttributeByType(SP_ATTRIBUTE_TYPE p_eVal);
    bool SqueezeIdAttribute();

    virtual const SP_ListAttribute* GetAttributes() { return &m_lAttributes; }
    virtual SP_DS_Attribute* RemoveAttribute(SP_DS_Attribute* p_pcVal);

    bool SetNetnumber(unsigned int p_nNewVal, unsigned int p_nOldVal = 0);
    bool ResetNetnumbers(SP_MapUInt2UInt* p_pmNet2Net, SP_DS_Graph* p_pcToGraph);

    SP_DELETE_STATE RemoveGraphic(unsigned int p_nNetnumber);

    /**	\brief	The MergeData function

		\param	p_pcElement	object of the same type as this

		\return	bool    TRUE in case of success, FALSE otherwise

		Reimplemented from SP_Data to take care of all attributes.
		Calls SP_Data::Merge on the end.
	*/
	bool MergeData(SP_Data* p_pcElement);
	/**	\brief	The Split function

		\param	p_pcGraphic	the graphical representation

		\return	SP_DS_Edge* the new Edge

		This function creates a new object of the same edgeclass as
		this, adds it to the list of elements of SP_DS_Edgeclass and
		asks all attributes to move their graphics over to the new
		edges attributes, too.

		It although removes the graphic from the list, associated
		with this object.

		All the Split functions apply to the evaluation of "logical"
		nodes, only.
	*/
	SP_DS_Edge* Split(SP_Graphic* p_pcGraphic);

	bool CanConnectNodes(SP_Data* p_pcSource, SP_Data* p_pcTarget);

    SP_DS_Edge* SetNodes(SP_Data* p_pcSource, SP_Data* p_pcTarget);
    //! source gr, target gr and maybe the gr of 'this' to add the graphic to
    SP_Graphic* SetNodes(SP_Graphic* p_pcSource, SP_Graphic* p_pcTarget, SP_Graphic* p_pcGraphic = NULL);

    inline void SetSource(SP_Data* p_pcVal) { m_pcSource = p_pcVal; }
    inline void SetTarget(SP_Data* p_pcVal) { m_pcTarget = p_pcVal; }

    inline SP_Data* GetSource() const { return m_pcSource; }
    inline SP_Data* GetTarget() const { return m_pcTarget; }

    SP_DS_Edge* Clone(bool p_bCloneGr = TRUE);

    SP_Data* AddToCopyMap(SP_DS_Graph** p_ppcGraph, SP_Graphic* p_pcGraphic, SP_MapGraphic2Graphic* p_pmGraphic2Graphic,SP_MapData2Data* p_pmData2Data);
    SP_Data* SynchronizeChildren(SP_DS_Edge* p_pcEdge, SP_Graphic* p_pcGraphic, SP_MapGraphic2Graphic* p_pmGraphic2Graphic);

    /**	\brief	traverses the list of attributes and calls all of them to add to the
                given graphical object.

	    \param	p_pcGraphic	the edge-graphic-parent

	    \return	TRUE in case of success, FALSE otherwise

        This function will always traverse all atributes, regardles whether one fails to add
        or not.
    */
    bool ShowAttributeGraphic(SP_GR_Edge* p_pcGraphic);

    bool ShowOnCanvas(SP_GUI_Canvas* p_pcCanvas, bool p_bLocalOnly = FALSE, double p_nX = -1, double p_nY = -1, int p_nKeys = 0);

    bool CheckIntegrity();

    bool GetGraphicsInNet(SP_ListGraphic* p_plGraphics, unsigned int p_nNet, const unsigned int p_eFilter = SP_ELEMENT_ALL);
    bool OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet, SP_ListGraphic* p_plGraphics = NULL);

    bool AddDialogAttributes(SP_MapString2ListAttribute* p_ptmAttributes);

    bool Remove();

    const wxString GetClassName();
    SP_ElementClass* GetClassObject();

    SP_DELETE_STATE RemoveGraphic(SP_ListGraphic* p_plGraphic, bool p_bDelete = TRUE);

    bool Update(bool p_bLocalOnly = FALSE);
};

#endif // __SP_DS_EDGE_H__
