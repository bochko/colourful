// colourful.cpp : Defines the entry point for the application.
//

#include "colourful.h"

int main()
{
	std::cout  << kRed.string_view() << "this text" << kReset.string_view() << std::endl;
	return 0;
}
