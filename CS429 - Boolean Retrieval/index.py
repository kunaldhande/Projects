# coding=utf-8
# Python 3.5.2

import re
import os
import collections
import time

class index:
    def __init__(self, path):
        """ Reading documents from the disk """
        self.documents = {name: open(os.path.join(path, name)).read() for _, _, files in os.walk(path) for name in files if name.endswith(".txt")}
        self.buildIndex()


    # function to read documents from collection, tokenize and build the index with tokens
    # index should also contain positional information of the terms in the document --- term: [(ID1,[pos1,pos2,..]), (ID2, [pos1,pos2,…]),….]
    # use unique document IDs
    def buildIndex(self):
        start = time.time()

        self.doc_list = {}
        self.result = collections.defaultdict(lambda: collections.defaultdict(list))

        """ iterating to each document """
        for docID, d in enumerate(self.documents):
            self.doc_list[docID] = d
            """ tokenizing each document and iterating through them to build idex with coreesponding docID and
                its respective position in the document."""
            for index, token in enumerate(self.tokenize(self.documents[d])):
                self.result[token][docID].append(index)

        done = time.time()
        print("Index built in", done - start, "seconds\n")


    # function for identifying relevant docs using the index
    def and_query(self, query_terms):
        start = time.time()

        """ retrieving documentIDs of the quey terms """
        result_dict = {query: sorted([doc_id for doc_id in self.result[query]]) for query in query_terms}
        query_terms = sorted(query_terms, key = lambda x: -len(result_dict[x]))

        """ solving problem with an algorithm for merging posting lists """
        result = result_dict[query_terms[0]]
        for query in query_terms[1:]:
            result = self.intersect(result, result_dict[query])

        done = time.time()

        """ Printing results """
        print("Results for the Query: ", " AND ".join(query_terms))
        print("Total Docs retrieved: ", len(result))

        for id in result:
            print(self.doc_list[id])
        print("Retrieved in", done - start, "seconds\n")


    # function to print the terms and posting list in the index
    def print_dict(self):
        for token in self.result:
            print(token, list(self.result[token].items()))


    # function to print the documents and their document id
    def print_doc_list(self):
        for docID in self.doc_list:
            print("Doc ID: ", docID, " ==> ", self.doc_list[docID])


    def tokenize(self, document):
        return re.findall('[\w]+', document.lower())


    def intersect(self, list1, list2):
        i, j = 0 ,0
        intersect = []
        while i < len(list1) and j < len(list2):
            """ if values beign compared are equal, add value to answer array and advance both pointers """
            if list1[i] == list2[j]:
                intersect.append(list1[i])
                i += 1
                j += 1
            else:
                # if value in list1 is less than value in list2,advance pointer to list1
                if list1[i] < list2[j]:
                    i += 1

                # if value in list1 is greater than value in list2,advance pointer to list2
                else:
                    j += 1
        return intersect


def main():
    idx = index(os.path.join('collection/'))
    idx.and_query(['with', 'without', 'yemen'])
    idx.and_query(['with', 'without', 'yemen', 'yemeni'])
    idx.and_query(['rest', 'ready', 'more'])
    idx.and_query(['with', 'ready', 'the', 'war'])
    idx.and_query(['time', 'political', 'less', 'then'])
    idx.print_dict()
    idx.print_doc_list()

if __name__ == '__main__':
    main()