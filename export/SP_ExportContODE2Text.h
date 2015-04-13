/*
 * SP_ExportContODE2Text.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 12.8.2010
 * Short Description:
 */
 //==============================================================================

#ifndef SP_EXPORTCONTODE2TEXT_H_
#define SP_EXPORTCONTODE2TEXT_H_

#include "export/SP_ExportPT.h"

/*
 * Export the continuous Petri Net's ODE to a text file
 */
class SP_ExportContODE2Text : public SP_ExportPT
{
  public:
	        bool DoWrite();

	        bool AcceptsDoc(SP_MDI_Doc* p_doc);

	        inline virtual wxString GetName()
		        { return wxT("Export ODEs to Text");}

	        inline virtual wxString GetDescr()
		        { return wxT("Export ODEs to Text File");}

	        virtual wxString GetExtension()
		        { return wxT("txt");}

	        virtual wxString GetFileDescr()
		        { return wxT("Text Files");}

	        SP_ExportContODE2Text();
		    virtual ~SP_ExportContODE2Text();

};

#endif /* SP_EXPORTCONTODE2TEXT_H_ */
