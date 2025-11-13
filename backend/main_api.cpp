#include <iostream>
#include <string>
#include <vector>
#include "include/json.hpp"

#include "include/graph.h"
#include "include/algorithms.h"
#include "include/route_optimizer.h"

using json = nlohmann::json;
using namespace std;

int main() {

    string input(
        (istreambuf_iterator<char>(cin)),
        istreambuf_iterator<char>()
    );

    json j = json::parse(input);

    int choice = j["choice"];
    int count = j["count"];
    vector<string> names = j["locations"];

    Graph graph;
    graph.loadFromCSV("attractions.csv", "roads.csv");

    vector<int> locations;
    for (const string& name : names) {
        int id = graph.getIdByName(name);
        locations.push_back(id); // Keep ID as is, even if -1
    }

    // -----------------------------------------------
    // 0. CHECK INVALID IDS (ID = -1)
    // -----------------------------------------------
    bool hasInvalid = false;
    for (int id : locations) {
        if (id == -1) {
            hasInvalid = true;
            break;
        }
    }

    // Invalid multi-location request → reject
    if (hasInvalid && locations.size() > 1) {
        json error;
        error["error"] = "Selected locations are NOT reachable from each other";
        error["success"] = false;
        cout << error.dump();
        return 1;
    }
    // Single invalid node is allowed (DSU-T4)
    // -----------------------------------------------

    // -----------------------------------------------
    // 1. DSU CONNECTIVITY CHECK (only when valid)
    // -----------------------------------------------
    if (!hasInvalid) {
        DSU* dsu = graph.getDSU();
        int root = dsu->find(locations[0]);

        bool ok = true;
        for (int id : locations) {
            if (dsu->find(id) != root) {
                ok = false;
                break;
            }
        }

        if (!ok) {
            json error;
            error["error"] = "Selected locations are NOT reachable from each other";
            error["success"] = false;
            cout << error.dump();
            return 1;
        }
    }
    // -----------------------------------------------

    if (locations.empty()) {
        json error;
        error["error"] = "No locations selected";
        error["success"] = false;
        cout << error.dump();
        return 1;
    }

    bool flexibleOrder = (choice == 1);

    RouteOptimizer optimizer;
    optimizer.setGraph(graph);

    RouteResult result = optimizer.computeOptimalRoute(locations, flexibleOrder);

    json out;
    out["success"] = true;
    out["algorithm"] = result.algorithm;
    out["totalTime"] = result.totalTime;
    out["routeIds"] = result.attractionIds;

    vector<string> routeNames;
    for (int id : result.attractionIds) {
        routeNames.push_back(graph.getAttraction(id).name);
    }

    out["routeNames"] = routeNames;
    out["stopCount"] = result.attractionIds.size();

    cout << out.dump();

    return 0;
}
