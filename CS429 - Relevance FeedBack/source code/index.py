# Python 3.0
import re
import collections
import math
from operator import add
from read import read_data
from relevance_feedback import Precision, Recall, MAP
import csv

alpha, beta, gamma = 1, 0.75, 0.15

class Index:
    def __init__(self, docs, stopwords):
        self.docs = docs
        self.documents = [self.tokenize(doc, stopwords) for doc in self.docs]

        self.doc_freqs = self.calculate_doc_frequencies()
        self.index = self.buildIndex()
        self.doc_lengths = self.calculate_doc_lengths()
        self.doc_norms = self.compute_doc_norms()


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
        return vector


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

        for each_query, query_idf in query_vector.items():
            if each_query in self.index:
                for lists in self.index[each_query]:
                    docID, tf_val = lists
                    tf_weight = (1.0 + math.log10(tf_val)) * idf(each_query)
                    scores[docID] += (tf_weight * query_idf)

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


    # Query to rettrived top k relevent documents (Exact match - cosine)
    def query(self, query, k=10):
        # function for exact top K retrieval using cosine similarity
        # Returns at the minimum the document names of the top K documents ordered in decreasing order of similarity score

        query_vector = self.query_to_vector(query)
        cosine_scores = self.calculate_cosine_scores(query_vector)

        cosine_scores = {key: round(values, 6) for key, values in cosine_scores.items()}

        lists = sorted(cosine_scores.items(), key=lambda k: -k[1])
        return [int(temp[0]) for temp in lists][:k]


    # Calculate number of tokens in document
    def to_vector(self, query_terms, dictinary):
        query_dict = []
        count = collections.Counter(query_terms)
        for el in dictinary:
            query_dict.append(count[el])

        return query_dict


    # Implementation of Rocchio Algorithm
    def rocchio(self, query_terms, pos_feedback, neg_feedback, alpha, beta, gamma):
        # function to implement rocchio algorithm
        # pos_feedback - documents deemed to be relevant by the user
        # neg_feedback - documents deemed to be non-relevant by the user
        # Return the new query terms and their weights

        dictionary = list(query_terms.keys())

        for docID in pos_feedback + neg_feedback:
            dictionary.extend(self.documents[docID-1])

        dictionary = list(set(dictionary))
        n = len(dictionary)

        query_vector = []
        for i in range(n):
            if dictionary[i] in query_terms:
                query_vector.append(query_terms[dictionary[i]])
            else:
                query_vector.append(0)

        # Summing positive feedback
        sum_pos_feedback = [0 for i in range(n)]
        for pos in pos_feedback:
            sum_pos_feedback = list(map(add, sum_pos_feedback, self.to_vector(self.documents[pos-1], dictionary)))

        # summing negative feedback
        sum_neg_feedback = [0 for i in range(n)]
        for neg in neg_feedback:
            sum_neg_feedback = list(map(add, sum_neg_feedback, self.to_vector(self.documents[neg-1], dictionary)))

        # calculating each term in rocchio algothm
        #   a = alpha * query
        #   b = beta  / total_number_of_pos_feedback * pos_feedback
        #   c = gamma / total_number_of_neg_feedback * neg_feedback

        a = [i * alpha for i in query_vector]
        b = [i * beta  / len(pos_feedback) for i in sum_pos_feedback] if len(pos_feedback) > 0 else sum_pos_feedback
        c = [i * gamma / len(neg_feedback) for i in sum_neg_feedback] if len(neg_feedback) > 0 else sum_neg_feedback

        # FORMULA: a + b - c
        return {dictionary[i]: a[i] + b[i] - c[i] for i in range(n)}


    # Function for relevence feedback
    #   - isPsuedoFeedback = False  :   User   Relevance Feedback
    #   - isPsuedoFeedback = True   :   Psuedo Relevance Feedback

    def run_relevance_feedback(self, key, query, relevant, isPsuedoFeedback, rel):
        print("\nQuery -", key, query)
        print("Precision\t Recall\t MAP")
        evaluators = [Precision(), Recall(), MAP()]
        results = collections.defaultdict(lambda : dict())
        query_token = self.tokenize(query)
        query_vector = {query_token[i]: vector for i, vector in enumerate(self.to_vector(query_token, query_token))}

        for i in range(5):
            hits = self.query(query_token, len(relevant))

            if isPsuedoFeedback:
                pos_feedback = hits[:3]
                neg_feedback = hits[3:]
            else:
                pos_feedback = rel[i]
                neg_feedback = list(set(hits) - set(pos_feedback))

            for evaluator in evaluators:
                results[i][str(evaluator)] = evaluator.evaluate(hits, relevant)

            query_vector = self.rocchio(query_vector, pos_feedback, neg_feedback, alpha, beta, gamma)
            query_token = list(query_vector.keys())

            results[i]["id"] = key
            results[i]["query"] = query
            results[i]["pos_feed_back"] = pos_feedback
            results[i]["neg_feed_back"] = neg_feedback
            results[i]["new_query"] = dict(sorted(query_vector.items(), key= lambda x : -x[1])[:10])

            print(round(results[i]["Precision"], 2), "\t\t", round(results[i]["Recall"], 2), "\t",
                  round(results[i]["MAP"], 2))

        self.writedataTocsv(results)


    # Function to print the terms and posting list in the index
    def print_dict(self):
        for token in self.index:
            print(token, [(i[0], i[2]) for i in self.index[token][1:]])

    # Function to print the documents and their document id
    def print_doc_list(self):
        for docID, _ in enumerate(self.documents):
            print("Doc ID: ", docID, " ==> ", docID + 17)

    # Function to tokenize string and remove stopwords
    def tokenize(self, document, stopwords=list()):
        # return [t.lower() for t in re.findall(r"\w+(?:[-']\w+)*", document) if t not in stopwords]
        return [item.lower() for item in re.findall('[\w]+', document.lower()) if item not in stopwords]

    # Function to write data into csv
    def writedataTocsv(self, data):
        with open('document.csv', 'a') as f:
            for i in data:
                w = csv.DictWriter(f, data[i].keys())
                w.writeheader()
                w.writerow(data[i])



# To run rocchio - Part A (Assignment)
def run_rocchio_algorithm():
    docs = ["good movie trailer", "shown trailer with good actor", "unseen movie"]
    query = "movie trailer"
    indexer = Index(docs, [])

    query_token = indexer.tokenize(query)
    query_vector = {query_token[i]: vector for i, vector in enumerate(indexer.to_vector(query_token, query_token))}
    new_query = indexer.rocchio(query_vector, [1, 2], [3], alpha, beta, gamma)
    print(new_query)



# To run relevence feedback - Part B and extra credit (Assignment)
def relevance_feedback(isPsuedoFeedback):
    queries, relevances, docs, stopwords = read_data()
    docs = [docs[i] for i in range(17, max(list(docs.keys())))]

    indexer = Index(docs, stopwords)

    rel = {
        1: {
            0: [374, 398, 304, 380],
            1: [374, 304, 380],
            2: [304, 268, 326],
            3: [304, 268, 326],
            4: [304, 268, 326]
        },
        6: {
            0: [304, 398, 402, 380, 374],
            1: [304, 398, 402],
            2: [304, 398, 402],
            3: [304, 398, 402],
            4: [304, 398, 402]
        },
        31: {
            0: [47, 72, 100],
            1: [47, 72, 100, 10],
            2: [47, 72, 100, 10],
            3: [47, 72, 100, 10],
            4: [47, 72, 100, 10]
        }
    }

    for key in [1, 6, 31]:
        query = queries[key]

        if len(relevances[key]) > 5:
            indexer.run_relevance_feedback(key, query, relevances[key], isPsuedoFeedback, rel[key])


def main():

    print("Part A - Rocchio Alogorithm")
    print("---------------------------")
    run_rocchio_algorithm()

    print("\n\nPart B - Relevance Feedback")
    print("-----------------------------")
    relevance_feedback(False)

    print("\n\nExtra Credit - Psuedo Relevance Feedback")
    print("------------------------------------------")
    relevance_feedback(True)


if __name__ == '__main__':
    main()
