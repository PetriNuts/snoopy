//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_NOTEBOOKPAGE_H__
#define __SP_WDG_NOTEBOOKPAGE_H__

#include <limits>
//#include <wx/wx.h>
#include <wx/scrolwin.h>

class SP_WDG_Notebook;

class SP_WDG_NotebookPage: public wxScrolledWindow
{
private:
    DECLARE_CLASS(SP_WDG_NotebookPage)
    DECLARE_EVENT_TABLE()
protected:
    wxBoxSizer* m_pcTopSizer;
    unsigned int m_nOrdering;
public:
    SP_WDG_NotebookPage(wxWindow* p_pcParent, unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max());

    /**	\brief	The AddControl function

	    \param	p_pcSizer	the sizer object, generated by the WDG_Dialog*
	    \param	p_nOpt      option paramter for the sizer
	    \param	p_nFlag     flag parameter for the sizer
	    \param	p_nBorder   border parameter for the sizer

	    \return	TRUE in case of success, FALSE otherwise

	    Adds a control, generated by a SP_WDG_DialogBase derived widget container.
        The optional parameters follow the rules, as defined in the wxSizer class
        and are just passed by.
    */
    bool AddControl(wxSizer* p_pcSizer,
        int p_nOpt = 0,
        int p_nFlag = 0,
        int p_nBorder = 0);

    bool SetOrdering(unsigned int p_nOrdering);
    inline unsigned int GetOrdering() const
    {
    	return m_nOrdering;
    }
};

#endif // __SP_WDG_NOTEBOOKPAGE_H__
