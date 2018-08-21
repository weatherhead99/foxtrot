#pragma once
#include <string>
#include <Error.h>

namespace foxtrot {
	class FOXTROT_EXPORT StubError: public Error
	{
		public:
		StubError(const std::string& msg);
	};
	
	
}