#ifndef DSU_H
#define DSU_H

#include <vector>

class DSU {
private:
    std::vector<int> parent;
    std::vector<int> rank;

public:
    DSU(int n);
    int find(int x);
    bool unite(int x, int y);
    int size() const { return parent.size(); }
};

#endif
