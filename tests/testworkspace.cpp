#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Document.hpp"
#include "Message.hpp"
#include "Prompt.hpp"
#include "Workspace.hpp"

namespace {

void writeFile(const std::string& path, const std::string& contents) {
    std::ofstream out(path, std::ios::binary);
    out << contents;
}

}

void prompt_smoke_test() {
    Prompt p("Reviewer", "Review this requirement.");
    assert(p.title() == "Reviewer");
    assert(!p.empty());
}

void workspace_smoke_test() {
    Workspace w("Demo");
    w.addPrompt(Prompt("Reviewer", "Review this requirement."));
    assert(w.promptCount() == 1);
}

void message_roundtrip_test() {
    Message defaulted;
    assert(defaulted.role() == MessageRole::User);
    assert(defaulted.empty());

    Message m(MessageRole::System, "You are a reviewer.");
    assert(m.role() == MessageRole::System);
    assert(m.text() == "You are a reviewer.");
    assert(!m.empty());

    m.setRole(MessageRole::Assistant);
    m.setText("Looks good.");
    assert(m.role() == MessageRole::Assistant);
    assert(m.text() == "Looks good.");
}

void message_equality_includes_role_test() {
    Message user(MessageRole::User, "Explain this.");
    Message sameUser(MessageRole::User, "Explain this.");
    Message assistant(MessageRole::Assistant, "Explain this.");

    assert(user == sameUser);
    assert(!(user != sameUser));
    assert(user != assistant);
}

void prompt_roundtrip_test() {
    Prompt defaulted;
    assert(defaulted.title().empty());
    assert(defaulted.empty());

    Prompt p("Reviewer", "Review this.");
    assert(p.title() == "Reviewer");
    assert(p.text() == "Review this.");

    p.setTitle("Summarizer");
    p.setText("Summarize this.");
    assert(p.title() == "Summarizer");
    assert(p.text() == "Summarize this.");

    assert(p == Prompt("Summarizer", "Summarize this."));
    assert(p != Prompt("Summarizer", "Review this."));
    assert(p != Prompt("Reviewer", "Summarize this."));
}

void prompt_empty_is_about_text_test() {
    Prompt titledOnly("Reviewer", "");
    assert(titledOnly.empty());

    Prompt untitled("", "Review this requirement.");
    assert(!untitled.empty());
}

void document_roundtrip_test() {
    Document defaulted;
    assert(defaulted.title().empty());
    assert(defaulted.sourcePath().empty());
    assert(defaulted.characterCount() == 0);
    assert(defaulted.empty());

    Document d("Notes", "ab\ncd\n");
    assert(d.title() == "Notes");
    assert(d.contents() == "ab\ncd\n");
    assert(d.characterCount() == 6);
    assert(!d.empty());
    assert(d.sourcePath().empty());

    d.setTitle("Renamed");
    assert(d.title() == "Renamed");
    assert(d.contents() == "ab\ncd\n");

    assert(d == Document("Renamed", "ab\ncd\n"));
    assert(d != Document("Notes", "ab\ncd\n"));
    assert(d != Document("Renamed", "other"));
}

void document_load_reads_whole_file_test() {
    const std::string path = "m0_load_test.txt";
    writeFile(path, "line one\nline two\n");

    Document d("Sample", "");
    assert(d.load(path));
    assert(d.contents() == "line one\nline two\n");
    assert(d.characterCount() == 18);
    assert(d.sourcePath() == path);
    assert(d.title() == path);
    assert(d != Document("Sample", "line one\nline two\n"));

    std::remove(path.c_str());
}

void document_load_empty_file_test() {
    const std::string path = "m0_empty_test.txt";
    writeFile(path, "");

    Document d;
    assert(d.load(path));
    assert(d.contents().empty());
    assert(d.sourcePath() == path);
    assert(d.title() == path);

    std::remove(path.c_str());
}

void document_failed_load_preserves_state_test() {
    Document d("Keep me", "original contents");
    const Document before = d;

    assert(!d.load("no_such_file_m0.txt"));
    assert(d == before);
    assert(d.title() == "Keep me");
    assert(d.contents() == "original contents");
    assert(d.sourcePath().empty());
}

void workspace_add_and_count_test() {
    Workspace w;
    assert(w.name().empty());
    assert(w.documentCount() == 0);
    assert(w.promptCount() == 0);
    assert(w.messageCount() == 0);

    w.setName("Demo");
    assert(w.name() == "Demo");

    w.addDocument(Document("Doc", "text"));
    assert(w.documentCount() == 1);
    assert(w.promptCount() == 0);
    assert(w.messageCount() == 0);

    w.addPrompt(Prompt("Reviewer", "Review this."));
    w.addMessage(Message(MessageRole::System, "first"));
    w.addMessage(Message(MessageRole::User, "second"));
    assert(w.documentCount() == 1);
    assert(w.promptCount() == 1);
    assert(w.messageCount() == 2);

    assert(w.messageAt(0).text() == "first");
    assert(w.messageAt(1).text() == "second");
    assert(w.messageAt(0).role() == MessageRole::System);
}

void workspace_mutable_at_test() {
    Workspace w("Demo");
    w.addPrompt(Prompt("Reviewer", "Review this."));

    w.promptAt(0).setText("Review this carefully.");
    assert(w.promptAt(0).text() == "Review this carefully.");

    Prompt& ref = w.promptAt(0);
    ref.setTitle("Careful reviewer");
    assert(w.promptAt(0).title() == "Careful reviewer");
}

void workspace_const_at_test() {
    Workspace w("Demo");
    w.addDocument(Document("Doc", "text"));

    const Workspace& cw = w;
    assert(cw.documentAt(0).title() == "Doc");
    assert(cw.documentCount() == 1);
}

void workspace_out_of_range_test() {
    Workspace w("Demo");

    try {
        w.documentAt(0);
        assert(false);
    } catch (const std::out_of_range&) {
    }

    w.addPrompt(Prompt("Reviewer", "Review this."));
    w.promptAt(0);

    try {
        w.promptAt(1);
        assert(false);
    } catch (const std::out_of_range&) {
    }

    const Workspace& cw = w;
    try {
        cw.promptAt(1);
        assert(false);
    } catch (const std::out_of_range&) {
    }
}

void workspace_copy_is_independent_test() {
    Workspace original("Original");
    original.addPrompt(Prompt("Reviewer", "Review this."));

    Workspace copy = original;
    assert(copy == original);

    copy.setName("Copy");
    copy.addPrompt(Prompt("Summarizer", "Summarize this."));
    copy.promptAt(0).setText("Changed.");

    assert(original.name() == "Original");
    assert(original.promptCount() == 1);
    assert(original.promptAt(0).text() == "Review this.");
    assert(copy != original);
}

void workspace_equality_test() {
    Workspace a("Demo");
    a.addDocument(Document("Doc", "text"));
    a.addMessage(Message(MessageRole::User, "Hello."));

    Workspace b("Demo");
    b.addDocument(Document("Doc", "text"));
    b.addMessage(Message(MessageRole::User, "Hello."));
    assert(a == b);

    Workspace differentName = a;
    differentName.setName("Other");
    assert(a != differentName);

    Workspace extraPrompt = a;
    extraPrompt.addPrompt(Prompt("Reviewer", "Review this."));
    assert(a != extraPrompt);

    Workspace differentMessage("Demo");
    differentMessage.addDocument(Document("Doc", "text"));
    differentMessage.addMessage(Message(MessageRole::Assistant, "Hello."));
    assert(a != differentMessage);
}

int main() {
    prompt_smoke_test();
    workspace_smoke_test();

    message_roundtrip_test();
    message_equality_includes_role_test();

    prompt_roundtrip_test();
    prompt_empty_is_about_text_test();

    document_roundtrip_test();
    document_load_reads_whole_file_test();
    document_load_empty_file_test();
    document_failed_load_preserves_state_test();

    workspace_add_and_count_test();
    workspace_mutable_at_test();
    workspace_const_at_test();
    workspace_out_of_range_test();
    workspace_copy_is_independent_test();
    workspace_equality_test();

    std::cout << "M0 tests passed\n";
    return 0;
}
