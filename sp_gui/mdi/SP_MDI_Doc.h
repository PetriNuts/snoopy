//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_MDI_DOC_H__
#define __SP_MDI_DOC_H__

#include <wx/docview.h>
#include <wx/cmdproc.h>
#include <wx/string.h>

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Netclass.h"
#include "sp_gui/widgets/SP_WDG_GraphTreectrl.h"
#include "sp_gui/widgets/SP_WDG_CoarseTreectrl.h"
#include "sp_gui/dialogs/dia_CPN/SP_WDG_DeclarationTreectrl.h"//liu
#include "sp_gui/mdi/SP_MDI_Diagram.h"

#include "sp_core/SP_Set.h"

class SP_MDI_CoarseDoc;
class SP_DLG_SearchId;
class SP_DLG_ChooseInvariant;
class SP_DLG_ReduceFTree;

enum SP_DOCTYPE
{
    SP_DOC_SIMPLE,
    SP_DOC_COARSE
};

typedef set<SP_MDI_CoarseDoc*> SP_SetCoarseDoc;

class SP_MDI_Doc: public wxDocument
{
private:
    DECLARE_DYNAMIC_CLASS(SP_MDI_Doc)
    virtual void RefreshAllViews();
protected:
    SP_DS_Graph* m_pcGraph;
    SP_MDI_Diagram m_pcDiagram;
    SP_WDG_GraphTreectrl* m_pcGraphTreectrl;
    SP_WDG_CoarseTreectrl* m_pcCoarseTreectrl;
    SP_WDG_DeclarationTreectrl* m_pcColorSetTreectrl;    
	SP_DLG_SearchId* m_SearchIdDialog;
	SP_DLG_ReduceFTree* m_ReduceFTDialog;
	SP_DLG_ChooseInvariant* m_ChooseInvariantDialog;
    SP_DOCTYPE m_eType;

    //! used in CoarseDoc
    bool m_bClose;

    SP_MDI_Doc* m_pcParentDoc;
    wxString m_sNameSuffix;
	wxString m_sNetclassName;
	SP_SetCoarseDoc m_tsChildDocs;

	bool m_bCompress;

public:

    SP_MDI_Doc();
    virtual ~SP_MDI_Doc();

    virtual bool OnCloseDocument();

    virtual bool OnSaveDocument(const wxString& p_sFile);
    virtual bool OnOpenDocument(const wxString& p_sFile);
    virtual bool SaveAs();

	virtual void HarmonizeConstantsForColPN();//by George

    virtual SP_DS_Graph* InitGraph(SP_DS_Netclass* p_pcNetclass);
    virtual SP_DS_Graph* SetGraph(SP_DS_Graph* p_pcVal) { return m_pcGraph; }
    inline SP_DS_Graph* GetGraph() { return m_pcGraph; }

    SP_DLG_SearchId* GetSearchIdDialog();
	SP_DLG_ReduceFTree* GetReduceFTDialog();
	  SP_DLG_ChooseInvariant* GetChooseInvariantDialog();
    void ResetChooseInvariantColours();

    inline wxString GetNetclassName() { return m_sNetclassName; }
		unsigned int GetNetnumber();
    void SetNetnumber(unsigned int p_nVal);
    bool SetClose(bool p_bVal);

    virtual void Modify(bool p_bVal);
    void DoSetModify(bool p_bVal, SP_MDI_Doc* p_pcSender);

    inline SP_MDI_Diagram *GetDiagram() { return &m_pcDiagram; }

    bool SetEditElement(SP_Type* p_pcElement);

    virtual bool DrawAllElements(bool p_bLocalOnly = FALSE, bool p_bIncludeChildDocs = false);
    virtual void Refresh();
    virtual bool ShowAll();

    inline void SetGraphTreectrl(SP_WDG_GraphTreectrl* p_pcVal) { m_pcGraphTreectrl = p_pcVal; }
    inline void SetCoarseTreectrl(SP_WDG_CoarseTreectrl* p_pcVal) { m_pcCoarseTreectrl = p_pcVal; }
    inline void SetColorSetTreectrl(SP_WDG_DeclarationTreectrl* p_pcVal) { m_pcColorSetTreectrl = p_pcVal; }   
	inline SP_WDG_CoarseTreectrl* GetCoarseTreectrl() const { return m_pcCoarseTreectrl; }
	inline SP_WDG_DeclarationTreectrl* GetColorSetTreectrl() const { return m_pcColorSetTreectrl; }
    inline SP_DOCTYPE GetType() const { return m_eType; }
    inline SP_MDI_Doc* GetParentDoc() const { return m_pcParentDoc; }

    inline void AddChildDoc(SP_MDI_CoarseDoc* p_pcVal) { m_tsChildDocs.insert(p_pcVal); }
    inline void RemoveChildDoc(SP_MDI_CoarseDoc* p_pcVal) { m_tsChildDocs.erase(p_pcVal); }

    void SetFilename(const wxString& p_sName, bool p_bNotify = FALSE);
    virtual void SetNameSuffix(wxString& p_sVal) { m_sNameSuffix = p_sVal; }
    inline wxString& GetNameSuffix() { return m_sNameSuffix; }

    bool GetCompress() { return m_bCompress; }
    void SetCompress(bool p_bCompress) { m_bCompress = p_bCompress; }
};

#endif // __SP_MDI_DOC_H__
