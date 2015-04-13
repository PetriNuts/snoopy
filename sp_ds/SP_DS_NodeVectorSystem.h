//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: kw $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/02/18 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DS_NODEVECTORSYSTEM_H_
#define __SP_DS_NODEVECTORSYSTEM_H_
#include "sp_core/base/SP_Error.h"
#include "sp_core/base/SP_Type.h"
#include "sp_core/base/SP_Name.h"
#include "sp_core/SP_List.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Node.h"

#include "sp_ds/SP_DS_Visitor.h"
#include "sp_ds/SP_DS_NodeVector.h"

typedef list<SP_DS_NodeVector*> SP_NodeVectorListType;
/**	\brief	class representing a set of node Vectors

  For example, this set can represent the minimal invariants of a petrinet.
*/
class SP_DS_NodeVectorSystem: public SP_Error, public SP_Name
{
private:
	// the nodes
   SP_ListNode m_lNodes;
	// the node vectors
	SP_NodeVectorListType* m_lElements;
	//method which answers the question p_sQuestion (true or false)
  bool AnswerQuestion(const wxString& p_sQuestion);
  // the informations at the start of the file
  wxString m_sInfo;

  int m_nMinNotNullOcc;
  int m_nMaxNotNullOcc;

protected:

public:

	/**	\brief	Constructor

	    \param	p_pcGraph	The graph, this class is part of
    */
  SP_DS_NodeVectorSystem(SP_DS_Graph* p_pcGraph);
    ~SP_DS_NodeVectorSystem();


	/**	\brief	Add a node vector to the node vector system

	    \param	p_pcNodeVec	the node vector
    */
  bool AddElement(SP_DS_NodeVector* p_pcNodeVec);


	/**	\brief	Remove a node vector from the node vector system

	    \param	p_pcNodeVec	the node vector
    */
	bool RemoveElement(SP_DS_NodeVector* p_pcNodeVec);


	/**	\brief	Remove all node vectors and set remove the nodeList
  */
	bool Clear();

	/**	\brief	apply visitor to all NodeVectors

	    \param	p_pcVisitor	the visitor
    */


	bool AcceptAll(SP_DS_Visitor* p_pcVisitor);

	/**	\brief	apply visitor to all NodeVectors where the answer of the system
						to the question is 'true'

	    \param	p_question	the question as string
	    \param	p_pcVisitor	the visitor
    */
	bool AcceptWhere(const wxString& p_question, SP_DS_Visitor* p_pcVisitor);


	/**	\brief	returns the nodevector where the id is p_nNumber

	    \param	p_nNumber	number of the node vector
    */
	SP_DS_NodeVector* GetNodeVectorById(int p_nNumber);


	/**	\brief	Set the node list that all node vectors belong to

			This node list holds the ordering of the nodes a nodevector is useless,
			if you don't know the ordering of the nodes in the vector
	    \param	p_lNodes	the node list
    */
	bool SetNodeList(const SP_ListNode& p_lNodes);

	/**	\brief	Adds a node to the node list that all node vectors belong to

			This node list holds the ordering of the nodes a nodevector is useless,
			if you don't know the ordering of the nodes in the vector
	    \param	p_pNode	the node list
    */
	bool AddNodeToNodeList(SP_DS_Node* p_pNode);

	/**	\brief	Get the node list that all node vectors belong to

			This node list holds the ordering of the nodes a nodevector is useless,
			if you don't know the ordering of the nodes in the vector
	    \return	m_lNodes	the node list
    */
	inline const SP_ListNode& GetNodes() { return m_lNodes; }


	/**	\brief	Return the number of node vectors this system consists of

			\return	the number of node vectors
    */
	int Size();

  int GetOccurrenceRangeMin(){return m_nMinNotNullOcc;}
  void SetOccurrenceRangeMin(int p_nMin) {m_nMinNotNullOcc = p_nMin;}
  int GetOccurrenceRangeMax(){return m_nMaxNotNullOcc;}
  void SetOccurrenceRangeMax(int p_nMax){m_nMaxNotNullOcc = p_nMax;}

  wxString GetInformation();

  void SetInformation(const wxString& p_sInfo);

};
#endif // __SP_DS_NODEVECTORSYSTEM_H__
