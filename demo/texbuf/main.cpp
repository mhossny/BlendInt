/**
 * BlendInt demo
 */

#include <iostream>

#include <Cpp/Events.hpp>
#include <BlendInt/Core/Types.hpp>

#include "TexBufWindow.hpp"
#include "TexBufContext.hpp"

using namespace BlendInt;
using namespace std;

namespace BI = BlendInt;

int main(int argc, char* argv[])
{
	BLENDINT_EVENTS_INIT_ONCE_IN_MAIN;

	Init();

	GLFWwindow* win = CreateWindow("Texture Buffer Demo", 1600, 800);

	TexBufContext* context = Manage(new TexBufContext);
#ifdef DEBUG
	context->set_name("TexBufContext");
#endif
	SetContext(context);
	context->Resize(1600, 800);

	RunLoop (win);
	Terminate();

	return 0;
}
