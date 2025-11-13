#pragma once
#include <string>
#include <vector>
#include "graph.h"
#include "route_optimizer.h"

struct ApiResult {
    std::string algorithm;
    double totalTime;
    std::vector<std::string> routeNames;
    std::vector<int> routeIds;
};

ApiResult runOptimizerAPI(int mode, const std::vector<std::string>& locations);
