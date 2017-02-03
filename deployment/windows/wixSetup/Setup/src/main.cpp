#include <iostream>
#include <windows.h>

int main()
{
	HKEY hKey;
#ifdef WIN64
	LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Coati Software OG\\Coati 64-bit\\coatiSoftwareAppData"), 0, KEY_READ, &hKey);
#else
	LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Coati Software OG\\Coati\\coatiSoftwareAppData"), 0, KEY_READ, &hKey);
#endif
	if (lResult == ERROR_SUCCESS)
	{
		std::cout << "Coati found" << std::endl;
		std::cout << "Performing upgrade" << std::endl;
	}
	else
	{
		std::cout << "Coati not found" << std::endl;
		std::cout << "Performing initial installation" << std::endl;
	}

	system("msiexec /i coati.msi");

	return 0;
}
