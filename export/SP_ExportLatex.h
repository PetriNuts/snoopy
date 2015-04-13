//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2011/12/22 $
// Short Description: export to latex
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORT_LATEX__
#define __SP_EXPORT_LATEX__

#include "export/SP_ExportRoutine.h"
#include "sp_core/base/SP_Error.h"

#include "sp_gui/dialogs/SP_DLG_ExportProperties.h"
#include "sp_gui/widgets/SP_WDG_CoarseTreectrl.h"
#include <wx/wx.h>
#include <wx/rearrangectrl.h>
#include <wx/event.h>

#include <wx/dc.h>
#include <wx/cmndata.h>


class SP_ExportLatex : public wxEvtHandler,
	public SP_ExportRoutine, public SP_Error
{
 public:
  SP_ExportLatex();
  virtual ~SP_ExportLatex() {}

  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);

  inline virtual wxString GetName()
    { return wxT("Export to LaTex");}
  inline virtual wxString GetDescr()
    { return wxT("Export document in LaTex Format");}
  virtual wxString GetExtension()
    { return wxT("tex");}
  virtual wxString GetFileDescr()
    { return wxT("LaTex Files");}

  public :

    virtual bool AddToDialog(SP_DLG_ExportProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc);
    virtual bool OnDialogOk();

    void LoadData();

    //General tab
    void AddGeneral();
    void OnClickGeneral(wxCommandEvent& p_cEvent);
    void OnClickGeneratePDF(wxCommandEvent& p_cEvent);

    //Basics tab
    void AddBasics();
    void AddAttributes_BasicsGeneral();
    void AddAttributes_BasicsNet();
    void AddAttributes_BasicsLayout();
    void AddAttributes_BasicsTypography();

    void Basics_Move(wxCommandEvent& p_cEvent);
    void Basics_UpdateUI(wxUpdateUIEvent& p_cEvent);
    void OnSelChange_Basics(wxCommandEvent& p_cEvent);
    void UpdateRightPanel_Basics(int id);


    //Graph Elements tab
    void AddGraphElements();
    bool AddAttributes_GraphElements();

    void Graph_Move(wxCommandEvent& p_cEvent);
    void Graph_UpdateUI(wxUpdateUIEvent& p_cEvent);
    void OnSelChange_Graph(wxCommandEvent& p_cEvent);
    void UpdateRightPanel_Graph(int id);

    //Declarations tab
    void AddDeclarations();
    bool AddAttributes_Declarations();

    void Declarations_Move(wxCommandEvent& p_cEvent);
    void Declarations_UpdateUI(wxUpdateUIEvent& p_cEvent);
    void OnSelChange_Declarations(wxCommandEvent& p_cEvent);
    void UpdateRightPanel_Declarations(int id);

    //Hierarchy tab
    void AddHierarchy();
    void copyTree_recur(const wxTreeItemId& x, const wxTreeItemId& y, wxString label, int id);


    void OnSelectClearAllItems_Graph(wxCommandEvent& p_cEvent);
    void OnSelectClearAllItems_Dec(wxCommandEvent& p_cEvent);

    bool StartDoc(const wxString& p_fileName);
    bool WriteLatex();
    bool WriteTitlePage();
    bool WriteBasics();
    bool WriteGraphElements();
    bool WriteDeclarations();
    bool WriteHierarchy();
    bool WriteReferences();

    void EndDoc();

    wxString EditStringforLatex(wxString filename, bool remove_space = true);
    bool SetUpPrintData( wxPrintData& pd, const wxString& p_fileName);

    wxTreeItemId FindTreeItemRec(const wxTreeItemId& p_Id, wxString label);
    void WriteHierarchylevelRec(const wxTreeItemId& tree_item, FILE* l_pstream);

    protected:

    FILE*             m_pstream;    //  output stream
    wxString          m_title;
    wxPrintData       m_printData;
    wxString m_sFilePath;

    float m_scale;
    int m_resolution;

    int m_bitmapCount;


    private:

    SP_WDG_NotebookPage* m_pcNotebookPageGeneral;
  	SP_WDG_NotebookPage* m_pcNotebookPageBasics;
  	SP_WDG_NotebookPage* m_pcNotebookPageGraph;
  	SP_WDG_NotebookPage* m_pcNotebookPageDeclarations;
  	SP_WDG_NotebookPage* m_pcNotebookPageHierarchy;

  	//graph elements mapping
    map<int, wxString> SP_Index2Node;
    map<int, wxArrayString > SP_Node2Attributes;
    map<int, wxSizer* > SP_Node2Sizer;

    map<wxString, wxArrayString > SP_Nodeclass2NodeList;
    map<int, wxCheckBox* > SP_Node2SelectClearAllCheckBox;
    map<int, wxCheckListBox* > SP_Node2NodeCheckList;
    map<int, wxCheckListBox* > SP_Node2AttributeCheckList;

    //declarations mapping
    map<int, wxString> SP_Index2DecNode;
    map<int, wxArrayString > SP_DecNode2Attributes;
    map<int, wxSizer* > SP_DecNode2Sizer;

    map<wxString, wxArrayString > SP_DecNodeclass2NodeList;
    map<int, wxCheckBox* > SP_DecNode2SelectClearAllCheckBox;
    map<int, wxCheckListBox* > SP_DecNode2NodeCheckList;
    map<int, wxCheckListBox* > SP_DecNode2AttributeCheckList;

    map<wxString, wxString> SP_HierarchyLabel2ID;

    map< pair<int, wxString>, wxString> SP_LatexReferencesIndex2Ref;

    map<wxString, SP_DS_Node*> SP_Name2Node;
    map<wxString, SP_DS_Edge*> SP_Name2Edge;
    map<wxString, SP_DS_Metadata*> SP_Name2Metadata;
    map<wxString, wxString> SP_NetNumber2Label;

    /////////
    SP_DS_Graph* m_pcGraph;
    SP_DLG_ExportProperties* p_pcDlg;
    SP_MDI_Doc* p_pcDoc;
    SP_DS_Metadata* m_pcMeta;
    SP_WDG_CoarseTreectrl* m_pcCoarseTreectrl;
    wxTreeCtrl* l_pcTree;

    wxString NetClass;
   // wxArrayString arrays;
    int m_nSelectionId_Graph;
    int m_nSelectionId_Declarations;

    wxSizer* m_pcMainSizerGeneral;
    wxSizer* m_pcSizerGeneratePDF;
    wxString m_sPDFname;

    wxArrayString tabs_items;
    wxArrayInt order;

    wxCheckListBox* checklist_general;
    wxCheckListBox* checklist_info;

    wxSizer* sizer1;
    wxSizer* sizer2;

    //General tab
    wxFilePickerCtrl* l_pcFilePickerCtrl1;
    wxFilePickerCtrl* l_pcFilePickerCtrl2;

    //Basics tab///////////////////////////
    wxSizer* m_pcMainSizer_Basics;
    wxSizer* m_pcLeftSizer_Basics;
    wxSizer* m_pcRightSizer_Basics;

    //right panel sizers
    wxSizer* m_pcSizer_Basics_GeneralInfo;
    wxSizer* m_pcSizer_Basics_NetInfo;
    wxSizer* m_pcSizer_Basics_ReportLayout;
    wxSizer* m_pcSizer_Basics_ReportTypography;

    wxArrayString l_pcOptions_basics;
    wxArrayInt l_pcOptions_basics_order;
    wxRearrangeList* m_pcRearrangelist_basics;
    wxButton* l_pcButtonUp;
    wxButton* l_pcButtonDown;

    wxArrayString m_Basics_General_Strings;
    wxArrayString m_Checklist_Basics_Net;

    wxArrayString m_Basics_FontFamily_Strings;
    wxArrayString m_Basics_FontSize_Strings;
    wxArrayString m_Basics_FontStyle_Strings;
    wxArrayString m_Basics_PaperSize_Strings;


    //Graph Elements tab//////////////////////
    wxSizer* m_pcMainSizer_Graph;
    wxSizer* m_pcLeftSizer_Graph;
    wxSizer* m_pcRightSizer_Graph;

    wxArrayString m_Options_Graph;
    wxArrayInt m_Options_Graph_order;

    wxRearrangeList* m_pcRearrangelist_Graph;
    wxButton* m_pcGraph_ButtonUp;
    wxButton* m_pcGraph_ButtonDown;

    //Declarations tab/////////////////////////
    wxSizer* m_pcMainSizer_Declarations;
    wxSizer* m_pcLeftSizer_Declarations;
    wxSizer* m_pcRightSizer_Declarations;

    wxArrayString m_Options_Declarations;
    wxArrayInt m_Options_Declarations_order;

    wxRearrangeList* m_pcRearrangelist_declarations;
    wxButton* m_pcDeclarations_ButtonUp;
    wxButton* m_pcDeclarations_ButtonDown;

    //////////////

    wxComboBox* m_pcComboBox_PaperSize;
    wxComboBox* m_pcComboBox_FontStyle;
    wxComboBox* m_pcComboBox_FontFamily;
    wxComboBox* m_pcComboBox_FontSize;

    wxString InputNetname;
    vector<wxCheckBox*> m_pcCheckBox_BasicsLayout;
    vector<wxCheckBox*> m_pcCheckBox_BasicsGeneral;
    vector<wxCheckBox*> m_pcCheckBox_BasicsNet;
    vector<wxTextCtrl*> m_pcTextCtrlHeaderFooter;

};
#endif // __SP_EXPORT_LATEX__
