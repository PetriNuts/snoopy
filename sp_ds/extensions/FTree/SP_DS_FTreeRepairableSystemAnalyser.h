//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 0.0 $
// $Date: 2006/08/07 10:00:00 $
// Short Description: calculate all reliability parameters of repairable systems
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREEREPAIRABLESYSTEMANALYSER_H__
#define __SP_DS_FTREEREPAIRABLESYSTEMANALYSER_H__

#include "sp_ds/extensions/FTree/SP_DS_FTreeAnalyser.h"

	/**
	\brief Calculation of the reliability parameters of a repairable system.
	Following parameters will be calculated:
		- Availability
		- Unavailability
		- MTTF
		- MTTR
		- MTBF
		- Repair Rate
	*/
class SP_DS_FTreeRepairableSystemAnalyser: public SP_DS_FTreeAnalyser
{
private:
	//pseudo polynom form to calculate the unavailability of an event
	SP_ListString* pseudoForm;
	//value of average failure frequency to calculate MTTF and MTTR
	double fFrequency;

protected:

	
	/**
	\brief Calculation of MTTF (mean time to failure) of an event
		
	\param	p_pcNode	component (event) of the fault tree
	*/
	void CalculateMTTF(SP_DS_Node* p_pcNode);

	/**
	\brief Calculation of MTTR (mean time to repair) of an event
		
	\param	p_pcNode	component (event) of the fault tree
	*/
	void CalculateMTTR(SP_DS_Node* p_pcNode);

	/**
	\brief Calculation of MTBF (mean time between failure) of an event
		
	\param	p_pcNode	component (event) of the fault tree
	*/
	void CalculateMTBF(SP_DS_Node* p_pcNode);

	/**
	\brief Calculation of the availability of an event
		
	\param	p_pcNode	component (event) of the fault tree
	*/
	void CalculateAvailability(SP_DS_Node* p_pcNode);

	/**
	\brief Calculation of the unavailability of an event
	The Unavailability is calculated by the pseudo-polynomform.
		
	\param	p_pcNode	component (event) of the fault tree
	*/
	void CalculateUnavailability(SP_DS_Node* p_pcNode);

	/**
	\brief Calculation of the repair rate of an event
	The repair rate is calculated only for intermediate events and the top event.
		
	\param	p_pcNode	component (event) of the fault tree
	*/
	void CalculateRepairRate(SP_DS_Node* p_pcNode);

	/**
	\brief Calculation of the average failure frequency of an event
	The average failure frequency is calculated by the pseudo-polynomform.
	It is needed to calculate MTTF and MTTR of intermediate events and the top event.
		
	\param	pseudoFormWithSeparateLiterals	the pseudo-polynomform of a component
	*/
	void CalculateAverageFailureFrequency(list<SP_ListString*>* pseudoFormWithSeparateLiterals);
	
	/**
	\brief Returns calculated value of the average failure frequency

	\returns	fFrequency		value of the average failure frequency
	*/
	double GetAverageFailureFrequency();

	/**
	\brief Returns calculated value of the unavailability of the component
		
	\param		pseudoFormWithSeparateLiterals	the pseudo-polynomform of a component
	\returns	value							result of unavailability
	*/
	double GetUnavailability(list<SP_ListString*>* pseudoFormWithSeparateLiterals);

public:
    SP_DS_FTreeRepairableSystemAnalyser();

    virtual ~SP_DS_FTreeRepairableSystemAnalyser();

	//calculate the reliability parameters of an event
	void DoAnalyseRepairableSystem(SP_DS_Node* p_pcNode);
};

#endif // __SP_DS_FTREEREPAIRABLESYSTEMANALYSER_H__


