/******************************************************************************
 * Project: Téma 21 - Paralelizace - Maximální tok v síti                     *
 * Course:  GAL - Graph Algorithms - FIT BUT                                  *
 * Year:    2022/2023                                                         *
 * Authors:                                                                   *
 *          Oliver Kuník  (xkunik00) - xkunik00@stud.fit.vutbr.cz             *
 *          Michal Dostál (xdosta51) - xdosta51@stud.fit.vutbr.cz             *
 ******************************************************************************/

 /**
 * @file edmonds_karp.cpp
 * @author Oliver KUNÍK  (xkunik00)
 * @author Michal DOSTÁL (xdosta51)
 * @date 28.11.2022
 * @brief Edmonds-Karp implementation using BFS
 */

#include "graph_matrix.hpp"
#include "graph_list.hpp"

#include <queue> // std::queue
#include <limits> // std::numeric_limits<uint32_t>::max()
#include <algorithm> // std::min()

#define WHITE 0
#define BLACK 1

uint32_t GraphMatrix::edmonds_karp(const uint32_t source, const uint32_t sink) const
{
    // Residual network
    std::vector<uint32_t> rn(this->c);

    // Maximum network flow, begining with feasible flow 0
    uint32_t max_flow = 0;

    // Parent node vector for BFS
    std::vector<uint32_t> parent(nodes_count);

    // While there is augmenting path
    while (this->bfs(rn, source, sink, parent)) {

        // Max path flow
        uint32_t max_path_flow = std::numeric_limits<uint32_t>::max();

        // Find max path flow traversing path
        for (uint32_t u = sink; u != source; u = parent[u]) {
            max_path_flow = std::min(max_path_flow, rn[E(parent[u], u)]);
        }

        // Update residual network
        for (uint32_t u = sink; u != source; u = parent[u]) {
            rn[E(parent[u], u)] -= max_path_flow;
            rn[E(u, parent[u])] += max_path_flow;
        }

        // Update max flow
        max_flow += max_path_flow;
    }
    return max_flow;
}

bool GraphMatrix::bfs(const std::vector<uint32_t> &rn,
                      const uint32_t source, const uint32_t sink,
                      std::vector<uint32_t> &parent) const
{
    // Set initial color to white
    std::vector<uint32_t> color(nodes_count, WHITE);

    // Set up source node
    color[source] = BLACK;
    parent[source] = source;

    std::queue<uint32_t> q;
    q.push(source);

    // While there is unvisited node
    while (!q.empty()) {
        uint32_t u = q.front();
        q.pop();

        // For each edge from u to white node
        for (uint32_t v = 0; v < nodes_count; ++v) {
            if (rn[E(u, v)] > 0 && color[v] == WHITE) {

                parent[v] = u;

                // There is path from source to sink
                if (v == sink) {
                    return true;
                }

                q.push(v);
            }
        }
        color[u] = BLACK;
    }
    return false;
}

uint32_t GraphList::edmonds_karp(const uint32_t source, const uint32_t sink) const
{
    // Nodes in network
    const uint32_t nodes_count = this->get_nodes_count();

    // Residual network
    std::vector<std::map<uint32_t, uint32_t>> rn(this->e);

    // Insert backward edges with 0 capacity
    for (uint32_t i = 0; i < this->e.size(); ++i) {
        for (const std::pair<const uint32_t, uint32_t> &e : this->e[i]) {
            rn[e.first].insert({i, 0});
        }
    }

    // Maximum network flow, begining with feasible flow 0
    uint32_t max_flow = 0;

    // Parent node vector for BFS
    std::vector<uint32_t> parent(nodes_count);

    // While there is augmenting path
    while (this->bfs(rn, source, sink, parent)) {

        // Max path flow
        uint32_t max_path_flow = std::numeric_limits<uint32_t>::max();

        // Find max path flow traversing path
        for (uint32_t u = sink; u != source; u = parent[u]) {
            max_path_flow = std::min(max_path_flow, rn[parent[u]].find(u)->second);
        }

        // Update residual network
        for (uint32_t u = sink; u != source; u = parent[u]) {
            rn[parent[u]].find(u)->second -= max_path_flow;
            rn[u].find(parent[u])->second += max_path_flow;
        }

        // Update max flow
        max_flow += max_path_flow;
    }
    return max_flow;
}

bool GraphList::bfs(const std::vector<std::map<uint32_t, uint32_t>> &rn,
                    const uint32_t source, const uint32_t sink,
                    std::vector<uint32_t> &parent) const
{
    // Nodes in network
    const uint32_t nodes_count = this->get_nodes_count();

    // Set initial color to white
    std::vector<uint32_t> color(nodes_count, WHITE);

    // Set up source node
    color[source] = BLACK;
    parent[source] = source;

    std::queue<uint32_t> q;
    q.push(source);

    // While there is unvisited node
    while (!q.empty()) {
        uint32_t u = q.front();
        q.pop();

        // For each edge from u to white node
        for (const std::pair<const uint32_t, uint32_t> &e : rn[u]) {
            if (e.second > 0 && color[e.first] == WHITE) {

                parent[e.first] = u;

                // There is path from source to sink
                if (e.first == sink) {
                    return true;
                }

                q.push(e.first);
            }
        }
        color[u] = BLACK;
    }
    return false;
}

/*************************** End of edmonds_karp.cpp **************************/
