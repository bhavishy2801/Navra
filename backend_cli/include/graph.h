#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>
#include <string>
#include <map>
#include "attraction.h"
#include "dsu.h"

class Graph {
private:
    std::unordered_map<int, Attraction> attractions;
    std::unordered_map<int, std::vector<std::pair<int, double>>> adjList;
    std::map<std::string, int> nameToId;

    int numVertices;

    DSU* dsu;  

public:
    Graph();

    void addAttraction(const Attraction& attr);
    void addEdge(int from, int to, double weight);

    std::vector<std::pair<int, double>> getNeighbors(int nodeId) const;
    Attraction getAttraction(int id) const;
    double getEdgeWeight(int from, int to) const;

    int size() const { return numVertices; }
    std::vector<int> getAllAttractionIds() const;

    bool hasAttraction(int id) const;
    int getIdByName(const std::string& name) const;

    void loadFromCSV(const std::string& attractionsFile, const std::string& roadsFile);

    void buildDSU();
    DSU* getDSU() const { return dsu; }   // IMPORTANT: return pointer, NOT value
    int getComponent(int id) const { return dsu->find(id); }
};

#endif
