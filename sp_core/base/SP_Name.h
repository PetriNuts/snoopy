//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_NAME_H__
#define __SP_NAME_H__

#include <wx/wx.h>

/**	\brief	Baseclass for all objects, that need to be identified by a 'name'

  Not to be mixed up with a 'Name'-Attribute for net-elements.
  This class should be a base for all objects, that need a name (i.e. graph,
  netclass, etc. but not nodes/edges)
*/
class SP_Name
{
protected:
    wxString m_sNameVal;
    wxString m_sDisplayNameVal;
    wxString m_sAbbreviation;

public:
    SP_Name(const wxString& p_pchValue, const wxString& p_sAbbr = {}):
    	m_sNameVal(p_pchValue),m_sDisplayNameVal(p_pchValue), m_sAbbreviation(p_sAbbr)
	{}
    virtual ~SP_Name() {}

    virtual const wxString GetName() { return m_sNameVal; }
    /**	\brief	The SetName function

	    \param	p_pchName	the new name
	    \return	wxString&   the new name
    */
    virtual wxString& SetName(const wxString& p_pchName) { m_sNameVal = p_pchName; return m_sNameVal; }

    virtual const wxString GetDisplayName() { return m_sDisplayNameVal; }
    virtual wxString& SetDisplayName(const wxString& p_pchName) { m_sDisplayNameVal = p_pchName; return m_sDisplayNameVal; }

    virtual const wxString GetAbbreviation() { return m_sAbbreviation.IsEmpty() ? m_sNameVal.at(0) : m_sAbbreviation; }
    virtual wxString& SetAbbreviation(const wxString& p_pchName) { m_sAbbreviation = p_pchName; return m_sAbbreviation; }

};

#endif // __SP_NAME_H__
