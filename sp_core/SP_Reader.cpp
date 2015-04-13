//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: XML Readr, writes graph structures actually
//////////////////////////////////////////////////////////////////////
#include "sp_core/SP_Reader.h"

SP_Reader::SP_Reader()
{
//     if (!m_bError)
//         Init();
}

void
SP_Reader::Init()
{
}

SP_Reader::~SP_Reader()
{
}

bool
SP_Reader::Read(SP_DS_Graph* p_pcGraph, const wxString& p_sFile)
{
    return !m_bError;
}

