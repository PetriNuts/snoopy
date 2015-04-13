/*
 * SP_DS_SimulatorThreadStochastic.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 16.11.2011
 * Short Description:
 */
 //==============================================================================
#ifndef SP_DS_SIMULATORTHREADSTOCHASTIC_H_
#define SP_DS_SIMULATORTHREADSTOCHASTIC_H_

#include "sp_ds/extensions/SP_DS_SimulatorThread.h"

class SP_DS_SimulatorThreadStochastic: public SP_DS_SimulatorThread
{
private:

protected:

	      virtual void* Entry();

public:
	      SP_DS_SimulatorThreadStochastic(wxWindow* p_pcParent,spsim::Simulator* p_pcSimulator);
	      virtual ~SP_DS_SimulatorThreadStochastic();
};

#endif /* SPDSSIMULATORTHREADSTOCHASTIC_H_ */
