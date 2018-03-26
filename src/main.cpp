/**
 * main.cpp
 * Contains implementation of main function
 * Author: akowalew
 */

#include "app/Application.hpp"

int main()
{
	auto& application = app::Application::instance();
	application.run();
}
