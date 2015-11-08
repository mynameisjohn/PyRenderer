from PyLiaison import *
import Util
import os

# All of these resources should be accessed by
# function, rather than just lying around loose
# (things like sounds should be shared... maybe?
#  what I really need is a class that this file 
#  can correspond to. )
class OBB:
	def __init__(self, ePtr):
		self.Ent = Entity(ePtr)
	
	def HandleCollision(self, otherID):
		pass

# TODO make class static
def GetResources():
	r_IqmFile = 'quad.iqm'
	r_ColPrim = 'OBB'
	r_Sounds = []

	if (len(r_Sounds) == 0):
		sep = bytes(os.sep.encode('ascii'))
		sndFiles = os.listdir(SND_DIR)
		r_Sounds = [f for f in sndFiles if Util.isSoundFile(f)]
	return (r_IqmFile, r_ColPrim, r_Sounds)

'''
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
'''
	
# This should go in some master file that isn't InitScene
def AddEntity(eID, ePtr):
	g_Entities[eID] = OBB(ePtr)
