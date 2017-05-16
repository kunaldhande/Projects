import collections
import tarfile

###############################################
#                   Read Data
###############################################

def read_data():
    tarfile.open('time.tar.gz', mode='r').extractall()

    queries = read_queries('TIME.QUE')
    print('read %d queries.' % len(queries))

    relevances = read_relevances('TIME.REL')
    print('read %d relevance judgements.' % len(relevances))

    docs = read_documents('TIME.ALL')
    print('read %d documents.' % len(docs))

    stopwords = read_stopwords('TIME.STP')
    print('read %d stopwords.' % len(stopwords))

    return queries, relevances, docs, stopwords


def read_queries(fname):
    docs = collections.defaultdict(lambda: str())
    idx = -1

    for line in open(fname).readlines():
        if len(line.split()) != 0:
            if line.startswith('*FIND'):
                try:
                    idx = int(line.split()[1])
                except IndexError:
                    continue

            elif line.startswith('*STOP'):
                continue

            else:
                docs[idx] += line.strip().lower() + ' '

    return docs


def read_documents(fname):
    docs = collections.defaultdict(lambda : str())
    idx = -1

    for line in open(fname).readlines():
        if len(line.split()) != 0:
            if line.startswith('*TEXT'):
                try:
                    idx = int(line.split()[1])
                except IndexError:
                    continue

            elif line.startswith('*STOP'):
                continue

            else:
                docs[idx] += line.strip().lower() + ' '

    return docs


def read_relevances(fname):
    result = collections.defaultdict(list)

    for line in open(fname).readlines():

        if len(line.split()) != 0:
            doc_id = int(line.split()[0])
            rest_part = [int(x) for x in line.split()[1:] if len(x)]

            try:
                result[doc_id] = rest_part
            except IndexError:
                continue
    return result


def read_stopwords(fname):
    return [stp.strip() for stp in open(fname).readlines() if len(stp.strip()) > 0]