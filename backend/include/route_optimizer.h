#ifndef ROUTE_OPTIMIZER_H
#define ROUTE_OPTIMIZER_H

#include "graph.h"
#include <vector>
#include <string>
#include <unordered_set>

struct TouristPreferences {
    std::vector<std::string> interests;
    double maxBudget;
    double maxTime;
    double maxDistance;
    bool preferPopular;
    int startLocationId;
    
    TouristPreferences():maxBudget(10000),maxTime(8),maxDistance(100),preferPopular(true),startLocationId(0) {}
};

struct RouteResult {
    std::vector<int> attractionIds;
    double totalDistance;
    double totalTime;
    double totalCost;
    double score;
};

class RouteOptimizer {
private:
    Graph graph;
    std::vector<int> filterAttractions(const TouristPreferences& prefs);
    double calculateRouteScore(const std::vector<int>& route,const TouristPreferences& prefs);
    RouteResult evaluateRoute(const std::vector<int>& route);
public:
    RouteOptimizer() {}
    void setGraph(const Graph& g) 
    {graph=g;}
    std::vector<RouteResult> recommendRoutes(const TouristPreferences& prefs,int numRoutes=3);
};

#endif