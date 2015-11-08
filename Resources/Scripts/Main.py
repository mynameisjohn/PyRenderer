from PyLiaison import *

CONTACT_DEBUG = False

# TODO handle more interesting key states
def GetPlayState(curState):
	ret = curState
	if (inputMgr.GetKeyState(K_SPACE) == 1):
		if (curState == PLAY):
			ret = PAUSE
		elif (curState == PAUSE):
			ret = PLAY

	elif (inputMgr.IsKeyDown(K_RIGHT)):
		if (curState == PAUSE):
			ret = STEP_FWD

	if (inputMgr.GetKeyState(K_c) == 1):
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
