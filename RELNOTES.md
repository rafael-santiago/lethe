>1. Latest releases are on the top.
>2. Usually, I try to do my best in order to bring you 'RELNOTES' instead of notes from hell ;)
>
>Rafael

## v2 [git-tag: 'v2']

### Features:

- None.

### Bugfixes:

- Fixed a bias created by get_rnd_filename() function when picking allowed chars to compose
  random file names. It was picking random array indexes in a naïve way [e.g.: rnd() % range].

## v1 [git-tag: 'v1']

### Features:

- Drop by using external and internal randomizer.
- Total of passes done during renaming and file oblivion configured by the user.
- Manual reader by using the system pager.
- Supported platforms: ``Linux``, ``FreeBSD``, ``OpenBSD``, ``NetBSD``, ``MINIX``, ``Solaris`` and ``Windows``.

### Bugfixes:

- None!
