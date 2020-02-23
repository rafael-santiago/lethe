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

This is my implementation of a suggestion given by the worldwide known cryptographer and information security specialist
Bruce Schneier in his book "Applied Cryptography", about destroying information:

> "Most commercial programs that claim to implement the DoD standard overwrite three times: first with all ones, then with
> all zeros and finally with a repeating one-zero pattern. Given my general level of paranoia, I recommend overwriting a
> deleted file seven times: the first time with all ones, the second time with all zeros, and five times with a
> cryptographically secure pseudo-random sequence. Recent developments at the National Institute of Standards and Technology
> with electron-tunneling microscopes suggest even that might not be enough. Honestly, if your data is sufficiently valuable,
> assume that it is *impossible* to erase data completely off magnetic media. Burn or shred the media; it's cheaper to buy
> media new than lose your secrets."
>
> -- Bruce Schneier ["Applied Cryptography" 228-229 pp.]

This book was written at 90's. ``DoD`` additionally states that:

> "The number of times an overwrite must be accomplished depends on the storage media, sometimes on its sensitivity, and
> sometimes on different DoD component requirements."
>
> -- National Computer Security Center ["A Guide to Understanding Data Rememberance in Automanted Information Systems"]

Here, overwrite times are configured by the user. Anyway, I think that burn-and-shred advices given by Mr. Schneier should not
be discarded.

**WARNING**: This general ``DoD`` information destruction method does not work with flash based filesystems. It probably will
not work with your smartphone. Do not use this tool for doing this! Maybe in future I may extend this code to give some kind of
support for flash stuff too, but by now, I won't do it.

For ensuring that the implemented data destruction here is working fine on your system the well-known forensic tool
``Foremost`` is used (if you are on ``Linux``, in this case and have it installed). Moreover, I strongly suggest you to
build the software on your machine by running all tests before installing the tool or using it as a library into your
own stuff. Be sure that it is actually working instead of just believing on it.

Finally, I am not reponsible for any misuse of information or code here. I am not responsible for any possible damage, data
destruction or loss (partial or total) done by this software, too. Who knows your babysitter! Use it at your own risk!

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

**Be careful** when using ``lethe_drop``, by default it does not prompt you about removing or not what was found.
After all you are taking a drop from Lethe, right? ;)

Anyway, if you want to have the possibility of changing your mind after calling ``lethe_drop``, you must add to the
drop-type argument ``kLetheUserPrompt`` mask.

```c
    if (lethe_drop("picture.jpeg",
                   kLetheDataOblivion | kLetheFileRemove | kLetheUserPrompt) == 0) {
        printf("done!\n");
    }
```

It is also possible to set some internal components of implemented data wiping system:

- Total of overwrites.
- Total o file renamings.
- The used 'stat' function.

Setting total of overwrites:

```c
    if (lethe_set_overwrite_nr(2) != 0) {
        fprintf(stderr, "ERROR: Overwrite numbers not set.\n");
    }
```

The passed number must be a number from ``1`` up to ``n``.

Setting total of file renamings:

```c
    if (lethe_set_rename_nr(312) != 0) {
        fprintf(stderr, "ERROR: Rename numbers not set.\n");
    }
```

Setting 'stat' function:

```c
    if (lethe_set_stat(ur_custom_stat) != 0) {
        fprintf(stderr, "ERROR: Stat function not set.\n");
    }
```
