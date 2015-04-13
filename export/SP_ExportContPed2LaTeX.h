//////////////////////////////////////////////////////////////////////
// $Author: dscheibl $
// $Version: 0.2 $
// $Date: 2005/08/09 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORT_CONTPED2LATEX__
#define __SP_EXPORT_CONTPED2LATEX__

#include "sp_core/SP_List.h"
#include "sp_core/SP_Map.h"
#include "sp_core/base/SP_Error.h"
#include "export/SP_ExportRoutine.h"
#include "sp_core/base/SP_Error.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/continuous/SP_DS_PlaceODE.h"
#include "sp_ds/extensions/continuous/SP_DS_EquationParser.h"
#include "sp_gui/dialogs/SP_DLG_SaveContinuousEquationProperties.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include <wx/ffile.h>
#include <wx/valgen.h>


class SP_DS_Node;
class SP_DS_Edge;
class SP_DS_Graph;

class SP_ExportContPed2LaTeX : public SP_ExportRoutine, public SP_Error
{
 protected:
  wxWindow* m_pcParent;
  bool m_bGreek;
  bool m_bTabular;
  bool m_bFull;
  bool m_bBold;
  bool m_bConst;
  bool m_bSort;
  bool m_bParentheses;
  bool m_bConvSpecialChar;
  wxCheckBox* m_pcSaveLaTeXTabularCheckBox;
  wxCheckBox* m_pcSaveLaTeXGreekCheckBox;
  wxCheckBox* m_pcSaveLaTeXFullCheckBox;
  wxCheckBox* m_pcSaveLaTeXBoldCheckBox;
  wxCheckBox* m_pcSaveLaTeXConstCheckBox;
  wxCheckBox* m_pcSaveLaTeXSortCheckBox;
  wxCheckBox* m_pcSaveLaTeXParenthesesCheckBox;
  wxCheckBox* m_pcSaveLaTeXConvSpecialCharCheckBox;
  unsigned int m_nSaveEquationProperties;
  wxRadioBox* m_pcSaveEquationPropertiesChoice;

  SP_MDI_Doc* m_doc;
  SP_DS_Graph* m_graph;
  wxString m_fileName;
  wxFFile m_file;

  SP_DS_EquationParser* ConstructODE(SP_DS_Node* p_pcNode);
  wxString EquationEdge(SP_DS_Node* p_pcNode,
                        SP_DS_Edge* p_pcEdge);

  SP_DS_EquationParser* ConstructEducts(SP_DS_Node* p_pcNode);
  SP_DS_EquationParser* ConstructProducts(SP_DS_Node* p_pcNode);
  wxString ProductEdge(SP_DS_Node* p_pcNode,
                       SP_DS_Edge* p_pcEdge);


  wxString EquationToTexString(const wxString& p_sODEstr,
                               const wxString& p_sName = wxT(""),
                               bool p_bGreek = FALSE,
                               bool p_bTabular = FALSE,
                               bool p_bBold = FALSE,
                               bool p_bTrans = FALSE,
                               bool p_bParentheses = FALSE);
  wxString ValueToTexString(const wxString& p_sName = wxT(""),
                            const wxString& p_sValue = wxT("0"),
                            bool p_bConst = FALSE,
                            bool p_bGreek = FALSE,
                            bool p_bTabular = FALSE,
                            bool p_bBold = FALSE,
                            bool p_bTrans = FALSE,
                            bool p_bParentheses = FALSE);

  wxString FormatListValues(SP_DS_ColListAttribute* p_pcListAttr = NULL);

  wxString DoWrite(const wxString& p_sODEstr,
                   const wxString& p_sName = wxT(""),
                   const wxString& p_sValue = wxT("0"),
                   bool p_bTrans = FALSE);
  wxString DoWrite(SP_DS_EquationParser* p_pcEductParser,
                   SP_DS_EquationParser* p_pcProductParser,
                   const wxString& p_sName = wxT(""),
                   bool p_bReversible = FALSE);

  virtual bool DoWrite();

  const wxString ToTexString(const wxString l_pVarName,bool p_bGreek,bool p_bParentheses,bool p_bConvSpecialChar);
  wxString ConvertGreek(const wxString& p_sName, bool p_bConvSpecialChar);
  wxString FormatTexString(const wxString& p_sName, bool p_bConvSpecialChar);

 public:
  SP_ExportContPed2LaTeX();
  virtual ~SP_ExportContPed2LaTeX();

  bool AddToSaveContinuousEquationDialog(SP_DLG_SaveContinuousEquationProperties* p_pcDlg,
                                         wxSizer* p_pcSizer);
  bool OnSaveContinuousEquationPropertiesOK();


  virtual bool AcceptsDoc(SP_MDI_Doc*);

  virtual bool Write(SP_MDI_Doc*, const wxString&);
  inline virtual wxString GetName()
    { return wxT("Export ODEs to LaTeX");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net in LaTeX Format");}
  virtual wxString GetExtension()
    { return wxT("tex");}
  virtual wxString GetFileDescr()
    { return wxT("LaTeX Files");}

  public :

    
    
    

};
#endif // __SP_EXPORT_CONTPED2LATEX__
