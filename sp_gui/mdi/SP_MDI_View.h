//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_MDI_VIEW_H__
#define __SP_MDI_VIEW_H__

#include <wx/wx.h>
#include <wx/docview.h>

#include <wx/cmdproc.h>

#include "sp_core/SP_List.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_core/base/SP_Type.h"
#include "sp_core/base/SP_Graphic.h"

class SP_GUI_Childframe;
class SP_DS_Graph;

enum SP_VIEWTYPE
{
    SP_VIEW_SIMPLE,
    SP_VIEW_COARSE
};

class SP_MDI_View: public wxView
{
private:
    DECLARE_DYNAMIC_CLASS(SP_MDI_View)
    DECLARE_EVENT_TABLE()
    virtual void RefreshAllViews();
protected:
    SP_GUI_Childframe* m_pcFrame;
    SP_GUI_Canvas*  m_pcCanvas;
    unsigned int m_nNetnumber;
    SP_VIEWTYPE m_eType;
    //! used in CoarseView
    bool m_bClose;
    bool m_bFrameWasMaximized;
    //! check for selections of edges
    bool AddToSelectList(SP_Graphic* p_pcGraphic, SP_ListGraphic* p_plList, SP_ListGraphic* p_plList2);
    bool FindConnectedEdges(SP_ListGraphic* p_plEdges);

    //by sl
    void ColorEdges(SP_DS_Graph* l_pcGraph, list<wxString>& l_lPlaces, bool isChecked);

public:
	bool CheckCoarseConsistency(SP_ListGraphic* p_plShapes, SP_DS_Node* p_pcCoarseNode);
public:
    SP_MDI_View(void);
    ~SP_MDI_View(void);

    bool SetEditElement(SP_Type* p_pcElement);

    virtual bool OnCreate(wxDocument *p_pcDoc, long p_nFlags);
    virtual void OnDraw(wxDC *p_pcDc);
    virtual void OnUpdate(wxView *p_pcSender, wxObject *p_pcHint = NULL);
    virtual bool OnClose(bool p_bDeleteWindow = TRUE);
    virtual void OnSelectAll(wxCommandEvent& p_cEvent);
    virtual void OnEditProperties(wxCommandEvent& p_cEvent);
    virtual void OnTransformShapes(wxCommandEvent& p_cEvent);

    virtual void OnConvertElement(wxCommandEvent& p_cEvent);

    virtual void OnActivateView(bool activate, wxView *activeView,
				wxView *deactiveView);

    void OnUpdateUI(wxUpdateUIEvent& p_cEvent);
    void OnDeleteAll(wxCommandEvent& p_cEvent);
    void OnHide(wxCommandEvent& p_cEvent);
    void OnUnHide(wxCommandEvent& p_cEvent);
    void OnEditGlobalOptions(wxCommandEvent& p_cEvent);
    void OnSqueeze(wxCommandEvent& p_cEvent);
	void OnLayout(wxCommandEvent& p_cEvent);

    void OnCoarse(wxCommandEvent& p_cEvent);
    void OnFlatten(wxCommandEvent& p_cEvent);

    virtual void OnSelectAllClass(wxCommandEvent& p_cEvent);
    virtual void OnEditPropertiesClass(wxCommandEvent& p_cEvent);

    wxShape *FindFirstSelectedShape(wxClassInfo *p_pcToFind = NULL);

    inline unsigned int GetNetnumber() { return m_nNetnumber; }
    inline void SetNetnumber(unsigned int p_nVal) { m_nNetnumber = p_nVal; }
    virtual bool ShowFrame();
    inline SP_GUI_Childframe* GetFrame() const { return m_pcFrame; }
    virtual bool DrawAllElements(SP_DS_Graph* p_pcGraph, bool p_bLocalOnly = FALSE);
    inline bool GetFrameWasMaximized()  const { return m_bFrameWasMaximized; }

    inline SP_VIEWTYPE GetType() const { return m_eType; }
    inline bool GetClose() const { return m_bClose; }
    inline bool SetClose(bool p_bVal) { m_bClose = p_bVal; return TRUE; }

    inline void Modify(bool p_bVal) { if (GetDocument()) GetDocument()->Modify(p_bVal); }
    inline bool IsModified() const { return (GetDocument()?GetDocument()->IsModified():FALSE); }

    void Refresh(bool p_bRealRefresh = true);

    inline SP_GUI_Canvas* GetCanvas() const { return m_pcCanvas; }

    // Scans the canvas for selections
    void FindSelectedShapes(wxList& p_lSelections, wxClassInfo* p_pcToFind = NULL);
    void FindSelectedGraphics(SP_ListGraphic& p_lSelectedGraphics, bool p_bAllEdges);
	void FindSelectedElements(SP_ListData &p_lSelectedElements);
	void FindSelectedCoarseElements(SP_ListData &p_lSelectedElements, unsigned int p_nNetNr);
    //--------------------------------------------------------------------------------

    // Do a cut operation for the given list of shapes
    virtual bool DoDelete(SP_ListGraphic* p_plShapes);
    virtual bool DoRotate(SP_ListGraphic* p_plShapes, double p_nX, double p_nY, double p_nAngle = 90.0);
    virtual bool DoFlip(SP_ListGraphic* p_plShapes, double p_nX);
    virtual bool DoMirror(SP_ListGraphic* p_plShapes, double p_nY);
    virtual bool DoCopy(SP_ListGraphic* p_plShapes);
    virtual bool DoCopyInNewNet(SP_ListGraphic* p_plShapes);
    virtual bool DoPaste();
    virtual bool DoEditProperties(SP_ListGraphic* p_plElements, bool p_bDifferent);
    bool DoEditPropertiesClass(const wxString& p_sClass);
    bool DoFlatten(SP_ListGraphic* p_plShapes);
    bool DoCoarse(SP_ListGraphic* p_plShapes, double p_nX, double p_nY, const wxString& p_sCoarseNode = wxT(""));

    // Do a general command
    void DoCmd(wxList& p_lShapes, wxList& p_lOldShapes, int p_nCmd, const wxString& p_sOp);

    // Select or deselect all
    void SelectAll(bool p_bSelect = TRUE);
    // Select all of the actual class
    void SelectAllClass(const wxString& p_sClass);
    // Select all of list of graphic
    void SelectAllGraphics(const SP_ListGraphic& p_lGraphics, bool p_bSelect = true);

    //! Event handlers
    //! can be overridden by plugging in a new entry
    //! in derived classes' event tables
    void OnCut(wxCommandEvent& p_cEvent);
    void OnDelete(wxCommandEvent& p_cEvent);
    void OnCopy(wxCommandEvent& p_cEvent);
    void OnPaste(wxCommandEvent& p_cEvent);
    void OnToggleAnim(wxCommandEvent& p_cEvent);
    void DoToggleAnim(bool p_bEnable);

    //by sl
    void OnToggleMarking(wxCommandEvent& p_cEvent);



    void OnCopyInNewNet(wxCommandEvent& p_cEvent);

    void OnSearchId(wxCommandEvent& p_cEvent);
    void OnSortNodes(wxCommandEvent& p_cEvent);
    void OnCheckDuplicateNodes(wxCommandEvent& p_cEvent);
    void OnNetInformations(wxCommandEvent& p_cEvent);
    void OnZoom(wxCommandEvent& p_cEvent);
    void OnRefresh(wxCommandEvent& p_cEvent);
	//Fault Tree
	void OnReduceFTree(wxCommandEvent& p_cEvent);
	void OnTransformFTree(wxCommandEvent& p_cEvent);

	void OnLoadInvariantFile(wxCommandEvent& p_cEvent);

	void OnIASubMenuSelection(wxCommandEvent& p_cEvent);
	void OnIATriggerGeneral(wxCommandEvent& p_cEvent);
	void OnIATriggerSingle(wxCommandEvent& p_cEvent);
	void OnIATriggerMakro(wxCommandEvent& p_cEvent);
	void OnIAUndoLastAction(wxCommandEvent& p_cEvent);
	void OnIAConfigure(wxCommandEvent& p_cEvent);
	void OnIARegCmdList(wxCommandEvent& p_cEvent);
	void UpdateIASubMenu();

	void OnMDEditGeneral(wxCommandEvent& p_cEvent);

	void OnGenerateMasterNets(wxCommandEvent& p_cEvent);
	void OnGenerateTwinNets(wxCommandEvent& p_cEvent);
	void OnColorizeAnyNets(wxCommandEvent& p_cEvent);

	void OnStartSimulation(wxCommandEvent& p_cEvent);
};

#endif // __SP_MDI_VIEW_H__
