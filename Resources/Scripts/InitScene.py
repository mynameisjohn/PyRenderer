from PyLiaison import *
import math
from collections import namedtuple
from platform import system

CONTACT_DEBUG = True

def makeQuat(x,y,z,theta):
	r = math.radians(theta)/2.
	c = math.cos(r)
	s = math.sin(r)
	return [c, s*x, s*y, s*z] #normalize...

import random
random.seed()

def randColor():
	return [random.random() for i in range (0,3)]+[1.]

r_ShaderSrc = { 'vert':'simple.vert', 
				'frag':'simple.frag',
				'Position':'a_Pos'}

if (system() == 'Darwin'):
	r_ShaderSrc['vert'] = 'mac_'+r_ShaderSrc['vert']
	r_ShaderSrc['frag'] = 'mac_'+r_ShaderSrc['frag']

def InitCamera(camPtr):
	c_Camera = Camera(camPtr)
	c_Camera.InitOrtho([-10., 10.], [-10., 10.], [-10., 10.])

def GetWalls():
    return [-9.,-9.,18.,18.]

r_Entities = []
Entity = namedtuple('Entity', 
	('vel', 'pos', 'scale', 'rot', 'mass', 'el', 'color', 'module'))

r_Entities.append(Entity(
		vel = [0.,0.],
		pos = [0.,-2.],
		scale = [1.,1.],
		rot = .2,
		mass = 1.,
		el = 1.,
		color = randColor(),
		module = 'OBB.py'
	))

r_Entities.append(Entity(
		vel = [0.,-10.],
		pos = [0.,2.],
		scale = [1.,1.],
		rot = 0.,
		mass = 1.,
		el = 1.,
		color = randColor(),
		module = 'AABB.py'
	))
