#Python 3.0
import re
import os
import collections
import time
import math
import random
from read import read_data
from numpy import linalg as LA
import numpy as np

class kmeans:
    # Init
    def __init__(self, docs, stopwords):
        self.docs = docs
        self.documents = [self.tokenize(doc, stopwords) for doc in self.docs]

        self.doc_freqs = self.calculate_doc_frequencies()
        self.index = self.buildIndex()
        self.doc_lengths = self.calculate_doc_lengths()
        self.doc_norms = self.compute_doc_norms()
        self.dictionary = sorted(self.index.keys())
        self.document_vector = self.calculate_doc_vector()

    # Calculating document norms
    def compute_doc_norms(self):
        N = len(self.documents)
        docNorms = collections.defaultdict(int)
        for word, lists in self.index.items():
            for docID, tf in lists:
                df_weight = float(math.log10(N / self.doc_freqs[word])) if self.doc_freqs[word] > 0 else 0.0
                tf_weight = float(1.0 + math.log10(tf))

                docNorms[docID] = docNorms[docID] + ((tf_weight * df_weight) ** 2)

        return {key: float(value ** 0.5) for key, value in docNorms.items()}


    # Calculating document length
    def calculate_doc_lengths(self):
        docLength = collections.defaultdict(float)

        for key, list1 in self.index.items():
            for docID, lent in list1:
                docLength[docID] += float(lent)

        return docLength

    # Calculate document frequency of each token
    def calculate_doc_frequencies(self):
        docFrequency = collections.defaultdict(int)

        for tokens in self.documents:
            for word in set(tokens):
                docFrequency[word] += 1.0

        return docFrequency


    # Calculating query vector
    def query_to_vector(self, query_terms):
        vector = collections.defaultdict(float)
        N = len(self.documents)
        for query in query_terms:
            df = self.doc_freqs[query]
            if df > 0:
                vector[query] = math.log10(N / df)
        return self.to_vector(vector)


    def calculate_doc_vector(self):
        return [self.query_to_vector(doc) for doc in self.documents]


    # Calculate number of tokens in document
    def to_vector(self, vector):
        query_dict = []
        count = collections.Counter(vector)
        for el in self.dictionary:
            query_dict.append(count[el])

        return query_dict


    # Calculate cosine scores
    def calculate_cosine_scores(self, query_vector):
        # calculate IDF
        def idf(term):
            df = float(self.doc_freqs[term])
            N = float(len(self.documents))

            try:
                return math.log10(N / df)
            except:
                return 0.0

        scores = collections.defaultdict(int)

        for id, value in enumerate(query_vector):
            each_query= self.dictionary[id]
            if each_query in self.index:
                for lists in self.index[each_query]:
                    docID, tf_val = lists
                    tf_weight = (1.0 + math.log10(tf_val)) * idf(each_query)
                    scores[docID] += (tf_weight * value)

        for docID in scores:
            scores[docID] = scores[docID]/self.doc_norms[docID] if self.doc_norms[docID]> 0 else 0.0

        return scores


    # Indexing documents
    def buildIndex(self):
        index = collections.defaultdict(list)
        for i, tokens in enumerate(self.documents):
            for toks in set(tokens):
                index[toks].append([i+1, float(tokens.count(toks))])
        return index


    # Find RSS
    def findRSS(self, centroids, clusters):
        avg_rss = 0
        print("Cluster\tRSS\t\t\t\tClosest Doc")

        # Iterate all centroids
        for i, centroid in enumerate(centroids):
            rss = 0
            cen = np.array(centroid)
            minn = math.inf
            closest_doc = 0
            for cluster in clusters[i]:
                c = np.array(self.document_vector[cluster])
                # Calculating l2norm
                l2 = LA.norm(np.subtract(cen, c))

                if minn > l2:
                    minn = l2
                    closest_doc = cluster

                minn = min(minn, l2)
                rss = np.add(rss, l2)
            print("%s\t\t%s\t%s" %(i, rss, closest_doc))
            avg_rss += rss
        return avg_rss


    # Calculate Centroids
    def findCentroids(self, clusters, centroid):
        for cluster in clusters.keys():
            c = centroid[cluster]
            for d in clusters[cluster]:
                for i, tok in enumerate(self.document_vector[d]):
                    c[i] += tok

            for i, tok in enumerate(c):
                c[i] /= len(clusters[cluster])

        return centroid


    # Assigning clusters
    def findClusters(self, centroids):
        ds = [self.calculate_cosine_scores(c) for c in centroids]

        d = collections.defaultdict(float)
        for k, _ in enumerate(self.documents):
            n = [d[k] for d in ds]
            d[k] = n.index(min(n))

        out = collections.defaultdict(list)
        for k, v in d.items():
            out[v].append(k)

        return out


    def clustering(self, kvalue):
        #function to implement kmeans clustering algorithm
        #Print out:
        ##For each cluster, its RSS values and the document ID of the document closest to its centroid.
        ##Average RSS value
        ##Time taken for computation.

        if len(self.documents) < kvalue:
            kvalue = len(self.documents)

        centroids = [self.query_to_vector(c) for c in random.sample(self.documents, kvalue)]

        avg_rss = 0
        for i in range(5):
            clusters = self.findClusters(centroids)
            centroids = self.findCentroids(clusters, centroids)
            rss = self.findRSS(centroids, clusters)
            avg_rss += rss
            print("")

        print("Average RSS:", avg_rss/5)
        return centroids


    # Function to tokenize string and remove stopwords
    def tokenize(self, document, stopwords=list()):
        # return [t.lower() for t in re.findall(r"\w+(?:[-']\w+)*", document) if t not in stopwords]
        return [item.lower() for item in re.findall('[\w]+', document.lower()) if item not in stopwords]


def main():
    start = time.clock()
    docs, stopwords = read_data()
    docs = [doc for i, doc in list(docs.items())]
    indexer = kmeans(docs, stopwords)
    print("Time to index: ", round(time.clock() - start, 3), "Seconds")


    ### PART A
    print("PART A")

    print("\nEnter number of clusters: ")
    indexer.clustering(int(input()))
    print("\nTime to cluster: ", round(time.clock() - start, 3), "Seconds")

    ### PART B

    print("\n\nPART B")

    start = time.clock()
    for i in range(2, 31):
        print("\nNumber of clusters:", i)
        indexer.clustering(i)
    print("\nTime to cluster: ", round(time.clock() - start, 3), "Seconds")

if __name__ == '__main__':
    main()