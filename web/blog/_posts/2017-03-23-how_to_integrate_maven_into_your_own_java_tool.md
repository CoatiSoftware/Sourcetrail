---
layout: post
title: "How to Integrate Maven into Your Own Java Tool"
description: "This post summarizes three different ways of integrating Maven into custom source code."
author: Malte Langkabel
author-link: http://www.maltelangkabel.com/
image:
  feature: post_background.png
---

I think we can all agree on one thing: Managing dependencies by hand sucks. Remember the project you needed to build? The one that required a library you never heard of before. Once you finally found it on the internet your compiler said: *“I’m really sorry, dude, but this seems to be the wrong version of that library.”* Opposed to that, using a dependency manager like Apache Maven seems to be a really good idea because it removes all this pain and all the specifying of required libraries in their respective version is up to the original developers of a project.

In the recent years I’ve come across many developers who like to write small tools to automate tasks of their daily routine. And I guess it’s no surprise that many of these tools involve working on source code. Wouldn’t it be nice if those tools could be using Maven to fetch additional information or even execute goals of the build process? In this post I want to point out three different ways of integrating Maven into custom source code, give some useful hints and talk about which way we used to integrate Maven into [Coati](http://www.coati.io/), our source code exploration tool.

## Using Maven Embedder

When searching the web for ways to execute maven commands from inside your own source code [Maven Embedder](http://maven.apache.org/ref/3-LATEST/maven-embedder/index.html) is probably the most recommended approach. Maven Embedder is a component that is part of Maven itself. Furthermore it is used by tools like [M2Eclipse](http://www.eclipse.org/m2e/) which is a plugin for Eclipse that provides functionality like executing Maven goals and organizing dependencies. The upside of using Maven Embedder is that full functionality is contained in jars that can be integrated (and also shipped) with your application. The only downside is that Maven Embedder has a lot of direct and transitive dependencies (over 30 at the moment) which may be a burden if you are thinking of all the external license info you need to evaluate and include if you are actually shipping your application. The following snippet shows the essentials of how to use Maven Embedder to execute the *"install"* goal of a Maven project. If you like to give it a try, you’ll find more example code [here](http://people.apache.org/~ltheussl/maven-stage-site/guides/mini/guide-embedding-m2.html).

#### Code example:

```java
import org.apache.maven.cli.MavenCli;

public class MavenUsage {
    public void executeMavenInstall() {
        MavenCli maven = new MavenCli();
        maven.doMain(new String[]{"install"}, "path/to/project/root", System.out, System.out);
    }
}
```

## Using an Available Maven Installation

If you don’t want to use any library you can always turn back and use the Maven instance that is already available on the user’s machine. The only difficulty is to find out where the user’s Maven home directory is located. If you can’t find it programmatically or don’t want to waste time on this, you can just ask the user to provide it on the first run.

#### Code Example

```java
import java.io.File;
import java.io.IOException;

public class MavenUsage {
    public void executeMavenInstall() {
        Runtime runtime = Runtime.getRuntime();
        try {
            Process process = runtime.exec("path/to/mvn install", null, new File("path/to/project/root"));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
```

## Using Maven Invoker

The [Maven Invoker](https://maven.apache.org/shared/maven-invoker/index.html) library does not seem to be known as well as Maven Embedder. In contrast to Maven Embedder it does not embed Maven into your application but wraps calls to the Maven instance that’s available on the user’s machine. As Maven Invoker does not have many dependencies it is very lightweight compared to Maven Embedder. On top of that it seems to be maintained quite actively (at the time of writing this post). By now I didn’t have the spare time to try Maven Invoker myself but I wanted to mention it here anyways. However, if you have some experience using this project, I’d like to invite you to share any thoughts in the comments below!

#### Code Example

```java
import java.io.File;
import java.util.Collections;

import org.apache.maven.shared.invoker.DefaultInvocationRequest;
import org.apache.maven.shared.invoker.DefaultInvoker;
import org.apache.maven.shared.invoker.InvocationRequest;
import org.apache.maven.shared.invoker.Invoker;
import org.apache.maven.shared.invoker.MavenInvocationException;

public class MavenUsage {
    public void executeMavenInstall() {
        InvocationRequest request = new DefaultInvocationRequest();
        request.setPomFile(new File("/path/to/pom.xml"));
        request.setGoals(Collections.singletonList("install"));

        Invoker invoker = new DefaultInvoker();
        try {
            invoker.execute(request);
        } catch (MavenInvocationException e) {
            e.printStackTrace();
        }
    }
}
```

## What We Did at Coati and Why

At Coati we decided to stick to the second approach and just use the Maven instance already available on the user’s machine. In addition we wrote some path detectors to automatically locate the user’s Maven home directory on Windows, Mac and Linux. We still provide a setting for this in the tool’s options so that the user may change the path if desired.
The reason why we picked this approach is really not a technical one. It’s more some kind of a legal issue. Coati is a commercial product and even though the license of Maven Embedder (and all the licenses of the entire list of dependencies) would allow for use in a commercial product, it is still a hassle to keep that list up to date. Furthermore many of the bigger companies maintain legal departments that need some time to go through all the license documents of an external product before finally allowing to buy that software. We simply wanted that time to be as short as possible ;)

In conclusion I have to state what you can already see by looking at the code snippets above: Integrating Maven into your own source code is quite simple and it makes your/your user’s life so much easier. However the Maven Integration of Coati is brand new (we just released a beta earlier this week) so we will see how things are working out and I will be able to update this post if anything changes. Stay tuned and follow us:

[mailing list](http://eepurl.com/bRSSFf) - [Twitter](https://twitter.com/CoatiSoftware) - [Facebook](https://www.facebook.com/Coati-1529980600658370) - [Google+](https://plus.google.com/u/0/108949374849112775331)

*Do you have any thoughts on this topic or know of another way to integrate Maven? Please comment below!*


