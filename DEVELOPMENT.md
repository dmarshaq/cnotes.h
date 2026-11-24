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
| `MEMBER`          | Optional. Use it only when a member needs explaining.                     | **Yes**                |
| `OUTPUTS`         | When a paramemeter value is used as output.                               | **Yes**                |
| `SINCE`           | Version that introduced the symbol, e.g. `SINCE: 0.2.0`.                  | **Yes**                |
| `NOTE`            | Useful detail the user might need to know, not a contract.                | **No**                 |
| `IMPORTANT`       | Contract the caller must respect: lifetime, ownership, preconditions.     | **No**                 |
| `AI`              | Description of AI usage in code. See **AI Usage** for more info.          | **No**                 |
| `TODO`            | Planned work.                                                             | **No**                 |
| `LEAK`            | Known memory leak, usually one done on purpose.                           | **No**                 |
| `TEMPORARY`       | Code meant to be replaced.                                                | **No**                 |
| `FIXME`           | Known bug.                                                                | **No**                 |
| `TEST`            | Signifies that doc comment is for the test. See **Testing**.              | **Yes**                |

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

* `AI: Generated`---written by AI, not yet fully reviewed or tested by the author. This tag can be removed if the code has been fully reviewed, tested and taken over by the author.

The `AI` tag can be used in both **Documentation** and **In-Code Comments**. It can be scoped to the whole function, or, if only small pieces of code are written by AI, placed in comments inside the implementation.


## Testing

The project relies heavely on **single execution**  tests.

### Commands

```bash
./nob test                      # Run all tests.
```

### How It Works

1. **Recording**: Each tests result is checked against the golden file created by the user: `.stdout.txt` file.
2. **Testing**: `./nob test` compiles and runs the test, comparing stdout against the recorded `.stdout.txt`.
3. **Results**: `SUCCESS`, `BUILD_FAIL`, `RUNTIME_FAIL`, `UNEXPECTED_OUTPUT` or `TIMEOUT`.

### Tests Structure

This section is specifically applied to `tests/parse/` tests since they will be primary testing
way for this library.

Each test gets one `.c` file. On the very top of the file optional doc `/**...*/` comment can be inserted that
starts with `TEST: ...` tag. It will be scanned by the testing tool for the `--flags`.

Flags are written on the last line of the comment, on their own, without any other text. A line
whose first non space characters are `--` is parsed as a flags line; any non flag text on it is an error.

```c
/**
 * TEST: One-sentence summary.
 *
 * Description of what the test covers.
 *
 * --gnu
 */
```

Upon completion the test will be ran with all flags accounted for. If no comment or no flags are written
the test will be ran in default configuration.

Once test is ran, the output is compared against `.stdout.txt` golden file that **MUST** be located next to
the test file.

| FLAG              | Meaning                                                                   |
|-------------------|---------------------------------------------------------------------------|
| `--gnu`           | Test will be ran with cnotes gnu extension attached.                      |


### Known tool feature limitation are all stated in README.md.

Everything else should be a fair ground to cover with testing.


## Personal Note

These guidelines are meant to help, not hinder, development. They can change or adapt as the library changes. They are meant to be simple to understand and as beneficial as possible to use!
