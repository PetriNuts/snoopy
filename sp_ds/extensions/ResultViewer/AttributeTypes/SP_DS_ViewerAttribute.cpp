/*
 * SP_DS_ViewerAttribute.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 14.06.2011
 * Short Description:
 *///=================================================
#include "sp_ds/extensions/ResultViewer/AttributeTypes/SP_DS_ViewerAttribute.h"

SP_DS_ViewerAttribute::SP_DS_ViewerAttribute(const wxString& p_sName,const wxString& p_sCategory,
                                             const wxString& p_sDisplayName)
:m_sName(p_sName),
 m_sCategory(p_sCategory),
 m_sDisplayName(p_sDisplayName)
{


}

SP_DS_ViewerAttribute::~SP_DS_ViewerAttribute()
{

}
