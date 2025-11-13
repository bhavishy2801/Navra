#include "api.h"
#include "algorithms.h"

ApiResult runOptimizerAPI(int mode, const std::vector<std::string>& locations) {
    Graph graph;
    graph.loadFromCSV("attractions.csv", "roads.csv");

    std::vector<int> ids;
    for (auto &name : locations) {
        int id = graph.getIdByName(name);
        if (id != -1) ids.push_back(id);
    }

    bool flexible = (mode == 1);

    RouteOptimizer optimizer;
    optimizer.setGraph(graph);

    RouteResult r = optimizer.computeOptimalRoute(ids, flexible);

    ApiResult out;
    out.algorithm = r.algorithm;
    out.totalTime = r.totalTime;
    out.routeIds = r.attractionIds;

    for (int id : r.attractionIds)
        out.routeNames.push_back(graph.getAttraction(id).name);

    return out;
}