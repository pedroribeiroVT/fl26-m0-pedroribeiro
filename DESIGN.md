# M0 Design and Understanding Note

Answer briefly in your own words. This is not intended to be a long report.

1. What responsibility belongs to `Workspace`, and what responsibilities belong to `Document`, `Prompt`, and `Message` instead?

`Workspace` stores documents, prompts, and messages, says how many of each it holds, and gives it access by the index. `Document` owns the text and the path it was read from, and it is the only class that touches the filesystem. `Prompt`  made to be used as reusable text instructions. `Message` is one reply in a conversation, which is why it carries a `MessageRole` to be able to differentiate between who is sending what.

2. Why are the collections inside `Workspace` private? Explain the purpose of the const and non-const `At` overloads.

The vectors are private so `Workspace` can control them. The only ways they can be accessed are in the `addDocument`,`addPrompt`,`addMessage` and the `At`, so any outside code cannot change them behind the workspace's back. The two `At` overloads are there so constness carries through to the item, which calling `promptAt` on a non-const workspace gives back a `Prompt&` you can edit in place, while calling it on a `const Workspace&` gives back a `const Prompt&`. Without the const overload a const workspace would be nearly unusable, since you could not read an item out of it at all. Both overloads use `vector::at`, so an index that is out of range throws `std::out_of_range` instead of being undefined behavior.

3. Explain one meaningful test you added. What behavior does it check, and what implementation error could it catch?

`document_load_empty_file_test` writes a file with nothing in it, loads it, and checks that `load` returns true and that it comes back empty. Reading the file with `buffer << in.rdbuf()` inserts zero characters when the file is empty, and that sets fail on `buffer`. So the check `if (!buffer) { return false; }` would report a valid empty file as a failed load. Testing `in.bad()` instead is what makes the function correct.

4. Describe one implementation decision that you verified, tested, or revised before submitting your work.

I decided that `sourcePath_` is part of a `Document`'s value, so `operator==` compares it with the title and the contents. To check that the tests actually covered that, I removed the `sourcePath_` check from `operator==` and ran it again. The reason was that every pair of documents I compared have the same source path, which was normally empty, so no test ever found the difference. I added an assertion that a document loaded from a file is not equal to an identical document built in memory, and after that change is caught.

5. If generative AI was used, disclose it as required by course policy. If no generative AI was used, state that. The disclosure itself is not used as proof of authorship or understanding.

Generative AI was used to mainly to plan out the test functions and what should be the output of each of the test functions. It did implement and generate code for the test cases, but all was checked by me to make sure the intended output was correct.