from PyLiaison import *

import Util

import random
import os

random.seed()

def randColor():
	return [random.random() for i in range (0,3)]+[1.]


# All of these resources should be accessed by
# function, rather than just lying around loose
# (things like sounds should be shared... maybe?
#  what I really need is a class that this file 
#  can correspond to. )
class OBB(Entity):
	def __init__(self):
		self.r_IqmFile = 'quad.iqm'
		self.r_ColPrim = 'OBB'
		
		sep = bytes(os.sep.encode('ascii'))
		sndFiles = os.listdir(SND_DIR)
		r_Sounds = [f for f in sndFiles if Util.isSoundFile(f)]
	def GetType(self):
		return OBB

ob = OBB()
print(ob)

r_IqmFile = b'quad.iqm'
sndFiles = os.listdir(SND_DIR)
separator = bytes(os.sep.encode('ascii')) # Thanks python 3...
r_Sounds = [x for x in sndFiles if Util.isSoundFile(x)]

r_ColPrim = 'OBB'

def HandleCollision(myID, theirID):
    e = g_Entities[myID]
    e.SendMessage_D(E_DR, E_DR_CLR, (randColor(),))
    try: # random.choice can raise an IndexError if seq is empty
        snd = random.choice(r_Sounds)
        PlaySound(snd)
    except IndexError:
        pass
	
# This should go in some master file that isn't InitScene
def AddEntity(eID, ePtr):
	g_Entities[eID] = Entity(ePtr)
