#include "WidgetTest1.hpp"
#include <BlendInt/Gui/Widget.hpp>
#include <Common/Window.hpp>

#include <BlendInt/Service/ContextManager.hpp>

using namespace BlendInt;

WidgetTest1::WidgetTest1()
: testing::Test()
{
	// TODO: add constructor code
}

WidgetTest1::~WidgetTest1()
{
	// TODO: add destructor code
}

/**
 * test Foo() method
 *
 * Expected result: 
 */
TEST_F(WidgetTest1, Add1)
{
    Init();
    GLFWwindow* win = CreateWindow("WidgetTest1 -- Show1");

	// TODO: add test code here
    
    Widget* widget1 = new Widget;
    widget1->set_name("widget1");
    widget1->SetPosition(200, 200);

    RunLoop(win);

    delete widget1;
    Interface::Release();

    Terminate();

	ASSERT_TRUE(true);
}

