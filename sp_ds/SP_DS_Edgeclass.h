//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_EDGECLASS_H__
#define __SP_DS_EDGECLASS_H__

#include "sp_core/base/SP_ElementClass.h"
#include "sp_ds/SP_DS_Edge.h"

class SP_DLG_GraphProperties;

typedef list<SP_DS_Edgeclass*> SP_ListEdgeclass;

/**	\brief	class representing a set of edges

  Each graph is built up by different classes of nodes and edges. Each class (i.e. set) defines
  unique members of the whole set, sharing the same definition (attributes, graphic, behaviour)
*/
class SP_DS_Edgeclass: public SP_ElementClass
{
private:
    SP_Graphic* m_pcStartShape;
    SP_Graphic* m_pcEndShape;
protected:
    SP_DS_Edge* m_pcPrototype;

    SP_ListEdge m_lElements;

    bool SynchronizeShowFlags(SP_DS_Attribute* p_pcAttr);
public:
    SP_DS_Edgeclass(SP_DS_Graph* p_pcGraph, const wxString& p_pchName, LongPtr p_nIdCounter = LongPtr(NULL));
    ~SP_DS_Edgeclass();

    SP_DS_Edge* GetPrototype();

    /**	\brief	Create a new element of this edge class

	    \param	p_nNetnumber	the netnumber, this edge should be created in
	    \param	p_pcSource      the source graphic
	    \param	p_pcTarget      the target graphic

	    \return	SP_DS_Edge*     the new edge, or NULL if the creation is vetoed

	    Creates a new edge of this class (by cloning the prototype) without
        checking for validity (see SP_DS_Netclass::EdgeRequirement).
        If p_pcSource and p_pcTarget are given, calls SetNodes on the new
        edge.
    */
    SP_DS_Edge* NewElement(unsigned int p_nNetnumber,
        SP_Graphic* p_pcSource = NULL, SP_Graphic* p_pcTarget = NULL);

    bool AddElement(SP_DS_Edge* p_pcVal);
    bool RemoveElement(SP_DS_Edge* p_pcVal);
    bool RemoveElements(unsigned int p_nNetnumber, SP_ListData* p_plCollect);
    inline void ResetTempShapes() { m_pcStartShape = NULL; m_pcEndShape = NULL; }

    inline SP_Graphic* GetStartShape() { return m_pcStartShape; }
    inline SP_Graphic* GetEndShape() { return m_pcEndShape; }

    /**
     * @return pointer to list of elements
     */
    inline const SP_ListEdge* GetElements() { return &m_lElements; }

    // convenience functions to prepare the prototype
    SP_DS_Attribute* AddAttribute(SP_DS_Attribute* p_pcAttr);
    bool HasAttributeType(SP_ATTRIBUTE_TYPE p_eVal);

    /**	\brief	Define a graphical representation for this class

	    \param	p_pcGraphic	the graphic object

	    \return	the graphic, given in the param
    */
    SP_Graphic* SetGraphic(SP_Graphic* p_pcGraphic);

    bool OnLeftClickOnCanvas(SP_GUI_Canvas* p_pcCanvas, double p_nX, double p_nY, int p_nKeys);
    bool OnStartElement(SP_Graphic* p_pcShape, double p_nX, double p_nY, int p_nKeys, int p_nAttach);
    bool OnDrawElement(double p_nX, double p_nY, int p_nKeys, int p_nAttach);
    SP_DS_Edge* OnEndElement(SP_Graphic* p_pcShape, double p_nX, double p_nY, int p_nKeys, int p_nAttach);

    bool SetNetnumbers(unsigned int p_nNewVal, unsigned int p_nOldVal);
    bool ResetNetnumbers(SP_MapUInt2UInt* p_pmNet2Net, SP_DS_Graph* p_pcToGraph);

    bool ShowOnCanvas(SP_GUI_Canvas* p_pcCanvas, bool p_bLocalOnly = FALSE);

    bool CheckIntegrity();

    bool GetGraphicsInNet(SP_ListGraphic* p_plGraphics, unsigned int p_nNet, const unsigned int p_eFilter = SP_ELEMENT_ALL);
    bool OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet, SP_ListGraphic* p_plGraphics = NULL);

    /**	\brief	The AddToCopyMap function

        \param  p_ppcGraph  reference to the graph we are copied to
	    \param	p_nNetnumber	a parameter of type unsigned int
	    \param	p_pmGraphic2Graphic	a parameter of type SP_Graphic*>*
	    \param	p_pmData2Data	a parameter of type SP_Data*>*

	    \return	bool


    */
    bool AddToCopyMap(SP_DS_Graph** p_ppcGraph, unsigned int p_nNetnumber, SP_MapGraphic2Graphic* p_pmGraphic2Graphic, SP_MapData2Data* p_pmData2Data);
    bool ShowGlobalOptions(SP_DLG_GraphProperties* p_pcDlg);
    bool OnGlobalOptions();
    bool SqueezeIdAttributes();
    bool ShowDebug();
    wxString& SetName(const wxString& p_pchName);

    bool Update();

};


#endif // __SP_DS_EDGECLASS_H__
