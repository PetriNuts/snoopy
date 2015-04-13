//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/01/05 11:55:00 $
// Short Description: Gate Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_NAMEDRECTANGLE_H__
#define __SP_GR_NAMEDRECTANGLE_H__

#include "sp_gr/shapes/SP_GR_DrawnShape.h"

class SP_GR_BaseDrawn;

class SP_GR_NamedRectangle: public SP_GR_DrawnShape
{
private:
protected:
	wxString m_shapeName;
	virtual bool Draw();

public:
    SP_GR_NamedRectangle(SP_DS_Node* p_pcParent,
			 const wxString& newName = wxT("%"), double p_nWidth = 20.0,
			double p_nHeight = 20.0);
    virtual ~SP_GR_NamedRectangle();


    virtual SP_Graphic* Clone(SP_Data* p_pcParent);
	virtual wxShape* GetPrimitive();
	virtual bool Update(bool p_bLocalOnly );




};

#endif // __SP_GR_NAMEDRECTANGLE_H__


