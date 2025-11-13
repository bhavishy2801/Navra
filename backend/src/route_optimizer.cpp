#include "../include/route_optimizer.h"
#include "../include/algorithms.h"
#include <limits>
#include <iostream>

using namespace std;

// Build distance matrix using Dijkstra for all pairs
vector<vector<double>> RouteOptimizer::buildDistanceMatrix(
    const vector<int>& locations) {
    
    int n = locations.size();
    vector<vector<double>> dist(n, 
        vector<double>(n, numeric_limits<double>::infinity()));
    
    for (int i = 0; i < n; i++) {
        dist[i][i] = 0;
        auto dijkDist = dijkstra(graph, locations[i]);
        
        for (int j = 0; j < n; j++) {
            if (i != j) {
                dist[i][j] = dijkDist[locations[j]];
            }
        }
    }
    
    return dist;
}

// Evaluate route and create result object
RouteResult RouteOptimizer::evaluateRoute(const vector<int>& route, 
                                         const string& algorithmName) {
    RouteResult result;
    result.attractionIds = route;
    result.totalTime = 0;
    result.algorithm = algorithmName;
    
    // Calculate total travel time
    for (size_t i = 0; i < route.size() - 1; i++) {
        auto dijkResult = dijkstraWithPath(graph, route[i]);
        double segmentTime = dijkResult.first[route[i + 1]];
        
        if (segmentTime < numeric_limits<double>::infinity()) {
            result.totalTime += segmentTime;
        }
    }
    
    return result;
}

// Main route computation function
RouteResult RouteOptimizer::computeOptimalRoute(const vector<int>& locations, 
                                                bool flexibleOrder) {
    RouteResult result;
    
    // Handle edge cases
    if (locations.empty()) {
        // cout << "[RouteOptimizer] Error: No locations provided" << endl;
        return result;
    }
    
    if (locations.size() == 1) {
        result.attractionIds = locations;
        result.totalTime = 0;
        result.algorithm = "Single Location";
        return result;
    }
    
    vector<int> route;
    string algorithm;
    
    if (flexibleOrder) {
        // ========================================
        // FLEXIBLE ORDER: Use TSP Optimization
        // ========================================
        
        int n = locations.size();
        // cout << "[RouteOptimizer] Computing flexible order for " 
                //   << n << " locations..." << endl;
        
        // Build distance matrix
        vector<vector<double>> distMatrix = buildDistanceMatrix(locations);
        
        if (n <= 10) {
            // Small instance: Use exact TSP DP
            // cout << "[Algorithm] TSP Dynamic Programming (Exact)" << endl;
            algorithm = "TSP DP (Exact)";
            
            auto tspResult = tspDP(distMatrix);
            
            if (!tspResult.second.empty()) {
                for (int idx : tspResult.second) {
                    route.push_back(locations[idx]);
                }
            }
            
        } else if (n <= 15) {
            // Medium instance: Compare DP vs MST
            // cout << "[Algorithm] Comparing TSP DP vs MST Approximation..." << endl;
            
            auto dpResult = tspDP(distMatrix);
            auto mstResult = tspMSTApproximation(graph, locations);
            
            if (dpResult.first <= mstResult.first) {
                cout << "[Selected] TSP DP (Time: " << dpResult.first << ")" << endl;
                algorithm = "TSP DP (Exact)";
                for (int idx : dpResult.second) {
                    route.push_back(locations[idx]);
                }
            } else {
                cout << "[Selected] MST Approximation (Time: " << mstResult.first << ")" << endl;
                algorithm = "MST + 2-Opt";
                for (int idx : mstResult.second) {
                    route.push_back(locations[idx]);
                }
            }
            
        } else {
            // Large instance: Use MST approximation
            // cout << "[Algorithm] MST-based Approximation (n > 15)" << endl;
            algorithm = "MST + 2-Opt + Greedy";
            
            auto mstResult = tspMSTApproximation(graph, locations);
            
            if (!mstResult.second.empty()) {
                for (int idx : mstResult.second) {
                    route.push_back(locations[idx]);
                }
            }
        }
        
    } else {
        // ========================================
        // FIXED ORDER: Use Dijkstra/A* Hybrid
        // ========================================
        
        // cout << "[RouteOptimizer] Computing fixed order route..." << endl;
        // cout << "[Algorithm] Dijkstra/A* Hybrid" << endl;
        algorithm = "Dijkstra + A*";
        
        auto orderedResult = computeOrderedRoute(graph, locations);
        route = orderedResult.second;
    }
    
    // Evaluate and return result
    if (route.empty()) {
        // cout << "[RouteOptimizer] Warning: No valid route found" << endl;
        result.algorithm = algorithm + " (Failed)";
        return result;
    }
    
    result = evaluateRoute(route, algorithm);
    
    // cout << "[RouteOptimizer] Route computed successfully" << endl;
    // cout << "  - Locations: " << result.attractionIds.size() << endl;
    // cout << "  - Total Time: " << result.totalTime << " minutes" << endl;
    // cout << "  - Algorithm: " << result.algorithm << endl;
    
    return result;
}
