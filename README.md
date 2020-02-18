<table border="0">
<tr>
<td width="150" style="border-top:none;border-bottom:none;border-left:none;border-right:none">
<img src="https://github.com/rafael-santiago/lethe/blob/master/etc/gustave-dore-dante-and-the-river-of-lethe.png" title= "'Dante and the river of Lethe' by Gustave Doré (1832-1883) / Public Domain">
</td>
<td style="border-top:none;border-bottom:none;border-left:none;border-right:none">
 <blockquote>
     <p>Let fancy still in my sense in Lethe steep; If it be thus to dream, still let me sleep!</p>
     <p>Lethe, the river of oblivion, rolls her watery labyrinth, which whoso drinks forgets both joy and grief</p>
     <p>O sister, mother, wife. Sweet Lethe is my life. I am never, never, never coming home!</p>
     <p></p>
     <p>
        <i>-- Willian Shakespeare [Twelfth Night], John Milton [Paradise Lost], Sylvia Plath [Amnesiac]</i>
     </p>
 </blockquote>
</td>
</tr>
</table>

# Lethe

Lethe is a tiny and well-simple library/tool for data wiping.

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

