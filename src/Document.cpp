#include "Document.hpp"

#include <fstream>
#include <sstream>
#include <utility>

Document::Document(std::string title, std::string contents)
    : title_(std::move(title)), contents_(std::move(contents)) {}

bool Document::operator==(const Document& other) const {
    return title_ == other.title_
        && sourcePath_ == other.sourcePath_
        && contents_ == other.contents_;
}

bool Document::operator!=(const Document& other) const {
    return !(*this == other);
}

bool Document::load(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return false;
    }

    std::ostringstream buffer;
    buffer << in.rdbuf();
    if (in.bad()) {
        return false;
    }

    contents_ = buffer.str();
    sourcePath_ = path;
    return true;
}

const std::string& Document::title() const noexcept {
    return title_;
}

const std::string& Document::sourcePath() const noexcept {
    return sourcePath_;
}

const std::string& Document::contents() const noexcept {
    return contents_;
}

void Document::setTitle(std::string title) {
    title_ = std::move(title);
}

std::size_t Document::characterCount() const noexcept {
    return contents_.size();
}

bool Document::empty() const noexcept {
    return contents_.empty();
}
