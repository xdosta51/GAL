/******************************************************************************
 * Project: Téma 21 - Paralelizace - Maximální tok v síti                     *
 * Course:  GAL - Graph Algorithms - FIT BUT                                  *
 * Year:    2022/2023                                                         *
 * Authors:                                                                   *
 *          Oliver Kuník  (xkunik00) - xkunik00@stud.fit.vutbr.cz             *
 *          Michal Dostál (xdosta51) - xdosta51@stud.fit.vutbr.cz             *
 ******************************************************************************/

 /**
 * @file graph_list.cpp
 * @author Oliver KUNÍK  (xkunik00)
 * @author Michal DOSTÁL (xdosta51)
 * @date 29.11.2022
 * @brief Default graph methods
 */

#include "graph_list.hpp"

#include <string> // std::string
#include <stdexcept> // std::invalid_argument
#include <inttypes.h> // SCNu32

GraphList::GraphList(std::istream &f)
{
    std::string line;

    if (!std::getline(f, line)) {
        throw std::invalid_argument( "Empty file" );
    }

    uint32_t count;

    if (sscanf(line.c_str(), "%" SCNu32, &count) != 1) {
        throw std::invalid_argument( "Wrongly formated file" );
    };

    this->e.resize(count);

    for (auto &v : this->e) {

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

            if (ev >= count) {
                throw std::invalid_argument( "Missing vertices" );
            }

            v.insert({ev, c});

            line.erase(0, pos + 1);
        }

        int retval = sscanf(line.c_str(), "%" SCNu32 "/%" SCNu32, &ev, &c);
        if (retval == 2) {
            if (ev >= count) {
                throw std::invalid_argument( "Missing vertices" );
            }

            v.insert({ev, c});
        }
        else if (retval != 0 && retval != EOF) {
            throw std::invalid_argument( "Wrongly formated file this" );
        }
    }
}

std::ostream& GraphList::print(std::ostream &os) const
{
    os << this->e.size() << std::endl;

    for (auto &v : this->e) {
        for (auto &e : v) {
            os << e.first << "/" << e.second << ' ';
        }
        os << std::endl;
    }
    return os;
}

/****************************** End of graph_list.cpp ******************************/
