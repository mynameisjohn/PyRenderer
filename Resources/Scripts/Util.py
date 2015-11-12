from random import random, choice, seed
from math import radians, sin, cos
import abc

seed()

def isSoundFile(f):
	w = None
	if (type(f) == str):
		w = '.wav'
	elif (type(f) == bytes):
		w = b'.wav'
	else:
		return false
	if (len(f) <= len(w)):
		return False
	if (f[-len(w):len(f)] != w):
		return False
	return True

def makeQuat(x,y,z,theta):
	r = radians(theta)/2.
	c = cos(r)
	s = sin(r)
	return [c, s*x, s*y, s*z] #normalize...

def randColor():
	return [random() for i in range (0,3)]+[1.]

class PyType(abc.ABC):
	@abc.abstractmethod
	def GetType(self): pass
	@abc.abstractmethod
	def GetResources(self): pass
