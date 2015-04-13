/*
 * SP_SimpleNetBuilder.h
 *
 *  Created on: Mar 18, 2015
 *      Author: crohr
 */

#ifndef SP_CORE_TOOLS_SP_SIMPLENETBUILDER_H_
#define SP_CORE_TOOLS_SP_SIMPLENETBUILDER_H_

#include "dssz/misc/net.h"

class SP_DS_Graph;

namespace dsszmc
{
namespace andl
{
	class simple_net_builder;
}
}

class SP_AbstractNetBuilder
{
public:
	virtual ~SP_AbstractNetBuilder() = default;

	virtual bool operator()(SP_DS_Graph* p_pcGraph) = 0;

	dsszmc::andl::Net_ptr& GetNet() { return m_Net; }
	SP_DS_Graph* GetGraph() { return m_pcGraph; }

protected:

	SP_DS_Graph* m_pcGraph;
	dsszmc::andl::Net_ptr m_Net;
};

class SP_SimpleNetBuilder final : public SP_AbstractNetBuilder
{
public:
	virtual ~SP_SimpleNetBuilder() = default;

	bool operator()(SP_DS_Graph* p_pcGraph);

private:
	bool CreatePlaces(dsszmc::andl::simple_net_builder& b);
	bool CreateTransitions(dsszmc::andl::simple_net_builder& b);
	bool CreateConstants(dsszmc::andl::simple_net_builder& b);
	bool CreateFunctions(dsszmc::andl::simple_net_builder& b);
	bool CreateObservers(dsszmc::andl::simple_net_builder& b);

};

class SP_ColoredNetBuilder final : public SP_AbstractNetBuilder
{
public:
	virtual ~SP_ColoredNetBuilder() = default;

	bool operator()(SP_DS_Graph* p_pcGraph);

private:
	  bool CreateVariables(dsszmc::andl::simple_net_builder& b);
	  bool CreateColorsets(dsszmc::andl::simple_net_builder& b);
	  bool CreateColorFunctions(dsszmc::andl::simple_net_builder& b);
	  bool CreatePlaces(dsszmc::andl::simple_net_builder& b);
	  bool CreateTransitions(dsszmc::andl::simple_net_builder& b);
	  bool CreateConstants(dsszmc::andl::simple_net_builder& b);
	  bool CreateFunctions(dsszmc::andl::simple_net_builder& b);
	  bool CreateObservers(dsszmc::andl::simple_net_builder& b);

};

#endif /* SP_CORE_TOOLS_SP_SIMPLENETBUILDER_H_ */
