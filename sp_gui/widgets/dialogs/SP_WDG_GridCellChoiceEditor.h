/*
 * SP_WDG_GridCellChoiceEditor.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 05.03.2013
 * Short Description:
 */
//=================================================


#ifndef SP_WDG_GRIDCELLCHOICEEDITOR_H_
#define SP_WDG_GRIDCELLCHOICEEDITOR_H_

#include <wx/grid.h>

/*
 * Wrapper class to wxGridCellChoiceEditor to allow adding new item
 * to the list of choices after the combobox is initialized.
 */
class SP_WDG_GridCellChoiceEditor : public wxGridCellChoiceEditor
{
public:
	    SP_WDG_GridCellChoiceEditor(const wxArrayString& choices,
                                    bool allowOthers = false);
	    virtual ~SP_WDG_GridCellChoiceEditor();

	    virtual void AddItem(const wxString& p_sItem);
};

#endif /* SP_WDG_GRIDCELLCHOICEEDITOR_H_ */
