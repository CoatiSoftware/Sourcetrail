#ifndef ACTIVATION_TRANSLATOR_H
#define ACTIVATION_TRANSLATOR_H

#include <memory>

class MessageActivateEdge;
class MessageActivateFile;
class MessageActivateNodes;
class MessageActivateTokenIds;
class MessageActivateTokens;
class MessageSearch;
class StorageAccess;

class ActivationTranslator
{
public:
	ActivationTranslator(StorageAccess* storageAccess);
	~ActivationTranslator();

	std::shared_ptr<MessageActivateTokens> translateMessage(const MessageActivateEdge* message) const;
	std::shared_ptr<MessageActivateTokens> translateMessage(const MessageActivateFile* message) const;
	std::shared_ptr<MessageActivateTokens> translateMessage(const MessageActivateNodes* message) const;
	std::shared_ptr<MessageActivateTokens> translateMessage(const MessageActivateTokenIds* message) const;
	std::shared_ptr<MessageActivateTokens> translateMessage(const MessageSearch* message) const;

private:
	StorageAccess* m_storageAccess;
};

#endif // ACTIVATION_TRANSLATOR_H
