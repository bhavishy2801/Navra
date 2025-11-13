#include "../include/algorithms.h"
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <limits>
using namespace std;
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double haversine(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371.0;
    double dLat = (lat2 - lat1)*M_PI/180.0;
    double dLon = (lon2 - lon1)*M_PI/180.0;
    lat1 = lat1*M_PI/180.0;
    lat2 = lat2*M_PI/180.0;
    double a = sin(dLat/2.0)*sin(dLat/2.0) +
               cos(lat1)*cos(lat2)*sin(dLon/2.0)*sin(dLon/2.0);
    double c = 2.0*atan2(sqrt(a), sqrt(1.0 - a));
    return R*c*1000; // Convert to meters, then scale for time approximation
}

struct AStarNode {
    int id;
    double fScore;
    bool operator>(const AStarNode& other) const {
        return fScore > other.fScore;
    }
};

vector<int> aStarPath(const Graph& g, int start, int goal) {
    priority_queue<AStarNode, vector<AStarNode>, greater<AStarNode>> pq;
    unordered_map<int, double> gScore;
    unordered_map<int, double> fScore;
    unordered_map<int, int> cameFrom;
    unordered_set<int> closedSet;

    Attraction startAttr = g.getAttraction(start);
    Attraction goalAttr = g.getAttraction(goal);

    // Check if coordinates are valid
    if (startAttr.latitude == 0 || goalAttr.latitude == 0) {
        return vector<int>(); // Return empty, fallback to Dijkstra
    }

    gScore[start] = 0;
    double h = haversine(startAttr.latitude, startAttr.longitude,
                        goalAttr.latitude, goalAttr.longitude)/1000.0; // Heuristic
    fScore[start] = h;
    pq.push({start, fScore[start]});

    while (!pq.empty()) {
        int current = pq.top().id;
        pq.pop();

        if (current == goal) {
            // Reconstruct path
            vector<int> path;
            while (cameFrom.find(current) != cameFrom.end()) {
                path.push_back(current);
                current = cameFrom[current];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());
            return path;
        }

        if (closedSet.find(current) != closedSet.end()) continue;
        closedSet.insert(current);

        auto neighbors = g.getNeighbors(current);
        for (size_t i = 0; i < neighbors.size(); i++) {
            int neighbor = neighbors[i].first;
            double weight = neighbors[i].second;
            
            if (closedSet.find(neighbor) != closedSet.end()) continue;

            double tentativeGScore = gScore[current] + weight;

            if (gScore.find(neighbor) == gScore.end() || tentativeGScore < gScore[neighbor]) {
                cameFrom[neighbor] = current;
                gScore[neighbor] = tentativeGScore;
                
                Attraction neighborAttr = g.getAttraction(neighbor);
                h = haversine(neighborAttr.latitude, neighborAttr.longitude,
                            goalAttr.latitude, goalAttr.longitude)/1000.0;
                fScore[neighbor] = gScore[neighbor] + h;
                pq.push({neighbor, fScore[neighbor]});
            }
        }
    }

    return vector<int>(); // No path found
}
