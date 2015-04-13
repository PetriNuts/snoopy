//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_NETELEMENT_H__
#define __SP_NETELEMENT_H__

#include <wx/wx.h>

/**	\brief	Base class for all objects, that represent a net element

  This class encapsulates the identifier for the physical or logical net,
  the object exists in.
*/
class SP_NetElement
{
private:
    //! queued to be deleted afterwards
    //! usually set in SP_DS_Attribute::Merge and
    //! evaluated in SP_DLG_ShapeProperties::OnDlgOk
    //! for DS and SP_Graphic::AddToDeleteQueue for GR
    bool m_bDelete;
    bool m_bHide;
    unsigned int m_nNetnumber;
    /**
     * count the number of lists, which the NetElement is into
     */
    unsigned int m_nCountLists;
protected:
public:
    SP_NetElement(unsigned int p_nVal = 0): m_bDelete(FALSE), m_bHide(FALSE), m_nNetnumber(p_nVal), m_nCountLists(0) { }
    virtual ~SP_NetElement() { }

    virtual bool SetNetnumber(unsigned int p_nNewVal, unsigned int p_nOldVal = 0);
    inline unsigned int GetNetnumber() const { return m_nNetnumber; }

    /**	\brief	The SetDelete function

	    \param	p_bVal	TRUE if this object is queued to be deleted

	    \return bool    TRUE in case of success, FALSE otherwise

	    An object beeing set to TRUE in its m_bDelete member is
        deleted from within the SP_WDG_DialogBase::OnDlgOk routine.
        That applies to all attributes of objects, merged with another
        one (i.e. set logical).
    */
    inline bool SetDelete(bool p_bVal = TRUE) { m_bDelete = p_bVal; return TRUE; }
    /**	\brief	The GetDelete function

	    \return	bool TRUE if the object is marked for deletion

	    This function is called from within SP_WDG_DialogBase::OnDlgOk where
        all the attributes, collected of beeing modified by the dialog and
        marked for deletion are really deleted.
        That applies to all attributes of objects, merged with another
        one (i.e. set logical).
    */
    inline bool GetDelete() const { return m_bDelete; }
    /**	\brief	The SetHide function

	    \param	p_bVal	TRUE if this object is hide

	    \return bool    TRUE in case of success, FALSE otherwise

	    An object beeing set to TRUE in its m_bHide member is
		being hidden from the Datastructure and the Graphic is
		grayed out.
    */
    inline bool SetHide(bool p_bVal) { m_bHide = p_bVal; return TRUE; }
    /**	\brief	The GetHide function

	    \return	bool TRUE if the object is hidden
    */
    inline bool GetHide() const { return m_bHide; }
    /**
     *
     * @return number of lists which the NetElement is in
     */
    inline unsigned int GetCountLists() const { return m_nCountLists; }
    /**
     *  NetElement added to list and counter increased
     */
    inline void IncreaseCountLists() { ++m_nCountLists; }
    /**
     *  NetElement removed from list and counter decreased
     */
    inline void DecreaseCountLists() { if(m_nCountLists > 0) --m_nCountLists; }
};

#endif // __SP_NETELEMENT_H__
