#include "Util.h"
#include "Init.h"

std::unique_ptr<Scene> sPtr(nullptr);

int main(int argc, char ** argv) {
	if (!InitGL() || !InitPython() || !InitScene(sPtr))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}