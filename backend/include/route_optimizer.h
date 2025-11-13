#ifndef ROUTE_OPTIMIZER_H
#define ROUTE_OPTIMIZER_H

#include "graph.h"
#include <vector>
#include <string>

struct RouteResult {
    std::vector<int> attractionIds;
    double totalTime;
    std::string algorithm;
    
    RouteResult() : totalTime(0), algorithm("") {}
};

class RouteOptimizer {
private:
    Graph graph;
    
    // Build distance matrix for TSP
    std::vector<std::vector<double>> buildDistanceMatrix(const std::vector<int>& locations);

public:
    RouteOptimizer() {}
    
    void setGraph(const Graph& g) { graph = g; }
    
    // Compute optimal route (main function)
    RouteResult computeOptimalRoute(const std::vector<int>& locations, bool flexibleOrder);
    
    // Helper to evaluate and construct route result
    RouteResult evaluateRoute(const std::vector<int>& route, const std::string& algorithmName);
};

#endif
