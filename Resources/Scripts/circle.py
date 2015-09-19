from PyLiaison import *

r_IqmFile = b'circle.iqm'

class Entity:
	def __init__(self, drPtr, cPtr):
		self.dr = Drawable(drPtr)
		self.c = Circle(cPtr)

g_Entities = []

def sayHello(ptr):
	print(str(ptr)+" says hello")
	
def isColliding():
	print('Collision Detected')
	
def HandleCollision(id, tX, tY):
	e = g_Entities[id]
	e.dr.Translate([tX, tY, 0.])
	
def AddEntity(drPtr, cPtr):
	g_Entities.append(Entity(drPtr, cPtr))
	return len(g_Entities)-1