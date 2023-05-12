/******************************************************************************
 * Project: Téma 21 - Paralelizace - Maximální tok v síti                     *
 * Course:  GAL - Graph Algorithms - FIT BUT                                  *
 * Year:    2022/2023                                                         *
 * Authors:                                                                   *
 *          Oliver Kuník  (xkunik00) - xkunik00@stud.fit.vutbr.cz             *
 *          Michal Dostál (xdosta51) - xdosta51@stud.fit.vutbr.cz             *
 ******************************************************************************/

 /**
 * @file graph_matrix.hpp
 * @author Oliver KUNÍK  (xkunik00)
 * @author Michal DOSTÁL (xdosta51)
 * @date 29.11.2022
 * @brief Graph structure
 */

#ifndef __GRAPH_MATRIX__
#define __GRAPH_MATRIX__

#include <cstdint> // uint32_t
#include <vector> // std::vector
#include <iostream> // std::ifstream

#include "graph_list.hpp"

// Macro for calculating edge in matrix from nodes
#define E(x,y) ((x)*(nodes_count) + (y))

/**
 * Graph class representing oriented graph with capacities on edges by matrix
 * @brief Oriented graph
 */
class GraphMatrix
{
private:
    uint32_t nodes_count;        /**< Vertices count */
    std::vector<uint32_t> c; /**< capacities of edges */

public:

    /**
     * Create a new Graph from GraphList
     * @brief Constructor
     * @param g Graph in adj list format
     */
    GraphMatrix(const GraphList &g);

    /**
     * Create a new Graph from description in file
     * @brief Constructor
     * @param f file containing graph description
     */
    GraphMatrix(std::istream &f);

    /**
     * Get number of nodes in graph
     * @return number of nodes
     */
    uint32_t get_nodes_count() const
    {
        return this->nodes_count;
    }


    /**
     * Operator << for Graph printing
     * @param os Output stream
     * @param g Graph
     * @return Output stream with graph
     */
    friend std::ostream& operator <<(std::ostream &os, const GraphMatrix &g)
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
    bool bfs(const std::vector<uint32_t> &rn,
             const uint32_t source, const uint32_t sink,
             std::vector<uint32_t> &parent) const;

};

#endif // __GRAPH_MATRIX__

/************************** End of graph_matrix.hpp ***************************/
