#******************************************************************************
 # Project: Téma 21 - Paralelizace - Maximální tok v síti                     *
 # Course:  GAL - Graph Algorithms - FIT BUT                                  *
 # Year:    2022/2023                                                         *
 # Authors:                                                                   *
 #          Oliver Kuník  (xkunik00) - xkunik00@stud.fit.vutbr.cz             *
 #          Dostál Michal (xdosta51) - xdosta51@stud.fit.vutbr.cz             *
 #*****************************************************************************/

#Imports .. 
import pynetgen
import random
import sys
import os
import subprocess
import time
import matplotlib.pyplot as plt 
import numpy as np
import network_gen
import multiprocessing

try:
    os.mkdir("./network_generator/graphs_mean_degree/")
except:
    pass

rand_nodes = 500

#Parse args for number of nodes...
if (len(sys.argv) > 1):
    rand_nodes = sys.argv[1]
    


mean_degree = range(25,251)
print("Range of mean degree to be computed: " + str(list(mean_degree)[0]) + " - "  + str(list(mean_degree)[-1]))
list_iter_nodes = list(mean_degree)





#For loop through range of nodes.
for mean in mean_degree:
    
  
    

    for mean_time_iter in range(5):
        
        

        nodes = rand_nodes
        density = int(rand_nodes) * mean
        
        output_graph, source, sink = network_gen.gen_network(rand_nodes, density)
        
        file = open("./network_generator/graphs_mean_degree/" + str(mean) + "_" + str(mean_time_iter) + "_Graph", "w")
 
        #convert variable to string
        
        file.write(str(output_graph))
        file.close()

        
      
     

    #Debugging print (To know if its still running.. xD)
    if mean % 10 == 0:
        print("Actual size of mean degree:" + str(mean))


