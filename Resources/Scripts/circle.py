from PyLiaison import *

import random
random.seed()

def randColor():
	return [random.random() for i in range (0,3)]+[1.]

r_IqmFile = b'circle.iqm'

g_Entities = []

#Updatecollision just posts the translate message
def UpdateCollision(id):
    g_Entities[id].PostMessage(0,0)

def HandleCollision(id1, id2):
	e1 = g_Entities[id1]
	e2 = g_Entities[id2]
	e1.PostMessage_v4(0,1,randColor())
	e2.PostMessage_v4(0,1,randColor())
	#print(str(e1)+' is colliding with '+str(e2))
	
def AddEntity(ePtr):
# I think this is kosher? Things are stored twice...
    g_Entities.append(Entity(ePtr))
    print(ePtr)
    return len(g_Entities)-1