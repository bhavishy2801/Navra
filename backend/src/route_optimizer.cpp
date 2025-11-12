#include "../include/algorithms.h"
#include <queue>
#include <vector>
#include <limits>
#include <algorithm>

struct Node {
    int id;
    double distance;
    
    bool operator>(const Node& other) const {
        return distance>other.distance;
    }
};

std::vector<double> dijkstra(const Graph& g,int start) {
    std::vector<double> dist(10000,std::numeric_limits<double>::infinity());
    std::priority_queue<Node,std::vector<Node>,std::greater<Node>> pq;
    dist[start]=0;
    pq.push({start,0});
    while (!pq.empty()) {
        Node curr=pq.top();
        pq.pop();
        if (curr.distance>dist[curr.id]) continue;
        auto neighbors=g.getNeighbors(curr.id);
        for (size_t i=0; i<neighbors.size(); i++) {
            int neighbor=neighbors[i].first;
            double weight=neighbors[i].second;
            double newDist=dist[curr.id]+weight;
            if (newDist<dist[neighbor]) {
                dist[neighbor]=newDist;
                pq.push({neighbor,newDist});
            }
        }
    }
    return dist;
}

std::pair<std::vector<double>,std::vector<int>>
dijkstraWithPath(const Graph& g,int start) {
    std::vector<double> dist(10000,std::numeric_limits<double>::infinity());
    std::vector<int> parent(10000,-1);
    std::priority_queue<Node,std::vector<Node>,std::greater<Node>> pq;  
    dist[start]=0;
    pq.push({start,0});
    while (!pq.empty()) {
        Node curr=pq.top();
        pq.pop();
        if (curr.distance>dist[curr.id]) continue;
        auto neighbors=g.getNeighbors(curr.id);
        for (size_t i=0; i<neighbors.size(); i++) {
            int neighbor=neighbors[i].first;
            double weight=neighbors[i].second;
            double newDist=dist[curr.id]+weight;
            if (newDist<dist[neighbor]) {
                dist[neighbor]=newDist;
                parent[neighbor]=curr.id;
                pq.push({neighbor,newDist});
            }
        }
    }   
    return std::make_pair(dist,parent);
}

std::vector<int> reconstructPath(const std::vector<int>& parent,int start,int end) {
    std::vector<int> path;
    int curr=end;
    while (curr!=-1) {
        path.push_back(curr);
        if (curr==start) break;
        curr=parent[curr];
    }
    std::reverse(path.begin(),path.end());
    return path;
}