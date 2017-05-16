#Python 3.0
import re
import os
import time
from collections import defaultdict
import math
import random
#import other modules as needed

class index:
    def __init__(self, path):

        # Read documents
        documents = {name: open(os.path.join(path, name)).read() for _, _, files in os.walk(path) for name in files
                          if name.endswith(".txt")}
        self.stopwords = open('stop-list.txt').readlines()


        # Store it in form of tokens and assign docID
        self.doc_list = {}
        self.toked_docs = {}
        for docID, d in enumerate(documents):
            self.doc_list[docID] = d
            self.toked_docs[docID] = self.tokenize(documents[d])

        # Building index
        self.doc_freqs = self.calculate_doc_frequencies(self.toked_docs)
        self.buildIndex()
        self.doc_lengths = self.calculate_doc_lengths(self.index)



    # Building index for exact quest, inexact query - Champion list and inextact query - Cluster pruning
    def buildIndex(self):
        #function to read documents from collection, tokenize and build the index with tokens
        # implement additional functionality to support methods 1 - 4
        #use unique document integer IDs

        print("Building tf-idf Index: ")
        self.index = self.build_tfidf_index(self.toked_docs, self.doc_freqs, True)

        print("Building Champion Index: ")
        self.champion_index = self.build_champion_index(self.index, 10)

        print("Building cluster pruning: ")
        self.cluster_pruning, self.cluster_pruning_leader_index = self.build_cluster_pruning_index()


    # Building tf-idf index
    def build_tfidf_index(self, docs, doc_freqs, printt):

        start = time.time()

        tfidf_index = defaultdict(list)

        for docID in docs:
            doc_dict = defaultdict(list)
            for index, term in enumerate(docs[docID]):
                doc_dict[term].append(index)
            self.get_tf_idf(docID, doc_dict, docs, doc_freqs, tfidf_index)

        sorted(tfidf_index.keys())

        done = time.time()

        if printt:
            print("Indexed in ", '{0:.10f}'.format(done - start), "seconds\n")
        return tfidf_index


    # Calculating tf-idf index
    def get_tf_idf(self, doc_id, term_dict, docs, doc_freqs, weight_dict):
        for word in term_dict:
            wt = (1 + math.log10(len(term_dict[word])))
            idf = math.log10(len(docs) / doc_freqs[word])

            # tf_idf_weight = wt * idf
            if len(weight_dict[word]) == 0:
                weight_dict[word].append(idf)
            weight_dict[word].append([doc_id, wt, term_dict[word]])

        return weight_dict


    # Building Champion list index
    def build_champion_index(self, index, r=0):

        start = time.time()

        champion_index = defaultdict(list)
        for word in index:
            champion_index[word] = index[word] + sorted(index[word][1:], key=lambda k: k[1])[:r]

        done = time.time()
        print("Indexed in ", '{0:.10f}'.format(done - start), "seconds\n")
        return champion_index


    # Building cluster pruning index
    def build_cluster_pruning_index(self):
        start = time.time()

        weight_dict = defaultdict(list)
        index = set(self.toked_docs.keys())
        # Determine leaders for cluster pruning
        leaders_no = math.ceil(math.sqrt(len(index)))
        leaders = set(random.sample(index, leaders_no))
        index -= leaders

        # Calculate index for leaders
        toked_docs = {key: self.toked_docs[key] for key in leaders}
        doc_freqs = self.calculate_doc_frequencies(toked_docs)
        doc_index = self.build_tfidf_index(toked_docs, doc_freqs, False)
        doc_lengths = self.calculate_doc_lengths(doc_index)

        # Detemining followers of each leader
        for doc in index:
            query_vector = self.query_to_vector(self.toked_docs[doc])
            cosine_scores = self.calculate_cosine_scores(query_vector, doc_index, doc_lengths)
            if len(cosine_scores)>0:
                weight_dict[cosine_scores[0][0]].append(doc)

        done = time.time()
        print("Indexed in ", '{0:.10f}'.format(done - start), "seconds\n")

        return weight_dict, doc_index


    # Calculate cosine scores
    def calculate_cosine_scores(self, query_vector, index, doc_lengths):
        scores = defaultdict(int)
        for query_term, query_wt in query_vector.items():
            if len(index[query_term]) != 0:
                idf = index[query_term][0]
                for doc_id, doc_wt, _ in index[query_term][1:]:
                    scores[doc_id] += query_wt * doc_wt * idf

        for idx in scores:
            try:
                scores[idx] /= doc_lengths[idx]
            except ZeroDivisionError:
                # Check for divide by zero error
                scores[idx] = 0

        return sorted(scores.items(), key=lambda k: k[1], reverse=True)


    # Calculating query vector
    def query_to_vector(self, query_terms):
        query_dict = defaultdict(int)
        for query in query_terms:
            N = len(self.toked_docs)
            df = self.doc_freqs[query]
            try:
                query_dict[query] = math.log10(N / df)
            except ZeroDivisionError:
                query_dict[query] = 0
        return query_dict

    # Calculating document length
    def calculate_doc_lengths(self, index):
        my_dict = defaultdict(int)
        for ky, big_list in index.items():
            idf = big_list[0]
            for doc_id in big_list[1:]:
                my_dict[doc_id[0]] = my_dict[doc_id[0]] + ((idf*doc_id[1]) ** 2)

        for val in my_dict:
            my_dict[val] = math.sqrt(my_dict[val])

        return my_dict


    # Calculate document frequency of each token
    def calculate_doc_frequencies(self, docs):
        freq_dict = defaultdict(int)

        for tokens in docs:
            temp_dict = defaultdict(int)

            for tok in docs[tokens]:
                if tok not in temp_dict:
                    temp_dict[tok] = 1

            for word in temp_dict:
                freq_dict[word] += 1
        return freq_dict


    def exact_query(self, query_terms, k):
        #function for exact top K retrieval (method 1)
        #Returns at the minimum the document names of the top K documents ordered in decreasing order of similarity score

        start = time.time()

        query_vector = self.query_to_vector(query_terms)
        cosine_scores = self.calculate_cosine_scores(query_vector, self.index, self.doc_lengths)[:k]

        done = time.time()
        print("Exact query -")
        for docID, cosine in cosine_scores:
            print(self.doc_list[docID])
        print("Retrieved in", '{0:.10f}'.format(done - start), "seconds\n")


    def inexact_query_champion(self, query_terms, k):
        #function for exact top K retrieval using champion list (method 2)
        #Returns at the minimum the document names of the top K documents ordered in decreasing order of similarity score

        start = time.time()

        query_vector = self.query_to_vector(query_terms)
        cosine_scores = self.calculate_cosine_scores(query_vector, self.champion_index, self.doc_lengths)[:k]

        done = time.time()
        print("Inexact query : Champion -")
        for docID, cosine in cosine_scores:
            print(self.doc_list[docID])
        print("Retrieved in", '{0:.10f}'.format(done - start), "seconds\n")




    def inexact_query_index_elimination(self, query_terms, k):
        #function for exact top K retrieval using index elimination (method 3)
        #Returns at the minimum the document names of the top K documents ordered in decreasing order of similarity score

        start = time.time()

        index = {query : self.index[query] for query in query_terms}
        index = dict(sorted(index.items(), key= lambda x: x[1], reverse= True)[:int(len(query_terms)/2)])

        query_vector = self.query_to_vector(index.keys())
        cosine_scores = self.calculate_cosine_scores(query_vector, index, self.doc_lengths)[:k]

        done = time.time()
        print("Inexact query : Index elimination - ")
        for docID, cosine in cosine_scores:
            print(self.doc_list[docID])
        print("Retrieved in", '{0:.10f}'.format(done - start), "seconds\n")


    def inexact_query_cluster_pruning(self, query_terms, k):
        #function for exact top K retrieval using cluster pruning (method 4)
        #Returns at the minimum the document names of the top K documents ordered in decreasing order of similarity score

        start = time.time()

        query_vector = self.query_to_vector(query_terms)
        result = []
        cosines = self.calculate_cosine_scores(query_vector, self.cluster_pruning_leader_index, self.doc_lengths)

        for cosine in cosines:
            if len(result) > k:
                break
            result.append(cosine[0])
            result.extend(self.cluster_pruning[cosine[0]])

        done = time.time()
        print("Inexact query : Cluster Pruning - ")
        for docID in result[:k]:
            print(self.doc_list[docID])
        print("Retrieved in", '{0:.10f}'.format(done - start), "seconds\n")


    def print_dict(self):
        #function to print the terms and posting list in the index
        for token in self.index:
            print(token, [(i[0], i[2]) for i in self.index[token][1:]])

    def print_doc_list(self):
        # function to print the documents and their document id
        for docID in self.doc_list:
            print("Doc ID: ", docID, " ==> ", self.doc_list[docID])

    def tokenize(self, document):
        return [item for item in re.findall('[\w]+', document.lower()) if item not in self.stopwords]




def main():
    """ DO NOT MODIFY.
    Main method. Constructs an Index object and runs a sample query. """
    indexer = index(os.path.join('collection/'))


    queries = [['with', 'without', 'yemen'], ['with', 'without', 'yemen', 'yemeni'], ['rest', 'ready', 'more'], ['with', 'ready', 'the', 'war'], ['time', 'political', 'less', 'then']]


    for query in queries:
        print(query)
        print("===============================================")
        indexer.exact_query(query, 10)
        indexer.inexact_query_champion(query, 10)
        indexer.inexact_query_index_elimination(query, 10)
        indexer.inexact_query_cluster_pruning(query, 10)
        print("")

    indexer.print_dict()
    indexer.print_doc_list()

if __name__ == '__main__':
    main()
