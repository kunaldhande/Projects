
class Precision():
    def __repr__(self):
        return 'Precision'

    def evaluate(self, hits, relevant):
        try:
            return float(len(set(hits) & set(relevant)) / len(hits))
        except:
            return 0.0


class Recall():
    def __repr__(self):
        return 'Recall'

    def evaluate(self, hits, relevant):
        try:
            return float( len(set(hits) & set(relevant)) / len(relevant) )
        except:
            return 0.0


class MAP():
    def __repr__(self):
        return 'MAP'

    def evaluate(self, hits, relevant):
        K = float( len(relevant) )
        count, prec = 0.0, 0.0
        
        for i, h in enumerate(hits):
            if h in relevant:
                count += 1.0
                prec += float(count / (i + 1))
        
        try:
            return float(prec / K)
        except:
            return 0.0



