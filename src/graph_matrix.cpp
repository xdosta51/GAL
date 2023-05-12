/******************************************************************************
 * Project: Téma 21 - Paralelizace - Maximální tok v síti                     *
 * Course:  GAL - Graph Algorithms - FIT BUT                                  *
 * Year:    2022/2023                                                         *
 * Authors:                                                                   *
 *          Oliver Kuník  (xkunik00) - xkunik00@stud.fit.vutbr.cz             *
 *          Michal Dostál (xdosta51) - xdosta51@stud.fit.vutbr.cz             *
 ******************************************************************************/

 /**
 * @file graph_matrix.cpp
 * @author Oliver KUNÍK  (xkunik00)
 * @author Michal DOSTÁL (xdosta51)
 * @date 29.11.2022
 * @brief Default graph methods
 */

#include "graph_matrix.hpp"

#include <string> // std::string
#include <stdexcept> // std::invalid_argument
#include <inttypes.h> // SCNu32

GraphMatrix::GraphMatrix(const GraphList &g)
{
    nodes_count = g.get_nodes_count();
    c.resize(nodes_count * nodes_count, 0);

    const std::vector<std::map<uint32_t, uint32_t>>& list = g.get_list();

    for (uint32_t i = 0; i < nodes_count; ++i) {
        for (const auto &e : list[i]) {
            c[E(i, e.first)] = e.second;
        }
    }
}

GraphMatrix::GraphMatrix(std::istream &f)
{
    std::string line;

    if (!std::getline(f, line)) {
        throw std::invalid_argument( "Empty file" );
    }

    if (sscanf(line.c_str(), "%" SCNu32, &nodes_count) != 1) {
        throw std::invalid_argument( "Wrongly formated file" );
    };

    this->c.resize(nodes_count * nodes_count, 0);

    for (uint32_t i = 0; i < nodes_count * nodes_count; i += nodes_count) {

        if (!std::getline(f, line)) {
            throw std::invalid_argument( "Missing vertices" );
        }

        size_t pos;
        std::string token;

        uint32_t ev, c;

        while ((pos = line.find(" ")) != std::string::npos) {
            token = line.substr(0, pos);

            if (sscanf(token.c_str(), "%" SCNu32 "/%" SCNu32, &ev, &c) != 2) {
                throw std::invalid_argument( "Wrongly formated file" );
            };

            if (ev >= nodes_count) {
                throw std::invalid_argument( "Missing vertices" );
            }

            this->c[i + ev] = c;

            line.erase(0, pos + 1);
        }

        int retval = sscanf(line.c_str(), "%" SCNu32 "/%" SCNu32, &ev, &c);
        if (retval == 2) {
            if (ev >= nodes_count) {
                throw std::invalid_argument( "Missing vertices" );
            }

            this->c[i + ev] = c;
        }
        else if (retval != 0 && retval != EOF) {
            throw std::invalid_argument( "Wrongly formated file this" );
        }
    }
}

std::ostream& GraphMatrix::print(std::ostream &os) const
{
    os << nodes_count << std::endl;

    for (uint32_t i = 0; i < nodes_count * nodes_count; i += nodes_count) {
        for (uint32_t j = 0; j < nodes_count; ++j) {
            if (this->c[i + j] > 0) {
                os << j << "/" << this->c[i + j] << ' ';
            }
        }
        os << std::endl;
    }
    return os;
}

/************************** End of graph_matrix.cpp ***************************/
