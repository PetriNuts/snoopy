//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/04/28 12:27:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_DIALOGEXTENDEDGRAPHIC_H__
#define __SP_WDG_DIALOGEXTENDEDGRAPHIC_H__

#define __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/clrpicker.h>
#include <wx/statline.h>

#include "sp_gui/widgets/dialogs/SP_WDG_DialogBase.h"
#include "sp_gr/shapes/SP_GR_ExtendedDrawnShape.h"
#include "sp_gr/edges/SP_GR_ExtendedEdge.h"

class SP_Graphic;
class wxSpinCtrl;

/**	\brief	The SP_WDG_DialogExtendedGraphic: class

*/
class SP_WDG_DialogExtendedGraphic: public SP_WDG_DialogBase
{
private:
    DECLARE_EVENT_TABLE()
protected:
    SP_ListGraphic m_lGraphics;
	SP_Data* m_parent_tmp;
    void OnColourPen(wxColourPickerEvent& p_cEvent);
    void OnColourBrush(wxColourPickerEvent& p_cEvent);
    void OnDefaultPen(wxCommandEvent& p_cEvent);
    void OnDefaultBrush(wxCommandEvent& p_cEvent);

    void ConnectEvents();
    void DisconnectEvents();

    SP_DLG_ShapeProperties* m_pcDlg;

#ifndef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
    wxSpinCtrl* m_pcSpinCtrl;
	wxSpinCtrl* m_pcArrowLeftSpinCtrl
	wxSpinCtrl* m_pcArrowRightSpinCtrl
#endif
#ifdef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
	//Type ListBox Node
    wxListBox* m_pcListBox;
    wxArrayString m_pcListBoxValues;
	//Type ListBox Edge
    wxListBox* m_pcListBox_Edge;
    wxArrayString m_pcListBoxValues_Edge;
	//Arrowedge Listbox
	wxListBox* m_pcArrowLeftListBox;
	wxListBox* m_pcArrowRightListBox;
    wxArrayString m_pcArrowListBoxValues;
#endif

    wxColourPickerCtrl* m_pcButtonPen;
    wxColourPickerCtrl* m_pcButtonBrush;

	wxSpinCtrl* m_scWidth;
	wxSpinCtrl* m_scHeight;
	wxSpinCtrl* m_scThickness;
	wxSpinCtrl* m_scArrowLThickness;
	wxSpinCtrl* m_scArrowRThickness;
    bool m_bBrushSet;
    bool m_bPenSet;

public:
    SP_WDG_DialogExtendedGraphic(const wxString& p_sPage = wxT("Graphic"), unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max());
    virtual ~SP_WDG_DialogExtendedGraphic();

    virtual SP_WDG_DialogBase* Clone();

    virtual bool AddToDialog(const SP_ListAttribute* p_ptlAttributes,
        SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview) { return TRUE; }
    virtual bool AddToDialog(SP_ListGraphic* p_plGraphics,
        SP_DLG_ShapeProperties* p_pcDlg);

    virtual bool OnDlgOk();

    /* ACCESSORS */
};

#endif // __SP_WDG_DIALOGEXTENDEDGRAPHIC_H__
