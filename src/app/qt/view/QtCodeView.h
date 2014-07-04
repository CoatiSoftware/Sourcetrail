#ifndef QT_CODE_VIEW_H
#define QT_CODE_VIEW_H

#include <memory>
#include <vector>

#include "component/view/CodeView.h"
#include "qt/utility/QtThreadedFunctor.h"
#include "utility/types.h"

class QtCodeFile;

class QtCodeView: public CodeView
{
public:
	QtCodeView(ViewLayout* viewLayout);
	~QtCodeView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initGui();

	// CodeView implementation
	virtual void addCodeSnippet(const std::string& str, const TokenLocationFile& locationFile, int startLineNumber);
	virtual void clearCodeSnippets();

	void activateToken(Id tokenId) const;

private:
	void doAddCodeSnippet(const std::string& str, const TokenLocationFile& locationFile, int startLineNumber);
	void doClearCodeSnippets();

	std::vector<std::shared_ptr<QtCodeFile> > m_files;

	QtThreadedFunctor<void> m_clearCodeSnippetsFunctor;
	QtThreadedFunctor<const std::string&, const TokenLocationFile&, int> m_addCodeSnippetFunctor;
};

# endif // QT_CODE_VIEW_H
