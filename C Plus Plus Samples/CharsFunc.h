#ifndef CHARSFUNC_H
#define CHARSFUNC_H
#include <iostream>
#include <cctype>
void UseCharFunctionSample()
{
	char digital = '9';
	char letter = 'a';
	std::cout << "digital:" << digital << "\t" << "letter:" << letter << std::endl;

	std::cout << "isdigit(digital)=" << isdigit(digital) << "\t" << "isdigit(letter)=" << isdigit(letter) << std::endl;
	std::cout << "isalpha(digital)=" << isalpha(digital) << "\t" << "isalpha(letter)=" << isalpha(letter) << std::endl;
	std::cout << "isalnum(digital)=" << isalnum(digital) << "\t" << "isalnum(letter)=" << isalnum(letter) << std::endl;
	std::cout << "islower(digital)=" << islower(digital) << "\t" << "islower(letter)=" << islower(letter) << std::endl;
	std::cout << "isupper(digital)=" << isupper(digital) << "\t" << "isupper(letter)=" << isupper(letter) << std::endl;
	std::cout << "isspace(digital)=" << isspace(digital) << "\t" << "isspace(letter)=" << isspace(letter) << std::endl;
	std::cout << "tolower(digital)=" << tolower(digital) << "\t" << "tolower(letter)=" << tolower(letter) << std::endl;
	std::cout << "toupper(digital)=" << toupper(digital) << "\t" << "toupper(letter)=" << toupper(letter) << std::endl;
}
#endif // !CHARSFUNC_H
