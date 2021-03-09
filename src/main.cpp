
#include <ctime>
#include <unordered_map>
#include <iostream>
#include <random>
#include <cassert>
#include <utility>
#include <iterator>
#include <vector>
#include <algorithm>

#include "LinkCut.h"

using std::swap;

class Graph {
public:
    explicit Graph(int n) : conj(n + 1), n(n) {}
    bool isConnected(int a, int b) const {
        std::vector<bool> marked(n + 1);
        dfs(a, marked);
        return marked[b];
    }
    void link(int v, int u) {
        edgesCounter++;
        conj[v].push_back(u);
        conj[u].push_back(v);
    }
    void cut(int v, int u) {
        edgesCounter--;
        conj[v].erase(std::find(conj[v].begin(), conj[v].end(), u));
        conj[u].erase(std::find(conj[u].begin(), conj[u].end(), v));
    }
    int edges() const {
        return edgesCounter;
    }
    bool hasEdge(int v, int u) const {
        return std::find(conj[v].begin(), conj[v].end(), u) != conj[v].end();
    }
private:
    void dfs(int vert, std::vector<bool>& marked) const {
        marked[vert] = true;
        for (int neighbor : conj[vert]) {
            if (!marked[neighbor]) {
                dfs(neighbor, marked);
            }
        }
    }
    int edgesCounter{};
    std::vector<std::vector<int>> conj;
    const int n;
};

int Test(int n, int i, int seed) {
    using namespace std;
    mt19937 rnd(seed);
    auto randomPair = [&rnd, &n]() -> std::pair<int, int> {
        return { rnd() % n + 1, rnd() % n + 1 };
    };
    auto randomMod3 = [&rnd]() -> int {
        return rnd() % 3;
    };
    LCT lct(n);
    Graph graph(n);
    while (i --> 0) {
        if (i % 10'000 == 0) {
            cout << "i == " << i << endl;
        }
        int op = randomMod3();
        if (op == 0) { // isConnected
            auto[v, u] = randomPair();
            assert(graph.isConnected(v, u) == lct.isConnected(v, u));
        } else if (op == 1 and graph.edges() != n - 1) { // link
            auto[v, u] = randomPair();
            bool lctIsConnected = lct.isConnected(v, u);
            bool graphIsConnected = graph.isConnected(u, v);
            assert(lctIsConnected == graphIsConnected);
            if (!lctIsConnected) {
                lct.link(v, u);
                graph.link(v, u);
            }
        } else if (op == 2 and graph.edges() != 0) { // cut
            auto[v, u] = randomPair();
            bool lctIsConnected = lct.isConnected(v, u);
            bool graphIsConnected = graph.isConnected(u, v);
            assert(lctIsConnected == graphIsConnected);
            if (graph.hasEdge(v, u)) {
                graph.cut(v, u);
                lct.cut(v, u);
            }
        }
    }
    cout << "\nSuccess!\n";
    return 0;
}

int main() {
    return Test(50'000, 50'000, 72'237);
}
