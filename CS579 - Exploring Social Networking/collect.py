"""
collect.py
"""

import sys
import time
from TwitterAPI import TwitterAPI, TwitterRestPager
import pickle
import os
import configparser

COLLECTION_FILE = "data/collection_data"
COUNT = 1000


def get_twitter():
    config = configparser.ConfigParser()
    config.read("twitter.cfg")
    return TwitterAPI(
                   config.get('twitter', 'consumer_key'),
                   config.get('twitter', 'consumer_secret'),
                   config.get('twitter', 'access_token'),
                   config.get('twitter', 'access_token_secret'))


def save_to_file(data):
    file = open(COLLECTION_FILE, 'ab')
    pickle.dump(data, file)   
    file.close()


def search_tweets(term):
    count = 0
    r = TwitterRestPager(get_twitter(), 'search/tweets', {'q':term, 'count':COUNT, 'lang': 'en'})
    for r in r.get_iterator():
        if 'message' in r and r['code'] == 88:
            print('Got error... Sleeping for 15 minutes.')
            sys.stderr.flush()
            time.sleep(61 * 15)
            
        d = {}
        d['id'] = r['id_str']
        d['screen_name'] = r['user']['screen_name']
        d['screen_id'] = r['user']['id_str']
        d['tweet'] = r['text']
        d['followcount'] = r['user']['friends_count']
        d['user_mentions'] = [user['id_str'] for user in r['entities']['user_mentions']]
        if d['user_mentions']:
            if len(d['user_mentions'])>1:
                save_to_file(d)
                count += 1
                
        if count > COUNT-1:
            break

    return count




def main():
    if not os.path.exists("data"):
        os.makedirs("data")
    if os.path.isfile(COLLECTION_FILE):
        os.remove(COLLECTION_FILE)

    print("Collecting tweets:")
    count = search_tweets("@realdonaldtrump")
    print("Tweets collected: ", str(count) )
    count += search_tweets("@narendramodi")
    print("Tweets collected: ", str(count))

if __name__ == '__main__':
    main()



