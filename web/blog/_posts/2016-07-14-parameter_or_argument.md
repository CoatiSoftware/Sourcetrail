---
layout: post
title: "Parameter or Argument?"
description: "What's the difference between a parameter and an argument? And how do I remember which one is what?"
modified: 2016-07-14
author: Malte Langkabel
author-link: http://www.maltelangkabel.com/
image:
  feature: post_background.png
---

I often hear people getting confused when talking about parameters and arguments. That confusion grows even stronger when one of them knows the difference but the other one doesn't. So let's shed some light on this issue and spread the knowledge! Programming involves talking to each other but that doesn't have to be more painful than it already is ;)

A [**Parameter**](http://eel.is/c++draft/defns.argument) is a variable in the signature of a function declaration or definition:

```plaintext
void function(int parameter) {
    // do something
}
```

An [**Argument**](http://eel.is/c++draft/defns.parameter) is the value that gets passed to the function when calling it:

```plaintext
function(argument);
```

Before knowing that difference I just used each of these terms when talking about any of these concepts. But now that each concept has its own term, talking about programming makes a lot more sense: When someone writes about a function argument I immediately know that code gets executed, but when someone talks about a parameter I know that he's looking at some declaration or definition!

And here's the 2nd best part: **Its quite simple to remember which one is what.** Just keep in mind that there is something called a **default argument** which is used when no real argument is specified.

The 1st best part is that this concept doesn't only apply to functions and methods! In C++ it also applies to templates! Don't worry, we won't be digging deeply into templates here.


A **Template Parameter** is a placeholder in declaration of a template:

```plaintext
template <typename TemplateParameter>
class TemplateClass
{
};
```

A **Template Argument** is the actual value or type that is used to instantiate that template:

```plaintext
TemplateClass<TemplateArgument> templateClass;
```

Following the example set by functions and methods one can also provide a **Template Default Argument** that specifies which value or type should be used when instantiating the template with no argument provided!

Even though these terms apply for C and C like languages, please note that other programming languages may have other terms to differentiate between these concepts.

<br />

___

**Edit:** Added links to the definition of the terms Parameter and Argument in the latest C++ draft. Added the above note regarding other languages. Added a semicolon to complete the syntax of the template class example ;)
