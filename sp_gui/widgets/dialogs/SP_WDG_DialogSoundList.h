//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dbayer $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2008/09/10 11:55:00 $
// Short Description: widget displays sound files and handles association
//					  to the transition
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_DIALOGSOUNDLIST_H__
#define __SP_WDG_DIALOGSOUNDLIST_H__

#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/filename.h>

#include <irrKlang.h>


#include "sp_gui/widgets/dialogs/SP_WDG_DialogBase.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"

class SP_WDG_DialogSoundList: public SP_WDG_DialogBase
	, public irrklang::ISoundStopEventReceiver

{

  private:

    wxString m_sOldCellValue;
    SP_DS_ColListAttribute* m_pcColList;

	irrklang::ISoundEngine* m_pcSoundEngine;
	irrklang::ISound* m_pcSound;

    DECLARE_EVENT_TABLE()

  protected:

    wxGrid* m_pcValueGrid;
	wxListBox* m_pcSoundBox;
	wxButton* m_pcSelectSoundButton;
	wxButton* m_pcPlaySoundButton;
	wxButton* m_pcStopSoundButton;

    SP_DLG_ShapeProperties* m_pcDlg;

	/** \brief loads all data from the attributes

		\param wxCommandEvent the event object
		\return success
	*/
    bool LoadData();

	/** \brief saves all the changes that were made to the attributes

		\param wxCommandEvent the event object
		\return success
	*/
    bool SaveData();

	/** \brief Opens a dialog that lets you see all transitions
		and sounds in a table

		\param wxCommandEvent the event object
	*/
    void OnOverview( wxCommandEvent& p_cEvent );

	/** \brief Opens a dialog that lets you select an algorithm
		thats applies sound to transitions automatically

		\param wxCommandEvent the event object
	*/
	void OnApplySound(wxCommandEvent& p_cEvent);

    void ConnectEvents();
    void DisconnectEvents();

	/** \brief Writes the name of the selected sound into a grid cell
		when the select button is clicked

		\param wxCommandEvent the event object
	*/
	void OnSelectSound(wxCommandEvent& p_cEvent);

	/** \brief starts playback of the selected file

		\param wxCommandEvent the event object
	*/
	void OnPlaySound(wxCommandEvent& p_cEvent);

	/** \brief stops playback of the selected file

		\param wxCommandEvent the event object
	*/
	void OnStopSound(wxCommandEvent& p_cEvent);

	/** \brief Enables the select, play and stop buttons when an
		element from the list is selected for the first time

		\param wxCommandEvent the event object
	*/
	void OnListBoxSelect(wxCommandEvent& p_cEvent);

	/** \brief Invoked when playback has stopped. This enables the
		play button again and disables the stop button
	*/
	virtual void OnSoundStopped (irrklang::ISound* sound, irrklang::E_STOP_EVENT_CAUSE reason, void* userData);

	/** \brief Returns all supported audio files in the given directory
		and its subdirectories

		\param p_Dir the full name and path of the directory from which to start the search
		\return wxArrayString an array containing all file names (relative to library)
	*/
	wxArrayString GetSoundsFromDir(wxString& p_Dir);

	/** \brief checks if a file is supported by the sound library

		\param p_fileName name of the file
		\return true if supported
	*/
	bool IsSupportedFile(wxFileName& p_fileName);

  public:

    SP_WDG_DialogSoundList( const wxString& p_sPage = wxT("Page 1"), unsigned int p_nOrdering = std::numeric_limits<unsigned int>::max() );
    virtual ~SP_WDG_DialogSoundList();

    virtual SP_WDG_DialogBase* Clone();

	/** \brief This adds controls to the widgets,
		that allow the user to attach sounds

		\param p_ptlAttributes attributes of the SP_DATA object
		\param p_pcDlg attributes of the SP_DATA object
	*/
    virtual bool AddToDialog( const SP_ListAttribute* p_ptlAttributes,
      SP_DLG_ShapeProperties* p_pcDlg, bool p_bOverview );

    virtual bool OnDlgOk();

    void OnGridCellValueChanged( wxGridEvent& ev );
    void OnGridCellSelected( wxGridEvent& ev );

};

#endif // __SP_WDG_DIALOGSOUNDLIST_H__
