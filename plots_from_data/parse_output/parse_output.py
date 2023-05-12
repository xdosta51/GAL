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


import multiprocessing

#Lists for times
list_ekm = []
list_ffm = []
list_ffmp = []
list_ekl = []
list_ffl = []
list_fflp = []


#Read file with data
file_with_graph = open("parse_output/" + "merged", "r")
input_file = file_with_graph.readlines()



#Iterate through lines
iterate = 0
max_len = len(input_file)



#Vars for times
ekm_time = 0
ffm_time = 0
ffmp_time = 0
ekl_time = 0
ffl_time = 0
fflp_time = 0

#Iterate through file
while iterate < max_len:
    
    #Get data to be used
    line_cutter = input_file[iterate].replace("network_generator/graphs/" , "").replace("\n", "").split(";")

  
    #Add time to the right list
    if (line_cutter[1]) == "EKM":
        ekm_time += float(line_cutter[5])/1000000
    elif (line_cutter[1]) == "FFM":
        ffm_time += float(line_cutter[5])/1000000
    elif (line_cutter[1]) == "FFMP":
        ffmp_time += float(line_cutter[5])/1000000
    elif (line_cutter[1]) == "EKL":
        ekl_time += float(line_cutter[5])/1000000
    elif (line_cutter[1]) == "FFL":
        ffl_time += float(line_cutter[5])/1000000
    elif (line_cutter[1]) == "FFLP":
        fflp_time += float(line_cutter[5])/1000000

    #Get number of nodes and number of iteration
    nodes_iter_cut = line_cutter[0].split("_")[:2]
    
    
    #If number of iteration is last calculate mean time
    if int(nodes_iter_cut[1]) == 49:
        
        temp = []
        temp.append(int(nodes_iter_cut[0]))
        
        if (line_cutter[1]) == "EKM":
            temp.append(ekm_time/50)
            list_ekm.append(temp)
            
            ekm_time = 0
        elif (line_cutter[1]) == "FFM":
            temp.append(ffm_time/50)
            list_ffm.append(temp)
            ffm_time = 0
        elif (line_cutter[1]) == "FFMP":
            temp.append(ffmp_time/50)
            list_ffmp.append(temp)
            ffmp_time = 0
        elif (line_cutter[1]) == "EKL":
            temp.append(ekl_time/50)
            list_ekl.append(temp)
            ekl_time = 0
        elif (line_cutter[1]) == "FFL":
            temp.append(ffl_time/50)
            list_ffl.append(temp)
            ffl_time = 0
        elif (line_cutter[1]) == "FFLP":
            temp.append(fflp_time/50)
            list_fflp.append(temp)
            fflp_time = 0

    iterate += 1

#List of nodes to be read
list_of_nodes = list(range(10,501))

#Names for graphs
names = ["Edmond_Karp_Matrix", "Ford_Fulkerson_Matrix", "Ford_Fulkerson_Matrix_Parallel (1 Thread)", "Edmond_Karp_List" , "Ford_Fulkerson_List" , "Ford_Fulkerson_List_Parallel (1 Thread)"]
iterate_names = 0

#Plot the figure
plt.figure()
plt.title('Speed of algorithms depending on the number of nodes') # Title of the plot

plt.xlabel('No. of nodes') # X-Label
plt.ylabel('Time [ms]') # Y-Label

#Sort time
def mySort(e):
    return e[0]

repair_time = 0
#Iterate through lists and make new lines in plots
for all_lists in [list_ekm, list_ffm, list_ffmp, list_ekl, list_ffl, list_fflp]:

    list_ekm_temp = []
    for nodes in range(10, 501):
        for item in all_lists:
            if item[0] == 43:
                repair_time = item[1]
            
            if item[0] == nodes:
                if item[0] == 44:
                    list_ekm_temp.append(repair_time)
                else:
                    list_ekm_temp.append(item[1])



    
    #Calculate running average
    average_y = []
    window = 30
    for ind in range(len(list_ekm_temp) - window + 1):
        average_y.append(np.mean(list_ekm_temp[ind:ind+window]))

    for ind in range(window - 1):
        average_y.insert(0, np.nan)
    
    
    plt.plot(list_of_nodes, average_y, label = str(names[iterate_names]))
    
    
    #plt.savefig(str(names[iterate_names]) + "_graph_time.png")
    iterate_names += 1
plt.legend()
plt.savefig("plot_all.png")
plt.show()