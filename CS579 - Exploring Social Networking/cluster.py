"""
cluster.py
"""
import pickle
import operator
from matplotlib import pylab as plt
import networkx as nx

COLLECTION_FILE = "data/collection_data"
CUSTER_FILE = "data/cluster_data"


def read_tweets_from_file():
    def pickleLoader(pklFile):
        try:
            while True:
                yield pickle.load(pklFile)
        except EOFError:
            pass

    with open(COLLECTION_FILE, "rb") as f:
        return [event for event in pickleLoader(f)]



def show_graph(graph, filename):
    #initialze Figure
    plt.figure(num=None, figsize=(20, 20), dpi=80)
    plt.axis('off')
    fig = plt.figure(1)
    pos = nx.spring_layout(graph)
    nx.draw_networkx_nodes(graph,pos)
    nx.draw_networkx_edges(graph,pos)

    cut = 1.00
    xmax = cut * max(xx for xx, yy in pos.values())
    ymax = cut * max(yy for xx, yy in pos.values())
    plt.xlim(0, xmax)
    plt.ylim(0, ymax)
    
    # plt.show()
    plt.savefig(filename, bbox_inches="tight")
    plt.close()
    del fig




def create_example_graph(edges):
    graph = nx.Graph()
    graph.add_edges_from(edges)
    return graph




def partition_girvan_newman(graph):
    G = graph.copy()

    betweenness = sorted(nx.edge_betweenness_centrality(G,normalized=False).items(), key=operator.itemgetter(1))
    components = [c for c in nx.connected_component_subgraphs(G)]

    while len(components) == 1:
        
        try:
            edge_to_remove = betweenness.pop()[0]
            G.remove_edge(*edge_to_remove)
            components = [c for c in nx.connected_component_subgraphs(G)]
        except:
            pass
       

    return G

def save_to_file(data):
    file = open(CUSTER_FILE, 'wb')
    pickle.dump(data, file)
    file.close()


def main():
    print("Reading tweets collected in Collect.py")
    search_results = read_tweets_from_file()

    edges = [(user1['screen_id'], user2) for user1 in search_results for user2 in user1['user_mentions']]

    print("Genearting graph")
    graph = create_example_graph(edges)
    show_graph(graph, "original_graph.png")

    print("\nSaving graph to original_graph.png \n")
    print("Total number of nodes: ", len(graph.nodes()))
    print("Total number of edges: ", len(graph.edges()))


    print("\n\nRunning girvan_newman for community detection")
    g = partition_girvan_newman(graph)
    print("Community detection done")

    print("\nNumber of communities: ", str(len([c for c in nx.connected_component_subgraphs(g)])))

    print("\nSaving cluster graph to cluster_graph.png")
    show_graph(g, "cluster_graph.png")
    print("Saving done")
    save_to_file(g)


if __name__ == '__main__':
    main()
