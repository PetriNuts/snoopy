//////////////////////////////////////////////////////////////////////
// $Author: akurth $
// $Version: 0.1 $
// $Date: 2005/06/23 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTSYREPA_H__
#define __SP_EXPORTSYREPA_H__

#include "export/SP_ExportPT.h"
#include "sp_core/SP_List.h"
#include "sp_core/base/SP_Error.h"
#include "sp_ds/extensions/FTree/SP_DS_FTreeLogicalExpressionReader.h"


#include <wx/ffile.h>

class SP_DS_Node;
class SP_DS_Edge;
class SP_DS_Graph;

struct Basic
{
    Basic(const wxString&, const wxString&, SP_DS_Node*);
   wxString m_number;
  wxString m_name;
  SP_DS_Node* m_node;
};

typedef map<long, Basic*> BasicMap;

class SP_ExportSyRePa:  public SP_ExportPT
{
  protected:
  SP_MDI_Doc* m_doc;
  SP_DS_Graph* m_graph;
  wxString f_fileName;
  wxFFile f_file;
  wxFFile data_file;
   wxString data_fileName;
	NodeMap m_logExp;
  BasicMap listElements;
  //SP_DS_Node* root;

 /**
	\brief Check the semantic of the fault tree
	*/
  bool checkSemantic();
  virtual bool DoWrite();

  SP_DS_Node* SearchRoot();

  /**
	\brief Replace the names of the logical expression
	names of the gates and events will be renamed on the basis of the requirements
	of the tool SyRePa.
	*/
  bool ReplaceNames();

  
  /**
   \brief Save Failure Rates and Repair Repair Rates of basic/undeveloped events

  \param	basicMap	list of basic/undeveloped events
	*/
  bool ReadData(BasicMap &);

  void ClearMap();
 


 public:
  SP_ExportSyRePa();
  virtual ~SP_ExportSyRePa();
  virtual bool AcceptsDoc(SP_MDI_Doc*);
  virtual bool Write(SP_MDI_Doc*, const wxString&);
	
  inline virtual wxString GetName()
    { return wxT("Export to SyRePa");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Fault Tree in SyRePa Format");}
  virtual wxString GetExtension()
    { return wxT("TXT");}
  virtual wxString GetFileDescr()
    { return wxT("SyRePa Files");}

 public :

     
     
    

};


#endif // __SP_EXPORTSYREPA_H__

