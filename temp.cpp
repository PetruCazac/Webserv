
#include "headers/HttpResponse.hpp"
#include <iostream>

int main()
{
	HttpResponse response(404);
	std::cout << response.getResponse() << std::endl;
	return 0;
}
