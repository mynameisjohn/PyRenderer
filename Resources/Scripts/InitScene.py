from PyLiaison import *
import math
from collections import namedtuple
import random
random.seed()

def makeQuat(x,y,z,theta):
	r = math.radians(theta)/2.
	c = math.cos(r)
	s = math.sin(r)
	return [c, s*x, s*y, s*z] #normalize...
def randColor():
	return [random.random() for i in range (0,3)]+[1.]

r_ShaderSrc = { b'vert':b'simple.vert', 
				b'frag':b'simple.frag',
				b'Position':b'a_Pos'}

def InitCamera(camPtr):
	c_Camera = Camera(camPtr)
	c_Camera.InitOrtho([-10., 10.], [-10., 10.], [-10., 10.])
		
r_Entities = []
Entity = namedtuple('Entity', ('pos', 'scale', 'rot', 'color', 'module'))

r_Entities.append(Entity(
		pos = [0.,0.,0.],
		scale = [1.,1.,1.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = b'circle.py'
	))
r_Entities.append(Entity(
		pos = [0.,7.,0.],
		scale = [1.,1.,1.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = b'circle.py'
	))
r_Entities.append(Entity(
		pos = [-2.,-2.,0.],
		scale = [1.,1.,1.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = b'circle.py'
	))	