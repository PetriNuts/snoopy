//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2011/10/26 $
// Short Description: export to ANDL
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTCPP__
#define __SP_EXPORTCPP__

#include "export/SP_ExportPT.h"

class SP_ExportCPP: public SP_ExportPT
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

	SP_MapString2UInt m_mPlace2Id;

 protected:
  bool WritePlaces();
  bool WriteTransitions();
  bool WriteConstants();
  bool WriteObservers();
  virtual bool DoWrite();

 public:
  SP_ExportCPP();
  virtual ~SP_ExportCPP();

  virtual bool AcceptsDoc(SP_MDI_Doc*);

  inline virtual wxString GetName()
    { return wxT("Export to C++ ");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net C++ source code");}
  virtual wxString GetExtension()
    { return wxT("cpp");}
  virtual wxString GetFileDescr()
    { return wxT("C++ Files");}

};

#endif // __SP_EXPORTCPP__


