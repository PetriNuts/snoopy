//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: XML Reader, reads graph structures
//////////////////////////////////////////////////////////////////////
#ifndef __SP_READER_H__
#define __SP_READER_H__

#include "sp_core/base/SP_Error.h"
#include "sp_ds/SP_DS_Graph.h"

class SP_Reader: public SP_Error
{
private:
protected:
    virtual void Init() = 0;
public:
    SP_Reader();
    virtual ~SP_Reader();

    virtual bool Read(SP_DS_Graph* p_pcGraph, const wxString& p_sFile);
};

#endif // __SP_READER_H__

