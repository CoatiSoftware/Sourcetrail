#ifndef LANGUAGE_PACKAGE_MANAGER_H
#define LANGUAGE_PACKAGE_MANAGER_H

#include <memory>
#include <vector>

class IndexerComposite;
class LanguagePackage;

class LanguagePackageManager
{
public:
	static std::shared_ptr<LanguagePackageManager> getInstance();
	static void destroyInstance();

	void addPackage(std::shared_ptr<LanguagePackage> package);
	std::shared_ptr<IndexerComposite> instantiateSupportedIndexers();

private:
	static std::shared_ptr<LanguagePackageManager> s_instance;
	LanguagePackageManager() = default;

	std::vector<std::shared_ptr<LanguagePackage>> m_packages;
};

#endif // LANGUAGE_PACKAGE_MANAGER_H