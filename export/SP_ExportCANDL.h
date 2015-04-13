//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2012/01/06 $
// Short Description: export to CANDL
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTCANDL__
#define __SP_EXPORTCANDL__

#include "export/SP_ExportPT.h"

class SP_ExportCANDL: public SP_ExportPT
{
 private:
	struct PWeights
	{
		unsigned vpt_; // PT weight met
		unsigned vtp_; // TP weight met
		unsigned vi_; // for inhibitor arcs
		unsigned vr_; // for read arcs
		unsigned ve_; // for equal arcs
		bool reset_; // for reset arcs
		bool modifier_; // for modifier arcs
		bool first_; // first is initially true, means no value has been changed
		PWeights():
			vpt_(0),
			vtp_(0),
			vi_(SP_INFINITY),
			vr_(0),
			ve_(0),
			reset_(false), modifier_(false),first_(true)
		{}

	};
	typedef map<long, PWeights> SP_MapWeights;

 protected:
  bool WritePlaces();
  bool WriteTransitions();
  bool WriteConstants();
  bool WriteColorsets();
  bool WriteVariables();
  bool WriteFunctions();
  bool WriteObservers();
  virtual bool DoWrite();

 public:
  SP_ExportCANDL();
  virtual ~SP_ExportCANDL();

  virtual bool AcceptsDoc(SP_MDI_Doc*);

  inline virtual wxString GetName()
    { return wxT("Export to CANDL ");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net in CANDL Format");}
  virtual wxString GetExtension()
    { return wxT("candl");}
  virtual wxString GetFileDescr()
    { return wxT("CANDL Files");}

};

#endif // __SP_EXPORTCANDL__


