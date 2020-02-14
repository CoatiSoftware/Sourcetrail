#include "StorageAccessProxy.h"

#include "Graph.h"
#include "NodeTypeSet.h"
#include "SourceLocationCollection.h"
#include "SourceLocationFile.h"

#include "FileInfo.h"
#include "FilePath.h"
#include "logging.h"

void StorageAccessProxy::setSubject(std::weak_ptr<StorageAccess> subject)
{
	m_subject = subject;
}

#define UNWRAP(...) __VA_ARGS__

#define DEF_GETTER_0(_METHOD_NAME_, _RETURN_TYPE_, _DEFAULT_VALUE_)                                \
	UNWRAP(_RETURN_TYPE_) StorageAccessProxy::_METHOD_NAME_() const                                \
	{                                                                                              \
		if (std::shared_ptr<StorageAccess> subject = m_subject.lock())                             \
		{                                                                                          \
			return subject->_METHOD_NAME_();                                                       \
		}                                                                                          \
		return _DEFAULT_VALUE_;                                                                    \
	}

#define DEF_GETTER_1(_METHOD_NAME_, _PARAM_1_TYPE_, _RETURN_TYPE_, _DEFAULT_VALUE_)                \
	UNWRAP(_RETURN_TYPE_) StorageAccessProxy::_METHOD_NAME_(_PARAM_1_TYPE_ p1) const               \
	{                                                                                              \
		if (std::shared_ptr<StorageAccess> subject = m_subject.lock())                             \
		{                                                                                          \
			return subject->_METHOD_NAME_(p1);                                                     \
		}                                                                                          \
		return _DEFAULT_VALUE_;                                                                    \
	}

#define DEF_GETTER_2(_METHOD_NAME_, _PARAM_1_TYPE_, _PARAM_2_TYPE_, _RETURN_TYPE_, _DEFAULT_VALUE_) \
	UNWRAP(_RETURN_TYPE_)                                                                           \
	StorageAccessProxy::_METHOD_NAME_(_PARAM_1_TYPE_ p1, _PARAM_2_TYPE_ p2) const                   \
	{                                                                                               \
		if (std::shared_ptr<StorageAccess> subject = m_subject.lock())                              \
		{                                                                                           \
			return subject->_METHOD_NAME_(p1, p2);                                                  \
		}                                                                                           \
		return _DEFAULT_VALUE_;                                                                     \
	}

#define DEF_GETTER_3(                                                                                \
	_METHOD_NAME_, _PARAM_1_TYPE_, _PARAM_2_TYPE_, _PARAM_3_TYPE_, _RETURN_TYPE_, _DEFAULT_VALUE_)   \
	UNWRAP(_RETURN_TYPE_)                                                                            \
	StorageAccessProxy::_METHOD_NAME_(_PARAM_1_TYPE_ p1, _PARAM_2_TYPE_ p2, _PARAM_3_TYPE_ p3) const \
	{                                                                                                \
		if (std::shared_ptr<StorageAccess> subject = m_subject.lock())                               \
		{                                                                                            \
			return subject->_METHOD_NAME_(p1, p2, p3);                                               \
		}                                                                                            \
		return _DEFAULT_VALUE_;                                                                      \
	}

#define DEF_GETTER_4(                                                                              \
	_METHOD_NAME_,                                                                                 \
	_PARAM_1_TYPE_,                                                                                \
	_PARAM_2_TYPE_,                                                                                \
	_PARAM_3_TYPE_,                                                                                \
	_PARAM_4_TYPE_,                                                                                \
	_RETURN_TYPE_,                                                                                 \
	_DEFAULT_VALUE_)                                                                               \
	UNWRAP(_RETURN_TYPE_)                                                                          \
	StorageAccessProxy::_METHOD_NAME_(                                                             \
		_PARAM_1_TYPE_ p1, _PARAM_2_TYPE_ p2, _PARAM_3_TYPE_ p3, _PARAM_4_TYPE_ p4) const          \
	{                                                                                              \
		if (std::shared_ptr<StorageAccess> subject = m_subject.lock())                             \
		{                                                                                          \
			return subject->_METHOD_NAME_(p1, p2, p3, p4);                                         \
		}                                                                                          \
		return _DEFAULT_VALUE_;                                                                    \
	}

#define DEF_GETTER_5(                                                                                  \
	_METHOD_NAME_,                                                                                     \
	_PARAM_1_TYPE_,                                                                                    \
	_PARAM_2_TYPE_,                                                                                    \
	_PARAM_3_TYPE_,                                                                                    \
	_PARAM_4_TYPE_,                                                                                    \
	_PARAM_5_TYPE_,                                                                                    \
	_RETURN_TYPE_,                                                                                     \
	_DEFAULT_VALUE_)                                                                                   \
	UNWRAP(_RETURN_TYPE_)                                                                              \
	StorageAccessProxy::_METHOD_NAME_(                                                                 \
		_PARAM_1_TYPE_ p1, _PARAM_2_TYPE_ p2, _PARAM_3_TYPE_ p3, _PARAM_4_TYPE_ p4, _PARAM_5_TYPE_ p5) \
		const                                                                                          \
	{                                                                                                  \
		if (std::shared_ptr<StorageAccess> subject = m_subject.lock())                                 \
		{                                                                                              \
			return subject->_METHOD_NAME_(p1, p2, p3, p4, p5);                                         \
		}                                                                                              \
		return _DEFAULT_VALUE_;                                                                        \
	}

#define DEF_GETTER_6(                                                                              \
	_METHOD_NAME_,                                                                                 \
	_PARAM_1_TYPE_,                                                                                \
	_PARAM_2_TYPE_,                                                                                \
	_PARAM_3_TYPE_,                                                                                \
	_PARAM_4_TYPE_,                                                                                \
	_PARAM_5_TYPE_,                                                                                \
	_PARAM_6_TYPE_,                                                                                \
	_RETURN_TYPE_,                                                                                 \
	_DEFAULT_VALUE_)                                                                               \
	UNWRAP(_RETURN_TYPE_)                                                                          \
	StorageAccessProxy::_METHOD_NAME_(                                                             \
		_PARAM_1_TYPE_ p1,                                                                         \
		_PARAM_2_TYPE_ p2,                                                                         \
		_PARAM_3_TYPE_ p3,                                                                         \
		_PARAM_4_TYPE_ p4,                                                                         \
		_PARAM_5_TYPE_ p5,                                                                         \
		_PARAM_6_TYPE_ p6) const                                                                   \
	{                                                                                              \
		if (std::shared_ptr<StorageAccess> subject = m_subject.lock())                             \
		{                                                                                          \
			return subject->_METHOD_NAME_(p1, p2, p3, p4, p5, p6);                                 \
		}                                                                                          \
		return _DEFAULT_VALUE_;                                                                    \
	}

#define DEF_GETTER_7(                                                                              \
	_METHOD_NAME_,                                                                                 \
	_PARAM_1_TYPE_,                                                                                \
	_PARAM_2_TYPE_,                                                                                \
	_PARAM_3_TYPE_,                                                                                \
	_PARAM_4_TYPE_,                                                                                \
	_PARAM_5_TYPE_,                                                                                \
	_PARAM_6_TYPE_,                                                                                \
	_PARAM_7_TYPE_,                                                                                \
	_RETURN_TYPE_,                                                                                 \
	_DEFAULT_VALUE_)                                                                               \
	UNWRAP(_RETURN_TYPE_)                                                                          \
	StorageAccessProxy::_METHOD_NAME_(                                                             \
		_PARAM_1_TYPE_ p1,                                                                         \
		_PARAM_2_TYPE_ p2,                                                                         \
		_PARAM_3_TYPE_ p3,                                                                         \
		_PARAM_4_TYPE_ p4,                                                                         \
		_PARAM_5_TYPE_ p5,                                                                         \
		_PARAM_6_TYPE_ p6,                                                                         \
		_PARAM_7_TYPE_ p7) const                                                                   \
	{                                                                                              \
		if (std::shared_ptr<StorageAccess> subject = m_subject.lock())                             \
		{                                                                                          \
			return subject->_METHOD_NAME_(p1, p2, p3, p4, p5, p6, p7);                             \
		}                                                                                          \
		return _DEFAULT_VALUE_;                                                                    \
	}

DEF_GETTER_1(getNodeIdForFileNode, const FilePath&, Id, 0)
DEF_GETTER_1(getNodeIdForNameHierarchy, const NameHierarchy&, Id, 0)
DEF_GETTER_1(getNodeIdsForNameHierarchies, const std::vector<NameHierarchy>, std::vector<Id>, {})
DEF_GETTER_1(getNameHierarchyForNodeId, Id, NameHierarchy, NameHierarchy(NAME_DELIMITER_UNKNOWN))
DEF_GETTER_1(getNameHierarchiesForNodeIds, const std::vector<Id>&, std::vector<NameHierarchy>, {})

typedef std::map<Id, std::pair<Id, NameHierarchy>> NodeIdToParentFileMap;
DEF_GETTER_1(getNodeIdToParentFileMap, const std::vector<Id>&, NodeIdToParentFileMap, {})

DEF_GETTER_1(getNodeTypeForNodeWithId, Id, NodeType, NodeType(NODE_SYMBOL))
DEF_GETTER_1(getEdgeById, Id, StorageEdge, StorageEdge())
DEF_GETTER_2(
	getFullTextSearchLocations,
	const std::wstring&,
	bool,
	std::shared_ptr<SourceLocationCollection>,
	std::make_shared<SourceLocationCollection>())
DEF_GETTER_3(
	getAutocompletionMatches,
	const std::wstring&,
	NodeTypeSet,
	bool,
	std::vector<SearchMatch>,
	std::vector<SearchMatch>())
DEF_GETTER_1(
	getSearchMatchesForTokenIds,
	const std::vector<Id>&,
	std::vector<SearchMatch>,
	std::vector<SearchMatch>())
DEF_GETTER_0(getGraphForAll, std::shared_ptr<Graph>, std::make_shared<Graph>())
DEF_GETTER_1(getGraphForNodeTypes, NodeTypeSet, std::shared_ptr<Graph>, std::make_shared<Graph>())
DEF_GETTER_3(
	getGraphForActiveTokenIds,
	const std::vector<Id>&,
	const std::vector<Id>&,
	bool*,
	std::shared_ptr<Graph>,
	std::make_shared<Graph>())
DEF_GETTER_1(getGraphForChildrenOfNodeId, Id, std::shared_ptr<Graph>, std::make_shared<Graph>())
DEF_GETTER_7(
	getGraphForTrail,
	Id,
	Id,
	NodeKindMask,
	Edge::TypeMask,
	bool,
	size_t,
	bool,
	std::shared_ptr<Graph>,
	std::make_shared<Graph>())
DEF_GETTER_0(getAvailableNodeTypes, NodeKindMask, 0);
DEF_GETTER_0(getAvailableEdgeTypes, Edge::TypeMask, 0);
DEF_GETTER_2(getActiveTokenIdsForId, Id, Id*, std::vector<Id>, {})
DEF_GETTER_1(getNodeIdsForLocationIds, const std::vector<Id>&, std::vector<Id>, {})
DEF_GETTER_1(
	getSourceLocationsForTokenIds,
	const std::vector<Id>&,
	std::shared_ptr<SourceLocationCollection>,
	std::make_shared<SourceLocationCollection>())
DEF_GETTER_1(
	getSourceLocationsForLocationIds,
	const std::vector<Id>&,
	std::shared_ptr<SourceLocationCollection>,
	std::make_shared<SourceLocationCollection>())
DEF_GETTER_1(
	getSourceLocationsForFile,
	const FilePath&,
	std::shared_ptr<SourceLocationFile>,
	std::make_shared<SourceLocationFile>(FilePath(), L"", false, false, false))
DEF_GETTER_3(
	getSourceLocationsForLinesInFile,
	const FilePath&,
	size_t,
	size_t,
	std::shared_ptr<SourceLocationFile>,
	std::make_shared<SourceLocationFile>(FilePath(), L"", false, false, false))
DEF_GETTER_2(
	getSourceLocationsOfTypeInFile,
	const FilePath&,
	LocationType,
	std::shared_ptr<SourceLocationFile>,
	std::make_shared<SourceLocationFile>(FilePath(), L"", false, false, false))
DEF_GETTER_2(getFileContent, const FilePath&, bool, std::shared_ptr<TextAccess>, nullptr)
DEF_GETTER_1(getFileInfoForFileId, Id, FileInfo, FileInfo())
DEF_GETTER_1(getFileInfoForFilePath, const FilePath&, FileInfo, FileInfo())
DEF_GETTER_1(getFileInfosForFilePaths, const std::vector<FilePath>&, std::vector<FileInfo>, {})
DEF_GETTER_0(getStorageStats, StorageStats, StorageStats())
DEF_GETTER_0(getErrorCount, ErrorCountInfo, ErrorCountInfo())
DEF_GETTER_1(getErrorsLimited, const ErrorFilter&, std::vector<ErrorInfo>, {})
DEF_GETTER_2(getErrorsForFileLimited, const ErrorFilter&, const FilePath&, std::vector<ErrorInfo>, {})
DEF_GETTER_1(
	getErrorSourceLocations,
	const std::vector<ErrorInfo>&,
	std::shared_ptr<SourceLocationCollection>,
	std::make_shared<SourceLocationCollection>())

Id StorageAccessProxy::addNodeBookmark(const NodeBookmark& bookmark)
{
	if (std::shared_ptr<StorageAccess> subject = m_subject.lock())
	{
		return subject->addNodeBookmark(bookmark);
	}

	return -1;
}

Id StorageAccessProxy::addEdgeBookmark(const EdgeBookmark& bookmark)
{
	if (std::shared_ptr<StorageAccess> subject = m_subject.lock())
	{
		return subject->addEdgeBookmark(bookmark);
	}

	return -1;
}

Id StorageAccessProxy::addBookmarkCategory(const std::wstring& categoryName)
{
	if (std::shared_ptr<StorageAccess> subject = m_subject.lock())
	{
		return subject->addBookmarkCategory(categoryName);
	}

	return -1;
}

void StorageAccessProxy::updateBookmark(
	const Id bookmarkId,
	const std::wstring& name,
	const std::wstring& comment,
	const std::wstring& categoryName)
{
	if (std::shared_ptr<StorageAccess> subject = m_subject.lock())
	{
		subject->updateBookmark(bookmarkId, name, comment, categoryName);
	}
}

void StorageAccessProxy::removeBookmark(const Id id)
{
	if (std::shared_ptr<StorageAccess> subject = m_subject.lock())
	{
		subject->removeBookmark(id);
	}
}

void StorageAccessProxy::removeBookmarkCategory(const Id id)
{
	if (std::shared_ptr<StorageAccess> subject = m_subject.lock())
	{
		subject->removeBookmarkCategory(id);
	}
}

DEF_GETTER_0(getAllNodeBookmarks, std::vector<NodeBookmark>, {})
DEF_GETTER_0(getAllEdgeBookmarks, std::vector<EdgeBookmark>, {})
DEF_GETTER_0(getAllBookmarkCategories, std::vector<BookmarkCategory>, {})
DEF_GETTER_2(
	getTooltipInfoForTokenIds, const std::vector<Id>&, TooltipOrigin, TooltipInfo, TooltipInfo())
DEF_GETTER_2(
	getTooltipInfoForSourceLocationIdsAndLocalSymbolIds,
	const std::vector<Id>&,
	const std::vector<Id>&,
	TooltipInfo,
	TooltipInfo())
