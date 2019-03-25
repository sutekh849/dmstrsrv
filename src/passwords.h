#ifndef PASSWORDS_H
#define PASSWORDS_H
#include <experimental/string_view>
class Passwords
{
public:
	static constexpr std::experimental::string_view dbPassword();
};
#endif
