from PyLiaison import *

CONTACT_DEBUG = False

# TODO handle more interesting key states
def HandleInput(curState):
	ret = curState
	allKeys = inputMgr.GetKeys()
	if K_ESC in allKeys:
		return QUIT
		
	if K_SPACE in allKeys:
		#print(allKeys[K_SPACE])
		if not allKeys[K_SPACE][0]:
			if (curState == PLAY):
				ret = PAUSE
			elif (curState == PAUSE):
				ret = PLAY
	elif K_RIGHT in allKeys:
		if (curState == PAUSE):
			ret = STEP_FWD

	if K_c in allKeys:
		if not allKeys[K_c][0]:
			global CONTACT_DEBUG
			CONTACT_DEBUG = not CONTACT_DEBUG

	return ret

def HandleCollision(eID1, eID2):
	e1 = g_Entities[eID1]
	e2 = g_Entities[eID2]
	e1.HandleCollision(eID2)
	e2.HandleCollision(eID1)

def Update():
	pass
