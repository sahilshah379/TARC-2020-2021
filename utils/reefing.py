import math

class Rocket:
    def __init__(self, preferences):
        self.mass = preferences['mass']

        self.tLower = preferences['tLower']
        self.tUpper = preferences['tUpper']
        self.tTarget = (self.tUpper+self.tLower)/2.0
        self.ascentTime = preferences['ascentTime']

        self.apogee = self.m_ft(preferences['apogee'])
        self.height = self.m_ft(preferences['reefingAltitude'])

        self.updateRate = preferences['updateInterval']
        self.gravity = 9.80665 # gravity

        self.v0 = math.sqrt(2*self.gravity*(self.apogee-self.height))
        self.k2 = 0.5*1.5*1.225*self.m_ft(self.m_ft(4.7124)) #parachute reference area
        self.t2 = self.tTarget - self.ascentTime - math.sqrt((2*(self.apogee-self.height))/self.gravity)

    def m_ft(self, ft):
        return ft/3.281
    def vt(self, k):
        return math.sqrt(self.mass*self.gravity/k)
    def C(self, v0, vt):
        return (v0-vt)/(v0+vt)
    def v_t(self, v0, k, t):
        return -self.vt(k)/math.tanh(0.5*math.log(self.C(v0,self.vt(k)))-k*self.vt(k)*t/self.mass)
    def h_t(self, v0, k, t):
        return (self.mass/k)*math.log(math.cosh(0.5*math.log(self.C(v0,self.vt(k)))-k*self.vt(k)*t/self.mass))
    def h(self, v0, k1, t1):
        return self.h_t(v0,k1,t1)+self.h_t(self.v_t(v0,k1,t1),self.k2,self.t2-t1)


def main():
    preferences = {
        'mass': 0.635,
        'tLower': 40,
        'tUpper': 43,
        'apogee': 800,
        'reefingAltitude': 400,
        'ascentTime': 7,
        'updateInterval': 1
    }
    r = Rocket(preferences)
    print(r.t2)
    print(r.height)
    print(r.h(r.v0, 0.21, 4))
    
if __name__ == '__main__':
	main()


# https://keisan.casio.com/exec/system/1231475371
# http://www.ambrsoft.com/Physics/FreeFall/FreeFallWairResistance.htm