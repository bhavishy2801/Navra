#include "../include/dsu.h"

DSU::DSU(int n) : parent(n), rank(n, 0) {
    for (int i = 0; i < n; ++i) parent[i] = i;
}

int DSU::find(int x) {
    if (x < 0 || x >= (int)parent.size()) return x;
    if (parent[x] != x) parent[x] = find(parent[x]);
    return parent[x];
}

bool DSU::unite(int x, int y) {
    if (x < 0 || y < 0 || x >= (int)parent.size() || y >= (int)parent.size()) return false;
    int px = find(x);
    int py = find(y);
    if (px == py) return false;

    if (rank[px] < rank[py]) parent[px] = py;
    else if (rank[px] > rank[py]) parent[py] = px;
    else { parent[py] = px; rank[px]++; }
    return true;
}
