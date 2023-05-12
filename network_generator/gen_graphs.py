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



try:
    os.mkdir("./network_generator/graphs/")
except:
    pass

#Calculate average of time list.
def Average(lst):
    return sum(lst) / len(lst)




#list_of_mean_degree = []
range_of_nodes = range(10,501)
print("Range of nodes to be computed: " + str(list(range_of_nodes)[0]) + " - "  + str(list(range_of_nodes)[-1]))
list_iter_nodes = list(range_of_nodes)


#range_mean_degree = 100 - 25
#iteration_mean_degree = range_mean_degree / 50



#For loop through range of nodes.
for iter_nodes in range_of_nodes:
    #Calculate mean time.
   
    #mean_degree = []
    #Loop through different graphs..
    

    for mean_time_iter in range(50):
        
        

        rand_nodes = iter_nodes
        density = 0
        #Calculate density of graphs, density = amount of paths.
        if (int(rand_nodes) >= 50):
            density = pow((int(rand_nodes)/3), 2)
        else:
            density = int(int(rand_nodes)*4) 

        #density = rand_nodes * multiple_density
        #density = random.choice(rand_density)
        #mean_degree.append(density)
        
        output_graph, source, sink = network_gen.gen_network(rand_nodes, density)
        file = open("./network_generator/graphs/" + str(iter_nodes) + "_" + str(mean_time_iter) + "_Graph", "w")
 
        #convert variable to string
        
        file.write(str(output_graph))
        file.close()
        
        

  
    #list_of_mean_degree.append(Average(mean_degree))
    #Debugging print (To know if its still running.. xD)
    if iter_nodes % 10 == 0:
        print("Actual number of nodes:" + str(iter_nodes))


