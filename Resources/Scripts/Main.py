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

	if (inputMgr.IsKeyDown(K_c)):
		print(CONTACT_DEBUG)
		CONTACT_DEBUG = not CONTACT_DEBUG
#		if (CONTACT_DEBUG):
#			CONTACT_DEBUG = False
#		else:
#			CONTACT_DEBUG = True
	return ret

def HandleCollision(eID1, eID2):
	e1 = g_Entities[eID1]
	e2 = g_Entities[eID2]
	e1.HandleCollision(eID2)
	e2.HandleCollision(eID1)

def Update():
	pass
