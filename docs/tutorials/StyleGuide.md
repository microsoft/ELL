# Style Guidelines for Tutorials

We aspire to write high quality tutorials, which create a strongly positive first impression on our users. Ideally, every new user-facing feature the ELL library will be highlighted in a tutorial. Each tutorial should be novel and sufficiently disjoint from the others.

The primary style guideline for writing tutorials is *consistency*, both within each tutorial and across tutorials. The tutorials should have the same consistency as chapters of a book - similar narrative, consistent terminology, consistent look and feel. The following guidelines and best practices are primarily intended to encourage and promote this consistency. Try to follow them unless there's a good reason not to. 

#### Be informal and friendly

The voice should be informal and friendly, but don't try to be hip or clever. Avoid slang.  

#### Shorter is better

Convey all useful information, but prefer to do so in a brief and concise way.    

* Good: "Raspian throttles the processor speed when under heavy load to protect itself from overheating."
* Not as good: "Raspian has an extensive internal logic that allows is to adapt the processor speed according to the amount of computational load at any given moment - when the processor is under heavy load it clocks itself down to prevent overheating, which can lead to severe and permanent damage." 

#### Don't say its easy

Avoid words like "obviously", "it's easy to see that", "clearly". Think about the reader who doesn't understand something - labeling that thing as "obvious" will make the reader feel worse.   

#### When describing a behavior or property, prefer present-simple tense, third-person active voice, indicative mood 

* Good: "The `predict` function returns an array."
* Not as good: "The `predict` function will return an array." - future tense
* Not as good: "An array is returned by the `predict` function" - passive voice
* Bad: "The `predict` function is returning an array." - present progressive tense

#### For step-by-step instructions, use imperative mood, second-person active voice 

* Good: "Copy these file to your Raspberry Pi."
* Good: "Once the process terminates, restart your computer."
* Not as good: "Next, we will copy the file to the Raspberry Pi." - indicative mood instead of imperative, should be "Next, copy the file..."
* Not as good: "You should copy the files to your Raspberry Pi." - suggestive mood instead of imperative, should be, "Copy the file ..."
* Not as good: "Let's copy the file to the Raspberry Pi." - suggestive mood instead of imperative, should be, "Copy the file ..."
* Not as good: "You would do this to copy files to your Raspberry Pi." - should be, "Do this to copy ..."
* Note as good: "At this point, we want to copy the file to your Raspberry Pi." - should be, "At this point, copy the file..."

#### Write introductory text in simple-future tense, referring to both the reader and the writer in first-person plural

Tutorials usually start with some introductory text. Sections of tutorials can also start with some introductions. First person plural is the preferred narrative in teaching texts (school books, academic papers, etc) because it gives a sense of "we're in this together". For example, think of how you would introduce a topic in a classroom: "Good morning class, today we will learn about ...". "We" includes both the teacher and the students. 

* Good: "In this tutorial, we will build a squirrel detector."
* Not as good: "In this tutorial, you will build a squirrel detector." - reader is referred to in the second person
* Not as good: "In this tutorial, we will teach you how to build a squirrel detector." - reader is referred to in the second person
* Not as good: "In this tutorial, we will be building a squirrel detector." - future continuous
* Not as good: "By the end of this tutorial, we will have built a squirrel detector." - future perfect 

#### Avoid technical synonyms

Use consistent terminology within a tutorial and across tutorials.

For example:

* classifier, classification function, predictor, hypothesis, model
* folder, directory, subdirectory
* USB power supply, USB power adapter, USB wall-wart
* webcam, USB cam, cam, camera
* tensor dimensions, tensor sizes, tensor shape

 These are lists of terms that are usually interchangeable. We choose one term (often arbitrarily) and stick to it across our tutorials (unless there's a good reason not to). Look at other tutorials to see which term is already in use. 
 
#### Write linear instructions

When possible, write linear step-by-step instructions. Avoid foreshadowing the future. Avoid forks and multiple choice, instead consider making multiple separate tutorials. 

## Other Best Practices

1. Skip end punctuation on titles and lists
2. Include a comma after each item in a list. "Shake, rattle, and roll"
3. Capitalize the first word of a sentence and any proper nouns (Python, CMake, Raspian, Pi). Avoid title case, even in titles
4. Use bullets for lists, not as a way to emphasize text
5. Don't use too many colons 

References:
http://www.technicalauthoring.com/wiki/index.php/Using_the_correct_language
