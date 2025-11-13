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

static double INF_DOUBLE() { return numeric_limits<double>::infinity(); }

vector<double> dijkstra(const Graph& g, int start) {
    vector<int> ids = g.getAllAttractionIds();
    int maxId = start;
    if (!ids.empty()) maxId = max(maxId, *max_element(ids.begin(), ids.end()));

    vector<double> dist(maxId + 1, INF_DOUBLE());
    priority_queue<Node, vector<Node>, greater<Node>> pq;

    if (start < 0 || start >= (int)dist.size()) return dist;

    dist[start] = 0.0;
    pq.push({start, 0.0});

    while (!pq.empty()) {
        Node cur = pq.top(); pq.pop();
        if (cur.id < 0 || cur.id >= (int)dist.size()) continue;
        if (cur.distance > dist[cur.id]) continue;

        for (auto &pr : g.getNeighbors(cur.id)) {
            int nb = pr.first;
            double w = pr.second;
            if (nb >= (int)dist.size()) dist.resize(nb + 1, INF_DOUBLE());
            double nd = dist[cur.id] + w;
            if (nd < dist[nb]) {
                dist[nb] = nd;
                pq.push({nb, nd});
            }
        }
    }
    return dist;
}

pair<vector<double>, vector<int>> dijkstraWithPath(const Graph& g, int start) {
    vector<int> ids = g.getAllAttractionIds();
    int maxId = start;
    if (!ids.empty()) maxId = max(maxId, *max_element(ids.begin(), ids.end()));

    vector<double> dist(maxId + 1, INF_DOUBLE());
    vector<int> parent(maxId + 1, -1);
    priority_queue<Node, vector<Node>, greater<Node>> pq;

    if (start < 0 || start >= (int)dist.size()) return {dist, parent};

    dist[start] = 0.0;
    pq.push({start, 0.0});

    while (!pq.empty()) {
        Node cur = pq.top(); pq.pop();
        if (cur.id < 0 || cur.id >= (int)dist.size()) continue;
        if (cur.distance > dist[cur.id]) continue;

        for (auto &pr : g.getNeighbors(cur.id)) {
            int nb = pr.first;
            double w = pr.second;
            if (nb >= (int)dist.size()) {
                dist.resize(nb + 1, INF_DOUBLE());
                parent.resize(nb + 1, -1);
            }
            double nd = dist[cur.id] + w;
            if (nd < dist[nb]) {
                dist[nb] = nd;
                parent[nb] = cur.id;
                pq.push({nb, nd});
            }
        }
    }
    return {dist, parent};
}

vector<int> reconstructPath(const vector<int>& parent, int start, int end) {
    vector<int> path;
    if (end < 0 || end >= (int)parent.size()) return path;
    int cur = end;
    while (cur != -1 && cur != start) {
        path.push_back(cur);
        cur = parent[cur];
    }
    if (cur == start) {
        path.push_back(start);
        reverse(path.begin(), path.end());
        return path;
    }
    return {};
}

pair<double, vector<int>> computeOrderedRoute(const Graph& g, const vector<int>& orderedList) {
    if (orderedList.empty()) return {0.0, {}};
    if (orderedList.size() == 1) return {0.0, orderedList};

    vector<int> fullPath;
    double totalTime = 0.0;

    for (size_t i = 0; i + 1 < orderedList.size(); ++i) {
        int from = orderedList[i];
        int to = orderedList[i + 1];

        auto result = dijkstraWithPath(g, from);
        auto &dist = result.first;
        auto &parent = result.second;

        if (to < 0 || to >= (int)dist.size() || dist[to] == INF_DOUBLE()) {
            return {INF_DOUBLE(), {}};
        }

        auto segment = reconstructPath(parent, from, to);
        if (segment.empty()) return {INF_DOUBLE(), {}};

        if (i == 0) fullPath = segment;
        else fullPath.insert(fullPath.end(), segment.begin() + 1, segment.end());

        totalTime += dist[to];
    }

    return {totalTime, fullPath};
}
