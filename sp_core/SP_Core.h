//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_CORE_H__
#define __SP_CORE_H__

#include "sp_gui/widgets/dialogs/SP_WDG_DialogBase.h"
#include "sp_ds/SP_DS_Netclass.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Animator.h"
#include "sp_core/base/SP_Graphic.h"
#include "sp_gui/management/SP_GM_Docmanager.h"

#include <wx/wx.h>
#include <wx/ogl/ogl.h>

extern unsigned int GetPasteNetnumber(SP_MapUInt2UInt* p_pmNet2Net, unsigned int p_nNumber, SP_DS_Graph* p_pcGraph);

typedef map<wxString, SP_ListAnimator > SP_MapString2ListAnimator;
typedef map<wxString, SP_DS_Netclass*> SP_MapString2Netclass;
typedef map<wxShape*, SP_Graphic*> SP_MapExternObjects;
typedef map<wxString, SP_WDG_DialogBase*> SP_MapString2DialogWidget;
typedef map<SP_Data*, SP_ListGraphic> SP_MapData2ListGraphic;
typedef map<SP_Data*, SP_Graphic*> SP_MapData2Graphic;
typedef map<wxString, SP_DS_Animation*> SP_MapString2Animation;
typedef map<SP_MDI_Doc*, bool> SP_MapAnimationRestart;

class SP_Core;
class SP_MDI_Doc;
class SP_DS_Animation;
class SP_DLG_Animation;

/**	\brief	Core class managing data

  There's only one object during the lifetime of the whole application, storing
  most of the data and making it available nearly everywhere, just by calling the
  static method SP_Core::Instance (singleton)

  Important members are the list of netclasses (m_mNetclasses), the mapping between
  objects from within the OGL and our own SP_Graphic derived ones (m_mExternObjects)
  and everything, that deals with the copy and paste mechanism.
*/
class SP_Core: public SP_Error
{
private:
    /**	\brief the list of netclasses

        Filled by SP_Core::InitialiseNetclasses and holding the SP_DS_Netclass
        derived objects, the user can access while running the application.
    */
    SP_MapString2Netclass m_mNetclasses;

    /**	\brief the Instance of this class, created by a protected constructor
    */
    static SP_Core* m_pcInstance;

    /**	\brief Mapping from OGL objects to Snoopy objects

    This map helps to find the proper snoopy object from within the OGL code.
    */
    SP_MapExternObjects m_mExternObjects;

    /** \brief map of registered dialog widgets to show attributes in dialogs

        The map is indexed by the type-id, every attribute can have an own one
    */
    SP_MapString2DialogWidget m_mDialogWidgets;

    /**	\brief  temporary list of elements, that are displayed in the properties
                dialog
    */
    SP_ListGraphic m_lDialogGraphics;

    /**	\brief map of lists, holding elements of the DS and some or all of its graphics

        This map is used to collect elements of a graph for later deletion. See
        SP_MDI_View::DoCut and SP_DS_Graph::RemoveElements
    */
    SP_MapData2ListGraphic m_pmlDeleteDataGraphic;

    SP_MDI_Doc* m_pcActiveRootDocument;

    /**	\name Copy and paste protected members

        The following members manage the data for copy and paste operations in
        the application.
    */
    //@{
    unsigned int m_nCopyFromNetnumber;
    unsigned int m_nPasteOffset;
    SP_DS_Graph* m_pcCopyGraph;
    /**	\brief	The ResetCopyState function

        \param p_pcGraph    graph where the copy command started from
	    \return	bool

	    Clears all datastructures, involved in Copy'n'paste and builds the new temp graph.
        Usually called right before any new work in SP_Core::Copy.
    */
    bool ResetCopyState(SP_DS_Graph* p_pcGraph);
    //@}

    /** \name Animation specific
    */
    //@{
    SP_MapString2ListAnimator  m_mlAnimatorRegistry;
    SP_MapString2Animation m_mAnimations;

    bool m_bAnimMode;
    SP_DLG_Animation* m_pcAnimControl;
    SP_DS_Animation* m_pcAnimation;
    //@}

	SP_MapAnimationRestart m_mRestartAnimMode;

    bool m_bDocumentReactivatingFlag;

    bool m_bSimulationMode;

protected:
    /**	\brief	Constructor, intentionally declared as protected

    This method is only called from within this class (or derived ones) to ensure, only one instance is created
    */
	SP_Core();

public:
	~SP_Core();

    /**	\brief	Static member function to be called to get the (singleton) SP_Core object

        Either constructs the object or just returns the already created one.

        \return	returns the singleton instance of type SP_Core
    */
    static SP_Core* Instance();

    /**	\brief	The InitialiseNetclasses function

	    \param	p_pcDocmanager	The docmanager to be used while running the application

	    \return	bool

	    This method is called from within Snoopy::OnInit and is the right place to
        put your stuff for your own netclasses in.

        Just create a new object of your netclass and call SP_DS_Netclass::AddToGui.
        In case of success, the new netclass should be added to the list of netclasses
        available to work with from now on.
    */
    bool InitialiseNetclasses(SP_GM_Docmanager* p_pcDocmanager);

    /**	\brief	The AddNetclass function

	    \param	p_pcParam	the netclass to be added to the list of netclasses

	    \return	the added netclass or NULL in case of an error

        This function tests, whether the name is unique, compared to the names, already
        stored in the list of netclasses.
    */
    SP_DS_Netclass* AddNetclass(SP_DS_Netclass* p_pcParam);
    /**	\brief	The GetNetclass function

	    \param	p_pchName	name of the netclass to be returned

	    \return	the first netclass with that name in the list, or NULL in case of an error
    */
    SP_DS_Netclass* GetNetclass(const wxString& p_pchName);

		int GetNetclassCount();
		wxString GetNetclassName(int p_nItem);

    /**	\brief	The RegisterDialogWidget function

	    \param	p_sNameType	    Combination of name'|'type of the attribute
	    \param	p_pcWidget	    the widget to register

	    \return	the 2nd parameter

	    This function inserts the widget into the map under the given name_type
        as key. Old entries will be removed, so that all types of attributes
        will have exactly one way to be displayed in a dialog.
    */
    SP_WDG_DialogBase* RegisterDialogWidget(const wxString& p_sNameType, SP_WDG_DialogBase* p_pcWidget);
    /**	\brief	The RegisterDialogWidget function

        \param  p_pcData        object that wants to be displayed using this widget
	    \param	p_pcWidget	    the widget to register

	    \return	the last parameter

	    Compiles the proper name'|'type and calls RegisterDialogWidget(string, widget)
    */
    SP_WDG_DialogBase* RegisterDialogWidget(SP_DS_Attribute* p_pcData, SP_WDG_DialogBase* p_pcWidget);
    /**	\brief	The GetDialogWidget function

	    \param	p_sNameType	    The attribute name'|'type to get the widget for

	    \return	a dialog widget

    */
    SP_WDG_DialogBase* GetDialogWidget(const wxString& p_sNameType);
    /**	\brief	The GetDialogWidget function

	    \param	p_pcData	    The attribute to get the widget for

	    \return	a dialog widget

    */
    SP_WDG_DialogBase* GetDialogWidget(SP_DS_Attribute* p_pcData);
   /**	\brief	The MakeDialogWidgetKey function

	    \param	p_pcData	    The attribute to get the widget for

	    \return	the dialog widget key

    */
    wxString GetDialogWidgetKey(SP_DS_Attribute* p_pcData);
    /**	\brief	The AddDialogGraphic function

	    \param	p_pcVal	graphic object to add to the dialog

	    \return	bool

	    This method is called from within the SP_DLG_ShapeProperties::AddDialogGraphic
        nethod and just stores all objects in the m_lDialogGraphics member list
        for later access.
    */
    bool AddDialogGraphic(SP_Graphic* p_pcVal);
    /**	\brief	The GetDialogGraphicList function

	    \return	SP_ListGraphic* pointer to the m_lDialogGraphics member

	    Returns the list of graphical objects, registered for beeing displayed
        in the next to be opened properties dialog.
    */
    SP_ListGraphic* GetDialogGraphicList() { return &m_lDialogGraphics; }
    /**	\brief	The ClearDialogGraphicList function

	    \return	bool

	    Clears the m_lDialogGraphics list member.
    */
    bool ClearDialogGraphicList() { m_lDialogGraphics.clear(); return TRUE; }

    /**
     * return the current active root document
     * @return
     */
    inline SP_MDI_Doc* GetRootDocument() const { return m_pcActiveRootDocument; }
    /**
     * set and return a new root document
     * @param p_pcVal
     * @return
     */
    inline SP_MDI_Doc* SetRootDocument(SP_MDI_Doc* p_pcVal) { m_pcActiveRootDocument = p_pcVal; return m_pcActiveRootDocument; }
	/**
	 * manages a document change obeying possibly active animation mode
	 *
	 * @param p_pcDoc
	 * @return
	 */
    bool ManageDocChange(SP_MDI_Doc* p_pcDoc);
    /**
     * removes a closed document from the local map m_mRestartAnimMode
     * @param p_pcDoc
     */
    inline void RemoveFromDocChange(SP_MDI_Doc* p_pcDoc) { m_mRestartAnimMode.erase(p_pcDoc); }
    /**	\brief	The AssociateExtern function

	    \param	p_pcKey	the pointer to the object, used in the 'real' graphic engine (OGL)
	    \param	p_pcVal	the object, associated with the key, derived from SP_Graphic

	    \return	bool

	    All objects created from within the OGL are only 'loose' connected to
        the application specific. This connection is held in the m_mExternObjects
        member list.
    */
    bool AssociateExtern(wxShape* p_pcKey, SP_Graphic* p_pcVal);
    /**	\brief	The RemoveExtern function

	    \param	p_pcKey	the pointer to the OGL object to be removed

	    \return	bool TRUE if the m_mExternObjects member is empty, FALSE otherwise

	    Remove the given key from the m_mExternObjects list, if in.
    */
    bool RemoveExtern(wxShape* p_pcKey);
    /**	\brief	The ResolveExtern function

	    \param	p_pcKey	object from within the OGL to get the Snoopy-object from

	    \return	SP_Graphic* the object associated with the param, or NULL

	    Returns the SP_Graphic object, associated with the OGL object just
        by looking up the m_mExternObjects member list.
        The returned object pointer should be valid...
    */
    SP_Graphic* ResolveExtern(wxShape* p_pcKey);

    /**	\brief	The AddToDeleteQueue function

	    \param	p_pcGraphic	pointer to the graphic object to be added

	    \return	bool

        Adds the parameter to the m_pmlDeleteDataGraphic map of lists member and calls
        SP_Graphic::SetDelete to mark it beeing deleted within the next call
        to SP_Core::RemoveQueuedElements.
        The m_pmlDeleteDataGraphic member holds a map of SP_Data derived objects and
        all their graphical representations, to be deleted.
    */
    bool AddToDeleteQueue(SP_Graphic* p_pcGraphic);
    /**	\brief	The RemoveQueuedElements function

	    \return	bool

	    Iterates over the m_pmlDeleteDataGraphic map of lists member and calls
        every key (SP_Data object) with its SP_Data::RemoveGraphic method and
        passes all graphics in the map, associated with this member.

        IMPORTANT!
        This method fills a list of pointers to all the SP_Data objects, that
        doesn't have any more graphics associated afterwards and calls 'delete'.
    */
    bool RemoveQueuedElements(bool p_bNoCallRemove = FALSE);

    /**	\name Copy and paste public members

        The following functions are for public access for copy and paste operations
        and usually called from within an object of SP_DS_Graph
    */
    //@{
    /**	\brief	Collect all data, needed to paste the objects afterwards

	    \param	p_pcGraph	the graph, the operation originates from
        \param  p_nNr       net number we started the copy operation on
	    \param	p_plShapes	list of pointers to SP_Graphic objects

	    \return	bool

	    The list contains pointers to SP_Graphic derived objects and is the
        filtered list of nodes (first) and edges (if any) compiled together
        from within SP_MDI_View::OnCopy.

        The list is always expected to have all 'nodes' queued in before any
        'edges' to ensure, that we find the associations between copied objects
        and copies for the source and target members of lines if we have to.
    */
    bool Copy(SP_DS_Graph* p_pcGraph, unsigned int p_nNr, SP_ListGraphic* p_plShapes);
    /**	\brief	Try to paste all collected data into the canvas

	    \param	p_pcGraph	graph object to paste the stored stuff into
	    \param	p_pcCanvas	the canvas, the paste command originated

	    \return	bool

	    Paste means, taking all the stored copies and try to add them to the
        graph managing the net numbers and show properties right.

        Additional care is taken off the 'proper' class of the graph, we are
        pasting in.
    */
    bool Paste(SP_DS_Graph* p_pcGraph, SP_GUI_Canvas* p_pcCanvas);
    //@}

    bool GetAnimRunning() const;
    void SetAnimRunning(bool p_bVal);
    inline bool GetAnimMode() const { return m_bAnimMode && m_pcAnimation; }
    SP_DS_Animation* GetAnim() const {return m_pcAnimation; }
    void SetAnimMode(bool p_bVal, SP_DS_Graph* p_pcGraph);
    bool CleanUpAnimation();

    void SetSimulationMode(bool p_bVal);
    bool GetSimulationMode() const;

    bool RegisterAnimator(const wxString& p_sKey, SP_DS_Animator* p_pcAnimator);
    bool RegisterAnimator(SP_DS_Node* p_pcNode, SP_DS_Animator* p_pcAnimator);
    bool RegisterAnimator(SP_DS_Edge* p_pcEdge, SP_DS_Animator* p_pcAnimator);
    bool RegisterAnimator(SP_ElementClass* p_pcClass, SP_DS_Animator* p_pcAnimator);
    SP_ListAnimator* GetAnimators(SP_ElementClass* p_pcClass);
    bool RegisterAnimation(SP_DS_Netclass* p_pcNetclass, SP_DS_Animation* p_pcAnimation);
    bool RegisterAnimation(const wxString& p_sKey, SP_DS_Animation* p_pcAnimation);
    SP_DS_Animation* GetAnimation(SP_DS_Graph* p_pcGraph);

	void ActivateAnimBAsedOnOptionSet(SP_DS_Graph* p_pcGraph,const int& p_nRef, const int&p_nDur);//by george 30.11.2020
	wxString GetActivatedRefreshDurationanim();//by george 30.11.2020

    /**	\brief	Avoid the reactivating of the last document

    */
    void SetDocumentReactivatingFlag( bool p_bFlag ) { m_bDocumentReactivatingFlag = p_bFlag; }
    bool GetDocumentReactivatingFlag() { return m_bDocumentReactivatingFlag; }

};

#endif // __SP_CORE_H__
