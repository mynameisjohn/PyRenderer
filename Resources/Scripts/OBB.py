from PyLiaison import *
import Util
import os

import random
random.seed()

# All of these resources should be accessed by
# function, rather than just lying around loose
# (things like sounds should be shared... maybe?
#  what I really need is a class that this file 
#  can correspond to. )
class OBB:
    r_IqmFile = 'quad.iqm'
    r_ColPrim = 'OBB'
    r_Sounds = []
    
    def __init__(self, ePtr):
        self.Ent = Entity(ePtr)
    
    def HandleCollision(self, otherID):
        try:
            snd = random.choice(OBB.r_Sounds)
            PlaySound(snd)
        except IndexError:
            pass

# TODO make class static
def GetResources():
    if (len(OBB.r_Sounds) == 0):
        sep = bytes(os.sep.encode('ascii'))
        sndFiles = os.listdir(SND_DIR)
        OBB.r_Sounds = [f for f in sndFiles if Util.isSoundFile(f)]
    return (OBB.r_IqmFile, OBB.r_ColPrim, OBB.r_Sounds)
	
# This should go in some master file that isn't InitScene
def AddEntity(eID, ePtr):
	g_Entities[eID] = OBB(ePtr)
