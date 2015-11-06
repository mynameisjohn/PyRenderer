def isSoundFile(f):
	w = '.wav'
	if (type(f) is not str):
		return False
	if (len(f) <= len(w)):
		return False
	if (f[-len(w):len(f)] != w):
		return False
	return True

import abc

class PyType(abc.ABC):
	@abc.abstractmethod
	def GetType(self): pass
	@abc.abstractmethod
	def GetResources(self): pass
