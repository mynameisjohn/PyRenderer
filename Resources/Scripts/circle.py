from PyLiaison import *

r_IqmFile = b'circle.iqm'

g_Entities = []
	
def HandleCollision(id):
	e = g_Entities[id]
	e.PostMessage(0, 0) #drawable : tranlsate
	
def AddEntity(ePtr):
# I think this is kosher? Things are stored twice...
    g_Entities.append(Entity(ePtr))
    return len(g_Entities)-1