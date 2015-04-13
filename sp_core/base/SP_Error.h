//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ERROR_H__
#define __SP_ERROR_H__

#include <wx/wx.h>

/**	\brief	simple class to derive from

  Stores informations about any error state and can report these to the caller.
  Numerous classes are derived from this class to be able to report any errors
  tho the caller of any of their functions.
  The SetLastError method is used within the CHECK_POINTER and CHECK_BOOL macros,
  defined in sp_defines.h.
  Derived classes cann use the << stream operator to fill the m_sMessage member
  successively.
*/
class SP_Error
{
protected:
	/** \brief human readable message for indicating an error    */
    wxString m_sMessage;
    /**	\brief Flag if an error occured
    */
    bool m_bError;
    /**	\brief	The SetLastError function

	    \param	p_sMsg	message to set the m_sMessage member to

	    Sets the m_sMessage member (and resets any value, that was set before) to
        the value of the parameter and switches the m_bError flag to TRUE
    */
    inline void SetLastError(const wxString& p_sMsg) { m_bError = TRUE; m_sMessage = p_sMsg; }
public:
    /**	\brief	The SP_Error function

        c'tor
    */
    SP_Error(): m_sMessage(wxT("")), m_bError(FALSE) { }

    /**	\brief	accessing the last error

	    \return	the message as const wxString&    */
    inline const wxString& GetLastError() const { return m_sMessage; }
    /**	\brief	The ResetErrors function

	    Resets the m_sMessage member to the empty string and m_bError to FALSE
    */
    inline void ResetErrors() { m_sMessage = wxT(""); m_bError = FALSE; }

    /**	\brief	The GetErrorSet function. Simple accessor

	    \return	bool the state of m_bError
    */
    inline const bool GetErrorSet() const { return m_bError; }
};

#endif // __SP_ERROR_H__
