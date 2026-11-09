# Development Conventions

This file describes the guidelines under which code is formatted and written in `cnotes.h`. Most of the rules relate to comments and documentation, as well as things like **AI Usage** and **Code Deprecation**.

## Comments and Tags

All comments should be written in English and generally follow standard English conventions. Lines should be kept reasonably short for readability.

Comments fall into two categories: **In-Code Comments** and **Documentation**.

* **In-Code Comments** should use double slashes `//`. They are not scanned by the documentation tool; they are there to guide a reader through the logic and structure of the code.
* **Documentation** follows multiline comment formatting `/**...*/` and should generally be written right next to the declaration it is attached to, so the documentation tool can pick it up and properly record it.

Tags are written inside comments to signify the importance of certain information. The format is `TAG: text`: an uppercase word, then a colon and a space, at the start of a comment line. All tags are listed below with their respective descriptions and whether they are documentation-specific or not.

| TAG               | Meaning                                                                   | Documentation-Specific |
|-------------------|---------------------------------------------------------------------------|------------------------|
| `RETURNS`         | Return value semantics, including failure values.                         | **Yes**                |
| `PARAM`           | Optional. Use it only when a parameter needs explaining.                  | **Yes**                |
| `OUTPUTS`         | When a paramemeter value is used as output.                               | **Yes**                |
| `SINCE`           | Version that introduced the symbol, e.g. `SINCE: 0.2.0`.                  | **Yes**                |
| `NOTE`            | Useful detail the user might need to know, not a contract.                | **No**                 |
| `IMPORTANT`       | Contract the caller must respect: lifetime, ownership, preconditions.     | **No**                 |
| `AI`              | Description of AI usage in code. See **AI Usage** for more info.          | **No**                 |
| `TODO`            | Planned work.                                                             | **No**                 |
| `LEAK`            | Known memory leak, usually one done on purpose.                           | **No**                 |
| `TEMPORARY`       | Code meant to be replaced.                                                | **No**                 |
| `FIXME`           | Known bug.                                                                | **No**                 |

By default, all tags should be renderable by the documentation tool---documentation-specific tags are simply the ones used only inside documentation blocks.

## API Stability

`cnotes.h` already contains a description of how API stability is achieved, so it won't be repeated here. The version number must be incremented according to the level of change. There are two things to remember:

* Deprecated functions are marked with `CN_DEPRECATED(msg)` and kept for at least one minor release, then removed.
* The changelog must be updated on every PATCH, MINOR or MAJOR change. For now, it is kept at the bottom of the `cnotes.h` file.

An API documentation block is formatted in the following way:

```c
/**
 * One-sentence summary.            <- first line, used as the index summary
 *
 * Longer description (optional).
 *
 * PARAM str: ...                   <- optional
 * RETURNS: ...
 * IMPORTANT: ...
 * NOTE: ...
 * SINCE: 0.1.0
 */
CNDEF Cn_String cn_name(Cn_String str);
```

The documentation tool then picks it up and renders it nicely.

## AI Usage

`cnotes.h` doesn't take a harsh stance against **AI**, but requires developers to be aware of **AI-written** code. A single tag, `AI`, with a fixed vocabulary is used to signify **AI Usage** in code:

* `AI: Generated`---written by AI, not yet fully reviewed or tested by the author.
* `AI: Assisted`---written by the author with AI help, or AI code that has been fully reviewed, tested and taken over by the author.

The `AI` tag can be used in both **Documentation** and **In-Code Comments**. It can be scoped to the whole function, or, if only small pieces of code are written by AI, placed in comments inside the implementation.

## Personal Note

These guidelines are meant to help, not hinder, development. They can change or adapt as the library changes. They are meant to be simple to understand and as beneficial as possible to use!
