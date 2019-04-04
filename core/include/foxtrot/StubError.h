#pragma once
#include <foxtrot/Error.h>

namespace foxtrot {
	class FOXTROT_CORE_EXPORT StubError: public Error
	{
		public:
		StubError(const std::string& msg);
	};
	
	
}
