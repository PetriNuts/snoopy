//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2006/08/07 10:00:00 $
// Short Description: Calculate reliability parameters of irreparable systems
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREEIRREPARABLESYSTEMANALYSER_H__
#define __SP_DS_FTREEIRREPARABLESYSTEMANALYSER_H__

#include "sp_ds/extensions/FTree/SP_DS_FTreeAnalyser.h"

/**
	\brief Calculation of the reliability parameters of an irreparable system
	Following parameters will be calculated:
		- Reliability
		- Unreliability
		- MTTF (still called lifetime of a component)
	*/

class SP_DS_FTreeIrreparableSystemAnalyser: public SP_DS_FTreeAnalyser
{
private:
	//pseudo polynom form to calculate the unrealiability of an event
	SP_ListString* pseudoForm;


protected:

	/**
	\brief Calculation of MTTF (mean time to failure) of an event
	MTTF is calculated by the negative Pseudo-Polynomform.
	
	\param	p_pcNode	componente (event)
	*/
	void CalculateMTTF(SP_DS_Node* p_pcNode);

	/**
	\brief Calculation of reliability of an event.
		
	\param	p_pcNode	componente (event)
	*/
	void CalculateReliability(SP_DS_Node* p_pcNode);

	/**
	\brief Calculation of unreliability of an event
	The Unreliability is calculated by the pseudo-polynomform.
	
	\param	p_pcNode	componente (event)
	*/
	void CalculateUnreliability(SP_DS_Node* p_pcNode);

	/**
	\brief Returns calculated value of the unreliability
		
	\param		pseudoFormWithSeparateLiterals	pseudo-polynomform of the event	
	\returns	value							result of the unreliability
	*/
	double GetUnreliability(list<SP_ListString*>* pseudoFormWithSeparateLiterals);

	/**
	\brief Returns calculated value of MTTF
	
	\param		pseudoFormwithSeparateLiterals	pseudo-polynomform of the event
	\returns	value							result of MTTF
	*/
	double GetMTTF(list<SP_ListString*>* pseudoFormWithSeparateLiterals);

public:
    SP_DS_FTreeIrreparableSystemAnalyser();

    ~SP_DS_FTreeIrreparableSystemAnalyser();

	// Calculation of all reliability parameters of an irreparable system
	void DoAnalyseIrreparableSystem(SP_DS_Node* p_pcNode);

};

#endif // __SP_DS_FTREEANALYSIS�RREPARABLE_H__


