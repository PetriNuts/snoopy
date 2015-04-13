/*
 * SP_WDG_GridCellChoiceEditor.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 05.03.2013
 * Short Description:
 */
//=================================================

#include "SP_WDG_GridCellChoiceEditor.h"
#include"wx/combobox.h"

SP_WDG_GridCellChoiceEditor::SP_WDG_GridCellChoiceEditor(const wxArrayString& choices,
                                                         bool allowOthers)
:wxGridCellChoiceEditor(choices,allowOthers)
{

}

SP_WDG_GridCellChoiceEditor::~SP_WDG_GridCellChoiceEditor()
{
}

void SP_WDG_GridCellChoiceEditor::AddItem(const wxString& p_sItem)
{
	m_choices.Add(p_sItem);

	//add the new group to the combobox
	if(Combo()!=NULL)
	{
		Combo()->Append(p_sItem);
	}

}

