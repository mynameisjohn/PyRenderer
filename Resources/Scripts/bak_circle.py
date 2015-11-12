from PyLiaison import *

import random
import os

random.seed()

def randColor():
	return [random.random() for i in range (0,3)]+[1.]

r_IqmFile = b'circle.iqm'
p = SND_DIR
separator = bytes(os.sep.encode('ascii')) # Thanks python 3...
r_Sounds = [x for x in os.listdir(p) if os.path.isfile(p+separator+x)]

r_ColPrim = 'Circle'

def HandleCollision(myID, theirID):
    e = g_Entities[myID]
    e.SendMessage_D(E_DR, E_DR_CLR, (randColor(),))
    try: # random.choice can raise an IndexError if seq is empty
        snd = random.choice(r_Sounds)
        PlaySound(snd)
    except IndexError:
        pass
	
def AddEntity(eID, ePtr):
	g_Entities[eID] = Entity(ePtr)
