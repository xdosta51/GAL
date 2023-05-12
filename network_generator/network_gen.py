import pynetgen
import os

def gen_network(rand_nodes, density):
    #Generate Random Flow Network.
    err_code = pynetgen.netgen_generate(seed=-1, mincost=1, maxcost=1, sources=1 ,sinks=1, nodes=rand_nodes, fname="out", rng=1, density=density)
    
    #Open file with flow network and read it.
    f = open("out", "r")
    output = f.readlines()

    #Variables to be used.
    nodes = 0
    density = 1
    #Variables for source and sink
    source = 0
    sink = 0

    #List of routes.
    list_of_routes = []

    #Parse file with graph and change it to format to be used as stdin in c++ Max Flow Computation.
    for l in output:
        if (l.strip().split('\n')[0][0] == "p"):
            #print (l.strip().split('\n')[0])
            separedp = l.strip().split('\n')[0].split(' ')
            nodes = separedp[2]
            density = separedp[3]
            
        if (l.strip().split('\n')[0][0] == "n"):
            #print (l.strip().split('\n')[0])
            separedn = l.strip().split('\n')[0].split(' ')
            if separedn[2] == "s":
                source = separedn[1]
                
            elif separedn[2] == "t":
                sink = separedn[1]
                
            

        if (l.strip().split('\n')[0][0] == "a"):
            #print (l.strip().split('\n')[0])
            separeda = l.strip().split('\n')[0].split(' ')[1:]
            list_of_routes.append(separeda)

    
    #Remove file with graph, no more needed.
    os.remove("out")


    #Debugging prints..

    #print("nodes:", nodes)
    #print("density:", density)
    #print("source:", source)
    #print("sink:", sink)
    #print("List of routes", list_of_routes)

    #Change the format of routes.
    list_of_new_routes = []

    for i2 in range(int(nodes)):
        
        for i in range(len(list_of_routes)):
            
            if int(list_of_routes[i][0]) == i2+1:
                list_of_new_routes.append(list_of_routes[i][0] + "/" + str(int(list_of_routes[i][1])-1) + "/" + list_of_routes[i][2])

    #Change the format of routes.
    output_graph = nodes + "\n"

    for i in range(int(nodes)):
        for line in list_of_new_routes:
            if (int(line.split("/")[0]) == i+1):
                line = line.split('/')
                output_graph += str(line[1]) + "/" + str(line[2]) + " "
        output_graph += "\n"

    return output_graph, source, sink