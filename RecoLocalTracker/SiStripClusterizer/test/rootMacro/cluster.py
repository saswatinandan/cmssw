class Cluster():
    def __init__(self, idx=0., event=0., run=0., lumi=0., detId=0.,
                  firstStrip=0., endStrip=0., barycenter=0., size=0., charge=0.,
                  matchLevel=-1):

        self.idx = idx
        self.event = event
        self.run = run
        self.lumi = lumi
        self.detId = detId
        self.firstStrip = firstStrip
        self.endStrip = endStrip
        self.barycenter = barycenter
        self.size = size
        self.charge = charge
        self.matchLevel = matchLevel

    def print(self):
        print("idx: " ,self.idx, ", event: ", self.event,
              ", run: ", self.run, ", lumi: ",self.lumi,
              ", detId: ", self.detId, ", firstStrip: ", self.firstStrip,
              ", endStrip: ", self.endStrip,
              ", barycenter: ", self.barycenter,
              ", size: ", self.size,
              ", charge: ", self.charge,
              ", matchLevel: ", self.matchLevel)
