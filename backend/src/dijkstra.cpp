#include "../include/algorithms.h"
#include <queue>
#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

struct Node {
    int id;
    double distance;
    bool operator>(const Node& other) const {
        return distance > other.distance;
    }
};

vector<double> dijkstra(const Graph& g, int start) {
    const int MAX_NODES = 1000;
    vector<double> dist(MAX_NODES, numeric_limits<double>::infinity());
    priority_queue<Node, vector<Node>, greater<Node>> pq;
    
    dist[start] = 0;
    pq.push({start, 0});

    while (!pq.empty()) {
        Node curr = pq.top();
        pq.pop();

        if (curr.distance > dist[curr.id]) continue;

        auto neighbors = g.getNeighbors(curr.id);
        for (size_t i = 0; i < neighbors.size(); i++) {
            int neighbor = neighbors[i].first;
            double weight = neighbors[i].second;
            double newDist = dist[curr.id] + weight;

            if (newDist < dist[neighbor]) {
                dist[neighbor] = newDist;
                pq.push({neighbor, newDist});
            }
        }
    }
    return dist;
}

pair<vector<double>, vector<int>> dijkstraWithPath(const Graph& g, int start) {
    const int MAX_NODES = 1000;
    vector<double> dist(MAX_NODES, numeric_limits<double>::infinity());
    vector<int> parent(MAX_NODES, -1);
    priority_queue<Node, vector<Node>, greater<Node>> pq;
    
    dist[start] = 0;
    pq.push({start, 0});

    while (!pq.empty()) {
        Node curr = pq.top();
        pq.pop();

        if (curr.distance > dist[curr.id]) continue;

        auto neighbors = g.getNeighbors(curr.id);
        for (size_t i = 0; i < neighbors.size(); i++) {
            int neighbor = neighbors[i].first;
            double weight = neighbors[i].second;
            double newDist = dist[curr.id] + weight;

            if (newDist < dist[neighbor]) {
                dist[neighbor] = newDist;
                parent[neighbor] = curr.id;
                pq.push({neighbor, newDist});
            }
        }
    }
    return make_pair(dist, parent);
}

vector<int> reconstructPath(const vector<int>& parent, int start, int end) {
    vector<int> path;
    int curr = end;
    
    while (curr != -1 && curr != start) {
        path.push_back(curr);
        curr = parent[curr];
    }
    
    if (curr == start) {
        path.push_back(start);
        reverse(path.begin(), path.end());
    } else {
        path.clear();
    }
    
    return path;
}

pair<double, vector<int>> computeOrderedRoute(const Graph& g, const vector<int>& orderedList) {
    vector<int> fullPath;
    double totalTime = 0;

    for (size_t i = 0; i < orderedList.size() - 1; i++) {
        int from = orderedList[i];
        int to = orderedList[i + 1];

        // Use A* for better performance if coordinates are available
        Attraction fromAttr = g.getAttraction(from);
        Attraction toAttr = g.getAttraction(to);
        
        vector<int> pathSegment;
        double segmentDist;
        
        if (fromAttr.latitude != 0 && toAttr.latitude != 0) {
            // Use A* if coordinates exist
            pathSegment = aStarPath(g, from, to);
            if (!pathSegment.empty()) {
                // Calculate distance
                segmentDist = 0;
                for (size_t j = 0; j < pathSegment.size() - 1; j++) {
                    segmentDist += g.getEdgeWeight(pathSegment[j], pathSegment[j+1]);
                }
            } else {
                // Fallback to Dijkstra
                auto result = dijkstraWithPath(g, from);
                pathSegment = reconstructPath(result.second, from, to);
                segmentDist = result.first[to];
            }
        } else {
            // Use Dijkstra
            auto result = dijkstraWithPath(g, from);
            pathSegment = reconstructPath(result.second, from, to);
            segmentDist = result.first[to];
        }

        if (pathSegment.empty()) {
            return make_pair(numeric_limits<double>::infinity(), vector<int>());
        }

        if (i == 0) {
            fullPath = pathSegment;
        } else {
            for (size_t j = 1; j < pathSegment.size(); j++) {
                fullPath.push_back(pathSegment[j]);
            }
        }

        totalTime += segmentDist;
    }

    return make_pair(totalTime, fullPath);
}
