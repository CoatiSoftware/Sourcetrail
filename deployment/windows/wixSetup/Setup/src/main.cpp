#include <iostream>
#include <windows.h>

int main()
{
	HKEY hKey;
	LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Coati Software OG\\Coati\\coatiSoftwareAppData"), 0, KEY_READ, &hKey);

	if (lResult == ERROR_SUCCESS)
	{
		std::cout << "Coati found" << std::endl;
		std::cout << "Performing upgrade" << std::endl;
		system("msiexec /i bin/coati.msi REINSTALL=ALL REINSTALLMODE=vomus");
	}
	else
	{
		std::cout << "Coati not found" << std::endl;
		std::cout << "Performing initial installation" << std::endl;
		system("msiexec /i bin/coati.msi");
	}

	return 0;
}