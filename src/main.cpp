#include <iostream>
#include <string>

#include "Document.hpp"
#include "Message.hpp"
#include "Prompt.hpp"
#include "Workspace.hpp"

int main(int argc, char* argv[]) {
    const std::string path = argc > 1 ? argv[1] : "../text/sample.txt";

    Workspace workspace("Demo");

    Document sample("Sample requirements", "");
    if (sample.load(path)) {
        std::cout << "loaded " << sample.sourcePath() << "\n";
    } else {
        std::cout << "could not load " << path << "\n";
    }
    workspace.addDocument(sample);

    workspace.addPrompt(Prompt("Reviewer", "Review this requirement."));
    workspace.addMessage(Message(MessageRole::User, "Is this requirement testable?"));

    std::cout << workspace.name() << ": "
              << workspace.documentCount() << " documents, "
              << workspace.promptCount() << " prompts, "
              << workspace.messageCount() << " messages\n";

    std::cout << workspace.documentAt(0).title() << " has "
              << workspace.documentAt(0).characterCount() << " characters\n";

    return 0;
}
