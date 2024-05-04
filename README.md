# blogc

A blog compiler.


## Quickstart

Clone the [Git repository](https://github.com/blogc/blogc) or grab the [latest release's source tarball](https://github.com/blogc/blogc/releases) and extract it.


### Dependencies

Building `blogc` requires:

- [CMake](https://cmake.org/)
- [Ninja](https://ninja-build.org/) (optional, but recommended)
- [ronn-ng](https://github.com/apjanke/ronn-ng) (if configured with `BUILD_MANPAGES=ON`)
- [cmocka](https://cmocka.org/) (if configured with `BUILD_TESTING=ON`)
- bash, diff and tee (if configured with `BUILD_TESTING=ON`)
- git, make and tar (if configured with `BUILD_TESTING=ON` and `BUILD_BLOGC_GIT_RECEIVER=ON`)

Running the `blogc-git-receiver` tool requires:

- git, make and tar


### Building and installing

Inside the source directory, run the following commands:

    $ mkdir build
    $ cmake \
          -B build \
          -S . \
          -G Ninja \
          [-DBUILD_BLOGC_GIT_RECEIVER=ON] \
          [-DBUILD_BLOGC_MAKE=ON] \
          [-DBUILD_BLOGC_RUNSERVER=ON] \
          [-DBUILD_MANPAGES=ON] \
          [-DBUILD_TESTING=ON]
    $ cmake \
          --build build \
          --config Release
    # cmake \
          --build build \
          --config Release \
          --target install

To create your first blog, please clone our example repository and adapt it to your needs:

    $ git clone https://github.com/blogc/blogc-example my-blog
    $ cd my-blog
    $ rm -rf .git
    $ git init
    $ git commit -am 'initial commit'

At this point you'll have an empty blog, that can be customized to suit your needs. You'll want to look at the `content/post/` directory and edit your first post. Each new post, template or asset must be added to the `Makefile`. Please read it carefully.

If some unexpected error happened, please [file an issue](https://github.com/blogc/blogc/issues/new).

-- Rafael G. Martins <rafael@rafaelmartins.eng.br>
