#include "../include/graph.h"
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

using namespace std;

// Trim whitespace and surrounding quotes
static inline std::string trim_and_unquote(const std::string &s_in) {
    size_t a = 0, b = s_in.size();
    while (a < b && isspace((unsigned char)s_in[a])) ++a;
    while (b > a && isspace((unsigned char)s_in[b - 1])) --b;
    if (b <= a) return "";
    string out = s_in.substr(a, b - a);
    if (out.size() >= 2) {
        if ((out.front() == '"' && out.back() == '"') || (out.front() == '\'' && out.back() == '\'')) {
            out = out.substr(1, out.size() - 2);
            // re-trim inside quotes
            size_t aa = 0, bb = out.size();
            while (aa < bb && isspace((unsigned char)out[aa])) ++aa;
            while (bb > aa && isspace((unsigned char)out[bb - 1])) --bb;
            if (bb <= aa) return "";
            out = out.substr(aa, bb - aa);
        }
    }
    return out;
}

Graph::Graph() : numVertices(0), dsu(nullptr) {}

void Graph::addAttraction(const Attraction& attr) {
    attractions[attr.id] = attr;
    nameToId[attr.name] = attr.id;
    if (adjList.find(attr.id) == adjList.end()) {
        adjList[attr.id] = vector<pair<int, double>>();
        numVertices++;
    }
}

void Graph::addEdge(int from, int to, double weight) {
    if (adjList.find(from) == adjList.end()) adjList[from] = {};
    if (adjList.find(to) == adjList.end()) adjList[to] = {};
    adjList[from].push_back(make_pair(to, weight));
    adjList[to].push_back(make_pair(from, weight));
}

vector<pair<int, double>> Graph::getNeighbors(int nodeId) const {
    auto it = adjList.find(nodeId);
    if (it != adjList.end()) return it->second;
    return {};
}

Attraction Graph::getAttraction(int id) const {
    auto it = attractions.find(id);
    if (it != attractions.end()) return it->second;
    return Attraction();
}

double Graph::getEdgeWeight(int from, int to) const {
    auto nb = getNeighbors(from);
    for (auto &p : nb) if (p.first == to) return p.second;
    return numeric_limits<double>::infinity();
}

vector<int> Graph::getAllAttractionIds() const {
    vector<int> ids;
    ids.reserve(attractions.size());
    for (auto &p : attractions) ids.push_back(p.first);
    return ids;
}

bool Graph::hasAttraction(int id) const {
    return attractions.find(id) != attractions.end();
}

int Graph::getIdByName(const string& name) const {
    auto it = nameToId.find(name);
    return (it != nameToId.end()) ? it->second : -1;
}

void Graph::buildDSU() {
    if (dsu) { delete dsu; dsu = nullptr; }

    int maxId = -1;
    for (const auto &p : attractions) if (p.first > maxId) maxId = p.first;

    int size = (maxId >= 0) ? (maxId + 1) : 0;
    dsu = new DSU(size);

    for (const auto& entry : adjList) {
        int u = entry.first;
        for (const auto& pr : entry.second) {
            int v = pr.first;
            if (u >= 0 && v >= 0 && u < size && v < size)
                dsu->unite(u, v);
        }
    }
}

void Graph::loadFromCSV(const string& attractionsFile, const string& roadsFile) {
    // reset
    attractions.clear();
    nameToId.clear();
    adjList.clear();
    numVertices = 0;
    if (dsu) { delete dsu; dsu = nullptr; }

    // --- Load attractions file robustly ---
    ifstream att(attractionsFile);
    if (!att.is_open()) {
        cerr << "Error: Cannot open " << attractionsFile << endl;
        return;
    }

    // Read first line and decide if it's a header (contains letters)
    string firstLine;
    if (!getline(att, firstLine)) { att.close(); return; }
    bool firstIsHeader = false;
    for (char c : firstLine) {
        if (isalpha((unsigned char)c)) { firstIsHeader = true; break; }
    }

    if (!firstIsHeader) {
        // first line is data; reset stream to beginning
        att.clear();
        att.seekg(0);
    }

    string line;
    int id = 0;
    while (getline(att, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string name, category, rating_s, duration_s, fee_s, pop_s, lat_s, lon_s;

        getline(ss, name, ',');
        getline(ss, category, ',');
        getline(ss, rating_s, ',');
        getline(ss, duration_s, ',');
        getline(ss, fee_s, ',');
        getline(ss, pop_s, ',');
        getline(ss, lat_s, ',');
        getline(ss, lon_s, ',');

        name = trim_and_unquote(name);
        category = trim_and_unquote(category);

        double rating = 0.0, duration = 0.0, fee = 0.0, lat = 0.0, lon = 0.0;
        int pop = 0;
        try { rating = rating_s.empty() ? 0.0 : stod(trim_and_unquote(rating_s)); } catch (...) { rating = 0.0; }
        try { duration = duration_s.empty() ? 0.0 : stod(trim_and_unquote(duration_s)); } catch (...) { duration = 0.0; }
        try { fee = fee_s.empty() ? 0.0 : stod(trim_and_unquote(fee_s)); } catch (...) { fee = 0.0; }
        try { pop = pop_s.empty() ? 0 : stoi(trim_and_unquote(pop_s)); } catch (...) { pop = 0; }
        try { lat = lat_s.empty() ? 0.0 : stod(trim_and_unquote(lat_s)); } catch (...) { lat = 0.0; }
        try { lon = lon_s.empty() ? 0.0 : stod(trim_and_unquote(lon_s)); } catch (...) { lon = 0.0; }

        Attraction attr(id, name, category, lat, lon, duration, rating, fee, pop);
        addAttraction(attr);
        id++;
    }
    att.close();

    // --- Load roads ---
    ifstream rf(roadsFile);
    if (!rf.is_open()) {
        cerr << "Error: Cannot open " << roadsFile << endl;
        buildDSU();
        return;
    }

    // Detect header similarly
    string rfFirst;
    if (!getline(rf, rfFirst)) { rf.close(); buildDSU(); return; }
    bool rfHeader = false;
    for (char c : rfFirst) if (isalpha((unsigned char)c)) { rfHeader = true; break; }
    if (!rfHeader) { rf.clear(); rf.seekg(0); }

    while (getline(rf, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string from, to, time_s;

        getline(ss, from, ',');
        getline(ss, to, ',');
        getline(ss, time_s);

        from = trim_and_unquote(from);
        to = trim_and_unquote(to);
        time_s = trim_and_unquote(time_s);

        double t = 0.0;
        try { t = time_s.empty() ? 0.0 : stod(time_s); } catch (...) { t = 0.0; }

        int u = getIdByName(from);
        int v = getIdByName(to);

        if (u != -1 && v != -1) addEdge(u, v, t);
        // else: intentionally silent (avoid noisy output in tests)
    }
    rf.close();

    // build DSU now that adjacency is ready
    buildDSU();
}
