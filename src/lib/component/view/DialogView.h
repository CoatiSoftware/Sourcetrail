#ifndef DIALOG_VIEW_H
#define DIALOG_VIEW_H

#include <string>
#include <vector>

#include "data/ErrorCountInfo.h"

class DialogView
{
public:
	DialogView();
	virtual ~DialogView();

	virtual void showProgressDialog(const std::string& title, const std::string& message);
	virtual void hideProgressDialog();

	virtual bool startIndexingDialog(size_t cleanFileCount, size_t indexFileCount);
	virtual void updateIndexingDialog(size_t fileCount, size_t totalFileCount, std::string sourcePath);
	virtual void finishedIndexingDialog(size_t fileCount, size_t totalFileCount, float time, ErrorCountInfo errorInfo);

	int confirm(const std::string& message);
	virtual int confirm(const std::string& message, const std::vector<std::string>& options);
};

#endif // DIALOG_VIEW_H
