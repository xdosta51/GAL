/******************************************************************************
 * Project: Téma 21 - Paralelizace - Maximální tok v síti                     *
 * Course:  GAL - Graph Algorithms - FIT BUT                                  *
 * Year:    2022/2023                                                         *
 * Authors:                                                                   *
 *          Oliver Kuník  (xkunik00) - xkunik00@stud.fit.vutbr.cz             *
 *          Michal Dostál (xdosta51) - xdosta51@stud.fit.vutbr.cz             *
 ******************************************************************************/

 /**
 * @file ford_fulkerson_parallel.cpp
 * @author Oliver KUNÍK  (xkunik00)
 * @author Michal DOSTÁL (xdosta51)
 * @date 01.12.2022
 * @brief Parallel implementation of Ford-Fulkerson algorithm
 */

#include "graph_matrix.hpp"
#include "graph_list.hpp"

#include <limits> // std::numeric_limits<uint32_t>::max()
#include <algorithm> // std::min()
#include <omp.h> // omp_lock_t
#include <cstring> // memset()

// Label types
#define UNLABELED 0
#define LABELED 1
#define PLUS 2
#define MINUS 4

/**
 * Structure representing label of a node
 */
struct LabelMatrix {
    uint32_t v; /**< previous node in augmenting path */
    uint32_t l; /**< possible augmenting capacity */
};

uint32_t GraphMatrix::ford_fulkerson_parallel(const uint32_t source, const uint32_t sink) const
{
    // Matrix with the network flow
    std::vector<uint32_t> f(nodes_count * nodes_count, 0);

    // Maximum network flow, begining with feasible flow 0
    uint32_t max_flow = 0;

    // Locks for node labels
    std::vector<omp_lock_t> locks(nodes_count);

    for (omp_lock_t &l : locks) {
        omp_init_lock(&l);
    }

    // Node labels
    std::vector<LabelMatrix> labels(nodes_count);

    // Label types must be reset every new augmenting path
    std::vector<uint32_t> label_types(nodes_count, UNLABELED);

    // while there is augmenting path
    while (true) {

        // Set all nodes to unlabeled
        memset(&label_types[0], UNLABELED, sizeof(uint32_t) * nodes_count);

        // Setup source node
        label_types[source] = LABELED;
        labels[source].l = std::numeric_limits<uint32_t>::max();

        // While there is not path from source to sink
        while (label_types[sink] == UNLABELED) {

            // Something changed in path
            bool change = false;

            // Parallel BFS
            #pragma omp parallel for schedule(static,1)
            for (uint32_t e = 0; e < nodes_count * nodes_count; ++e) {

                // If this edge exists
                if (c[e] == 0) {
                    continue;
                }

                // Edge from u to v
                const uint32_t u = e / nodes_count;
                const uint32_t v = e % nodes_count;

                // u is labeled, v is unlabeled and there is free capacity in edge
                if (label_types[u] != UNLABELED &&
                    label_types[v] == UNLABELED &&
                    f[e] < c[e]) {

                    // Try to lock label of node v to setup its label
                    if (omp_test_lock(&locks[v])) {

                        // Label has changed while trying to lock it
                        if (label_types[v] != UNLABELED) {
                            omp_unset_lock(&locks[v]);
                            continue;
                        }

                        label_types[v] = PLUS;
                        labels[v] = {
                            u,
                            std::min(labels[u].l, c[e] - f[e])
                        };

                        omp_unset_lock(&locks[v]);

                        // Path has changed
                        change = true;
                    }
                }
                // v is labeled, u is unlabeled and there is reverse flow in edge
                else if (label_types[v] != UNLABELED &&
                         label_types[u] == UNLABELED &&
                         f[e] > 0) {

                    // Try to lock label of node u to setup its label
                    if (omp_test_lock(&locks[u])) {

                        // Label has changed while trying to lock it
                        if (label_types[u] != UNLABELED) {
                            omp_unset_lock(&locks[u]);
                            continue;
                        }

                        label_types[u] = MINUS;
                        labels[u] = {
                            v,
                            std::min(labels[v].l, f[e])
                        };

                        omp_unset_lock(&locks[u]);

                        // Path has changed
                        change = true;
                    }
                }
            }

            // There was no change in path, so there is no augmenting path
            if (!change) {
                break;
            }

        }

        // There is no augmenting path, so flow will not change
        if (label_types[sink] == UNLABELED) {
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
 * Structure representing label of a node
 */
struct LabelList {
    uint32_t v; /**< previous node in augmenting path */
    uint32_t e; /**< edge to previous node in augmenting path */
    uint32_t l; /**< possible augmenting capacity */
};

/**
 * Structure representing edge for parallel BFS
 */
struct ParallelEdge {
    const uint32_t u; /**< out node */
    const uint32_t v; /**< in node */
    const uint32_t c; /**< capacity */
    uint32_t f; /**< flow */
};

uint32_t GraphList::ford_fulkerson_parallel(const uint32_t source, const uint32_t sink) const
{
    std::vector<ParallelEdge> edges;

    // Nodes in network
    const uint32_t nodes_count = this->get_nodes_count();

    // Maximum network flow, begining with feasible flow 0
    uint32_t max_flow = 0;

    // Initialize edges to feasible flow 0
    for (uint32_t u = 0; u < nodes_count; ++u) {
        for (const std::pair<const uint32_t, uint32_t> &edge : this->e[u]) {
            edges.push_back({u, edge.first, edge.second, 0});
        }
    }

    // Locks for node labels
    std::vector<omp_lock_t> locks(nodes_count);

    for (omp_lock_t &l : locks) {
        omp_init_lock(&l);
    }

    // Node labels
    std::vector<LabelList> labels(nodes_count);

    // Label types must be reset every new augmenting path
    std::vector<uint32_t> label_types(nodes_count, UNLABELED);

    // while there is augmenting path
    while (true) {

        // Set all nodes to unlabeled
        memset(&label_types[0], UNLABELED, sizeof(uint32_t) * nodes_count);

        // Setup source node
        label_types[source] = LABELED;
        labels[source].l = std::numeric_limits<uint32_t>::max();

        // While there is not path from source to sink
        while (label_types[sink] == UNLABELED) {

            // Something changed in path
            bool change = false;

            // Parallel BFS
            #pragma omp parallel for schedule(static,1)
            for (uint32_t i = 0; i < edges.size(); ++i) {

                ParallelEdge &e = edges[i];

                // u is labeled, v is unlabeled and there is free capacity in edge
                if (label_types[e.u] != UNLABELED &&
                    label_types[e.v] == UNLABELED &&
                    e.f < e.c) {

                    // Try to lock label of node v to setup its label
                    if (omp_test_lock(&locks[e.v])) {

                        // Label has changed while trying to lock it
                        if (label_types[e.v] != UNLABELED) {
                            omp_unset_lock(&locks[e.v]);
                            continue;
                        }

                        label_types[e.v] = PLUS;
                        labels[e.v] = {
                            e.u,
                            i,
                            std::min(labels[e.u].l, e.c - e.f)
                        };

                        omp_unset_lock(&locks[e.v]);

                        // Path has changed
                        change = true;
                    }
                }
                // v is labeled, u is unlabeled and there is reverse flow in edge
                else if (label_types[e.v] != UNLABELED &&
                         label_types[e.u] == UNLABELED &&
                         e.f > 0) {

                    // Try to lock label of node u to setup its label
                    if (omp_test_lock(&locks[e.u])) {

                        // Label has changed while trying to lock it
                        if (label_types[e.u] != UNLABELED) {
                            omp_unset_lock(&locks[e.u]);
                            continue;
                        }

                        label_types[e.u] = MINUS;
                        labels[e.u] = {
                            e.v,
                            i,
                            std::min(labels[e.v].l, e.f)
                        };

                        omp_unset_lock(&locks[e.u]);

                        // Path has changed
                        change = true;
                    }
                }
            }

            // There was no change in path, so there is no augmenting path
            if (!change) {
                break;
            }
        }

        // There is no augmenting path, so flow will not change
        if (label_types[sink] == UNLABELED) {
            break;
        }

        // Flow update in augmenting path
        const uint32_t flow_update = labels[sink].l;

        // Update flow in augmenting path in direction from sink to source
        for (uint32_t x = sink, y = labels[x].v; x != source; x = y, y = labels[x].v) {

            // Flow in edge from y to x
            uint32_t &flow = edges[labels[x].e].f;

            if (label_types[x] == PLUS) {
                flow += flow_update;
            }
            else {
                flow -= flow_update;
            }
        }

        // Update max flow
        max_flow += flow_update;
    }

    return max_flow;
}

/******************** End of ford_fulkerson_parallel.cpp **********************/
