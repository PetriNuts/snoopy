//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: kw $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/02/21 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DS_NODEVECTOR_H_
#define __SP_DS_NODEVECTOR_H_

#include "sp_core/base/SP_Error.h"
#include "sp_core/base/SP_Type.h"
#include "sp_core/base/SP_Name.h"
#include "sp_core/SP_List.h"
#include "sp_ds/SP_DS_Attribute.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_core/base/SP_Graphic.h"

#include "sp_ds/SP_DS_Visitor.h"
#include  "sp_ds/SP_DS_NodeVectorSystem.h"
class SP_DS_NodeVectorSystem;


/**	\brief	class representing a set of node Vectors

  For example, this node vector can represent invariants of a petrinet. To a
	node vector belong informations about the nodes, their occurrence and some
	additional informations holded in attributes
*/
class SP_DS_NodeVector: public SP_Error, public SP_Name, public SP_Data,
public SP_Graphic
{
private:
	//nodeclass
    //SP_ListNode m_lNodes;
	SP_VectorLong m_aVector;

	// the node vector system where the instance belong to
	SP_DS_NodeVectorSystem* m_pcNvs;

protected:
	/**	\brief list of attributes, this node shows
    */
    SP_ListAttribute m_lAttributes;

public:

	/**	\brief	Constructor
	    \param	p_pcGraph	The graph, this class is part of
	    \param	p_pchName	The name of the system to be shown to the user
    */
    SP_DS_NodeVector(SP_DS_Graph* p_pcGraph, SP_DS_NodeVectorSystem* p_pcNvs);
    ~SP_DS_NodeVector();

	/**	\brief	Sets the occurrence information.

			\param	p_aVector	the vector which holds the occurrence informations
  */

	bool SetVector(const SP_VectorLong& p_aVector) { m_aVector = p_aVector; return true; }

	// the standard set and getAttribute methods
	SP_DS_Attribute* AddAttribute(SP_DS_Attribute* p_pcAttr);
  SP_DS_Attribute* GetAttribute(const wxString& p_pchName);
  bool HasAttributeType(SP_ATTRIBUTE_TYPE p_eVal);
  SP_DS_Attribute* GetFirstAttributeByType(SP_ATTRIBUTE_TYPE p_eVal);

  inline const SP_ListAttribute* GetAttributes() { return &m_lAttributes; }
  SP_DS_Attribute* RemoveAttribute(SP_DS_Attribute* p_pcVal);

	/**	\brief	Returns the occurrence information.

			\return	a vector which holds the occurrence informations
  */
	inline const SP_VectorLong& GetVector() { return m_aVector; }

	/**	\brief	Applys the visit-method from the visitor p_pcVisitor to instance.

			\param	p_pcVisitor the visitor
  */
	bool Accept(SP_DS_Visitor *p_pcVisitor);

	/**	\brief	Removes the instance from the parent node vector system .
  */
	bool Remove();

	/**	\brief	The AddDialogAttributes function

	    \param	p_ptmAttributes	Map of attribute names to attributes

	    \return	bool    TRUE in case of success, FALSE otherwise

	    This function is called from SP_Graphic::EditProperties and adds
        all attributes to the map in order to get them shown in the
        properties-dialog
    */
  bool AddDialogAttributes(SP_MapString2ListAttribute* p_ptmAttributes);


  const wxString GetClassName();
  SP_ElementClass* GetClassObject();

  // to implement SP_Graphic
  virtual wxShape* GetPrimitive();
  virtual SP_Graphic* Clone(SP_Data* p_pcParent);

  bool EditProperties();

  SP_DS_NodeVectorSystem* GetNodeVectorSystem(){ return m_pcNvs; }

//  bool operator<(SP_DS_NodeVector* p_pcLeft, SP_DS_NodeVector* p_pcRight);


};
#endif // __SP_DS_NODEVECTOR_H__
