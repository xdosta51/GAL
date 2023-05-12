/******************************************************************************
 * Project: Téma 21 - Paralelizace - Maximální tok v síti                     *
 * Course:  GAL - Graph Algorithms - FIT BUT                                  *
 * Year:    2022/2023                                                         *
 * Authors:                                                                   *
 *          Oliver Kuník  (xkunik00) - xkunik00@stud.fit.vutbr.cz             *
 *          Michal Dostál (xdosta51) - xdosta51@stud.fit.vutbr.cz             *
 ******************************************************************************/

 /**
 * @file ford_fulkerson.cpp
 * @author Oliver KUNÍK  (xkunik00)
 * @author Michal DOSTÁL (xdosta51)
 * @date 01.12.2022
 * @brief Sequential implementation of Ford-Fulkerson algorithm
 */

#include "graph_matrix.hpp"
#include "graph_list.hpp"

#include <queue> // std::queue
#include <limits> // std::numeric_limits<uint32_t>::max()
#include <algorithm> // std::min()
#include <cstring> // memset()
#include <set> // std::set

// Label types
#define UNLABELED 0
#define LABELED 1
#define PLUS 2
#define MINUS 4

/**
 * Structure representing label of a node
 */
struct LabelM {
    uint32_t v; /**< previous node in augmenting path */
    uint32_t l; /**< possible augmenting capacity */
};

uint32_t GraphMatrix::ford_fulkerson(const uint32_t source, const uint32_t sink) const
{
    // Matrix with the network flow
    std::vector<uint32_t> f(nodes_count * nodes_count, 0);

    // Maximum network flow, begining with feasible flow 0
    uint32_t max_flow = 0;

    // Node labels
    std::vector<LabelM> labels(nodes_count);

    // Label types must be reset every new augmenting path
    std::vector<uint32_t> label_types(nodes_count, UNLABELED);

    // while there is augmenting path
    while (true) {

        // Queue for BFS
        std::queue<uint32_t> q;

        // Set all nodes to unlabeled
        memset(&label_types[0], UNLABELED, sizeof(uint32_t) * nodes_count);

        // Setup source node
        label_types[source] = LABELED;
        labels[source].l = std::numeric_limits<uint32_t>::max();

        q.push(source);

        // While there is not path from source to sink
        while (label_types[sink] == UNLABELED && !q.empty()) {

            // Scan node u
            uint32_t u = q.front();
            q.pop();

            // For all edges from u
            for (uint32_t v = 0; v < nodes_count; ++v) {

                // v is unlabeled
                if (label_types[v] == UNLABELED) {

                    // (u,v) in E and there is free capacity in (u,v)
                    if (c[E(u,v)] != 0 && f[E(u,v)] < c[E(u,v)]) {

                        label_types[v] = PLUS;
                        labels[v] = {
                            u,
                            std::min(labels[u].l, c[E(u,v)] - f[E(u,v)])
                        };
                        q.push(v);
                    }
                    // (v,u) in E and there is there is reverse flow in edge
                    else if (c[E(v,u)] != 0 && f[E(v,u)] > 0) {

                        label_types[v] = MINUS;
                        labels[v] = {
                            u,
                            std::min(labels[u].l, f[E(v,u)])
                        };
                        q.push(v);
                    }
                }
            }

        }

        // There is no augmenting path, so flow will not change
        if (q.empty()) {
            break;
        }

        // Flow update in augmenting path
        const uint32_t flow_update = labels[sink].l;

        // Update flow in augmenting path in direction from sink to source
        for (uint32_t x = sink, y = labels[x].v; x != source; x = y, y = labels[x].v) {

            if (label_types[x] == PLUS) {
                f[E(y,x)] += flow_update;
            }
            else {
                f[E(x,y)] -= flow_update;
            }
        }

        // Update max flow
        max_flow += flow_update;
    }

    return max_flow;
}

/**
 * Structure representing edge
 */
struct EdgeFlow {
    const uint32_t c; /**< capacity */
    uint32_t f; /**< flow */
};

/**
 * Structure representing label of a node
 */
struct LabelL {
    uint32_t v; /**< previous node in augmenting path */
    uint32_t *f; /**< pointer to flow between this and v node */
    uint32_t l; /**< possible augmenting capacity */
};

uint32_t GraphList::ford_fulkerson(const uint32_t source, const uint32_t sink) const
{
    // Nodes in network
    const uint32_t nodes_count = this->get_nodes_count();

    // Maximum network flow, begining with feasible flow 0
    uint32_t max_flow = 0;

    std::vector<std::map<uint32_t, EdgeFlow>> f(nodes_count);
    std::vector<std::map<uint32_t, EdgeFlow*>> reverse_edges(nodes_count);

    // Create flow list and list of reverse edges
    for (uint32_t u = 0; u < nodes_count; ++u) {
        for (const std::pair<const uint32_t, uint32_t> &edge : this->e[u]) {
            EdgeFlow *ef = &f[u].insert({edge.first, {edge.second, 0}}).first->second;
            reverse_edges[edge.first].insert({u, ef});
        }
    }

    // Node labels
    std::vector<LabelL> labels(nodes_count);

    // Label types must be reset every new augmenting path
    std::vector<uint32_t> label_types(nodes_count, UNLABELED);

    // while there is augmenting path
    while (true) {
        // Queue for BFS
        std::queue<uint32_t> q;

        // Set all nodes to unlabeled
        memset(&label_types[0], UNLABELED, sizeof(uint32_t) * nodes_count);

        // Setup source node
        label_types[source] = LABELED;
        labels[source].l = std::numeric_limits<uint32_t>::max();

        q.push(source);

        // While there is not path from source to sink
        while (label_types[sink] == UNLABELED && !q.empty()) {

            // Scan node u
            const uint32_t u = q.front();
            q.pop();

            // For each edge (u,v)
            for (std::pair<const uint32_t, EdgeFlow> &edge : f[u]) {
                const uint32_t v = edge.first;
                EdgeFlow &e = edge.second;

                // v is unlabeled and there is free capacity in (u,v)
                if (label_types[v] == UNLABELED && e.f < e.c) {

                    label_types[v] = PLUS;
                    labels[v] = {
                        u,
                        &e.f,
                        std::min(labels[u].l, e.c - e.f)
                    };

                    q.push(v);
                }
            }
            // For each edge (v,u)
            for (const std::pair<const uint32_t, EdgeFlow * > &edge : reverse_edges[u]) {
                const uint32_t v = edge.first;
                EdgeFlow &e = *edge.second;

                // v is unlabeled and there is reverse flow in edge
                if (label_types[v] == UNLABELED && e.f > 0) {

                    label_types[v] = MINUS;
                    labels[v] = {
                        u,
                        &e.f,
                        std::min(labels[u].l, e.f)
                    };

                    q.push(v);
                }
            }
        }

        // There is no augmenting path, so flow will not change
        if (q.empty()) {
            break;
        }

        // Flow update in augmenting path
        const uint32_t flow_update = labels[sink].l;

        // Update flow in augmenting path in direction from sink to source
        for (uint32_t x = sink, y = labels[x].v; x != source; x = y, y = labels[x].v) {

            if (label_types[x] == PLUS) {
                *labels[x].f += flow_update;
            }
            else {
                *labels[x].f -= flow_update;
            }
        }
        max_flow += flow_update;
    }

    return max_flow;
}

/************************* End of ford_fulkerson.cpp **************************/
