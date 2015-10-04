from PyLiaison import *
import math
from collections import namedtuple

def makeQuat(x,y,z,theta):
	r = math.radians(theta)/2.
	c = math.cos(r)
	s = math.sin(r)
	return [c, s*x, s*y, s*z] #normalize...

import random
random.seed()

def randColor():
	return [random.random() for i in range (0,3)]+[1.]

r_ShaderSrc = { b'vert':b'simple.vert', 
				b'frag':b'simple.frag',
				b'Position':b'a_Pos'}

def InitCamera(camPtr):
	c_Camera = Camera(camPtr)
	c_Camera.InitOrtho([-10., 10.], [-10., 10.], [-10., 10.])

def GetWalls():
    return [-9.,-9.,18.,18.]

r_Entities = []
Entity = namedtuple('Entity', ('pos', 'scale', 'rot', 'color', 'module'))

# walls
r_Entities.append(Entity(
		pos = [-18.,0.],
		scale = [9.,9.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = 'AABB.py'
	))
r_Entities.append(Entity(
		pos = [18.,0.],
		scale = [9.,9.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = 'AABB.py'
	))
r_Entities.append(Entity(
		pos = [0.,18.],
		scale = [9.,9.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = 'AABB.py'
	))
r_Entities.append(Entity(
		pos = [0.,-18.],
		scale = [9.,9.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = 'AABB.py'
	))

r_Entities.append(Entity(
		pos = [6.,3.],
		scale = [1.,1.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = b'Circle.py'
	))
r_Entities.append(Entity(
		pos = [-6.,-3.],
		scale = [1.,1.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = b'Circle.py'
	))
r_Entities.append(Entity(
		pos = [2.,-2.],
		scale = [3.,3.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = b'Circle.py'
	))	

r_Entities.append(Entity(
		pos = [-3.,-3.],
		scale = [1.,1.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = b'Circle.py'
	))	

r_Entities.append(Entity(
		pos = [-1.,4.],
		scale = [2.,2.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = b'Circle.py'
	))	

r_Entities.append(Entity(
		pos = [-1.,0.],
		scale = [0.5,0.5],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = b'Circle.py'
	))	