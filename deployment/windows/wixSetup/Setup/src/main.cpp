#include <iostream>
#include <windows.h>

int main()
{
	HKEY hKey;
#ifdef WIN64
	LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Coati Software OG\\Sourcetrail 64-bit\\sourcetrailAppData"), 0, KEY_READ, &hKey);
#else
	LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Coati Software OG\\Sourcetrail\\sourcetrailAppData"), 0, KEY_READ, &hKey);
#endif
	if (lResult == ERROR_SUCCESS)
	{
		std::cout << "Sourcetrail found" << std::endl;
		std::cout << "Performing upgrade" << std::endl;
	}
	else
	{
		std::cout << "Sourcetrail not found" << std::endl;
		std::cout << "Performing initial installation" << std::endl;
	}

	system("msiexec /i sourcetrail.msi");

	return 0;
}
