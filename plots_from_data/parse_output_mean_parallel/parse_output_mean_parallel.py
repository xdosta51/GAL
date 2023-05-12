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
import re

import multiprocessing

#Subplot
fig, (ax1, ax2) = plt.subplots(1, 2)
fig.suptitle('Speed of parallel algorithms depending on the size of mean degree')

ax1.set_title("FFMP by No. of threads and FFM Single Core")
ax2.set_title("FFLP by No. of threads and FFM Single Core")

ax1.set(xlabel='Mean degree size', ylabel='Time [ms]')
ax2.set(xlabel='Mean degree size', ylabel='Time [ms]')

def getint(name):
    result = re.search('network_generator/graphs_mean_degree/(.*)_Graph;', name)
    return int(result.group(1).split("_")[0])

#Iterate through all files
for opening_file in ["merged_mean_n1", "merged_mean_n2", "merged_mean_n4", "merged_mean_n8", "merged_mean_n16", "merged_mean_n32", "merged_mean_n64", "merged_mean_n128"]:

    #Lists of times
    list_ekm = []
    list_ffm = []
    list_ffmp = []
    list_ekl = []
    list_ffl = []
    list_fflp = []


    #Open file with data
    file_with_graph = open("parse_output_mean_parallel/" + opening_file, "r")
    input_file = file_with_graph.readlines()
    #Sort file
    input_file.sort(key=getint)

    

    #Iterate through lines
    iterate = 0
    max_len = len(input_file)

    #Local variables for times
    ekm_time = 0
    ffm_time = 0
    ffmp_time = 0
    ekl_time = 0
    ffl_time = 0
    fflp_time = 0

    #Iterating through lines in file
    while iterate < max_len:
        


        #Get needed data
        line_cutter = input_file[iterate].replace("network_generator/graphs_mean_degree/" , "").replace("\n", "").split(";")

        
        

        #Add time to the right variable and calculate time in ms
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

        #Get the number of nodes and number of iteration
        nodes_iter_cut = line_cutter[0].split("_")[:2]
        
        
        #There is only one iteration
        if int(nodes_iter_cut[1]) == 4:
            
            temp = []
            temp.append(int(nodes_iter_cut[0]))
            
            if (line_cutter[1]) == "EKM":
                temp.append(ekm_time/5)
                list_ekm.append(temp)
                
                ekm_time = 0
            elif (line_cutter[1]) == "FFM":
                temp.append(ffm_time/5)
                list_ffm.append(temp)
                ffm_time = 0
            elif (line_cutter[1]) == "FFMP":
                temp.append(ffmp_time/5)
                list_ffmp.append(temp)
                ffmp_time = 0
            elif (line_cutter[1]) == "EKL":
                temp.append(ekl_time/5)
                list_ekl.append(temp)
                ekl_time = 0
            elif (line_cutter[1]) == "FFL":
                temp.append(ffl_time/5)
                list_ffl.append(temp)
                ffl_time = 0
            elif (line_cutter[1]) == "FFLP":
                temp.append(fflp_time/5)
                list_fflp.append(temp)
                fflp_time = 0

        iterate += 1
    #Size of mean degree
    list_of_nodes = list(range(26,251))

    names = ["Ford_Fulkerson_Matrix_Parallel", "Ford_Fulkerson_List_Parallel"]
    iterate_names = 0

    #Plot the figure and save it to file.
    

    def mySort(e):
        return e[0]

    repair_time = 0


    #FFM Single core plot
    if opening_file == "merged_mean_n1":
        list_ekm_temp = []
        for nodes in range(26, 251):
            for item in list_ffm:
                if item[0] == nodes:
                        list_ekm_temp.append(item[1])

        #poly = np.polyfit(list_of_nodes,list_ekm_temp,2)
        #poly_y = np.poly1d(poly)(list_of_nodes)
        
        

        average_y = []
        window = 30
        for ind in range(len(list_ekm_temp) - window + 1):
            average_y.append(np.mean(list_ekm_temp[ind:ind+window]))

        for ind in range(window - 1):
            average_y.insert(0, np.nan)
        ax1.plot(list_of_nodes, average_y, label = ("Ford_Fulkerson_Matrix_Single_Core"), ls='--')
        ax2.plot(list_of_nodes, average_y, label = "Ford_Fulkerson_Matrix_Single_Core", ls='--')

    #Plot FFMP according to thread
    list_ekm_temp = []
    for nodes in range(26, 251):
        for item in list_ffmp:
            if item[0] == nodes:
                    list_ekm_temp.append(item[1])


    poly = np.polyfit(list_of_nodes,list_ekm_temp,2)
    poly_y = np.poly1d(poly)(list_of_nodes)
    
    average_y = []
    window = 30
    for ind in range(len(list_ekm_temp) - window + 1):
        average_y.append(np.mean(list_ekm_temp[ind:ind+window]))

    for ind in range(window - 1):
        average_y.insert(0, np.nan)



    ax1.plot(list_of_nodes, average_y, label = ("Ford_Fulkerson_Matrix_Parallel_" + opening_file.replace("merged_mean_n", "Threads: ")))

    #Plot FFLP According to thread
    list_ekm_temp = []
    for nodes in range(26, 251):
        for item in list_fflp:
            if item[0] == nodes:
                    list_ekm_temp.append(item[1])


    poly = np.polyfit(list_of_nodes,list_ekm_temp,2)
    poly_y = np.poly1d(poly)(list_of_nodes)

    average_y = []
    window = 30
    for ind in range(len(list_ekm_temp) - window + 1):
        average_y.append(np.mean(list_ekm_temp[ind:ind+window]))

    for ind in range(window - 1):
        average_y.insert(0, np.nan)


    ax2.plot(list_of_nodes, average_y, label = "Ford_Fulkerson_List_Parallel_" + opening_file.replace("merged_mean_n", "Threads: "))
    
    
    #plt.savefig(str(names[iterate_names]) + "_graph_time.png")
   
ax1.legend()
ax2.legend()
plt.savefig("plot_all_mean_parallel.png")
plt.show()