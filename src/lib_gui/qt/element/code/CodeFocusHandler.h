#ifndef CODE_FOCUS_HANDLER_H
#define CODE_FOCUS_HANDLER_H

#include <vector>

#include "types.h"

class QPushButton;
class QtCodeArea;
class QtCodeFile;

class CodeFocusHandler
{
public:
	enum class Direction
	{
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	struct Focus
	{
		QtCodeFile* file = nullptr;
		QtCodeArea* area = nullptr;
		QPushButton* scopeLine = nullptr;
		size_t lineNumber = 0;
		size_t columnNumber = 0;
		Id locationId = 0;
		std::vector<Id> tokenIds;

		bool isEmpty() const
		{
			return file == nullptr && area == nullptr && scopeLine == nullptr && lineNumber == 0 &&
				locationId == 0 && !tokenIds.size();
		}
	};

	void focus();
	void defocus();

	bool isFocused() const;

	void clearCurrentFocus();
	const Focus& getCurrentFocus() const;
	void setCurrentFocus(const Focus& focus, bool fromMouse);
	bool hasCurrentFocus() const;

	void setFocusedLocationId(
		QtCodeArea* area,
		size_t lineNumber,
		size_t columnNumber,
		Id locationId,
		const std::vector<Id>& tokenIds,
		bool updateTargetColumn,
		bool fromMouse);
	void setFocusedScopeLine(QtCodeArea* area, QPushButton* scopeLine, size_t lineNumber);
	void setFocusedFile(QtCodeFile* file);

	size_t getTargetColumn() const;

	void focusView();

	virtual void updateFiles() = 0;

private:
	Focus m_focus;
	Focus m_oldFocus;

	size_t m_targetColumn = 0;
	bool m_hasFocus = false;
};

#endif	  // CODE_FOCUS_HANDLER_H
