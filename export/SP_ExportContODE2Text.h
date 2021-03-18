/*
 * SP_ExportContODE2Text.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 12.8.2010
 * $Modified: George Assaf$
 * $Modification date:20.11.2020$
 * Short Description:
 */
 //==============================================================================

#ifndef SP_EXPORTCONTODE2TEXT_H_
#define SP_EXPORTCONTODE2TEXT_H_

#include "export/SP_ExportPT.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"
#include "spsim/spsim.h"

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

//george
private:
      spsim::Simulator* m_pcMainSimulator;
	  SP_VectorString m_asPlaceNames;
	  SP_MapString2Long m_mPlaceName2Position;
	  SP_VectorString m_asTransitionNames;
	  SP_MapString2Long m_mTransitionName2Position;
	  SP_MapString2Double m_msParameterName2Value;
private:
      void LoadContPlaces();
	  void LoadConTrans();
	  void LoadConnections();
	  void LoadConnectionOfType(const wxString& p_sArcType, const spsim::ConnectionType& p_ArcType);
	  bool IsEvaluatedArcWeight(const wxString& p_sArcWeight, double& p_dVal);
	  void LoadParams();
	  bool InitialiseContSim();
	  void DeSubstituteConstants(const wxString& p_sOde, const wxString& p_scon, wxString p_sval, wxString& p_sRes);


};

#endif /* SP_EXPORTCONTODE2TEXT_H_ */
