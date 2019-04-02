#pragma once
/*
*Author: George Assaf
*version:
*date:
*description: convert an Element
*/
#ifndef SP_CONVERTELEMENTG_H_
#define SP_CONVERTELEMENTG_H_
#include "sp_core/SP_XmlWriter.h"
#include <vector>
class SpConvertElement
{
private:
	std::vector<wxString>  m_lConvertedElements;
 
public:
	SpConvertElement();
	~SpConvertElement();


	virtual SP_DS_Node* ConvertNode(SP_DS_Node* p_pcOldNode, SP_DS_Nodeclass* p_pcNewNodeClass);
	virtual bool UpdateSourceEdgeGraphics(SP_DS_Edge& p_edge, const SP_ListGraphic& p_oldGraphicList, const   SP_ListGraphic& p_newGraphicList) const;
	virtual SP_Graphic* FindGraphic(const SP_ListGraphic& p_oldGraphicList, const SP_ListGraphic& p_newGraphicList, const SP_Graphic& p_oldGraphic)  const;
	virtual bool  UpdateTargetEdgeGraphics(SP_DS_Edge& p_edge, const SP_ListGraphic& p_oldGraphicList, const SP_ListGraphic& p_newGraphicList) const ;
	virtual bool  IsConverted(wxString& elementName);
	virtual SP_DS_Node* Clone(SP_DS_Node& p_pcOldNode, SP_DS_Nodeclass& p_pcNewNodeClass) const;
	
};
#endif /* SP_CONVERTELEMENTG_H_ */