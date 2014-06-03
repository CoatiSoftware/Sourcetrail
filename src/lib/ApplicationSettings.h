#ifndef APPLICATION_SETTINGS_H
#define APPLICATION_SETTINGS_H

#include <memory>

class ApplicationSettings
{
public:
	static std::shared_ptr<ApplicationSettings> getInstance();

	~ApplicationSettings();

private:
	ApplicationSettings();
	ApplicationSettings(const ApplicationSettings&);
	void operator=(const ApplicationSettings&);

	static std::shared_ptr<ApplicationSettings> s_instance;
};

#endif // APPLICATION_SETTINGS_H
