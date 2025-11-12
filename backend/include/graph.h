#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <unordered_map>
#include <utility>
#include <limits>
#include "attraction.h"

class Graph {
private:
    std::unordered_map<int,Attraction> attractions;
    std::unordered_map<int,std::vector<std::pair<int,double>>> adjList;
    int numVertices;
    
public:
    Graph();
    void addAttraction(const Attraction& attr);
    void addEdge(int from,int to,double distance);
    std::vector<std::pair<int,double>> getNeighbors(int nodeId) const;
    Attraction getAttraction(int id) const;
    double getEdgeWeight(int from,int to) const;
    int size() const { return numVertices; }
    std::vector<int> getAllAttractionIds() const;
    bool hasAttraction(int id) const;
    
    void buildCompleteGraph();
};

#endif
