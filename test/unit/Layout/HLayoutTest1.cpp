#include "HLayoutTest1.hpp"

#include <BlendInt/Interface.hpp>
#include <BlendInt/Gui/HLayout.hpp>
#include <BlendInt/Gui/VLayout.hpp>
#include <BlendInt/Gui/Widget.hpp>

#include <boost/smart_ptr.hpp>

using namespace BlendInt;

// test the default Add() method
TEST_F(HLayoutTest1, Mix1)
{
	Init();
	GLFWwindow* window = CreateWindow("Mix Layout Test 1", 640, 480);

	// add test code here
	Context* context = Manage(new Context);
#ifdef DEBUG
	context->set_name("Context");
#endif
	Interface::instance->SetCurrentContext(context);

	HLayout* hlayout = Manage(new HLayout);
	hlayout->SetPosition(100, 100);

	Widget* widget1 = Manage(new Widget);
	Widget* widget2 = Manage(new Widget);
	Widget* widget3 = Manage(new Widget);

	VLayout* vlayout = Manage(new VLayout);
	vlayout->PushBack(widget2);
	vlayout->PushBack(widget3);

	hlayout->PushBack(widget1);
	hlayout->PushBack(vlayout);

	context->PushBack(hlayout);

	RunLoop(window);

	Terminate();

	ASSERT_TRUE(true);
}
