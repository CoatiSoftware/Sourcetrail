#include "qt/network/QtUpdateChecker.h"

#include <QDesktopServices>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>

#include "LicenseChecker.h"
#include "qt/network/QtRequest.h"
#include "settings/ApplicationSettings.h"
#include "utility/Version.h"
#include "utility/utility.h"
#include "utility/utilityApp.h"
#include "utility/utilityUuid.h"

void QtUpdateChecker::check(bool force)
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	if (!force && TimeStamp::now().deltaHours(appSettings->getLastUpdateCheck()) < 24)
	{
		return;
	}

	appSettings->setLastUpdateCheck(TimeStamp::now());
	appSettings->save();

	QString urlString = "https://www.sourcetrail.com/api/v1/versions/latest";

	// OS
	std::string osString;

	switch (utility::getOsType())
	{
		case OS_WINDOWS:
			osString = "windows";
			break;
		case OS_MAC:
			osString = "macOS";
			break;
		case OS_LINUX:
			osString = "linux";
			break;
		default:
			return;
	}

	urlString += ("?os=" + osString).c_str();

	// architecture
	std::string platformString = (utility::getApplicationArchitectureType() == APPLICATION_ARCHITECTURE_X86_64 ? "64" : "32");
	urlString += ("&platform=" + platformString + "bit").c_str();

	// version
	urlString += ("&version=" + Version::getApplicationVersion().toDisplayString()).c_str();

	// license
	std::string licenseString;
	switch (LicenseChecker::getInstance()->getCurrentLicenseType())
	{
		case MessageEnteredLicense::LICENSE_NONE:
			licenseString = "trial";
			break;
		case MessageEnteredLicense::LICENSE_TEST:
			licenseString = "test";
			break;
		case MessageEnteredLicense::LICENSE_NON_COMMERCIAL:
			licenseString = "private";
			break;
		case MessageEnteredLicense::LICENSE_COMMERCIAL:
			licenseString = "commercial";
			break;
	}

	urlString += ("&license=" + licenseString).c_str();

	// user token
	std::string token = appSettings->getUserToken();
	if (!token.size())
	{
		token = utility::getUuidString();
		appSettings->setUserToken(token);
		appSettings->save();
	}

	urlString += ("&token=" + token).c_str();

	// send request
	QtRequest* request = new QtRequest();
	QObject::connect(request, &QtRequest::receivedData,
		[force, request](QByteArray bytes)
		{
			do
			{
				QJsonDocument doc = QJsonDocument::fromJson(bytes);
				if (!doc.isObject())
				{
					LOG_ERROR_STREAM(<< "Update response couldn't be parsed as JSON");
					break;
				}

				QString version = doc.object().find("version")->toString();
				QString url = doc.object().find("url")->toString();

				Version updateVersion = Version::fromString(version.toStdString());
				if (!updateVersion.isValid())
				{
					LOG_ERROR_STREAM(<< "update version string is not valid: " << version.toStdString());
					break;
				}

				if (updateVersion > Version::getApplicationVersion())
				{
					QMessageBox msgBox;
					msgBox.setText("Update Check");
					msgBox.setInformativeText(
						"Sourcetrial " + version + " is available for download: <a href=\"" + url + "\">" + url + "</a>");
					msgBox.addButton("Close", QMessageBox::ButtonRole::NoRole);
					QPushButton* but = msgBox.addButton("Download", QMessageBox::ButtonRole::YesRole);
					msgBox.setDefaultButton(but);

					if (msgBox.exec() == 1)
					{
						QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
					}
				}
				else if (force)
				{
					QMessageBox msgBox;
					msgBox.setText("Update Check");
					msgBox.setInformativeText(
						("Sourcetrial " + Version::getApplicationVersion().toDisplayString() + " is still up-to-date.").c_str());
					msgBox.setStandardButtons(QMessageBox::Ok);
					msgBox.setDefaultButton(QMessageBox::Ok);
					msgBox.exec();
				}
			}
			while (false);

			request->deleteLater();
		}
	);

	request->sendRequest(urlString);
}

void QtUpdateChecker::checkUpdate()
{
	check();
}
