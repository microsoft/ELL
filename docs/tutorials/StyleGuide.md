# Embedded Learning Library (ELL) tutorial writer's guide to consistency


## Why a style guide for tutorials?

We built Embedded Learning Library (ELL) for makers, technology enthusiasts, students, entrepreneurs, and developers who aspire to build intelligent devices and AI-powered gadgets. We want to provide them with high quality tutorials that create a positive first impression of ELL. Our goal is to highlight each new user-facing feature of ELL with novel, distinctive tutorials.  We need to write in a way that ensures success for all skill levels. And, even though each tutorial will be unique in focus, we want the tutorials to have the same voice and tone, consistent terminology, and identical format.

This style guide offers advice and best practices that will promote the consistency that will help our readers be successful in using ELL. Try to follow them unless there's a good reason not to.

## How to use this guide

This guide specifies style and format for contributors to use when developing tutorials for Microsoft Research. Be warm and relaxed, crisp and clear, and ready to lend a hand to your readers. The following sections offer guidance on structure, style, and formatting. Additional resources for word choice and other topics can be found in the [```Microsoft Writing Guide```](https://docs.microsoft.com/en-us/style-guide/welcome/).

## Tutorial structure

Each tutorial should include the following sections, as required (delete sections that are not applicable for a given topic):

1. **Tutorial title** : one line that summarizes the goal of the tutorial (e.g., Setting up Raspberry Pi for tutorials)
2. **Introductory text (no heading)**: an overview of goal and the expected results
3. **Before you begin (heading)**: pre-requisites and a list of materials to be used
4. **Steps (headings)**: Sections for each major step should include:
    - Heading that reflects the action to be accomplished (e.g., Activate your environment and create a tutorial directory). Do not number these sections, but do present them in the order you want the reader to execute.
	- Introductory text that orients the reader.
	- Code blocks and instructions, as needed.
	- Transition to the next step or section, as appropriate.
5. **Results (heading)**: summary of what the tutorial accomplished
6. **Next steps (heading)**: if applicable, suggests additional tutorials
7. **Troubleshooting (heading)**: if applicable, covers the top issues that a reader might experience while trying to complete the tutorial.

## Writing style basics

When writing tutorials, keep some basic guidelines in mind.

**Keep it simple.** Write short, simple sentences. Replace complex sentences with lists and tables.

• Instead of: Raspbian has an extensive internal logic that allows it to adapt the processor speed according to the amount of computational load at any given moment - when the processor is under heavy load it clocks itself down to prevent overheating, which can lead to severe and permanent damage.

• Use: To protect the processor from damage caused by overheating, Raspbian throttles the processor speed during heavy loads.

**Be clear and concise — not hip or clever.** Avoid idioms, colloquial expressions, and culture-specific references.

• Instead of: Down the road, ELL will grow.

• Use: In the future, ELL will grow.

**Don't say it's easy.** You're writing for a variety of readers, with varying degrees of ability. Don't assume you know what's challenging or easy for someone. Avoid adverbs such as _obviously, clearly,_ and _apparently_. Also avoid "filler" adverbs, such as _very, really,_ and _actually_.

**Use technical terms carefully**. Your audience is varied and includes high school robotics enthusiasts and experienced hobbyists, so define terms in context if there's a chance your audience won't understand them. Use plain language whenever you can.

## Grammar and usage

These basic guidelines will get you started creating clear and useful content.

**Use present-tense verbs.** Present-tense verbs indicate the action is happening now, like _is_ and _open_. Avoid _will, was,_ and verbs ending in _–ed,_ which indicate that text isn't in the present tense. .

- **Instead of:** The predict function will return an array.
- **Use:** The predict function returns an array.

**Be direct.** Write simple statements of fact (called the indicative mood) or use direct commands (called imperative mood) for procedures and instructions.

- **Instead of:** Next, we will copy the file to the Raspberry Pi. OR You should copy the files to your Raspberry Pi.
- **Use:** Copy these files to your Raspberry Pi.
- **Instead of:** At the point where the process terminates, you should restart your computer.
- **Use:** After the process terminates, restart your computer.

**Use active voice** (where the subject performs the action). In passive voice, the receiver of the action is the subject. 

- Active example: The predict function returns an array.
- Passive example: An array is returned by the predict function.

**Speak to the reader.** Use second person most of the time. Second person often uses the pronoun _you_, as though you're speaking to the reader.

- Instead of: Next, we'll create a directory for this tutorial anywhere on your computer.
- Use: Next, you'll learn how to create a directory for this tutorial on your computer.

## Capitalization


- Always capitalize proper nouns, such as Raspberry Pi.
- Lowercase everything except the first word in a sentence, UI label, phrase, heading, or title (including the titles of blogs, articles, and press releases).
- Don't capitalize the spelled-out form of an acronym unless it's a proper noun.
- Use title-style capitalization for product and service names, book and song titles, article titles in citations, names of blogs, and titles of people (Vice President, for example). This typically means capitalizing the first and last word of a title, and using lower case for articles, prepositions, and conjuctions.
- In programming languages, follow the traditional capitalization of keywords and other special terms. Review the [ELL code style guide](https://github.com/Microsoft/ELL/blob/master/StyleGuide.md) for further guidance. 
- Don't use all uppercase for emphasis.
- Don't use all lowercase as a design choice.

## Punctuation

- End all sentences with a period, even if they're only two words.
- Include a colon at the end of a phrase that directly introduces a list.

Example:

Make sure you have the following materials before you begin this tutorial:

• Laptop or desktop computer

• Raspberry Pi 3

• Raspberry Pi camera or USB webcam.

- Include commas after items in a series, including before the second-to-last item, even when a conjuction (such as *and*) is used. 
Example: networks, storage, and virtual machines

- Don't use a slash (/) to indicate a choice or as a substitute for or.

## Headings

Readers scan text to find what they need. To help them, write to support scanning, and organize content into discrete, scannable chunks.

- Use sentence-style capitalization and no end punctuation.

Example heading: Getting started with image classification on Raspberry Pi

- Use parallel construction (same verb form or grammatical construction) for headings at the same level.

This parallel example uses active verb phrases (all the same tense and mood):


  - Activate your environment
  - Download pre-trained model
  - Compile and run the model

This example does not use parallel construction:


  - Activating your environment (gerund phrase)
  - Download pre-trained model (active verb phrase)
  - How to compile and run the model (adverb paired with a to-infinitive clause)

## Formatting text and code in instructions

Follow the [ELL Style Guide](https://github.com/Microsoft/ELL/blob/master/StyleGuide.md) for code snippets and samples. When you reference elements or parameters in a paragraph, format the item accordingly; for example: `over\_voltage`

When you want a reader to type code, use the following construction and format.

To install OpenBLAS, type:

`sudo apt-get install -y libopenblas-dev`

Code blocks are part of the markdown spec, but syntax highlighting isn't. However, many renderers (including GitHub) support syntax highlighting.

- Use `back-ticks` around inline `code`.
- For blocks of code, start and end the code block  with three back-ticks.

Examples for JavaScript and Python:

```javascript

var s = "JavaScript syntax highlighting";

alert(s);

```

```python

s = "Python syntax highlighting"

print s

```

## Word choice preferences

Choose simple, precise words that you use in conversations.

- Use common contractions, such as that's and don't. They're friendly and conversational. [Learn more](https://worldready.cloudapp.net/Styleguide/Read?id=2700&amp;topicid=26899).
- Avoid words with more than one meaning. [Learn more](https://worldready.cloudapp.net/Styleguide/Read?id=2700&amp;topicid=25512).

Refer to this list to understand which common terms to use and when. (List to be updated regularly.)

| Term | When to use |
| --- | --- |
| app, application, demo, program | Use **application** to refer to the "actuation code" that a user writes that runs on the target device. Do not use app, demo, or program to refer to this code. |
| camera | Use **camera** when the format of the camera doesn't matter. Use webcam or USB cam, as appropriate, when the procedure requires it. |
| classifier, classification function | Use to refer the output of a machine learning algorithm. Do not use predictor, hypothesis, or model interchangeably when referring to this output.  |
| folder, directory, subdirectory | Use **directory** to refer to the structure of the file system or to match the API. |
| root directory | Use root directory to refer to the directory or folder from which all other directories or folders branch. |
| tensor dimensions, tensor sizes, tensor shape  | Use the term **tensor shape** to reference the size or dimensions of a tensor|
| USB power cord | Use instead of USB power supply, USB power adapter, or USB wall-wart. |
| USB webcam | Use when the camera must be a USB webcam. |
| webcam | Use when referring to a video camera that feeds or streams its image in real time to or through a computer to a computer network, regardless of connector type. |
|model| Use this term only when referring to an actual ELL model. |
| (More terms to come)  |  TBD |
|   |   |
|   |   |