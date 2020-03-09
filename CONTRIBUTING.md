## CONTRIBUTING

Firstly, thank you for contributing. You should consider the following before submitting any stuff:

- Be honest;
- Forks always must be from <https://github.com/rafael-santiago/lethe> (master);
- New stuff should be tested;
- New stuff should be detailed and documented;
- Bugfixes should be tested;
- Memory leaks are bad, try always to eliminate it before any pull request;
- If you added some feature that has some drawback or this is not so strong against some kind of attack(s), be fair by making it clear in documentation;
- Security by obscurity is silly, thanks but no;
- Details are awesome in a pull request;
- The first idea is providing a new feature thinking in all supported platforms;
- Speed is secondary;
- Some observed points in K.I.S.S. and [Suckless](https://suckless.org) are awesome;

The best way of knowing what could be done is reading [doc/todo.txt](https://github.com/rafael-santiago/lethe/blob/master/doc/todo.txt).

## Code style

There is no absolute truth (a.k.a unicorn) when talking about code style, anyway, this is the truth that I have
been taking into consideration here.

- Comments are good;
- Tests are awesome (use the testing [library](https://github.com/rafael-santiago/cutest) used by the project);
- If your test need a human to drive it, sorry but your test is a kind of crap;
- We like C (C, not C with sugar or anything similar);
- Do not use 'func()' when prototyping use 'func(void)';
- Arrays as function parameters are not welcome, use pointers;
- Pointers are good. Taking into consideration that you are a fluent and experienced C programmer;
- The project should be the most self-contained possible;
- Git submodules are ok;
- This project is GPLv2. Always include the copyright disclaimer in new code files;
- Respect the credit from other people;
- Give credit to other people;
- Give credit to yourself;

## Code constructions

Always include the statmements between ``{ ... }``.

This is considered bad here:

```c
        if (x < y) do_stuf();

        if (x < y)
            do_this();
        else
            do_that();
```

Macros are ok, however, "undef" it when it is not necessary anymore:

```c
    #define scoped_left_sh(x, s) ( (x) << (s) ) | ( (x) >> ( (sizeof(x) << 3) - (s) ) )

        ...

    #undef scoped_left_sh
```

When passing a string pass its size besides assuming it as a null terminated string.

When commenting some code use the following format:

```c
    // INFO(your name, or as people know you): This is an information.

    // WARN(your name, or as people know you): This is a warning.

    // FIXME(your name, or as people know you): I do not know how to solve it.

    // BUG(your name, or as people know you): I found a bug here, it should be fixed.

    // TODO(your name, or as people know you): To do item.

    // TIP(your name, or as people know you): You are giving the tips for people understand
    //                                        some craziness, weird code chunk.
```

Static functions even being static must be prototyped at the beginning of the implementation file.

Avoid using double quotes when including files in C implementation stuff (local includes). Headers
and implemenation files should be relative to the toplevel src subdirectory.

Gotos are ok if it always move forward but never ever backward.

Avoid capital letters in function and variable names.

C Defines:

- while constants must be in upper case;
- while macros must be in lower case;
- while a DSL statement must be in upper case;

Use ``lethe_memcmp``, ``lethe_memcpy``, ``lethe_memset`` and ``lethe_stat`` in replacement to ``memcmp``, ``memcpy``, ``memset`` and ``stat`` respectively.
Those functions are defined in ``lethe_libc.h`` but if you has included ``lethe_types.h`` you indirectly has included ``lethe_libc.h`` too.
