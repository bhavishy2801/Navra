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
        if (id != -1) {
            locations.push_back(id);
        }
    }

    bool flexibleOrder = (choice == 1);

    RouteOptimizer optimizer;
    optimizer.setGraph(graph);

    RouteResult result = optimizer.computeOptimalRoute(locations, flexibleOrder);

    json out;
    out["algorithm"] = result.algorithm;
    out["totalTime"] = result.totalTime;
    out["routeIds"] = result.attractionIds;

    vector<string> routeNames;
    for (int id : result.attractionIds) {
        routeNames.push_back(graph.getAttraction(id).name);
    }

    out["routeNames"] = routeNames;

    cout << out.dump();

    return 0;
}