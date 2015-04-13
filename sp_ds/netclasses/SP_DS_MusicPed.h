//////////////////////////////////////////////////////////////////////
// $Author: dbayer $
// $Version: 0.1 $
// $Date: 2008/06/25 $
// Short Description: musical petri net
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_MUSICPED_H__
#define __SP_DS_MUSICPED_H__

#include "sp_ds/netclasses/SP_DS_SimplePed.h"

class SP_DS_MusicPed: public SP_DS_SimplePed
{
private:
protected:
public:
    SP_DS_MusicPed();
    SP_DS_MusicPed(const wxString& p_pchName);

	/** \brief Creates the grap with all node types and attributes
		This adds attributes a basic petri net to enable
		association with sound files. Also sets the animation
		class, which takes care of playing back the files
		\param p_pcGraph the graph
	*/
    virtual SP_DS_Graph* CreateGraph(SP_DS_Graph* p_pcGraph);

};

#endif // __SP_DS_MUSICPED_H__
