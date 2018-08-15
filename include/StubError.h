#pragma once
#include <string>
#include <Error.h>

namespace foxtrot {
	class StubError: public Error
	{
		public:
		StubError(const std::string& msg);
	};
	
	
}