# preact-3ds

This is currently a proof of concept, but aims to be much more. The goal is to
allow simple UI-based applications (not games) to be written for the Nintendo
3DS using JavaScript with React and other helpful developer-friendly libraries
(such as redux).

## Motivation

Writing homebrew for the 3DS is a complicated process at the moment. Though
there are some solutions like [LuaPlayerPlus](https://github.com/Rinnegatamante/lpp-3ds)
that allow for easier development for the platform, it is still somewhat obscure
and not as practical.

When it comes to using C or C++, it is really easy to get to a point where your
code is bug-riddled mess. These languages are also not really that accessible.
Another wall easy to hit when using C/C++ to write 3DS homebrew is library
documentation, which is really lackluster.

Even though there are some examples for uses of libctru, it might be difficult
to perform seemingly easy tasks like doing HTTP requests in a homebrew app.

This project comes as an attempt to provide an easy way of writing simple 3DS
apps that are just user interfaces with some logic to perform simple tasks. It
is really not intended for games, but for apps where performance isn't a problem
because it isn't a goal. I also hope this serves as a way for people experienced
with web programing to get into writing code for their own 3DSs in a way that
keeps the community alive.

## Contributing

At the moment I'm trying to get familiar with the way the 3DS, (P)React, Duktape
and Yoga work in order to try to make the simplest interface I can with react
and javascript in the 3DS, so any help related to any of these topics is welcome.

Please just open an issue or a pull request (even though there isn't much in
order to guide you through one).

## Building

This should build like a 3DS template project, just run `make` in the project's
root. You'll need devKitARM from devkitPRO.


