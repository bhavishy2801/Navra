#include "../include/graph.h"
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Graph::Graph() : numVertices(0) {}

void Graph::addAttraction(const Attraction& attr) {
    attractions[attr.id] = attr;
    nameToId[attr.name] = attr.id;
    if (adjList.find(attr.id) == adjList.end()) {
        adjList[attr.id] = vector<pair<int, double>>();
        numVertices++;
    }
}

void Graph::addEdge(int from, int to, double weight) {
    adjList[from].push_back(make_pair(to, weight));
    adjList[to].push_back(make_pair(from, weight)); // Undirected
}

vector<pair<int, double>> Graph::getNeighbors(int nodeId) const {
    auto it = adjList.find(nodeId);
    if (it != adjList.end()) {
        return it->second;
    }
    return vector<pair<int, double>>();
}

Attraction Graph::getAttraction(int id) const {
    auto it = attractions.find(id);
    if (it != attractions.end()) {
        return it->second;
    }
    return Attraction();
}

double Graph::getEdgeWeight(int from, int to) const {
    auto neighbors = getNeighbors(from);
    for (size_t i = 0; i < neighbors.size(); i++) {
        if (neighbors[i].first == to) {
            return neighbors[i].second;
        }
    }
    return numeric_limits<double>::infinity();
}

vector<int> Graph::getAllAttractionIds() const {
    vector<int> ids;
    for (auto it = attractions.begin(); it != attractions.end(); ++it) {
        ids.push_back(it->first);
    }
    return ids;
}

bool Graph::hasAttraction(int id) const {
    return attractions.find(id) != attractions.end();
}

int Graph::getIdByName(const string& name) const {
    auto it = nameToId.find(name);
    if (it != nameToId.end()) {
        return it->second;
    }
    return -1;
}

void Graph::loadFromCSV(const string& attractionsFile, const string& roadsFile) {
    // Load attractions
    ifstream attFile(attractionsFile);
    if (!attFile.is_open()) {
        cerr << "Error: Cannot open " << attractionsFile << endl;
        return;
    }

    string line;
    getline(attFile, line); // Skip header
    int id = 0;

    while (getline(attFile, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string name, category;
        double rating, duration, fee, lat, lon;
        int pop;

        getline(ss, name, ',');
        getline(ss, category, ',');
        ss >> rating; ss.ignore();
        ss >> duration; ss.ignore();
        ss >> fee; ss.ignore();
        ss >> pop; ss.ignore();
        ss >> lat; ss.ignore();
        ss >> lon;

        Attraction attr(id, name, category, lat, lon, duration, rating, fee, pop);
        addAttraction(attr);
        id++;
    }
    attFile.close();
    cout << "Loaded " << id << " attractions." << endl;

    // Load roads
    ifstream roadsFileStream(roadsFile);
    if (!roadsFileStream.is_open()) {
        cerr << "Error: Cannot open " << roadsFile << endl;
        return;
    }

    getline(roadsFileStream, line); // Skip header
    int edgeCount = 0;

    while (getline(roadsFileStream, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string from, to;
        double time;

        getline(ss, from, ',');
        getline(ss, to, ',');
        ss >> time;

        // Remove \r if present
        if (!from.empty() && from[from.length()-1] == '\r') 
            from = from.substr(0, from.length()-1);
        if (!to.empty() && to[to.length()-1] == '\r') 
            to = to.substr(0, to.length()-1);

        int fromId = getIdByName(from);
        int toId = getIdByName(to);

        if (fromId != -1 && toId != -1) {
            addEdge(fromId, toId, time);
            edgeCount++;
        }
    }
    roadsFileStream.close();
    cout << "Loaded " << edgeCount << " roads." << endl;
}
