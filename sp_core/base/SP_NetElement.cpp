//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#include "sp_core/base/SP_NetElement.h"

#include <wx/wx.h>

bool
SP_NetElement::SetNetnumber(unsigned int p_nNewVal, unsigned int p_nOldVal)
{
    if ((p_nOldVal == 0) || (p_nOldVal == m_nNetnumber))
    {
        m_nNetnumber = p_nNewVal;
    }

    return TRUE;
}

