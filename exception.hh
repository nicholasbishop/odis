#ifndef EXCEPTION_HH
#define EXCEPTION_HH

#include <exception>
#include <string>

namespace Odis {
	class Exception : public std::exception {
	public:
		Exception(const char *description_) : description(description_) {}

		virtual const char* what() const throw() {
			return description.c_str();
		};

		virtual ~Exception() throw() {};
		
	private:
		const std::string description;
	};
}

#endif
