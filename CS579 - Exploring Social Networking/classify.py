"""
classify.py
"""

import urllib.request
import zipfile
import urllib.request
import pandas as pd
from sklearn.feature_extraction.text import CountVectorizer
import numpy as np
import os.path
from sklearn.linear_model import LogisticRegression
import pickle

COLLECTION_FILE = "data/collection_data"
CLASSIFY_FILE = "data/classification_data"
POSITIVE = "data/positive_data"
NEGATIVE = "data/negative_data"


def load_training_data():
    if not os.path.isfile("trainingandtestdata.zip"):
        url = 'http://cs.stanford.edu/people/alecmgo/trainingandtestdata.zip'
        urllib.request.urlretrieve(url, 'data/trainingandtestdata.zip')
        zfile = zipfile.ZipFile('data/trainingandtestdata.zip')
        zfile.extractall("data/")
        zfile.close()

    tweets = pd.read_csv('data/training.1600000.processed.noemoticon.csv',
                         header=None,
                         names=['polarity', 'id', 'date', 'query', 'user', 'text'],
                         encoding='cp1252')

    return tweets


def save_model_to_file(data):
    file = open("data/model", 'ab')
    pickle.dump(data, file)
    file.close()


def read_tweets_from_file():
    def pickleLoader(pklFile):
        try:
            while True:
                yield pickle.load(pklFile)
        except EOFError:
            pass

    with open(COLLECTION_FILE, "rb") as f:
        return [event for event in pickleLoader(f)]



def init_model():
    if os.path.isfile("data/model"):
        with open("data/model", "rb") as f:
            model, vectorizer = pickle.load(f)
    else:
        print("Loading sentiment traning data")
        tweets = load_training_data()

        y = np.array(tweets['polarity'])

        vectorizer = CountVectorizer(min_df=1, ngram_range=(1, 1))
        X = vectorizer.fit_transform(tweets['text'])
        print('Instances: %d\nFeatures: %d' % (X.shape[0], X.shape[1]))

        print("\nInitializing Logistic Regression Model")

        model = LogisticRegression()
        model.fit(X, y)
        save_model_to_file([model, vectorizer])
    return model, vectorizer

def save_to_file(data, file):
    file = open(file, 'wb')
    pickle.dump(data, file)
    file.close()


def main():
    search_results = read_tweets_from_file()

    model, vectorizer = init_model()

    print("\nCalculating sentiment for tweets collecting collect.py")
    X_test = vectorizer.transform([t['tweet'] for t in search_results])
    predicted = model.predict(X_test)

    neutral, negative, positive = [],[], []

    for i in range(len(search_results)):
        search_results[i]['sentiment'] = predicted[i]
        if predicted[i] == 0:
            negative.append((predicted[i], search_results[i]['tweet']))
        elif predicted[i] == 2:
            neutral.append((predicted[i], search_results[i]['tweet']))
        elif predicted[i] == 4:
            positive.append((predicted[i], search_results[i]['tweet']))

    print("Totals Tweets: ", len(search_results))
    print("Positive tweets: ", len(positive))
    print("Negative tweets: ", len(negative))

    save_to_file(positive, POSITIVE)
    save_to_file(negative, NEGATIVE)

if __name__ == '__main__':
    main()
