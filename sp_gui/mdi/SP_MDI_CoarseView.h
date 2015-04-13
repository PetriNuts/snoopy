//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_MDI_COARSEVIEW_H__
#define __SP_MDI_COARSEVIEW_H__

#include "sp_gui/mdi/SP_MDI_View.h"

class SP_MDI_CoarseView: public SP_MDI_View
{
private:
    DECLARE_DYNAMIC_CLASS(SP_MDI_CoarseView)
    DECLARE_EVENT_TABLE()
protected:
public:
    SP_MDI_CoarseView(void);
    ~SP_MDI_CoarseView(void);

    virtual void OnActivateView(bool activate, wxView *activeView,
				wxView *deactiveView);

    bool OnClose(bool p_bDeleteWindow = TRUE);
};

#endif // __SP_MDI_COARSEVIEW_H__
