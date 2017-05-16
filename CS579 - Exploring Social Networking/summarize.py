"""
sumarize.py
"""
import pickle
import networkx as nx

COLLECTION_FILE = "data/collection_data"
CUSTER_FILE = "data/cluster_data"
POSITIVE = "data/positive_data"
NEGATIVE = "data/negative_data"

def read_tweets_from_file():
    def pickleLoader(pklFile):
        try:
            while True:
                yield pickle.load(pklFile)
        except EOFError:
            pass

    with open(COLLECTION_FILE, "rb") as f:
        return [event for event in pickleLoader(f)]



def main():
    tweets = read_tweets_from_file()
    users = set()
    for tweet in tweets:
        a = tweet['user_mentions']
        a.append(tweet['screen_id'])
        users |= set(a)

    print("Number of users collected:\t %d" %len(users))
    print("Number of messages collected:\t %d" %len(tweets))


    with open(CUSTER_FILE, "rb") as f:
        graph = pickle.load(f)
    components = [c for c in nx.connected_component_subgraphs(graph)]
    print("\nNumber of communities discovered:\t %d" %len(components))

    sum = 0
    for component in components:
        sum += len(component.nodes())
    print("Average number of users per community:\t %d" %(sum/len(components)))


    print("\nNumber of instances per class found:\t ")

    with open(POSITIVE, "rb") as f:
        positive = pickle.load(f)
    print("  -  Positive Instances:\t %d" %len(positive))

    with open(NEGATIVE, "rb") as f:
        negative = pickle.load(f)
    print("  -  Negative Instances:\t %d" % len(negative))

    print("\nOne example from each class:\t ")
    print("  -  Positive Instances Example:")
    print("\t", positive[0][1])

    print("  -  Negative Instances Example:")
    print("\t", negative[0][1])

if __name__ == '__main__':
    main()