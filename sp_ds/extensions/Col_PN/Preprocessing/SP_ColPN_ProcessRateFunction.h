//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: liu $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2010/6/17 11:55:00 $
// Short Description: XML Writer, writes graph structures actually
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ColPN_ProcessRateFunction_H__
#define __SP_ColPN_ProcessRateFunction_H__

#include <wx/wx.h>
#include "sp_core/SP_Vector.h"
#include "sp_core/SP_Map.h"

#include "snoopy.h"
#include "sp_utilities.h"

#include "sp_ds/SP_DS_Node.h"

#include "sp_ds/extensions/Col_PN/ColorSetProcessing/SP_CPN_ColorSetClass.h"

struct SP_ColPN_RateFunction
{
	wxString m_sFormula;
	wxString m_sValue;
	wxString m_sColPlace; 
	wxString m_sColorSet;

};

class SP_ColPN_ProcessRateFunction
{

private:
	bool TransformRateFunction(wxString &p_sRateFunction, map<wxString, SP_ColPN_RateFunction>* p_pmColPlace2Color);
	bool GetColoredPlaces(wxString p_sFormula,SP_DS_Node* p_pcNode,map<wxString, SP_ColPN_RateFunction>* p_pmColPlace2Color, map<wxString, wxString>* p_mPlaceName2ColorSetName);

public:
	bool CheckColoredRateFunction(wxString p_sCheckFormula,SP_DS_Node* p_pcNode);
	bool ProceedRateFunction(wxString &p_sRateFunction, SP_DS_Node* p_pcNode, map<wxString, SP_ColPN_RateFunction>* p_pmColPlace2Color, map<wxString, wxString>* p_mPlaceName2ColorSetName);
	bool ReplaceRateFunction(wxString &p_sFormula,map<wxString, SP_ColPN_RateFunction>* p_pmColPlace2Color, SP_CPN_ColorSetClass* p_ColorSetClass, bool p_bExport = false);


public:
    SP_ColPN_ProcessRateFunction();
    virtual ~SP_ColPN_ProcessRateFunction();

   
};

#endif // __SP_ColPN_ProcessRateFunction_H__

