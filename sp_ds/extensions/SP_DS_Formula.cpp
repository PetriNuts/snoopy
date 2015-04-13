/*
 * SP_DS_Formula.cpp
 *
 *  Created on: Mar 24, 2010
 *      Author: crohr
 */

#include "SP_DS_Formula.h"

SP_DS_Formula::SP_DS_Formula(SP_DS_Graph* p_pcGraph, const rfdag::RFDAG& p_ExpressionTree):
m_pcGraph(p_pcGraph),
m_Expression(p_ExpressionTree)
{

}

SP_DS_Formula::~SP_DS_Formula()
{

}
