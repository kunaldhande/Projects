# Python 3.0
import re
import os
import collections
import time
import numpy as np

alpha = 0.15
EPS = 1E-3

# Calculating power vectors
def powerVector(M, r_old):
    while True:
        r = np.matmul(r_old, M)

        # Checking if pagerank vector is repeating??
        if np.linalg.norm(r_old - r) < EPS:
            return r
        r_old = r


# Calculating page rank
def page_rank(mat, k):
    N, _ = mat.shape

    # Calculating transition matrix with teleportation
    for i, row in enumerate(mat):
        no_of_1 = np.count_nonzero(row)
        for j, value in enumerate(row):
            if no_of_1:
                mat[i][j] = alpha/N if value == 0 else (1 - alpha) / no_of_1 + (alpha / N)
            else:
                mat[i][j] = 1/N

    # Calculating power vectors = alpha * transition matrix with teleportation
    rank = powerVector(mat, np.array([alpha for i in range(N)]))

    # sorting documents score wrt pagerank
    rank = {i: k for i, k in enumerate(rank)}

    # return top k docs with score
    return sorted(rank.items(), key=lambda x:x[1])[:k]



def main():
    fnames = ["test1.txt", "test2.txt"]
    k = 10
    for fname in fnames:
        print(fname, "-")
        with open(fname) as f:
            # Read number of nodes
            n = int(f.readline())

            # Read number of edges
            e = int(f.readline())

            # Generate matrix using edges
            Matrix = np.zeros((n, n), dtype=np.float64)
            for x in f:
                a, b = map(int, x.split())
                Matrix[a][b] = 1

        rank = page_rank(Matrix, k)
        print("Doc\t Page Rank Score")
        print("============================")
        for doc, score in rank:
            print(doc,"\t", score)
        print("============================\n")

if __name__ == "__main__":
    main()
