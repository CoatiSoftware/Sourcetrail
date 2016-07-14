---
layout: post
title: "Parameter or Argument?"
description: "What's the difference between a parameter and an argument? And how do I remember which one is what?"
author: Malte Langkabel
author-link: http://www.maltelangkabel.com/
image:
  feature: post_background.png
---

I often hear people getting confused when talking about parameters and arguments. That confusion grows even stronger when one of them knows the difference but the other one doesn't. So let's shed some light on this issue and spread the knowledge! Programming involves talking to each other but that doesn't have to be more painful than it already is ;)

A **Parameter** is a variable in the signature of a function declaration or definition:

```
void function(int parameter) {
    // do something
}
```

An **Argument** is the value that gets passed to the function when calling it:

```
function(argument);
```

Before knowing that difference I just used each of these terms when talking about any of these concepts. But now that each concept has its own term, talking about programming makes a lot more sense: When someone writes about a function argument I immediately know that code gets executed, but when someone talks about a parameter I know that he's looking at some declaration or definition!

And here's the 2nd best part: **Its quite simple to remember which one is what.** Just keep in mind that there is something called a **default argument** which is used when no real argument is specified.

The 1st best part is that this concept doesn't only apply to functions and methods! In C++ it also applies to templates! Don't worry, we won't be digging deeply into templates here.


A **Template Parameter** is a placeholder in declaration of a template:

```
template <typename TemplateParameter>
class TemplateClass
{
}
```

A **Template Argument** is the actual value or type that is used to instantiate that template:

```
TemplateClass<TemplateArgument> templateClass;
```

Following the example set by functions and methods one can also provide a **Template Default Argument** that specifies which value or type should be used when instantiating the template with no argument provided!


