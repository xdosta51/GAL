/******************************************************************************
 * Project: Téma 21 - Paralelizace - Maximální tok v síti                     *
 * Course:  GAL - Graph Algorithms - FIT BUT                                  *
 * Year:    2022/2023                                                         *
 * Authors:                                                                   *
 *          Oliver Kuník  (xkunik00) - xkunik00@stud.fit.vutbr.cz             *
 *          Michal Dostál (xdosta51) - xdosta51@stud.fit.vutbr.cz             *
 ******************************************************************************/

/**
 * @file push_relabel.cpp
 * @author Oliver KUNÍK  (xkunik00)
 * @author Michal DOSTÁL (xdosta51)
 * @date 07.12.2022
 * @brief Implementations of Push-Relabel algorithm
 */

#include "graph_list.hpp"

#include <limits> // std::numeric_limits<uint32_t>::max()
#include <vector> // std::vector
#include <queue> // std::queue
#include <algorithm> // std::min()

struct V {
    uint32_t d = 0;
    uint32_t d2 = 0;
    uint32_t e = 0;
    uint32_t addedExcess = 0;
    bool isDiscovered = false;
    uint32_t work = 0;
    std::vector<uint32_t> outE;
    std::vector<uint32_t> inE;
    std::vector<uint32_t> discoveredVertices;
};

struct E {
    uint32_t from;
    uint32_t to;
    uint32_t c;
    int32_t f;
    int32_t rf;
};

void globalRelabel(std::vector<V> &nodes, std::vector<E> &edges, const uint32_t source, const uint32_t sink);

uint32_t GraphList::push_relabel(const uint32_t source, const uint32_t sink) const
{
    // Nodes in network
    const uint32_t n = this->get_nodes_count();

    // Edges in network
    uint32_t m = 0;

    // Nodes
    std::vector<V> nodes(n);

    // Edges
    std::vector<E> edges;

    for (uint32_t v = 0; v < n; ++v) {
        for (const std::pair<const uint32_t, uint32_t> &edge : this->e[v]) {
            edges.push_back({v, edge.first, edge.second, 0, 0});
            nodes[v].outE.push_back(m);
            nodes[edge.first].inE.push_back(m);
            m++;
        }
    }

    nodes[source].d = n;

    for (const uint32_t edge : nodes[source].outE) {
        E &e = edges[edge];
        e.f = e.c;
        e.rf = -e.c;
        nodes[e.to].e = e.c;
    }

    uint32_t workSinceLastGR = std::numeric_limits<uint32_t>::max();

    while (true) {
        std::vector<uint32_t> workingSet;

        if (workSinceLastGR >> 1 > 6*n + m) {
            workSinceLastGR = 0;
            globalRelabel(nodes, edges, source, sink);

            for (uint32_t v = 0; v < n; ++v) {
                if (nodes[v].d < n) {
                    workingSet.push_back(v);
                }
            }
        }

        if (workingSet.empty()) {
            break;
        }

        for (const uint32_t v_id : workingSet) {
            V& v =  nodes[v_id];

            v.d2 = v.d;
            uint32_t e = v.e;

            v.work = 0;

            while (e > 0) {
                uint32_t newLabel = n;
                uint32_t skipped = 0;

                for (uint32_t edg : v.outE) {
                    E &edge = edges[edg];
                    if (e == 0) {
                        break;
                    }
                    V &w = nodes[edge.to];

                    bool admissible = v.d2 == w.d + 1;

                    if (w.e) {
                        bool win = v.d == w.d + 1 ||
                                   v.d < w.d - 1 ||
                                   (v.d == w.d && edge.from < edge.to);
                        if (admissible && !win) {
                            skipped = 1;
                            continue;
                        }
                    }
                    if (admissible && (edge.c - edge.f) > 0) {
                        uint32_t delta = std::min((edge.c - edge.f), v.e);
                        edge.f += delta;
                        edge.rf -= delta;
                        e -= delta;

                        w.addedExcess += 1;
                        if (edge.to != sink && w.isDiscovered) {
                            v.discoveredVertices.push_back(edge.to);
                        }
                        w.isDiscovered = true;
                    }
                    if ((edge.c - edge.f) > 0 && w.d >= v.d2) {
                        newLabel = std::min(newLabel, w.d + 1);
                    }
                }
                for (uint32_t edg : v.inE) {
                    E &edge = edges[edg];
                    if (e == 0) {
                        break;
                    }
                    V &w = nodes[edge.from];

                    bool admissible = v.d2 == w.d + 1;

                    if (w.e) {
                        bool win = v.d == w.d + 1 ||
                                   v.d < w.d - 1 ||
                                   (v.d == w.d && edge.to < edge.from);
                        if (admissible && !win) {
                            skipped = 1;
                            continue;
                        }
                    }
                    if (admissible && (-edge.rf) > 0) {
                        uint32_t delta = std::min((-edge.rf), (int)v.e);
                        edge.rf += delta;
                        edge.f -= delta;
                        e -= delta;

                        w.addedExcess += 1;
                        if (edge.from != sink && w.isDiscovered) {
                            v.discoveredVertices.push_back(edge.from);
                        }
                        w.isDiscovered = true;
                    }
                    if ((-edge.rf) > 0 && w.d >= v.d2) {
                        newLabel = std::min(newLabel, w.d + 1);
                    }
                }

                if (e == 0 || skipped) {
                    break;
                }
                v.d2 = newLabel;
                v.work = v.outE.size() + 12;
                if (v.d2 == n) {
                    break;
                }
            }

            v.addedExcess = e - v.e;

            if (e && v.isDiscovered) {
                v.discoveredVertices.push_back(v_id);
            }
            v.isDiscovered = true;
        }

        std::vector<uint32_t> ws2(n, 0);

        for (const uint32_t v_id : workingSet) {
            V& v =  nodes[v_id];
            v.d = v.d2;
            v.e += v.addedExcess;
            v.addedExcess = 0;
            v.isDiscovered = false;

            workSinceLastGR += v.work;

            for (const uint32_t vertex : v.discoveredVertices) {
                ws2[vertex] = 1;
            }
        }

        workingSet.clear();

        for (uint32_t i = 0; i < n; ++i) {
            if (ws2[i]) {
                workingSet.push_back(i);
            }
        }

        for (const uint32_t v_id : workingSet) {
            V& v =  nodes[v_id];
            v.e += v.addedExcess;
            v.addedExcess = 0;
            v.isDiscovered = false;
        }

    }

    uint32_t max_flow = 0;

    for (uint32_t edge : nodes[sink].inE) {
        max_flow += edges[edge].f;
    }

    for (uint32_t v = 0; v < n; ++v) {
        printf("%u, %u, %u, %u, %u\n", v, nodes[v].d, nodes[v].e, nodes[v].addedExcess, nodes[v].isDiscovered);
    }
    for (uint32_t e = 0; e < m; ++e) {
        printf("%u, %u, %u, %u, %i, %i\n", e, edges[e].from, edges[e].to, edges[e].c, edges[e].f, edges[e].rf);
    }

    return max_flow;
}


void globalRelabel(std::vector<V> &nodes, std::vector<E> &edges, const uint32_t source, const uint32_t sink)
{
    std::cerr << "relabel\n";

    // Nodes in network
    const uint32_t n = nodes.size();

    for (V &v : nodes) {
        v.d = n;
    }

    nodes[sink].d = 0;

    std::queue<uint32_t> q;

    q.push(sink);

    while (!q.empty()) {
        std::cerr << "v= " << q.front() << std::endl;
        V& v =  nodes[q.front()];
        q.pop();

        for (uint32_t edge : v.outE) {
            E &e = edges[edge];
            V &w = nodes[e.to];
            if (e.to != source && -e.rf > 0 && e.to != sink && w.d == n) {
                w.d = v.d + 1;
                q.push(e.to);
                std::cerr << "HU " << e.to << std::endl;;
            }
        }
        for (uint32_t edge : v.inE) {
            E &e = edges[edge];
            V &w = nodes[e.from];
            if (e.from != source && (e.c - e.f > 0) && e.from != sink && w.d == n) {
                w.d = v.d + 1;
                q.push(e.from);
                std::cerr << "H " << e.from << " " << w.d << " " << v.d << std::endl;;
            }
        }
    }
}
