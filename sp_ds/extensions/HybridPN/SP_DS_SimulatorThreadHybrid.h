/*
 * SP_DS_SimulatorThreadHybrid.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 21.10.2011
 * Short Description:
 */
 //==============================================================================
#ifndef SP_DS_SIMULATORTHREADHYBRID_H_
#define SP_DS_SIMULATORTHREADHYBRID_H_

#include "sp_ds/extensions/SP_DS_SimulatorThread.h"

class SP_DS_SimulatorThreadHybrid: public SP_DS_SimulatorThread
{
private:

protected:

	      virtual void* Entry();

public:
	      SP_DS_SimulatorThreadHybrid(wxWindow* p_pcParent,spsim::Simulator* p_pcSimulator);
	      virtual ~SP_DS_SimulatorThreadHybrid();
};

#endif /* SP_DS_SIMULATORTHREADHYBRID_H_ */
