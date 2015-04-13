//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/03/31 11:55:00 $
// Short Description: Graphical Metadata-Base Header
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GR_METADATA_H__
#define __SP_GR_METADATA_H__

#include "sp_core/base/SP_Error.h"
#include "sp_core/base/SP_Type.h"
#include "sp_core/base/SP_Graphic.h"

#include "sp_core/SP_List.h"

//#include "sp_ds/SP_DS_Metadata.h"
class SP_DS_Metadata;

class SP_GR_Metadata: public SP_Error, public SP_Graphic
{
private:
protected:
    // indicates, whether the shape is editable
    // default is TRUE
    bool m_bFixedSize;
public:
    SP_GR_Metadata(SP_DS_Metadata* p_pcDataParent);
    virtual ~SP_GR_Metadata();

    //-----------------------------------------------------
    // Accessors
    virtual bool SetWidth(double p_nVal);
    virtual double GetWidth();
    virtual bool SetHeight(double p_nVal);
    virtual double GetHeight();

    virtual wxShape* GetPrimitive() = 0;

    //-----------------------------------------------------
    //! - in base class
    void SetFixedSize(bool p_bVal);

    inline bool GetFixedSize() { return m_bFixedSize; }

    bool ShowAttributeGraphic();

    bool AddToCanvas(SP_GUI_Canvas* p_pcCanvas, double p_nX = -1, double p_nY = -1, int p_nKeys = 0);
    bool AddToDeleteQueue(SP_GRAPHIC_STATE p_eFilter = SP_STATE_ALL);

    virtual SP_Graphic* Clone(SP_Data* p_pcParent) = 0;

    virtual bool OnDoubleClick(int p_nKeys);

    bool EditProperties();

    virtual bool Update(bool p_bLocalOnly = FALSE);
    virtual bool ResetGraphicState();

    //! for call from within derived_classes::Clone method
    virtual bool CloneBase(SP_Graphic* p_pcCopy);
};

#endif // __SP_GR_METADATA_H__
