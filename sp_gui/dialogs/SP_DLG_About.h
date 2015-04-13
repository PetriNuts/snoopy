//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2006/11/21 $
// Short Description: About Dialog
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_ABOUT_H__
#define __SP_DLG_ABOUT_H__

#include <wx/wx.h>
#include <wx/hyperlink.h> 



class SP_DLG_About: public wxDialog {

public:
    //! constructor
    SP_DLG_About (wxWindow *parent, int milliseconds = 0);

    //! destructor
    virtual ~SP_DLG_About ();

private:

};



#endif // __SP_DLG_ABOUT_H__

