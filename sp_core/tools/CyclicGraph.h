/*
 * CyclicGraph.h
 *
 *  Created on: 14.02.2013
 *      Author: dell-notebook
 */

#ifndef CYCLICGRAPH_H_
#define CYCLICGRAPH_H_


#include<iostream>
#include <list>
#include <limits.h>

using namespace std;

class CyclicGraph
{
    int V;
    list<int> *adj;
    bool isCyclicUtil(int v, bool visited[], bool *rs);
public:
    CyclicGraph(int V);
    void addEdge(int v, int w);
    bool isCyclic();
};


#endif /* CYCLICGRAPH_H_ */
