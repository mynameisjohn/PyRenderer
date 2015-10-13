from PyLiaison import *

import random
import os

random.seed()

def randColor():
	return [random.random() for i in range (0,3)]+[1.]

r_IqmFile = b'quad.iqm'
p = SND_DIR
r_Sounds = [x for x in os.listdir(p) if os.path.isfile(p+os.sep+x)]

r_ColPrim = 'OBB'

g_Entities = {}

#Updatecollision just posts the translate message
def UpdateCollision(id):
    g_Entities[id].PostMessage(0,0)

def HandleCollision(myID, theirID):
	e1 = g_Entities[myID]
	#e2 = g_Entities[myID]
	e1.PostMessage_v4(0,1,randColor())
	#e2.PostMessage_v4(0,1,randColor())
	#PlaySound(random.choice(r_Sounds))
	#print(str(e1)+' is colliding with '+str(e2))
	
def AddEntity(eID, ePtr):
	g_Entities[eID] = Entity(ePtr)

#def AddEntity(ePtr):
## I think this is kosher? Things are stored twice...
#    g_Entities.append(Entity(ePtr))
#    print(ePtr)
#    return len(g_Entities)-1
