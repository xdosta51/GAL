/******************************************************************************
 * Project: Téma 21 - Paralelizace - Maximální tok v síti                     *
 * Course:  GAL - Graph Algorithms - FIT BUT                                   *
 * Year:    2022/2023                                                         *
 * Authors:                                                                   *
 *          Oliver Kuník  (xkunik00) - xkunik00@stud.fit.vutbr.cz             *
 *          Dostál Michal (xdosta51) - xdosta51@stud.fit.vutbr.cz             *
 ******************************************************************************/

 /**
 * @file main.cpp
 * @author Oliver KUNÍK  (xkunik00)
 * @author Michal DOSTÁL (xdosta51)
 * @date 28.11.2022
 * @brief Main
 */

#include <getopt.h> // getopt_long()
#include <fstream> // I/O
#include <cassert>
#include <limits>
#include <inttypes.h> // SCNu32
#include <chrono>
#include <omp.h>

#include "graph_matrix.hpp"
#include "graph_list.hpp"

#define NO_ERROR               0
#define ARGUMENT_PARSING_ERROR 1

#define DEBUG_R

#define EK 0
#define FF 1
#define FFP 2
#define PRL 3

struct AlgoM{
    std::string n;
    std::string sn;
    bool r;
    uint32_t (GraphMatrix::*a)(const uint32_t, const uint32_t) const;
};
struct AlgoL{
    std::string n;
    std::string sn;
    bool r;
    uint32_t (GraphList::*a)(const uint32_t, const uint32_t) const;
};

// Long options
const struct option long_options[] = {
    {"help",        no_argument,       0, 'h'},
    {"input-graph", required_argument, 0, 'i'},
    {"source",      required_argument, 0, 's'},
    {"sink",        required_argument, 0, 't'},
    {"count",       required_argument, 0, 'c'},
    {"num-threads", required_argument, 0, 'n'},
    {"csv-output",  no_argument,       0, 'o'},
    {"e-k-matrix",  no_argument,       0, 'a'},
    {"e-k-list",    no_argument,       0, 'b'},
    {"f-f-matrix",  no_argument,       0, 'd'},
    {"f-f-matrix-p",no_argument,       0, 'e'},
    {"f-f-list",    no_argument,       0, 'f'},
    {"f-f-list-p",  no_argument,       0, 'g'},
    {"push-rel",    no_argument,       0, 'j'},
    {0,             0,                 0, 0  }
};

// Help string
const std::string help_text =
"GAL - Maximum flow problem \n"
"Usage: \n"
"./GAL [-i FILE] [-s SOURCE] [-t SINK] [-c COUNT] [-n NUM] [-abdefg] [-o]\n"
"\n"
"  -h, --help              Display this help and exit.\n"
"  -i, --input-graph=FILE  Input file containing graph representation.\n"
"                          If no file is provided, expect graph on STDIN\n"
"  -s, --source=NODE       Source node. Default source is node 0\n"
"  -t  --sink=NODE         Sink node. Default sink is last node\n"
"  -c  --count=COUNT       Number of repetitions. Default is 1\n"
"  -n  --num-threads=NUM   Number of threads for parallel algorithms\n"
"  -o  --csv-output        Prints output in csv format\n"
"Possible algorithms:      (If no algorithm is specified, run all)\n"
"  -a  --e-k-matrix        Run Edmonds-Karp using matrix graph representation\n"
"  -b  --e-k-list          Run Edmonds-Karp using adj list graph representation\n"
"  -d  --f-f-matrix        Run Ford-Fulkerson using matrix graph representation\n"
"  -e  --f-f-matrix-p      Run parallel Ford-Fulkerson using matrix graph representation\n"
"  -f  --f-f-list          Run Ford-Fulkerson using adj list graph representation\n"
"  -g  --f-f-list-p        Run parallel Ford-Fulkerson using adj list graph representation\n"
"  -j  --push-rel          Run Push-Relabel using adj list graph representation\n";

int main(int argc, char *argv[])
{
    // Input file with graph
    char *input_file = nullptr;

    std::vector<AlgoM> am;
    am.push_back({"Edmonds-Karp algorithm on graph represented by matrix", "EKM", false, &GraphMatrix::edmonds_karp});
    am.push_back({"Ford-Fulkerson algorithm on graph represented by matrix", "FFM", false, &GraphMatrix::ford_fulkerson});
    am.push_back({"Ford-Fulkerson parallel algorithm on graph represented by matrix", "FFMP", false, &GraphMatrix::ford_fulkerson_parallel});

    std::vector<AlgoL> al;
    al.push_back({"Edmonds-Karp algorithm on graph represented by adj list", "EKL", false, &GraphList::edmonds_karp});
    al.push_back({"Ford-Fulkerson algorithm on graph represented by adj list", "FFL", false, &GraphList::ford_fulkerson});
    al.push_back({"Ford-Fulkerson parallel algorithm on graph represented by adj list", "FFLP", false, &GraphList::ford_fulkerson_parallel});
    al.push_back({"Push-Relabel algorithm on graph represented by adj list", "PRL", false, &GraphList::push_relabel});

    uint32_t source = std::numeric_limits<uint32_t>::max();
    uint32_t sink = std::numeric_limits<uint32_t>::max();
    size_t count = std::numeric_limits<size_t>::max();
    size_t num_threads = std::numeric_limits<size_t>::max();

    bool csv = false;

    // Argument parsing
    while (true) {
        // Parsing error
        bool parse_error = false;

        int option_index = 0;

        int c = getopt_long(argc, argv, "abc:defghi:jn:s:t:o", long_options, &option_index);

        // No more arguments
        if (c == -1) {
            break;
        }

        switch (c) {
        case 'a':
            if (am[EK].r) {
                parse_error = true;
            }
            am[EK].r = true;
            break;
        case 'b':
            if (al[EK].r) {
                parse_error = true;
            }
            al[EK].r = true;
            break;

        case 'c':
            if (count != std::numeric_limits<size_t>::max()) {
                parse_error = true;
            }
            if (sscanf(optarg, "%zu", &count) != 1) {
                parse_error = true;
            }
            break;
        case 'n':
            if (num_threads != std::numeric_limits<size_t>::max()) {
                parse_error = true;
            }
            if (sscanf(optarg, "%zu", &num_threads) != 1) {
                parse_error = true;
            }
            else {
                omp_set_dynamic(0);
                omp_set_num_threads(num_threads);
            }
            break;

        case 'd':
            if (am[FF].r) {
                parse_error = true;
            }
            am[FF].r = true;
            break;
        case 'e':
            if (am[FFP].r) {
                parse_error = true;
            }
            am[FFP].r = true;
            break;
        case 'f':
            if (al[FF].r) {
                parse_error = true;
            }
            al[FF].r = true;
            break;
        case 'g':
            if (al[FFP].r) {
                parse_error = true;
            }
            al[FFP].r = true;
            break;
        case 'j':
            if (al[PRL].r) {
                parse_error = true;
            }
            al[PRL].r = true;
            break;

        // Help
        case 'h':
            std::cerr << help_text;

            // Cannot combine with another argument
            if (argc > 2) {
                parse_error = true;
            }
            else {
                return NO_ERROR;
            }
            break;

        case 'i':
            if (input_file) {
                parse_error = true;
            }
            input_file = optarg;
            break;

        case 's':
            if (source != std::numeric_limits<uint32_t>::max()) {
                parse_error = true;
            }
            if (sscanf(optarg, "%" SCNu32, &source) != 1) {
                parse_error = true;
            }
            break;
        case 't':
            if (sink != std::numeric_limits<uint32_t>::max()) {
                parse_error = true;
            }
            if (sscanf(optarg, "%" SCNu32, &sink) != 1) {
                parse_error = true;
            }
            break;

        case 'o':
            if (csv) {
                parse_error = true;
            }
            csv = true;
            break;

        // Unrecognizable argument
        default:
            std::cerr << c << std::endl;
            parse_error = true;
        }

        if (parse_error) {
            std::cerr << "Argument parsing error" << std::endl;
            return ARGUMENT_PARSING_ERROR;
        }
    }

    bool runM = false;

    for (auto &a : am) {
        runM = runM || a.r;
    }

    bool runL = false;

    for (auto &a : al) {
        runL = runL || a.r;
    }

    if (!(runM || runL)) {
        for (auto &a : am) {
            a.r = true;
        }

        runM = true;

        for (auto &a : al) {
            a.r = true;
        }

        runL = true;
    }

    if (count == std::numeric_limits<size_t>::max()) {
        count = 1;
    }

    if (source == std::numeric_limits<uint32_t>::max()) {
        source = 0;
    }

    std::ifstream input_stream = input_file ? std::ifstream(input_file) : std::ifstream();
    GraphList g(input_file ? input_stream : std::cin);

    if (sink == std::numeric_limits<uint32_t>::max()) {
        sink = g.get_nodes_count() - 1;
    }

    if (sink >= g.get_nodes_count()) {
        std::cerr << "Argument parsing error" << std::endl;
        return ARGUMENT_PARSING_ERROR;
    }

#ifdef DEBUG_R
    uint32_t result = std::numeric_limits<uint32_t>::max();
#endif

    if (runM) {
        GraphMatrix gm(g);

        for (auto &a : am) {
            if (a.r) {
                if (!csv) {
                    std::cout << a.n << std::endl;
                }

                auto start = std::chrono::high_resolution_clock::now();

                for (size_t i = 0; i < count; ++i) {
#ifdef DEBUG_R
                    uint32_t r = (gm.*(a.a))(source, sink);
                    assert((result == std::numeric_limits<uint32_t>::max() || r == result));
                    result = r;
#else
                    (gm.*(a.a))(source, sink);
#endif
                }

                auto stop = std::chrono::high_resolution_clock::now();

                auto duration = stop - start;

                if (csv) {
                    std::cout << input_file << ";" << a.sn << ";" << num_threads << ";" << count << ";" << duration.count() << ";" << duration.count() / double(count) << std::endl;
                }
                else {
                    std::cout << "Total time: " << duration.count() / 1000 << " μs" << std::endl;
                    std::cout << "Time per iteration: " << duration.count() / count / 1000 << " μs" << std::endl;
                }
            }
        }
    }

    if (runL) {
        for (auto &a : al) {
            if (a.r) {
                if (!csv) {
                    std::cout << a.n << std::endl;
                }

                auto start = std::chrono::high_resolution_clock::now();

                for (size_t i = 0; i < count; ++i) {
#ifdef DEBUG_R
                    uint32_t r = (g.*(a.a))(source, sink);
                    assert((result == std::numeric_limits<uint32_t>::max() || r == result));
                    result = r;
#else
                    (g.*(a.a))(source, sink);
#endif
                }

                auto stop = std::chrono::high_resolution_clock::now();

                auto duration = stop - start;

                if (csv) {
                    std::cout << input_file << ";" << a.sn << ";" << num_threads << ";" << count << ";" << duration.count() << ";" << duration.count() / double(count) << std::endl;
                }
                else {
                    std::cout << "Total time: " << duration.count() / 1000 << " μs" << std::endl;
                    std::cout << "Time per iteration: " << duration.count() / count / 1000 << " μs" << std::endl;
                }
            }
        }
    }

#ifdef DEBUG_R
    if (!csv) {
        std::cout << "Maximum flow: " << result << std::endl;
    }
#endif

    return NO_ERROR;
}

/****************************** End of main.cpp *******************************/
