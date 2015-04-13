//////////////////////////////////////////////////////////////////////
// $Author: rr $
// $Version: 0.1 $
// $Date: 2008/01/07 $
// Short Description: place animator for Modulo Nets
//////////////////////////////////////////////////////////////////////

#include "sp_ds/extensions/SP_DS_Animation.h"
#include "sp_ds/animators/SP_DS_ModNetPlaceAnimator.h"
#include "sp_ds/animators/SP_DS_TransAnimator.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"


SP_DS_ModNetPlaceAnimator::SP_DS_ModNetPlaceAnimator(const wxString& p_pchAttributeName,
						   SP_DS_Animation* p_pcAnim,
						   SP_DS_Node* p_pcParent)
  : SP_DS_PlaceAnimator(p_pchAttributeName, p_pcAnim, p_pcParent),
  m_nModToken(0)
{
}

SP_DS_ModNetPlaceAnimator::~SP_DS_ModNetPlaceAnimator()
{
}

SP_DS_Animator*
SP_DS_ModNetPlaceAnimator::Clone(SP_Data* p_pcParent, SP_DS_Animation* p_pcAnim)
{
  if (p_pcParent->GetElementType() != SP_ELEMENT_NODE)
    return NULL;

  SP_DS_ModNetPlaceAnimator* l_pcNew = new
    SP_DS_ModNetPlaceAnimator(m_sAttributeName,
			     p_pcAnim,
			     dynamic_cast<SP_DS_Node*>(p_pcParent));

  return dynamic_cast<SP_DS_Animator*>(l_pcNew);
}



long
SP_DS_ModNetPlaceAnimator::DecrementedMarking(const SP_ListEdge* p_edges,
					     long p_tokens)
{
	if (!p_edges) return p_tokens;


	bool l_bHasModulo=false;
	long l_nOldVal = m_pcAttribute->GetValue();
	long l_nModuloVal=-1;

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Nodeclass* l_pcNodeclass = l_pcGraph->GetNodeclass(wxT("Modulo"));

	if (! l_pcNodeclass->GetElements()->empty())
	{
		SP_DS_Node* l_pcNode = (* l_pcNodeclass->GetElements()->begin());
		wxString l_sModuloVal = l_pcNode->GetAttribute(wxT("Modulo"))->GetValueString();
		l_sModuloVal.ToLong(&l_nModuloVal);
	}
	if (l_nModuloVal == 0){
			SP_MESSAGEBOX(wxT("The Modulo Value must be greater zero"),
						  wxT("Error"), wxOK | wxICON_ERROR);
			return p_tokens;

	}

	SP_DS_Attribute* l_pcAttr;
	SP_ListEdge::const_iterator l_eIt;


	for (l_eIt = p_edges->begin(); l_eIt != p_edges->end(); ++l_eIt)
	{
		if ((*l_eIt)->GetClassName()== wxT("Undirected Edge") )
		{
			if (l_nModuloVal == -1)
			{
				SP_MESSAGEBOX(wxT("The Modulo Value must be set: all undireted edges are ignored"),
						  wxT("Error"), wxOK | wxICON_ERROR);
				break;
			}
			l_pcAttr = (*l_eIt)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MULTIPLICITY);
			if(l_pcAttr)
			{
				long l_nArcMulti = dynamic_cast<SP_DS_MultiplicityAttribute*>(l_pcAttr)->GetValue();
				m_nModToken += (l_nOldVal + l_nArcMulti )% l_nModuloVal;
				l_bHasModulo= true;
			}//end if
		} //end if
	}//end for

	if (l_bHasModulo) return m_nModToken;


	for (l_eIt = p_edges->begin(); l_eIt != p_edges->end(); ++l_eIt)
	{
		if ((*l_eIt)->GetClassName()== wxT("Edge") )
		{
			  l_pcAttr = (*l_eIt)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MULTIPLICITY);
			  if (l_pcAttr)
			  {
				p_tokens -= dynamic_cast<SP_DS_MultiplicityAttribute*>(l_pcAttr)->GetValue();
			  } //end if
		}//end if
	} //end for
	return p_tokens;
} //end DecrementedMarking

bool
SP_DS_ModNetPlaceAnimator::DecrementMark()
{
  CHECK_POINTER(m_pcAttribute, return FALSE);
  CHECK_POINTER(m_pcNode, return FALSE);

  long val = m_pcAttribute->GetValue();

  map<SP_DS_TransAnimator*, SP_ListEdge >::iterator it;
  for (it = m_mlCandidates.begin(); it != m_mlCandidates.end(); ++it)
  {
    val = DecrementedMarking(&((*it).second), val);
  }	//end for

  m_pcAttribute->SetValue(val);
  m_pcNode->Update(TRUE);
  m_nModToken=0;

  return TRUE;
}//end DecrementedMark



unsigned int
SP_DS_ModNetPlaceAnimator::SumEdge(SP_DS_Edge* p_edge)
{
  if (! p_edge) return 0;
  if (p_edge->GetClassName()== wxT("Edge") ){
    return SP_DS_PlaceAnimator::SumEdge(p_edge);
  }
  return 0;
}
