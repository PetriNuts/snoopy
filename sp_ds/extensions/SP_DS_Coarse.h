//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Extension for netelements (only nodes actually)
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_COARSEEXTENSION_H__
#define __SP_DS_COARSEEXTENSION_H__

#include "sp_core/base/SP_Error.h"
#include "sp_core/base/SP_Name.h"
#include "sp_core/SP_Map.h"
#include "sp_core/base/SP_Type.h"

#include "sp_gui/widgets/SP_WDG_Treectrl.h"

class SP_Data;
class SP_Graphic;
class SP_DS_Graph;
class SP_DS_Nodeclass;
class SP_MDI_CoarseDoc;
class SP_DS_Node;
class SP_DS_Edge;

class SP_DS_Coarse: public SP_Error, public SP_Name
{
private:
protected:
    //! flag, whether to clone the graph or not
    bool m_bClone;
    //! are we in an update circle?
    bool m_bUpdate;
    //! Graph to show
    SP_DS_Graph* m_pcGraph;
    //! element we belong to
    SP_Data* m_pcParent;
    //! graphic of our parent for short access
    SP_Graphic* m_pcGraphic;
    //! name of the nodeclass, we display in our subnet
    //! on connection
    wxString m_sInnerClass;
    wxString m_sLabelAttribute;
    //! netnumber, we are displaying inside
    unsigned int m_nNetnumber;
    //! document, we are displaying
    SP_MDI_CoarseDoc* m_pcDocument;
    wxTreeItemId m_nCoarseId;
public:
		void ShowDebug();

		SP_DS_Coarse(const wxString& p_pchName,
        const wxString& p_pchInnerClass,
        SP_DS_Graph* p_pcContent,
        bool p_bClone = TRUE);

    ~SP_DS_Coarse();

    SP_DS_Coarse* Clone();

    inline SP_MDI_CoarseDoc* GetCoarseDoc() const {return m_pcDocument;}
		inline void SetCoarseDoc(SP_MDI_CoarseDoc *p_pcDoc) { m_pcDocument = p_pcDoc; }

    SP_Data* SetParent(SP_Data* p_pcVal);
    inline SP_Data* GetParent() const { return m_pcParent; }

    void SetLabelAttribute(const wxString& p_pchVal) { m_sLabelAttribute = p_pchVal; }
    const wxString GetLabelAttribute() const { return m_sLabelAttribute; }
    const wxString GetLabelAttributeLabel();

    inline SP_DS_Graph* GetGraph() const { return m_pcGraph; }

    SP_Graphic* GetGraphic();
    inline const wxString GetInnerClass() const { return m_sInnerClass; }

    unsigned int GetNetnumber();
    bool SetNetnumber(unsigned int p_nNewVal, unsigned int p_nOldVal = 0, bool p_bRecursive = FALSE);
    bool Flatten(unsigned int p_nNewNet, SP_ListGraphic* p_plGraphics = NULL);
    bool OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet);
    bool OnCoarse(unsigned int p_nToNet);

    void SetCoarseId(wxTreeItemId p_nId);
		inline wxTreeItemId GetCoarseId() { return m_nCoarseId; }
		inline void SetUpdate(bool p_bVal) { m_bUpdate = p_bVal; }

    bool ChangedClass(SP_Type* p_pcNewClass);

    bool Show(bool p_bShowWindow = true);
    bool Update(bool p_bShowWindow = FALSE, bool p_bLocalOnly = FALSE);

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

    /**	\brief	The AddToCopyMap function

        \param  p_ppcGraph  reference to the graph we are copied to
	    \param	p_pcGraphic	a parameter of type SP_Graphic*
	    \param	p_pmGraphic2Graphic	a parameter of type SP_Graphic*>*
	    \param	p_pmData2Data	a parameter of type SP_Data*>*

	    \return	bool
    */
    bool AddToCopyMap(SP_DS_Graph** p_ppcGraph, SP_Graphic* p_pcGraphic, SP_MapGraphic2Graphic* p_pmGraphic2Graphic, SP_MapData2Data* p_pmData2Data);
};

#endif // __SP_DS_COARSEEXTENSION_H__


