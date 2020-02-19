<table border="0">
<tr>
<td width="150" style="border-top:none;border-bottom:none;border-left:none;border-right:none">
<img src="https://github.com/rafael-santiago/lethe/blob/master/etc/gustave-dore-dante-and-the-river-of-lethe.png" title= "'Dante and the river of Lethe' by Gustave Doré (1832-1883) / Public Domain">
</td>
<td style="border-top:none;border-bottom:none;border-left:none;border-right:none">
 <blockquote>
     <p>
        Let fancy still in my sense in Lethe steep;<br>
        If it be thus to dream, still let me sleep!<br>
        Lethe, the river of oblivion, rolls her watery labyrinth,<br>
        which whoso drinks forgets both joy and grief<br>
        O sister, mother, wife.<br>
        Sweet Lethe is my life.<br>
        I am never, never, never coming home!
     </p>
     <p>
        <i>-- Willian Shakespeare [Twelfth Night], John Milton [Paradise Lost], Sylvia Plath [Amnesiac]</i>
     </p>
 </blockquote>
</td>
</tr>
</table>

# Lethe

``Lethe`` is a tiny and well-simple library/tool for data wiping. In Greek mythology ``Lethe`` is one of the five rivers of
``Hades``. According to the myth, who touch, drink or even take a single ``drop`` from this river will experience
forgetfulness.

## Okay, tell me the simplest way of cloning this repo, please...

Well,

```
    you@Hades:~/src# git clone https://github.com/rafael-santiago/lethe --recursive
    you@Hades:~/src# _
```

All done.

## How can I build it?

Lethe's build is based on another tool of mine called Hefesto <https://github.com/rafael-santiago/hefesto>.
After following all steps for making Hefesto works on your system, if you want to build the library:

```
    you@Hades:~/src# cd lethe/src
    you@Hades:~/src/lethe/src# hefesto --forge-type=lib
    you@Hades:~/src/lethe/src# _
```

If the build succeeds the ar file will be at '../lib/liblethe.a'.

Now, supposing you want the tool:

```
    you@Hades:~/src# cd lethe/src
    you@Hades:~/src/lethe/src# hefesto --forge-type=tool
    you@Hades:~/src/lethe/src# _
```

If the build succeeds the binary will be at '../bin/lethe'.

## How to use let into my own stuff?

By the way, ``Lethe`` is ``GPLv2``. If this license fits to you, for using ``Lethe`` as a library, you only need to include
``src/lethe.h`` and "call" ``lethe_drop()``. For building it you need to link your stuff with ``liblethe.a``.

Now, ``lethe_drop`` basically takes two arguments:

    - The filename or a glob pattern.
    - The drop type.

Take a look:

```c
// test-lethe.c
#include <lethe.h>

int main(int argc, char **argv) {
    ...
    if (lethe_drop("sensible-data.txt", kLetheDataOblivion) == 0) {
        printf("done!\n");
    }
    return 0;
}
```

Supposing the ``Lethe's`` code base is located at ``~src/lethe``:

```
    you@Hades:~/src/test# cc test-lethe.c -I~/src/lethe/src -L~/src/lethe/lib -llethe
    you@Hades:~/src/test# _
```

Pretty simple. Now explaining:

```c
    lethe_drop("sensible-data.txt", kLetheDataOblivion)
```

When you call ``lethe_drop`` by passing ``kLetheDataOblivion`` the content of the file will "fall into oblivion" but the
file will remain.

If you want to remove the file too, use:

```c
// test-lethe.c
#include <lethe.h>

int main(int argc, char **argv) {
    ...
    if (lethe_drop("sensible-data.txt", kLetheDataOblivion | kLetheFileRemove) == 0) {
        printf("done!\n");
    }
    return 0;
}
```

``Lethe`` needs some randomizer to perform all file oblivion stuff. If you prefer using your custom randomizer you must pass
it in ``lethe_drop`` call. The randomizer must have the following prototype:

```c
    unsigned char your_custom_randomizer(void);
```

Now you must add to drop type argument the mask ``kLetheCustomRandomizer``, as follows:

```c
    if (lethe_drop("sensible?data-[01]*.txt",
                   kLetheDataOblivion | kLetheFileRemove | kLetheCustomRandomizer,
                   ur_awesome_csprng) == 0) {
        printf("done!\n");
    }
```

**Be careful** when using ``lethe_drop``, by default it does not prompt you about remove or not what was has found.
After all you are taking a drop, right? ;)

Anyway, if you want to have the possibility of changing your mind after calling ``lethe_drop``, you must add to the
drop-type argument ``kLetheUserPrompt`` mask.

```c
    if (lethe_drop("picture.jpeg",
                   kLetheDataOblivion | kLetheFileRemove | kLetheUserPrompt) == 0) {
        printf("done!\n");
    }
```
