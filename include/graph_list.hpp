/******************************************************************************
 * Project: Téma 21 - Paralelizace - Maximální tok v síti                     *
 * Course:  GAL - Graph Algorithms - FIT BUT                                   *
 * Year:    2022/2023                                                         *
 * Authors:                                                                   *
 *          Oliver Kuník  (xkunik00) - xkunik00@stud.fit.vutbr.cz             *
 *          Dostál Michal (xdosta51) - xdosta51@stud.fit.vutbr.cz             *
 ******************************************************************************/

 /**
 * @file graph_list.hpp
 * @author Oliver KUNÍK  (xkunik00)
 * @author Michal DOSTÁL (xdosta51)
 * @date 29.11.2022
 * @brief Graph structure
 */

#ifndef __GRAPH_LIST__
#define __GRAPH_LIST__

#include <cstdint> // uint32_t
#include <vector> // std::vector
#include <iostream> // std::ifstream
#include <map> // std::map


/**
 * Graph class representing oriented graph with capacities on edges by adjacency list
 * @brief Oriented graph
 */
class GraphList
{
private:
    std::vector<std::map<uint32_t, uint32_t>> e; /**< capacities of edges */

public:

    /**
     * Create a new Graph from description in file
     * @brief Constructor
     * @param f file containing graph description
     */
    GraphList(std::istream &f);

    /**
     * Get number of nodes in graph
     * @return number of nodes
     */
    uint32_t get_nodes_count() const
    {
        return e.size();
    }

    /**
     * Get adjacency list
     * @return adj list
     */
    const std::vector<std::map<uint32_t, uint32_t>>& get_list() const
    {
        return e;
    }

    /**
     * Operator << for Graph printing
     * @param os Output stream
     * @param g Graph
     * @return Output stream with graph
     */
    friend std::ostream& operator <<(std::ostream &os, const GraphList &g)
    {
        return g.print(os);
    }

    /**
     * Computes maximum flow using Edmonds-Karp algorithm
     * @see Cormen, Leiserson, Rivest, Stein: Introduction to algorithms. The MIT Press and McGraw-Hill, 2001
     * @param source source node
     * @param sink sink node
     * @return maximum flow
     */
    uint32_t edmonds_karp(const uint32_t source, const uint32_t sink) const;

    /**
     * Computes maximum flow using Ford-Fulkerson algorithm
     * @see http://worldcomp-proceedings.com/proc/p2013/PDP3767.pdf
     * @param source source node
     * @param sink sink node
     * @return maximum flow
     */
    uint32_t ford_fulkerson(const uint32_t source, const uint32_t sink) const;

    /**
     * Computes maximum flow using parallel Ford-Fulkerson algorithm
     * @see http://worldcomp-proceedings.com/proc/p2013/PDP3767.pdf
     * @param source source node
     * @param sink sink node
     * @return maximum flow
     */
    uint32_t ford_fulkerson_parallel(const uint32_t source, const uint32_t sink) const;

    /**
     * Computes maximum flow using Push-Relabel algorithm
     * @see https://arxiv.org/pdf/1507.01926.pdf
     * @param source source node
     * @param sink sink node
     * @return maximum flow
     */
    uint32_t push_relabel(const uint32_t source, const uint32_t sink) const;

private:
    std::ostream& print(std::ostream &os) const;

    /**
     * Helper function calculating breadth-first search for Edmonds-Karp algorithm
     * @brief BFS
     * @param rn residual network
     * @param s source
     * @param t sink
     * @param parent array of predecessors
     */
    bool bfs(const std::vector<std::map<uint32_t, uint32_t>> &rn,
             const uint32_t source, const uint32_t sink,
             std::vector<uint32_t> &parent) const;

};

#endif // __GRAPH_LIST__

/*************************** End of graph_list.hpp ****************************/
