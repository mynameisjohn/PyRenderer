from PyLiaison import *

CONTACT_DEBUG = False

# TODO handle more interesting key states
def GetPlayState(curState):
	ret = curState
	if (inputMgr.IsKeyDown(K_SPACE)):
		if (curState == PLAY):
			ret = PAUSE
		elif (curState == PAUSE):
			ret = PLAY

	elif (inputMgr.IsKeyDown(K_RIGHT)):
		if (curState == PAUSE):
			ret = STEP_FWD

	if (inputMgr.IsKeyDown(K_c)):
		CONTACT_DEBUG = not CONTACT_DEBUG
	return ret

def HandleCollision(eID1, eID2):
	e1 = g_Entities[eID1]
	e2 = g_Entities[eID2]
	e1.HandleCollision(e2)
	e2.HandleCollision(e1)
