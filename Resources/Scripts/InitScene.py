from PyLiaison import *
import math
from collections import namedtuple
from platform import system
import Util

def GetShaderSrc():
	r_ShaderSrc = { 'vert':'simple.vert', 
					'frag':'simple.frag',
					'Position':'a_Pos'}

	if (system() == 'Darwin'):
		r_ShaderSrc['vert'] = 'mac_'+r_ShaderSrc['vert']
		r_ShaderSrc['frag'] = 'mac_'+r_ShaderSrc['frag']

	return r_ShaderSrc

def InitCamera(camPtr):
	c_Camera = Camera(camPtr)
	c_Camera.InitOrtho([-10., 10.], [-10., 10.], [-10., 10.])

def GetMainModuleName():
	return 'Main.py'

def GetEntities():
	r_Entities = []
	Entity = namedtuple('Entity', 
		('vel', 'pos', 'scale', 'rot', 'mass', 'el', 'color', 'module'))

	r_Entities.append(Entity(
			vel = [3.,0.],
			pos = [1.,0.],
			scale = [1.,1.],
			rot = math.pi/4,
			mass = 1.,
			el = 1.,
			color = Util.randColor(),
			module = 'OBB.py'
		))

	r_Entities.append(Entity(
			vel = [0.,0.],
			pos = [-3.,0.],
			scale = [1.,1.],
			rot = 0.,
			mass = 1.,
			el = 1.,
			color = Util.randColor(),
			module = 'OBB.py'
		))

	r_Entities.append(Entity(
			vel = [2.,10.],
			pos = [3.2,1.5],
			scale = [.5,.5],
			rot = 0.,
			mass = 1.,
			el = 1.,
			color = Util.randColor(),
			module = 'circle.py'
		))

	r_Entities.append(Entity(
			vel = [0.,0.],
			pos = [5.,0.],
			scale = [3.5,1.],
			rot = math.pi/2,
			mass = 1.e10,
			el = 1.,
			color = Util.randColor(),
			module = 'OBB.py'
		))

	r_Entities.append(Entity(
			vel = [10.,-0.],
			pos = [-8.,0.],
			scale = [1.,1.],
			rot = 0.,
			mass = 1.,
			el = 1.,
			color = Util.randColor(),
			module = 'OBB.py'
		))
	'''
	r_Entities.append(Entity(
			vel = [10.,10.],
			pos = [0.,-8.],
			scale = [1.,1.],
			rot = 0.,
			mass = 1.,
			el = 1.,
			color = Util.randColor(),
			module = 'OBB.py'
		))

	r_Entities.append(Entity(
			vel = [-10.,10.],
			pos = [0.,8.],
			scale = [1.,1.],
			rot = 0.,
			mass = 1.,
			el = 1.,
			color = Util.randColor(),
			module = 'circle.py'
		))

	r_Entities.append(Entity(
			vel = [0.,15.],
			pos = [0.,0.],
			scale = [1.,1.],
			rot = 0.,
			mass = 1.,
			el = 1.,
			color = Util.randColor(),
			module = 'circle.py'
		))

	r_Entities.append(Entity(
			vel = [3.,3.],
			pos = [-6.,-6.],
			scale = [2.,2.8],
			rot = 0.,
			mass = 1.,
			el = 1.,
			color = Util.randColor(),
			module = 'OBB.py'
		))

	r_Entities.append(Entity(
			vel = [0.,15.],
			pos = [6.,6.],
			scale = [3.,3.],
			rot = 0.,
			mass = 1.,
			el = 1.,
			color = Util.randColor(),
			module = 'circle.py'
		))
		'''
	r_Entities.append(Entity(
			vel = [0.,0.],
			pos = [0.,18.],
			scale = [9.,9.],
			rot = 0.,
			mass = 1.e10,
			el = 1.,
			color = Util.randColor(),
			module = 'AABB.py'
		))

	r_Entities.append(Entity(
			vel = [0.,0.],
			pos = [0.,-18.],
			scale = [9.,9.],
			rot = 0.,
			mass = 1.e10,
			el = 1.,
			color = Util.randColor(),
			module = 'AABB.py'
		))

	r_Entities.append(Entity(
			vel = [0.,0.],
			pos = [18.,0.],
			scale = [9.,9.],
			rot = 0.,
			mass = 1.e10,
			el = 1.,
			color = Util.randColor(),
			module = 'AABB.py'
		))

	r_Entities.append(Entity(
			vel = [0.,0.],
			pos = [-18.,0.],
			scale = [9.,9.],
			rot = 0.,
			mass = 1.e10,
			el = 1.,
			color = Util.randColor(),
			module = 'AABB.py'
		))

	return r_Entities
