﻿from PyLiaison import *

import random
import os

random.seed()

def randColor():
	return [random.random() for i in range (0,3)]+[1.]

r_IqmFile = b'quad.iqm'
p = SND_DIR
separator = bytes(os.sep.encode('ascii')) # Thanks python 3...
r_Sounds = [x for x in os.listdir(p) if os.path.isfile(p+separator+x)]

r_ColPrim = 'OBB'

g_Entities = {}

#Updatecollision just posts the translate message
def UpdateCollision(id):
    g_Entities[id].PostMessage(E_DR, E_DR_TR)

def HandleCollision(myID, theirID):
	e1 = g_Entities[myID]
	e1.PostMessage_v4(E_DR, E_DR_CLR, (randColor(),))
	PlaySound(random.choice(r_Sounds))
	
def AddEntity(eID, ePtr):
	g_Entities[eID] = Entity(ePtr)

#def AddEntity(ePtr):
## I think this is kosher? Things are stored twice...
#    g_Entities.append(Entity(ePtr))
#    print(ePtr)
#    return len(g_Entities)-1
