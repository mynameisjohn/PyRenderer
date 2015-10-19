from PyLiaison import *
import math
from collections import namedtuple
from platform import system

CONTACT_DEBUG = False

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
Entity = namedtuple('Entity', ('vel', 'pos', 'scale', 'rot', 'color', 'module'))

r_Entities.append(Entity(
		vel = [0.,0.],
		pos = [-18.,0.],
		scale = [9.,9.],
		rot = 0.,
		color = randColor(),
		module = 'AABB.py'
	))

r_Entities.append(Entity(
		vel = [0.,0.],
		pos = [18.,0.],
		scale = [9.,9.],
		rot = 0.,
		color = randColor(),
		module = 'AABB.py'
	))

r_Entities.append(Entity(
		vel = [0.,0.],
		pos = [0.,18.],
		scale = [9.,9.],
		rot = 0.,
		color = randColor(),
		module = 'AABB.py'
	))

r_Entities.append(Entity(
		vel = [0.,0.],
		pos = [0.,-18.],
		scale = [9.,9.],
		rot = 0.,
		color = randColor(),
		module = 'AABB.py'
	))

r_Entities.append(Entity(
		vel = [-10.,-10.],
		pos = [8.,0.],
		scale = [1.,1.],
		rot = 50.,
		color = randColor(),
		module = 'OBB.py'
	))


r_Entities.append(Entity(
		vel=[0.,0.],
		pos = [5.,0.],
		scale = [1.,1.],
		rot = math.pi / 4.,
		color = randColor(),
		module = 'OBB.py'
	))


r_Entities.append(Entity(
		vel=[10.,0.],
		pos = [-2.,0.],
		scale = [1.,1.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = 'circle.py'
	))

r_Entities.append(Entity(
		vel=[-1.,10.],
		pos = [0.,-8.],
		scale = [1.,1.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = 'OBB.py'
	))

r_Entities.append(Entity(
		vel=[0.,-10.],
		pos = [0.,8.],
		scale = [1.,1.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = 'circle.py'
	))

r_Entities.append(Entity(
		vel=[-2.,-10.],
		pos = [3.,5.],
		scale = [1.,1.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = 'circle.py'
	))

r_Entities.append(Entity(
		vel=[5.,3.],
		pos = [-3.,-5.],
		scale = [1.,1.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = 'circle.py'
	))

r_Entities.append(Entity(
		vel=[3.,-7.],
		pos = [-4.,8.],
		scale = [2.,2.],
		rot = makeQuat(0.,0.,1.,0.),
		color = randColor(),
		module = 'circle.py'
	))
# r_Entities.append(Entity(
		# pos = [-18.,0.],
		# scale = [9.,9.],
		# rot = makeQuat(0.,0.,1.,0.),
		# color = randColor(),
		# module = 'AABB.py'
	# ))
# r_Entities.append(Entity(
		# pos = [18.,0.],
		# scale = [9.,9.],
		# rot = makeQuat(0.,0.,1.,0.),
		# color = randColor(),
		# module = 'AABB.py'
	# ))
# r_Entities.append(Entity(
		# pos = [0.,18.],
		# scale = [9.,9.],
		# rot = makeQuat(0.,0.,1.,0.),
		# color = randColor(),
		# module = 'AABB.py'
	# ))
# r_Entities.append(Entity(
		# pos = [0.,-18.],
		# scale = [9.,9.],
		# rot = makeQuat(0.,0.,1.,0.),
		# color = randColor(),
		# module = 'AABB.py'
	# ))

# r_Entities.append(Entity(
		# pos = [6.,3.],
		# scale = [1.,1.],
		# rot = makeQuat(0.,0.,1.,0.),
		# color = randColor(),
		# module = b'Circle.py'
	# ))
# r_Entities.append(Entity(
		# pos = [-6.,-3.],
		# scale = [1.,1.],
		# rot = makeQuat(0.,0.,1.,0.),
		# color = randColor(),
		# module = b'Circle.py'
	# ))
# r_Entities.append(Entity(
		# pos = [2.,-2.],
		# scale = [3.,3.],
		# rot = makeQuat(0.,0.,1.,0.),
		# color = randColor(),
		# module = b'Circle.py'
	# ))	

# r_Entities.append(Entity(
		# pos = [-3.,-3.],
		# scale = [1.,1.],
		# rot = makeQuat(0.,0.,1.,0.),
		# color = randColor(),
		# module = b'Circle.py'
	# ))	

# r_Entities.append(Entity(
		# pos = [-1.,4.],
		# scale = [2.,2.],
		# rot = makeQuat(0.,0.,1.,0.),
		# color = randColor(),
		# module = b'Circle.py'
	# ))	

# r_Entities.append(Entity(
		# pos = [-1.,0.],
		# scale = [0.5,0.5],
		# rot = makeQuat(0.,0.,1.,0.),
		# color = randColor(),
		# module = b'OBB.py'
	# ))