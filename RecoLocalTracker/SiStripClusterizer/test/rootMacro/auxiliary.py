from enum import Enum, auto
from math import sqrt

class cut(Enum):
    nocut = auto()
    dzSig = auto()
    chi2 = auto()
    ptRes = auto()
    nhits = auto()

    @classmethod
    def max_value(cls):
        return cls.nhits.value

def deltaR(e1, e2, p1, p2):

    dp = abs(p1 -p2);
    if (dp > math.pi):
        dp -= float(2 * math.pi);
    return sqrt(pow((e1 - e2), 2) + pow(dp, 2));
