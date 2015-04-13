//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: kw $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/02/18 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/SP_DS_NodeVectorSystem.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"

SP_DS_NodeVectorSystem::SP_DS_NodeVectorSystem(SP_DS_Graph* p_pcGraph) :
	SP_Name(wxT("NodeVectorSystem")),
	m_nMinNotNullOcc(0),
	m_nMaxNotNullOcc(0)
{
	m_lElements = new SP_NodeVectorListType();
}

SP_DS_NodeVectorSystem::~SP_DS_NodeVectorSystem()
{
	Clear();
	wxDELETE(m_lElements);
}

bool SP_DS_NodeVectorSystem::Clear()
{
	SP_NodeVectorListType::iterator l_itElement;
	for(l_itElement = m_lElements->begin();
		l_itElement != m_lElements->end();
		l_itElement = m_lElements->begin())
	{
		SP_DS_NodeVector* l_pcNodeVector = (*l_itElement);
		m_lElements->erase(l_itElement);
		wxDELETE(l_pcNodeVector);
	}
	m_lNodes.clear();
	return true;
}

// Add a node vector to the node vector system
bool SP_DS_NodeVectorSystem::AddElement(SP_DS_NodeVector* p_pcNodeVec)
{
	/* @todo pruefen ob schon drin */
	CHECK_POINTER(p_pcNodeVec, return FALSE);
	m_lElements->push_back(p_pcNodeVec);
	return TRUE;
}

// Remove a node vector from the node vector system
bool SP_DS_NodeVectorSystem::RemoveElement(SP_DS_NodeVector* p_pcNodeVec)
{
	/* @todo pruefen ob ueberhaupt drin */
	CHECK_POINTER(p_pcNodeVec, return FALSE);
	m_lElements->remove(p_pcNodeVec);
	return TRUE;
}

bool SP_DS_NodeVectorSystem::AnswerQuestion(const wxString& p_sQuestion)
{
	return TRUE;
}

SP_DS_NodeVector*
SP_DS_NodeVectorSystem::GetNodeVectorById(int p_nNumber)
{
	CHECK_POINTER(m_lElements, return FALSE);
	SP_DS_Attribute* l_pcAttr;

	SP_NodeVectorListType::const_iterator l_itNodeVector = m_lElements->begin();
	while (l_itNodeVector != m_lElements->end())
	{
		l_pcAttr = (*l_itNodeVector)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID);
		if (l_pcAttr && (p_nNumber == dynamic_cast<SP_DS_IdAttribute*> (l_pcAttr)->GetValue()))
		{
			return (*l_itNodeVector);
		}
		l_itNodeVector++;
	}
	return NULL;
}

// apply visitor to all NodeVectors
bool SP_DS_NodeVectorSystem::AcceptAll(SP_DS_Visitor* p_pcVisitor)
{

	CHECK_POINTER(p_pcVisitor, return FALSE);
	CHECK_POINTER(m_lElements, return FALSE);
	SP_NodeVectorListType::const_iterator l_itNodeVector = m_lElements->begin();
	while (l_itNodeVector != m_lElements->end())
	{
		(*l_itNodeVector)->Accept(p_pcVisitor);
		l_itNodeVector++;
	}
	return TRUE;

}

bool SP_DS_NodeVectorSystem::AcceptWhere(const wxString& p_question, SP_DS_Visitor* p_pcVisitor)
{
	CHECK_POINTER(p_pcVisitor, return FALSE);
	CHECK_POINTER(m_lElements, return FALSE);

	SP_NodeVectorListType::const_iterator l_itNodeVector = m_lElements->begin();
	while (l_itNodeVector != m_lElements->end())
	{
		if (AnswerQuestion(p_question))
		{
			(*l_itNodeVector)->Accept(p_pcVisitor);
		}
		l_itNodeVector++;
	}
	return TRUE;
}

// Set the node list that all node vectors belong to
bool SP_DS_NodeVectorSystem::SetNodeList(const SP_ListNode& p_lNodes)
{

	m_lNodes = p_lNodes;
	return TRUE;

}

bool SP_DS_NodeVectorSystem::AddNodeToNodeList(SP_DS_Node* p_pNode)
{
	CHECK_POINTER(p_pNode, return FALSE);

	m_lNodes.push_back(p_pNode);

	return true;
}

int SP_DS_NodeVectorSystem::Size()
{
	CHECK_POINTER(m_lElements, return 0);

	return m_lElements->size();
}

wxString SP_DS_NodeVectorSystem::GetInformation()
{
	return m_sInfo;
}

void SP_DS_NodeVectorSystem::SetInformation(const wxString& p_sInfo)
{
	m_sInfo = p_sInfo;
}

