//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/03/27 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_METADATACLASS_H__
#define __SP_DS_METADATACLASS_H__

#include "sp_core/base/SP_ElementClass.h"
#include "sp_ds/SP_DS_Metadata.h"

class SP_DLG_GraphProperties;

typedef list<SP_DS_Metadataclass*> SP_ListMetadataclass;

/**	\brief	class representing a set of nodes

  Each graph is built up by different classes of nodes and edges. Each class (i.e. set) defines
  unique members of the whole set, sharing the same definition (attributes, graphic, behaviour)
*/
class SP_DS_Metadataclass: public SP_ElementClass
{
private:
protected:
    /**	\brief Special member, defining the look, the attributes and the behaviour of all members of this class
    */
    SP_DS_Metadata* m_pcPrototype;

    /**	\brief List of all elements of this class
    */
    SP_ListMetadata m_lElements;

    bool SynchronizeShowFlags(SP_DS_Attribute* p_pcAttr);

public:
    /**	\brief	Constructor

	    \param	p_pcGraph	The graph, this class is part of
	    \param	p_pchName	The name of the set to be shown to the user
    */
    SP_DS_Metadataclass(SP_DS_Graph* p_pcGraph, const wxString& p_pchName, LongPtr p_nIdCounter = LongPtr(NULL));
    ~SP_DS_Metadataclass();

    /**	\brief	Accessor to the prototype

        \return	the m_pcPrototype member
    */
    SP_DS_Metadata* GetPrototype();

    /**	\brief	Creates a new element, by cloning the prototype

	    \param	p_nNetnumber	The initial netnumber, important for hirarchical nets
        \param  p_pcCloneRef    object to be used as blueprint while cloning

	    \return	the new member
    */
    SP_DS_Metadata* NewElement(unsigned int p_nNetnumber, SP_DS_Metadata* p_pcCloneRef = NULL);

    bool AddElement(SP_DS_Metadata* p_pcVal);
    bool RemoveElement(SP_DS_Metadata* p_pcVal);

    bool RemoveElements(unsigned int p_nNetnumber, SP_ListData* p_plCollect);

    //TODO: do not use this function, need to be removed
    bool RemoveAll();

    /**
     * @return pointer to list of elements
     */
    inline const SP_ListMetadata* GetElements() { return &m_lElements; }

    SP_DS_Metadata* GetRelative(SP_DS_Metadata* p_pcMetadata, const wxString& p_pchRefAttribute, bool p_bLogic);

    // convenience functions to prepare the prototype
    /**	\brief	call AddAttribute of the prototype

	    \param	p_pcAttr	the prototype

	    \return	the attribute given in the param
    */
    SP_DS_Attribute* AddAttribute(SP_DS_Attribute* p_pcAttr);
    bool HasAttributeType(SP_ATTRIBUTE_TYPE p_eVal);

    SP_DS_Coarse* SetCoarse(SP_DS_Coarse* p_pcVal);

    /**	\brief	Define a graphical representation for this class

	    \param	p_pcGraphic	the graphic object

	    \return	the graphic, given in the param
    */
    SP_Graphic* SetGraphic(SP_Graphic* p_pcGraphic);

    bool OnLeftClickOnCanvas(SP_GUI_Canvas* p_pcCanvas, double p_nX, double p_nY, int p_nKeys);

    SP_Data* GetCoarseNode(unsigned int p_nNetnumber);
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
    bool ResetCoarseTree();
    bool ShowGlobalOptions(SP_DLG_GraphProperties* p_pcDlg);
    bool OnGlobalOptions();
    bool SqueezeIdAttributes();

    bool ShowDebug();
    wxString& SetName(const wxString& p_pchName);

    bool Update();

};


#endif // __SP_DS_METADATACLASS_H__
